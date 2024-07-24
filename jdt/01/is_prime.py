def is_prime(n):
    if n < 2:
        return False
    for i in range(2, int(n ** 0.5) + 1):
        if n % i == 0:
            return False
    return True


num = int(input("请输入一个正整数："))

if is_prime(num):
    print(f"{num} 是素数")
else:
    print(f"{num} 不是素数")
