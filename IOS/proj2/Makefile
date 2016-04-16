# Makefile
# Riesenie IOS-uloha2, 28. 4. 2015
# Autor: Michal Cyprian (xcypri01), FIT
# Popis: Preklada a linkuje jednotlive moduly programu,
#        vytvori spustitelny subor h2o

# Prekladac
CC=gcc

# Parametre prekladu
CFLAGS=-std=gnu99 -Wall -Wextra -Werror -pedantic 

all: h2o

# Navod na vytvorenie suboru h2o
h2o: h2o.o resources.o processes.o
	$(CC) h2o.o resources.o processes.o -pthread -o h2o

# Navody na vytvorenie objektovych suborov
h2o.o: h2o.c
	$(CC) $(CFLAGS) -c h2o.c

resources.o: resources.c
	$(CC) $(CFLAGS) -c resources.c

processes.o: processes.c
	$(CC) $(CFLAGS) -c processes.c

# Vymaze objektove subory
clean:
	rm -f *.o

# Vymaze objektove subory a spustitelny subor
cleanall:
	rm -f *.o h2o
# Zabali vsetky potebne subory do archivu
pack:
	zip xcypri01.zip *.c *.h Makefile

