#!/bin/bash
#
#	Inf3331 Oblig 1. Del 1. Oppgave 1.1
#		- Christopher Storheim -
#
# This script lists all files that has been modified the last n days
# List the result sorted and with file size.
#
ERROR=1				# Return number if something went wrong.
ALLGOOD=0			# Return number if all went well.
ROOTDIR=$1			# Root directory from where the search start.
NUMDAYS=$2			# The days at which the files have been modiefied.
FNAME=$0			# Script name.
NEED_ARGS=2			# Number of needed arguments to execute script.
FOUND=0
SORTED=0

# Check that needed info is supplied:
if [ $# -ne $NEED_ARGS ]
# Test wether command-line argument is present (non-empty).
then
echo "INFO: ${FNAME##/} Lists all files modified the last n-days"	# Print info
echo "USAGE: ${FNAME} [Base Dir] [Num Days]" 						# Print usage.		
exit $ERROR															# then exit.
fi	

# Not a good idea to use ls-command to list the files. This is because files can
# contain arbitrary characters like \n, commas, white-spaces etcetera. Plus
# the fact that ls searches for new-line character and not NULL-character.

FOUND=$(find "$ROOTDIR" -mindepth 1 -type f -mtime -"$NUMDAYS" -exec du -bh {} \; | sort -n)

# Search (find) from "ROOTDIR" +1 (-mindepth 1), after files (-type f) stamped with modified date (-mtime)
# for "NUMDAYS" up to today (-"NUMDAYS"). Start (-exec)  disk usage (du) and check every file,
# then add size in bytes (-b) and transform it into human readable (h). Pipe this into sort (sort)
# which sorts numericly (-n). curly braces {} is a flag read by exec for it to  understand that the results from find are to
# be used. "\;" is set to stop further expansion until find and exec is done.

# Display result:
echo "$FOUND" 


##Exit
exit $ALLGOOD
