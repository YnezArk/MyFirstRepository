i = 0
while i < 3:
    user_name = input("酷爱输入用户名:")
    pwd = input("酷爱输入密码:")
    if user_name == "luson" and pwd == "<PASSWORD>":
        print("登录成功,luson太牛逼")
        i = 6
    else:
        i += 1
        print(f"密码错误,你还有{3-i}次机会")
if i == 3:
    print("luson太菜了")

