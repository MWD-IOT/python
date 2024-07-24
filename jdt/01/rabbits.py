# 相当于斐波那契数列


def count_rabbits(month):
    if month <= 2:
        return 1
    else:
        return count_rabbits(month-1) + count_rabbits(month-2)


month = int(input("请输入月份数："))
total_rabbits = count_rabbits(month)
print(total_rabbits)
