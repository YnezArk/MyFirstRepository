for i in range(1,9):
    for j in range(1,i+1):
        print("*",end="")
    print()
print("----------------------")
h = int(input("height:"))
i = 0
while i <= h:
    print(" "*(h-i),end="")
    print("*"*(2*i-1))#1,3,5
    i+=1

for i in range(1,11):
    for j in range(1,2*i-11):
        if j == 1 or j == 2*i-11:
            print("*", end="")
        else:
            print(" ",end="")
    for k in range(1,2*(21-(2*i-1))):
        if k == 1 or k == 2*(21-(2*i-1)):
            print("*",end="")
    print()
print("----------------------")

