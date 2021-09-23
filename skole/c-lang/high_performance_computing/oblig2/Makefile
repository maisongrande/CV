CC=mpicc
CFLAGS=-fopenmp
CWARN=-Wall -Wextra 
CUNWARN=-Wno-unused-parameter -Wno-unused-variable
COPT=-O2
CSTD=-std=c99

# SLAAR AV/PA TIDTAKING CMD: make PROFILE=NO/YES
PROFILE ?= "NO"
ifeq ($(PROFILE), NO)
	CFLAGS+=-DNPROFILE
else
       	CFLAGS+=-DPROFILE 
endif

SRC=oblig2.c
SRC+=matrix.c

OBJ=oblig2.o 
OBJ+=matrix.o

PROG=oblig2

UTFIL=resultMatrix.bin

.PHONY: clean

$(PROG): $(OBJ)
	$(CC) $(OBJ) $(CFLAGS) -o $(PROG)

$(OBJ): $(SRC)
	$(CC) $(CSTD) $(CWARN) $(CUNWARN) $(CFLAGS) $(COPT) $(SRC) -c
	
#==CLEAN: ====================================================================
# Removes *.o plus executable:
clean:
	$(RM) $(UTFIL) *.o *~ $(PROG)
