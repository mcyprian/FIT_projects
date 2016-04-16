// h2o.c
// Riesenie IOS-uloha2, 24. 4. 2015
// Autor: Michal Cyprian (xcypri01), FIT
// Popis: Program je modifikovanou verziou synchronizacneho problemu
//        Building H2O, Existuju dva typy procesov, kyslik a vodik. Aby
//        sa mohli spojit do molekuly vody, je potrebne pockat kym bude
//        k dipozicii jeden atom kysliku a dva atomy vodikum potom sa spoja
//        do molekuly a proces sa opakuje. Vystupom programu je subor
//        h2o.out, ktory obsahuje zaznam o cinnosti jednotlivych atomov.
//
// Argumenty: ./h2o N GH GO B
//            N je pocet atomov kyslika, pocet atomov vodika je 2*N, N > 0
//            GH je maximalna doba po ktorej je generovany novy 
//               proces pre vodik
//            GO je maximalna doba po ktorej je generovany novy
//               proces pre kyslik
//            B je maximalna doba trvania funkcie bond
//            GH, GO, B >= 0 && GH, GO, B < 5001

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <semaphore.h>
#include <sys/wait.h>
#include <signal.h>
#include <time.h>
#include "h2o.h"
#include "resources.h"
#include "processes.h"

// Deklaracia globalnych premennych
int shmid = 0;
TSharedMemory *shared = NULL;
sem_t *sem = NULL;


// Obsluha signalov SIGTERM/SIGINT hlavnym procesom,
// posle signal na ukoncenie vsetkym potomkom, 
// uvolni alokovane zdroje a nasledne sa sam ukonci 
// s navratovým kódom 2.
void armagedon() {
    killpg(getpgrp(), SIGUSR1);
    sharedResourcesFree();
    exit(EXIT_SIG_FAIL);
}

// No operation funkcia na obsluhu signalu SIGUSR1 pre
// hlavny proces
void noop() {
    return;
}

// Otestuje ci retazec, ktory preberie ako parameter je mozne previest
// na cislo a na zaklade stavu kontroluje ci je vysledok prevodu
// v intervale pre dany tym argumentu programu, ak su obidve podmienky
// splnene navratova hodnota je 1, v pripade  ze nie, 
// navratova hodnota je 0.
int testNum(char *x, int state) {
    char *ptr;
    int retVal = strtol(x, &ptr, 10);
    switch(state) {
        case number:
            if (( *ptr == '\0') && retVal > 0)  
                return 1;
            break;
        case timeInt:
            if (( *ptr == '\0') && retVal >= 0 && retVal < 5001)
                return 1;
            break;
    }
    return 0;
}

// Spracovanie argumentov programu
int parseArguments(TParams *P, int argc, char **argv) {
    if (argc != 5) {
        fprintf(stderr, "Chyba: nespravny pocet argumentov.\n");
        return 1;
    }

    if (testNum(argv[1], number) && testNum(argv[2], timeInt) 
        && testNum(argv[3], timeInt) && testNum(argv[4], timeInt)) {
        P->N = strtol(argv[1], NULL, 10);
        P->GH = strtol(argv[2], NULL, 10);
        P->GO = strtol(argv[3], NULL, 10);
        P->B = strtol(argv[4], NULL, 10);
        return 0;
    } else {
        fprintf(stderr, "Chyba: nespravny format arumentov.\n");
        return 1;
    }
}
  
// Obsahuje kod hlavneho procesu
int main(int argc, char **argv) {
  
    TParams P = {.N = 1, .GH = 0, .GO = 0, .B = 0};

    if(parseArguments(&P, argc, argv))
        return EXIT_FAILURE;

    srand(time(NULL) * getpid());
    pid_t childPid = 0;
    signal(SIGTERM, armagedon);
    signal(SIGINT, armagedon);
    signal(SIGUSR1, noop);

    // Inicializacia zdielanych zdrojov
    if (sharedResourcesInit()) 
        return EXIT_SIG_FAIL;
    
    // Vytvori procesy pre tvorbu hydrogen a oxygen
    childPid = fork();
    if (childPid == 0) {
        newHydroMaker(&P);
    } else if (childPid == -1) {
         fprintf(stderr, "Chyba tvorby noveho procesu\n");
         armagedon();
    }         
    childPid = fork();
    if (childPid == 0) {
        newOxyMaker(&P);   
    } else if (childPid == -1) {
         fprintf(stderr, "Chyba tvorby noveho  procesu\n");
         armagedon();
    }

    // Cakanie na ukoncenie procesov hydroMaker a oxyMaker
    int status;
    wait(&status);
    if (status)
       armagedon();
    wait(&status);
    if (status)
       armagedon();
    // Uvolnenie zdielanych zdrojov
    if (sharedResourcesFree())
        return EXIT_SIG_FAIL;

    return EXIT_SUCCESS;
}
