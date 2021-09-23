
def isPalindrome(n):
    number = str(n)
    if number == number[::-1]:
        bin = str(format(n, 'b'))
        if bin == bin[::-1]:
            return True
    return False


s = 0

for i in xrange(1000000):
    if isPalindrome(i):
        s += i

print s
