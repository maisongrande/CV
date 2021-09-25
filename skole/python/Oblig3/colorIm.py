#!/usr/bin/env python2




def rgb2hsi(imgData, uR=0,uG=0,uB=0):
    '''
    Converts the image's RGB channels over to the 
    HSI band. This is done through Weave and the 
    compiled algorithm described in cLib.py

    >>> i = [[[0,222,2],[22,0,33],[44,123,1]]]
    >>> rgb2hsi(i)
    array([[[ 120.44903564,    1.        ,   74.66666412],
            [ 280.8934021 ,    1.        ,   18.33333397],
            [  99.66841888,    0.98214287,   56.        ]]])
    '''
    from scipy.weave import inline
    from cLib import rgbTohsi
    import ctypes
    import numpy as np
    v = np.array(imgData, dtype=np.float)
    v.ravel(order = 'C')
    n,m,k = v.shape
    io = ['n', 'm','v', 'uR','uG','uB']
    inline(rgbTohsi, io, headers=['<math.h>'])
    return v





def colorImgDnoise(imgdat, iters=10, kappa=0.1):
    '''
    "Close to the metal" noise reduction done through c-code.

    Arguments:
        -imageObject.
        -nTimes smoothing (default 10).
        -Degree of filtering (default 0.1).
        
    Returns:
        - Numpy array.
    '''
    from cLib import dNoiseClr
    from scipy.weave import inline
    import numpy as np
    v = imgdat.astype(np.float)
    n, m, k = imgdat.shape[:3]
    t = np.zeros_like(imgdat)
    io = ['n', 'm', 'k', 't', 'iters', 'kappa', 'v', 'imgdat']
    inline(dNoiseClr, io)
    return v 




def hsi2rgb(hsiImg, uH=0, uS=0, uI=0):
    '''
    Converts the image's RGB channels over to the 
    HSI band. This is done through Weave and the 
    compiled algorithm described in cLib.py

    >>> i = [[[120.44903564, 1., 74.66666412],[280.8934021 , 1. , 18.33333397],[99.66841888, 0.98214287, 56.]]]
    >>> hsi2rgb(i)
    array([[[  0, 221,   1],
            [ 22,   0,  32],
            [ 43, 123,   1]]], dtype=uint8)
    '''
    from scipy.weave import inline
    from cLib import hsiToRgb
    import ctypes
    import numpy as np
    v = np.array(hsiImg, dtype=np.float)
    u = np.array(hsiImg, dtype=np.uint8)
    v.ravel(order = 'C')
    n, m, k = v.shape[:3]
    io = ['n', 'm', 'v', 'uH', 'uS', 'uI']
    inline(hsiToRgb, io, headers=['<math.h>'])
    u = v.astype(np.uint8)
    return u




def _test():
    '''
    Doctest
    '''
    import doctest
    doctest.testmod()



if __name__=='__main__':
    '''
    If ran as standalone program.
    -Doctest.
    -Start timer.
    -Run image process.
    -stop timer.
    -Print result.
    '''
    import cProfile, pstats, StringIO
    from PIL import Image

    _test()
    pr = cProfile.Profile()
    pr.enable()
    
    data = Image.open('disastercolor.jpg')
    arr = rgb2hsi(data)
    arr1 = colorImgDnoise(arr)
    arr2 = hsi2rgb(arr1)

    Image.fromarray(arr2).save("colorTest.jpg")

    pr.disable()
    s=StringIO.StringIO()
    sortby='cumulative'
    ps=pstats.Stats(pr, stream=s).sort_stats(sortby)
    ps.print_stats(10)
    print s.getvalue()


    


 
