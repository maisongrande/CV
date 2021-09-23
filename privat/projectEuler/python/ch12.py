from math import sqrt

def make_triangular(n):
    return len([i for i in xrange(1, int(sqrt(n)+1)) if not n%i ])*2

i = 1
growth = 2

while make_triangular(i) < 500:
    i += growth
    growth += 1


print i
