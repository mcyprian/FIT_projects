/*
 * Subor:   proj1.c
 * Datum:   2014/10/23
 * Autor:   Michal Cyprian, xcypri01@stud.fit.vutbr.cz
 * Projekt: Výpočty v tabulce, projekt č. 1 pre predmet IZP
 * Popis:   Program vykonáva funkcie minima, maxima, sumy a aritmetického priemeru 
 *          vybraných buniek tabuľky, ktorú očakáva na štandartnom vstupe.
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <stdbool.h>

/* Deklarácia enumerických premenných pre výber operácie a výberu. */
enum toperacia {select, min, max, sum, avg};
 
enum tvyber {row, col, rows, cols, range};

/* Chybové kódy programu. */
enum tecodes {EOK, EOPER, EVYB, EPPARAM, EFPARAM, EPA, ECOUNTCHAR, EEMPTYSEL, ELINELENGTH, EUNKNOW};

/* Chybové hlásenia, zodpovedajúce chybovým kódom. */
const char *ECODEMSG[] =
{
	[EOK] = "Všetko v poriadku.\n",
	[EOPER] = "Neprípustný parameter operácie!\n",
	[EVYB] = "Neprípustný parameter výberu!\n",
	[EPPARAM] = "Neprípustný počet parametrov pre zadaný výber!\n",
	[EFPARAM] = "Neprípustný formát číselného parametru!\n",
	[EPA] = "Neprípustný počet argumentov!\n", 
	[ECOUNTCHAR] = "Operácie min, max, sum, avg nepracujú nad bunkami, obsahujúcimi nečíselné údaje!\n",
	[EEMPTYSEL] = "Na zadanom výbere sa v tabuľke nenachádzajú žiadne prvky!\n",
	[ELINELENGTH] = "Prekročená maximálna dĺžka riadku!\n",
	[EUNKNOW] = "Nastala nepredvídaná chyba!\n",
};

const char *HELPMSG =
   "\n                       Program Výpočty v tabulce\n"
   "Autor Michal Cyprian 2014\n"
   "Program vykonáva funkcie minima, maxima, sumy a aritmetického priemeru\n" 
   "vybraných buniek tabulky, ktorú očakáva na štandartnom vstupe.\n"
   "Podoby parametrov spustenia programu:\n"
   "--help   vypíše nápovedu programu.\n"
   "operácia výber_buniek: vykoná zadanú operáciu nad vybranými bunkami tabuľky\n"
   "Možnosti výberu operácie:\n"
   "select  vypíše vybrané bunky tabuľky.\n"
   "min  vypíše minimum z vybraných buniek tabuľky.\n"
   "max  vypíše maximum z vybraných buniek tabuľky.\n"
   "sum  vypíše sumu vybraných buniek tabuľky.\n"
   "avg  vypíše aritmetický priemer vybraných buniek tabuľky.\n" 
   "Možnosti výberu buniek:\n" 
   "row X      výber všetkých buniek na riadku X.\n"
   "col X      výber všetkých buniek v stĺpci X.\n"
   "rows X  Y  výber všetkých buniek od riadku X po riadok Y vrátane.\n"
   "cols X  Y  výber všetkých buniek od stĺpca X po stĺpec Y vrátane.\n"
   "range A B X Y  výber všetkých buniek od riadku A po riadok B\n"
   "a od stĺpca X po stĺpec Y vrátane.\n\n";
   
/* Štruktúra obsahujúca hodnoty parametrov príkazového riadku a počítadlá */
typedef struct
{
	unsigned short N;
	int ecode;
	int operacia;
	int vyber;
	unsigned short a, b, x, y;
	int lineNum, wordNum, ordNum;
	double res;
	bool help;
	
} TParams;

/* Definícia nového typu ukazatel na funciu, parametre 
 * zodpovedajú parametrom funkcií fselect, fminmax a fsumavg. */
typedef void (*foo)(TParams *, char *);

/* Deklarácie funkcií, ktoré budú vykonávať jednotlivé operácie a pomocných funkcií. */
void fselect(TParams *P, char *slovo);
void fminmax(TParams *P, char *slovo);
void fsumavg(TParams *P, char *slovo);
void vypisRes(TParams *P);
int prechodTabulky(TParams *P);

/* Konštanta pre maximálnu dĺžku reťazca. */
const int MAXLINELENGTH = 1024;

/* Funkcia na výpis vypraného chybového hlásenia na stderr. */
void printECode(int ecode)
{
	if (ecode < EOK || ecode > EUNKNOW)
       ecode = EUNKNOW;
      
    fprintf(stderr, "%s", ECODEMSG[ecode]);
}

/* Funkcia na spracovanie parametrov programu. Funkcia preberie parametre, 
 * skontroluje výber operácie, výberu a počet parametrov v závislosti od výberu, 
 * v prípade potreby nastaví príslušný chybový kód. */
TParams getParams(int argc, char* argv[])
{
	TParams P =
    {
		.N = argc,
        .ecode = EOK,
        .lineNum = 0,
        .wordNum = 0,
        .ordNum = 0,
        .help = false,
        .a = 0, .b = 0, .x = 0, .y = 0, 
    };
    char *ptr;
 
/* Ak je počet parametrov neprípustný, nastaví chybový kód a vráti P. */   
   switch (P.N)
   {
	   case 2:
	       if (strcmp("--help", argv[1]) == 0)     
	       {
               P.help = true;
               return P;
		   }
           else
           {
               P.ecode = EOPER;
               return P;
           }
           break;
       case 4:
	       if (strtol(argv[3], &ptr, 10) <= 0 || *ptr != '\0')
	           P.ecode = EFPARAM;
	       else
	           P.x = strtol(argv[3], NULL, 10); 
	       break;  
	   case 5:
	       if ((strtol(argv[3], &ptr, 10) <= 0 || *ptr != '\0') || 
	          (strtol(argv[4], &ptr, 10) < strtol(argv[3], NULL, 10) || *ptr != '\0'))
	           P.ecode = EFPARAM;
	       else
	       {
	           P.x = strtol(argv[3], NULL, 10); 
	           P.y = strtol(argv[4], NULL, 10);
		   }      
           break;
	   case 7:
	       if ((strtol(argv[3], &ptr, 10) <= 0 || *ptr != '\0') || 
	          (strtol(argv[4], &ptr, 10) < strtol(argv[3], NULL, 10 || *ptr != '\0')))
	             P.ecode = EFPARAM;
	       else if ((strtol(argv[5], &ptr, 10) <= 0 || *ptr != '\0') || 
	               (strtol(argv[6], &ptr, 10) < strtol(argv[5], NULL, 10 || *ptr != '\0')))
	               P.ecode = EFPARAM;
	           else
	           {           
                   P.a = strtol(argv[3], NULL, 10);
				   P.b = strtol(argv[4], NULL, 10);
                   P.x = strtol(argv[5], NULL, 10);
                   P.y = strtol(argv[6], NULL, 10);
	           } 
           break;
       default:
          P.ecode = EPA;
          return P;
          break;
   }
	   
/* Nastavenie operácie podľa reťazca arg[1], 
 * v prípade zle zadaného argumentu nastaví chybové hlásenie. */    
   if (strcmp("select", argv[1]) == 0)
       P.operacia = select;	   
   else if (strcmp("min", argv[1]) == 0)
	        P.operacia = min;
        else if (strcmp("max", argv[1]) == 0)
                 P.operacia = max;
             else if (strcmp("sum", argv[1]) == 0)
			          P.operacia = sum;
			      else if (strcmp("avg", argv[1]) == 0)
				           P.operacia = avg;
				       else
				       {
			               P.ecode = EOPER;
			               return P;
		               }
/* Nastavenie výberu podľa druhého argumentu, prípadné nastavenia chybového hlásenia. */
   if (strcmp("row", argv[2]) == 0)
       {
	       P.vyber = row;
	       if (P.N != 4)
	           P.ecode = EPPARAM;
	   }
   else if (strcmp("col", argv[2]) == 0)
        {
            P.vyber = col;
            if (P.N != 4)
                P.ecode = EPPARAM;
	    }
        else if (strcmp("rows", argv[2]) == 0)
		     {
                 P.vyber = rows;
                 if (P.N != 5)
                    P.ecode = EPPARAM;
		     }
             else if (strcmp("cols", argv[2]) == 0)
			      {
                      P.vyber = cols;
                      if (P.N != 5)
                         P.ecode = EPPARAM;
			      }
              else if (strcmp("range", argv[2]) == 0)
			       {
                       P.vyber = range;
                       if (P.N != 7)
                          P.ecode = EPPARAM;
			       }
                   else
                   {
				       P.ecode = EVYB;  
				       return P;   
				   }
    return P;
}				   

 
int main(int argc, char* argv[])
{	
/* Volanie funkcie na spracovanie argumentov, programu. Získaná štruktúra sa uloží do premennej P. */
    TParams P = getParams(argc, argv);
 
/* V prípade ľubovolnej chyby sa vypíše chybové hlásenie a program sa ukončí,
 * v prípade spustenia s argumentom "-h" vypíše help. */  
    if (P.ecode != EOK)
    {
	    printECode(P.ecode);
	    return EXIT_FAILURE;
    }
   
    if (P.help == true)
    {
	    printf("%s", HELPMSG);
	    return EXIT_SUCCESS;
    }
	
    if (prechodTabulky(&P) != 0)         // Kontrola návratovej hodnoty ak došlo k nejakej chybe
    {                                    // vypíše sa chybový kód a program sa ukončí.
        printECode(P.ecode);
	    return EXIT_FAILURE;
    }
    
	 vypisRes(&P);

     return EXIT_SUCCESS;
}

/* Priradí ukazateľu funkciu podľa hodnoty premennej operácia, 
 * nastavenej na základe argumentov príkazového riadku. */   
foo switchFunction(TParams *P)
{
   foo pFun;

   switch (P->operacia)
   {  
       case select:
          pFun = &fselect;
          break;
       case min:
       case max:
          pFun = &fminmax;
          break;
       case sum:
       case avg:
          pFun = &fsumavg;
          break;
       default:
           P->ecode = EUNKNOW;
           break;      
   }
   return pFun;
}

/* V prípade, že pri výbere col, cols alebo range chýba vo vybranom stĺpci hodnota, nastaví chybový kód. */
void columnTest(TParams *P, int sirkaTab)
{
	switch(P->vyber)
	{
	    case cols:
	    case range:
		    if ((P->lineNum >= 2) && ((P->y > sirkaTab) || ((P->y == sirkaTab) && (P->wordNum < sirkaTab))))
		    {
			    P->ecode = EEMPTYSEL;
		    }
		    break;          
		 case col:
		     if ((P->lineNum >= 2) && ((P->x > sirkaTab) || ( (P->x == sirkaTab)&& (P->wordNum < sirkaTab))))
		     {
			    P->ecode = EEMPTYSEL;
		     }  
		     break;
		 default:;
   }
}

/* Skontroluje, či nebol zadaný parameter, ktorý prekračuje počet riadkov tabuľky. */
void rowTest(TParams *P, int vyska)
{
	switch (P->vyber)
	{
		case row:
		   if (vyska < P->x)
		      P->ecode = EEMPTYSEL;
		   break;
		case rows:
		   if (vyska < P->y)
		      P->ecode = EEMPTYSEL;
		   break;
		case range:
		   if (vyska < P->b)
		      P->ecode = EEMPTYSEL;
		   break;
		default:;
	}
}

/* Načíta 1 riadok zo stdin, vyhodí prípadné biele znaky na začiatku reťazca
 *  a viacnásobné biele znaky uprostred reťazca. */
int nacitajRiadok(char* riadok)
{
	int c = 0, n = 0, m = 0, medzera = 0;
	char vstupRiadok[MAXLINELENGTH];
	
	while (((c = getchar()) != '\n') && (c != EOF))
	{
	    vstupRiadok[n] = c; 
		n++;
    }
    
    if (n >= MAXLINELENGTH)
		return 1;
	   
	if (c == EOF)
        return EOF;
	   
	vstupRiadok[n] = '\0';
	n = 0;
	   
	while ((vstupRiadok[n] != '\0') && (isspace(vstupRiadok[n])))     // Vynechá prípadné medzery na začiatku reťazca.
	{                                                                 
        n++;                                                          
	}
   
	while (vstupRiadok[n] != '\0')                                    // Prechádza vstupRiadok, v prípade, 
	{                                                                 // že nájde viac bielych znakov za sebou 
        if ((isspace(vstupRiadok[n])) && (medzera == 1))              // do reťazca riadok zapíše iba jednu medzeru.
	    {
	        n++;
		}
		else if (isspace(vstupRiadok[n]))
		{
	        riadok[m] = ' ';
			n++;
			m++;
			medzera = 1;
		}
		     else
		     {
		         riadok[m] = vstupRiadok[n];
				 n++;
				 m++;
				 medzera = 0;
			 }   		   
    }
    
	riadok[m] = ' ';
    riadok[m+1] = '\n';
	        
	return EXIT_SUCCESS;
}

/* Funkcia v prípade výberu operácie min, max, sum alebo avg vypíše P.res. */
void vypisRes(TParams *P)
{
	switch(P->operacia)
	{
		case min:
		case max:
		case sum:
		   printf("%.10g\n", P->res);
		   break;
		case avg:
		   P->res = P->res/P->ordNum;
		   printf("%.10g\n", P->res);
		   break;
		default:;
		    break;
	}
}

 /* Ak je aktuálne číslo riadku väčšie ako horná hranica výberu vráti 1, ináč vráti 0. */
int zastavit(TParams *P)
{
	switch(P->vyber)
	{
		case row:
		    if (P->lineNum > P->x)
		        return 1;
		    break;
		case rows:
		    if (P->lineNum > P->y)
		        return 1;
		    break;
		case range: 
		    if (P->lineNum > P->b)
		        return 1;
		    break;    
    }
return 0;        
}

/* Funkcia zistí, či predané slovo spadá do zvoleného výberu, v prípade, 
 * že áno zavolá funkciu príslušnej operácie. */
void testVyberu(TParams *P, char *slovo)
{	
	/* nastavenie ukazatela na funkciu. */   
    foo pFun = switchFunction(P);
    
	switch(P->N)
	{
		case 4 :
	        switch (P->vyber)
	        {      
                case row:
                    if (P->lineNum == P->x)
                       pFun (P, slovo);
                    break;  
                case col:
		            if (P->wordNum == P->x)
		               pFun (P, slovo);
		            break;
		        default:;
		            break;  
            }
            break;
        case 5:
            switch (P->vyber)
	        {
		        case rows:
		            if ((P->lineNum >= P->x) && (P->lineNum <= P->y))
		                pFun (P, slovo);
		            break;	 
	            case cols:
		            if ((P->wordNum >= P->x) && (P->wordNum <= P->y))
		                pFun (P, slovo);
		            break;
		        default: 
		            break;  	
	       }
	       break;
	    case 7:
	        if ((P->lineNum >= P->a) && (P->lineNum <= P->b) && 
	           (P->wordNum >= P->x) && (P->wordNum <= P->y))
	            pFun (P, slovo);
	        break;
	    default: P->ecode = EUNKNOW;
	        break;
	}
}

/* Funkcia prechádza tabuľku po jednotlivých slovách a volá funkciu testVyberu. 
 * Ak prejde celú tabuľku bez problémov vráti hodnotu 0, pri chybe vráti 1. */
int prechodTabulky(TParams *P)
{
	int i, n = 0, k = 0, p = 0, sirkaTab = 0, vyskaTab = 0;
	char riadok[MAXLINELENGTH];
	char slovo[MAXLINELENGTH];
	
      
/* Preberie spracovaný riadok tabuľky, vyberá z neho po 1 slove a volá ukazatel na funckiu. */	
    while ((k = (nacitajRiadok(riadok))) != EOF)
    {
	    if (k == 1)
	    {                                    // Ak došlo k prekročeniu maximálnej dĺžky riadku
	        P->ecode = ELINELENGTH;         // Vypíše chybové hlásenie.
			return 1;
		}
		 
	    P->lineNum++;
		if (P->lineNum == 2)               // Zisťuje počet stĺpcov tabuľky.
		    sirkaTab = P->wordNum; 
		if (P->lineNum > 1)
		{
		    columnTest(P, sirkaTab);
		    if (P->ecode != EOK)
		        return 1;
	    }
	    			    
		P->wordNum = 0;
		n = 0;
         
        while (riadok[n] != '\n')
		{
            i = 0;
            
		    while (!isspace(riadok[n]))
		    {
		       slovo[i] = riadok[n];
		       n++;
		       i++;
		       p = 1;
		    }
		     
		    if (p != 0)               // Kontroluje prechod predchádzajúceho while. 
		    {
		        slovo[i] = '\0';
		        P->wordNum++; 
                testVyberu(P, slovo);
                p = 0;
		    }
			  
            if (P->ecode != EOK)       // Po každom slove kontrola chybového kódu,                       
                return 1;
            
		    n++;
        }  
        if (zastavit(P))
            break; 
    }
            
    vyskaTab = P->lineNum; 
    rowTest(P, vyskaTab);
    if (P-> ecode != EOK)
        return 1;  
        
    return 0; 	
}

/* Funkcia skontroluje zadané slovo, ak má slovo číselnú hodnotu, 
 * prevedie ho na typ double a vypíše vo formáte %.10g, 
 * v prípade že prevod na double je neúspešný vypíše ho ako typ string. */
void fselect(TParams *P, char *slovo)
{
	double slovoDouble;
	char *ptr;
	
	P->ordNum++;
	
	if (((strtod(slovo, &ptr) == 0 && *ptr != '\0') && (!(strtod(slovo, &ptr) == 0 && *ptr == '\0'))) ||
	   (strtod(slovo, &ptr) != 0 && *ptr != '\0'))
	{
        printf("%s\n", slovo);
	}   
	else
    {

	    slovoDouble = strtod(slovo, NULL);
		printf("%.10g\n", slovoDouble);
	}
}

/* Funkcia sa pokúsi previesť prebraté slovo na typ double, 
 * ak je prevod úspešný porovná ho s momentálnou hodnotou P.res
 * ak je to nové minimum/maximum priradí hodnotu P.res. */
void fminmax(TParams *P, char *slovo)
{	
	double slovoDouble = strtod(slovo, NULL);
	char *ptr;
	
	if (((strtod(slovo, &ptr) == 0 && *ptr != '\0') && (!(strtod(slovo, &ptr) == 0 && *ptr == '\0'))) ||
	   (strtod(slovo, &ptr) != 0 && *ptr != '\0'))
    {
        P->ecode = ECOUNTCHAR;
        return;   
    }
    
	P->ordNum++;
	if (P->ordNum == 1)
	{
	    P->res = slovoDouble;
	}
	else switch(P->operacia)
    {
	    case min:
	        if (slovoDouble < P->res)
		    {
    	       P->res = slovoDouble;
	        }
	        break;			 
	    case max:
	        if (slovoDouble > P->res)
		    {
		       P->res = slovoDouble;
            }
	        break;   
	    default:;
	        break;
	}  
}

/* Funkcia sa pokúsi previesť prebraté slovo na typ double,
 * ak je prevod úspešný, pričíta ho k P.res, v prípade neúspechu nastaví chybový kód. */
void fsumavg(TParams *P, char *slovo)
{
	double slovoDouble = strtod(slovo, NULL);
	char *ptr;
	
    if (((strtod(slovo, &ptr) == 0 && *ptr != '\0') && (!(strtod(slovo, &ptr) == 0 && *ptr == '\0'))) ||
	   (strtod(slovo, &ptr) != 0 && *ptr != '\0'))
    {
        P->ecode = ECOUNTCHAR;   
    }
   
    P->ordNum++;
    if (P->ordNum == 1)
    {
        P->res = slovoDouble;
    }
    else
    {
	    P->res = P->res + slovoDouble;
    }
}
