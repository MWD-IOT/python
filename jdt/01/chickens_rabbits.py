n = int(input())  # 输入测试数据的组数

for _ in range(n):
    feet = int(input())  # 输入脚的数量

    # 计算最小值和最大值
    min_animals = feet // 4 + (feet % 4) // 2
    max_animals = feet // 2

    # 根据最小值和最大值进行输出
    print(min_animals, max_animals)
