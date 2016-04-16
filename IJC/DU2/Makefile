# Makefile
# Riesenie IJC-DU2, 20. 4. 2015
# Autor: Michal Cyprian, FIT
# Popis: Vytvori programy tail, tail2, wordcount, wordcount-dynamic a
#        kniznice libhtable.a a libhtable.so           

# Prekladace
CC=gcc

# Parametre prekladu
CFLAGS=-std=c99 -pedantic -Wall -Wextra -g


LIBFILES=hash_function.o htab_init.o htab_lookup.o htab_foreach.o htab_remove.o htab_clear.o htab_free.o htab_statistics.o


all: tail tail2 wordcount wordcount-dynamic

# Vytvori subor tail
tail: tail.o
	$(CC) tail.o -o tail

tail.o: tail.c
	$(CC) $(CFLAGS) -c tail.c

# Vytvori subor tail2
tail2: tail2.o
	g++  tail2.o -o tail2

tail2.o: tail2.cc
	g++ -std=c++11 -pedantic -Wall  -c tail2.cc
	

# Vytvori wordcount
wordcount: wordcount.o io.o libhtable.a
	$(CC) $(CFLAGS) -o wordcount -static io.o wordcount.o libhtable.a

# Vytvori wordcount-dynamic
wordcount-dynamic: wordcount.o io.o libhtable.so
	$(CC) $(CFLAGS) -o wordcount-dynamic wordcount.o io.o -L.  libhtable.so
	

# Vytvori staticku knoznicu
libhtable.a: $(LIBFILES)
	ar crs libhtable.a $(LIBFILES)
	
# vytvori dynamicku kniznicu
libhtable.so: $(LIBFILES)
	$(CC) $(CFLAGS) -shared -fPIC -o libhtable.so $(LIBFILES)

# Preklady modulov knizice
hash_function.o: hash_function.c
	$(CC) $(CFLAGS) -c -fPIC hash_function.c

htab_init.o: htab_init.c
	$(CC) $(CFLAGS) -c -fPIC htab_init.c

htab_lookup.o: htab_lookup.c
	$(CC) $(CFLAGS) -c -fPIC htab_lookup.c

htab_foreach.o: htab_foreach.c
	$(CC) $(CFLAGS) -c -fPIC htab_foreach.c

htab_remove.o: htab_remove.c
	$(CC) $(CFLAGS) -c -fPIC htab_remove.c

htab_clear.o: htab_clear.c
	$(CC) $(CFLAGS) -c -fPIC htab_clear.c

htab_free.o: htab_free.c
	$(CC) $(CFLAGS) -c -fPIC htab_free.c

htab_statistics.o: htab_statistics.c
	$(CC) $(CFLAGS) -c -fPIC htab_statistics.c

# Parameter clean vymaze objektove subory
clean:
	rm -f *.o
