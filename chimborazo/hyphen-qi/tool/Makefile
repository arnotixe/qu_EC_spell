# File:    Makefile
# Purpose: automatic making of hyphenation tools
#
# Author:  Piet Tutelaers (rcpt@urc.tue.nl)
# Version: 1.0 (May 1993)

CFLAGS = -g

PROGRAMS = c2ascii diacrit hyphenate wordmatch wordmerge wordsort \
           wordints worddiff

all: $(PROGRAMS)

c2ascii: c2ascii.o
	cc -o c2ascii c2ascii.o

diacrit: diacrit.o
	cc -o diacrit diacrit.o

hyphenate: hyphenate.o
	cc -o hyphenate hyphenate.o

wordmatch: wordmatch.o
	cc -o wordmatch wordmatch.o -lc

wordmerge: wordmerge.o string.o
	cc -o wordmerge wordmerge.o string.o

wordsort: wordsort.o qsort.o string.o
	cc -o wordsort wordsort.o qsort.o string.o

wordints: wordints.o string.o
	cc -o wordints wordints.o string.o

worddiff: worddiff.o string.o
	cc -o worddiff worddiff.o string.o

clean:
	rm -rf  $(PROGRAMS) *.o

qsort.c: char.h
string.c: char.h
