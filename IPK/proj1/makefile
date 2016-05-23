CC=g++
CFLAGS=-O2 -std=c++11 -Wall -pedantic
LDFLAGS=-Wl,-rpath=/usr/local/lib/gcc49/

all: webclient

webclient: webclient.o
	$(CC) webclient.o -o webclient $(LDFLAGS)

webclient.o: webclient.cc
	$(CC) $(CFLAGS) -c webclient.cc

clean:
	rm -f *.o

cleanall:
	rm -f *.o webclient

pack:
	tar -cvzf xcypri01.tar.gz makefile webclient.cc
