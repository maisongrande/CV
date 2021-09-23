
itow = {0:'',1:'one', 2:'two', 3:'three', 4:'four', 5:'five', 6:'six',
               7:'seven', 8:'eight', 9:'nine',
               10:'ten',11:'eleven',12:'twelve',13:'thirteen',
               14:'fourteen',15:'fifteen', 16:'sixteen', 17:'seventeen',
               18:'eighteen', 19:'nineteen', 20:'twenty', 30:'thirty',
               40:'forty', 50:'fifty', 60:'sixty', 70:'seventy',
               80:'eighty', 90:'ninety', 100:'hundred', 1000:'onethousand'}



def number(n):
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
    
s = 0    

for i in xrange(1,1001):
    s += len(number(i))
    #print number(i)
        
print s
