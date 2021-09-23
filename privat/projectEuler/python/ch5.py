
i = 2520
not_done = True

def test_mod(n):
    if sum([n%i for i in xrange(2, 22)]) == 0:
        return False
    return True

while (not_done):
    i += 10
    not_done = test_mod(i)
        
#232792560

print i
