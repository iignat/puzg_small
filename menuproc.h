#include <LiquidCrystal.h>

#define MAX_MENU 6

#define ID_START_NUM 2
#define ID_DIZEL_STARTER_TIME 3
#define ID_ACTCONFIDX 1
#define ID_OSTANOV_GENERATORA1_CNT 4
#define ID_OSTANOV_GENERATORA2_CNT 5
#define ID_OSNOVNAYA_GENERATOR_SWH_DELAY 6

extern uint8_t currmenupos;
extern bool menuactive;
extern unsigned int actconfidx;



extern LiquidCrystal lcd;

void menuinit();
void menuprint();
void menunext();
void menuprev();
void menuprocess(uint8_t key);
