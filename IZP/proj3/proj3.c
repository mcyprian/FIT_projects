/*
 * Subor:   proj3.c
 * Datum:   2014/11/8
 * Autor:   Michal Cyprian, xcypri01@stud.fit.vutbr.cz
 * Projekt: Průchod bludištěm, projekt č. 3 pre predmet IZP
 * Popis:   Program hľadá cestu von z bludiska
 *          zadaného v textovom súbore. Výstupom je výpis
 *          súradníc políčok bludiska cez ktoré vedie cesta
 *          od vchodu do bludiska k východu.
 */
 
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <stdbool.h>
#include <string.h>

enum states {S_HELP, S_TEST, S_RPATH, S_LPATH, S_SHORTEST}; // Stavové kódy programu

enum direction {LEFT, RIGHT, UP, DOWN};

// Chybové kódy programu
enum ecodes {E_OK, E_PARAMSFORMAT, E_PARAMSNUMBER, E_FILEOPEN, E_FILECLOSE,
	         E_ALLOC, E_STARTPOSITON, E_STARTBORDER, E_CLOSED, E_NOTNUMBER, E_BIGGERMAP, E_SMALLERMAP, E_INVALID, E_UNKNOW};

const char *ECODEMSG[] = 
{ // E_OK
    "Všetko v poriadku.\n",
  // E_PARAMSFORMAT
    "Neprípustný formát parametrov!\n",
  // E_PARAMSNUMBER
    "Neprípustný počet parametrov!\n",
  // E_FILEOPEN 
    "Nepodarilo sa otvoriť zadaný súbor!\n",
  // E_FILECLOSE
    "Nepodarilo sa zatvoriť zadaný súbor!\n",
  // E_ALLOC
    "Neúspešná alokácia pamäte!\n",
  // E_STARTPOSITON
    "Nesprávne zadané vstupné súradnice\n",
  // E_STARTBORDER
     "Na zadaných vstupných súradniciach sa nenachádza vchod do bludiska!\n",
  // E_CLOSED
     "Zo zadaných vstupných súradníc sa nedá nájsť cesta von!\n",
  // E_NOTNUMBER
    "V zadanom textovom súbore sa nachádza neceločíselná hodnota, alebo je jeden z rozmerov mapy 0!\n",
  // E_BIGGERMAP
    "Počet definícií buniek v zadanom textovom súbore prekračuje zadané rozmery obrázka!\n",
  // E_SMALLERMAP
    "Počet definícií buniek v zadanom textovom súbore nedosahuje zadané rozmery obrázka!\n",
  // E_INVALID
    "Zadaný súbor neobsahuje riadnu definíciu mapy bludiska!\n",
  // E_UNKNOW
    "Nastala neočakávaná chyba!\n" 
};

void printECode(int ecode) // Funkcia na výpis nastaveného chybového hlásenia.
{
	if (ecode < E_OK || ecode > E_UNKNOW)
	    ecode = E_UNKNOW;
	fprintf(stderr, "%s", ECODEMSG[ecode]);    
}

const char *HELPMSG =
    "\n                       Program Průchod bludištěm\n"
    "Autor Michal Cyprian 2014\n"
    "Program hľadá cestu von z bludiska zadaného v textovom súbore.\n"
    "Výstupom je výpis súradníc políčok bludiska cez ktoré vedie\n"
    "cesta od vchodu do bludiska k východu.\n"
    "Podoby parametrov spustenia programu:\n"
    "--help: program vypíše nápovedu.\n"
    "--test subor.txt: program skontroluje, či sa v súbore\n"
    "zadanom druhým parametrom nachádza riadna definícia mapy bludiska.\n"
    "V prípade že počet definícií políčok zodpovedá rozmerom bludiska daným\n"
    "dvomi číselnými hodnotami an začiatku súboru, v súbore sa nachádzajú \n"
    "iba celočíselné údaje a susediace políčka majú rovnaký typ zdielanej\n"
    "hranice vypíše Valid, ak je porušená niektorá z týchto podmienok vypíše\n"
    "Invalid.\n"
    "--rpath R C bludiste.txt: Vypisuje súradnice políčok mapy zo súboru\n"
    "zadaného štvrtým parametrom od vchodu na súradniciach R C k východu z bludiska\n"
    "podľa pravidla pravej ruky.\n"
    "--lpath R C bludiste.txt: Hľadá cestu von z bludiska zadaného štvrtým argumentom\n"
    "so začiatočnými súradnicami R C podľa pravidla ľavej ruky.\n"
    "--shortest R C bludiste.txt: Hľadá najkratšiu cestu von z bludiska zadaného štvrtým argumentom\n"
    "so začiatočnými súradnicami R C.\n\n";
    

typedef struct {   // Štruktúra na spracovanie parametrov programu.
	int state;
	int ecode;
	int R, C;
	char *labyrinthFile;
} TParams;

typedef struct {    // Štruktúra na uloženie definície mapy bludiska.
	int rows;
	int cols;
	unsigned char *cells;
} Map;

int findPath(Map *map, TParams *P);
int findShortest(Map *map, TParams *P);

/* Otestuje či je možné previesť reťazec, ktorý preberie ako parameter na  celé číslo,
 *  v prípade že áno, návratová hodnota je 1, v prípade že nie, návraová hodnota je 0. */
int testNum(char *x)
{
	char *ptr;
	 if (((strtol(x, &ptr, 0) == 0 && *ptr != '\0') && (!(strtol(x, &ptr, 0) == 0 && *ptr == '\0'))) ||
	   (strtol(x, &ptr, 0) != 0 && *ptr != '\0'))
         return 0;    
     else
         return 1;
}

/* Spracovanie parametrov programu, v prípade, že sú počet a hodnoty
 * argumentov správne, vráti ich v štruktúre TParams, ak nie sú
 * správne nastaví chybový kód. */
TParams getParams(int argc, char **argv)
{
	TParams P = {.state = S_HELP, .ecode = E_OK, .R = 0, .C = 0, .labyrinthFile = NULL};
	
	switch (argc) {
		case 2: 
		    if ((strcmp(argv[1], "--help")) == 0) {
				P.state = S_HELP;
		    } else
		        P.ecode = E_PARAMSFORMAT;
		    break; 
		case 3:
		    if ((strcmp(argv[1], "--test")) == 0) {
				P.state = S_TEST;
				P.labyrinthFile = argv[2];  
			} else
			    P.ecode = E_PARAMSFORMAT;
			break;
		case 5:
		    P.labyrinthFile = argv[4];
		    if (testNum(argv[2]) && testNum(argv[3]) &&
		        strtol(argv[2], NULL, 0) > 0 && strtol(argv[3], NULL, 0) > 0) {
				P.R = strtol(argv[2], NULL, 0);
				P.C = strtol(argv[3], NULL, 0);
			} else 
			    P.ecode = E_PARAMSFORMAT;
		      
		    if ((strcmp(argv[1], "--rpath")) == 0)
				P.state = S_RPATH;
			else if ((strcmp(argv[1], "--lpath")) == 0)
			    P.state = S_LPATH; 
			else if ((strcmp(argv[1], "--shortest")) == 0)
			         P.state = S_SHORTEST; 
			else    
			   P.ecode = E_PARAMSFORMAT;
			break;
		default: 
		    P.ecode = E_PARAMSNUMBER;
		    break;
	}
	return P;      
}

void freeMap(Map *map)         // Uvolní alokovanú pamať
{
	if (map->cells != NULL) {
	    free(map->cells);
	    map->cells = NULL;
    }
}

/* Ak sú údaje v zadanom súbore správne vytvorí definíciu mapy bludiska
 *  v štruktúre map, v opačnom prípade nastaví chybový kód. */
Map makeMap(char *labyrinthFile, TParams *P)
{
	FILE *fr;
	Map map = {.rows = 0, .cols = 0, .cells = NULL};
	int c, i = 0, j = 0, index = 0;
	char word[128] = {'\0', };
	
	if ((fr = fopen(labyrinthFile, "r")) == NULL) {  
		P->ecode = E_FILEOPEN;
		return map; 
	} 
	
	while (j < 2) {              // Nastavenie hodnôt m.rows a m.cols podľa 1. a 2. čísla v dokumente.         
	    if((c = fgetc(fr)) == EOF) {
			P->ecode = E_INVALID;
			return map;
		}
	        
	    if (isspace(c) && word[0] == '\0') {
		    continue;
		}
		else if (isspace(c)) {   // Ak máme slovo a c je medzera, prevedieme slovo na číslo a priradíme m.rows/ m.cols.
	        word[i] = '\0';
	        if (!(testNum(word)) || strtol(word, NULL, 10) <= 0) {
		        P->ecode = E_NOTNUMBER;
		        fclose(fr);
		        fr = NULL;
				return map;
			}
			if (j == 0) {
			    map.rows = strtol(word, NULL, 0);
			    j++;
			    i = 0;
			    word[0] = '\0';
			} else {
			    map.cols = strtol(word, NULL, 0);
			    j++;
			}
		} else {               // Ak sme vo vnútri slova, pridáme jeho ďalší znak.
			word[i] = c;
			i++;
		}
	}
	
	if (P->state != S_TEST && P->state != S_SHORTEST) {
	    if (!(((P->R == map.rows || P->R == 1) || (P->C == map.cols || P->C == 1)) &&
	          P->R <= map.rows && P->C <= map.cols)) {   
	        P->ecode = E_STARTPOSITON;            // Ak bola zadaná štartovacia pozícia mimo obvodu bludiska    
	        fclose(fr);                         // nastaví chybový kód
	        fr = NULL;
	        return map;
		}
	}
	
	if (P->state == S_SHORTEST)
		if ((P->R > map.rows || P->R < 1) || (P->C > map.cols || P->C < 1)) {
				        P->ecode = E_STARTPOSITON;            // Ak bola zadaná štartovacia pozícia mimo bludiska    
	        fclose(fr);                                      // nastaví chybový kód
	        fr = NULL;
	        return map;
		}
				
	if ((map.cells = (unsigned char *) malloc(map.rows * map.cols)) == NULL) {  
		P->ecode = E_ALLOC;                // Alokácia priestoru pre maticu bludiska - počet
		fclose(fr);                      // riadkov * počet stĺpcov.
		fr = NULL;                           
		return map;                           
	}
	
	while (1) {            // Postupné spracovávanie číselných hodnôt zo súboru
		word[0] = '\0';
		i = 0;
		while ((c = fgetc(fr)) != EOF) {
			if ((isspace(c)) && word[0] == '\0') {
				continue;
			} else if (isspace(c))
			    break;
			word[i] = c;
			i++;
		}
		if (c == EOF)
		    break;  
		       
		word[i] = '\0';
		
		if (!(testNum(word))) {        // Ak sa v súbore nachádza neceločíselná hodnota
				P->ecode = E_NOTNUMBER;  // nastaví chybový kód.
				freeMap(&map);
				fclose(fr);
				fr = NULL;
				return map;
		}
		if ((index) > (map.rows * map.cols -1)) {   // Kontrola či v nasledujúcom cykle 
			P->ecode = E_BIGGERMAP;                // nepríde k zápisu mimo alokovaný priestor.
			freeMap(&map);
			fclose(fr);
			fr = NULL;
			return map;
		}
		
		map.cells[index] = strtol(word, NULL, 10) & 7;                // Uloženie posledných 3 bitov čísla v dekadickom tvare na 
		index ++;                                                     // príslušný index.	 
    }
   
    if ((index - 1) < (map.rows * map.cols - 1)) {  // Kontrola či počet zapísaných definícií buniek 
		P->ecode = E_SMALLERMAP;                      // nie je menší ako rozmery obrázka.
		freeMap(&map);
		fclose(fr);
		fr = NULL;
		return map;
	}
		
    if (fr != NULL)
	    if ((fclose(fr)) == EOF) {
	        freeMap(&map);
	        P->ecode = E_FILECLOSE;
	    }
	
	return map;
}

/* AK sa na políčku mapy map na súradniciach r c nachádza stena
 * vráti true, ak je tam cesta vráti false. */
bool isborder(Map *map, int r, int c, int border)
{
	switch (border) {
		case LEFT:
	        if (((map->cells[(r-1) * map->cols + (c - 1)]) &  1) == 1)
	            return true;
	            break;
	    case RIGHT:
	        if (((map->cells[(r-1) * map->cols + (c - 1)]) &  2) == 2)
	            return true;
	            break;
	    case UP:
	    case DOWN:
	        if (((map->cells[(r-1) * map->cols + (c - 1)]) &  4) == 4)
	            return true;
	            break;
	}
	return false;
}

/* Ak sa v štruktúre map nachádza riadna definícia mapy bludiska
 * vráti 0, v opačnom prípde nastaví chybový kód a  vráti 1. */ 
int validityTest(Map *map, int *ecode)
{
	int i, j;
	
	if ((*ecode >= E_NOTNUMBER) && (*ecode <= E_INVALID))
	    return 1;
	    
	for (j = 1; j <= map->rows; j++)         // Prechod celej bitovej mapy bludiska.
	    for (i = 1; i < map->cols; i++) {    // index stlpca iba po predposledný stlpec v riadku.
			if (isborder(map, j, i, RIGHT) != isborder(map, j, i + 1, LEFT)) {
				*ecode = E_INVALID;
				return 1; 
			}
			
		    if (i % 2 != j % 2)    // nepárny riadok, párny stlpec, alebo párny riadok, nepárny stlpec.
			    if ((j < map->rows) && isborder(map, j, i, DOWN) != isborder(map, j + 1, i, UP)) {
					*ecode = E_INVALID;
					return 1;
				}
			}
	return 0;				
} 

/* Určí ktorá stena sa bude nasledovať na základe štartovacej
 * pozície na súradniciach r, c v závislosti od zvoleného pravidla,
 * ak sa na zadaných súradniciach nenachádza vchod nastaví chyboví kód a vráti -1. */
int start_border(Map *map, int r, int c, int leftright)
{
	if (leftright == S_RPATH) {       // Pravidlo pravej ruky
		if (r == 1 && c == 1) {             // Ak je štartovacia pozicia v rohu mapy, zistí
		    if (!isborder(map, 1, 1, LEFT)) // ktorá stena je priechodná, podľa toho vyberie
		        return RIGHT;               // stenu.
		    if (!isborder(map, 1, 1, UP))
		        return LEFT;
		}
		if (r == 1 && c == map->cols) {
		    if (!isborder(map, 1, map->cols, RIGHT)) {
		        if (c % 2)
		            return UP;
		        else
		            return LEFT;
			}
		    if (!isborder(map, 1, map->cols, UP))
		        return LEFT;
		}
        if (r == map->rows && c == 1) {
		    if (!isborder(map, map->rows, 1, LEFT)) {
				if (map->rows % 2)
				    return RIGHT;
				else
				    return DOWN;
				}
		    if (!isborder(map, map->rows, 1, DOWN))
		        return RIGHT;
		}	
		if (r == map->rows && c == map->cols) {
		    if (!isborder(map, map->rows, map->cols, RIGHT)) {
				if (map->rows % 2 == map->cols % 2)
				    return UP;
				else
				    return LEFT;
				}
		    if (!isborder(map, map->rows, map->cols, DOWN))
		        return RIGHT;
		}	
		if (r == 1) {
			if ((c % 2 != r % 2) || isborder(map, r, c, UP))               // Vchod zhora
			    return -1;
		    return LEFT;
		}
		if (r == map->rows) {
			if ((c % 2 == r % 2) || isborder(map, r, c, DOWN))              // Vchod zdola
			    return -1; 
		    return RIGHT;
		}
		if ((c == 1) && (r % 2)) {
			if (isborder(map, r, c, LEFT))             // Zlava na nepárnom riadku
		        return -1;
		    return RIGHT;
		}
		else if (c == 1) {
			if (isborder(map, r, c, LEFT))
			    return -1;                          // Zlava na párnom riadku
		    return DOWN;
		}
		if ((c == map->cols) && (r % 2 == c % 2)) {
			if (isborder(map, r, c, RIGHT)) 
			    return -1;                       // Zprava na párnom riadku
		    return UP;
		}
		else if (c == map->cols) {
			if (isborder(map, r, c, RIGHT))
			    return -1;                  // Zprava na nepárnom riadku
		    return LEFT;
		}
		    
	} else if (leftright == S_LPATH){   // Pravidlo lavej ruky
		if (r == 1 && c == 1) {
		    if (!isborder(map, 1, 1, LEFT))
		        return UP;
		    if (!isborder(map, 1, 1, UP))
		        return RIGHT;
		}
		if (r == 1 && c == map->cols) {
		    if (!isborder(map, 1, map->cols, RIGHT)) {
		        if (c % 2)
		            return LEFT;
		        else
		            return DOWN;
			}
		    if (!isborder(map, 1, map->cols, UP))
		        return RIGHT;
		}
        if (r == map->rows && c == 1) {
		    if (!isborder(map, map->rows, 1, LEFT)) {
				if (map->rows % 2)
				    return UP;
				else
				    return RIGHT;
				}
		    if (!isborder(map, map->rows, 1, DOWN))
		        return LEFT;
		}	
		if (r == map->rows && c == map->cols) {
		    if (!isborder(map, map->rows, map->cols, RIGHT)) {
				if (map->rows % 2 == map->cols % 2)
				    return LEFT;
				else
				    return DOWN;
				}
		    if (!isborder(map, map->rows, map->cols, DOWN))
		        return LEFT;
		}	
		if (r == 1) {
			if ((c % 2 != r % 2) || isborder(map, r, c, UP))
			    return -1;                               // Vchod zhora
		    return RIGHT;
		}
		if (r == map->rows) {
			if ((c % 2 == r % 2) || isborder(map, r, c, DOWN))              // Vchod zdola
			    return -1;
		    return LEFT;  
		}                        
		if ((c == 1) && (r % 2)) {
		    if (isborder(map, r, c, LEFT))             // Zlava na nepárnom riadku
		        return -1;                 
		    return UP;
		}
		else if (c == 1) {
			if (isborder(map, r, c, LEFT))
			    return -1;                          // Zlava na párnom riadku
		    return DOWN;
		}
		if ((c == map->cols) && (r % 2 == c % 2)) {
			if (isborder(map, r, c, RIGHT)) 
			    return -1;                       // Zprava na párnom riadku
		    return LEFT;
		}
		else if (c == map->cols) {
			if (isborder(map, r, c, RIGHT))
			    return -1;                  // Zprava na nepárnom riadku
		    return DOWN;
		}
	}
		    
    return -1;	
}


int main(int argc, char **argv)
{
    TParams P = getParams(argc, argv);
    
    if (P.ecode != E_OK && (P.ecode < E_NOTNUMBER || P.ecode > E_INVALID)) {
		printECode(P.ecode);
		return EXIT_FAILURE;
	}
	
	if (P.state == S_HELP) {
		printf("%s", HELPMSG);
		return EXIT_SUCCESS;
	}
	
	Map map = makeMap(P.labyrinthFile, &P);
	
	if (P.ecode != E_OK && (P.ecode < E_NOTNUMBER || P.ecode > E_INVALID)) {
		printECode(P.ecode);
		return EXIT_FAILURE;
	}

	switch (P.state) {
		case S_TEST:
		    if (validityTest(&map, &P.ecode)) 
		        printf("Invalid\n");
            else
                printf("Valid\n");
	        break;
	   case S_RPATH:
	   case S_LPATH:  
	       if (validityTest(&map, &P.ecode)) {
		       freeMap(&map);
			   printECode(P.ecode);
			   return EXIT_FAILURE;
		   }
		   if (findPath(&map, &P)) {
		       freeMap(&map);
			   printECode(P.ecode);
			   return EXIT_FAILURE;
		   }
	       break;
	   case S_SHORTEST: 
	       if (validityTest(&map, &P.ecode)) {
				freeMap(&map);
				printECode(P.ecode);
				return EXIT_FAILURE;
		   }
		   if(findShortest(&map, &P)) {
			   printECode(P.ecode);
			   freeMap(&map);
			   return EXIT_FAILURE;
		   }
		   break;	    
	   default:
	       printECode(E_UNKNOW);
	       return EXIT_FAILURE;    
	} 
	        
	freeMap(&map);
	
	return EXIT_SUCCESS;
}

/* Výpíše pozíciu, podľa smeru pohybu, inkrementuje resp. dekrementuje
 * príslušnú súradnnicu a v závislosti od zvoleného pravidla
 * nastaví hranicu, ktorá sa má ďalej nasledovať. */
void move(int *x, int *y, int *direction, int leftright)
{
	printf("%d,%d\n", *x, *y);
	if (leftright == S_RPATH) {
	    switch (*direction) {
		    case LEFT:
		        (*y)--;
		        if (*x % 2 == *y % 2)
		            *direction = UP;
		        break;
		    case RIGHT:
		        (*y)++;
		        if (*x % 2 != *y % 2)
		            *direction = DOWN;		    
		        break;
		    case UP:
		        (*x)--;
		            *direction = RIGHT;		    
		        break;
		     case DOWN:
		        (*x)++;
		           *direction = LEFT;
		        break;
		    }
	} else if (leftright == S_LPATH) {
			    switch (*direction) {
		    case LEFT:
		        (*y)--;
		        if (*x % 2 != *y % 2)
		            *direction = DOWN;
		        break;
		    case RIGHT:
		        (*y)++;
		        if (*x % 2 == *y % 2)
		            *direction = UP;		    
		        break;
		    case UP:
		        (*x)--;
		            *direction = LEFT;		    
		        break;
		     case DOWN:
		        (*x)++;
		           *direction = RIGHT;
		        break;
		    }
	 }
}

/* Funkcia mení prioritnú border kým nenájde volnú cestu, 
 * presunie sa na nasledujúce políčko v danom smere,
 * kým nenájde cestu von. */
int findPath(Map *map, TParams *P)
{
	int x = P->R, y = P->C;
	int border = start_border(map, x, y, P->state);
	
	if (border == -1) {
		P->ecode = E_STARTBORDER;
		return 1;
	}
	
	while (x >= 1 && x <= map->rows && y >= 1 && y <= map->cols) {  // Ak sme našli cestu von cyklus končí
	    while (isborder(map, x, y, border)) {   // Ak je v smere priority stena, 
            switch(P->state) {                  // nastaví sa nasledujúca najvyššia priorita
				case S_RPATH:                   // v závislosti od nastaveného pravidla
                    if (x % 2 == y % 2) {        
		                if (border == UP)
			            border = LEFT;
		            else 
			            border++;
			        continue;   
	            } else {
		            if (border == DOWN)
			            border = RIGHT;
			        else if (border == LEFT)
			            border = DOWN;
			             else
			             border--;
		            continue;	
		          }
                  break;
                case S_LPATH:  
                    if (x % 2 == y % 2) {
		                if (border == LEFT)
			            border = UP;
		            else 
			            border--;
			        continue;   
	            } else {
		            if (border == DOWN)
			            border = LEFT;
			        else if (border == RIGHT)
			            border = DOWN;
			             else
			             border++;
		            continue;	
		          } 
		          break;               	        
	        }  
		}        
	    move(&x, &y, &border, P->state);
    }
    return 0;   
}

int getDistance(Map *mask, int r, int c)    // Vráti ohodnotenie políčka na zadaných súradniciach
{
	return mask->cells[(r - 1) * mask->cols + (c - 1)];
}

bool isInRange(Map *mask, int r, int c)  // Ak existuje políčko so zadanými súadnicami vráti true
{
    if (r <= mask->rows && r > 0 && c <= mask->cols && c > 0)
        return true;
    return false;    
}

bool isBigger(Map *mask, int r, int c, int distance)         // Ak políčko na zadaných súradniciach malo vaščie ohodnotenie
{                                                            // ako hodnota distance vráti true.
	if (mask->cells[(r - 1) * mask->cols + (c - 1)] > distance)
	    return true;
	return false;    
}

void initMask(Map *mask)         // Inicializuje masku.
{
	int i;
	
	for (i = 0; i < mask->rows * mask->cols; i++)
	    mask->cells[i] = 255;    
}

void fillMask(Map *map, Map *mask, int r, int c, int distance) // Rekurzívna funkcia postupne vypĺńa ohodnotenia všetkých
{                                                              // neizolovaných políčok mapy.
	mask->cells[(r - 1) * mask->cols + (c - 1)] = distance;
	
	if (r % 2 == c % 2) {
		if (isInRange(mask, r - 1, c) && isBigger(mask, r - 1, c, distance + 1) && (!isborder(map, r, c, UP)))
		    fillMask(map, mask, r - 1, c, distance + 1);
	} else {
		if (isInRange(mask, r + 1, c) && isBigger(mask, r + 1, c, distance + 1) && (!isborder(map, r, c, DOWN)))
		    fillMask(map, mask, r + 1, c, distance + 1);
	}
		
	if (isInRange(mask, r, c - 1) && isBigger(mask, r, c - 1, distance + 1) && (!isborder(map, r, c, LEFT))) 	    
		    fillMask(map, mask, r, c - 1, distance + 1);
    if (isInRange(mask, r, c + 1) && isBigger(mask, r, c + 1, distance + 1) && (!isborder(map, r, c, RIGHT))) 
		    fillMask(map, mask, r, c + 1, distance + 1);
} 

int findExit(Map *map, Map *mask, int *r, int *c)    // Nájde východ z bludiska s najmenším ohodnotením.
{
	int min = 255;
	int i;
	
	for (i = 1; i <= map->cols; i++) {                     // Prechod spodnej hrany bludiska.
		if (i % 2 != map->rows % 2)
		    if(!(isborder(map, map->rows, i, DOWN)))
		        if (getDistance(mask, map->rows, i) < min) {
					min = getDistance(mask, map->rows, i);
					*r = map->rows;
					*c = i;
				}
   }
   for (i = 1; i <= map->rows; i++) {                     // Prechod pravej hrany bludiska.
	   if(!(isborder(map, i, map->cols, RIGHT)))
		   if (getDistance(mask, i, map->cols) < min) {
			   min = getDistance(mask, i, map->cols);
			   *r = i;
			   *c = map->cols;
		   }
   }
   for (i = 1; i <= map->cols; i++) {   
	   if (i % 2 == 1 % 2)                  // Prechod vrchnej hrany bludiska.
	       if(!(isborder(map, 1, i, UP)))
		       if (getDistance(mask, 1, i) < min) {
			       min = getDistance(mask, 1, i);
				   *r = 1;
				   *c = i;
			   }
   } 
   for (i = 1; i <= map->rows; i++) {                     // Prechod lavej hrany bludiska.
	   if(!(isborder(map, i, 1, LEFT)))
		   if (getDistance(mask, i, 1) < min) {
			   min = getDistance(mask, i, 1);
			   *r = i;
			   *c = 1;
		   } 	       
   }
   return min;
}

void printShortest(Map *mask, int r, int c) // Rekurzívna funkcia, vypíše najkratšiu cestu spätnou kontrolou ohodnotení políčok
{
	int distance = getDistance(mask, r, c);
	
	if (distance != 0) {		
        if ((r % 2 == c % 2) && isInRange(mask, r - 1, c) && (getDistance(mask, r - 1, c) == distance - 1)) // Jedna z ciest vedie hore
		    printShortest(mask, r - 1, c);     
        else if ((r % 2 != c % 2) && isInRange(mask, r + 1, c) && (getDistance(mask, r + 1, c) == distance - 1))  // Jedna z ciest vedie dole
	        printShortest(mask, r + 1, c);
	         else if (isInRange(mask, r, c - 1) && (getDistance(mask, r, c - 1) == distance - 1))
		         printShortest(mask, r, c -1);
	              else if (isInRange(mask, r, c + 1) && (getDistance(mask, r, c + 1) == distance - 1))
		              printShortest(mask, r, c + 1);
	}
	printf("%d,%d\n", r, c);	    
}

int findShortest(Map *map, TParams *P)   // Volá čiastkové funkcie na nájdenie najkratšej cesty z bludiska.
{
	int i = 0, j = 0;
	Map mask = makeMap(P->labyrinthFile, P);
    initMask(&mask); 
    fillMask(map, &mask, P->R, P->C, 0);
    findExit(map, &mask, &i, &j);
    if (i == 0 || j == 0) {
		P->ecode = E_CLOSED;
		freeMap(&mask);
		return 1;
	}
    printShortest(&mask, i, j);	
    freeMap(&mask); 
    return 0;  
}
