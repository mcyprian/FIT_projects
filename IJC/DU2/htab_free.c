// htab_free.c
// Riesenie IJC-DU2, priklad 2, 18. 4. 2015
// Autor: Michal Cyprian, FIT
// Prelozene: gcc 4.8.2
// Popis: Vycisti vsetky polozky tabulky a uvolni pole ukazatelov
//        volanim funkcie htab_clear, nasledne uvolni 
//        pamat struktury tabulky
  
#include <stdlib.h>
#include "htable.h"


void htab_free(htab_t *htable) {
    htab_clear(htable);
    free(htable);
    htable = NULL;
}
