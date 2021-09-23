from itertools import permutations

lst = list('0123456789')
perm = permutations(lst)

# litt treg:
#res = list(perm)
#print res[999999]


# Raskere:
k = 1
for i in perm:
    if k == 1000000:
        print "".join(i)
        break
    k +=1
