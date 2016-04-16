// tail.c
// Riesenie IJC-DU2, priklad 1 a), 11. 4. 2015
// Autor: Michal Cyprian, FIT
// Prelozene: gcc 4.8.2
// Popis: Vypise poslednych 10 riadkov zo vstuneho suboru zadaneho
//        prvym argumentom, ak nie je zadany cita zo stdin. Ak je
//        programu zadany argument -n cislo pocet vypisanych riadkov
//        z konca suboru urcenych hodnotou cislo, ak je zadany
//        parameter -n +cislo, vypisuje riadky od cislo do konca
//        suboru.

#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>

#define MAXLINELENGTH 510 

enum states { standart, numToEnd };

/* Struktura na spracovanie parametrov programu */
typedef struct {
    unsigned n;
    char *file;
    int state;
} TParams;

// Definicia noveho typu pre vytvorenie a indexovanie
// cyklickeho bufferu
typedef char line_t [MAXLINELENGTH + 2];
/* Otestuje ci  je mozne previest retazec, ktory preberie ako parameter na  
 *  cele, kladne cislo,  v pripade ze ano, navratova hodnota je 1, 
 *  v pripade  ze nie, navratova hodnota je 0. */
int testNum(char *x) {
     char *ptr;
     int retVal = strtol(x, &ptr, 10);

     if (( *ptr != '\0') || retVal <= 0) 
        return 0;    
    else
        return 1;
}

/* Funkcia spracuje argumenty programu a vrati ich v strukture TParams
 * pri chybe vrati NULL*/
TParams * getParams(TParams *p, int argc, char**argv) {
    
    switch (argc) {
        case 1:
            p->state = standart;
            break;
        case 2:
            p->state = standart;
            p->file = argv[1];
            break;
        case 3:
        case 4:
            if ((strcmp(argv[1], "-n") == 0)) {
                if (argv[2][0] == '+'
                   && testNum(&argv[2][1])) {
                    p->state = numToEnd;
                    p->n = strtol(argv[2], NULL, 10);
                } else if (testNum(argv[2])) {
                    p->state = standart;
                    p->n = strtol(argv[2], NULL, 10);
                } else {
                    fprintf(stderr, "Nesprávny parameter prepínača -n \n");
                    return NULL;
                }
            } else {
                fprintf(stderr, "Nesprávny formát argumentov\n");
                return NULL;
            }
            if (argc == 4)
                p->file = argv[3];
            break;
        default:
            fprintf(stderr, "Nesprávny počet argumentov\n");
            return NULL;
    }
     
    return p;    
}

// Uvolni pamat predaneho ukazatela a nastavi ho na NULL
void freeMem(void *p) {
    free(p);
    p = NULL;
}

// Zatvori subor predany ako prvy parameter
void closeFile(FILE *file, const char *fileName) {
    if ((fclose(file)) != 0)
        fprintf(stderr, "Chyba pri zatvarani suboru %s\n", fileName);
}


/* Na zaklade nastaveneho stavu a poctu riadkov vypise vystup na 
 * stdout, pri uspechu vracia 0, pri neuspechu vypise chybove hlasenie
 * na stderr a vrati 1*/
int printTail(char *fileName, unsigned n) {
    line_t line;
    unsigned firstPos = 0;
    FILE *fr = NULL;
    int c;
    unsigned i = 0;
    unsigned warningPrinted = 0;

    
    line_t *lineBuffer = malloc(n * sizeof(line_t));

    if (lineBuffer == NULL) {
        fprintf(stderr, "Chyba pri alokacii pamate.\n");
        return 1;
    }

    // Ak nebol zadany subor bude sa citat  zo stdin
    if (fileName == NULL)
        fr = stdin;
    else {
        fr = fopen(fileName, "r");

        if (fr == NULL) {
            fprintf(stderr, "Nepodarilo sa otvorit subor %s.\n", fileName);
            freeMem(lineBuffer);
            return 1;
        }
    }

    while (fgets(line, MAXLINELENGTH + 1,  fr) != NULL) { 
        if ((strcmp(line, "\0")) == 0) {
            fprintf(stderr, "Nepodarilo sa nacitat riadok suboru.\n");
            freeMem(lineBuffer);
            closeFile(fr, fileName);
            return 1;
        }
        // Ak nakonci bufferu nie je znak \n, nenacital sa cely riadok
        // pri prvom prekroceni limitu dlzky riadka vypise varovanie
        // zvysok riadka sa zahodi
        if (line[strlen(line) - 1] != '\n') {
            if (!warningPrinted) {
                fprintf(stderr, "Dlzka nektoreho z riadkov presahuje limit 510 znakov.\n");
                warningPrinted = 1;
            }
            line[strlen(line) + 1] = '\0';
            line[strlen(line)] = '\n';
            do {
                c = fgetc(fr);
             } while (c != '\n');
        }
        strcpy(&lineBuffer[i % n][0], line);


        /* Ak neprepisuje buffer v prvom cykle nastavi zacatocnu poziciu na
         * najstarsi riadok, teda nasledujuci za naposledy prepisanym */
        if (i / n > 0) 
            firstPos = i % n + 1;
        i++;

    }
    if (i < n)
        n = i;

    // Vypise aktualny obsah bufferu na stdin

    for(i = firstPos; i < firstPos + n; i++) 
       printf("%s", lineBuffer[i % n]); 

    freeMem(lineBuffer);
    closeFile(fr, fileName);
    return 0;
}

/* Vypise na stdout riadky od n do konca suboru, pri uspechu vrati 0
 * pri chybe vypise chybove hlasenie na stderr a vrati 0 */
int printToEnd(char *fileName, unsigned n) {
    char lineBuffer [MAXLINELENGTH + 2];
    FILE *fr = NULL;
    int c;
    unsigned warningPrinted = 0;
    unsigned lineNumber = 0;

    // Ak nie je nastaveny subor cita zo stdin
    if (fileName == NULL)
        fr = stdin;
    else {
        fr = fopen(fileName, "r");
        if (fr == NULL) {
            fprintf(stderr, "Nepodarilo sa otvorit subor.\n");
            return 1;
        }
    }

    while (fgets(lineBuffer, MAXLINELENGTH + 1, fr) != NULL) {
        lineNumber++;
        if (lineBuffer[strlen(lineBuffer) - 1] != '\n') {
            if (!warningPrinted) {
                fprintf(stderr, "Dlzka riadku presahuje limit 510 znakov.\n");
                warningPrinted = 1;
            }
            lineBuffer[strlen(lineBuffer) +1] = '\0';
            lineBuffer[strlen(lineBuffer)] = '\n';
            do {
                c = fgetc(fr);
            } while (c != '\n');
        }

        if (lineNumber >= n)
            printf("%s", lineBuffer);
    }
    
    closeFile(fr, fileName);
    return 0;
}


int main(int argc, char **argv) {
    TParams p = {.n = 10, .file = NULL, .state = standart};

    if (getParams(&p, argc, argv) == NULL)
        return 1;

    switch(p.state) {
        case standart:
            if (printTail(p.file, p.n))
                return 1;
            break;
        case numToEnd:
            if (printToEnd(p.file, p.n))
                return 1;
            break;
    }

    return 0;
}
