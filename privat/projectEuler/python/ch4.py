def palind(d):
    st = str(d)
    if st[::-1] == st:
        return True
    return False


res = 0

for i in xrange(999, 100, -1):
    for j in xrange(999, 100, -1):
        if palind(j*i):
            if j*i > res:
                res = j*i

print res


