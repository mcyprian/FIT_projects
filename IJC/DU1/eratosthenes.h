// eratosthenes.h
// Riešenie IJC-DU1, príklad a), 7. 6. 2015
// Autor: Michal Cyprian, FIT
// Preložené: gcc 4.9.2
// Popis: Rozhranie k súboru eratosthenes.c

#ifndef ERATOSTHENES_H
#define ERATOSTHENES_H

// Nastaví indexy všetkých násobkov čísla num na 1.
void setMultiples(BitArray_t array, unsigned long num);

// Uskutoční algoritmus Eratosthenovo sito nad polom array
void Eratosthenes(BitArray_t array);

#endif /* !ERATOSTHENES_H */
