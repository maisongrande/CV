CFLAGS = -g  -Wall -O2 -std=c99

OS_TYPE := $(shell uname -s)

LINKLIB =

ifeq ($(OS_TYPE), Linux)
	LINKLIB += -lm
endif

CC = gcc
OUTPUT = -o
NOLINKAGE = -c

SRC = project_euler.c
SRC += challenges.c eulerTools.c

OBJ = project_euler.o
OBJ += challenges.o eulerTools.o

PROG = euler

.PHONY: clean

all: $(PROG)

$(PROG): $(OBJ)
	$(CC) $(OBJ) $(OUTPUT) $(PROG) $(LINKLIB) 

$(OBJ): $(SRC)
	$(CC) $(CFLAGS) $(NOLINKAGE) $(SRC)

clean:
	$(RM) *.o *.s
