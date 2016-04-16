// h2o.h
// Riesenie IOS-uloha2, 28. 4. 2015
// Autor: Michal Cyprian (xcypri01), FIT
// Popis: Rozhranie modulu h2o.c

#ifndef H2O_H
#define H2O_H

// Struktura na ulozenie parametrov programu
typedef struct {
    unsigned N;
    unsigned GH;
    unsigned GO;
    unsigned B;
} TParams;

// Rozlisenie stavov funkcie testNum
enum testNumStates {number, timeInt};

// Hlavicky funkcii
void armagedon();
void noop();
int testNum(char *x, int state);
int parseArguments(TParams *P, int argc, char **argv);

#endif // !H2O_H
