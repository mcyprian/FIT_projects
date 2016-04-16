// bit-array.h
// Riešenie IJC-DU1, príklad a), 2. 3. 2015
// Autor: Michal Cyprian, FIT
// Preložené: gcc 4.9.2
// Popis: Deklaruje dátový typ makrá na prácu s bitovým polom,
//        definíciu a inicializáciu pola, kontrolu hodnoty a nastavovanie
//        bitov na zadaných indexoch. Na implementáciu využíva typ unsigned
//        long, obsahuje k makrám inline funkcie s rovnakou funkčnosťou
//        a funkcie na indexovanie a nastavovanie bitov v poli lubovolného
//        typu.

#include <stdio.h>
#include <limits.h>
#include "error.h"

#ifndef BIT_ARRAY_H
#define BIT ARRAY_H

typedef unsigned long  *BitArray_t;

// Konštanta pre veľkosť typu long v bitoch
#define _longBits (sizeof(unsigned long) * CHAR_BIT)

/* Pomocné makro na výpočet veľkosti pola typu unsigned long
 * potrebnú na vytvorenie bitového polavelkosti zadanej
 * parametrom size */
#define numOfLongs(size)                        \
     ((size) % _longBits == 0 ?                 \
      (size) / _longBits + 1 :                  \
      (size) / _longBits + 2)

/* Pomocné makro na výpočet indexu pola unsigned long v ktorom 
 * sa nachádza bit so zadaným indexom, vrátený index je posunutý
 * o 1 pretože v poli  BitArray_t je na indexe 0 velkosť pola.
 * index + 1 koli indexovaniu od 0*/
#define segIndex(index, typeSize)                   \
    ((index + 1) % (typeSize) == 0 ?                \
     (index + 1) / (typeSize) :                     \
     (index + 1) / (typeSize) + 1)

/* Vytvorí pole bitov pomocou pola unsigned long potrebnej velkosti,
 * prvý long obsahuje veľkosť pola v bitoch, ostatné sa inicialuzujú
 * na 0 */
#define BA_create(name, size)                                   \
     unsigned long  name[numOfLongs(size)] = {size, 0}  

/* Pomocné makro na získanie bitu na indexe v poli lubovolného
 * celočíselného typu, výsledok segIndex posunutý o -1, pretože
 * tu indexujeme od name[0] na rozdiel od pola BitArray_t */
#define DU1_GET_BIT_(name, index)                                        \
        (((name)[segIndex((index), (sizeof((name)[0]) * CHAR_BIT)) - 1]  \
         & (1lu << ((sizeof((name)[0]) * CHAR_BIT) - (index)             \
                    % (sizeof((name)[0]) * CHAR_BIT) -1)))? 1 : 0)

/* Pomocné makro nastaví bit na indexe v poli lubovolného typu */
#define DU1_SET_BIT_(name, index, value)                        \
    do {                                                        \
        if (value) {                                            \
            ((name)[segIndex((index), (sizeof((name)[0]) * CHAR_BIT)) - 1]\
            |= (1lu << ((sizeof((name)[0]) * CHAR_BIT) - (index)          \
                    % (sizeof((name)[0]) * CHAR_BIT) -1)));               \
        }  else {                                                         \
            ((name)[segIndex((index), (sizeof((name)[0]) * CHAR_BIT)) - 1]\
            &= ~(1lu << ((sizeof((name)[0]) * CHAR_BIT) - (index)         \
                    % (sizeof((name)[0]) * CHAR_BIT) -1)));               \
        }                                                                 \
    } while (0)

#ifndef USE_INLINE

// Vráti veľkosť pola v bitoch, uloženú na prvom indexe pola
#define BA_size(name)                                 \
    (name)[0]

/* AK je value != 0 nastaví bit so zadaným indexom na 1, ak nie
 * vynuluje ho, pri nulovaní zneguje celý long na príslušnom indexe, 
 * nastaví požadovaný bit na 1, opakovaná negácia bit vynuluje, ostatné
 * sa vrátia na pôvodnú hodnotu */ 
#define BA_set_bit(name, index, value)                             \
    do {                                                           \
        if ((index) < 0 || (index) >= (name)[0])                   \
            FatalError("Index %ld mimo rozsah 0..%ld",             \
                       (long)(index), (long)(name)[0]);            \
        else {                                                     \
            if(value) {                                            \
               (name)[segIndex((index), _longBits)]                \
               |= (1lu << (_longBits - (index) % _longBits -1));   \
            } else {                                               \
               (name)[segIndex((index), _longBits)]                \
               &= ~(1lu << (_longBits - (index) % _longBits -1));  \
            }                                                      \
        }                                                          \
    } while(0)

/* Zisťuje hodnotu bitu na indexe zadanom parametrom pomocou
 * & s bitovou maskou */
#define BA_get_bit(name, index)                                 \
        (((index) < 0 || (index) >= (name)[0]) ?                \
            FatalError("Index %ld mimo rozsah 0..%ld",          \
                       (long)(index), (long)(name)[0]),-1  :    \
        ((name[segIndex(index, _longBits)]                      \
         & (1lu << (_longBits - (index) % _longBits -1)))       \
         ? 1 : 0))                                         

#else /* !USE_INLINE */

// inline varianty funkcií
static inline unsigned long BA_size(unsigned long *name) {
    return name[0];
}

static inline void BA_set_bit(unsigned long *name, unsigned long index, int value) {
    if (index < 0 || index >= name[0])
        FatalError("Index %ld mimo rozsah 0..%ld",       \
                  (long)index, (long)name[0]);
    else {
        if (value) { 
            name[segIndex(index, _longBits)] 
            |= (1lu << (_longBits - index % _longBits -1)); 
            } else { 
                name[segIndex(index, _longBits)] 
                &= ~(1LU << (_longBits - index % _longBits -1));
        } 
    }
}    

static inline unsigned BA_get_bit(unsigned long *name, unsigned long index) {
    if (index < 0 || index >= name[0]) {
        FatalError("Index %ld mimo rozsah 0..%ld",      \
                  (long)index, (long)name[0]);
        return -1;
    }
    return ((name[segIndex(index, _longBits)]   
            & (1lu << (_longBits - index % _longBits -1))) ? 1 : 0); 
}

#endif /* !USE_INLINE */
#endif /* !BIT_ARRAY_H */
