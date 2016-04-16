// htab_init.c
// Riesenie IJC-DU2, priklad 2, 18. 4. 2015
// Autor: Michal Cyprian, FIT
// Prelozene: gcc 4.8.2
// Popis: Vytvori pole ukazatelov na polozky tabulky a inicializuje
//        ich hodnotou NULL, vrati ukazatel na tabulku, v pripade ze
//        sa nepodari niektora z alokacii pamate, uvolni dovtedy
//        alokovanu pamat a vrati NULL

#include <stdlib.h>
#include "htable.h"

htab_t * htab_init(unsigned size) {

    unsigned i;
    // Alokacia struktury tabilky
    htab_t *htable = malloc(sizeof(htab_t));
    if (htable == NULL) {
        return NULL;
    }

    htable->list = malloc(size * sizeof(struct htab_listitem));
    if (htable->list == NULL) {
        free(htable);
        return NULL;
    }

    for (i = 0; i < size; i++) 
      htable->list[i] = NULL;
    
    htable->size = size;

    return htable;
}
