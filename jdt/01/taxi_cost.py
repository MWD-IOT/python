km = int(input("请输入里程数/千米："))
wait_time = int(input("请输入等待时间/分钟："))


def taxi_cost(km, wait_time):
    if km <= 3:
        cost = 13 + wait_time
    elif 3 < km <= 15:
        cost = 13 + (km - 3) * 2.3 + wait_time
    else:
        cost = 13 + (km - 3) * 2.3 + (km - 15) * 2.3 * 1.5 + wait_time 
    print("车费：{:.2f}".format(cost))


taxi_cost(km, wait_time)
