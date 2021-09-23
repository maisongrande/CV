leap_months = [31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31]
reg_months = [31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31]
week_days = ['monday','tuesday','wednesday','thursday','friday','saturday','sunday']


def isLeapYear(n):
    return not n%4 or not n%100




def leapYear():
    global days_passed, sundays
    for months in leap_months:
        for days in xrange(1, int(months)+1):
            days_passed += 1
            if days == 1 and week_days[days_passed%7] == 'sunday':
                sundays += 1



def commonYear():
    global days_passed, sundays
    for months in reg_months:
        for days in xrange(1, int(months)+1):
            days_passed += 1
            if days == 1 and week_days[days_passed%7] == 'sunday':
                sundays += 1

sundays = 0
days_passed = 0
            
for years in xrange(1901,2001):
    if isLeapYear(years):
        leapYear()
    else:
        commonYear()
        
print days_passed, sundays
