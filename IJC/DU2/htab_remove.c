// htab_remove.c
// Riesenie IJC-DU2, priklad 2, 19. 4. 2015
// Autor: Michal Cyprian, FIT
// Prelozene: gcc 4.8.2
// Popis: Vyhlada v tabulke zaznam na zaklade kluca zadaneho druhym
//        argumentom, ak zaznam najde uvolni jeho pamat.

#include <stdlib.h>
#include <string.h>
#include "htable.h"

void htab_remove(htab_t *htable, char *key) {
    struct htab_listitem *itemPtr;
    struct htab_listitem *prevPtr = NULL;
    unsigned index = hash_function(key, htable->size);

    if (htable->list[index] != NULL) {
        // Prehladanie zoznamu na danom indexe
        for (itemPtr = htable->list[index]; itemPtr != NULL; 
                prevPtr = itemPtr, itemPtr = itemPtr->next) {

            if (strcmp(itemPtr->key, key) == 0) {
                if (prevPtr == NULL) {
                    // prvy zaznam sa zhoduje s klucom
                    htable->list[index] = itemPtr->next;
                } else {
                    // zaznam zo stredu zoznamu sa zhoduje s klucom
                    prevPtr->next = itemPtr->next;
                }
                free(itemPtr->key);
                free(itemPtr);
                return;
            }
        }
    }
}

