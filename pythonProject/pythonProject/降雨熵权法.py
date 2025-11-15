import numpy as np
import pandas as pd
from sklearn.preprocessing import MinMaxScaler
from scipy.stats import variation
import matplotlib.pyplot as plt
import seaborn as sns


class TOPSIS_Entropy:
    def __init__(self):
        self.weights = None

    def calculate_entropy_weights(self, data):
        """计算熵权法权重"""
        # 数据标准化 (越大越好)
        scaler = MinMaxScaler()
        normalized_data = scaler.fit_transform(data)

        # 避免log(0)的情况
        epsilon = 1e-12
        normalized_data = np.where(normalized_data == 0, epsilon, normalized_data)

        # 计算第j项指标下第i个样本的比重
        p = normalized_data / np.sum(normalized_data, axis=0)

        # 计算熵值
        e = -np.sum(p * np.log(p), axis=0) / np.log(len(data))

        # 计算权重
        weights = (1 - e) / np.sum(1 - e)
        return weights

    def calculate_indicators(self, rainfall_data):
        """计算三个维度的指标"""
        indicators = []
        station_ids = []

        for station_id in rainfall_data.index:
            station_data = rainfall_data.loc[station_id].values

            # 1. 数据质量可靠性维度
            # 数据完整性（非零比例）
            completeness = np.sum(station_data > 0) / len(station_data)
            # 数据稳定性（变异系数倒数）
            non_zero_data = station_data[station_data > 0]
            if len(non_zero_data) > 1:
                data_cv = variation(non_zero_data)
                stability = 1 / (1 + data_cv)
            else:
                stability = 0.5  # 默认值
            # 数据一致性（与组内平均值的相关性）
            reliability_score = 0.6 * completeness + 0.4 * stability

            # 2. 区域覆盖有效性维度
            # 降雨强度贡献度
            rainfall_intensity = np.mean(station_data) if np.mean(station_data) > 0 else 0.001
            # 降雨时间覆盖率
            time_coverage = 1 - (np.sum(station_data == 0) / len(station_data))
            # 降雨空间代表性（与相邻站点的相关性）
            coverage_score = 0.5 * rainfall_intensity + 0.5 * time_coverage

            # 3. 特征捕捉精准性维度
            # 极端降雨事件捕捉
            if len(non_zero_data) > 0:
                extreme_threshold = np.percentile(non_zero_data, 90)
                extreme_ratio = np.sum(station_data > extreme_threshold) / len(station_data)
            else:
                extreme_ratio = 0
            # 降雨过程连续性
            consecutive_count = 0
            max_consecutive = 0
            for value in station_data:
                if value > 0:
                    consecutive_count += 1
                    max_consecutive = max(max_consecutive, consecutive_count)
                else:
                    consecutive_count = 0
            continuity = min(max_consecutive / 10, 1.0)  # 归一化
            precision_score = 0.6 * extreme_ratio + 0.4 * continuity

            indicators.append([reliability_score, coverage_score, precision_score])
            station_ids.append(station_id)

        return np.array(indicators), station_ids

    def topsis_evaluation(self, indicators, station_ids, group_name):
        """TOPSIS综合评价"""
        # 计算熵权权重
        weights = self.calculate_entropy_weights(indicators)

        # 标准化决策矩阵
        scaler = MinMaxScaler()
        normalized_matrix = scaler.fit_transform(indicators)

        # 加权标准化矩阵
        weighted_matrix = normalized_matrix * weights

        # 理想解和负理想解
        ideal_best = np.max(weighted_matrix, axis=0)
        ideal_worst = np.min(weighted_matrix, axis=0)

        # 计算距离
        distance_best = np.sqrt(np.sum((weighted_matrix - ideal_best) ** 2, axis=1))
        distance_worst = np.sqrt(np.sum((weighted_matrix - ideal_worst) ** 2, axis=1))

        # 计算相对贴近度
        closeness = distance_worst / (distance_best + distance_worst + 1e-12)

        # 等级划分
        grades = []
        for score in closeness:
            if score >= 0.8:
                grades.append('A+')
            elif score >= 0.7:
                grades.append('A')
            elif score >= 0.6:
                grades.append('B')
            elif score >= 0.5:
                grades.append('C')
            else:
                grades.append('D')

        results = []
        for i, station_id in enumerate(station_ids):
            results.append({
                '气象站ID': station_id,
                '所属分组': group_name,
                '数据质量可靠性': indicators[i][0],
                '区域覆盖有效性': indicators[i][1],
                '特征捕捉精准性': indicators[i][2],
                '综合得分': closeness[i],
                '等级': grades[i],
                '权重_可靠性': weights[0],
                '权重_覆盖性': weights[1],
                '权重_精准性': weights[2]
            })

        return results


def load_rainfall_data(file_path):
    """加载降雨数据"""
    # 读取Excel文件，跳过第一行（标题）
    df = pd.read_excel(file_path, header=0)

    # 提取B2到HJ41区域的数据（40行217列）
    # 假设第一列是气象站ID，B到HJ列是降雨数据
    rainfall_data = df.iloc[0:40, 1:218]  # 40行，217列

    # 设置气象站ID为索引
    station_ids = [f"P{i + 1:02d}" for i in range(40)]
    rainfall_data.index = station_ids

    # 处理可能的空值
    rainfall_data = rainfall_data.fillna(0)

    return rainfall_data


def main():
    # 定义分组
    groups = {
        '第1组': ['P06', 'P08', 'P11', 'P12', 'P16', 'P19', 'P21', 'P22', 'P23', 'P28', 'P30', 'P33', 'P36', 'P37'],
        '第2组': ['P05', 'P18', 'P39', 'P40'],
        '第3组': ['P02', 'P15', 'P17', 'P24', 'P32'],
        '第4组': ['P01', 'P04', 'P07', 'P09', 'P13', 'P20', 'P25', 'P26', 'P27', 'P29', 'P38'],
        '第5组': ['P03', 'P10', 'P14', 'P31', 'P34', 'P35']
    }

    try:
        # 加载降雨数据
        print("正在加载降雨数据...")
        rainfall_data = load_rainfall_data('附件2.xlsx')
        print(f"数据加载成功！共{len(rainfall_data)}个气象站，{len(rainfall_data.columns)}个时间点")

        # 创建评价模型
        model = TOPSIS_Entropy()

        # 对每个分组进行评价
        all_results = []

        for group_name, station_list in groups.items():
            print(f"正在评估{group_name}...")

            # 提取该组的降雨数据
            group_data = rainfall_data.loc[station_list]

            # 计算指标
            indicators, valid_station_ids = model.calculate_indicators(group_data)

            # TOPSIS评价
            group_results = model.topsis_evaluation(indicators, valid_station_ids, group_name)
            all_results.extend(group_results)

        # 创建结果DataFrame
        results_df = pd.DataFrame(all_results)

        # 按综合得分排序
        results_df = results_df.sort_values('综合得分', ascending=False)

        # 输出结果
        print("\n" + "=" * 100)
        print("气象站降雨监测综合效能评价结果")
        print("=" * 100)
        print(
            f"{'排名':<4} {'气象站ID':<8} {'所属分组':<8} {'综合得分':<10} {'等级':<6} {'数据质量':<12} {'区域覆盖':<12} {'特征捕捉':<12}")
        print("-" * 100)

        for i, (idx, row) in enumerate(results_df.iterrows(), 1):
            print(f"{i:<4} {row['气象站ID']:<8} {row['所属分组']:<8} {row['综合得分']:.6f}   {row['等级']:<6} "
                  f"{row['数据质量可靠性']:.4f}       {row['区域覆盖有效性']:.4f}       {row['特征捕捉精准性']:.4f}")

        # 统计信息
        print("\n统计信息:")
        print("=" * 50)
        print(f"总气象站数量: {len(results_df)}")
        print(f"平均综合得分: {results_df['综合得分'].mean():.4f}")
        print(f"最高得分: {results_df['综合得分'].max():.4f}")
        print(f"最低得分: {results_df['综合得分'].min():.4f}")

        # 等级分布
        grade_distribution = results_df['等级'].value_counts().sort_index()
        print("\n等级分布:")
        for grade, count in grade_distribution.items():
            print(f"{grade}级: {count}个气象站 ({count / len(results_df) * 100:.1f}%)")

        # 分组统计
        print("\n按分组统计:")
        group_stats = results_df.groupby('所属分组').agg({
            '综合得分': ['mean', 'std', 'max', 'min'],
            '等级': lambda x: (x == 'A').sum() + (x == 'A+').sum()
        }).round(4)
        print(group_stats)

        # 可视化
        visualize_results(results_df)

        # 保存结果
        results_df.to_excel('气象站综合评价结果.xlsx', index=False)
        print(f"\n详细结果已保存到 '气象站综合评价结果.xlsx'")

        # 提供决策建议
        provide_recommendations(results_df)

    except Exception as e:
        print(f"错误: {e}")
        print("请检查文件路径和数据格式是否正确")


def visualize_results(results_df):
    """可视化结果"""
    plt.figure(figsize=(15, 10))


    # 设置中文字体（根据你的系统选择合适的字体）

    plt.rcParams['font.sans-serif'] = ['SimHei', 'FangSong', 'Microsoft YaHei', 'KaiTi', 'Arial Unicode MS']

    plt.rcParams['axes.unicode_minus'] = False  # 正常显示负号
    # 子图1: 综合得分分布
    plt.subplot(2, 2, 1)
    plt.hist(results_df['综合得分'], bins=20, alpha=0.7, color='skyblue')
    plt.title('气象站综合得分分布')
    plt.xlabel('综合得分')
    plt.ylabel('频数')

    # 子图2: 等级分布
    plt.subplot(2, 2, 2)
    grade_counts = results_df['等级'].value_counts().sort_index()
    plt.bar(grade_counts.index, grade_counts.values, color=['red', 'orange', 'yellow', 'green', 'lightgreen'])
    plt.title('气象站等级分布')
    plt.xlabel('等级')
    plt.ylabel('数量')

    # 子图3: 三个维度得分热力图
    plt.subplot(2, 2, 3)
    dimension_scores = results_df[['数据质量可靠性', '区域覆盖有效性', '特征捕捉精准性']]
    sns.heatmap(dimension_scores.corr(), annot=True, cmap='coolwarm', center=0)
    plt.title('三个维度相关性热力图')

    # 子图4: 各组平均得分
    plt.subplot(2, 2, 4)
    group_means = results_df.groupby('所属分组')['综合得分'].mean().sort_values()
    plt.barh(group_means.index, group_means.values, color='lightcoral')
    plt.title('各组平均综合得分')
    plt.xlabel('平均综合得分')

    plt.tight_layout()
    plt.savefig('气象站评价结果可视化.png', dpi=300, bbox_inches='tight')
    plt.show()


def provide_recommendations(results_df):
    """提供决策建议"""
    print("\n" + "=" * 80)
    print("监测优化与资源倾斜建议")
    print("=" * 80)

    # 优秀站建议
    excellent_stations = results_df[results_df['等级'].isin(['A+', 'A'])]
    if len(excellent_stations) > 0:
        print(f"\n优秀气象站 ({len(excellent_stations)}个):")
        print("建议作为基准站，维持现有监测水平")
        for _, station in excellent_stations.iterrows():
            strength = max(['数据质量可靠性', '区域覆盖有效性', '特征捕捉精准性'],
                           key=lambda x: station[x])
            print(f"  {station['气象站ID']} ({station['所属分组']}) - 优势维度: {strength}")

    # 需改进站建议
    poor_stations = results_df[results_df['等级'] == 'D']
    if len(poor_stations) > 0:
        print(f"\n需重点改进气象站 ({len(poor_stations)}个):")
        print("建议优先进行资源倾斜和设备维护")
        for _, station in poor_stations.iterrows():
            weakness = min(['数据质量可靠性', '区域覆盖有效性', '特征捕捉精准性'],
                           key=lambda x: station[x])
            print(f"  {station['气象站ID']} ({station['所属分组']}) - 薄弱维度: {weakness}")

    # 分组建议
    print(f"\n分组资源分配建议:")
    for group_name in results_df['所属分组'].unique():
        group_data = results_df[results_df['所属分组'] == group_name]
        avg_score = group_data['综合得分'].mean()

        if avg_score >= 0.7:
            suggestion = "资源配置充足，可维持现状"
        elif avg_score >= 0.6:
            suggestion = "资源配置适中，可适当优化"
        elif avg_score >= 0.5:
            suggestion = "资源相对不足，需要增加投入"
        else:
            suggestion = "资源严重不足，需重点倾斜"

        print(f"  {group_name}: 平均得分{avg_score:.3f} - {suggestion}")


if __name__ == "__main__":
    main()