// Autor: Michal Cyprian Subor: test.c

#include <stdio.h>
#include "bit-array.h"
#include "error.h"
#include "error.c"

  //BA_create(globalArray, 100000L);
void printLongBinary(long number) {
    int i;
    unsigned long bit = (1lu << ((sizeof(long)) * 8 - 1));
    for (i = 0; i < 64; i++) {
        if (number & bit)
            printf("1");
        else
            printf("0");
        bit >>= 1;
    }
    printf("\n");
}

void printIntBinary(int number) {
    int i;
    int bit = 1;
    for (i = 0; i < 32; i++) {
        if (number & bit)
            printf("1");
        else
            printf("0");
        bit <<= 1;
    }
    printf("\n");
}
int main()
{
    BA_create(pole, 60 + 40);

//    FatalError("Niečo sa pokazilo...\n");

/*    char *p = &globalArray[1];
    int i;
    
    printf("global array:\n");
    printf("%lu\n", globalArray[0]);
    for(i = 0; i < 16; i++) {
        printf("%d, ", *p);
        p++;
    }
    printf("\n");
    printf("%lu\n", sizeof(globalArray) * 8 - sizeof(long) * 8);
*/
    printf("Size of array: %lu\n", BA_size(pole));
//    char *p = &pole[0]; 
   unsigned int i;

   for (i = 1; i < 65; i++) {
        if (i % 3 == 0)
            BA_set_bit(pole, i, 5);
    }


/*    printf("global array\n");
    printLongBinary(globalArray[1]);
    BA_set_bit(globalArray, 1, 1);
    printLongBinary(globalArray[1]);
    BA_set_bit(globalArray, 1, 0);

    if (BA_get_bit(globalArray, 1) == 1) printf("bit 1 == 1\n");
    else printf("bit 1 == 0\n");
    if (BA_get_bit(globalArray, 12) == 0) printf("bit 12 == 0\n");
*/
    printLongBinary(pole[1]);
    BA_set_bit(pole, 0lu, 1);
    printLongBinary(pole[1]);
    BA_set_bit(pole, 0, 0);
    printLongBinary(pole[1]);
    printLongBinary(pole[2]);
 //   printLongBinary(pole[2]);

    if (BA_get_bit(pole, 1)) printf("bit 1 == 1\n");
    else printf("bit 1 == 0\n");
    if (BA_get_bit(pole, 64)) printf("bit 64 == 1\n");
    else printf("bit 64 == 0\n");
    
/*    printf("%lu\n", pole[0]);
    for(i = 0; i < 16; i++) {
        printf("%d ", *p);
        p++;
    }
    printf("\n");

    unsigned long oneBit = 1;
    printLongBinary(oneBit);

    p = &oneBit;
    for(i = 0; i < 8; i++) {
        printf("%d ", *p);
        p++;
    }
    printf("\n");

    oneBit <<= 0;
    p = &oneBit;
    for(i = 0; i < 8; i++) {
        printf("%d ", *p);
        p++;
    }
    printf("\n");
*/
    printf("Bits allocated: %lu\n", sizeof(pole) * 8 - sizeof(long) * 8);

//   int a[8] = {-1, };
   char b[12] = {0, };

   printLongBinary(b[1]);
   printf("sizeof b[0] %d\n", sizeof(b[0]) * 8);
   
  if (DU1_GET_BIT_(b, 7))
      printf("bit 0 je rovný 1\n");
  else
      printf("bit 0 je rovný 0\n");
  DU1_SET_BIT_(b, 15, 1);
   printLongBinary(b[1]);
  if (DU1_GET_BIT_(b, 7))
      printf("bit 7 je rovný 1\n");
  else
      printf("bit 7 je rovný 0\n");
  for (i = 0; i < 5; i++) 
      printf("%i ", b[i]);


//FatalError("Nastala chyba %d pri alokacii %d %d pola\n", 42, 3, i);   
return 0;
}
