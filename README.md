# FIT_project
### School projects (Faculty of Information Technology, Brno University of Technology)

# IZP/proj1

### Zadanie:
#### Popis projektu

Implementujte jednoduchý tabulkový kalkulátor. Program bude implementovat funkce vyhledání maxima, minima,
funkce součtu a aritmetického průměru vybraných buněk. Tabulku ve formě textového souboru bude program očekávat
na standardním vstupu. Požadovanou operaci a výběr buněk specifikuje uživatel v argumentu příkazové řádky. 

#### Argumenty programu:


    --help způsobí, že program vytiskne nápovědu používání programu a skončí.
    operace reprezentuje jednu z následujících operací:
        select značí operaci, která z dané tabulky pouze vybere a následně vytiskne hodnoty daných buněk,
        min značí vyhledání a následný tisk minimální hodnoty z daného rozsahu buněk,
        max značí vyhledání a následný tisk maximální hodnoty z daného rozsahu buněk,
        sum značí výpočet a následný tisk sumy hodnot všech vybraných buněk,
        avg značí výpočet a následný tisk aritmetického průměru vybraných buněk.
    výběr_buněk reprezentuje rozsah tabulky, nad kterým má být provedena daná operace. Rozsah je definován 
    jedním z následujících argumentů:
        row X značí výběr všech buněk na řádku X (X > 0),
        col X značí výběr všech buněk ve sloupci X (X > 0),
        rows X Y značí výběr všech buněk od řádku X (včetně) až po Y (včetně). 0 < X <= Y.
        cols X Y značí výběr všech buněk od sloupce X (včetně) až po Y (včetně). 0 < X <= Y.
        range A B X Y značí výběr buněk od řádku A po řádek B a od sloupce X po sloupec Y (včetně daných 
        řádků a sloupců). 0 < A <= B, 0 < X <= Y.


#### Implementační detaily

Program čte zpracovávanou tabulku ze standardního vstupu (stdin). Veškeré informace vypisuje na standardní 
výstup (stdout). Řádek tabulky je zakončen znakem konce řádku. Sloupce tabulky (resp. buňky na řádku) jsou odděleny 
jedním a více bílých znaků. Buňka může obsahovat číslo nebo text (bez bílých znaků). Maximální délka jednoho řádku 
je 1024 znaků.

#### Operace a jejich výstup

Operace select vypíše seznam hodnot z daného výběru. Každá hodnota bude vypsána na jeden řádek. V případě, 
že hodnota je číslo, bude vypsáno formátem %.10g funkce printf. V případě, že výběr zahrnuje více řádků 
či sloupců, hodnoty tabulky budou zpracovávány po řádcích, tj. nejprve všechny hodnoty daného řádku 
zleva doprava,pak hodnoty následujícího řádku.
Operace min, max, sum a avg pracují pouze nad buňkami obsahující číselné údaje. Funkce vypíší výsledek jako 
jedno číslo formátu %.10g na samostatný řádek.

#### Neočekávané chování

Na chyby za běhu programu reagujte obvyklým způsobem: Na neočekávaná vstupní data, argumenty příkazového řádku, 
formát vstupních dat nebo chyby při volání funkcí reagujte přerušením programu se stručným a výstižným 
chybovým hlášením na příslušný výstup a odpovídajícím návratovým kódem. 


# IZP/proj2

### Zadanie:
#### Popis projektu

Implementujte výpočet vzdálenosti a výšky měřeného objektu pomocí údajů ze senzorů 
natočení měřicího přístroje. Výpočet proveďte pouze pomocí matematických operací ``+``,``-``,``*``,``/``. 

#### Syntax spuštění

Program se spouští v následující podobě:

``./proj2 --help``

nebo

``./proj2 --tan A N M``

nebo

``./proj2 [-c X] -m A [B]``

#### Argumenty programu:

* ``--help`` způsobí, že program vytiskne nápovědu používání programu a skončí.
* ``--tan`` srovná přesnosti výpočtu tangens úhlu A (v radiánech)             mezi voláním tan z matematické knihovny, 
        a výpočtu tangens pomocí Taylorova polynomu a zřetězeného zlomku. Argumenty N a M udávají, ve kterých
        iteracích iteračního výpočtu má srovnání probíhat. 0 < N <= M < 14
* ``-m`` vypočítá a změří vzdálenosti.
        Úhel α (viz obrázek) je dán argumentem A v radiánech. Program vypočítá a vypíše vzdálenost měřeného 
        objektu. 0 < A <= 1.4 < π/2.
        Pokud je zadán, úhel β udává argument B v radiánech. Program vypočítá a vypíše i výšku měřeného objektu. 
        0 < B <= 1.4 < π/2
        Argument -c nastavuje výšku měřicího přístroje c pro výpočet. Výška c je dána argumentem X (0 < X <= 100). 
        Argument je volitelný - implicitní výška je 1.5 metrů.

#### Implementační detaily

Je zakázané použít funkce z matematické knihovny. Jedinou výjimkou je funkce tan použitá pouze pro srovnání výpočtů,
funkce isnan a isinf a konstanty NAN a INF. Ve všech výpočtech používejte typ double. Uvažujte přesnost na 10 míst. 
Počet iterací v iteračních výpočtech odvoďte.

Implementace tangens

Funkci tangens implementujte dvakrát a to pomocí Taylorova polynomu a zřetězených zlomků.

###### 1. podúkol

Tangens pomocí Taylorova polynomu implementujte ve funkci s prototypem:

```double taylor_tan(double x, unsigned int n);```

kde x udává úhel a n rozvoj polynomu (počet členů). Taylorův polynom pro funkci tangens vypadá následovně:

přičemž řada čitatelů ve zlomcích koeficientů je zde a řada jmenovatelů ve zlomcích koeficientů je zde. 
Ve svém programu použijte pouze 13 prvních koeficientů. 

###### 2. podúkol

Tangens pomocí zřetězených zlomků (viz demonstrační cvičení) implementujte ve funkci s prototypem:

```double cfrac_tan(double x, unsigned int n);```

kde x udává úhel a n rozvoj zřetězeného zlomku.

rovnání přesnosti výpočtu tangens

Srovnání iteračních a zabudovaných výpočtů realizujte pro zadaný počet iterací. Výstup srovnání bude 
v podobě řádků obsahující následující:

I M T TE C CE

(odpovídající formátu "%d %e %e %e %e %e\n"), kde:

    I značí počet iterací iteračního výpočtu,
    M výsledek z funkce tan matematické knihovny,
    T výsledek z funkce taylor_tan,
    TE absolutní chybu mezi výpočtem pomocí Taylorova polynomu a matematickou knihovnou,
    C výsledek z funkce cfrac_tan a
    CE absolutní chybu mezi výpočtem pomocí zřetězených zlomků a matematickou knihovnou.

Počet iterací udává interval daný argumenty programu <N;M>. Výstup programu tedy bude obsahovat N-M+1 řádků. 
První řádek tedy začíná číslem N.

###### Měření

Pro měření vzdálenosti a výšky použijte vámi implementovanou funkci cfrac_tan.

V případě zadaného úhlu α i β vypište dvě čísla odpovídající délce d a výšce v (v tomto pořadí). 
Oba číselné údaje vypisujte formátovaným výstupem pomocí "%.10e", každý údaj samostatně na jeden řádek. 


# IZP/proj3

### Zadanie:
#### Popis projektu
Vytvořte program, který v daném bludišti a jeho vstupu najde průchod ven. Bludiště je uloženo v textovém 
souboru ve formě obdélníkové matice celých čísel. Cílem programu je výpis souřadnic políček bludiště, 
přes které vede cesta z vchodu bludiště do jeho východu

#### Syntax spuštění

Program se spouští v následující podobě:

``./proj3 --help``

nebo

``./proj3 --test soubor.txt``

nebo

``./proj3 --rpath R C bludiste.txt``

nebo

``./proj3 --lpath R C bludiste.txt``

nebo (nepovinné, prémiové)

``./proj3 --shortest R C bludiste.txt``

#### Argumenty programu:

* ``--help`` způsobí, že program vytiskne nápovědu používání programu a skončí.
* ``--test`` pouze zkontroluje, že soubor daný druhým argumentem programu obsahuje řádnou definici mapy bludiště. 
        V případě, že formát obrázku odpovídá definici (viz níže), vytiskne Valid. V opačném případě 
        (např. ilegálních znaků, chybějících údajů nebo špatných hodnot) program tiskne Invalid.
* ``--rpath`` hledá průchod bludištěm na vstupu na řádku R a sloupci C. Průchod hledá pomocí pravidla pravé ruky 
        (pravá ruka vždy na zdi).
* ``--lpath`` hledá průchod bludištěm na vstupu na řádku R a sloupci C, ale za pomoci pravidla levé ruky.
* ``--shortest`` (prémiové řešení) hledá nejkratší cestu z bludiště při vstupu na řádku R a sloupci C.
    
 Bludiště je aproximováno do mapy trojúhelníkových políček. Mapa bludiště je uložena v textovém souboru obsahující 
 číselné údaje oddělené bílými znaky. První dva číselné údaje reprezentují velikost obrázku v počtu trojúhelníkových 
 políček (výška a šířka mapy - tedy počet řádků a počet sloupců). Následují definice jednotlivých řádků 
 (oddělené bílým znakem), kde každý řádek obsahuje hodnoty jednotlivých políček. 3 bitová hodnota každého políčka 
 definuje, jak vypadá hranice daného políčka. Bit s hodnotou 1 značí, že dané poličko má neprůchozí stěnu na dané 
 hranici, bit s hodnotou 0 značí, že daná hranice je průchozí:

    nejnižší bit popisuje levou šikmou hranici,
    druhý nejnižší bit popisuje pravou šikmou hranici,
    třetí nejnižší bit popisuje buď horní nebo spodní hranici (závisí na typu políčka).

Mapa bludiště je validní, pokud popisuje všechny políčka a všechny sousedící políčka mají stejný typ sdílené hranice.

Příklad: Nechť políčko na souřadnici 2,3 má stěnu na levé a spodní hranici. Potom jeho hodnota v matici bude 1+0+4=5. 

#### Implementační detaily

Cesta bude tištěna jako posloupnost souřadnic políček, přes které cesta vede. Každá souřadnice bude vypsána na 
jeden řádek ve formátu řádek,sloupec. Čísla řádků a sloupců začínají od 1.

###### 1. podúkol

Mapu implementujte pomocí dvojrozměrného pole s využitím následujícího datového typu:
```
typedef struct {
    int rows;
    int cols;
    unsigned char *cells;
} Map;
```
kde rows je počet řádků, cols je počet sloupců a cells je ukazatel na pole řádků, každý řádek obsahuje kódy 
jednotlivých políček na daném řádku (pole typu char). Implementujte podpůrné funkce tohoto typu (inicializace mapy, 
načtení mapy, uvolnění mapy).

###### 2. podúkol

Ve vámi implementovaných algoritmech vyhledávání útvarů neprovádějte dotazy na typ hranice přímým přístupem do paměti 
(tj. do struktury bitmapy), ale striktně pomocí mapovací funkce s následujícím prototypem 
(její definici implementujte sami):

```bool isborder(Map *map, int r, int c, int border);```

Parametr map je platný ukazatel na strukturu mapy, r a c je souřadnice (řádek a sloupec) požadovaného políčka
a border označuje dotazovanou hranici políčka (hodnoty si definujte své). Funkce vrací true, pokud na zmíněné 
hranici políčka stojí stěna. 

###### 3. podúkol

Implementujte funkci, která vrátí, která hranice se má po vstupu do bludiště následovat 
(tj. která má být vedena rukou).

```int start_border(Map *map, int r, int c, int leftright);```

Parametr map je platný ukazatel na strukturu mapy, r a c je souřadnice vstupního políčka a leftright udává, 
které pravidlo se při průchodu aplikuje (hodnoty si definujte sami). Funkce vrací hodnotu identifikující danou 
hranici na vstupním políčku (konkrétní hodnoty si definujte sami). V případě hledání cesty podle pravé ruky 
bude počáteční hranice:

    pravá při vstupu zleva do bludiště na lichém řádku,
    dolní při vstupu zleva do bludiště na sudém řádku,
    levá při vstupu shora do bludiště,
    pravá při vstupu zespodu do bludiště,
    horní při vstupu zprava do bludiště, pokud má políčko horní hranici a
    levá při vstupu zprava do bludiště, pokud má políčko dolní hranici.

Přirozeně a obdobně pro hledání pomocí pravidla levé ruky. 


# IOS/p1

### Zadanie:

#### Popis úlohy

Cílem úlohy je vytvořit skript (tzv. wrapper), který bude spouštět textový editor. Skript si bude pamatovat, 
které soubory byly v jakém adresáři prostřednictvím daného skriptu editovány. Pokud bude skript spuštěn 
bez parametrů, vybere skript soubor, který má být editován.

#### Specifikace chování skriptu

###### JMÉNO

``wedi`` - wrapper textového editoru s možností automatického výběru souboru

###### POUŽITÍ
```
wedi [ADRESÁŘ]
wedi -m [ADRESÁŘ]
wedi SOUBOR
wedi -l [ADRESÁŘ]
wedi -b|-a DATUM [ADRESÁŘ]
```
###### POPIS

    Pokud byl skriptu zadán soubor, bude editován.
    Pokud není argumentem skriptu zadáno jméno editovaného souboru, skript z daného adresáře vybere soubor pro 
    editaci. Výběr je následující:
        Pokud bylo v daném adresáři editováno skriptem více souborů, vybere se soubor, který byl pomocí skriptu 
        editován jako poslední. Editací souboru se myslí třeba i prohlížení jeho obsahu pomocí skriptu 
        (tj. není nutné, aby byl soubor změněn).
        Pokud byl zadán argument -m, vybere se soubor, který byl pomocí skriptu editován nejčastěji.
        Pokud nebyl v daném adresáři editován ještě žádný soubor, jedná se o chybu.
        Pokud nebyl zadán adresář, předpokládá se aktuální adresář.
    Skript dokáže také zobrazit seznam všech souborů (argument -l), které byly v daném adresáři editovány.
    Pokud byl zadán argument -b resp. -a (before, after), skript zobrazí seznam souborů, které byly editovány 
    před resp. po zadaném datu. DATUM je formátu YYYY-MM-DD. Jsou zobrazeny soubory, které byly skriptem 
    editovány od daného data včetně.

###### NASTAVENÍ A KONFIGURACE

    Skript si pamatuje informace o svém spouštění v souboru, který je dán proměnnou WEDI_RC. Pokud není 
    proměnná nastavena, jedná se o chybu. Formát souboru není specifikován.
    Skript spouští editor, který je nastaven v proměnné EDITOR. Pokud není proměnná EDITOR nastavená, 
    respektuje proměnnou VISUAL. Pokud ani ta není nastavená, jedná se o chybu.

###### NÁVRATOVÁ HODNOTA

    Skript vrací úspěch v případě úspěšné operace nebo v případě úspěšné editace. Pokud editor vrátí chybu, 
    skript vrátí stejný chybový návratový kód. Interní chyba skriptu bude doprovázena chybovým hlášením.

###### POZNÁMKY

    Skript nebere v potaz soubory, se kterými dříve počítal a které jsou nyní smazané.
    Při rozhodování relativní cesty adresáře je doporučeno používat reálnou cestu (realpath), např.:

``$ wedi .``

``$ wedi `pwd` ``

#### Implementační detaily

    Skript by měl běžet na všech běžných shellech (dash, ksh, bash). Ve školním prostředí můžete použít 
    základní (POSIX) /bin/sh.
    Skript by měl ošetřit i chybový případ, že na daném stroji utilita realpath není dostupná.
    Jako referenční stroj můžete použít merlin (Linux) a eva (FreeBSD).
    Skript nesmí používat dočasné soubory.


# IOS/p2

### Zadanie:
#### Popis úlohy
Implementujte v jazyce C modifikovaný synchronizační problém Building H20. Existují dva typy procesů, kyslík a vodík. 
Aby se mohly spojit do molekuly vody, musí se počkat na příchod příslušných atomů, tj. jeden atom kyslíku a dva atomy 
vodíku. Poté se spojí do molekuly vody a proces se opakuje. 

#### Detailní specifikace úlohy
##### Spuštění

``$ ./h2o N GH GO B``

kde

    N je počet procesů reprezentujících kyslík; počet procesů reprezentujících vodík bude vždy 2*N, N > 0.
    GH je maximální hodnota doby (v milisekundách), po které je generován nový proces pro vodík. GH >= 0 && GH < 5001.
    GO je maximální hodnota doby (v milisekundách), po které je generován nový proces pro kyslík. GO >= 0 && GO < 5001.
    B je maximální hodnota doby (v milisekundách) provádění funkce bond (viz sekce Popis procesů). B >= 0 && B < 5001.
    Všechny parametry jsou celá čísla.

##### Implementační detaily

    Každému atomu odpovídá jeden proces.
    Hlavní proces vytváří dva pomocné procesy, každý pro generování procesů atomů stejné kategorie
        jeden proces generuje procesy pro vodík; každý nový proces je generován po uplynutí náhodné doby z intervalu <0, GH>; vygeneruje 2*N procesů.
        další proces generuje procesy pro kyslík; každý nový proces je generován po uplynutí náhodné doby z intervalu <0, GO>; vygeneruje N procesů.
        postupně tedy vznikne hlavní proces, dva pomocné procesy pro generování atomů a 3*N procesů atomů.
    Každý proces hydrogen i oxygen bude interně identifikován celým číslem I, začínajícím od 1. Číselná řada je pro každou kategorii atomů zvlášť.
    Každý proces atomu vykonává své akce a současně zapisuje informace o akcích do souboru s názvem h2o.out.
        Přístup k výstupnímu zařízení (zápis informací) musí být výlučný; pokud zapisuje jeden proces a další chce také zapisovat, musí počkat na uvolnění zdroje.
        Součástí výstupních informací o akci je pořadové číslo A prováděné akce (viz popis výstupů). Akce se číslují od jedničky.
    Použijte sdílenou paměť pro implementaci čítače akcí a sdílených proměnných nutných pro synchronizaci.
    Použijte semafory pro synchronizaci procesů.
    Nepoužívejte aktivní čekání (včetně cyklického časového uspání procesu) pro účely synchronizace.
    Procesy atomů, které již utvořily molekulu, čekají na všechny ostatní procesy; všechny procesy atomů se ukončí současně.
    Hlavní proces čeká na ukončení všech vytvořených procesů. Poté se ukončí s kódem (exit code) 0.
    Budete-li potřebovat generovat unikátní klíč, je vhodné použít funkci ftok.
    Další funkce a systémová volání: fork, wait, shmat, semctl, semget, shmget, sem_open, usleep, ...

##### Chybové stavy

    Pokud některý ze vstupů nebude odpovídat očekávanému formátu nebo bude mimo povolený rozsah, program vytiskne 
    chybové hlášení na standardní chybový výstup, uvolní všechny dosud alokované zdroje a ukončí se s kódem (exit code) 1.
    Pokud selže systémové volání, program vytiskne chybové hlášení na standardní chybový výstup, uvolní všechny 
    alokované zdroje a ukončí se s kódem (exit code) 2.

##### Popis procesů a jejich výstupů

###### Poznámka k výstupům:

    A je pořadové číslo prováděné akce,
    NAME je zkratka kategorie příslušného procesu, tj. H nebo O,
    I je interní identifikátor procesu v rámci příslušné kategorie,
    Při vyhodnocování výstupu budou ignorovány mezery a tabelátory.
    
###### Proces atomu (oxygen i hydrogen)

    Po spuštění tiskne A: NAME I: started.
    Ověřuje stav za následujících podmínek:
        pokud se zrovna tvoří jiná molekula, proces čeká, dokud nebude schopen provést bod 2b
        pokud se zrovna netvoří jiná molekula, proces ověří, zda je k dispozici potřebný počet volných atomů příslušných typů:
            pokud ne, tiskne A: NAME I: waiting a čeká na chybějící procesy
            pokud ano (je k dispozici dvakrát hydrogen a jednou oxygen), tiskne A: NAME I: ready a začne proces tvorby molekuly (tento proces a procesy, které tiskly waiting, přechází na bod 3)
    Jakmile jsou k dispozici tři vhodné atomy, zavolá každý z těchto tří procesů funkci bond, v které:
        tiskne A: NAME I: begin bonding
        uspí proces na náhodnou dobu z intervalu <0, B>
    Jakmile se atomy spojí do molekuly, tj. všechny tři procesy dokončily funkci bond, tak poté tiske každý z těchto procesů A: NAME I: bonded.
    Procesy atomů se ukončí současně, tj. čekají, až se vytvoří molekuly ze všech atomů. Poté každý proces atomu tiskne A: NAME I: finished.
    Omezující podmínky: Pokud se tvoří molekula (body 2.b.II, 3 a 4), nesmí další procesy provádět totéž, tj. v jednom okamžiku může vznikat pouze jedna molekula. Další atomy musí počkat, dokud se úplně nedokončí předchozí molekula, tj. skončí provádění bodu 4. Toto omezení platí pro body 2.b, 3 a 4 (při tvorbě molekuly se nesmí objevit výstup z bodů 2.b, 3 a 4 z atomů jiné molekuly).


# IJC/DU1

### Zadanie:
a) V rozhraní "bit-array.h" definujte pro pole bitů:
   Typ:
    ``` typedef BitArray_t```
       Typ bitového pole (pro předávání parametru do funkce)

   Makra:
     ```BA_create(jmeno_pole,velikost)```
       definuje a nuluje pole (POZOR: MUSÍ _INICIALIZOVAT_ pole bez ohledu na
       to, zda je pole statické nebo automatické/lokální!  Vyzkoušejte si obě
       varianty, v programu použijte lokální pole.)
       Př: BA_create(p,100);     // p = pole 100 bitů
           BA_create(q,100000L); // q = pole 100000 bitů

     ```BA_size(jmeno_pole)```
       vrátí deklarovanou velikost pole v bitech

     ```BA_set_bit(jmeno_pole,index,výraz)```
       nastaví zadaný bit v poli na hodnotu zadanou výrazem
       (nulový výraz == bit 0, nenulový výraz == bit 1)
       Př: BA_set_bit(p,20,1);

     ```BA_get_bit(jmeno_pole,index)```
       získá hodnotu zadaného bitu, vrací hodnotu 0 nebo 1
       Př: if(BA_get_bit(p,i)==1) printf("1");
           if(!BA_get_bit(p,i))   printf("0");

   Kontrolujte meze polí. V případě chyby volejte funkci
     ```FatalError("Index %ld mimo rozsah 0..%ld", (long)index, (long)mez)```.
   (Použijte modul error.c/error.h z příkladu b)

   Program musí fungovat na 32 (gcc -m32) i 64bitové platformě.

   Podmíněným překladem zajistěte, aby se při definovaném symbolu
   USE_INLINE místo těchto maker definovaly inline funkce stejného jména
   všude kde je to možné (bez změn v následujícím testovacím příkladu!).
   USE_INLINE nesmí být definováno ve zdrojovém textu --
   překládá se s argumentem -D (gcc -DUSE_INLINE ...).

   Napište pomocná makra DU1_GET_BIT_(p,i), DU1_SET_BIT_(p,i,b)
   pro indexování bitů v poli T *p nebo T p[NN] bez kontroly mezí,
   kde T je libovolný celočíselný typ (char, unsigned short, ...).
   (Tato makra pak použijete v dalsích makrech a inline funkcích.)
   Pro vaši implementaci použijte pole typu: unsigned long [].
   Implementace musí efektivně využívat paměť (využít každý
   bit pole až na posledních max. X-1, pokud má unsigned long X bitů).

   Jako testovací příklad implementujte funkci, která použije algoritmus známý
   jako Eratostenovo síto (void Eratosthenes(BitArray_t pole);) a použijte ji
   pro výpočet posledních 10 prvočísel ze všech prvočísel od 2 do
   N=201000000 (201 milionů). (Doporučuji program nejdříve odladit pro N=100.)
   Funkci Eratosthenes napište do samostatného modulu "eratosthenes.c".

   Budete pravděpodobně potřebovat zvětšit limit velikosti zásobníku.
   Na Unix-like systémech použijte příkaz ulimit -a pro zjištění velikosti
   limitu a potom "ulimit -s zadana_velikost".

   Každé prvočíslo tiskněte na zvláštní řádek v pořadí
   vzestupném.  Netiskněte  nic  jiného  než  prvočísla (bude se
   automaticky  kontrolovat!).  Pro kontrolu správnosti prvočísel
   můžete použít program  factor (./prvocisla|factor).

   Zdrojový text programu se musí jmenovat "prvocisla.c" !
   Příkaz "make" musí vytvořit obě varianty: prvocisla a prvocisla-inline
(7b)

Poznámky:  Eratosthenovo síto (přibližná specifikace):
   1) Nulujeme bitové pole  p  o rozměru N, index i nastavit na 2
   2) Vybereme nejmenší index i, takový, že p[i]==0. Potom je
      i prvočíslo
   3) Pro všechny násobky i nastavíme bit p[n*i] na 1
      ('vyškrtneme' násobky - nejsou to prvočísla)
   4) i++; dokud nejsme za sqrt(N), opakujeme bod 2 až 4
      (POZOR: sestavit s matematickou knihovnou parametrem -lm)
   5) Výsledek: v poli p jsou na prvočíselných indexech hodnoty 0

   Efektivita výpočtu: cca 5s na Core2duo/2.8GHz/Linux64 (gcc -O2)
   Porovnejte efektivitu obou variant (makra vs. inline funkce).

b) Napište modul "error.c" s rozhraním v "error.h", který definuje
   funkci void Warning(const char *fmt, ...) a
   funkci void FatalError(const char *fmt, ...). Tyto funkce mají
   stejné parametry jako printf(); tisknou text "CHYBA: " a potom
   chybové hlášení podle formátu fmt. Vše se tiskne do stderr
   (funkcí vfprintf) a potom FatalError ukončí program voláním
   funkce exit(1).  Použijte definice ze stdarg.h.

 * Napište modul "ppm.c" s rozhraním "ppm.h",
   ve kterém definujete typ:
   ```
     struct ppm {
        unsigned xsize;
        unsigned ysize;
        char data[];    // RGB bajty, celkem 3*xsize*ysize
     };
    ```
   a funkce:
     ```struct ppm * ppm_read(const char * filename);```
        načte obsah PPM souboru do touto funkcí dynamicky
        alokované struktury. Při chybě formátu použije funkci Warning
        a vrátí NULL.  Pozor na "memory leaks".

     ```int ppm_write(struct ppm *p, const char * filename);```
        zapíše obsah struktury p do souboru ve formátu PPM.
        Při chybě použije funkci Warning a vrátí záporné číslo.

    Můžete omezit max. velikost obrazových dat vhodným implementačním
    limitem (např 5000*5000*3), aby bylo možné použít statickou inicializaci
    BitArray pro následující testovací program.

    Popis formátu PPM najdete na Internetu, implementujte pouze
    binární variantu P6 s barvami 0..255 bez komentářů:
    ```
      "P6" <ws>+
      <xsizetxt> <ws>+ <ysizetxt> <ws>+
      "255" <ws>
      <binarni data, 3*xsize*ysize bajtu RGB>
      <EOF>
    ```

  * Napište testovací program "steg-decode.c", kde ve funkci main načtete ze
    souboru zadaného jako jediný argument programu obrázek ve formátu PPM
    a v něm najdete uloženou "tajnou" zprávu. Zprávu vytisknete na stdout.

    Zpráva je řetězec znaků (char, včetně '\0') uložený po jednotlivých bitech
    (počínaje LSb) na nejnižších bitech (LSb) vybraných bajtů barevných složek
    v datech obrázku. Dekódování ukončete po dosažení '\0'.
    Pro DU1 budou vybrané bajty určeny prvočísly -- použijte Eratostenovo síto
    podobně jako v příkladu "prvocisla.c".

    Program použije FatalError v případě chyby čtení souboru (chybný formát),
    v případě, že zpráva obsahuje netisknutelné znaky (použijte isprint) nebo
    není korektně ukončena '\0'.

    Použijte program "make" pro překlad/sestavení programu.
    Testovací příkaz:  ./steg-decode du1-obrazek.ppm

   Zařiďte, aby příkaz "make" bez parametrů vytvořil všechny spustitelné
   soubory pro DU1.  Při změně kteréhokoli souboru musí přeložit jen změněný
   soubor a závislosti.


# IJC/DU2

### Zadanie:

##### 1) (max 5b)
a) V jazyku C napište program "tail.c", který ze zadaného 
vstupního souboru vytiskne posledních 10 řádků. Není-li 
zadán vstupní soubor, čte ze stdin. Je-li programu zadán 
parametr -n číslo, bude se tisknout tolik posledních řádků, 
kolik je zadáno parametrem 'číslo'. Pokud je programu zadán 
parametr -n +číslo, bude se tisknout od řádku 'číslo' 
(první řádek má číslo 1) až do konce souboru. 
Případná chybová hlášení tiskněte do stderr. Příklady: 
```
tail soubor 
tail -n 20 <soubor 
tail -n +3 
```
[Poznámka: výsledky by měly být +-stejné jako u POSIX příkazu tail] 

Je povolen implementační limit na délku řádku (např. 510 znaků), 
v případě prvního překročení mezí hlaste chybu na stderr (řádně otestujte) 
a pokračujte se zkrácenými řádky (zbytek řádku přeskočit/ignorovat). 

b) Napište stejný program jako v a) v C++11 s použitím standardní 
knihovny C++. Jméno programu: "tail2.cc". Tento program 
musí zvládnout řádky libovolné délky a jejich libovolný počet, 
jediným možným omezením je volná paměť. 
Použijte funkci 
```std::getline(istream, string)```
a vhodný STL kontejner (např. std::queue<string>). 
Poznámka: Pro zrychlení použijte std::ios::sync_with_stdio(false); 

##### 2) (max 10b) 
Přepište následující C++ program do jazyka ISO C 
```
// wordcount-.cc // Použijte GCC>=4.7: g++ -std=c++11 
// Příklad použití STL kontejneru map<> nebo unordered_map<> 
// Program počítá četnost slov ve vstupním textu, 
// slovo je cokoli oddělené "bílým znakem" === isspace

#include <string> 
#include <iostream> 

#if 1 // {0,1} - vyzkoušejte si obě varianty 

# include <map> 

int main() { 
    using namespace std; 
    map<string,int> m; // asociativní pole - indexem je slovo 
    string word; 
    while (cin >> word) // čtení slova 
        m[word]++; // počítání výskytů slova 
    
    for (auto &mi: m) // pro všechny prvky kontejneru m 
        cout << mi.first << "\t" << mi.second << "\n"; 
        // tisk slovo (klíč) počet (data) 
  } 
  
  #else 
  # include <unordered_map> 
  
  int main() { 
      using namespace std; 
      unordered_map<string,int> m; // asociativní pole 
      string word; 
      
      while (cin >> word) // čtení slova 
          m[word]++; // počítání výskytů slova 
      
      for (auto &mi: m) // pro všechny prvky kontejneru m 
          cout << mi.first << "\t" << mi.second << "\n"; 
      // tisk slovo (klíč) počet (data) 
  } 
  
  #endif 
  ```
  Výstupy programů musí být pro stejný vstup stejné (kromě pořadí a příliš dlouhých slov). 
  Výsledný program se musí jmenovat "wordcount.c". 
  Veškeré operace s tabulkou budou v samostatné knihovně (vytvořte statickou 
  i dynamickou/sdílenou verzi). V knihovně musí být každá funkce ve 
  zvláštním modulu - to umožní případnou výměnu hash_function() ve vašem 
  staticky sestaveném programu (vyzkoušejte si to: definujte svoji 
  hash_function v programu). 
  
  Knihovna s tabulkou se musí jmenovat 
  "libhtable.a" (na Windows je možné i "htable.lib") pro statickou variantu, 
  "libhtable.so" (na Windows je možné i "htable.dll") pro sdílenou variantu 
  a rozhraní "htable.h". 
  
  Podmínky: 
     - Implementace musí být dynamická (malloc/free) a musíte zvládnout 
       správu paměti v C (použijte valgrind, nebo jiný podobný nástroj). 
     - Asociativní pole implementujte nejdříve prototypově jednoduchým seznamem
       a potom tabulkou (hash table). Odevzdává se řešení s tabulkou. 
       
  Vhodná rozptylovací funkce pro řetězce je podle literatury 
  (http://www.cse.yorku.ca/~oz/hash.html varianta sdbm): 
  ```
    unsigned int hash_function(const char *str, unsigned htab_size) {
        unsigned int h=0; 
        const unsigned char *p; 
        for(p=(const unsigned char*)str; 
        *p!='\0'; p++) h = 65599*h + *p; return h % htab_size; 
    } 
   ```
  její výsledek určuje index do tabulky. 
- Tabulka je struktura obsahující pole seznamů a velikost:
    ```
    +-----------+ 
    | htab_size | 
    +-----------+ 
    +---+ 
    |ptr|-->[key,data,next]-->[key,data,next]-->[key,data,next]--|
    +---+ 
    |ptr|-->[key,data,next]-->[key,data,next]--| +---+ |ptr|--|
    +---+ 
    ```
Položka htab_size je velikost následujícího pole ukazatelů (použijte 
C99: "flexible array member"). V programu zvolte vhodnou velikost pole a 
v komentáři zdůvodněte vaše rozhodnutí. 
    
- Napište funkce 
    ```
    t=htab_init(size) pro vytvoření a inicializaci tabulky 
    ptr=htab_lookup(t,key) vyhledávání - viz dále 
    htab_foreach(t,function) volání funkce pro každý prvek 
    htab_remove(t,key) vyhledání a zrušení zadané položky 
    htab_clear(t) zrušení všech položek v tabulce 
    htab_free(t) zrušení celé tabulky (volá clear) 
    htab_statistics(t) tisk průměrné a min/max. délky seznamů 
                       vtabulce - vhodné pro ladění a testování 
  ```
                       
kde t je ukazatel na tabulku (typu htab_t *), 
b je typu bool, 
ptr je ukazatel na záznam (položku tabulky), 
function je funkce s parametry (key,value) 
  
- Vhodně zvolte typy parametrů funkcí. 
- Zvažte, které z uvedených operací bude vhodné udělat inline a které ne. 
  
- Záznam [key,data,next] je typu 
```struct htab_listitem ```
a obsahuje položky: 
    key .... ukazatel na dynamicky alokovaný řetězec, 
    data ... počet výskytů a 
    next ... ukazatel na další záznam 
      
- Funkce htab_foreach(t,function) volá zadanou funkci pro každý prvek 
tabulky, obsah tabulky nemění. (Vhodné např. pro tisk obsahu.) 

- Funkce 
```struct htab_listitem * htab_lookup(htab_t *t, const char *key); ```
v tabulce t vyhledá záznam odpovídající řetězci key a 
    - pokud jej nalezne, vrátí ukazatel na záznam 
    - pokud nenalezne, automaticky přidá záznam a vrátí ukazatel 
Poznámka: Dobře promyslete chování této funkce k parametru key.

- Pokud htab_init nebo htab_lookup nemohou alokovat paměť, vrací NULL 

- Napište funkci 
   ``` int fgetw(char *s, int max, FILE *f); ```
    
která čte jedno slovo ze souboru f do zadaného pole znaků 
a vrátí délku slova (z delších slov načte prvních max-1 znaků, 
a zbytek přeskočí). Funkce vrací EOF, pokud je konec souboru. 
Umístěte ji do zvláštního modulu "io.c" (nepatří do knihovny). 
Poznámka: Slovo je souvislá posloupnost znaků oddělená isspace znaky. 

Omezení: řešení v C může tisknout jinak seřazený výstup 
    a je povoleno použít implementační limit na maximální 
    délku slova (zvolte 127 znaků), delší slova se ZKRÁTÍ a program 
    při prvním delším slovu vytiskne varování na stderr (max 1 varování).
    
Poznámka: Vhodný soubor pro testování je například seznam slov
          v souboru /usr/share/dict/words 
          nebo texty z http://www.gutenberg.org/ 
          případně výsledek příkazu: seq 1000000 2000000 
          
Použijte implicitní lokalizaci (= nevolat setlocale()). 
Napište soubor Makefile tak, aby příkaz make vytvořil programy 
"tail", "tail2", "wordcount", "wordcount-dynamic" a knihovny "libhtable.a", 
"libhtable.so" (nebo "htable.DLL"). 
Program "wordcount" musí být staticky sestaven s knihovnou "libhtable.a". 
Program "wordcount-dynamic" musí být dynamicky sestaven s knihovnou "libhtable.so". 
Tento program otestujte se stejnými vstupy jako u staticky sestavené verze. 


# IPP/JSN

### Zadanie:
Vytvořte skript pro konverzi JSON formátu (viz RFC 4627) do XML. Každému prvku z JSON formátu 
(objekt, pole, dvojice jméno-hodnota) bude odpovídat jeden párový element se jménem podle jména dvojice 
a obsahem podle hodnoty dvojice. Každé pole bude obaleno párovým elementem ``<array>`` a každý prvek pole bude obalen 
párovým elementem ``<item>``. JSON hodnoty typu string a number a JSON literály true, false a null budou transformovány
v závislosti na parametrech skriptu na atribut value daného elementu s odpovídající hodnotou (stejného tvaru jako v JSON vstupu; 
nezapomeňte na požadavek ouvozovkování atributů v XML) nebo na textový element v případě hodnoty typu string a number 
či na párový element (ve zkráceném zápise) ``<true/>``, ``<false/>`` a ``<null/>``. 
Je-li hodnotou desetinné číslo (případně s exponentem), bude na výstupu celé číslo zaokrouhleno dolů (čísla mimo meze neuvažujte).

Tento skript bude pracovat s těmito parametry:

* ``--help`` vypíše na standardní výstup nápovědu skriptu (nenačítá žádný vstup).
* ``--input=filename`` zadaný vstupní JSON soubor v kódování UTF-8
* ``--output=filename`` textový výstupní XML soubor v kódování UTF-8 s obsahem převedeným ze vstupního souboru
* ``-h=subst`` ve jméně elementu odvozeném z dvojice jméno-hodnota nahraďte každý nepovolený znak ve jméně XML 
               značky řetězcem subst. Implicitně (i při nezadaném parametru -h) uvažujte nahrazování znakem pomlčka (-).
               Vznikne-li po nahrazení invalidní jméno XML elementu, skončete s chybou a návratovým kódem 51.
* ``-n`` negenerovat XML hlavičku na výstup skriptu (vhodné například v případě kombinování více výsledků)
* ``-r=root-element`` jméno párového kořenového elementu obalujícího výsledek. Pokud nebude zadán, 
                      tak se výsledek neobaluje kořenovým elementem, ač to potenciálně porušuje validitu XML 
                      (skript neskončí s chybou). Zadání řetězce root-element vedoucího na nevalidní XML značku 
                      ukončí skript s chybou a návratovým kódem 50 (nevalidní znaky nenahrazujte).
* ``--array-name=array-element`` tento parametr umožní přejmenovat element obalující pole z implicitní hodnoty array 
                                 na array-element. Zadání řetězce array-element vedoucího na nevalidní XML značku ukončí 
                                 skript s chybou a návratovým kódem 50 (nevalidní znaky nenahrazujte).
* ``--item-name=item-element`` analogicky, tímto parametrem lze změnit jméno elementu pro prvky pole 
                               (implicitní hodnota je item). Zadání řetězce item-element vedoucího na nevalidní
                               XML značku ukončí skript s chybou a návratovým kódem 50 (nevalidní znaky nenahrazujte).
* ``-s`` hodnoty (v dvojici i v poli) typu string budou transformovány na textové elementy místo atributů.
* ``-i`` hodnoty (v dvojici i v poli) typu number budou transformovány na textové elementy místo atributů.
* ``-l`` hodnoty literálů (``true``, ``false``, ``null``) budou transformovány na elementy ``<true/>``, ``<false/>`` a ``<null/>`` místo na atributy.
* ``-c`` tento přepínač oproti implicitnímu chování aktivuje překlad problematických znaků. Pro XML 
         problematické znaky s UTF-8 kódem menším jak 128 ve vstup ních řetězcových hodnotách (ve dvojicích i polích) 
         konvertujte na odpovídající zápisy v XML pomocí metaznaku & (např. ``&amp;``, ``&lt;``, ``&gt;`` atd.). 
         Ostatní problematické znaky konvertovat nemusíte.
* ``-a, --array-size`` u pole bude doplněn atribut size s uvedením počtu prvků v tomto poli.
* ``-t, --index-items`` ke každému prvku pole bude přidán atribut index s určením indexu prvku v tomto poli 
                        (číslování začíná od 1, pokud není parametrem --start určeno jinak).
* ``--start=n`` inicializace inkrementálního čitače pro indexaci prvků pole (nutno kombinovat s parametrem --index-items, 
                jinak chyba s návratovým kódem 1) na zadané kladné celé číslo n včetně nuly (implicitně n = 1).

# IPP/MKA

### Zadanie:
Vytvořte skript pro zpracování a případnou minimalizaci konečného automatu. Skript bude zpracov-
ávat textový zápis zadaného konečného automatu, validovat, že je dobře specifikovaný, a generovat
ekvivalentní minimální konečný automat přesně podle algoritmu z 11. přednášky (snímek 23/35)
předmětu Formální jazyky a překladače (IFJ).
Komentáře do konce řádku začínají znakem #. Bílé znaky jako konec řádku (\n i \r), mezera či
tabulátor jsou ignorovány (až na později definované případy). Stav je reprezentován identifikátorem jazyka C, který nezač
íná ani nekončí podtržítkem. Vstupní symbol je reprezentovaný libovolným znakem uzavřeným v apostrofech.
U stavů i vstupních symbolů záleží na velikosti písmen. Jako vstupní symboly lze využít i metaznaky popisu automatu 
a všechny bílé znaky, tj. '(', ')', '{', '}', '''', '-', '>', ',', '.', '#', ' ', znak konce řádku v apostrofech
a znak tabulátoru v apostrofech. Apostrof musí být navíc uvnitř apostrofů zdvojený. Prázdná dvojice apostrofů ''
reprezentuje prázdný řetězec.
Celý konečný automat je zapisován podobnou notací jako ve formálních jazycích (IFJ). Celý konečný automat je pětice uzavřená do kulatých závorek. Každá komponenta kromě komponenty určující počáteční stav je dále uzavřena ve složených závorkách a od ostatních komponent oddělena čárkou. Jednotlivé prvky množin reprezentujících komponenty (opět kromě počátečního stavu) jsou
odděleny také čárkou.
Nejprve je definována konečná množina stavů, následuje neprázdná vstupní abeceda, poté definice množiny pravidel,
dále určení počátečního stavu a nakonec množina koncových stavů. Množina pravidel je popsána seznamem pravidel. Každé pravidlo je zapsáno ve tvaru: pa->q, kde p je výchozí stav, a je čtený vstupní symbol (případně reprezentace prázdného řetězce), následuje dvojznak pomlčka s většítkem reprezentující šipku (tento dvojznak nesmí být rozdělen jiným znakem) a poslední
část pravidla q určuje cílový stav. Příklad vstupního zápisu konečného automatu:
Příklad konečného automatu ve vstupním formátu úlohy MKA

```
({s, f,q4,q2 , q1, # nějaký komentář
q3}, # nějaký komentář
{'á', ')' }, {
s 'á'->f, f  'á' ->s, s')'->q3, s ')' -> q4,
q1 'á'->q1, q2'á' -> q2, q3 'á'->q4, q4 'á'->
q3, q1 ')' -> s, q2')'
->f , q3 ')'->q1, q4')'->q2  # další komentář
}, # následuje komponenta definující počáteční stav
s
, {f, s } ) # koncové stavy a ukončení definice automatu
\# zde může následovat libovolný počet bílých znaků nebo kome
ntářů.
```

* ``--help`` vypíše na standardní výstup nápovědu skriptu (nenačítá žádný vstup).
* ``--input=filename`` zadaný vstupní JSON soubor v kódování UTF-8
* ``--output=filename`` textový výstupní XML soubor v kódování UTF-8 s obsahem převedeným ze vstupního souboru
* ``-f, --find-non-finishing`` hledá  neukončující  stav  zadaného  dobře  specifikovaného konečného automatu (automat se
                               nevypisuje). Nalezne-li jej, bez odřádkování jej vypíše na výstup; jinak vypíše pouze
                               číslici 0. (Před hledáním se provede validace na dobrou specifikovanost automatu.) Parametr
                               nelze kombinovat s parametrem -m (resp.--minimize).
* ``-m, --minimize`` provede minimalizaci dobře specifikovaného konečného automatu, nelze kombinovat s parametrem
                    ``-f`` (resp.``--find-non-finishing``).    
* ``-i,--case-insensitive`` nebude brán ohled na velikost znaků při porovnávání symbolů či stavů (tj. a = A, ahoj = AhOj
                            nebo A b = a B); ve výstupu potom budou všechna velká písmena převedena na malá.
    Pokud nebude uveden parametr ``-m`` ani ``-f``, tak dojde pouze k validaci načteného dobře specifikovaného konečného automatu 
    a k jeho normalizovanému výpisu.
