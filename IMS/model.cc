/**
 * Subor: model.cc
 * Autori: Michal Cyprian <xcypri01@stud.fit.vutbr.cz>, Filip Baric
 * <xbaric01@stud.fit.vutbr.cz>
 *
 * Predmet IMS 2016/17, tema projektu: 1. Model vyrobneho podniku
 *
 * Implementacia modelu strediska pre tvorbu, triedenie a naslednu distribuciu
 * zasielok
 * s vyuzitim kniznice SIMLIB v3.02.
 *
 */

#include <ctime>
#include <iostream>
#include <sstream>
#include <stdexcept>

#include <unistd.h>

#include "model.hh"

#define STANDARDNY_POCET_ZAMESTNANCOV 12
#define STANDARDNY_POCET_RAMP 2
#define POCET_PRIDAVNYCH_PASOV 2

#define KAPACITA_KOMPONENTY 1800
#define KAPACITA_SPRACOVANE 1600

using namespace std;

const char *SPUSTENIE =
    "Spustenie: ./model [VOLBA...]\n"
    "-z           pocet zamestnancov\n"
    "-k           kapacita triediaceho pasu\n"
    "-r           pocet ramp\n"
    "-t           stredna hodnota exponencialneho rozdelenia\n"
    "             pravdepodobnosti prichodu kamionov\n"
    "-p           pocet zamestnancov obsluhujucich pas\n"
    "-s           pocet zamestnancov obsluhujucich vstupny sken\n"
    "-b           pocet zamestnancov obsluhujucich proces balenia\n"
    "-f           vystupny subor\n"
    "-h           napoveda programu\n";

struct nastavenia nastavenia;
const debug_level DEBUG = nizky;

Store Zamestnanci("Zamestnanci depa", STANDARDNY_POCET_ZAMESTNANCOV);
Store Rampa("Nakladacie rampy", STANDARDNY_POCET_RAMP);

Stat CasKamionov("Doba kamionu stravena v stredisku");
Histogram PasPreplnenyHist("Doba pocas ktoreho bol pas preplneny", 8, 0.5, 10);
Histogram VstupPrazdnyHist("Doba pocas ktoreho bol vstup skenu prazdny", 20, 5,
                           10);

Semaphore ZamokKomponent("Zarucenie vylucneho pristupu ku komponentom");
Semaphore ZamokBalenia[3];
Queue FrontaNakladanie[3];
Queue FrontaBalenie[3];
Queue PasPreplneny[POCET_PRIDAVNYCH_PASOV + 1];
Queue VstupPrazdny("Fronta pre sken cakajuci na baliky na vstupe");

const double C_ZACIATOK = 0.0 * 60.0;
const double C_KONIEC = 20.0 * 60.0;
const double KONIEC_RUSH_HOUR = 15.0 * 60.0;

long prislo_kamionov = 0;
long odislo_kamionov = 0;

long priate_komponenty = 0;
long rozbite = 0;
long vstupny_sken = 0;
long vystupny_sken = 0;
long baliky = 0;
long roztriedene_komponenty[3] = {0, 0, 0};
long spracovane_baliky[3] = {0, 0, 0};

void Kamion::Behavior() {
  this->prichod = Time;
  Echo("prichod\n");
  prislo_kamionov++;
  /* Vykladanie kamiona */
  Enter(Rampa, 1);
  Echo("Enter rampa\n");
  Enter(Zamestnanci, 2);
  Echo("Enter zamestnanci\n");
  Echo("vykladanie\n");
  Wait(Uniform(20, 30));
  priate_komponenty += KAPACITA_KOMPONENTY;
  if (!VstupPrazdny.Empty()) {
    do {
      VstupPrazdny.GetFirst()->Activate();
    } while (!VstupPrazdny.Empty());
  }
  this->pocet = mozem_nakladat(this->smer);
  if (this->pocet < 0) {
    Leave(Rampa, 1);
    Leave(Zamestnanci, 2);
    Echo("cakanie na hotove baliky\n");
    this->Into(FrontaNakladanie[this->smer]);
    this->Passivate();
    /* Nakladanie kamiona */
    Enter(Zamestnanci, 2);
    Enter(Rampa, 1);
  } else {
    Echo("okamzite nakladanie\n");
  }
  Wait(Uniform(20, 30));
  debug_print(stredny, "KAMION nalozene (%d) : %ld\n", this->smer, this->pocet);
  Leave(Rampa, 1);
  Leave(Zamestnanci, 2);
  odislo_kamionov++;
  CasKamionov(Time - this->prichod);
  Echo("odchod\n");
}

void Kamion::Echo(const char *msg) {
  if (DEBUG <= stredny) {
    Print("Cas: %d:%d KAMION (%d): %s", int(Time / 60), int(Time) % 60,
          this->smer, msg);
  }
}

Kamion::Kamion(smery smer) {
  this->smer = smer;
  this->pocet = 0;
}

void Kamion::nastavPocet(double pocet) {
  this->pocet = pocet;
  debug_print(vysoky, "Pocet zmeneny %ld\n", this->pocet);
}

void PrichodKamionov::Behavior() {
  double rand = Random();
  smery smer = ((rand < 0.2) ? zapad : ((rand < 0.55) ? vychod : stred));
  (new Kamion(smer))->Activate();
  if (Time > KONIEC_RUSH_HOUR) {
    debug_print(stredny, "Po rush hour", nullptr);
    this->Activate(Time + Exponential(4 * this->time));
  } else {
    this->Activate(Time + Exponential(this->time));
  }
}

PrichodKamionov::PrichodKamionov(double time) { this->time = time; }

int Pas::Id() { return this->id; }
bool Pas::Bezi() { return this->bezi; }
long Pas::pocet() { return this->komponenty; }

Pas::Pas(bool kontrola, int id) {
  this->id = id;
  this->kontrola = kontrola;
  this->komponenty = 0;
  this->Activate();
  this->bezi = true;
}
void Pas::dodavka(long pocet) { this->komponenty += pocet; }

void Pas::Behavior() {
  double rand;
  smery smer;
  Enter(Zamestnanci, nastavenia.zamestnanci_pas);
  for (;;) {
    if (this->komponenty == 0) { /* Prazdny pas  - cakanie */
      Leave(Zamestnanci, nastavenia.zamestnanci_pas);
      this->bezi = false;
      this->Passivate();
      this->bezi = true;
      debug_print(vysoky, "PAS restart\n", nullptr);
      Enter(Zamestnanci, nastavenia.zamestnanci_pas);
    }
    Wait(Uniform(0.3 / nastavenia.zamestnanci_pas,
                 0.4 / nastavenia.zamestnanci_pas));
    if (Random() < 0.001) {
      rozbite++;
      this->komponenty--;
    } else {
      this->komponenty--;
      if (this->komponenty <
              nastavenia.kapacita_pasu - nastavenia.kapacita_pasu / 10 &&
          !PasPreplneny[this->id].Empty()) {
        do {
          debug_print(vysoky, "Pas aktivacia skenov\n", nullptr);
          PasPreplneny[this->id].GetFirst()->Activate();
        } while (!PasPreplneny[this->id].Empty());
      }
      rand = Random();
      /* Triedenie na smery */
      smer = ((rand < 0.15) ? zapad : ((rand < 0.5) ? vychod : stred));
      roztriedene_komponenty[smer]++;
      if (this->kontrola && (roztriedene_komponenty[smer] > 12 * 2) &&
          !FrontaBalenie[smer].Empty()) {
        do {
          (FrontaBalenie[smer].GetFirst())->Activate();
        } while (!FrontaBalenie[smer].Empty());
      }
    }
  }
}

Balenie::Balenie(smery smer) { this->smer = smer; }

void Balenie::Behavior() {
  int pocet_komponent;
  double rand;
  Enter(Zamestnanci, nastavenia.zamestnanci_balenie);
  for (;;) {
  balenie_restart:
    ZamokBalenia[this->smer].P();
    if (roztriedene_komponenty[this->smer] == 0) {
      debug_print(vysoky, "Balenie (%d) cakanie\n", this->smer);
      Leave(Zamestnanci, nastavenia.zamestnanci_balenie);
      this->Into(FrontaBalenie[this->smer]);
      ZamokBalenia[this->smer].V();
      this->Passivate();
      debug_print(vysoky, "Balenie (%d) restart\n", this->smer);
      Enter(Zamestnanci, nastavenia.zamestnanci_balenie);
      goto balenie_restart;
    } else {
      if (roztriedene_komponenty[this->smer] < 12) {
        pocet_komponent = 1;
      } else {
        rand = Random();
        /* Vyber poctu komponent */
        pocet_komponent =
            ((rand < 0.2) ? 1 : ((rand < 0.4) ? 2 : ((rand < 0.7) ? 3 : 5)));
      }
      roztriedene_komponenty[this->smer] -= pocet_komponent;
      ZamokBalenia[this->smer].V();
    }
    Wait(Uniform(0.3, 0.5));
    vystupny_sken += pocet_komponent;
    spracovane_baliky[this->smer]++;
    baliky++;
  }
}

void ManagerAktivator::Behavior() {
  (new Manager(vychod))->Activate();
  (new Manager(stred))->Activate();
  (new Manager(zapad))->Activate();
  this->Activate(Time + 5);
}

Manager::Manager(smery smer) { this->smer = smer; }

void Manager::Behavior() {
  Kamion *k;
  long ret = 0;
  if (!FrontaNakladanie[this->smer].Empty()) {
    if ((ret = mozem_nakladat(this->smer)) > 0) {
      debug_print(vysoky, "MANAGER posielam kamion\n", nullptr);
      debug_print(stredny,
                  "Cas: %d:%d manager: spracovane_baliky %ld priate %ld\n",
                  int(Time / 60), int(Time) % 60, spracovane_baliky[this->smer],
                  priate_komponenty);
      k = (Kamion *)FrontaNakladanie[this->smer].GetFirst();
      k->nastavPocet(ret);
      k->Activate();
    }
  }
}

VstupnySken::VstupnySken(Pas *pas) {
  this->pas = pas;
  this->Activate();
  this->bezi = true;
}

bool VstupnySken::VstupnySken::Bezi() { return this->bezi; }

void VstupnySken::Behavior() {
  Enter(Zamestnanci, nastavenia.zamestnanci_sken);
  for (;;) {
  vstup_restart:
    if (this->pas->pocet() > nastavenia.kapacita_pasu) {
      /* Pas je preplneny */
      cas_preplneny = Time;
      debug_print(vysoky, "VSTUP: pas preplneny\n", nullptr);
      Leave(Zamestnanci, nastavenia.zamestnanci_sken);
      this->bezi = false;
      this->Into(PasPreplneny[this->pas->Id()]);
      this->Passivate();
      Enter(Zamestnanci, nastavenia.zamestnanci_sken);
      debug_print(vysoky, "VSTUP: restart\n", nullptr);
      PasPreplnenyHist(Time - cas_preplneny);
    }
    ZamokKomponent.P();
    if (priate_komponenty == 0) {
      /* Vstup je prazdny */
      this->cas_prazdny = Time;
      Leave(Zamestnanci, nastavenia.zamestnanci_sken);
      this->bezi = false;
      debug_print(vysoky, "SCAN: sleep\n", nullptr);
      this->Into(VstupPrazdny);
      ZamokKomponent.V();
      this->Passivate();
      debug_print(vysoky, "SCAN: woke up\n", nullptr);
      Enter(Zamestnanci, nastavenia.zamestnanci_sken);
      VstupPrazdnyHist(Time - cas_prazdny);
      goto vstup_restart;
    } else {
      priate_komponenty--;
      ZamokKomponent.V();
    }
    Wait(Uniform(0.05, 0.08));
    vstupny_sken++;
    this->pas->dodavka(1);
    if (!this->pas->Bezi() && this->pas->pocet() > 5) {
      debug_print(stredny, "Starting pas\n", nullptr);
      this->pas->Activate();
    }
  }
}

bool vsetko_spracovane(void) {
  return (((vstupny_sken == vystupny_sken) && (!priate_komponenty)) ||
          (Time > KONIEC_RUSH_HOUR));
}

long mozem_nakladat(smery smer) {
  if (spracovane_baliky[smer] >= KAPACITA_SPRACOVANE ||
      (spracovane_baliky[smer] >= KAPACITA_SPRACOVANE / 2 &&
       vsetko_spracovane())) {
    return ziskaj_pocet(smer);
  } else {
    return -1;
  }
}

long ziskaj_pocet(smery smer) {
  long pocet = (spracovane_baliky[smer] >= KAPACITA_SPRACOVANE)
                   ? KAPACITA_SPRACOVANE
                   : spracovane_baliky[smer];
  debug_print(stredny, "Ziskany Pocet %ld\n", pocet);
  spracovane_baliky[smer] -= pocet;
  return pocet;
}

/* Prevod retazca na cele cislo, v pripade neuspechu vyhadzuje vynimku
 * runtime_error.
 */
int str_to_number(const string &data) {
  if (!data.empty()) {
    int data_number;
    istringstream iss(data);
    iss >> dec >> data_number;
    if (iss.fail()) throw runtime_error("Nespravne argumenty programu.");
    return data_number;
  }
  throw runtime_error("Nespravne argumenty programu.");
}

/* Spracovanie argumentov a nastavenie konfiguracie pred simulaciou. */
int nastavenie_konfiguracie(int argc, char **argv) {
  int option_char;
  nastavenia.pocet_zamestnancov = STANDARDNY_POCET_ZAMESTNANCOV;
  nastavenia.pocet_ramp = STANDARDNY_POCET_RAMP;
  nastavenia.kapacita_pasu = 300;
  nastavenia.cas_prichodu = 2.5 * 60;
  nastavenia.zamestnanci_pas = 2;
  nastavenia.zamestnanci_sken = 1;
  nastavenia.zamestnanci_balenie = 1;
  nastavenia.balenie_vychod = 3;
  nastavenia.balenie_stred = 3;
  nastavenia.balenie_zapad = 2;
  nastavenia.output = "model.out";

  while ((option_char = getopt(argc, argv, "z:k:r:t:p:s:b:f:h")) != -1) {
    switch (option_char) {
      case 'z':
        nastavenia.pocet_zamestnancov = str_to_number(optarg);
        break;
      case 'k':
        nastavenia.kapacita_pasu = str_to_number(optarg);
        break;
      case 'r':
        nastavenia.pocet_ramp = str_to_number(optarg);
        break;
      case 't':
        nastavenia.cas_prichodu = str_to_number(optarg);
        break;
      case 'p':
        nastavenia.zamestnanci_pas = str_to_number(optarg);
        break;
      case 's':
        nastavenia.zamestnanci_sken = str_to_number(optarg);
        break;
      case 'b':
        nastavenia.zamestnanci_balenie = str_to_number(optarg);
        break;
      case 'f':
        nastavenia.output = optarg;
        break;
      case 'h':
        cout << SPUSTENIE;
        return 1;
        break;
    }
  }
  return 0;
}

int main(int argc, char **argv) {
  try {
    if (nastavenie_konfiguracie(argc, argv)) return 0;
  } catch (exception &e) {
    cerr << e.what() << endl;
    cout << SPUSTENIE;
    return 1;
  }

  Init(C_ZACIATOK, C_KONIEC);
  RandomSeed(time(nullptr));
  Pas *pas = new Pas(true, 0);
  Pas *pridavne_pasy[POCET_PRIDAVNYCH_PASOV];
  Zamestnanci.SetCapacity(nastavenia.pocet_zamestnancov);
  Rampa.SetCapacity(nastavenia.pocet_ramp);

  new VstupnySken(pas);
  new VstupnySken(pas);

  for (int i = 0; i < POCET_PRIDAVNYCH_PASOV; i++) {
    pridavne_pasy[i] = new Pas(false, i + 1);
    new VstupnySken(pridavne_pasy[i]);
    new VstupnySken(pridavne_pasy[i]);
  }

  (new ManagerAktivator())->Activate();

  // Baliky z predchadzajuceho dna
  spracovane_baliky[vychod] = 1500 * Random();
  spracovane_baliky[stred] = 1500 * Random();
  spracovane_baliky[zapad] = 1500 * Random();

  (new PrichodKamionov(nastavenia.cas_prichodu))->Activate();
  for (int i = 0; i < nastavenia.balenie_vychod; i++) {
    (new Balenie(vychod))->Activate();
  }
  for (int i = 0; i < nastavenia.balenie_stred; i++) {
    (new Balenie(stred))->Activate();
  }
  for (int i = 0; i < nastavenia.balenie_zapad; i++) {
    (new Balenie(zapad))->Activate();
  }
  SetOutput(nastavenia.output.c_str());
  /* Vypis konfiguracie */
  debug_print(nizky, "START PRE KONFIGURACIU:\n", nullptr);
  debug_print(nizky, "Pocet zamestnancov: %ld\n",
              nastavenia.pocet_zamestnancov);
  debug_print(nizky, "Kapacita pasu : %ld\n", nastavenia.kapacita_pasu);
  debug_print(nizky, "Pocet ramp: %ld\n", nastavenia.pocet_ramp);
  debug_print(nizky, "Cas prichodu kamionov: %ld min\n",
              nastavenia.cas_prichodu);
  debug_print(nizky, "Pocet zamestnancov obsluhujucich pas: %ld\n",
              nastavenia.zamestnanci_pas);
  debug_print(nizky, "Pocet zamestnancov obsluhujucich sken: %ld\n",
              nastavenia.zamestnanci_sken);
  debug_print(nizky, "Pocet zamestnancov obsluhujucich balenie: %ld\n",
              nastavenia.zamestnanci_balenie);
  debug_print(nizky, "Pocet baliacich procesov vychod: %ld\n",
              nastavenia.balenie_vychod);
  debug_print(nizky, "Pocet baliacich procesov stred: %ld\n",
              nastavenia.balenie_stred);
  debug_print(nizky, "Pocet baliacich procesov zapad: %ld\n",
              nastavenia.balenie_zapad);
  debug_print(nizky, "Pocet pridavnych pasov: %ld\n", POCET_PRIDAVNYCH_PASOV);

  Run();
  /* Vypis statistik simulacie */
  CasKamionov.Output();
  PasPreplnenyHist.Output();
  VstupPrazdnyHist.Output();
  Zamestnanci.Output();
  Rampa.Output();
  debug_print(nizky, "Pocet Kamionov:\n    prislo: %ld\n    odislo: %ld\n",
              prislo_kamionov, odislo_kamionov);
  debug_print(nizky, "Komponenty:\n    priate %ld\n    na pase %ld\n",
              priate_komponenty, pas->pocet());

  for (int i = 0; i < POCET_PRIDAVNYCH_PASOV; i++) {
    debug_print(nizky, "    Na pridavnom pase %ld\n",
                pridavne_pasy[i]->pocet());
  }
  debug_print(nizky,
              "Roztriedene:\n    vychod: %ld\n    stred %ld\n    zapad %ld\n",
              roztriedene_komponenty[vychod], roztriedene_komponenty[stred],
              roztriedene_komponenty[zapad]);
  debug_print(nizky,
              "Spracovane:\n    vychod: %ld\n    stred %ld\n    zapad %ld\n",
              spracovane_baliky[vychod], spracovane_baliky[stred],
              spracovane_baliky[zapad]);

  debug_print(nizky, "Vstupny sken: %ld vystupny sken %ld\n", vstupny_sken,
              vystupny_sken);
  debug_print(nizky, "Rozbite %ld\n", rozbite);
  debug_print(nizky, "Spracovane %.2lf%\n",
              float(vystupny_sken + rozbite) /
                  float((priate_komponenty + vstupny_sken) / 100));
  return 0;
}
