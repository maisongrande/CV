from math import sqrt

def abundant_num(number):
    '''
    Samme algoritme som for de triangulaere tallene, men tallet selv
    utgaar som element
    '''
    abundant_div_sum  = sum([i for i in xrange(1, number/2 + 1) if not number % i ])
    return True if abundant_div_sum > number else False


list_of_abundant = set()
nats = set(xrange(28124))
to_be_del = set()


for i in xrange(28124):
    if abundant_num(i):
        list_of_abundant.add(i)



for elem in list_of_abundant:
    for j in list_of_abundant:
        x = elem + j
        if x in nats:
            to_be_del.add(x)

            
for k in to_be_del:
    if k in nats:
        nats.remove(k)

print sum(nats)

