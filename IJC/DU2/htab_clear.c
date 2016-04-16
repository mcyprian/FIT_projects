// htab_clear.c
// Riesenie IJC-DU2, priklad 2, 18. 4. 2015
// Autor: Michal Cyprian, FIT
// Prelozene: gcc 4.8.2
// Popis: Prehlada celu tabulku a uvolni pamat vsetkych alokovanych
//        prvkov zoznamu a nakoniec aj pole ukazatelov.

#include <stdlib.h>
#include "htable.h"

// Uvolnenie pamate vsetkych poloziek tabulky
void htab_clear(htab_t *htable) {
    unsigned i;
    struct htab_listitem *itemPtr;
    struct htab_listitem *nextPtr;

    for (i = 0; i < htable->size; i++) {
        if (htable->list[i] != NULL) {
            itemPtr = htable->list[i];
            while (itemPtr != NULL) {
                nextPtr = itemPtr->next;
                free(itemPtr->key);
                free(itemPtr);
                itemPtr = nextPtr;
            }
        }
    }
    free(htable->list);
}
