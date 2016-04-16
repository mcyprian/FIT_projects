// resources.c
// Riesenie IOS-uloha2, 27. 4. 2015
// Autor: Michal Cyprian (xcypri01), FIT
// Popis: Modul obsahuje funkcie na inicializaciu a uvolnenie zdrojov,
//        zdielanych medzi procesmi

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <semaphore.h>
#include <sys/shm.h>
#include <sys/ipc.h>
#include <sys/mman.h>
#include <sys/types.h>
#include "resources.h"

// Definicie premennych
extern int shmid;
extern TSharedMemory *shared;
extern sem_t *sem;

// Namapuje pole semaforov, vytvori zdielanu pamat struktury shared a
// inicializuje semafory a zdielane premenne, v pripade chyby uvolni
// vsetky do vtedy naalokovane zdroje, vypise chybovy kod a vrati 2
int sharedResourcesInit() {
    unsigned initMarks[NUMOFSEMS] = {0, }; // pole na oznacenie uspesne
    int initError = 0;                     // inicializovanych semaforov
    int semNum;
    // Namapovanie struktury semaforov
    if ((sem = mmap(NULL, NUMOFSEMS * sizeof(sem_t), PROT_READ | PROT_WRITE, 
        MAP_ANONYMOUS | MAP_SHARED, 0, 0)) == MAP_FAILED) {
        fprintf(stderr, "Chyba mapovania pamate\n");
        return EXIT_SIG_FAIL;
    }
    
    // Iniacializacia mutexov turnstile2 na 1
    for (semNum = mutex; semNum <= turnstile2; semNum++) {
        if(sem_init (&sem[semNum], 1, 1) == -1) {
            initError = 1;
        } else 
            initMarks[semNum] = 1;   // Oznaci semafor ako inicializovany
    }
    // Inicializacia barier, front turnstile na 0
    for (semNum = turnstile; semNum <= oxyQueue; semNum++) {
        if (sem_init (&sem[semNum], 1, 0) == -1) {
            initError = 1;
        } else 
            initMarks[semNum] = 1;  
    }
    // Doslo k chybe inicializacie niektoreho zo semaforov
    if (initError == 1) {
        fprintf(stderr, "Chyba inicializacie semaforu\n");
        // Znici semafory, ktore boli inicializovane
        for (semNum = mutex; semNum <= oxyQueue; semNum++) {
            if (initMarks[semNum] == 1) {
               if ((sem_destroy(&sem[semNum]) == -1)) 
                   fprintf(stderr, "Chyba odstranovania semaforu\n");
            }      
        } 
        if (munmap(sem, NUMOFSEMS * sizeof(sem_t)) == -1)
            fprintf(stderr, "chyba odmapovania pamate\n");
        sem = NULL;
            return EXIT_SIG_FAIL;
    }

    // Vytvorenie zdielanej pamate
    if ((shmid = shmget(IPC_PRIVATE, sizeof(TSharedMemory), IPC_CREAT |
                    IPC_EXCL | 0666)) == -1) {
        fprintf(stderr, "Chyba vytvarania zdielanej pamate\n");
        for (semNum = mutex; semNum <= oxyQueue; semNum++) {
            if ((sem_destroy(&sem[semNum]) == -1))
                   fprintf(stderr, "Chyba odstranovania semaforu\n");
        }  
        if (munmap(sem, NUMOFSEMS * sizeof(sem_t)) == -1)
            fprintf(stderr, "Chyba odmapovania pamate\n");
        sem = NULL;
        return EXIT_SIG_FAIL;
    }

    if ((shared = (TSharedMemory*) shmat(shmid, NULL, 0)) == (void *) -1) {
        fprintf(stderr, "Chyba pristupu k zdielanej pamati\n");
        if ((shmctl(shmid, IPC_RMID, NULL)) == -1)
            fprintf(stderr, "Chyba uvolnovania zdielanej pamate\n");
        for (semNum = mutex; semNum <= oxyQueue; semNum++) {
            if ((sem_destroy(&sem[semNum]) == -1))
                   fprintf(stderr, "Chyba odstranovania semaforu\n");
        }  
        if (munmap(sem, NUMOFSEMS * sizeof(sem_t)) == -1)
            fprintf(stderr, "Chyba odmapovania pamate\n");
        sem = NULL;
        return EXIT_SIG_FAIL;
    }

    // inicailizacia zdielanych premennych
    shared->numOfHydrogen = 0;
    shared->numOfOxygen = 0;
    shared->numOfAll = 0;
    shared->messageNum = 1;
    shared->bondingCount = 0;

    if ((shared->fp = fopen("h2o.out", "w")) == NULL) {
        fprintf(stderr, "Nepodarilo sa otvorit subor h2o.out\n");
        if ((shmdt(shared) == -1) ||
           (shmctl(shmid, IPC_RMID, NULL)) == -1)
           fprintf(stderr, "Chyba uvolnovania zdielanej pamate\n");
            for (semNum = mutex; semNum <= oxyQueue; semNum++) {
                if ((sem_destroy(&sem[semNum]) == -1))
                       fprintf(stderr, "Chyba odstranovania semaforu\n");
            }  
            if (munmap(sem, NUMOFSEMS * sizeof(sem_t)) == -1)
                fprintf(stderr, "Chyba odmapovania pamate\n");
            sem = NULL;
            shared = NULL;
       
        return EXIT_SIG_FAIL; 
    }
    return EXIT_SUCCESS;
}

// Uvolnenie vsetkych zdielanych zdrojov, ak pocas uvolnovania dojde 
// k chybe vypise chybove hlasenie a vrati 2
int sharedResourcesFree() {
    int semNum;
    int retVal = EXIT_SUCCESS;
    if (sem != NULL) {
        if (munmap(sem, NUMOFSEMS * sizeof(sem_t)) == -1) {
            retVal = EXIT_SIG_FAIL;
            fprintf(stderr, "Chyba odmapovania pamate\n");
        }
        // Znicenie semaforov
        for (semNum = mutex; semNum <= oxyQueue; semNum++) {
            if ((sem_destroy(&sem[semNum]) == -1)) {
                retVal = EXIT_SIG_FAIL;    
                fprintf(stderr, "Chyba odstranovania semaforu\n");
            }
        } 
        sem = NULL;
    }
    // Uvolnenie zdielanej pamate
    if (shared != NULL) {
        if (shared->fp != NULL) {
            fclose(shared->fp);
            shared->fp = NULL;
        }
        if ((shmdt(shared) == -1) ||
           (shmctl(shmid, IPC_RMID, NULL)) == -1) {
           fprintf(stderr, "Chyba uvolnovania pamate\n");
           retVal = EXIT_SIG_FAIL;
        }
        shared = NULL;
    }
    return retVal; 
}
