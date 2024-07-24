n = int(input())
list = [x**n for x in range(10)]
[print(num) for num in range(10**(n-1), 10**n) if num == sum([list[int(i)] for i in str(num)])]
'''
n位的正整数必在区间[10**n-1, 10**n)内，
所有遍历所有此区间内的数，
并判断该数是否等于其各位上的数的n次幂之和
'''
