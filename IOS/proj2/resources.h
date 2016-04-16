// resources.h
// Riesenie IOS-uloha2, 28. 4. 2015
// Autor: Michal Cyprian (xcypri01), FIT
// Popis: Rozhranie modulu resources.c

#ifndef H2O_RESOURCES_H
#define H2O_RESOURCES_H

#include <stdio.h>

#define EXIT_SIG_FAIL 2
#define NUMOFSEMS 9

// enum na indexovanie semaforov v poli sem
enum semaphores {mutex, printMutex, bondingBarrierMutex, mainBarrierMutex, 
                 turnstile2, turnstile, mainBarrier, hydroQueue, oxyQueue}; 

// Struktura obsahujuca zdielane zdroje
typedef struct {
    unsigned messageNum;
    unsigned numOfHydrogen;
    unsigned numOfOxygen;
    unsigned numOfAll;
    unsigned bondingCount;
    FILE *fp;
} TSharedMemory;

// Hlavicky funkcii
int sharedResourcesInit();
int sharedResourcesFree();

#endif // !H2O_RESOURCES_H
