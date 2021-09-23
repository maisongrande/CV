import numpy as np

with open('p067_triangle.txt', 'rb') as tri_file:
    tri = np.asarray([[int(i) for i in lines.split()] for lines in tri_file])

for i in xrange(98, -1, -1):
    for j in xrange(0, i+1):
        tri[i][j] = tri[i][j] + max(tri[i+1][j],tri[i+1][j+1])

print tri[0][0]
