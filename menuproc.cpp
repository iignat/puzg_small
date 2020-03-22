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

void change_actconfidx(int val){
  actconfidx+=val;
  if(actconfidx<0)actconfidx=0;
  if(actconfidx>100)actconfidx=50;
}

void change_OSTANOV_GENERATORA1_CNT(int val){
  OSTANOV_GENERATORA1_CNT+=val;
  if(OSTANOV_GENERATORA1_CNT<0)actconfidx=0;
  if(OSTANOV_GENERATORA1_CNT>=OSTANOV_GENERATORA2_CNT)OSTANOV_GENERATORA1_CNT=OSTANOV_GENERATORA2_CNT-1;
}

void change_OSTANOV_GENERATORA2_CNT(int val){
  OSTANOV_GENERATORA2_CNT+=val;
  if(OSTANOV_GENERATORA2_CNT<=OSTANOV_GENERATORA1_CNT)OSTANOV_GENERATORA2_CNT=OSTANOV_GENERATORA1_CNT+1;
}

void change_OSNOVNAYA_GENERATOR_SWH_DELAY(int val){
  OSNOVNAYA_GENERATOR_SWH_DELAY+=val;
  if(OSNOVNAYA_GENERATOR_SWH_DELAY<0)OSNOVNAYA_GENERATOR_SWH_DELAY=0;
}

void menuprint() {
  if (!menuactive)return;
  switch(currmenupos) {
    case ID_START_NUM:
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print(F("YCTAHOBKA"));
      lcd.setCursor(0, 1);
      lcd.print(F("KO"));lcd.write(3);lcd.print(F(". 3A"));lcd.write(4);lcd.print(F("YCK."));
      lcd.print(F(" "));lcd.print(START_NUM);
    break;
    case ID_DIZEL_STARTER_TIME:
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print(F("YCTAHOBKA"));
      lcd.setCursor(0, 1);
      lcd.print(F("BPEM"));lcd.write(5);lcd.print(F(" 3A"));lcd.write(4);lcd.print(F("YCKA"));
      lcd.print(F(" "));lcd.print(DIZEL_STARTER_TIME/1000);
    break;
    case ID_ACTCONFIDX:
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print(F("YCTAHOBKA"));
      lcd.setCursor(0, 1);
      lcd.print(F("AKT"));lcd.write(1);lcd.print(F("BH. KOH"));lcd.write(7);lcd.print(F("."));
      lcd.print(F(" "));lcd.print(actconfidx);
    break;
    case ID_OSTANOV_GENERATORA1_CNT:
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print(F("YCTAHOBKA"));
      lcd.setCursor(0, 1);
      lcd.print(F("BPEM"));lcd.write(5);lcd.print(F(" OTK"));lcd.write(3);lcd.print(F(".A"));
      lcd.print(F(" "));lcd.print(OSTANOV_GENERATORA1_CNT);
    break;
    case ID_OSTANOV_GENERATORA2_CNT:
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print(F("YCTAHOBKA"));
      lcd.setCursor(0, 1);
      lcd.print(F("BPEM"));lcd.write(5);lcd.print(F(" OTK"));lcd.write(3);lcd.print(F(".B"));
      lcd.print(F(" "));lcd.print(OSTANOV_GENERATORA2_CNT);
    break;
    case ID_OSNOVNAYA_GENERATOR_SWH_DELAY:
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print(F("YCTAHOBKA"));
      lcd.setCursor(0, 1);
      lcd.print(F("BPEM"));lcd.write(5);lcd.print(F(" "));lcd.write(4);lcd.print(F("EPEK"));lcd.write(3);lcd.print(F("."));
      lcd.print(F(" "));lcd.print(OSNOVNAYA_GENERATOR_SWH_DELAY/1000);
    break;
  }
}

void menuprocess(uint8_t key){
  if (!menuactive)return;
  switch(key) {
    case 1:menunext();break;
    case 4:menuprev();break;
    case 2:
      if(currmenupos==ID_START_NUM)change_START_NUM(1);
      if(currmenupos==ID_DIZEL_STARTER_TIME)change_DIZEL_STARTER_TIME(1000);
      if(currmenupos==ID_ACTCONFIDX)change_actconfidx(1);
      if(currmenupos==ID_OSTANOV_GENERATORA1_CNT)change_OSTANOV_GENERATORA1_CNT(1);
      if(currmenupos==ID_OSTANOV_GENERATORA2_CNT)change_OSTANOV_GENERATORA2_CNT(1);
      if(currmenupos==ID_OSNOVNAYA_GENERATOR_SWH_DELAY)change_OSNOVNAYA_GENERATOR_SWH_DELAY(1000);
      
    break;
    case 3:
      if(currmenupos==ID_START_NUM)change_START_NUM(-1);
      if(currmenupos==ID_DIZEL_STARTER_TIME)change_DIZEL_STARTER_TIME(-1000);
      if(currmenupos==ID_ACTCONFIDX)change_actconfidx(-1);
      if(currmenupos==ID_OSTANOV_GENERATORA1_CNT)change_OSTANOV_GENERATORA1_CNT(-1);
      if(currmenupos==ID_OSTANOV_GENERATORA2_CNT)change_OSTANOV_GENERATORA2_CNT(-1);
      if(currmenupos==ID_OSNOVNAYA_GENERATOR_SWH_DELAY)change_OSNOVNAYA_GENERATOR_SWH_DELAY(-1000);
    break;
  }
}
