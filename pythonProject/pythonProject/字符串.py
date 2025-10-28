n = input("num:")
if n.replace(".","",1).isdigit():
    n = float(n)
else:
    print("not num")
    exit()