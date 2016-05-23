CC=g++
CFLAGS=-O2 -std=c++11 -Wall -pedantic
LDFLAGS=-lpthread -Wl,-rpath=/usr/local/lib/gcc49/

all: client server

client: client.o cypro.o
	$(CC) client.o cypro.o -o client $(LDFLAGS)

client.o: client.cc
	$(CC) $(CFLAGS) -c client.cc

server: server.o cypro.o
	$(CC) server.o cypro.o -o server $(LDFLAGS)

server.o: server.cc
	$(CC) $(CFLAGS) -c server.cc

cypro.o: cypro.cc
	$(CC) $(CFLAGS) -c cypro.cc

clean:
	rm -f *.o

cleanall:
	rm -f *.o client

pack:
	tar -cvzf xcypri01.tar.gz makefile *.cc *.hh protokol.pdf test.sh salad_quote1 README
