
names = ""
names_lst = []
with open('names.txt', 'rb') as infile:
    names = infile.read()
    names_lst = sorted(names.replace('"',' ').replace(',','').split())
    
alpha = 'ABCDEFGHIJKLMNOPQRSTUVWXYZ'
alpha_dict = {}

for i, letter in enumerate(alpha):
    alpha_dict[letter] = i+1

    
summ = 0
part_sum = 0

for ind, nam in enumerate(names_lst):
    part_sum = 0
    for l in nam:
        part_sum += alpha_dict[l]

    summ += (ind+1)*part_sum

print summ
