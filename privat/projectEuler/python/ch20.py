fac = lambda n: 1 if n < 1 else n*fac(n-1)

res = sum([int(i) for i in str(fac(100))])

print res
