#!/bin/bash
#
#	Inf3331 Oblig 1. Del 1. Oppgave 1.4
#		- Christopher Storheim -
#
#
# Sort content of a file and create new file with sorted content
#

ERROR=1				# Return number if something went wrong.
ALLGOOD=0			# Return number if all went well.
FNAMEIN=$1			# Root directory from where the search start.
SNAME=$0			# Script name.
FNAMEOUT=$2			# Name of file to write result to.
NEED_ARGS=2			# Number of needed arguments to execute script.
RESULT=0

# Check that needed info is supplied:
if [ $# -ne $NEED_ARGS ]
# Test wether command-line argument is present (non-empty).
then
echo "INFO: ${FNAME##/} Sorts in-file and writes result to out-file " # Print info
echo "USAGE: ${FNAME} [in-file] [out-file]" # Print usage.		
exit $ERROR# then exit.
fi	

sort "$FNAMEIN" -f -o "$FNAMEOUT" # Easy does it.

##Exit
exit $ALLGOOD
