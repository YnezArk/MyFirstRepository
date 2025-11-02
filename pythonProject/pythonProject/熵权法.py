import pandas as pd
import numpy as np

class IndicatorsAggregation:
    """
    综合评价类：支持多种指标权重与得分计算方法。
    当前已实现：
    - entropy_weight_method: 熵权法
    """
    @staticmethod
    def entropy_weight_method(df, cols, directions=None):
        """
        对指定的多列数据执行熵权法，返回每列的权重和得分
        参数:
            df (pd.DataFrame): 输入数据框
            cols (list): 要参与计算的数值列名列表
            directions (dict): 每列的方向 {"col1": "正向指标", "col2": "负向指标"}，默认全为正向
        返回:
            pd.DataFrame: 包含权重、信息熵等中间结果的数据框
            pd.DataFrame: 包含得分、排名的结果数据框
        """
        if directions is None:
            directions = {col: "正向指标" for col in cols}

        # 归一化处理
        normalized_df = pd.DataFrame()
        for col in cols:
            x_min = df[col].min()
            x_max = df[col].max()

            if x_max == x_min:
                normalized_df[col + "_normalized"] = 0.5
            else:
                if directions.get(col, "正向指标") == "正向指标":
                    normalized_df[col + "_normalized"] = (df[col] - x_min) / (x_max - x_min)
                else:
                    normalized_df[col + "_normalized"] = (x_max - df[col]) / (x_max - x_min)

        # 计算每列的概率 p_ij
        p = normalized_df / normalized_df.sum()
        p = p.fillna(0) + 1e-10  # 防止 log(0)

        # 计算信息熵 e_j
        entropy = -(p * np.log(p)).sum() / np.log(len(df))

        # 差异系数 d_j 与 权重 w_j
        d = 1 - entropy
        weight = d / d.sum() if d.sum() != 0 else 0.0
        
        # 数据集结：加权合成得分
        weighted_score = (normalized_df * weight).sum(axis=1)

        # 构造结果 DataFrame
        result_df = pd.DataFrame({
            '信息熵': entropy,
            '差异系数': d,
            '权重': weight,
        })
        score_df = pd.DataFrame({
            '得分': weighted_score,
            '排名': weighted_score.rank(method='dense', ascending=False).astype(int)
        })

        return result_df, score_df

# 构建学生成绩数据框
data = {
    '语文(150)': [80, 60, 70, 90, 100, 100, 90],
    '数学(150)': [90, 90, 100, 70, 100, 70, 100],
    '英语(150)': [90, 100, 90, 80, 80, 70, 90],
    '物理(110)': [70, 70, 80, 100, 70, 90, 70],
    '化学(100)': [90, 90, 70, 80, 90, 80, 100],
    '生物(90)': [90, 80, 70, 80, 60, 80, 80]
}
df = pd.DataFrame(data, index=['A同学', 'B同学', 'C同学', 'D同学', 'E同学', 'F同学', 'G同学'])

# 定义指标列和方向（所有学科均为正向指标，分数越高越好）
cols = ['语文(150)', '数学(150)', '英语(150)', '物理(110)', '化学(100)', '生物(90)']
directions = {col: "正向指标" for col in cols}

# 调用熵权法计算
result_df, score_df = IndicatorsAggregation.entropy_weight_method(df, cols, directions)

# 输出结果
print("各学科权重与信息熵结果：")
print(result_df)
print("\n各同学综合得分与排名：")
print(score_df)
