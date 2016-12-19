CC=g++
CFLAGS=-O2 -std=c++11 -Wall -pedantic

all: dserver

dserver: dserver.o options.o address_pool.o
	$(CC) dserver.o options.o address_pool.o -o dserver

dserver.o: dserver.cc
	$(CC) $(CFLAGS) -c dserver.cc

options.o: options.cc
	$(CC) $(CFLAGS) -c options.cc

address_pool.o: address_pool.cc
	$(CC) $(CFLAGS) -c address_pool.cc

clean:
	rm -f *.o

cleanall:
	rm -f *.o dserver

pack:
	tar -cvf xcypri01.tar *.cc *.hh Makefile README manual.pdf
