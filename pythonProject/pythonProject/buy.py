goods = {1001: 'Huawei Mate 60 Pro', 1002: 'IPhone 15 Pro Max', 1003: 'IQOO 13', 1004: 'Redmi K80'}
for i in goods:
    print(f'商品代码:{i},商品名称:{goods[i]}')
car = []
while True:
    num = input('输入代码:')
    if num.isdigit():
        num = int(num)
        if num in goods:
            car.insert(0, goods[num])
            print(f'已添加，当前购物车{car}')
        else:
            print(f'cannot find,当前购物车{car}')
    elif num == 'q':
        print("quit")
        if not bool(car):
            print("nothing")
        else:
            print(f"car:{car}")
        break
    else:
        print(f'not number，当前购物车:{car}')
