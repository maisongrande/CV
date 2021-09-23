from decimal import *
cycle = 0
numerator = 1.
old_sum = 0
new_sum = 0


for denumerator in range(1, 100):
    frac = Decimal(numerator) / Decimal(denumerator)
    cycle = str(frac).split('.')[-1]
    first = cycle[0]
    last_part =  cycle[len(cycle)//2:]
    first_part = cycle[:len(cycle)//2]
    print cycle
    print first_part, last_part
    if first_part == last_part:
        new_sum = len(first_part)
        if new_sum > old_sum:
            old_sum = new_sum
            
print old_sum
