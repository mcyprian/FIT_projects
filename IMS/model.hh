/**
 * Subor: model.hh
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

#include <stdio.h>
#include <cstdarg>
#include <string>

#include "simlib.h"

enum smery { vychod = 0, stred, zapad };
enum debug_level { vysoky, stredny, nizky };

#define debug_print(level, fmt, ...) \
  {                                  \
    if (level >= DEBUG) {            \
      Print(fmt, ##__VA_ARGS__);     \
    }                                \
  }

/* Konfiguracia simulacie */
struct nastavenia {
  long pocet_zamestnancov;
  long kapacita_pasu;
  long pocet_ramp;
  long cas_prichodu;
  long zamestnanci_pas;
  long zamestnanci_sken;
  long zamestnanci_balenie;
  long balenie_vychod;
  long balenie_stred;
  long balenie_zapad;
  std::string output;
};

/* Proces monitoruje pocet spracovanych balikov na jednotlive smery
 * a vydava prikaz na zaciatok nakladania kamiona.
 */
class Manager : public Process {
  smery smer;
  void Behavior();

 public:
  Manager(smery smer);
  Manager(){};
  ~Manager(){};
};

/* Aktivuje proces Manager v pravidelnych intervaloch. */
class ManagerAktivator : public Event {
  void Behavior();
};

/* Proces ma na starosti triedenie komponent na tri smery. */
class Pas : public Process {
  long komponenty;
  bool bezi;
  int id;
  bool kontrola;
  void Behavior();

 public:
  Pas(bool kontrola, int id);
  Pas(){};
  ~Pas(){};
  void dodavka(long pocet);
  long pocet();
  bool Bezi();
  int Id();
};

/* Proces modeluje vstupne skenovanie vylozenych komponenet, zasobuje triediaci
 * pas. */
class VstupnySken : public Process {
  Pas *pas;
  double cas_preplneny;
  bool bezi;
  double cas_prazdny;
  void Behavior();

 public:
  VstupnySken(Pas *pas);
  VstupnySken(){};
  ~VstupnySken(){};
  bool Bezi();
};

/* Proces modeluje prichadzajuci kamion, ktory vylozi komponenty a
 * vyckava na dostatocny pocet spracovanych balikov na jeho smer.
 */
class Kamion : public Process {
  double prichod;
  long pocet;
  smery smer;
  void Behavior();
  void Echo(const char *msg);

 public:
  Kamion(smery smer);
  Kamion(){};
  ~Kamion(){};
  void nastavPocet(double pocet);
};

/* Generator prichodu kamionov. */
class PrichodKamionov : public Event {
  double time;
  void Behavior();

 public:
  PrichodKamionov(double time);
  PrichodKamionov(){};
  ~PrichodKamionov(){};
};

/* Proces tvorby hotovych balikov z premenliveho pocty roztriedenych komponent.
 */
class Balenie : public Process {
  smery smer;
  void Behavior();

 public:
  Balenie(smery smer);
  Balenie(){};
  ~Balenie(){};
};

/* Funkcia poskytuje informaciu o tom ci boli vsetky baliky v systeme
 * spracovane. */
bool vsetko_spracovane(void);
/* Urcuje pocet balikov ktore budu nalozene do kamiona, ak nakladanie nie je
 * mozne
 * vracia hodnotu -1.
 */
long mozem_nakladat(smery smer);
/* Funkcia vyuzivana funkciou mozem_nakladat, meni pocet spracovanych balikov.
 */
long ziskaj_pocet(smery smer);
