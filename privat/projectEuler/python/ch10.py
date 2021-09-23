from math import sqrt

def isPrime(n):
    pivot = int(sqrt(n))
    for i in xrange(2, pivot+1):
        if not n%i:
            return False
    return True

final_num = 2000000
current_sum = 2

i = 3

while i < final_num:
    if isPrime(i):
        current_sum += i
    i += 1

print current_sum
