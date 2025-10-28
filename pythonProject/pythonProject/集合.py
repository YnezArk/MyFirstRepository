lst1 = list(range(1,11))
print(lst1)
for num,item in enumerate(lst1,5):
    print(num,item)
lst1.reverse()
print(lst1)
import random
lst2 =[random.randint(1,100) for i in range(100)]
lst2.sort(key=None,reverse=False)
print(lst2)
lst3=list([[i for i in range(21)] for j in range(10)])
print(lst3)
for row in lst3:
    for item in row:
        print(item,end=" ")
    print()
print("-"*40)
t = (i for i in range(1,5))
print(t.__next__())

