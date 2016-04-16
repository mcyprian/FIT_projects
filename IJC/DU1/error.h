// error.h
// Riešenie IJC-DU1, príklad b), 7. 6. 2015
// Autor: Michal Cyprian, FIT
// Preložené: gcc 4.9.2
// Popis: Rozhranie k súboru error.c

#ifndef ERROR_H
#define ERROR_H

/* Vypíše hlásenie na stderr, očakáva argumenty vo formáte ako
 *  funkcia printf */
void Warning(const char *fmt, ...);

/* Vypíše chybové hlásenie a ukončí program s návratovou hodnotiu 1 */
void FatalError(const char *fmt, ...);

#endif /* !ERROR_H */
