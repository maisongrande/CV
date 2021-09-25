#!/usr/bin/env python2

def imgCprocess(imDat, iters=10, kappa=0.1):
    '''
    "Close to the metal" noise reduction done through c-code.

    Arguments:
        -imageObject.
        -nTimes smoothing (default 10).
        -Degree of filtering (default 0.1).
        
    Returns:
        - Numpy array.

    Dependencies:
        - cLib.
        - scipy.weave.
        - ctypes.
        - numpy.

    Example:
    >>> imgCprocess([[111],[222],[333]], 1, 0.1)
    array([[111],
           [222],
           [ 77]], dtype=uint8)
    '''
    from cLib import dNoisebw
    from scipy.weave import inline
    import ctypes
    import numpy as np
    u = v = np.array(imDat, dtype=np.uint8)
    u.ravel(order = 'C')
    v.ravel(order = 'C')
    n, m = u.shape[:2]
    t = np.zeros((n,m), dtype=np.uint8)
    io = ['n', 'm','t','iters','kappa', 'u', 'v']
    inline(dNoisebw, io)
    return v


def _test():
    '''Doctest'''
    import doctest
    doctest.testmod()


if __name__=='__main__':
    '''
    If ran as standalone:
    -Doctest.
    -Start timer.
    -Denoise.
    -Stop timer.
    -Print result.
    '''
    import cProfile, pstats, StringIO
    from PIL import Image
    _test()
    pr = cProfile.Profile()
    pr.enable()
    #---- Process.
    data = Image.open('disasterbefore.jpg')
    arr = imgCprocess(data)
    Image.fromarray(arr).save("test.jpg")
    #---- End Process.
    pr.disable()
    s=StringIO.StringIO()
    sortby='cumulative'
    ps=pstats.Stats(pr, stream=s).sort_stats(sortby)
    ps.print_stats(10)
    print(s.getvalue())
