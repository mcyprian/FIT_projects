// htable.h
// Riesenie IJC-DU2, priklad 2, 19. 4. 2015
// Autor: Michal Cyprian, FIT
// Prelozene: gcc 4.8.2
// Popis: Rozhranie kniznice libhtable.a 

// struktura polozky hashovacej tabulky
struct htab_listitem {
    char *key;
    unsigned data;
    struct htab_listitem *next;
};

// struktura tabulky
typedef struct {
    unsigned size;
    struct htab_listitem **list;
} htab_t;

// Hlavicky funkcii kniznice
unsigned int hash_function(const char *str, unsigned htab_size);
void freeItem(struct htab_listitem *item);
void htab_clear(htab_t *htable);
void htab_foreach(htab_t *htable, void (*function)(char *key, unsigned value));
void htab_free(htab_t *htable);
htab_t * htab_init(unsigned size);
struct htab_listitem * addWord(htab_t *htable, char *word);
struct htab_listitem * htab_lookup(htab_t *htable, char *key);
void htab_remove(htab_t *htable, char *key);
void htab_statistics(htab_t *htable);
