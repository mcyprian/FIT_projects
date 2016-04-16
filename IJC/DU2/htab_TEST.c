#include <stdio.h>
#include "htable.h"
#include "io.h"

void printItem(char *key, unsigned value) {
    printf("%s\t%d\n", key, value);
}


int main() {

    char *words[] = {"tvaroh", "jogurt", "musli", "palacinky", "mlieko",
        "jogurt", "bryndza", "banan"};

    htab_t *htable = htab_init(5);

    if (htable == NULL) {
        htab_free(htable);
        return 1;
    }
    for (unsigned i = 0; i < 8; i++) {
        if (htab_lookup(htable, words[i]) == NULL) {
            htab_free(htable);
            return 1;
        }
    }

    htab_foreach(htable, printItem);

    htab_statistics(htable);
    htab_remove(htable, "palacinky");
  
    htab_foreach(htable, printItem);

    htab_free(htable);

    // Testovanie fgetw()
    char s[7];
    FILE *f = NULL;
    f = fopen("input", "r");
    if (f == NULL) {
        fprintf(stderr, "Nepodarilo sa otvorit subor\n");
        return 1;
    }

    int retVal = fgetw(s, 6, f);
    if (retVal != EOF)
        printf("retVal %d s %s\n", retVal, s);
    else
        printf("EOF\n");
    retVal = fgetw(s, 5, f);
    if (retVal != EOF)
        printf("retVal %d s %s\n", retVal, s);
    else
        printf("EOF\n");
    fclose(f);

    return 0;
}
