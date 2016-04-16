// error.c
// Riešenie IJC-DU1, príklad b), 7. 3. 2015
// Autor: Michal Cyprian, FIT
// Preložené: gcc 4.9.2
// Popis: definuje funkcie na v7pis chybových hlásení, funkcie očakávajú
//        variabilný počet argumentov vo formáte ako funkcia printf

#include <stdarg.h>
#include <stdlib.h>
#include <stdio.h>
#include "error.h"

/* Vypíše hlásenie na stderr, očakáva argumenty vo formáte ako
 * funkcia printf */
void Warning(const char *fmt, ...) {
    va_list args;
    va_start(args, fmt);
    fprintf(stderr, "CHYBA: ");
    vfprintf(stderr,  fmt, args);
    va_end(args);
}

/* Vypíše chybové hlásenie a ukončí program s návratovou hodnotiu 1 */
void FatalError(const char *fmt, ...) {
    va_list args;
    va_start(args, fmt);
    fprintf(stderr, "CHYBA: ");
    vfprintf (stderr, fmt, args);
    va_end(args);
    exit(1);
}
