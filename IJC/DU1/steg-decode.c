// steg-decode.c 
// Riešenie IJC-DU1, príklad b), 13. 3. 2015
// Autor: Michal Cyprian, FIT
// Preložené: gcc 4.9.2
// Popis: Načíta byty z obrázka v ppm formáte zadaného prvým argumentom
//        do štruktúry, nájde v ňom ukrytú správu zakódovanú na LSb 
//        prvočíselných bytov obrázka a vypíše ju na stdin, dekódovanie
//        sa končí po načítaní znaku ukončúceho reťazec.

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <ctype.h>
#include "bit-array.h"
#include "ppm.h"
#include "error.h"
#include "eratosthenes.h"

// Maximálne veľkosť obrazových dát
#define MAX 5000 * 5000 * 3


void printLongBinary(long number) {
    int i;
    unsigned long bit = (1lu << ((sizeof(long)) * CHAR_BIT - 1));
    for (i = 0; i < 64; i++) {
        if (number & bit)
            printf("1");
        else
            printf("0"); 
            bit >>= 1;
        }
        printf("\n");
}

int decodeMSG(const char *filename) {
    unsigned long i = 2;
    char ByteToPrint[1] = {1};
    struct ppm *p = ppm_read(filename);
    unsigned index = 7;
    bool decoded = 0;
    
    if (p == NULL) {
        FatalError("Súbor %s sa nepodarilo prečítať\n", filename);
        return -1;
    }

    BA_create(primes, MAX + 1);         
    Eratosthenes(primes);
    
    while (i <= p->xsize * p->ysize * 3) {
        if (BA_get_bit(primes, i) == 0)  {
                                     // indexovanie od 0
           if (DU1_GET_BIT_(p->data, CHAR_BIT * (i + 1) - 1)) {
               DU1_SET_BIT_(ByteToPrint, index, 1);
           } else {
               DU1_SET_BIT_(ByteToPrint, index, 0);
           }
           index--;
           if (index == -1) {
               if (ByteToPrint[0] == '\0' || isprint(ByteToPrint[0])) {
                   printf("%c", ByteToPrint[0]);
                   index = 7;
                   if (ByteToPrint[0] == '\0') {
                       decoded = 1;
                       break;
                   }
               } else {
                   FatalError("Správa obsahuje nezobraziteľný znak.\n");
                   return -1;
               }
           }
        }
        i++;
    }
    freeMemory(p);
    if (!decoded) {
        FatalError("Správa v súbore %s nebola ukončená znakom \\0\n", filename);
        return -1;
    }
    printf("\n");
    return 1;
}

int main(int argc, char **argv) {
    if(argc != 2) {
        Warning("%d je neprípustný počet argumentov, program očakáva"
                " obrázok vo formáte PPM ako jediný argument\n", argc);
        return 1;
    }

    if (!decodeMSG(argv[1]))
        return 1;

    return 0;
}
