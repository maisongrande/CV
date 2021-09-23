import numpy as np
s1 = [1]
s2 = [1]


for i in range(1, 1001):
    s1.append(s1[-1] + 2*i)


for j in range(1000):
    s2.append(s2[-1] + ((j/2)+1) * 4)

print sum(s1+s2)-1
