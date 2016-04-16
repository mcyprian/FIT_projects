# makefile
# Riešenie IJC-DU1, 15. 3. 2015
# Autor: Michal Cyprian, FIT
# Popis: Prekladá a linkuje jednotlivé moduly programu,
#        vytvorí spustiteľné súbory prvocisla, prvocisal-inline s
#        použitím inline funkcií z bit-array.h a steg-decode

# Spustenie: make -vytvorí všetky spustiteľné súbory
#            make clean -vymaže všetky súbory objektové súbory


# Prekladač
CC=gcc
# Parametre prekladu:
CFLAGS=-O2 -std=c99 -Wall -pedantic -lm


all: prvocisla  prvocisla-inline steg-decode

# Vytvorí súbor prvocisla
prvocisla: prvocisla.o eratosthenes.o error.o
	$(CC) prvocisla.o eratosthenes.o error.o -lm -o prvocisla

prvocisla.o: prvocisla.c
	$(CC) $(CFLAGS) -c prvocisla.c

eratosthenes.o: eratosthenes.c
	$(CC) $(CFLAGS) -c eratosthenes.c 

error.o: error.c
	$(CC) $(CFLAGS) -c error.c

# Vytvorí súbor prvocisla-inline	
prvocisla-inline: prvocisla-inline.o eratosthenes-inline.o error.o
	$(CC) prvocisla-inline.o eratosthenes-inline.o error.o -lm -o prvocisla-inline

prvocisla-inline.o: prvocisla.c
	$(CC) $(CFLAGS) -DUSE_INLINE -c prvocisla.c -o prvocisla-inline.o

eratosthenes-inline.o: eratosthenes.c
	$(CC) $(CFLAGS) -DUSE_INLINE -c eratosthenes.c -o eratosthenes-inline.o

# Vytvorí súbor steg-decode
steg-decode: steg-decode.o ppm.o eratosthenes.o error.o
	$(CC) steg-decode.o ppm.o eratosthenes.o error.o -lm -o steg-decode

steg-decode.o: steg-decode.c
	$(CC) $(CFLGS) -c steg-decode.c

ppm.o: ppm.c
	$(CC) $(CFLAGS) -c ppm.c

# maže objektové súbory
clean:
	rm -f *.o 
