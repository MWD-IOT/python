for x in range(1, 21):
    for y in range(1, 34):
        for z in range(1, 99):
            if (x + y + z == 100) and (5 * x + 3 * y + z / 3 == 100) and (z % 3 == 0):
                print("公鸡：{} 母鸡：{} 小鸡：{}".format(x, y, z))
