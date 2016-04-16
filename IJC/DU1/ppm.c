// ppm.c 
// Riešenie IJC-DU1, príklad b), 10. 3. 2015
// Autor: Michal Cyprian, FIT
// Preložené: gcc 4.9.2
// Popis: definuje funkcie na prácu so štruktúrov typu struct ppm
//        ktorá slúži na načítanie rozmerov a RGB bytov obrázka 
//        vo formáte ppm, umožňuje vytvoriž súbor v ppm formáte
//        z dát uložených v štruktúre

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "bit-array.h"
#include "error.h"
#include "ppm.h"

/* Ak je predaný platný ukazatel uvolní jeho pamäť */
void freeMemory(struct ppm *p) {
    if (p != NULL)
        free(p);
}

/* Zatvorí súbor predaný ako prvý parameter */
void closeFile(FILE *file, const char *filename) {
    if (file != NULL)
        if((fclose(file)) != 0)
            Warning("Nastala chyba pri zatváraní súboru %s\n", filename);
}

/* Otestuje či je možné previesť reťazec, ktorý preberie ako parameter na  
 *  celé číslo,  v prípade že áno, návratová hodnota je 1, v prípade že nie, návraová hodnota je 0. */
int testNum(char *x) {
    char *ptr;
        // x = "abc"
    if (((strtol(x, &ptr, 0) == 0 && *ptr != '\0') 
        // x = "0"        
        && (!(strtol(x, &ptr, 0) == 0 && *ptr == '\0'))) 
        // x = "95a"   
       || (strtol(x, &ptr, 0) != 0 && *ptr != '\0'))
        return 0;    
    else
        return 1;
}

/* Vypíše počet bytov pola data zadaný parametrom num na stdout */
void printPPM(char data[], int num) {
    int i;
    for (i = 0; i < num; i++)
        printf("%d ", data[i]); // (i % 50 != 49 ? "" : "\n"));i
    printf("\n\n");
}

/* Načíta obsah p súboru zadaného prvým parametrom do štruktúry
 * struct p a vráti ukazatel na túto štruktúru, ak dôjde k chybe
 * vypíše chybové hlásenie a vráti NULL */
struct ppm* ppm_read(const char *filename) { 
    FILE *fr = NULL;
    struct ppm *p = NULL;

    if ((fr = fopen(filename, "rb")) == NULL) {
        Warning("Nepodarilo sa otvoriť súbor %s\n", filename);
        return NULL;
    }

    char fileTypeID[4];
    char xstring[5]; 
    char ystring[5];      
    char maxValue[4];   

    if (fscanf(fr, "%3s%4s%4s%3s\n", fileTypeID, xstring, ystring, maxValue)             != 4) {
        Warning("Chyba čítania súboru %s\n", filename);
        closeFile(fr, filename);
        return NULL;
    }

    if (strcmp(fileTypeID, "P6") != 0) {
        Warning("Typ súboru %s nezodpovedá požadovanému formátu\n", 
                fileTypeID);
        closeFile(fr, filename);
        return NULL;
    }

    if (!(testNum(xstring) && testNum(ystring))) {
        Warning("Nie je možné zistiť rozmery obrázka\n");
        closeFile(fr, filename);
        return NULL;
    }
    
    int x = strtol(xstring, NULL, 0);
    int y  = strtol(ystring, NULL, 0);

    if (x * y > 5000 * 5000) {
        Warning("%lu prekračuje limit maximálnej veľkosti obrazových dát"
                " 5000 * 5000 * 3\n", x * y * 3);
        closeFile(fr, filename);
        return NULL;
    }

    if (strcmp(maxValue, "255") != 0) {
        Warning("Maximálna farebná hĺbka %s súboru  %s nemá požadovanú"
               " hodnotu 255\n", maxValue, filename);
        closeFile(fr, filename);
        return NULL;
    }

   if ((p = malloc(sizeof(struct ppm) + x * y * 3)) == NULL) {
        Warning("Nepodarilo sa nalokovať pamäť\n");
        closeFile(fr, filename);
        return NULL;
    }

    p->xsize = x; 
    p->ysize = y;

    if((fread(p->data, 1, p->xsize * p->ysize * 3, fr)) != 
            p->xsize * p->ysize * 3) {
        Warning("Nepodarilo sa načítať RGB byty zo súboru %s\n", filename);
    closeFile(fr, filename);
    freeMemory(p);
    return NULL;
    }

    closeFile(fr, filename);

    return p;
}


/* Obsah štruktúry, predanej ako prvý parameter zapíše do súboru zadaného
 * druhým parametrom, dp súboru pridá hlavičku P6, farebnú hĺbku 255 a EOF
 * na koniec súboru. */
int ppm_write(struct ppm *p, const char *filename) {
    FILE *fw = NULL;
    if ((fw = fopen(filename, "wb")) == NULL) {
        Warning("Nepodarilo sa otvoriť súbor %s\n", filename);
        return -1;
    }

    if ((fprintf(fw, "P6\n%u %u\n255\n", p->xsize, p->ysize) < 0)) {
        Warning("Nepodaril sa zápis do súboru %s\n", filename);
        closeFile(fw, filename);
        return -1;
    }
    
    if ((fwrite(p->data, 1, p->xsize * p->ysize * 3, fw)) != 
            p->xsize * p->ysize * 3) {
        Warning("Nepodarilo sa zapísať RGB byty do súboru %s\n", filename);
        closeFile(fw, filename);
        return -1;
    }

    closeFile(fw, filename);
    return 0; 
}

/*
int main(void) {
    struct ppm *p = ppm_read("du1-obrazek.ppm"); 
    if (p == NULL)
        return 1;

    // struct ppm *prz = ppm_read("prezil..?");

    // printPPM(&p->data[p->xsize*p->ysize*3 -20], 20);
    // printPPM(&p->data[prz->xsize*prz->ysize*3 -20], 20);
    
      for (int i = 0; i < p->xsize  * p->ysize * 3 / 2; i++)
        if ((i % 100) == 99)
            p->data[i] = 255;

    ppm_write(p, "prezil..?");
    freeMemory(p);

    return 0;
}*/
