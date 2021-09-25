#!/usr/bin/env python2

def deNoiseList(origin, cols, rows, kappa=0.1, iters=10):
    '''
    Function de-noises an image recieved as a python list. 

    The de-noising algorithm is iterated over iters-times.
    
    The algorithm starts at second row, skips every first and
    last coloumn plus the last row. Else it filters every point.
    
    Lists are exchanged and the process is started over until
    end of iterations.

    >>> orig = [12,13,14]
    >>> c = 3; r = 3
    >>> deNoiseList(orig, c, r, 0.9, 10)
    [12, 13, 14]
    '''
    from copy import copy
    imgData = copy(origin)

    for y in xrange(0, iters):
        for x in xrange(cols, ((rows-1)*cols)-1):
            if (x-1)%cols != 0 and x%cols != 0:
                origin[x] = imgData[x] + kappa*(imgData[x-1] + imgData[x+1] -4*imgData[x] + imgData[x-cols] +imgData[x+cols])

        temp = list(imgData)
        imgData = list(origin)
        origin = list(temp)

    return origin






def test_deNoiseList():
    '''
    Tests the output of module against the original picture at one pixel.
    '''
    from PIL import Image
    #--- Collect original values.--->
    file2test = 'disasterbefore.jpg'
    origImage = Image.open(file2test)
    origData = list(origImage.getdata())
    test = Image.open(file2test)
    testDat = list(origImage.getdata())
    On, Om = test.size
    #---- Run de-noising on one.------>
    procImg = deNoiseList(testDat, On, Om, 0.1, 2)
    #----- Test Values:------->
    testVal =  origData[502]+ 0.1*(origData[501] + origData[503] - 4*origData[502] + origData[2] + origData[1002])
    expectVal = procImg[502]
    tolerance = 1e-14
    assert abs(expectVal-testVal) < tolerance, "Failed again..." 




def img2data(fName):
    '''
    Function opens the recieved image through the PIL module.
    PIL converts it to a list which is returned to caller.
    '''
    from PIL import Image
    im = Image.open(fName)
    img = list(im.getdata())
    n, m = im.size
    procIm = deNoiseList(img,n,m)
    return n, m, procIm




def data2img(n, m, data):
    '''
    Function restores the list into an image format
    and saves the file.
    '''
    from PIL import Image
    done = Image.new("L", (n, m))
    done.putdata(data)
    done.save('pyStyleDnoise.jpg')





def _test():
    '''
    Doc test function called when
    ran as standalone.
    '''
    import doctest, pytest
    doctest.testmod()





if __name__== '__main__':    
    '''
    When ran as stand alone module:
    - Doctest.
    - Start timer.
    - Run denoise.
    - Stop timer.
    - Print result.
    '''
    import cProfile, pstats, StringIO
    _test()
    pr = cProfile.Profile()
    pr.enable()
    #------ Process.
    n,m,procImg = img2data('disasterbefore.jpg')
    data2img(n,m,procImg)
    #------ Process Ends
    pr.disable()
    s=StringIO.StringIO()
    sortby='cumulative'
    ps=pstats.Stats(pr, stream=s).sort_stats(sortby)
    ps.print_stats(10)
    print(s.getvalue())


    
