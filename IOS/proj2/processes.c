// processes.c
// Riesenie IOS-uloha2, 24. 4. 2015
// Autor: Michal Cyprian (xcypri01), FIT
// Popis: Modul obsahuje funkcie s kodmi procesov reprezentujucich atomy
//        vodika a kyslika a kody procesov, hydroMaker a oxyMaker,
//        ktore vytvaraju potrebny pocet procesov reprezentujucich
//        atomy a cakaju na ich ukoncenie

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <signal.h>
#include <semaphore.h>
#include <sys/wait.h>
#include <sys/types.h>
#include "processes.h"

extern TSharedMemory *shared;
extern sem_t *sem;

//Obsluha signalu SIGUSR1 pre procesy potomkov
void endChild() {
    exit(EXIT_SIG_FAIL);
}

// Generator pseudonahodnych cisel s prevodom mikrosekund na milisekundy
int randGen(unsigned a) {
        return((rand() % (a + 1))*1000);
}


// Vypise spravu a uspi proces na nahodnu dobu z interval (0, B).
void bond(TSharedMemory *shared, char type, unsigned iNum, unsigned B) {
    sem_wait(&sem[printMutex]);
        fprintf(shared->fp, "%d\t: %c %d\t: begin bonding\n",
                shared->messageNum++, type, iNum);
        fflush(shared->fp);
    sem_post(&sem[printMutex]);
    usleep(randGen(B));
}

// Vypise spravu predanu ako prvy parameter do suboru, 
// pomocou semaforu je zaruceny  vylucny pristup k suboru
void safePrint(char *msg, unsigned iNum) {
    sem_wait(&sem[printMutex]);
        fprintf(shared->fp, msg, shared->messageNum++, iNum);
        fflush(shared->fp);
    sem_post(&sem[printMutex]);
}   
// Kod procesu reprezentujuceho vodik, vylucny zapis do suboru zabezpecuje
// semafor printMutex, na vhodnych miestach su vypisy o cinnosti procesu
int newHydrogenProcess(unsigned iNum, unsigned N, unsigned B) {

    safePrint("%d\t: H %d\t: started\n", iNum);

    sem_wait(&sem[mutex]);

    shared->numOfHydrogen++;
    if (shared->numOfHydrogen >= 2 && shared->numOfOxygen >= 1) {
        safePrint("%d\t: H %d\t: ready\n", iNum);
        
        sem_post(&sem[hydroQueue]);
        sem_post(&sem[hydroQueue]);
        shared->numOfHydrogen -= 2;
        sem_post(&sem[oxyQueue]);
        shared->numOfOxygen --;
    } else {
        safePrint("%d\t: H %d\t: waiting\n", iNum);
        sem_post(&sem[mutex]);
    }
    
    sem_wait(&sem[hydroQueue]);
    bond(shared, 'H', iNum, B);
    
    // Znovupouzitelna bariera zabezpecuje spolocne ukoncenie tvorby molekuly
    sem_wait(&sem[bondingBarrierMutex]);
        shared->bondingCount++;
        if (shared->bondingCount == 3) {
            sem_wait(&sem[turnstile2]);
            sem_post(&sem[turnstile]);
        }
    sem_post(&sem[bondingBarrierMutex]);

    sem_wait(&sem[turnstile]);
    sem_post(&sem[turnstile]);

    safePrint("%d\t: H %d\t: bonded\n", iNum);
   
    sem_wait(&sem[bondingBarrierMutex]);
        shared->bondingCount--;
        if (shared->bondingCount == 0) {
            sem_wait(&sem[turnstile]);
            sem_post(&sem[turnstile2]);
        }
    sem_post(&sem[bondingBarrierMutex]);
   
    sem_wait(&sem[turnstile2]);
    sem_post(&sem[turnstile2]);

    // Bariera zaruci spolocne ukoncenie vsetkych procesov
    sem_wait(&sem[mainBarrierMutex]);
        shared->numOfAll++;
    sem_post(&sem[mainBarrierMutex]);

    if (shared->numOfAll == 3*N)
        sem_post(&sem[mainBarrier]);

    sem_wait(&sem[mainBarrier]);
    sem_post(&sem[mainBarrier]);
    // Koniec barriery


    safePrint("%d\t: H %d\t: finished\n", iNum);
    
    exit(EXIT_SUCCESS);
}

// Kod procesu reprezentujuceho kyslik
int newOxygenProcess(unsigned iNum, unsigned N, unsigned B) {

    safePrint("%d\t: O %d\t: started\n", iNum);

    sem_wait(&sem[mutex]);
    shared->numOfOxygen++;
    if (shared->numOfHydrogen >= 2) {
        safePrint("%d\t: O %d\t: ready\n", iNum);
        
        sem_post(&sem[hydroQueue]);
        sem_post(&sem[hydroQueue]);
        shared->numOfHydrogen -= 2;
        sem_post(&sem[oxyQueue]);
        shared->numOfOxygen--;
    } else {
        safePrint("%d\t: O %d\t: waiting\n", iNum);
        sem_post(&sem[mutex]);
    }
    
    sem_wait(&sem[oxyQueue]);
    bond(shared, 'O', iNum, B);
    
  // Znovupouzitelna bariera zabezpecuje spolocne ukoncenie tvorby molekuly
    sem_wait(&sem[bondingBarrierMutex]);
        shared->bondingCount++;
        if (shared->bondingCount == 3) {
            sem_wait(&sem[turnstile2]);
            sem_post(&sem[turnstile]);
        }
    sem_post(&sem[bondingBarrierMutex]);

    sem_wait(&sem[turnstile]);
    sem_post(&sem[turnstile]);

    safePrint("%d\t: O %d\t: bonded\n", iNum);
   
    sem_wait(&sem[bondingBarrierMutex]);
        shared->bondingCount--;
        if (shared->bondingCount == 0) {
            sem_wait(&sem[turnstile]);
            sem_post(&sem[turnstile2]);
        }
    sem_post(&sem[bondingBarrierMutex]);
   
    sem_wait(&sem[turnstile2]);
    sem_post(&sem[turnstile2]);
    
    sem_post(&sem[mutex]);

    // Bariera zabezpecuje spolocne ukoncenie vsetkych procesov
    sem_wait(&sem[mainBarrierMutex]);
        shared->numOfAll++;
    sem_post(&sem[mainBarrierMutex]);

    if (shared->numOfAll == 3*N)
        sem_post(&sem[mainBarrier]);

    sem_wait(&sem[mainBarrier]);
    sem_post(&sem[mainBarrier]);
    // Koniec barriery

    safePrint("%d\t: O %d\t: finished\n", iNum);

    exit (EXIT_SUCCESS);
}

// Kod procesu, ktory vytvara 2*N procesov H, nasledne caka na ukoncenie
// vsetych potomkov, ak dojde k chybe vrati 2
int newHydroMaker(TParams *P) {
    pid_t childPid;
    unsigned i;

    signal(SIGUSR1, endChild); 
    for (i = 0; i < 2*P->N; i++) {
        usleep(randGen(P->GH));
        switch (childPid = fork()) {
            case -1:
                fprintf(stderr, "Chyba tvorby noveho procesu\n");
                exit(EXIT_SIG_FAIL);
            case 0:
                newHydrogenProcess(i + 1, P->N, P->B);
            default:
                break;
        }
    }
    for (i = 0; i < 2*P->N; i++) {   // Cakanie na potomkov
        wait(NULL);
    }
    exit(EXIT_SUCCESS);
}

// Kod procesu, ktory vytvara N procesov O.
int newOxyMaker(TParams *P) {   
    pid_t childPid;
    unsigned i;

    signal(SIGUSR1, endChild); 
    for (i = 0; i < P->N; i++) {
        usleep(randGen(P->GO));       // Caka nahodnu dobu z intervalu {0, GO}
        switch (childPid = fork()) {
            case -1:
                fprintf(stderr, "Chyba tvorby noveho procesu\n");
                exit(EXIT_SIG_FAIL);
            case 0:
                newOxygenProcess(i + 1, P->N, P->B);
            default:
                break;
        }
    }
    for (i = 0; i < P->N; i++) {  // Cakanie na potomkov
        wait(NULL);
    }
    exit(EXIT_SUCCESS);
}
