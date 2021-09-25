#!/bin/bash
#
#	Inf3331 Oblig 1. Del 1. Oppgave 1.2
#	Author: Christopher N. Storheim	
#
# This script searches within the specified file tree for a specific word
# both which are supplied by the user.

ERROR=1				# Return number if something went wrong.
ALLGOOD=0			# Return number if all went well.
BASEDIR=$1			# Base dir from where to search from.
KEYWORD=$2			# Word to search for.
FNAME=$0			# Script name.
NEED_ARGS=2			# Number of needed arguments to execute script.
FOUNDNUM=0			# Number of found instances of word.
FOUND=0				# Which file are they found in.

# Check that needed info is supplied:
if [ $# -ne $NEED_ARGS ]
    # Test wether command-line argument is present (non-empty).
then
    echo "INFO: ${FNAME##/} Find all files containg a specific word " 	# Print info
    echo "USAGE: ${FNAME} [Base Dir] [Key Word]"                        # Print usage.		
    exit $ERROR	                                                        # then exit.
fi	

# SEARCH and STORE: Run command 'find' from basedir. Look for -type files
# open grep with keys: 
# I - Which file did we find a match in
# i - Dont be case sensitive (grep shouldn't be by default)
# n - prefix every match with corresponding line number
FOUND=$(find $BASEDIR -type f -exec grep -Iin $KEYWORD {} +) # Store result in FOUND
FOUNDNUM=$(echo -n "$FOUND" | wc -l)	# echo result into wc to count num lines with hits
# wc counts lines (match on new line char) and returns num hits and stores in FOUNDNUM

# we now have result on the form:
# ./dir/filename:linenumber:textline'matching word' 


# If more than zero files where found, display files.
if [ $FOUNDNUM -gt '0' ]
then
    # Display number of matches to the user:
    echo "Found "$FOUNDNUM" files containing "$KEYWORD": "
    echo
    echo "$FOUND"
else
    echo "No files containing "$KEYWORD" found."
fi

##Exit
exit $ALLGOOD
