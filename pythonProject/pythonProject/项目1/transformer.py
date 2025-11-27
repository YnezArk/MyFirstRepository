import math
import torch
import torch.nn as nn
import numpy as np
import torch.utils.data as data
import torchvision.transforms as transforms
import torchvision.datasets as datasets
from tqdm import tqdm
import pytorch_lightning as pl
from pytorch_lightning.callbacks import ModelCheckpoint
import os

# 位置编码类：为Transformer提供序列位置信息（因Transformer无循环结构，需显式编码位置）
class PositionalEncoding(nn.Module):
    def __init__(self, d_model, max_len=5000):
        super().__init__()
        pe = torch.zeros(max_len, d_model)  # 初始化位置编码矩阵
        position = torch.arange(0, max_len, dtype=torch.float).unsqueeze(1)  # 生成位置序列
        # 计算位置编码的分母项，用于正弦/余弦编码
        div_term = torch.exp(torch.arange(0, d_model, 2).float() * (-math.log(10000.0) / d_model))
        pe[:, 0::2] = torch.sin(position * div_term)  # 偶数维度用正弦编码
        pe[:, 1::2] = torch.cos(position * div_term)  # 奇数维度用余弦编码
        pe = pe.unsqueeze(0)  # 增加batch维度
        self.register_buffer('pe', pe, persistent=False)  # 注册为非参数缓冲区

    def forward(self, x):
        x = x + self.pe[:, :x.size(1)]  # 给输入嵌入添加位置编码
        return x

# 缩放点积注意力函数：计算注意力权重并输出加权后的值
def scaled_dot_product(q, k, v, mask=None):
    d_k = q.size()[-1]
    attn_logits = torch.matmul(q, k.transpose(-2, -1))  # 计算Q与K的点积
    attn_logits = attn_logits / math.sqrt(d_k)  # 缩放（避免点积过大导致softmax梯度消失）
    if mask is not None:
        attn_logits = attn_logits.masked_fill(mask == 0, -9e15)  # 掩码屏蔽无效位置
    attention = nn.functional.softmax(attn_logits, dim=-1)  # 注意力权重归一化
    values = torch.matmul(attention, v)  # 加权求和得到输出
    return values, attention

# 扩展掩码函数（此处为占位，若需复杂掩码逻辑可补充）
def expand_mask(mask):
    return mask

# 多头注意力类：将注意力拆分为多个头并行计算，增强模型表达能力
class MultiheadAttention(nn.Module):
    def __init__(self, input_dim, embed_dim, num_heads):
        super().__init__()
        assert embed_dim % num_heads == 0, "embedding 维度必须整除 num_head"
        self.embed_dim = embed_dim
        self.num_heads = num_heads
        self.head_dim = embed_dim // num_heads
        # 合并Q、K、V的线性变换层（提高计算效率）
        self.qkv_proj = nn.Linear(input_dim, 3 * embed_dim)
        self.o_proj = nn.Linear(embed_dim, embed_dim)  # 注意力输出的线性变换层
        self._reset_parameters()

    def _reset_parameters(self):
        # 初始化权重（Xavier均匀初始化）
        nn.init.xavier_uniform_(self.qkv_proj.weight)
        self.qkv_proj.bias.data.fill_(0)
        nn.init.xavier_uniform_(self.o_proj.weight)
        self.o_proj.bias.data.fill_(0)

    def forward(self, x, mask=None, return_attention=False):
        batch_size, seq_length, _ = x.size()
        if mask is not None:
            mask = expand_mask(mask)
        qkv = self.qkv_proj(x)  # 计算Q、K、V的合并张量
        # 形状转换：[batch, seq_len, num_heads*3*head_dim] → [batch, num_heads, seq_len, 3*head_dim]
        qkv = qkv.reshape(batch_size, seq_length, self.num_heads, 3 * self.head_dim)
        qkv = qkv.permute(0, 2, 1, 3)
        q, k, v = qkv.chunk(3, dim=-1)  # 拆分得到Q、K、V
        values, attention = scaled_dot_product(q, k, v, mask=mask)  # 计算注意力
        # 形状还原：[batch, num_heads, seq_len, head_dim] → [batch, seq_len, embed_dim]
        values = values.permute(0, 2, 1, 3)
        values = values.reshape(batch_size, seq_length, self.embed_dim)
        o = self.o_proj(values)  # 注意力输出的线性变换
        if return_attention:
            return o, attention
        else:
            return o

# 编码器块类：包含多头注意力和前馈神经网络，配合残差连接和层归一化
class EncoderBlock(nn.Module):
    def __init__(self, input_dim, num_heads, dim_feedforward, dropout=0.0):
        super().__init__()
        self.self_attn = MultiheadAttention(input_dim, input_dim, num_heads)  # 多头自注意力
        # 前馈神经网络（MLP）
        self.linear_net = nn.Sequential(
            nn.Linear(input_dim, dim_feedforward),
            nn.Dropout(dropout),
            nn.ReLU(inplace=True),
            nn.Linear(dim_feedforward, input_dim)
        )
        self.norm1 = nn.LayerNorm(input_dim)  # 层归一化（注意力部分）
        self.norm2 = nn.LayerNorm(input_dim)  # 层归一化（MLP部分）
        self.dropout = nn.Dropout(dropout)

    def forward(self, x, mask=None):
        # 注意力子层 + 残差连接 + 层归一化
        attn_out = self.self_attn(x, mask=mask)
        x = x + self.dropout(attn_out)
        x = self.norm1(x)
        # MLP子层 + 残差连接 + 层归一化
        linear_out = self.linear_net(x)
        x = x + self.dropout(linear_out)
        x = self.norm2(x)
        return x

# Transformer编码器类：堆叠多个编码器块
class TransformerEncoder(nn.Module):
    def __init__(self, num_layers, input_dim, num_heads, dim_feedforward, dropout=0.0):
        super().__init__()
        self.layers = nn.ModuleList([
            EncoderBlock(input_dim, num_heads, dim_feedforward, dropout)
            for _ in range(num_layers)
        ])

    def forward(self, x, mask=None):
        for l in self.layers:
            x = l(x, mask=mask)
        return x

    def get_attention_maps(self, x, mask=None):
        attention_maps = []
        for l in self.layers:
            _, attn_map = l.self_attn(x, mask=mask, return_attention=True)
            attention_maps.append(attn_map)
            x = l(x)
        return attention_maps

# 数据集与特征提取配置
DATASET_PATH = "./datasets"
device = torch.device("cuda:0") if torch.cuda.is_available() else torch.device("cpu")
# ImageNet统计数据（用于图像归一化）
DATA_MEANS = np.array([0.485, 0.456, 0.406])
DATA_STD = np.array([0.229, 0.224, 0.225])

# 图像预处理： resize到224×224 + 张量转换 + 归一化
transform = transforms.Compose([
    transforms.Resize((224, 224)),
    transforms.ToTensor(),
    transforms.Normalize(DATA_MEANS, DATA_STD)
])
# 加载CIFAR100训练集和测试集
train_set = datasets.CIFAR100(root=DATASET_PATH, train=True, transform=transform, download=True)
test_set = datasets.CIFAR100(root=DATASET_PATH, train=False, transform=transform, download=True)

# 特征提取函数：从图像中提取特征（可替换为ResNet、ViT等预训练模型）
@torch.no_grad()
def extract_features(dataset, save_file):
    if not os.path.isfile(save_file):
        data_loader = data.DataLoader(dataset, batch_size=128, shuffle=False, drop_last=False, num_workers=4)
        extracted_features = []
        # 临时特征提取模型（实际场景建议替换为预训练模型）
        class PretrainedModel(nn.Module):
            def __init__(self):
                super().__init__()
                self.features = nn.Sequential(
                    nn.Conv2d(3, 64, kernel_size=7, stride=2, padding=3),
                    nn.ReLU(inplace=True),
                    nn.MaxPool2d(kernel_size=3, stride=2, padding=1),
                    nn.AdaptiveAvgPool2d((1, 1)),
                    nn.Flatten()
                )
            def forward(self, x):
                return self.features(x)
        pretrained_model = PretrainedModel().to(device)
        for imgs, _ in tqdm(data_loader):
            imgs = imgs.to(device)
            feats = pretrained_model(imgs)
            extracted_features.append(feats)
        extracted_features = torch.cat(extracted_features, dim=0)
        extracted_features = extracted_features.detach().cpu()
        torch.save(extracted_features, save_file)
    else:
        extracted_features = torch.load(save_file)
    return extracted_features

CHECKPOINT_PATH = "./saved_models"
os.makedirs(CHECKPOINT_PATH, exist_ok=True)
# 提取并保存训练集和测试集特征
train_feat_file = os.path.join(CHECKPOINT_PATH, "train_set_features.tar")
train_set_feats = extract_features(train_set, train_feat_file)
test_feat_file = os.path.join(CHECKPOINT_PATH, "test_set_features.tar")
test_feats = extract_features(test_set, test_feat_file)

# 异常检测数据集类：构造包含正常样本和异常样本的图像集合
class SetAnomalyDataset(data.Dataset):
    def __init__(self, img_feats, labels, set_size=10, train=True):
        super().__init__()
        self.img_feats = img_feats
        self.labels = labels
        self.set_size = set_size - 1  # 正常样本数 = set_size - 1，异常样本数为1
        self.train = train
        self.num_labels = labels.max() + 1
        # 按类别分组图像下标
        self.img_idx_by_label = torch.argsort(self.labels).reshape(self.num_labels, -1)
        if not train:
            self.test_sets = self._create_test_sets()  # 测试集预生成样本集合

    def _create_test_sets(self):
        test_sets = []
        np.random.seed(42)  # 固定随机种子保证可复现
        for idx in range(len(self)):
            test_sets.append(self.sample_img_set(self.labels[idx]))
        test_sets = torch.stack(test_sets, dim=0)
        return test_sets

    def sample_img_set(self, anomaly_label):
        # 采样与异常类别不同的正常类别
        set_label = np.random.randint(self.num_labels - 1)
        if set_label >= anomaly_label:
            set_label += 1
        # 从正常类别中采样图像下标
        img_indices = np.random.choice(self.img_idx_by_label.shape[1], size=self.set_size, replace=False)
        img_indices = self.img_idx_by_label[set_label, img_indices]
        return img_indices

    def __len__(self):
        return len(self.img_feats)

    def __getitem__(self, idx):
        anomaly = self.img_feats[idx]  # 异常样本
        if self.train:
            img_indices = self.sample_img_set(self.labels[idx])  # 训练集动态采样正常样本
        else:
            img_indices = self.test_sets[idx]  # 测试集使用预生成的正常样本集合
        # 拼接正常样本和异常样本，异常样本在最后一个位置
        img_set = torch.cat([self.img_feats[img_indices], anomaly[None]], dim=0)
        indices = torch.cat([img_indices, torch.LongTensor([idx])], dim=0)
        label = img_set.shape[0] - 1  # 异常样本的位置标签
        return img_set, indices, label

# 划分训练集和验证集
labels = torch.LongTensor(train_set.targets)
num_labels = labels.max() + 1
sorted_indices = torch.argsort(labels).reshape(num_labels, -1)
num_val_exmps = sorted_indices.shape[1] // 10  # 每个类别取10%作为验证集
val_indices = sorted_indices[:, :num_val_exmps].reshape(-1)
train_indices = sorted_indices[:, num_val_exmps:].reshape(-1)
# 提取训练集、验证集、测试集的特征和标签
train_feats, train_labels = train_set_feats[train_indices], labels[train_indices]
val_feats, val_labels = train_set_feats[val_indices], labels[val_indices]
test_labels = torch.LongTensor(test_set.targets)

SET_SIZE = 10  # 集合大小（正常样本数+异常样本数）
# 初始化异常检测数据集
train_anom_dataset = SetAnomalyDataset(train_feats, train_labels, set_size=SET_SIZE, train=True)
val_anom_dataset = SetAnomalyDataset(val_feats, val_labels, set_size=SET_SIZE, train=False)
test_anom_dataset = SetAnomalyDataset(test_feats, test_labels, set_size=SET_SIZE, train=False)

# 初始化数据加载器
train_anom_loader = data.DataLoader(
    train_anom_dataset,
    batch_size=64,
    shuffle=True,
    drop_last=True,
    num_workers=0,
    pin_memory=True
)
val_anom_loader = data.DataLoader(
    val_anom_dataset,
    batch_size=64,
    shuffle=False,
    drop_last=False,
    num_workers=0
)
test_anom_loader = data.DataLoader(
    test_anom_dataset,
    batch_size=64,
    shuffle=False,
    drop_last=False,
    num_workers=0
)

# 异常检测模型（基于PyTorch Lightning封装）
class AnomalyPredictor(pl.LightningModule):
    def __init__(self, input_dim, model_dim, num_heads, num_classes, num_layers, dropout, input_dropout, lr, warmup, max_iters):
        super().__init__()
        self.save_hyperparameters()  # 保存超参数用于日志和加载
        # Transformer编码器
        self.encoder = TransformerEncoder(
            num_layers,
            input_dim=input_dim,
            num_heads=num_heads,
            dim_feedforward=model_dim*4,
            dropout=dropout
        )
        self.input_dropout = nn.Dropout(input_dropout)  # 输入 dropout
        self.classifier = nn.Linear(model_dim, num_classes)  # 分类器（判断是否为异常）
        self.lr = lr
        self.warmup = warmup
        self.max_iters = max_iters
        self.current_iter = 0

    def forward(self, x):
        x = self.input_dropout(x)  # 输入 dropout
        x = self.encoder(x)  # Transformer编码
        x = x[:, -1]  # 取异常样本（最后一个）的特征
        x = self.classifier(x)  # 输出预测结果
        return x

    def training_step(self, batch, batch_idx):
        img_set, _, label = batch
        label = label.float()
        logits = self.forward(img_set).squeeze()  # 模型前向传播
        loss = nn.BCEWithLogitsLoss()(logits, label)  # 二分类损失（异常/正常）
        acc = ((logits > 0.5) == label).float().mean()  # 准确率计算
        self.log('train_loss', loss, on_step=True, on_epoch=True)
        self.log('train_acc', acc, on_step=True, on_epoch=True)
        self.current_iter += 1
        return loss

    def validation_step(self, batch, batch_idx):
        img_set, _, label = batch
        label = label.float()
        logits = self.forward(img_set).squeeze()
        loss = nn.BCEWithLogitsLoss()(logits, label)
        acc = ((logits > 0.5) == label).float().mean()
        self.log('val_loss', loss, on_epoch=True)
        self.log('val_acc', acc, on_epoch=True)
        return acc

    def test_step(self, batch, batch_idx):
        img_set, _, label = batch
        label = label.float()
        logits = self.forward(img_set).squeeze()
        acc = ((logits > 0.5) == label).float().mean()
        self.log('test_acc', acc, on_epoch=True)
        return acc

    def configure_optimizers(self):
        # Adam优化器
        optimizer = torch.optim.Adam(
            self.parameters(),
            lr=self.lr,
            betas=(0.9, 0.98),
            eps=1e-9
        )
        # 余弦退火学习率调度
        scheduler = torch.optim.lr_scheduler.CosineAnnealingLR(
            optimizer,
            T_max=self.max_iters
        )
        return [optimizer], [{"scheduler": scheduler, "interval": "step"}]

# 训练异常检测模型的函数
def train_anomaly(**kwargs):
    root_dir = os.path.join(CHECKPOINT_PATH, "SetAnomalyDetect")
    os.makedirs(root_dir, exist_ok=True)
    # 初始化PyTorch Lightning训练器
    trainer = pl.Trainer(
        default_root_dir=root_dir,
        callbacks=[ModelCheckpoint(save_weights_only=True, mode="max", monitor="val_acc")],
        accelerator="cuda" if str(device).startswith("cuda") else "cpu",
        devices=1,
        max_epochs=100,
        gradient_clip_val=2
    )
    trainer.logger._default_hp_metric = None  # 关闭默认超参数日志
    # 检查是否有预训练模型
    pretrained_filename = os.path.join(CHECKPOINT_PATH, "SetAnomalyDetect.ckpt")
    if os.path.isfile(pretrained_filename):
        print("Found pretrained model, loading...")
        model = AnomalyPredictor.load_from_checkpoint(pretrained_filename)
    else:
        # 初始化新模型并训练
        model = AnomalyPredictor(max_iters=trainer.max_epochs * len(train_anom_loader), **kwargs)
        trainer.fit(model, train_anom_loader, val_anom_loader)
        # 加载最佳模型并保存
        model = AnomalyPredictor.load_from_checkpoint(trainer.checkpoint_callback.best_model_path)
        torch.save(model, pretrained_filename)
    # 在训练、验证、测试集上评估模型
    train_result = trainer.test(model, train_anom_loader, verbose=False)
    val_result = trainer.test(model, val_anom_loader, verbose=False)
    test_result = trainer.test(model, test_anom_loader, verbose=False)
    result = {
        "test_acc": test_result[0]["test_acc"],
        "val_acc": val_result[0]["test_acc"],
        "train_acc": train_result[0]["test_acc"]
    }
    model = model.to(device)
    return model, result

# 执行训练并输出结果
anomaly_model, anomaly_result = train_anomaly(
    input_dim=train_anom_dataset.img_feats.shape[-1],
    model_dim=256,
    num_heads=4,
    num_classes=1,
    num_layers=4,
    dropout=0.1,
    input_dropout=0.1,
    lr=5e-4,
    warmup=100,
    max_iters=100 * len(train_anom_loader)  # 总迭代次数 = 轮数 × 每轮迭代数
)

print("训练结果：", anomaly_result)