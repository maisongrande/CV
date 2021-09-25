#!/usr/bin/env python2
import sys, subprocess, os, re

def compile(texFile, searchingFor=':\d+\:.*$'):
	'''
	Function to compile a Latex document. If there are errors
	during compilation these will be displayed in terminal along with
	last two lines from log.
	'''
	Vprint("Compiling %s ..." %texFile)
	if not os.path.isfile(texFile):
		print"{} is not a file".format(texFile)
		sys.exit(0)
	interAction='nonstopmode'
	action = 'pdflatex -file-line-error -interaction={0} {1}'.format(interAction, texFile)
	Vprint("Running subprocess:\n%s:\n" % action)
	try:
		pdfProc = subprocess.Popen(action, shell=True, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
	except ValueError:
		print"{0} is not a valid argument.".format(interAction)
		sys.exit(0)
	stdoutdata, stderrdata = pdfProc.communicate()
	errorLog = stdoutdata
	errorMsg = stderrdata
	if errorMsg:
		print'Something went wrong..'
		print errorMsg
	try:
		foundErr = re.findall(re.compile(r'{0}+{1}'.format(texFile, searchingFor), re.MULTILINE), errorLog)
	except:
		print("Unable to compile search pattern")
		sys.exit(0)
	if foundErr:
		Vprint("Found errors:")
	else:
		Vprint("Done")
	for errors in foundErr:
		print errors
	logFile = texFile.split('.')
	if os.name is 'nt':	# linux/mac os = 'posix', win = 'nt'
		with open(logFile[0]+'.log', 'r') as lf:
			logLines = lf.read().split('\n')
		lastLines = []; j = 0; i = 1
		while j < 2 and i < len(logLines):
			x = len(logLines)-i
			if logLines[x] != '':
				lastLines.insert(0, logLines[x])
				j += 1 
			i+=1
		for lines in lastLines:
			print lines
	else:
		try:
			twoLast = subprocess.Popen('cat {0}.log | grep "." | tail -2'.format(logFile[0]), shell=True, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
		except ValueError:
			print"Non valid argument given."
			sys.exit(0)
		stdout, stderr = twoLast.communicate()
		errorMsg = stderr
		print stdout
		if errorMsg:
			print'Something went wrong..'
			print errorMsg



if __name__ == '__main__':
	import argparse
	parser = argparse.ArgumentParser(	
	description='Pdf-compiler for *.tex files.',
	epilog = '''Compile.py compiles Latex documents. If there are errors
	during compilation these will be displayed in terminal along with
	last two lines from log.''')

	# Non optional arguments:
	parser.add_argument('filename', 
						type = str,
						help='Name of the *.tex file that is to be compiled.')
	# Optional arguments:
	parser.add_argument('--opt_search',
						'-s',
						type = str,
						default=':\d+\:.*$',
						help='Use own regex-pattern to search for information')
	parser.add_argument('--verbose',
						'-v', 
						action='store_true',
						help='Get realtime information about the process.')

	args = parser.parse_args()
	verbose = args.verbose

	if verbose:
		def Vprint(*args):
			for arg in args:
				print arg,
			print
	else:
		Vprint = lambda *a: None
	
	compile(args.filename, args.opt_search)


