from collections import Counter
key = [1,2,3]
value = ["apple","banana","cherry"]
d = dict(zip(key,value))
print(d)
print("max",d[max(d)])
print("min",d.get(min(d)))
print("len",len(d))
print("-"*40)
lst = input("Enter text:")
print(lst)
r = Counter(lst)
print(r)
print("max:",max(r),r[max(r)],"times","min:",min(r),r[min(r)],"times")
for k,v in r.items():
    print(k,v)
