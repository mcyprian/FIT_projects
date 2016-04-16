/*
 * Subor: proj2.c
 * Datum: 2014/10/27
 * Autor: Michal Cyprian, xcypri01@stud.fit.vutbr.cz
 * Projekt: Iterační výpočty, projekt č. 2 pre predmet IZP
 * Popis: Program počíta vzdialenosť a výšku meraného objetu
 *        na základe uhlov natočenia meracieho prístroja, pri
 *        nasmerovaní na vrchný a spodný okraj objektu.
 */
 
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <math.h>

# define PI 3.14159265359

/* Stavy programu zodpovedajúce spôsobom spustenia programu. */
enum tstates {SHELP, STAN, SDIMENS};

/* Chybové kódy programu. */
enum tecodes {EOK, ENUMOPAR, EFORMP, EVOP, EUNKNOW};

/* Chybové hlásenia, zodpovedajúce chybovým kódom. */
const char *ECODEMSG[] =
{
	[EOK] = "Všetko v poriadku.\n",
	[ENUMOPAR] = "Neprípustný počet parametrov!\n",
	[EFORMP] = "Neprípustný formát parametrov!\n",
	[EVOP] = "Neprípustné hodnoty argumentov!\n",
	[EUNKNOW] = "Nastala neočakávaná chyba",
};

/* Funkcia na výpis chybového hlásenia na stderr. */
void printECode(int ecode)
{
	if (ecode < EOK || ecode > EUNKNOW)
		   ecode = EUNKNOW;
	fprintf(stderr, "%s", ECODEMSG[ecode]);
}

const char *HELPMSG =
   "\n                       Program Iterační výpočty\n"
   "Autor Michal Cyprian 2014\n"
   "Program počíta vzdialenosť a výšku meraného objetu na základe uhlov\n"
   "natočenia meracieho prístroja, pri nasmerovaní na vrchný a spodný okraj objektu\n"
   "Podoby parametrov spustenia programu:\n"
   "--help: program vypíše nápovedu.\n"
   "--tan A N M: (0 < A <= 1.4, 0 < N <= M < 14) program vypíše porovnania prestnosti\n"
   "výpočtov funkcie tangens uhla A pomocou funkcie tan s matematickej knižnice,\n"
   "Taylorovho polynomu a zreťazeného zlomku po počte iterácií iteračných výpočtov\n"
   "z intervalu <N, M>.\n"
   "[-c X] -m A [B]: (parametre v hranatých zátvorkách sú voliteľné\n"
   "0 < A <= 1.4, 0 < B < 1.4, 0 < X <= 100) program vypočíta vzdialenosť meraného objektu\n"
   "keď uhol pri nasmerovaní na spodný okraj objektu je A. Argument B udáva veľkosť uhla\n"
   "pri nasmerovaní na vrchný okraj objetu, ak je argument zadaný, program vypočíta aj výšku\n"
   "meraného objetu. Argument X udáva výšku meracieho prístroja v metroch,\n"
   "ak nie je zadaný počíta sa s výškou 1.5 metra.\n\n";
   	

/* Štruktúra obsahujúca hodnoty parametrov príkazového riadku, spôsob použita programu a chybový kód. */
typedef struct
{
	int NumOP;
	int ecode;
	int state;
	double A;
	int N, M;
	double alfa, beta, c;
} TParams;

/* Prototypy funkcií na výpočet funkcie tangens pomocou iteračných výpočtov. */
double taylor_tan(double x, unsigned int n);
double cfrac_tan(double x, unsigned int n);
double shiftA(double A);
void calculateDimens(double c, double alfa, double beta);

const int BetaNotSet = -1;    // Konštanta na overenie, či bola zadaná beta.

/* Otestuje či je možné previesť reťazec, ktorý preberie ako parameter na číslo typu double,
 *  v prípade že áno, návratová hodnota je 1, v prípade že nie, návraová hodnota je 0. */
int testDouble(char *x)
{
	char *ptr;
	 if (((strtod(x, &ptr) == 0 && *ptr != '\0') && (!(strtod(x, &ptr) == 0 && *ptr == '\0'))) ||
	   (strtod(x, &ptr) != 0 && *ptr != '\0'))
         return 0;    
     else
         return 1;
}

/* Otestuje či je možné previesť reťazec, ktorý preberie ako parameter na číslo typu int,
 *  v prípade že áno, návratová hodnota je 1, v prípade že nie, návraová hodnota je 0. */
int testInt(char *x)
{
	char *ptr;
	 if (((strtol(x, &ptr, 0) == 0 && *ptr != '\0') && (!(strtol(x, &ptr, 0) == 0 && *ptr == '\0'))) ||
	   (strtol(x, &ptr, 0) != 0 && *ptr != '\0'))
         return 0;    
     else
         return 1;
}

/* Funkcia na spracovanie argumentov programu, v prípade že je počet a forma argumentov správna, 
 * nastaví príslušné premenné, v opačnom prípade nastaví chybový kód. */
TParams getParams(int argc, char **argv)
{
	TParams P =
	{.NumOP = argc,
	 .ecode = EOK,
	 .state = STAN,
	 .A = 0.0,
	 .N = 0,
	 .M = 0,
	 .alfa = 0,
	 .beta = 0,
	 .c = 1.5,
    };	
    
    switch (P.NumOP)
    {
		case 2:
		    if (strcmp("--help", argv[1]) == 0)
				P.state = SHELP;
		    else
		        P.ecode = EFORMP;
		    break;
		
		case 3:
		    if ((strcmp("-m", argv[1]) == 0) && testDouble(argv[2]))
		    {
			    P.state = SDIMENS;
			    P.alfa = strtod(argv[2], NULL);
			    P.beta = BetaNotSet;
			   
			    if (P.alfa <= 0 || P.alfa >1.4)
			        P.ecode = EVOP;
		    }
		    else
		        P.ecode = EFORMP;  
		    break;
		
		case 4:
		    if ((strcmp("-m", argv[1]) == 0) &&
	            testDouble(argv[2]) && testDouble(argv[3]))
		    {
		        P.state = SDIMENS;
			    P.alfa = strtod(argv[2], NULL);
			    P.beta = strtod(argv[3], NULL);
			    
			    if ((P.alfa <= 0 || P.alfa > 1.4) || (P.beta <= 0 || P.beta > 1.4))
			    {
			        P.ecode = EVOP;
		        }
		    }
		    else
		       P.ecode = EFORMP;
		    break;	         
		
		case 5:
		    if ((strcmp("--tan", argv[1]) == 0) &&
		         (testDouble(argv[2]) && testInt(argv[3]) && testInt(argv[4])))
		    {
				P.state = STAN;
				P.A = strtod(argv[2], NULL);
				P.N = strtol(argv[3], NULL, 10);
				P.M = strtol(argv[4], NULL, 10);
				
				if (P.N <= 0 || P.N > P.M || P.M >= 14 || P.A <= 0 || P.A > 1.4)
				   P.ecode = EVOP;        // Ak je niektorý z parametrov mimo prípustný rozsah
			}
			else if ((strcmp("-c", argv[1]) == 0) && testDouble(argv[2]) &&
			        (strcmp("-m", argv[3]) == 0) && testDouble(argv[4]))
			     {
					 P.state = SDIMENS;
					 P.alfa = strtod(argv[4], NULL);
					 P.beta = BetaNotSet;
					 P.c = strtod(argv[2], NULL);
					 
					 if ((P.alfa <= 0 || P.alfa >1.4) || (P.c <= 0 || P.c > 100))
			             P.ecode = EVOP;
			     }
			      else        
			          P.ecode = EFORMP;
	        break;
	    
	    case 6:
	        if ((strcmp("-c", argv[1]) == 0) && testDouble(argv[2]) &&
			   (strcmp("-m", argv[3]) == 0) && testDouble(argv[4]) &&
			    testDouble(argv[5])) 
			{
			    P.state = SDIMENS;
				P.alfa = strtod(argv[4], NULL);
				P.beta = strtod(argv[5], NULL);
				P.c = strtod(argv[2], NULL);
				
			    if ((P.alfa <= 0 || P.alfa > 1.4) ||
			       (P.beta <= 0 || P.beta > 1.4) ||
				   (P.c <= 0 || P.c > 100)) 
			        P.ecode = EVOP;
			}
			else        
			    P.ecode = EFORMP;
	        break;
	    
	    default:
	        P.ecode = ENUMOPAR;
	        break;
	}
return P;	    
}


int main(int argc, char **argv)
{
	int i;
	double abs_chyba_taylor;
	double abs_chyba_cfrac;
    TParams P = getParams(argc, argv);     // Spracovanie argumentov programu.
    
    if (P.ecode != EOK)
	{	   
		printECode(P.ecode);
		return EXIT_FAILURE;
	}
	
	switch (P.state)               // Podľa nastaveného stavu vyberie vetvu priebehu programu.
	{
		case SHELP:
		    printf("%s", HELPMSG);
		    break;
		    
		case STAN:
		    for (i = P.N; i <= P.M; i++)
	        {
		        abs_chyba_taylor = (tan(P.A) - taylor_tan(P.A, i)) >= 0 ?
		            (tan(P.A) - taylor_tan(P.A, i)) : -(tan(P.A) - taylor_tan(P.A, i));  // Výpočet absolútnej chyby medzi výsledkami funkcie tan  
		        abs_chyba_cfrac = (tan(P.A) - cfrac_tan(P.A, i)) >= 0 ?                  // z matematickej knižnice a implementovaných funkcií taylor, cfranc.
		            (tan(P.A) - cfrac_tan(P.A, i)) : -(tan(P.A) - cfrac_tan(P.A, i));     
		        printf("%d %e %e %e %e %e\n", i, tan(P.A), taylor_tan(P.A, i),
	            abs_chyba_taylor, cfrac_tan(P.A, i), abs_chyba_cfrac);
            }
            break;
        
        case SDIMENS:
            calculateDimens(P.c, P.alfa, P.beta);
            break;
        
        default:
            printECode(EUNKNOW);
            return EXIT_FAILURE;
            break;
	}
return EXIT_SUCCESS;		
}

/* Funkcia na výpočet funkcie tangens, pomocou Taylorovho polynómu. */
double taylor_tan(double x, unsigned int n)
{
	unsigned int i;
	double res = x;
	double ti = x;
	double mocnina = x*x;
	
	double cit [13] = 
	       {1, 1, 2, 17, 62, 1382, 21844,
            929569, 6404582, 443861162, 18888466084,
            113927491862, 58870668456604,};
            
    double men [13] = 
           {1, 3, 15, 315, 2835, 155925,
			6081075, 638512875, 10854718875, 
			1856156927625, 194896477400625, 
			49308808782358125, 3698160658676859375,};
			
	
	for(i = 1; i < n; i++)
	{
		x = x * mocnina;
		ti = x * cit[i] / men[i];
		res = res + ti;
	}
	return res;
}

/* Funkcia na výpočet funkcie tangens, pomocou zreťazeného zlomku. */
double cfrac_tan(double x, unsigned int n)
{
	unsigned int i;
	double res = 0.0;
	double ak = x*x;
	double bk = 2*n -1;
	
	for (i = n; i > 1; i--)
	{
		res = ak / (bk - res);
		bk -= 2.0;
	}
	return x / (1 - res);
}

/* Funkcia na výpočet a výpis vzdialenosti a výšky, v prípade, 
 * že beta nebola zadaná vypočíta a vypáše iba vzdialenosť. */ 
void calculateDimens(double c, double alfa, double beta)
{
	double d, v, tanAlfa, tanBeta;
	
	tanAlfa = (alfa > 0.7) ? cfrac_tan(alfa, 9) : cfrac_tan(alfa, 6);
	d = c / tanAlfa;
	printf("%.10e\n", d);
	
	if (beta != BetaNotSet)
	{
		tanBeta = (beta > 0.7) ? cfrac_tan(beta, 9) : cfrac_tan(alfa, 6);
		v = tanBeta * d + c;
		printf("%.10e\n", v);			
	}	
}
