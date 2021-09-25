#!/usr/bin/env python2
import re, os, shutil, sys, shlex, subprocess, fileinput
from StringIO import StringIO
#---------------------
# Global variables:
#---------------------
finale_name = ''
keyTokens = [
re.compile(r'\%\@\exec\s+(\w+\s+\w+\.{1}\w+\s+\S+)'),\
re.compile(r'\%\@\import\s+(\w+\.{1}\w+)\s+(.*)'),\
re.compile(r'\\include{((.*?))}'),\
re.compile(r'%@(python|bash)\s+(\w+\.{1}(?:sh|py))\s+((\w*)\s+(w*(?:.*?)))%@', re.DOTALL)\
]

texTokens = [
r'\\begin{verbatim}\n',\
r'\\end{verbatim}',\
r'\\begin{shadedquoteBlueBar}\n\\fontsize{9pt}{9pt}\n\\begin{Verbatim}\n',\
r'\\end{Verbatim}\n\\end{shadedquoteBlueBar}\n\\noindent',\
r'\\begin{Verbatim}[numbers=none,frame=lines,label=\\fbox{{\\tiny Terminal}},fontsize=\\fontsize{9pt}{9pt},\nlabelposition=topline,framesep=2.5mm,framerule=0.7pt]\n',\
r'\\end{Verbatim}\n\\noindent'\
]

texFormat = [r'''\usepackage{fancyvrb}
\usepackage{framed}
\usepackage{color}
\providecommand{\shadedwbar}{}
\definecolor{shadecolor}{rgb}{0.87843, 0.95686, 1.0}
\renewenvironment{shadedwbar}{
\def\FrameCommand{\color[rgb]{0.7,     0.95686, 1}\vrule width 1mm\normalcolor\colorbox{shadecolor}}\FrameRule0.6pt
\MakeFramed {\advance\hsize-2mm\FrameRestore}\vskip3mm}{\vskip0mm\endMakeFramed}
\providecommand{\shadedquoteBlueBar}{}
\renewenvironment{shadedquoteBlueBar}[1][]{
\bgroup\rmfamily
\fboxsep=0mm\relax
\begin{shadedwbar}
\list{}{\parsep=-2mm\parskip=0mm\topsep=0pt\leftmargin=2mm
\rightmargin=2\leftmargin\leftmargin=4pt\relax}
\item\relax}
{\endlist\end{shadedwbar}\egroup}
''']



def codeFormat(final):
	'''
	In a nut shell this function reads input file via the 
	fileinput module. It searches for keyword, and if found 
	raises True-flag. So on the next iteration it sees that
	keyword has been found and print specified text at that
	position. Resets flag to False to continue search. 
	
	If no other or non is found it prints every line back to file until
	no more lines are to be found.
	
	Params:
		- Name of the processed file.
	Returns: 
		- Nothing.
	'''
	proc_docClass = False
	
	for line in fileinput.input(final, inplace=1):
		if line.startswith(r'\documentclass{'):
			proc_docClass = True
		else:
			if proc_docClass:
				print(texFormat[0])
			proc_docClass = False
		print line,
	fileinput.close()



def readDocument(texFile):
	'''
	Opens extern *.tex file.
	Checks if file exists, if not ends script.
	Makes a copy of the file with extension '.origin'.
	Reads file into memory and returns this new file/string to caller.
	
	Params:
		- File to process.
	Returns:
		- Same file loaded into memory.
	'''
	if not os.path.isfile(texFile): 							#Open Supplied texfile and Check if file exists:
		print("Could not find %s in specified directory" % texFile)
		sys.exit(0)
	shutil.copy2(texFile, texFile+'.origin')					# Make backup file:
	with open(texFile) as infile:								# Now open file.
		memDoc = infile.read()									# Load file into memory.
	Vprint("File %s read.\nNow processing ..." % texFile)
	return memDoc



def writeDocument(oldTexFile, newTexDoc, fname, extension='.tex'):
	'''
	Writes processed file to current directory.
	Replaces original file name with new file-type name by defult: "name.tex"
	Possible to supply own name.
	Will not over write last file, but extend name with processnumber.
	Params:
		- The original name of the tex file.
		- The processed file.
		- A user specified name for processed file (optional)
		- Processed file type (optional)
	Returns:
		- Name of new file.
	'''
	Vprint('Trying to write processed file')
	i = 1
	if fname:
		newFileName = fname+extension
	else:
		newFileName = oldTexFile.replace('.xtex', extension)	# Replace file-type name for new file.
	while os.path.isfile(newFileName):
		nfm = newFileName.split('.')
		newFileName = nfm[0]+extension+str(i)
		i += 1 	

	with open(newFileName, "w") as newOutFile:			# Open this file.	
		newOutFile.write(newTexDoc)						# Write new content to file.
	Vprint("Processed file written.\nName: %s"% newFileName)
	return newFileName



def include(tex_include):
	'''
	Searches for keyword: \include{path/to/file}.
	Path/to/file is a path to a file which content is to be pasted into the position
	where key word is found.
	'''
	inc = keyTokens[2].search(tex_include) 
	if inc: 
		Vprint("Found include statement. Trying to process file %s .." % inc.group(1))
		if os.path.isfile(os.path.normpath(inc.group(1))):
			with open(inc.group(1), 'r') as incF:
				inclFile = incF.read()
			newCont = keyTokens[2].sub(inclFile, tex_include, count=1)
			tex_include = newCont
			Vprint("Text included from files.")
			tex_include = include(tex_include)
		else:
			print"Can't find file {}.\n".format(num[i][0])
	return tex_include



def importCode(tex_import, fancyVerbatim):
	'''
	Searches for key-word %@index in file. 
	If found, it is replaced by pre-determined rules.
	Params: 
		- Latex file to read from (name.tex).
	Returns:
		- Processed file.
		- False if problems with files.
	Example result with 2 hits:
	[('script_example.py', '" *out =(.|\\n)*?return out"'), ('example.py', '" *out =(.|\\n)*?return out"')]
	Global varibles: texTokens, keyTokens
	'''
	imp = re.findall(keyTokens[1], tex_import) 					# Initial search for hit and amount of hits on %@import.
	Vprint('Found @import: %s'% imp[:])
	if keyTokens[1].search(tex_import): 						# Do we have a match on %@import?
		for i in range(len(imp)):								# If so, use number of hits on %@import to go through every instance.
			if os.path.isfile(imp[i][0]):						# Check if file is exists in patch
				try:
					with open(imp[i][0]) as searchF:			# Open file found in extention of %@import.
						importfile = searchF.read()				# Read whole file into mem.
				except IOError:
					Print(r"%%@import: Found but failed to open file: %s" % imp[i][0]) 
					break 
				try:											# Check to see if supplied regex is up to par.
					compiled = re.compile((imp[i][1].strip('"')), re.DOTALL) # Compile found regex pattern.
				except:											# Nothing to brag home about?
					print("%%@import: Pattern: %s doesn't match any known Regex patterns" % imp[i][1])
					sys.exit(0)									# Exit with message.
				if compiled.search(importfile):					# If we have a match in this file.
					findPat = re.search(compiled, importfile)	# Apply where it was found to variable.
				else:											# If nothing was found let user know.
					print("%%@import: No match for regex: %s in file: %s" % (imp[i][1], imp[i][0]))
				if fancyVerbatim:	
					Vprint("Processed OK.\nAdding fancy verbatim\nAdded to document.")
					newTexDoc = keyTokens[1].sub(texTokens[2]+findPat.group()+'\n'+texTokens[3], tex_import, count=1) # Add tokens and substitute.
					tex_import = newTexDoc 						# Load the new doc file into original in case more searches to be done.
				else:
					Vprint("Processed OK.\nAdded to document.")
					newTexDoc = keyTokens[1].sub(texTokens[0]+findPat.group()+'\n'+texTokens[1], tex_import, count=1) # Add tokens and substitute.
					tex_import = newTexDoc 						# Load the new doc file into original in case more searches to be done.
			else:												# File-name found in tex doc misspelled or non existing.
				print("%%@import: File: %s specified in document does not exist" % imp[i][0]) 
				return tex_import #!!!!!!!!!!!
		return newTexDoc
	else:
		Vprint("%%@import: Searched, but found no instance of keyword %%@import")
		return tex_import



def execCode(tex_exec, fancyVerbatim):
	'''
	Searches for key-word %@exec in file. 
	If found, name of file is executed and result written to texfile.
	Params: 
		- Latex file to read from (name.tex).
			Does not matter if under processing or first instance.
	Returns:
		- Processed file.
		- False to caller if something bad happens
	Global varibles: texTokens, keyTokens
	'''
	exe = re.findall(keyTokens[0], tex_exec)	# Initial search for hit and amount of hits on %@exec.
	Vprint("Found %%@exec: %s"%exe)
	if keyTokens[0].search(tex_exec):	# Do we have a match on %@exec?
		for i in range(len(exe)):	# If so, use number of hits on %@exec to go through every instance.
			args = shlex.split(exe[i])	# Use module shlex to split string into args.
			if not os.path.isfile(args[1]):	# Sjekker om filen fra documentet eksisterer
				print("%%@exec: Could not find %s in specified directory" % args[1])
				return tex_exec
			try:	# Try to execute the whole string, catch messages from std-buffers
				sproc = subprocess.Popen(args, stdout=subprocess.PIPE, stderr = subprocess.PIPE)	
			except OSError:# Recomended to at least catch OSError.
					print("%%@exec: Unable to open %s with %s." % (args[1], args[0]))
					return False	# Return -1 back to caller
			if sproc.stderr.read(): # If there are messages in stdbuffer:
				print("%%@exec: Error while executing program %s!" % args[1])
			else:					# No stderr messages to be found:
				output = sproc.stdout.read()	# Catch output from program and insert it in document.
				if fancyVerbatim:
					Vprint("Processed OK.\nAdding fancy verbatim\nAdded to document.")
					newDoc = keyTokens[0].sub(texTokens[2]+'$ '+exe[i]+'\n'+output+texTokens[3], tex_exec, count=1) # Replace match only once pr iteration
					tex_exec = newDoc	# If there are more iterations to be done, it's a good idea to replace string to search
				else:
					Vprint("Processed OK.\nAdded to document.")
					newDoc = keyTokens[0].sub(texTokens[0]+'$ '+exe[i]+'\n'+output+texTokens[1], tex_exec, count=1) # Replace match only once pr iteration
					tex_exec = newDoc	# If there are more iterations to be done, it's a good idea to replace string to search

			if sproc.poll() != 0:	# Executed program did not return OK?
				print(sproc.stderr.read())	# Give user feedback on what happens, stderr knows what happened.
		return tex_exec	# Return changes to document back to caller.
	else:
		print("No %%@exec found in document")			
		return tex_exec



def execPySh(tex_pysh, fancyVerbatim):
	'''
	Searches for key-word %@python <code> @ or %@bash <code>  @ in file. 
	If found, content between @'s is executed and result written to texfile.
	Params: 
		- Latex file to read from (name.tex).
	Returns:
		- Processed file.
		- False if something goes wrong along the way.
	Global varibles: texTokens, keyTokens
	# Bash list: ['bash','name of script', 'Rubbish', 'argument', 'what to execute']
	# Maybe not a good pattern initially and definately not pretty. but does the job... 
	'''
	#Open Supplied texfile and Check if file exists:
	execRes = []
	num = re.findall(keyTokens[3], tex_pysh) # Initial search for hit and amount of hits on %@py|sh.
	pysh = keyTokens[3].search(tex_pysh) 
	if pysh: # Do we have a match on %@py|sh?
		for i in range(len(num)):	# If so, use number of hits on %@py|sh to go through every instance.
			if num[i][0] == 'bash': 
				Vprint("Found keyword %s. Executing code: %s" % (num[i][0],num[i][4])) #VERBOSE
				bashcmd = num[i][4].strip('\n') 
				try:
					bash = subprocess.Popen(bashcmd, shell=True, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
				except OSError:
					print("%%@bash: Unable to execute %s in document" % bashcmd)
					print(bash.stderr.read())
					return False
				execRes.append(bash.stdout.read().strip('\n')) # Appending result from exec to list
				Vprint("Got result: %s" % execRes[i]) # VERBOSE
			elif num[i][0] == 'python':
				Vprint("Found keyword %s. Executing code:\n %s" % (num[i][0], pysh.group(3)))
				sout = StringIO() # creating filebuffer 
				sys.stdout = sout # to hold stdout messages.
				try:
					exec(pysh.group(3)) # Executing script.
				except:
					print("%%@python: Unable to process string %s in document.", pysh.group(3))
					return False
				sys.stdout = sys.__stdout__	# Give stdout back to OS.
				result = sout.getvalue() # Get what was loaded into buffer.
				sout.close() # Free mem
				execRes.append(result.strip('\n'))	# Append to exec list.	
				Vprint("Got result %s"%result.strip('\n'))
			if fancyVerbatim:
				newDoc = keyTokens[3].sub(texTokens[4]+'$ '+num[i][0]+' '+num[i][1]+' '+num[i][3]+'\n'+execRes[i]+'\n'+texTokens[5], tex_pysh, count=1)
			else:
				newDoc = keyTokens[3].sub('\\$ '+num[i][0]+' '+num[i][1]+' '+num[i][3]+'\n'+execRes[i], tex_pysh, count=1)
			tex_pysh = newDoc
		return tex_pysh
	else:
		print("No %%@python ... @ or %%@bash .... @ found in document")			
		return tex_pysh
'''
newDoc = keyTokens[3].sub('\\$ '+num[i][0]+' '+num[i][1]+' '+num[i][3]+'\n'+execRes[i], tex_pysh, count=1)
'''

def _test():
	import doctest
	doctest.testmod()



if __name__ == '__main__':
	import argparse
	parser = argparse.ArgumentParser(
	description='Preprocesses *.tex files.',
	epilog = '''Prepro.py searches *tex-files for keyword "%@" through out the file.If found, the word appended to "%@" is evaluated against predefined rules.\n
	Prepro.py then tries to execute programs and or text and writes result to a new
	and edited file. This file has the default type *.3331tex, but can be modified.
	Original file is backed up before excution of script with *.origin as extension.''')

	# Non optional arguments:
	parser.add_argument('filename', 
						type = str,
						help='Name of the *.tex file that is to be processed.')
	# Optional arguments:
	parser.add_argument('--name',
						'-n',
						type = str,
						help='Name of the outputfile.')
	parser.add_argument('--filetype',
						'-t',
						type = str,
						default='.tex',
						help='Type of the outputfile.')
	parser.add_argument('--fancy',
						'-f',
						action='store_true',
						default=False,
						help='Changes the default verbatim to fancy verbatim.')
	parser.add_argument('--verbose',
						'-v', 
						action='store_true', 
						default=False,
						help='Get realtime information about the process.')

	args = parser.parse_args()
	verbose = args.verbose
	name = args.name

	if args.filetype:
		filetype = args.filetype
	else:
		filtype = args.filename


	if verbose:

		def Vprint(*args):
			'''
			This function uses the wild card argument to print out
			whatever is sent into it when called.
			example:
			--------
			>>> Vprint('Alot of verboseness 1234...')
			Alot of verboseness 1234...
			'''
			for arg in args:
				print arg,
			print

	else:
		Vprint = lambda *a: None

	# Begin:

	unProcFile = readDocument(args.filename)
	inc_proc = include(unProcFile)
	imp_proc = importCode(inc_proc, args.fancy)
	exec_proc = execCode(imp_proc, args.fancy) 
	pb_proc = execPySh(exec_proc, args.fancy)	
	allDone = writeDocument(args.filename, pb_proc, name, filetype)
	finale_name = allDone
	if args.fancy:
		codeFormat(finale_name)

# --Test functions:


def test_Vprint(capsys):
	'''
	Function to test the output from
	Vprint function
	'''
	Vprint('Hello World!')
	expect1 = capsys.readouterr()
	expect2 = () 
	expect2 = ("hello World!\n", '')
	message = 'Hello world test failed'
	assert expect1 == expect2, message



def test_readDoc():
	'''
	Function to test the readDocument function.
	Dependent on making a file with some content.
	'''
	fileToRead = 'testRD.tex'
	expect = "Testing a test function\n"
	assert expect == readDocument(fileToRead)

