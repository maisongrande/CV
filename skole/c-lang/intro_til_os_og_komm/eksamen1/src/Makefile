# My general Makefile modified for
# 	home exam in inf 1060
#
#	GLOBAL VARIABLES:
# Compiler flags: (gcc for c)
# Compiler name.
# Wall: almost all warnings applied.
# -g flag for gdb-debugging.
# -o put output of compilation into file 'name'.
# -c compile or assemble but do not link.
# -v print out steps that are being done and version of gcc.
# -Werror Make all Warnings error, might as well learn the hard way.

#DEBUG ?=?
#ifeq (DEBUG,1)
CFLAGS= -Wall -g -DNDEBUG
#else 
#CFLAGS=-Wall
#endif

CC = gcc
OUTPUT = -o
DBG = - DDEBUG -g
NODBG = -DNDEBUG
NOLINKAGE = -c
PRINTCOM = -v
# White spaces immediately after '=' and surrounding variable name are ignored. 

# If header files exists elsewhere, specify:
INCLUDES = -I$(PWD)/header
INCLUDES += -I$(PWD)/work/headers
# Move *.o files to seperate dir:
ODIR = ofiles

# include other library files existing elsewhere:
#LFLAGS = :

# Source files to be linked:
SRC = shproc.c unixcmd.c  ifish.c safefork.c
SRC += ./work/historyhandler.c ./work/historyslave.c ./work/linkhist.c 

# Object files to be compiled:
OBJ = shproc.o unixcmd.o ifish.o safefork.o
OBJ += historyhandler.o historyslave.o linkhist.o

#Name of executable when make is done:
PROG = ifish

# PHONY: To avoid conflicts between name of make recipe and file names. 
# Let's say there is a file in dir named clean. When 'make clean' is executed
# it would not do clean up, but rather see if there are changes to the file clean.
# make output: "clean is up to date".
# Avoid this by declaring clean as phony target.
.PHONY: clean

# Following sequence happens when all is appended to make. Dependencies follows commands.
all: $(PROG)
	@echo program has been linked and is ready for execution.
	@echo name: "$(PROG)"

# dependency obj-files.
$(PROG): $(OBJ) 	
	$(CC) $(CFLAGS) $(OBJ) $(OUTPUT) $(PROG) 

# dependency src-files.
$(OBJ): $(SRC) 
	$(CC) $(NOLINKAGE) $(SRC) $(INCLUDES)
	@echo compiling done. now linking:

# Removes *.o plus executable:
clean:
	$(RM) *.o *~ $(PROG)
	@echo Cleaned up all *.o and executable file
