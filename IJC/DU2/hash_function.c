// hash_function.c
// Riesenie IJC-DU2, priklad 2, 18. 4. 2015
// Autor: Michal Cyprian, FIT
// Prelozene: gcc 4.8.2
// Popis: Rozptylovacia funkcia na urcenie indexu do tabulky.

unsigned int hash_function(const char *str, unsigned htab_size) {
    unsigned int h = 0;
    const unsigned char *p;

    for (p = (const unsigned char*)str; *p!='\0'; p++)
        h = 65599*h + *p;
    return h % htab_size;
}

