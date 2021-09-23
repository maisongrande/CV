def collatz(n, i):
    if n == 1:
        return i
    if not n%2:
        return collatz2(n/2, i+1)
    else:
        return collatz2(3*n + 1, i+1)


i = 1
old_res = 0
the_num = 0


while i < 1000000:
    res = collatz(i, 1)
    if res > old_res:
        old_res = res
        the_num = i
    i +=1

print the_num, old_res
