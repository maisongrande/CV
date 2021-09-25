#!/bin/bash
#
#	Inf3331 Oblig 1. Del 1. Oppgave 1.3
#
# This script deletes all files in the file tree that are larger than a user
# specified size.

ROOT_UID=0          # User ID for root on *UX computers. (find your id by: >>id 'username' and change it accordingly if on uio machine) 
ERR_NOTROOT=87      # Return number for non root invoking.
ERROR=1             # Return number if something went wrong.
ALLGOOD=0           # Return number if all went well.
ROOTDIR=$1          # Directory from where the search start.
FILESIZE=$2         # User want files larger than this removed.
FILECOUNT=0         # How many files where found?
FNAME=$0            # Script name.
NEED_ARGS=2         # Number of needed arguments to execute script.
FOUND=0             # Number of files found.
PERMISSION=644      # Permission criteria change accordingly.

# We are potentially removing a great amount of files, might be a good idea to run as root
if [ "$UID" -ne "$ROOT_UID" ]
# If user ID is not equal root ID put on the brakes!
then
	echo 'Must be root to run this script.'
	exit $ERR_NOTROOT
fi

# Script takes two arguments, one for file tree base, one for file size.
if [ $# -ne $NEED_ARGS ]
# Test wether command-line argument is present (non-empty).
then
echo "INFO: ${FNAME##/} Delete recursively all files from base dir that are larger than chosen size (in kB)."   # Print info
echo "USAGE: ${FNAME} [Base Dir] <File Size>"		                                                            # Print usage.		
exit $ERROR											                                                            # then exit.
fi	

## SEARCH FILE TREE FOR FILES LARGER THAN $2, COUNT THEM. 
# Find 'type' files from specified directory 'ROOTDIR + one 'mindepth'. See if they are grater than '+' size in Kb 'k'
# Do they match your file-permissions "rwx" '-perm'? All unwanted (error) '2' messages redirected to garbage can '/dev/null'
FOUND=$(find "$ROOTDIR" -mindepth 1 -type f -size +"$FILESIZE"k \( -perm -"$PERMISSION" \) -print 2>/dev/null)
FILECOUNT=$(echo -n "$FOUND" | wc -l) # echo result into word counter, which returns number of read lines, now stored in FILECOUNT
#                 ^
#		do not output trailing newline			

if [ $FILECOUNT -eq 0 ]
# Did we find any at all?
then
	echo "No files of size "$FILESIZE"kb or larger found."
	exit $ERROR
fi

((FILECOUNT++)) # Must be a better way....
## DISPLAY FOUND FILES.
echo "$FILECOUNT files found:"

## DISPLAY FILES.
echo "$FOUND"
echo

## CHECK IF USER WANTS TO DELETE ALL OF THE LISTED FILES.
while true; do
	read -p "Remove all files?  no/Yes: "  yn
	case $yn in
			y|Y|Yes|yes ) break;;				
			n|N|No|no ) exit $ALLGOOD			
	esac
done

## REMOVE FILES !!!!! WARNING un-comment next line and files will be removed!!!!!!!!
#rm $FOUND
echo "I've commented out the deletion of files at the end of the script, uncomment to delete"
echo "All $FILECOUNT files successfully removed."
##Exit
exit $ALLGOOD
