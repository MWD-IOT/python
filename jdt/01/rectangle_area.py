length = float(input("请输入矩形的长度："))
width = float(input("请输入矩形的宽度："))


def area(length, width):
    print("面积：{:.2f}".format(length * width))
    return True


area(length, width)
