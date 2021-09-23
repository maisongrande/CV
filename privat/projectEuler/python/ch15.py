
'''
Binomial koeffisient hvor antallet valg er 40,
antallet delvalg er 20
'''

def fac(n):
    if n < 1:
        return 1
    return n*fac(n-1)

possib = fac(40) / (fac(20) * fac(40 - 20))
print possib
