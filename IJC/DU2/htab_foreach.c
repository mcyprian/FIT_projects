// htab_foreach.c
// Riesenie IJC-DU2, priklad 2, 18. 4. 2015
// Autor: Michal Cyprian, FIT
// Prelozene: gcc 4.8.2
// Popis: Zavola funkciu, ktoru preberie ako druhy argument
//        pre kazdy prvok tabulky

#include <stdlib.h>
#include "htable.h"

void htab_foreach(htab_t *htable, void (*function)(char *key, unsigned value)) {
    unsigned i;
    struct htab_listitem *itemPtr;

    for (i = 0; i < htable->size; i++) {
        if (htable->list[i] != NULL) { 
            for (itemPtr = htable->list[i]; itemPtr != NULL; itemPtr = itemPtr->next) {
                function(itemPtr->key, itemPtr->data);
            }
        }
    }
}
