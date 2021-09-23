## Makefile for Obligatorisk innlevering 1 inf2220
## Av: Christopher Storheim.

JFLAGS=-g # Generer debug informasjon.
JCOMP=javac # Java kompilator brukt.

# Override standard make regler for kompilering (*.c *.o)
.SUFFIXES: .java .class

# Regler for hvordan kildefiltype skal konverteres
.java.class:
	$(JCOMP) $(JFLAGS) $*.java


# Klassene som tilhorer obligen.
CLASSES = \
	  Project.java\
	  Task.java\
	  Oblig2.java

.PHONY: clean tarball

# entrepunkt:
default: classes

# konverter java->class filer vha reglene over.
classes: $(CLASSES:.java=.class)

# Lag tarball
tarball:
	@localdir=`pwd`; localdir=$${localdir##*/}; \
		rm --force $$localdir.tar.gz;       \
		tar --exclude=$(TARBALL_EXCLUDE)    \
		--exclude=$(EXECUTABLE)             \
		--create                            \
		--gzip                              \
		--verbose                           \
		--file  $$localdir.tar.gz *

# Rydd opp
clean:
	$(RM) *.class
