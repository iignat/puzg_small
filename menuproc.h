#include <LiquidCrystal.h>

#define MAX_MENU 2

extern uint8_t currmenupos;
extern bool menuactive;



extern LiquidCrystal lcd;

void menuinit();
void menuprint();
void menunext();
void menuprev();
void menuprocess(uint8_t key);
