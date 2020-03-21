#include "menuproc.h"
#include "puzgfnc.h"

uint8_t currmenupos=1;
bool menuactive=false;



void menuinit() {
  
}

void menunext() {
  currmenupos++;
  if (currmenupos>MAX_MENU)currmenupos=1;
}

void menuprev() {
  currmenupos--;
  if (currmenupos<1)currmenupos=MAX_MENU;
}

void change_START_NUM(int val){
  START_NUM+=val;
  if (START_NUM<1)START_NUM=1;
}

void change_DIZEL_STARTER_TIME(int val){
  DIZEL_STARTER_TIME+=val;
  if(DIZEL_STARTER_TIME<1000)DIZEL_STARTER_TIME=1000;
}

void menuprint() {
  if (!menuactive)return;
  switch(currmenupos) {
    case 1:
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print(F("YCTAHOBKA"));
      lcd.setCursor(0, 1);
      lcd.print(F("KO"));lcd.write(3);lcd.print(F(". 3A"));lcd.write(4);lcd.print(F("YCK."));
      lcd.print(F(" "));lcd.print(START_NUM);
    break;
    case 2:
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print(F("YCTAHOBKA"));
      lcd.setCursor(0, 1);
      lcd.print(F("BPEM. 3A"));lcd.write(4);lcd.print(F("YCK."));
      lcd.print(F(" "));lcd.print(DIZEL_STARTER_TIME/1000);
    break;
  }
}

void menuprocess(uint8_t key){
  if (!menuactive)return;
  switch(key) {
    case 1:menunext();break;
    case 4:menuprev();break;
    case 2:
      if(currmenupos==1)change_START_NUM(1);
      if(currmenupos==2)change_DIZEL_STARTER_TIME(1000);
    break;
    case 3:
      if(currmenupos==1)change_START_NUM(-1);
      if(currmenupos==2)change_DIZEL_STARTER_TIME(-1000);
    break;
  }
}
