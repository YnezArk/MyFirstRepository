def isDouble(n):
    if n % 2 == 0:
        return True
    else:
        return False
num = int(input())
if isDouble(num):
    print("ou")
else:
    print("ji")