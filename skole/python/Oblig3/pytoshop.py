#!/usr/bin/env python2



def testImgType(fileName):
    '''
    Function tests if image is of band-type 'L'
    or RGB (B&W or Color). 

    Returns True or False pluss image object back to caller.
     
    >>> testImgType("disasterbefore.jpg") #doctest: +ELLIPSIS
    (<PIL.JpegImagePlugin.JpegImageFile image mode=L size=... at 0x...>, True)
    >>> testImgType("disastercolor.jpg") #doctest: +ELLIPSIS
    (<PIL.JpegImagePlugin.JpegImageFile image mode=RGB size=... at 0x...>, False)
    '''
    import sys, os
    if not os.path.isfile(fileName):
        print('Unable to locate file')
        sys.exit(0)
    im = Image.open(fileName)
    
    if im.getbands() == ('L',):
        return im, True
    elif im.getbands() == ('R','G','B'):
        return im, False
    else:
        print('%s is of unknown type'%filename)




def deNoiseBW(imgData, outFname, pyStyle, cStyle, i, k):
    '''
    Function opens image-file and collects relevant 
    data. Then based on which processor c/py is chosen 
    runs data through process and save result to new 
    image-file.
    
    -Arguments:
        image-filename that is to be processed (string).
        image-filename that is to be saved (string).
        pyStyle chooses python list proc (Bool).
        cStyle chooses weave implementation (Bool).
        Number if iterations over smoothing algorithm (int).
        Smoothing factor (float).
    -Returns:
        No returns.
    '''
    from pyDenoise import deNoiseList
    from cDenoise import imgCprocess

    if pyStyle:
        Vprint("Denoising B&W image using pure Python...")
        img = list(imgData.getdata())
        n, m = imgData.size
        procIm = deNoiseList(img, n, m, k, i)
        Vprint("Done!")
        done = Image.new("L", (n, m))
        done.putdata(procIm)
        done.save(outFname)
        Vprint("File saved: %s"%outFname)

    if cStyle:
        Vprint("Denoising B&W image using Python + Weave.")
        arr =imgCprocess(imgData, i, k) 
        Vprint("Done!")
        Image.fromarray(arr).save(outFname)
        Vprint("File saved: %s"%outFname)




def modRGBband(imgDat, RGB):
    '''
    Lets user modify the individual bands
    R,G,B
    '''
    from colorIm import rgb2hsi, hsi2rgb

    rgb =  [0]*3
    for i, elem in enumerate(RGB):
        rgb[i] = elem%256
    RGBproc = rgb2hsi(data, rgb[0],rgb[1],rgb[2])
    return hsi2rgb(RGBproc)




def modHSIband(imgDat, HSI):
    '''
    Lets user modify the individual bands
    H,S,I
    '''
    from colorIm import rgb2hsi,hsi2rgb

    hsi = [0]*3
    for i, elem in enumerate(HSI):
        hsi[i] = elem%360
    imgDat = rgb2hsi(imgDat)        
    imgDat = hsi2rgb(imgDat, hsi[0],hsi[1],hsi[2])
    return imgDat




def deNoiseClr(imgData, outfile, i,k):
    '''
    Denoise processing on image containing
    RGB-band
    '''
    from colorIm import rgb2hsi, colorImgDnoise, hsi2rgb

    arr = rgb2hsi(imgData)
    if i != 0 and k != 0:
        arr1 = colorImgDnoise(arr, i,k)
    arr2 = hsi2rgb(arr1)
    Image.fromarray(arr2).save(outfile)
    Vprint("File saved: %s"%outfile)




def _test():
    '''
    Runs doctests only on functions specified.
    '''
    import doctest
    doctest.run_docstring_examples(testImgType, globals())



if __name__ == '__main__':
    '''
    Front end.
        Handels interaction from cli.
    '''
    import argparse, sys
    from PIL import Image
    _test()

    parser = argparse.ArgumentParser(description='Image Processor')

    parser.add_argument('infile_name', 
	    				type = str,
	    				help='Name of the file that is to be processed.')
    parser.add_argument('outfile_name', 
		    			type = str,
	    				help='Name of the file that is to be processed.')
    # Non optional arguments:
    parser.add_argument('--denoise',
					    '-d',
    					action='store_true',
					    help='denoise image.') 
    parser.add_argument('--iterations',
					    '-i',
					    type = int,
                        nargs = '?',
                        default = 10,
					    help='n-times smoothing steps. default 10') 
    parser.add_argument('--kappa',
    					'-k',
    					type = float,
                        nargs = '?',
                        default = 0.1,
    					help='Degree of filtering. [0-1], default 0.1 ')
    parser.add_argument('--RGB',
    					type = int,
                        nargs = '+',
                        default = [0,0,0],
    					help='Manually adjust red-, green-, and blue- bands [0-255,0-255,0-255]')
    parser.add_argument('--HSI',
    					type = int,
                        nargs = '+',
                        default = [0,0,0],
    					help='Manually adjust Hue-, saturation-, and intensity- bands [0-360,0-360,0-360]')
    parser.add_argument('--cStyle',
    					'-c', 
                        default=False,
    					action='store_true',
    					help='Noise reduction through C.')
    parser.add_argument('--pyStyle',
    					'-p', 
                        default=False,
    					action='store_true',
    					help='Noise reduction through python.')
    parser.add_argument('--Timeit',
    					'-t', 
                        default=False,
    					action='store_true',
    					help='Time the process.')
    parser.add_argument('--verbose',
    					'-v', 
    					action='store_true',
    					help='Get realtime information about the process.')

    arg = parser.parse_args()
    fnameIN = arg.infile_name
    fnameOUT = arg.outfile_name
    
    if arg.verbose:
        def Vprint(*args):
            '''
            Function works as print() when
            verbose is defined, else none.
            '''
            for arg in args:
                print arg,
            print
    else:
        Vprint = lambda *a: None
    
	
    #---- Start Timing.
    if arg.Timeit:
        import cProfile,StringIO, pstats
        pr = cProfile.Profile()
        Vprint('Timing of process starts...')
        pr.enable()

    #---- Process: ------------------------------#
    data, BW = testImgType(fnameIN)

    #---- Init test of arguments         
    if len(arg.RGB) > 3 or len(arg.HSI) > 3:
        print'Too many arguments to do color manipulation'
        sys.exit(0)

    #---- Modify RGB Band.
    if sum(arg.RGB) != 0 and not BW:
        data = modRGBband(data, arg.RGB)
        if sum(arg.HSI) == 0 and not arg.denoise:
            Image.fromarray(data).save(fnameOUT)
            Vprint("File saved: %s"%fnameOUT)

    #---- Modify HSI Band.
    if sum(arg.HSI) != 0 and not BW:
        data = modHSIband(data, arg.HSI)
        if not arg.denoise:
            Image.fromarray(data).save(fnameOUT)
            Vprint("File saved: %s"%fnameOUT)

    #---- Denoise image.
    if arg.denoise:
        if BW:
            Vprint("Found Black and white image.")
            Vprint('Denoise image chosen')
            deNoiseBW(data, fnameOUT, arg.pyStyle, arg.cStyle, arg.iterations, arg.kappa)
        elif not BW:
            Vprint("Found color image.")
            Vprint('Denoise image chosen')
            deNoiseClr(data, fnameOUT, arg.iterations, arg.kappa)

    if not arg.denoise and (sum(arg.HSI) - sum(arg.RGB)) == 0:
        print "Nothing to do!"

    #---- End processing.-------------------------#
    if arg.Timeit:        
        pr.disable()
        Vprint('Timing done!\nResult:')
        s=StringIO.StringIO()
        sortby='cumulative'
        ps=pstats.Stats(pr, stream=s).sort_stats(sortby)
        ps.print_stats(10)
        print s.getvalue() 
    #---- End Timing.

