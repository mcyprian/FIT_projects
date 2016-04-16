// eratosthenes.c
// Riešenie IJC-DU1, príklad a), 7. 3. 2015
// Autor: Michal Cyprian, FIT
// Preložené: gcc 4.9.2
// Popis: Uskutočňuje algoritmus eratostenovo sito, v poli typu BytArray-T
//        nastaví prvky na prvočíselných indexoch na 0, neprvočíselné na 1.

#include <stdio.h>
#include <math.h>
#include "bit-array.h"
#include "eratosthenes.h"

// Nastaví indexy všetkých násobkov čísla num na 1.
void setMultiples(BitArray_t array, unsigned long num) {
    unsigned long i;

    for (i = 2 * num; i < BA_size(array); i += num) {
        BA_set_bit(array, i, 1);
    }
}

// Uskutoční algoritmus Eratosthenovo sito nad polom array
void Eratosthenes(BitArray_t array) {
    unsigned long i;
    unsigned long sqrtNum = sqrt(BA_size(array) - 1lu);

    for (i = 2; i <= sqrtNum; i++) {
        if (BA_get_bit(array, i) == 0) {
            setMultiples(array, i);
        }
    }
}
