// prvocisla.c
// Riešenie IJC-DU1, príklad a), 8. 3. 2015
// Autor: Michal Cyprian, FIT
// Preložené: gcc 4.9.2
// Popis: Vypýše na stdin posledných 10 prvočísel menších ako definovaná
//        konštanta N.

#include <stdio.h>
#include "bit-array.h"
#include "eratosthenes.h"

// Interval <2, N>  určuje interval v ktorom sa budú prvočísla vyhladávať
#define N 201000000

// Do pola uloží 10 posledných prvočísel pola array, vypíše ich vzostupne.
void printPrimeNumbers(BitArray_t array) {
    int i = 0;
    unsigned long primeNumbers [10];
    unsigned long n = BA_size(array) - 1lu;        
    while (i < 10) {
        if (BA_get_bit(array, n) == 0) {
            primeNumbers[i] = n;
            i++;
        }   
        n--;
    }

    for (i = 9; i >= 0; i--)
       printf("%lu\n", primeNumbers[i]);
}

int main(void) {

    BA_create(p, N + 1);
    Eratosthenes(p);
    printPrimeNumbers(p);
    return 0;
}
