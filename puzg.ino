// include the library code:
#include <LiquidCrystal.h>
#include "puzgfnc.h"

//LiquidCrystal lcd(3, 2, 28, 27, 26, 25);
LiquidCrystal lcd(13, 12, 8, 9, 10, 11);

byte rusLetterB[8]= {0x1F,0x11,0x10,0x1E,0x11,0x11,0x1E,0};//Б
//byte rusLetterG[8]= {0x1F,0x11,0x10,0x10,0x10,0x10,0x10,0};//Г
//byte rusLetterD[8]= {0x0F,0x05,0x05,0x05,0x09,0x11,0x1F,0x11};//Д
byte rusLetterGH[8]= {0x15,0x15,0x15,0x0E,0x15,0x15,0x15,0};//Ж
byte rusLetterI[8]= {0x11,0x11,0x13,0x15,0x19,0x11,0x11,0};//И
//byte rusLetterIY[8]= {0x0A,0x15,0x13,0x15,0x19,0x11,0x11,0};//Й
byte rusLetterL[8]= {0x0F,0x05,0x05,0x05,0x05,0x15,0x09,0}; //Л
byte rusLetterP[8]= {0x1F,0x11,0x11,0x11,0x11,0x11,0x11,0};//П
//byte rusLetterF[8]= {0x04,0x0E,0x15,0x15,0x15,0x0E,0x04,0};//Ф
//byte rusLetterC[8]= {0x11,0x11,0x11,0x11,0x11,0x11,0x1F,1};//Ц
//byte rusLetterCH[8]= {0x11,0x11,0x11,0x0F,0x01,0x01,0x01,0};//Ч
byte rusLetterSH[8]= {0x15,0x15,0x15,0x15,0x15,0x15,0x1F,0};//Ш
//byte rusLetterSCH[8] = {0x15,0x15,0x15,0x15,0x15,0x15,0x1F,1};//Щ
//byte rusLetterTZN[8] = {0x18,0x08,0x08,0x0E,0x09,0x09,0x0E,0};//Ъ
//byte rusLetterEI[8]= {0x11,0x11,0x11,0x19,0x15,0x15,0x19,0};//Ы
//byte rusLetterEE[8]= {0x0E,0x11,0x01,0x07,0x01,0x11,0x0E,0};//Э
//byte rusLetterYU[8]= {0x12,0x15,0x15,0x1D,0x15,0x15,0x12,0};//Ю
byte rusLetterYA[8]= {0x0F,0x11,0x11,0x0F,0x05,0x09,0x11,0};//Я

byte start_err_notification=0;
byte f_force_osnovnaya_fail=0;

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
     case SET_OSNOVNAYA:lcd.print(F("HOPMA "));break;     
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
  //String num="";
  byte i=10,k=0;
  // set up the LCD's number of columns and rows:
  lcd.begin(16, 2);
  lcd.clear();
  
  analogWrite(24,30);///?
  
  lcd.setCursor(0, 0);
     
  lcd.createChar(0, rusLetterSH);      // создаем символ и записываем его в память LCD
  lcd.createChar(1, rusLetterI);
  lcd.createChar(2, rusLetterB);
  lcd.createChar(3, rusLetterL);
  lcd.createChar(4, rusLetterP);
  lcd.createChar(5, rusLetterYA);
  lcd.createChar(6, rusLetterGH);
  
  pinMode(OSNOVNAYA,INPUT);
  pinMode(GENERATOR,INPUT);
  
   
  pinMode(OSNOVNAYA_SWCH,OUTPUT);
  pinMode(GENERATOR_ON,OUTPUT);
  pinMode(GENERATOR_OFF,OUTPUT);
  pinMode(GENERATOR_SWCH,OUTPUT);
  
  pinMode(20,OUTPUT);
  pinMode(21,OUTPUT);
  pinMode(22,OUTPUT); 
  
  lcd.clear();
}

uint8_t c_step=0;
byte strInUse=0;
byte heartbeat=0;

void loop() {

  byte val=0;
  String s="";
  unsigned int i=0;
  
  lcd.setCursor(13, 0);
  switch(heartbeat){
    case 0:lcd.print(" ");heartbeat++;break;
    case 1:lcd.print("*");heartbeat=0;break;
  }
    
  ProcessFunc();
  printCurState();
  delay(1000);
  
   
}





