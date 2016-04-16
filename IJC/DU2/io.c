// io.c
// Riesenie IJC-DU2, priklad 2, 20. 4. 2015
// Autor: Michal Cyprian, FIT
// Prelozene: gcc 4.8.2
// Popis: Nacita jedno slovo zo suboru f do pola znakov s vrati dlzku 
//        slova, v pripade ze narazi na koniec suboru vrati EOF

#include <stdio.h>
#include <ctype.h>
#include <stdbool.h>
#include "io.h"


int fgetw(char *s, int max, FILE *f) {
    if (f == NULL) {
        fprintf(stderr, "Predany neplatny FILE ukazatel.\n");
        return -2;
    }
    
    int c;
    int i = 0;
    unsigned length = 0;  // Pocitadlo precitanych znakov
    static bool warningPrinted = 0;
    
    // Vymaze pripadne biele znamky na zaciatku streamu
    while ((c = fgetc(f)) != EOF) {
        if (!isspace(c))
            break;
    }
    if (c == EOF)
        return EOF;

    // Vrati posledny precitany znak
    ungetc(c, f);

    while ((c = fgetc(f)) != EOF) {
        if (isspace(c) || (i == max - 1)) {
            if (i == max -1) {
                if (warningPrinted == 0) {    // Ak ide o prve prekrocenie
                                              // vypise varovane
                    fprintf(stderr, "Varovanie: bol prekroceny limit " 
                        "maximalnej dlzky slova 127 znakov.\n");
                    warningPrinted = 1;
                }
                while(!isspace(c) && c != EOF) { // zkrati slovo
                    c = fgetc(f);
                }
                if (c == EOF)
                    return EOF;
            }
            break;
        }
        s[i++] = c;
        length++;
    }
    if (c == EOF)
        return EOF;
    
    s[i] = '\0';
    return length;
}
