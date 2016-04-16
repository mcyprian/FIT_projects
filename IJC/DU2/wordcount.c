// wordcount.c
// Riesenie IJC-DU2, priklad 2, 20. 4. 2015
// Autor: Michal Cyprian, FIT
// Prelozene: gcc 4.8.2
// Popis: Uklada pocet slov precitanych zo stdin so hashovacej tabulky
//        po precitani EOF vypise zoznam slov a pocet ich vyskytov

#include <stdio.h>
#include "htable.h"
#include "io.h"

#define MAXWORDLENGTH 128
#define HTABLESIZE 12289
/*
 * Zvolil som velkost pola tabulky 12289, z literatury som sa 
 * dozvedel ze ako pocet indexov do hashovacej tabulky je vhodne
 * prvocislo, pretoze to zmensuje pocet kolizii, v zadani nebolo
 *  specifikovane aky objem slov bude program spracovavat, preto som 
 * sa snazil zvolit strednu cestu. */ 

void printItem(char *key, unsigned value) {
    printf("%s\t%d\n", key, value);
}

int main(void) {
    char wordBuffer[MAXWORDLENGTH];

    htab_t *htable = htab_init(HTABLESIZE);

    // Vytvorenie a inicializacia tabulky
    if (htable == NULL) {
        fprintf(stderr, "Chyba:Nepodarila sa alokacia pamate "
                "pre tabulku.\n");
        return 1;
    }

    while (fgetw(wordBuffer, MAXWORDLENGTH, stdin) != EOF) {
        if (htab_lookup(htable, wordBuffer) == NULL) {
            fprintf(stderr, "Chyba: Nepodarila sa alokacia pamate "
                    "pre novy zaznam\n");
            htab_free(htable);
            return 1;
        }
    }

    htab_foreach(htable, printItem);

    htab_free(htable);
    return 0;
}
