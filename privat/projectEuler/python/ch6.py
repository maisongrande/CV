import numpy

res1 = [ pow(i, 2) for i in xrange(1, 101)]
res2 = pow(sum(xrange(1, 101)),2)

tot = res2 - sum(res1)

print tot

        
        
