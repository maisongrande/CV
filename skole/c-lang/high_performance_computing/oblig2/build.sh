#!/bin/bash
NUMPROC=4 # ANTALL PROSESSORER
EXECNAME=oblig2

echo 'Building Oblig 2 inf3380'
mpicc -W{all,extra} -std=c99 -fopenmp -O2\
	oblig2.c \
	matrix.c \
	-DDEBUG\
	-o \
	$EXECNAME

if [ $? -eq 0 ] 
then
	echo "Build succeeded"
	echo -n "Run build? [y/n]"	
	read yno
	case $yno in
		[yY] | [yY][Ee][Ss] )
			clear;
			mpirun -np $NUMPROC $EXECNAME;
			;;
		[nN] | [n|N][O|o] )
			echo "Aborted";
			exit 1
			;;
		*) echo "Invalid input"
			;;
	esac
fi

