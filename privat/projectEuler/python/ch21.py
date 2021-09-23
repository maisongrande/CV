

def d(n):
    return sum([i for i in range(1, n/2 + 1) if not n%i])


cache = [-1]*100000

s = 0
b = 0

for a in range(10000):
    b = d(a)
    if d(b) == a and a != b:
        s+=a
        cache[b] = b

print s
