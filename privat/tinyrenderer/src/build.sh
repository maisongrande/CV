#!/bin/bash
if [ "$1" == "-asm" ]
then
	gcc -W{all,error,extra,pedantic,strict-aliasing=2}\
		-Wno-unused-variable\
		-Wno-unused-parameter\
		-fstrict-aliasing\
		-fopenmp\
		-std=c99\
		../src/{tiny_renderer,TGAimage,point,model,geometry,matrix,linalg}.c\
		-I../includes/\
		-lm\
		-march=native\
		-msse2\
		-O2\
		-S;

	mv *.s ../asm/;
fi

gcc -W{all,error,extra,pedantic,strict-aliasing=2}\
	-fstrict-aliasing\
	-Wno-unused-variable\
	-Wno-unused-parameter\
	-fopenmp\
	-std=c99\
	../src/{tiny_renderer,TGAimage,point,model,geometry,matrix,linalg}.c\
	-I../includes/\
	-lm\
	-O2\
	-march=native\
	-msse2\
	-o\
	../build/renderer

