
def fib(n):
    '''
    Regner ut fibonacci vha
    memisering.
    '''
    global cache
    if n < 2:
        return n
    if cache[n] != -1:
        return cache[n]        
    cache[n] = fib(n-1) + fib(n-2)
    return cache[n]

cache = [-1]*500000
res = 0;
i = -1 # hack 

while res < 1000:
    i += 1
    fib(i)
    res = len(str(cache[i]))


print i



