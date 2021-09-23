
fib = lambda n: n if n < 2 else fib(n-1)+fib(n-2)

MAX_SUM = 4000000
i = s = 0

while s < MAX_SUM:
    if fib(i) % 2:
       s += fib(i)
    i += 1

print s

    
