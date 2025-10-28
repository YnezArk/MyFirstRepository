x = input("输入数字：")
num = len(x)
x = int(x)
mask = 10 ** (num - 1)
for i in range(num):
    result = int(x // mask)
    x = x % mask
    mask /= 10
    print(result, end=" ")
print("\n-------------------\n\n\n\n\n")

