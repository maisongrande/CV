from math import sqrt


def isPrime(n):
    if n < 2:
        return False
    for i in xrange(2, int(sqrt(n)) + 1):
        if not n%i:
            return False
    return True


def isCircular(n):
    parts = str(n)
    l = len(parts)
    for i in xrange(l):
        parts = parts[1:] + parts[0]        
        if not isPrime(int(parts)):
            return False
    return True


circ_count = 13 # 13 circular primes < 100

for i in xrange(100, 1000000):
    if isCircular(i):
        circ_count += 1
            
print circ_count
