// htab_lookup.c
// Riesenie IJC-DU2, priklad 2, 18. 4. 2015
// Autor: Michal Cyprian, FIT
// Prelozene: gcc 4.8.2
// Popis: Vyhlada v tabulke polozku podla kluca zadaneho ako druhy
//        argument, ak je hladanie uspesne inkrementuje pocet vyskytov
//        slova a vrati ukazatel na polozku, v opacnom pripade vytvori
//        novy zaznam s tymto klucom. AK pride pocas alokacie k chybe
//        vrati NULL

#include <stdlib.h>
#include <string.h>
#include "htable.h"

// Vyhlada v tabulke polozku podla zadaneho kluca a vrati ukazatel na 
// zaznam polozky, ak sa zaznam o polozke v tabulke nenachadza
// prida ho do tabulky a vrati ukazatel na tento zaznam
struct htab_listitem * htab_lookup(htab_t *htable, char *key) {
    struct htab_listitem *itemPtr;
    unsigned index = hash_function(key, htable->size);
         
    // Vyhlada slovo medzi polozkami na danom indexe.
    if (htable->list[index] != NULL) {
        for (itemPtr = htable->list[index]; itemPtr != NULL; itemPtr = itemPtr->next) {
            if (strcmp(itemPtr->key, key) == 0) {
                itemPtr->data++;                   // inkrement poctu vyskytov slova
                return itemPtr;
            }
        }
    }
  
    // Hladanie neuspesne je potrebne pridat zaznam
    // Na danom indexe este nie je ziadna polozka
    if (htable->list[index] == NULL) {
        htable->list[index] = malloc(sizeof(struct htab_listitem));
        if (htable->list[index] == NULL) {
            return NULL;
        }

        htable->list[index]->key = malloc(strlen(key) + 1);
        if (htable->list[index]->key == NULL) {
            free(htable->list[index]);
            htable->list[index] = NULL;
            return NULL;
        }

        strcpy(htable->list[index]->key, key);
        htable->list[index]->data = 1;
        htable->list[index]->next = NULL;
        
        itemPtr = htable->list[index];

    } else {
       
        // kolizia, na danom indexe už je aspoň jedna položka
        // Prida novy prvok na zaciatok zoznamu na danom indexe
        itemPtr = malloc(sizeof(struct htab_listitem));
        if (itemPtr == NULL) {
            return NULL;
        }
        itemPtr->key = malloc(strlen(key) + 1);
        if (itemPtr->key == NULL) {
            free(itemPtr);
            return NULL;
        }

        strcpy(itemPtr->key, key);
        itemPtr->data = 1;

        itemPtr->next = htable->list[index];
        htable->list[index] = itemPtr;
    }
    return itemPtr;
}
