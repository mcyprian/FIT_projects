CC=g++
CFLAGS=-O2 -std=c++11 -g -Wall -pedantic
LDFLAGS=-lsimlib -lm

all: model

model: model.o
	$(CC) model.o -o model $(LDFLAGS) 

model.o: model.cc
	$(CC) $(CFLAGS) -c model.cc

clean:
	rm -f *.o

cleanall:
	rm -f *.o model

pack:
	tar -czvf 01_xcypri01_xbaric01.tar.gz *.cc *.hh Makefile technicka_zprava.pdf
