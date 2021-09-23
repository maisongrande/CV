primes = []
primes.append(2)

def isPrime(x, y):
    for i in xrange(2, y+1):
        if not x%i:
            return False
    return True

i = 2

while len(primes) < 10001:
    i += 1
    if isPrime(i, len(primes)):
        primes.append(i)

print(primes[-1])
