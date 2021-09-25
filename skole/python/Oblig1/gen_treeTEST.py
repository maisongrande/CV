#!/usr/bin/env python2
from random import randint as rdint     # Random number generator.
import random
import os                               # Crossplatform OS rutines.
import sys                              # interpreter tools.
import argparse                         # command line argument parser.
import time



legal_chars = "abcdefghijklmnopqrstuvwxyz"+\
        "ABCDEFGHIJKLMNOPQRSTUVWXYZ"+"0123456789_"



parser = argparse.ArgumentParser(description='Generate a random sized file tree based on some user defined specs.')
'''
Opt args:
    default: 
        target: Base directory from where to operate.
        folders: Each parent directory shall have [0, folders] child directoies.
        files:  Each directory shall contain [0, files]. Files speciefied by user.
    
    optional:
        size: Number of characters written to file [1, size * 1024]
        rec_depth:  Decides length of folder tree.
        start: Give file a new access time (atime)
        end: Give file a new modiefied time (mtime)
        seed: deterministic if random.seed(1234).
        verbose: Shout out the steps that are being done.
'''
# Non optional arguments:
parser.add_argument('target', type = str,
                    help='Define base folder from where the file tree will grow.')
parser.add_argument('folders', type = int,
                    help='Define number of sub folders in each directory.')
parser.add_argument('files', type = int,
                    help='Define number of files in each directory and sub directory.')
# Optional arguments:
parser.add_argument('--size', nargs=1, type = int, default=0,
                    help='Defines number of characters in file times 1024')
parser.add_argument('--rec_depth', type = int,  nargs=1, default=3,
                    help='Defines how deep the folders will accumulate')
parser.add_argument('--start', type = int,  nargs=1, default=1,
                    help = 'Lower boundry for timestamp')
parser.add_argument('--end', type = int,  nargs=1, default=1,
                    help='Upper boundry for timestamp')
parser.add_argument('--seed', type = int,  nargs=1, default=1,
                    help='Reproduces same result the next time script is executed')
parser.add_argument('--verbose', action='store_true',
                    help='Makes script very talkative')

args = parser.parse_args()

base = args.target      # target directory to from where tree will grow
numdirs = args.folders  # Number of subdirectories in each folder.
numfiles = args.files   # Number of files in each directory

size = args.size        # Size of the files
if size != 0:
    size = args.size[0] # Have to do it this way 

rec_depth = args.rec_depth # How deep the tree will grow
if rec_depth != 3:
    rec_depth = args.rec_depth[0]

start_t = args.start    # Start time
if start_t != 1:
    start_t = args.start[0] 

end_t = args.end        # End time
if end_t != 1:
    end_t = args.end[0]

seed = args.seed        # Random seed, to make script predictable

verbose = args.verbose  # Make script loud


def random_string(length=6, prefix="", legal_chars=legal_chars):
    """
Create a random string of text.

Parameters
----------
length : int
    Length of the string (minus the prefix part).
prefix : string
    Prefix the string with some text.
legal_chars : string
    A string of charracter that are allowed to be used in the
    output.

Returns
-------
rnd_str : str
    A string of random charracters.
    """
# Function start:
    len_LC = (len(legal_chars) - 1) # find length of legal characters.
    rnd_str = prefix                # prefix string with 'prefix'.
    split_str = []                  # make empty list to hold upcoming string objects.

    for i in range(length):         # itterate through wanted string length
        split_str.append(legal_chars[rdint(0, len_LC)]) # find random char and append to elements in list

    final ='' # Create an empty string for which name will be stored
    final = final.join(split_str) 
    return rnd_str+final  # Join prefix with elements in list to form a string. Return result.



def generate_tree(target, numdirs,  numfiles, rec_d, verbose=False):
    """
Genereate a random folder structure with random names.
Parameters
----------

target : str
    Path to the root where folders are to be created.
dirs : int
    Maximum number of directories to be created per directory.
rec_depth : int
    Maximum directory depth.
verbose : bool
    Be loud about what to do.
numfiles: int
    User specified file size.
    """
    print(rec_d)
    if rec_d == 0:
        return
    origin = os.getcwd() # get path of where we are
    base = os.path.join(origin, target) # Join defined dir with current
    if not os.path.exists(base): # if that directory does not excist
        os.mkdir(base)          # Make it
    os.chdir(base)  # Change into it
   
    # Make a list of dir names for first branch. 
    sub_d = [random_string(rdint(2,22)) for i in range(rec_d)] 

    # Walk up the list while creating dir and populating them
    for elem in sub_d: 
        os.mkdir(elem)
        os.chdir(elem)
        if(verbose):
            print('\nIn directory:\n %s' % (os.getcwd()))
        create_files(numfiles)  # create files in new diectory
            # We are now at the end of the tree, first branch.
    for i in range(numdirs):
        sub = os.path.join(os.getcwd(), random_string(rdint(2,22))) # combine random name to current directory we are in
        os.mkdir(sub) # make the above directory 
        os.chdir(sub) # Change into it
        if(verbose):  # scream out where we are
            print('\nIn directory:\n %s' % (sub))
        create_files(numfiles)  # create the wanted files in new diectory
        os.chdir(os.path.dirname(os.getcwd())) # change to the dir underneath  
        #os.chdir(os.path.dirname(os.getcwd())) # Same when done.
    os.chdir(origin)
    rec_d -= 1
    generate_tree(target, numdirs, numfiles, rec_d, verbose)
        # My plan was to start algorithm again here (recurse), but it ended up with so many files
        # and directories so it was realy realy hard to know if there was any logic to all of this.
'''
This was my idea of a 3 x 3 file tree: That makes a total of 36 sub folders to base, and probably not
what was intended? It would have made a pretty nice file-bush though.


1x3:                2x3: same paatern as 1  3x3: same pattern as 1 and 2
            /lmn
        def/abc
       /     /mop
      /
     /    //sdf
/base/def/rgw       then base/klm/      then /base/sdf
    \    \ /gwg
     \
      \     //sfa
        def/ghi/
            \/sfd
'''
def timestamp(filename, start_t=start_t, end_t=end_t):
    '''
    Put on a modified and access stamp on the file with
    number of seconds since 1970
    '''
    at = access_time(start_t, end_t)  # Collect access time
    mt = modified_time(start_t, end_t)    # Collect modified time
    os.utime(filename, (at,mt)) # Stamp file
    return at, mt       # return time.


def access_time(start, end):
    ''' 
    Create an access time stamp 
    '''
    if start == 1 and end == 1: #init values set as default to 1
        at = time.time() - 60*60*24 * rdint(1, 356) # Set random num days within the last year
    else: 
        elem = rdint(end, start) # Get a random day within interval supplied by user
        at = time.time() - 60*60*24 * elem  # time.time is todays time since 00.00 01.01.1970 in sec
    return at                               # Subtract num days (elem) from today. Return this time.



def modified_time(start, end):
    ''' 
    Create a modified time stamp 
    '''
    if start == 1 and end == 1: # Same algorithm as access_time()
        mt = time.time() - 60*60*24 * rdint(1, 356)
    else:
        elem = rdint(end, start)
        mt = time.time() - 60*60*24 * elem
    return mt



def file_content(filename, file_sz=size):    
    """ 
    Makes a file with random name and size. Size is either prvovided by user
    or randomly generated.
    """
    rand_file = open(filename, "w")     # Open file then check if:
    
    if file_sz == 0:                    # No filesize provided?
        rndtxtsz = rdint(1, 1024)           # Get size between 1-1024 bytes
        txt = random_string(rndtxtsz)       # Get string of rand size
        rand_file.write(txt)            # Write to file
        rand_file.close()             # Close file
        return rndtxtsz
    else:                               # Else:
        txt = random_string(file_sz)    # Get rand str of user def size
        rand_file.write(txt)            # Write it to file
        rand_file.close()               # Close file
        return file_sz



def create_files(n, Verbose=verbose):
    """
    Creates n files in directory specified by the caller.
    """
    for i in range(numfiles):
        name_len = rdint(3, 33)                 # Get apropriate file-name size
        name = random_string(name_len)          # Get a name from random string (prefixed with file for readability)
        size = file_content(name)               # Fill the file with content and collect number of bytes written
        acces, mod = timestamp(name, start_t, end_t)    # Put time stamps on file and collect times
        if verbose == True:
            print('Created file: %s\nsize: %d bytes. Modified-time stamp: %gs access-time stamp: %gs (in sec since 00.00 01.01.1970)' % (name, size, acces, mod))




if __name__ == "__main__":
    '''if statement to ensure that nothing is run unless it is referenced 
       as standalone process from terminal.
    '''
    # Check if seed is applied
    if seed != 1:           # Seed flag set
        seed = args.seed[0]  # Have to do it this way cause of some hick ups in args.parse 
        random.seed(seed)       # Initialize random num gen and apply  wanted argument

    generate_tree(base, numdirs, numfiles, rec_depth, verbose)    
