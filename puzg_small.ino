#define F_CPU 16000000L
// include the library code:
#include <LiquidCrystal.h>  
#include <SoftwareSerial.h>
#include "puzgfnc.h"
#include "gsmfnc.h"
#include "gprsfnc.h"
#include "timer-api.h"
#include "menuproc.h"
#include "sigproc.h"
#include <EEPROM.h>

//#define GSMMODULE 1
#define SIG_LEN 250

//LiquidCrystal lcd(3, 2, 28, 27, 26, 25);
LiquidCrystal lcd(8, 9, 4, 5, 6, 7);
SoftwareSerial gsm(0, 1); // RX, TX

float Vavg=0;
float Havg=0;
unsigned short avgidx=0;

unsigned short a1=0,a2=0,a3=0;
float v_max=0;
byte sig_arr_flag=0;
unsigned long start_time = 0;
unsigned long end_time = 0;
unsigned long _time = 0;

float Ak=0,Bk=0,Ck=0;

short gprsupdcnt=0;

struct EEPROMObj_t{
  uint8_t START_NUM;
  unsigned int DIZEL_STARTER_TIME;
  uint16_t OSTANOV_GENERATORA1_CNT;
  uint16_t OSTANOV_GENERATORA2_CNT;
  unsigned int OSNOVNAYA_GENERATOR_SWH_DELAY;
} EEPROMObj;
unsigned int actconfidx;

unsigned int Vin=0;
unsigned int HZin=0;

byte rusLetterB[8]= {0x1F,0x11,0x10,0x1E,0x11,0x11,0x1E,0};//Р‘
//byte rusLetterG[8]= {0x1F,0x11,0x10,0x10,0x10,0x10,0x10,0};//Р“
//byte rusLetterD[8]= {0x0F,0x05,0x05,0x05,0x09,0x11,0x1F,0x11};//Р”
byte rusLetterGH[8]= {0x15,0x15,0x15,0x0E,0x15,0x15,0x15,0};//Р–
byte rusLetterI[8]= {0x11,0x11,0x13,0x15,0x19,0x11,0x11,0};//Р�
//byte rusLetterIY[8]= {0x0A,0x15,0x13,0x15,0x19,0x11,0x11,0};//Р™
byte rusLetterL[8]= {0x0F,0x05,0x05,0x05,0x05,0x15,0x09,0}; //Р›
byte rusLetterP[8]= {0x1F,0x11,0x11,0x11,0x11,0x11,0x11,0};//Рџ
byte rusLetterF[8]= {0x04,0x0E,0x15,0x15,0x15,0x0E,0x04,0};//Р¤
//byte rusLetterC[8]= {0x11,0x11,0x11,0x11,0x11,0x11,0x1F,1};//Р¦
//byte rusLetterCH[8]= {0x11,0x11,0x11,0x0F,0x01,0x01,0x01,0};//Р§
byte rusLetterSH[8]= {0x15,0x15,0x15,0x15,0x15,0x15,0x1F,0};//РЁ
//byte rusLetterSCH[8] = {0x15,0x15,0x15,0x15,0x15,0x15,0x1F,1};//Р©
//byte rusLetterTZN[8] = {0x18,0x08,0x08,0x0E,0x09,0x09,0x0E,0};//РЄ
//byte rusLetterEI[8]= {0x11,0x11,0x11,0x19,0x15,0x15,0x19,0};//Р«
//byte rusLetterEE[8]= {0x0E,0x11,0x01,0x07,0x01,0x11,0x0E,0};//Р­
//byte rusLetterYU[8]= {0x12,0x15,0x15,0x1D,0x15,0x15,0x12,0};//Р®
byte rusLetterYA[8]= {0x0F,0x11,0x11,0x0F,0x05,0x09,0x11,0};//РЇ

byte start_err_notification=0;
byte f_force_osnovnaya_fail=0;

byte getKey() {
  short x = analogRead (0);
  if (x < 100) {
    return 1; //0-100 RIGHT
  } else if (x < 200) {
    return 2; //100-200 UP
  } else if (x < 400){
    return 3; //200-400 DOWN
  } else if (x < 600){
    return 4; //400-600 LEFT
  } else if (x < 800){
    return 5; //600-800 SELECT
  }
  return 0;
}

bool isacchanged(unsigned int idx){
  unsigned int e_idx;
  EEPROM.get(0,e_idx);
  if(e_idx!=idx)return true;
  return false;
}

bool isnewconfig(unsigned int idx){
  EEPROMObj_t o1;
  uint8_t i=0;
  EEPROM.get(sizeof(idx)+sizeof(o1)*idx,o1);

  for(i=0;i<sizeof(o1);i++){
    if( ((uint8_t *)&o1)[i]!=255)return false;
  }
  return true;
}

void GetCurrInfo(String *text){
    const String s1="PRINUDITELNO ";
    const String s2=" GENERATORA";
    const String s3="NORMA";
    const String s4="\r\n";
    
    byte f_osnovnaya=0,f_generator=0;
    f_osnovnaya=digitalRead(OSNOVNAYA);
    f_generator=digitalRead(GENERATOR);  

    text[0]+="Osnovnaya set':";
    if(f_osnovnaya==1)text[0]+=s3;
    else text[0]+="OTKLUCHENA";
    text[0]+=s4;
    
    text[0]+="Generator:";
    if(f_generator==1)text[0]+=s3;
    else text[0]+="OTKLUCHEN";
    text[0]+=s4;
    text[0]+="Regim raboty:";
    switch(curr_state){
      case SET_OSNOVNAYA:text[0]+=s3;break;
      case SET_GENERATORA:text[0]+=s2.substring(0,s2.length()-1);break;
      case ZAPUSK_GENERATORA:text[0]+=("ZAPUSK"+s2);break;
      case OSHIBKA_ZAPUSKA :text[0]+=("OSHIBKA ZAPUSKA")+s2;break;
      case OSTANOV_GENERATORA :text[0]+=("OTKLUCHENIE")+s2;break;
      case PROPALA_OSNOVNAYA_SET:text[0]+="PROPALA OSNOVNAYA SET'";break;
      case FORCE_GENERATOR:text[0]+=(s1+"GENERATOR");break;
      case FORCE_OSNOVNAYA:text[0]+=(s1+"OSNOVNAYA SET'");break;
      case FORCE_NOPOWER:text[0]+=(s1+"OTKLUCHENIE ENERGII");break;
      default:text[0]+="OSHIBKA !!!";break;  
    }  
    text[0]+=s4;
    
}

void printCurState() {
   lcd.setCursor(0, 1);
   switch(curr_state) {
     case SET_OSNOVNAYA:lcd.print(F("HOPMA           "));break;     
     case SET_GENERATORA:        lcd.print(F("PE3EPB          "));break;
     case ZAPUSK_GENERATORA:     lcd.print(F("CTAPT... #"));lcd.print(starts_try_num);lcd.print(F("      "));break;
     case OSHIBKA_ZAPUSKA:       lcd.print(F("O"));lcd.write(byte(0));lcd.write(byte(1));lcd.write(byte(2));lcd.print(F("KA CTAPTA   "));break;
     case OSTANOV_GENERATORA:    lcd.print(F("OCTAHOBKA...    "));break;
     case PROPALA_OSNOVNAYA_SET: lcd.print(F("HET HA"));lcd.write(byte(4));lcd.print("P");lcd.write(byte(5));lcd.write(byte(6));lcd.print(F("EH"));lcd.write(byte(1));lcd.write(byte(5));lcd.print(F("   "));break;
     case FORCE_GENERATOR:lcd.write(byte(4));lcd.print(F("P"));lcd.write(byte(1));lcd.print(F("H. PE"));lcd.write(byte(6));lcd.write(byte(1));lcd.print(F("M    1"));break;
     case FORCE_OSNOVNAYA:lcd.write(byte(4));lcd.print(F("P"));lcd.write(byte(1));lcd.print(F("H. PE"));lcd.write(byte(6));lcd.write(byte(1));lcd.print(F("M    2"));break;
     case FORCE_NOPOWER:lcd.write(byte(4));lcd.print(F("P"));lcd.write(byte(1));lcd.print(F("H. PE"));lcd.write(byte(6));lcd.write(byte(1));lcd.print(F("M    3"));break;
     case GLOBAL_ERROR:lcd.print(F("ERROR !!!"));break;
   } 
}

void setup() {

  float x1=745.0;
  float y1=250.0;

  float x2=700.0;
  float y2=200.0;

  float x3=651.0;
  float y3=150.0;
  
  //float x2=720.0;
  //float y2=220.0;

  //float x3=700.0;
  //float y3=200.0;

  float d=det(x1*x1,x1,1,x2*x2,x2,1,x3*x3,x3,1);

  Ak=det(y1,x1,1,y2,x2,1,y3,x3,1)/d;
  Bk=det(x1*x1,y1,1,x2*x2,y2,1,x3*x3,y3,1)/d;
  Ck=det(x1*x1,x1,y1,x2*x2,x2,y2,x3*x3,x3,y3)/d;
  
  // set up the LCD's number of columns and rows:
  pinMode(A0,INPUT);
  pinMode(A1,INPUT);
  pinMode(OSNOVNAYA,INPUT);
  pinMode(GENERATOR,INPUT);
  
   
  pinMode(OSNOVNAYA_SWCH,OUTPUT);
  pinMode(GENERATOR_ON,OUTPUT);
  pinMode(GENERATOR_OFF,OUTPUT);
  pinMode(GENERATOR_SWCH,OUTPUT);

  
  EEPROM.get(0,actconfidx);
  EEPROM.get(sizeof(actconfidx)+sizeof(EEPROMObj)*actconfidx,EEPROMObj);
  DIZEL_STARTER_TIME=EEPROMObj.DIZEL_STARTER_TIME;
  START_NUM=EEPROMObj.START_NUM;
  OSTANOV_GENERATORA1_CNT=EEPROMObj.OSTANOV_GENERATORA1_CNT;
  OSTANOV_GENERATORA2_CNT=EEPROMObj.OSTANOV_GENERATORA2_CNT;
  OSNOVNAYA_GENERATOR_SWH_DELAY=EEPROMObj.OSNOVNAYA_GENERATOR_SWH_DELAY;
  
  pins_init();
  //delay(1000);
  Serial.begin(9600);

#ifdef GSMMODULE
  gsm.begin(57600);
  gprsinit(); 
#endif

  ClearSerial();
  
  //pinMode(0,INPUT_PULLUP);
  //pinMode(1,INPUT_PULLUP);
  //pinMode(14,INPUT_PULLUP);
  //pinMode(15,INPUT_PULLUP);
  //pinMode(16,INPUT_PULLUP);
 // pinMode(A0,INPUT_PULLUP);
  //pinMode(A1,INPUT_PULLUP);
  //pinMode(A2,INPUT_PULLUP);
  //pinMode(A3,INPUT_PULLUP);
  //pinMode(A4,INPUT_PULLUP);
  //pinMode(A5,INPUT_PULLUP);
  //pinMode(A6,INPUT_PULLUP);
  //pinMode(A7,INPUT_PULLUP);
 
  lcd.begin(16, 2);
  lcd.clear();
  
  //analogWrite(24,30);///?
  
  lcd.setCursor(0, 0);
     
  lcd.createChar(0, rusLetterSH);      // СЃРѕР·РґР°РµРј СЃРёРјРІРѕР» Рё Р·Р°РїРёСЃС‹РІР°РµРј РµРіРѕ РІ РїР°РјСЏС‚СЊ LCD
  lcd.createChar(1, rusLetterI);
  lcd.createChar(2, rusLetterB);
  lcd.createChar(3, rusLetterL);
  lcd.createChar(4, rusLetterP);
  lcd.createChar(5, rusLetterYA);
  lcd.createChar(6, rusLetterGH);
  lcd.createChar(7, rusLetterF);
  
  lcd.clear();
  for(int i=0;i<10;i++){
    lcd.setCursor(i, 2);
    lcd.print('-');
    delay(100);
  }
  wdt_enable(WDTO_8S);
  timer_init_ISR_1KHz(_TIMER1);

}

byte heartbeat=0;
byte key=0;
byte menutime=0;
byte sig_meg_count=0;
byte sig_meg_HZ=0;

void loop() {

//================================= sig process =======================================
if(avgidx==SIG_LEN){
    
    HZin=round(Havg/(float)(SIG_LEN));
    Vin=round(Vavg/(float)(SIG_LEN));
    
    //HZin=1000000.00/(float)(end_time-start_time);
    //Vin=Ak*v_max*v_max+Bk*v_max+Ck;
    
    Serial.print("Hz=");Serial.println(HZin);
    Serial.print("V=");Serial.println(Vin);
    
    avgidx=0;
    Havg=0;
    Vavg=0;
}

//================================= end sig process ===================================
  
  lcd.clear();
  wdt_reset();
  key=getKey();
  if(key==5 && menuactive==false) {
    menuactive=true;
    menutime=0;
  }else if(key==5 && menuactive==true) {
    menuactive=false;
    if (isacchanged(actconfidx)) {
      EEPROM.put(0,actconfidx);
      if(isnewconfig(actconfidx)){
        EEPROMObj.DIZEL_STARTER_TIME=DIZEL_STARTER_TIME;
        EEPROMObj.START_NUM=START_NUM;
        EEPROMObj.OSTANOV_GENERATORA1_CNT=OSTANOV_GENERATORA1_CNT;
        EEPROMObj.OSTANOV_GENERATORA2_CNT=OSTANOV_GENERATORA2_CNT;
        EEPROMObj.OSNOVNAYA_GENERATOR_SWH_DELAY=OSNOVNAYA_GENERATOR_SWH_DELAY;
        EEPROM.put(sizeof(actconfidx)+sizeof(EEPROMObj)*actconfidx,EEPROMObj);
      }else {
          EEPROM.get(sizeof(actconfidx)+sizeof(EEPROMObj)*actconfidx,EEPROMObj);
          DIZEL_STARTER_TIME=EEPROMObj.DIZEL_STARTER_TIME;
          START_NUM=EEPROMObj.START_NUM;
          OSTANOV_GENERATORA1_CNT=EEPROMObj.OSTANOV_GENERATORA1_CNT;
          OSTANOV_GENERATORA2_CNT=EEPROMObj.OSTANOV_GENERATORA2_CNT;
          OSNOVNAYA_GENERATOR_SWH_DELAY=EEPROMObj.OSNOVNAYA_GENERATOR_SWH_DELAY;  
      }      
    }else {
      EEPROMObj.DIZEL_STARTER_TIME=DIZEL_STARTER_TIME;
      EEPROMObj.START_NUM=START_NUM;
      EEPROMObj.OSTANOV_GENERATORA1_CNT=OSTANOV_GENERATORA1_CNT;
      EEPROMObj.OSTANOV_GENERATORA2_CNT=OSTANOV_GENERATORA2_CNT;
      EEPROMObj.OSNOVNAYA_GENERATOR_SWH_DELAY=OSNOVNAYA_GENERATOR_SWH_DELAY;
      EEPROM.put(sizeof(actconfidx)+sizeof(EEPROMObj)*actconfidx,EEPROMObj);
    }
  }
  
  
  if(menuactive) {
    menuprocess(key);
    menuprint();
    if(key==0)menutime++; else menutime=0;    
    if(menutime>30){
      menuactive=false;
      EEPROM.get(0,actconfidx);
      EEPROM.get(sizeof(actconfidx)+sizeof(EEPROMObj)*actconfidx,EEPROMObj);
      DIZEL_STARTER_TIME=EEPROMObj.DIZEL_STARTER_TIME;
      START_NUM=EEPROMObj.START_NUM;
      OSTANOV_GENERATORA1_CNT=EEPROMObj.OSTANOV_GENERATORA1_CNT;
      OSTANOV_GENERATORA2_CNT=EEPROMObj.OSTANOV_GENERATORA2_CNT;
      OSNOVNAYA_GENERATOR_SWH_DELAY=EEPROMObj.OSNOVNAYA_GENERATOR_SWH_DELAY;
    }
    delay (500);
    return;
  }
  
  lcd.setCursor(0, 0);
  //lcd.print("KL1=");lcd.print(digitalRead(OSNOVNAYA)); 
  if(Vin<150){

    lcd.print("---B --Hz ");
    //lcd.print("H");lcd.write(byte(1));lcd.print("3K.HA");lcd.write(byte(4));lcd.print("P.");
  }
  else {
    lcd.print(Vin);lcd.print("B ");lcd.print(HZin);lcd.print("Hz ");
  }
  //lcd.setCursor(5, 0);
  //lcd.print("  KL2=");lcd.print(digitalRead(GENERATOR)); 
  lcd.setCursor(10, 0);
  if(digitalRead(OSNOVNAYA)==1)lcd.print("+");else lcd.print("-"); 
  if(digitalRead(GENERATOR)==1)lcd.print("+");else lcd.print("-");
  
  lcd.setCursor(13, 0);
  switch(heartbeat){
    case 0:lcd.print(" ");heartbeat++;break;
    case 1:lcd.print("*");heartbeat=0;break;
  }
    
  ProcessFunc();
  printCurState();
  
  delay(1000);
   
}

void timer_handle_interrupts(int timer) {
#ifdef GSMMODULE
    gprsupdcnt++;
    if(gprsupdcnt>50) {
      gprsupddata(curr_state,0,0,gprsupdcnt-50);
    }
    if(gprsupdcnt>60)gprsupdcnt=0;
#endif    

    a1=a2;
    a2=a3;
    a3=analogRead(1);
    
    if(sig_arr_flag==0){
      if(a1<a2 && a2>=a3) {
           start_time=micros();
           sig_arr_flag++;
           v_max=a2;
      }
    }else if(sig_arr_flag==1) {
      if(a1<a2 && a2>=a3) {
           end_time=micros();
           sig_arr_flag++;
           v_max=a2;
      }
    }
    
    if(sig_arr_flag==2) {
      if (avgidx<SIG_LEN){
          Havg=Havg+1000000.00/(float)(end_time-start_time);
          Vavg=Vavg+(Ak*v_max*v_max+Bk*v_max+Ck);
          avgidx++;
      }
      sig_arr_flag=0;
    }


    
}
