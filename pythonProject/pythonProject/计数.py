from collections import Counter

# 给定的文本
text = input("shuru:")

# 将文本转换为小写并去除空格
text = text.lower().replace(" ", "")

# 使用Counter统计每个字母的出现次数
counter = Counter(text)

# 找出出现次数最多的字母
most_common_letter = counter.most_common(1)

# 打印结果
print(f"The most common letter is '{most_common_letter[0][0]}' with {most_common_letter[0][1]} occurrences.")