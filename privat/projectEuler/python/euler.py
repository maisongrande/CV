#!/usr/bin/env python2


## TOOLS:
fib = lambda n: n if n < 2 else fib(n-1)+fib(n-2)

def isPalindrome(n):
    number = str(n)
    if number == number[::-1]:
        bin = str(format(n, 'b'))
        if bin == bin[::-1]:
            return True
    return False


def palind(d):
    st = str(d)
    if st[::-1] == st:
        return True
    return False



def isPrime(n):
    if n < 2:
        return False
    for i in xrange(3, int(sqrt(n)) + 2):
        if not n%i:
            return False
    return True



def fac(n):
    if n < 1:
        return 1
    return n*fac(n-1)




##############################################################################


def challenge1():
    res = sum(filter(lambda x: not x%3 or not x%5, range(1000)))
    return res



##############################################################################


def challenge2():
    MAX_SUM = 4000000
    i = s = 0
    while s < MAX_SUM:
        if fib(i) % 2:
            s += fib(i)
        i += 1
    return s

##############################################################################

def challenge4():
    res = 0
    for i in xrange(999, 100, -1):
        for j in xrange(999, 100, -1):
            if palind(j*i):
                if j*i > res:
                    res = j*i
    return res


##############################################################################


def challenge5():
    i = 2520
    not_done = True    
    def test_mod(n):
        if sum([n%i for i in xrange(11, 21)]) == 0:
            return False
        return True
    while (not_done):
        i += 10
        not_done = test_mod(i)
    return i


##############################################################################


def challenge6():
    res1 = [pow(i, 2) for i in xrange(1, 101)]
    res2 = pow(sum(xrange(1, 101)),2)
    tot = res2 - sum(res1)
    return tot


##############################################################################

def challenge7():
    primes = []
    primes.append(2)
    i = 2
    while len(primes) < 10001:
        i += 1
        if isPrime(i):
            primes.append(i)
    return primes[-1]

##############################################################################



bigNum =\
"73167176531330624919225119674426574742355349194934\
96983520312774506326239578318016984801869478851843\
85861560789112949495459501737958331952853208805511\
12540698747158523863050715693290963295227443043557\
66896648950445244523161731856403098711121722383113\
62229893423380308135336276614282806444486645238749\
30358907296290491560440772390713810515859307960866\
70172427121883998797908792274921901699720888093776\
65727333001053367881220235421809751254540594752243\
52584907711670556013604839586446706324415722155397\
53697817977846174064955149290862569321978468622482\
83972241375657056057490261407972968652414535100474\
82166370484403199890008895243450658541227588666881\
16427171479924442928230863465674813919123162824586\
17866458359124566529476545682848912883142607690042\
24219022671055626321111109370544217506941658960408\
07198403850962455444362981230987879927244284909188\
84580156166097919133875499200524063689912560717606\
05886116467109405077541002256983155200055935729725\
71636269561882670428252483600823257530420752963450"

def challenge8():
    new_sum = 1; old_sum = 1
    for i in range(13, len(bigNum)+1, 1):
        for j in range(i-13, i, 1):
            new_sum *= int(bigNum[j])
            if new_sum > old_sum:
                old_sum = new_sum
                new_sum = 1
    return old_sum



##############################################################################


def challenge9():
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
    return product


##############################################################################


def challenge10():
    final_num = 2000000
    current_sum = 2
    i = 3
    while i < final_num:
        if isPrime(i):
            current_sum += i
        i += 1
    return current_sum


##############################################################################

ch11grid = \
"08 02 22 97 38 15 00 40 00 75 04 05 07 78 52 12 50 77 91 08 \
49 49 99 40 17 81 18 57 60 87 17 40 98 43 69 48 04 56 62 00 \
81 49 31 73 55 79 14 29 93 71 40 67 53 88 30 03 49 13 36 65 \
52 70 95 23 04 60 11 42 69 24 68 56 01 32 56 71 37 02 36 91 \
22 31 16 71 51 67 63 89 41 92 36 54 22 40 40 28 66 33 13 80 \
24 47 32 60 99 03 45 02 44 75 33 53 78 36 84 20 35 17 12 50 \
32 98 81 28 64 23 67 10 26 38 40 67 59 54 70 66 18 38 64 70 \
67 26 20 68 02 62 12 20 95 63 94 39 63 08 40 91 66 49 94 21 \
24 55 58 05 66 73 99 26 97 17 78 78 96 83 14 88 34 89 63 72 \
21 36 23 09 75 00 76 44 20 45 35 14 00 61 33 97 34 31 33 95 \
78 17 53 28 22 75 31 67 15 94 03 80 04 62 16 14 09 53 56 92 \
16 39 05 42 96 35 31 47 55 58 88 24 00 17 54 24 36 29 85 57 \
86 56 00 48 35 71 89 07 05 44 44 37 44 60 21 58 51 54 17 58 \
19 80 81 68 05 94 47 69 28 73 92 13 86 52 17 77 04 89 55 40 \
04 52 08 83 97 35 99 16 07 97 57 32 16 26 26 79 33 27 98 66 \
88 36 68 87 57 62 20 72 03 46 33 67 46 55 12 32 63 93 53 69 \
04 42 16 73 38 25 39 11 24 94 72 18 08 46 29 32 40 62 76 36 \
20 69 36 41 72 30 23 88 34 62 99 69 82 67 59 85 74 04 36 16 \
20 73 35 29 78 31 90 01 74 31 49 71 48 86 81 16 23 57 05 54 \
01 70 54 71 83 51 54 69 16 92 33 48 61 43 52 01 89 19 67 48"


def horisAndVertMax(gr):
    '''
    Maximum sum of vertical and horisontal numbers.
    '''
    hori_sum = 0
    vert_sum = 0
    for i in xrange(20):
        for j in xrange(3, 20, 1):
            new_hori_sum = gr[i][j] * gr[i][j-1] * gr[i][j-2] * gr[i][j-3]
            new_vert_sum = gr[j][i] * gr[j-1][i] * gr[j-2][i] * gr[j-3][i]
            if new_hori_sum > hori_sum:
                hori_sum = new_hori_sum
            if new_vert_sum > vert_sum:
                vert_sum = new_vert_sum 
            new_hori_sum = new_vert_sum = 0
    return hori_sum, vert_sum

            
# Right Diag:
def rightDiagMax(gr):
    '''
    Maximum sum of right leaning diagonals.
    '''
    r_diag_sum = 0
    for i in xrange(16):
        for j in xrange(16):
            new_R_diag_sum  = gr[i][j] * gr[i+1][j+1] * gr[i+2][j+2] * gr[i+3][j+3]
            if new_R_diag_sum > r_diag_sum:
                r_diag_sum = new_R_diag_sum
            new_R_diag_sum = 0
    return r_diag_sum

        

def leftDiagMax(gr):
    '''
    Maximum sum of left leaning diagonals.
    '''
    l_diag_sum = 0
    for i in xrange(16):
        for j in xrange(3, 20, 1):
            new_L_diag_sum  = gr[i][j] * gr[i+1][j-1] * gr[i+2][j-2] * gr[i+3][j-3]
            if new_L_diag_sum > l_diag_sum:
                l_diag_sum = new_L_diag_sum
            new_L_diag_sum = 0
    return l_diag_sum



def challenge11():
    grid_arr = [int(i) for i in ch11grid.split()]
    grid = np.array(grid_arr).reshape(20,20)
    max1, max2 = horisAndVertMax(grid)
    max3 = rightDiagMax(grid)
    max4 = leftDiagMax(grid)
    return max(max1, max2, max3, max4)


##############################################################################


def make_triangular(n):
    return len([i for i in xrange(1, int(sqrt(n)+1)) if not n%i ])*2

def challenge12():
    i = 1
    growth = 2
    while make_triangular(i) < 500:
        i += growth
        growth += 1
    return i




##############################################################################


def collatz(n, i):
    if n == 1:
        return i
    if not n%2:
        return collatz(n/2, i+1)
    else:
        return collatz(3*n + 1, i+1)


def challenge13():
    i = 1
    old_res = 0
    the_num = 0
    while i < 1000000:
        res = collatz(i, 1)
        if res > old_res:
            old_res = res
            the_num = i
        i +=1
    return the_num, old_res




##############################################################################


def challenge14():
    '''
    Binomial koeffisient hvor antallet valg er 40,
    antallet delvalg er 20
    '''
    possib = fac(40) / (fac(20) * fac(40 - 20))
    return possib


##############################################################################


def challenge15():
    return sum([int(i) for i in str(pow(2,1000))])




##############################################################################


itow = {0:'',1:'one', 2:'two', 3:'three', 4:'four', 5:'five', 6:'six',
               7:'seven', 8:'eight', 9:'nine',
               10:'ten',11:'eleven',12:'twelve',13:'thirteen',
               14:'fourteen',15:'fifteen', 16:'sixteen', 17:'seventeen',
               18:'eighteen', 19:'nineteen', 20:'twenty', 30:'thirty',
               40:'forty', 50:'fifty', 60:'sixty', 70:'seventy',
               80:'eighty', 90:'ninety', 100:'hundred', 1000:'onethousand'}



def numberToText(n):
    global itow
    if n < 20:
        return itow[n]
    elif n < 100:
        return itow[n//10 * 10] + itow[n % 10]
    elif n < 1000:
        numb = str(n)
        if not n % 100:
            return itow[int(numb[0])]+itow[100]
        if numb[1] == '0':
            return itow[int(numb[0])]+itow[100]+'and'+itow[int(numb[-1])] 
        elif numb[1] == '1':
            return itow[int(numb[0])]+itow[100]+'and'+itow[int(numb[1:])]
        else:
            return itow[int(numb[0])]+itow[100]+'and'+itow[int(numb[1:])/10*10]+itow[int(numb[-1])]
    elif n == 1000:
        return itow[n]


    
def challenge16():    
    s = 0    
    for i in xrange(1,1001):
        s += len(numberToText(i))
    return s



##############################################################################


challenge17num = [[75],
     [95, 64],
     [17, 47, 82],
     [18, 35, 87, 10],
     [20, 4, 82, 47, 65],
     [19, 1, 23, 75, 3, 34],
     [88, 2, 77, 73, 7, 63, 67],
     [99, 65, 4, 28, 6, 16, 70, 92],
     [41, 41, 26, 56, 83, 40, 80, 70, 33],
     [41, 48, 72, 33, 47, 32, 37, 16, 94, 29],
     [53, 71, 44, 65, 25, 43, 91, 52, 97, 51, 14],
     [70, 11, 33, 28, 77, 73, 17, 78, 39, 68, 17, 57],
     [91, 71, 52, 38, 17, 14, 91, 43, 58, 50, 27, 29, 48],
     [63, 66, 4, 68, 89, 53, 67, 30, 73, 16, 69, 87, 40, 31],
     [4, 62, 98, 27, 23, 9, 70, 98, 73, 93, 38, 53, 60, 4, 23]]



def challenge17():
    tria = np.asarray(challenge17num)
    for i in xrange(13, -1, -1):
        for j in xrange(0, i+1):
            tria[i][j] = tria[i][j] + max(tria[i+1][j], tria[i+1][j+1])
    return tria[0]


##############################################################################


# leap_months = [31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31]
# reg_months = [31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31]
# week_days = ['monday','tuesday','wednesday','thursday','friday','saturday','sunday']


# def isLeapYear(n):
#     return not n%4 or not n%100



# def leapYear():
#     global days_passed, sundays
#     for months in leap_months:
#          for days in xrange(1, int(months)+1):
#             days_passed += 1
#             if days == 1 and week_days[days_passed%7] == 'sunday':
#                 sundays += 1



# def commonYear():
#     global days_passed, sundays
#     for months in reg_months:
#         for days in xrange(1, int(months)+1):
#             days_passed += 1
#             if days == 1 and week_days[days_passed%7] == 'sunday':
#                 sundays += 1

# sundays = 0
# days_passed = 0
            
# for years in xrange(1901,2001):
#     if isLeapYear(years):
#         leapYear()
#     else:
#         commonYear()
        
# print days_passed, sundays




##############################################################################

def challenge19():
    fac = lambda n: 1 if n < 1 else n*fac(n-1)
    res = sum([int(i) for i in str(fac(100))])
    return res

##############################################################################

def d(n):
    return sum([i for i in range(1, n/2 + 1) if not n%i])



def challenge20():
    cache = [-1]*100000
    s = 0
    b = 0
    for a in range(10000):
        b = d(a)
        if d(b) == a and a != b:
            s+=a
            cache[b] = b
    return s



##############################################################################


def challenge21():
    names = ""
    names_lst = []
    summ = 0
    part_sum = 0

    with open('names.txt', 'rb') as infile:
        names = infile.read()
        names_lst = sorted(names.replace('"',' ').replace(',','').split())
    
    alpha = 'ABCDEFGHIJKLMNOPQRSTUVWXYZ'
    alpha_dict = {}

    for i, letter in enumerate(alpha):
        alpha_dict[letter] = i+1

    for ind, nam in enumerate(names_lst):
        part_sum = 0
        for l in nam:
            part_sum += alpha_dict[l]
            summ += (ind+1)*part_sum
    return summ



##############################################################################



def abundant_num(number):
    '''
    Samme algoritme som for de triangulaere tallene, men tallet selv
    utgaar som element
    '''
    abundant_div_sum  = sum([i for i in xrange(1, number/2 + 1) if not number % i ])
    return True if abundant_div_sum > number else False


def challenge22():
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
    return sum(nats)



####ch23##########################################################################





# lst = list('0123456789')
# perm = permutations(lst)

# # litt treg:
# #res = list(perm)
# #print res[999999]


# # Raskere:
# k = 1
# for i in perm:
#     if k == 1000000:
#         print "".join(i)
#         break
#     k +=1

# def fib(n):
#     '''
#     Regner ut fibonacci vha
#     memisering.
#     '''
#     global cache
#     if n < 2:
#         return n
#     if cache[n] != -1:
#         return cache[n]        
#     cache[n] = fib(n-1) + fib(n-2)
#     return cache[n]

# cache = [-1]*500000
# res = 0;
# i = -1 # hack 

# while res < 1000:
#     i += 1
#     fib(i)
#     res = len(str(cache[i]))


# print i

##############################################################################



# cycle = 0
# numerator = 1.
# old_sum = 0
# new_sum = 0


# for denumerator in range(1, 100):
#     frac = Decimal(numerator) / Decimal(denumerator)
#     cycle = str(frac).split('.')[-1]
#     first = cycle[0]
#     last_part =  cycle[len(cycle)//2:]
#     first_part = cycle[:len(cycle)//2]
#     print cycle
#     print first_part, last_part
#     if first_part == last_part:
#         new_sum = len(first_part)
#         if new_sum > old_sum:
#             old_sum = new_sum
            
# print old_sum


##############################################################################


# s1 = [1]
# s2 = [1]


# for i in range(1, 1001):
#     s1.append(s1[-1] + 2*i)


# for j in range(1000):
#     s2.append(s2[-1] + ((j/2)+1) * 4)

# print sum(s1+s2)-1





##############################################################################






# def isCircular(n):
#     parts = str(n)
#     l = len(parts)
#     for i in xrange(l):
#         parts = parts[1:] + parts[0]        
#         if not isPrime(int(parts)):
#             return False
#     return True


# circ_count = 13 # 13 circular primes < 100

# for i in xrange(100, 1000000):
#     if isCircular(i):
#         circ_count += 1
            
# print circ_count





##############################################################################





# s = 0

# for i in xrange(1000000):
#     if isPalindrome(i):
#         s += i

# print s



##############################################################################




# with open('p067_triangle.txt', 'rb') as tri_file:
#     tri = np.asarray([[int(i) for i in lines.split()] for lines in tri_file])

# for i in xrange(98, -1, -1):
#     for j in xrange(0, i+1):
#         tri[i][j] = tri[i][j] + max(tri[i+1][j],tri[i+1][j+1])

# print tri[0][0]




if __name__ == '__main__':
    from decimal import *
    from itertools import permutations
    import numpy as np
    from math import sqrt

    print "{} {}".format("Answer one: ",challenge1())
    print "{} {}".format("Answer two: ",challenge2())

    print "{} {}".format("Answer four: ",challenge4())
    print "{} {}".format("Answer five: ",challenge5())
    print "{} {}".format("Answer six: ",challenge6())
    print "{} {}".format("Answer seven: ",challenge7())
    print "{} {}".format("Answer eight: ",challenge8())
    print "{} {}".format("Answer nine: ",challenge9())
    print "{} {}".format("Answer ten: ",challenge10())
    print "{} {}".format("Answer elleven: ",challenge11())
    print "{} {}".format("Answer twelve: ",challenge12())
    print "{} {}".format("Answer thirteen: ",challenge13())
    print "{} {}".format("Answer fourteen: ",challenge14())        
    print "{} {}".format("Answer fifteen: ",challenge15())
    print "{} {}".format("Answer sixteen: ",challenge16())
    print "{} {}".format("Answer seventeen: ",challenge17())        
    #print "{} {}".format("Answer eighteen: ",challenge18())
    print "{} {}".format("Answer nineteen: ",challenge19())
    print "{} {}".format("Answer twenty: ",challenge20())    
    print "{} {}".format("Answer twentyone: ",challenge21())
    print "{} {}".format("Answer twentytwo: ",challenge22())
    #print "{} {}".format("Answer twentythree: ",challenge23())
    #print "{} {}".format("Answer twentyfour: ",challenge24())    
    
