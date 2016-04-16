// htab_statistics.c
// Riesenie IJC-DU2, priklad 2, 19. 4. 2015
// Autor: Michal Cyprian, FIT
// Prelozene: gcc 4.8.2
// Popis: Prejde celu tabulku a vypise priemernu, maximalnu
//        a minimalnu dlzku zoznamov na indexe tabulky

#include <stdio.h>
#include <stdlib.h>
#include "htable.h"

void htab_statistics(htab_t *htable) {
    unsigned *arrayOfLengths = malloc(htable->size * sizeof(unsigned));
    unsigned i;
    unsigned sum = 0;
    struct htab_listitem *itemPtr;

    // Alokacia pola velkosti tabulky
    if (arrayOfLengths == NULL) {
        fprintf(stderr, "Nepodarila sa alokacia pamate.\n");
        return;
        }
    // Prechod tabulky a ulozenie poctu zaznamov na prislusny  index pola
    for (i = 0; i < htable->size; i++) {
        if (htable->list[i] == NULL)
            sum = 0;
        else {
            sum = 0;
            for (itemPtr = htable->list[i]; itemPtr != NULL; itemPtr =
                itemPtr->next)
            sum++;
        }
        arrayOfLengths[i] = sum;
    }
    // Najde maximalny pocet zaznamov
    unsigned max = arrayOfLengths[0];
    for (i = 1; i < htable->size; i++) {
        if (arrayOfLengths[i] > max)
            max = arrayOfLengths[i];
    }
    // Najde minimalny pocet zaznamov
    unsigned min = arrayOfLengths[0];
    for (i = 1; i < htable->size; i++) {
        if (arrayOfLengths[i] < max)
            min = arrayOfLengths[i];
    }
    // Vypocet aritmetickeho priemeru
    double average = 0;
    for (i = 0; i < htable->size; i++)
            average += (double) arrayOfLengths[i];

    average /= htable->size;

    printf("Priemer %lf max %d min %d\n", average, max, min);
    free(arrayOfLengths);
}
