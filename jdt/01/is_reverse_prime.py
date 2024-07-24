# 判断是不是素数
def is_prime(num):
    if num < 2:
        return False
    for i in range(2, int(num ** 0.5) + 1):  # 遍历 2 ~ 根号num+1 之间的所有整数，
        if num % i == 0:  # 若num能被任一个i整除则非素数，否则num是素数
            return False
    return True


# 判断是不是反素数
def is_reverse_prime(num):
    reverse_num = int(str(num)[::-1])
    return is_prime(reverse_num)


n = int(input("请输入一个数（求前多少个反素数）："))


reverse_primes = []
num = 2

while len(reverse_primes) < n:
    if is_reverse_prime(num) and not str(num) == str(num)[::-1]:  # 判断是反素数并且不是回文数
        reverse_primes.append(num)
    num += 1

print(' '.join(map(str, reverse_primes)))
