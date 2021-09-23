from math import sqrt

done = False
product = 0

for a in xrange(500):
    if done:
        break
    for b in xrange(500):
        c =  sqrt(pow(a,2) + pow(b,2))
        the_sum = a + b + c
        if the_sum == 1000:
            product = a * b * c
            done = True
        
print product

