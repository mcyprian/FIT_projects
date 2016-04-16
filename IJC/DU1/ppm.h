// ppm.h 
// Riešenie IJC-DU1, príklad b), 10. 3. 2015
// Autor: Michal Cyprian, FIT
// Preložené: gcc 4.9.2
// Popis: Rozhranie k súboru ppm.c

#ifndef PPM_H
#define PPM_H

/* štruktúra na uloženie obsahu ppm súboru */
struct ppm {
    unsigned xsize;
    unsigned ysize;
    char data[];
};

/* Ak je predaný platný ukazatel uvolní jeho pamäť */
void freeMemory(struct ppm *p);

/* Zatvorí súbor predaný ako prvý parameter */
void closeFile(FILE *file, const char *filename);

/* Otestuje či je možné previesť reťazec, ktorý preberie ako parameter na  
 * celé číslo,  v prípade že áno, návratová hodnota je 1, v prípade že nie,
 * návraová hodnota je 0. */
int testNum(char *x);

/* Načíta obsah p súboru zadaného prvým parametrom do štruktúry
 *  * struct p a vráti ukazatel na túto štruktúru, ak dôjde k chybe
 *   * vypíše chybové hlásenie a vráti NULL */
struct ppm* ppm_read(const char *filename);

/* Obsah štruktúri, predanej ako prvý parameter zapíše do súboru zadaného
 * druhým parametrom, dp súboru pridá hlavičku P6, farebnú hĺbku 255 a EOF
 * na koniec súboru. */
int ppm_write(struct ppm *p, const char *filename);

#endif /* !PPM_H */
