from math import sqrt

a = float(input("请输入方程的二次系数："))
b = float(input("请输入方程一次系数："))
c = float(input("请输入方程的常数："))


def equation(a, b, c):
    if a == 0:
        if b == 0:
            print("Data error!")
        else:
            x = -c / b
            print("{:.2f}".format(x))
    else:
        D = b ** 2 - (4 * a * c)
        if D < 0:
            print("该方程无实数解")
        elif D == 0:
            x1 = x2 = -b / (2 * a)
            print('{:.2f}'.format(x1))
        elif D > 0:
            x1 = (-b + sqrt(D)) / (2 * a)
            x2 = (-b - sqrt(D)) / (2 * a)
            print("{:.2f} {:.2f}".format(max(x1, x2), min(x1, x2)))
    return


equation(a, b, c)
