
s = float(input())
if s < 0:
    print("error")
else:
    salary = s - 5000
    if salary <= 0:
        fee, num = 0, 0
    elif salary <= 3000:
	    fee, num = 3, 0
    elif salary <= 12000:
	    fee, num = 10, 210
    elif salary <= 25000:
        fee, num = 20, 1410
    elif salary <= 35000:
	    fee, num = 25, 2660
    elif salary <= 55000:
	    fee, num = 30, 4410
    elif salary <= 80000:
        fee, num = 35, 7160
    else:
	    fee, num = 45, 15160
    tax = abs(salary * fee / 100 - num)
    print("应缴税款{:.2f}元，实发工资{:.2f}元。".format(tax,salary + 5000 - tax))
