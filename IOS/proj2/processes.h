// processes.h
// Riesenie IOS-uloha2, 24. 4. 2015
// Autor: Michal Cyprian (xcypri01), FIT
// Popis: Rozhranie modulu processes.c

#include "resources.h"
#include "h2o.h"

// Hlavicky funkcii
void endChild();
int randGen(unsigned a);
void safePrint(char *msg, unsigned iNum);
void bond(TSharedMemory *shared, char type, unsigned iNum, unsigned B);
int newHydrogenProcess(unsigned iNum, unsigned N, unsigned B);
int newOxygenProcess(unsigned iNum, unsigned N, unsigned B);
int newHydroMaker(TParams *P);
int newOxyMaker(TParams *P);
