// include the library code:
#include <LiquidCrystal.h>
#include "puzgfnc.h"
#include "gsmfnc.h"

#define BALANS_UPDATE_INTERVAL 3600 //sec
String COMMAND_PHONE ="";

LiquidCrystal lcd(3, 2, 28, 27, 26, 25);

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
int balance_cnt=0;
String blns_str=""; 
byte start_err_notification=0;
byte signal_oper_update_cnt=0;
byte gsm_flag=1;
short restart_sim_cnt=0;
byte f_force_osnovnaya_fail=0;

void GetCurrInfo(String *text){
    const String s1="PRINUDITELNO ";
    const String s2=" GENERATORA";
    const String s3="NORMA";
    const String s4="\r\n";
    
    byte f_osnovnaya=0,f_generator=0;
    f_osnovnaya=digitalRead(OSNOVNAYA);
    f_generator=digitalRead(GENERATOR);  
  
    if(balans(text)!=1)text[0]="Oshibka zaprosa balansa\r\n";
    else text[0]+=s4;
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
   
     case SET_OSNOVNAYA:{
         lcd.print(F("HOPMA "));
         if(balance_cnt==0){
           if(balans(&blns_str)==1)
              //lcd.print(blns_str.substring(blns_str.indexOf(":")+1,blns_str.indexOf("r")));
              lcd.print(blns_str);
           else {
              blns_str="--";
              balance_cnt=-50;
           }
           //lcd.print("PY");lcd.write(byte(2));lcd.print("     ");
         }//else if(balance_cnt<0)lcd.print(balance_cnt);
         else lcd.print(blns_str);
         lcd.print(F("PY"));lcd.write(byte(2));lcd.print(F("     "));
         balance_cnt++;
         if (balance_cnt>BALANS_UPDATE_INTERVAL)balance_cnt=0;  
         break;
     }
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
  analogWrite(24,30);
  lcd.setCursor(0, 0);
  lcd.print(F("Starting GSM"));
  ///lcd.print("Start ...");
  Serial.begin(57600);
  //Serial.begin(19200);
  //Serial.begin(9600);
  for (k=0;k<10;k++){
    Serial.println(F("AT"));
    Serial.flush();
    delay(1000);
    lcd.setCursor(k, 1);
    lcd.print(F("*"));
 }
  
  //send("ATD*111*6*2#");
  Serial.println(F("AT+CMGF=1"));
  Serial.flush();
  delay(300);
  
  Serial.println(F("AT+CSCS=\"GSM\""));
  Serial.flush();
  delay(300);
  
  Serial.println(F("AT+CPMS=\"SM\""));
  Serial.flush();
  delay(300);
  
  Serial.println(F("AT+CNMI=1,1,0,0,0"));
  Serial.flush();
  delay(300);
  
  Serial.println(F("AT+CMGD=1,4"));  
  Serial.flush();
  delay(300);
  
  lcd.createChar(0, rusLetterSH);      // создаем символ и записываем его в память LCD
  lcd.createChar(1, rusLetterI);
  lcd.createChar(2, rusLetterB);
  lcd.createChar(3, rusLetterL);
  lcd.createChar(4, rusLetterP);
  lcd.createChar(5, rusLetterYA);
  lcd.createChar(6, rusLetterGH);
  
  pinMode(OSNOVNAYA,INPUT);
  pinMode(GENERATOR,INPUT);
  
  pinMode(RESTART_SIM,OUTPUT);
  
  pinMode(OSNOVNAYA_SWCH,OUTPUT);
  pinMode(GENERATOR_ON,OUTPUT);
  pinMode(GENERATOR_OFF,OUTPUT);
  pinMode(GENERATOR_SWCH,OUTPUT);
  pinMode(20,OUTPUT);
  pinMode(21,OUTPUT);
  pinMode(22,OUTPUT);
  //sendTextMessage("+79166076723","Test");
  lcd.clear();
  ClearSerial();
  lcd.setCursor(0,0);
  if(getMasterNum(&COMMAND_PHONE)<0) {
    lcd.print(COMMAND_PHONE);
    gsm_flag=0;
  }else {
    COMMAND_PHONE.replace("+7","8");
    lcd.print(F("Master num:"));
    lcd.setCursor(0,1);
    lcd.print(COMMAND_PHONE);
    lcd.print(F(";"));
  }
  delay(5000);
  lcd.clear();
  ClearSerial();  
}

uint8_t c_step=0;
byte strInUse=0;
byte heartbeat=0;
void SendCommandComplite() {
    sendTextMessage(COMMAND_PHONE,"Kommanda vipolnena");
}

void loop() {

  byte val=0;
  String s="";
  String OtvetSMS="";
  String phone="",text="";
  unsigned int i=0;
  
  //lcd.clear();
/*  
  */
  //lcd.setCursor(0, 0);
  lcd.setCursor(13, 0);
  switch(heartbeat){
    case 0:lcd.print(" ");heartbeat++;break;
    case 1:lcd.print("*");heartbeat=0;break;
  }
  if(gsm_flag>0){//start gsm_flag processing
  restart_sim_cnt++;
  if (restart_sim_cnt>=65534){
    digitalWrite(RESTART_SIM,HIGH);
    delay(1000);
    digitalWrite(RESTART_SIM,LOW);
    restart_sim_cnt=0;
  }
  
  if(curr_state==SET_OSNOVNAYA || curr_state==SET_GENERATORA || curr_state==FORCE_GENERATOR ||  curr_state==FORCE_OSNOVNAYA || curr_state==FORCE_NOPOWER || curr_state==OSHIBKA_ZAPUSKA) {
    if(strInUse==0) {
      if(signal_oper_update_cnt==0){
        updateSignal();
        updateOper();
      }
      signal_oper_update_cnt++;
      //lcd.setCursor(10,0);
      //lcd.print("  ");
      //lcd.setCursor(10,0);
      //lcd.print(signal_oper_update_cnt);
      if(signal_oper_update_cnt==60)signal_oper_update_cnt=0;    
    }else {
      strInUse--;
      if(strInUse==0) {
        lcd.setCursor(0, 0);
        lcd.print(F("                "));
        signal_oper_update_cnt=0;
      }
    }
    ClearSerial();
  }
  
  if(curr_state==OSHIBKA_ZAPUSKA && start_err_notification==0) {
    start_err_notification++;
    GetCurrInfo(&OtvetSMS);
    sendTextMessage(COMMAND_PHONE,OtvetSMS);
  }
  
  if(curr_state==FORCE_OSNOVNAYA && digitalRead(OSNOVNAYA)==0) {
    if(f_force_osnovnaya_fail==0) {
      SendAlertSMS(COMMAND_PHONE);
      f_force_osnovnaya_fail++;
    }
  }else{
    f_force_osnovnaya_fail=0;
  } 
  
  
  phone=" ";
  text=" ";
  if(curr_state==SET_OSNOVNAYA || curr_state==SET_GENERATORA || curr_state==FORCE_GENERATOR ||  curr_state==FORCE_OSNOVNAYA || curr_state==FORCE_NOPOWER || curr_state==OSHIBKA_ZAPUSKA) {
      val=getTextMessage(&phone,&text);  
      if(val==-1){
          strInUse=20;
          lcd.setCursor(0, 0);
          lcd.print(F("                "));
          lcd.setCursor(0, 0);
          lcd.print(F("SMS HE "));lcd.write(4);lcd.print("P");lcd.write(1);lcd.print("H");lcd.write(5);lcd.print(F("TO"));
      }else if(val==1) {
           strInUse=20;
           lcd.setCursor(0, 0);
           lcd.print(F("                "));
           lcd.setCursor(0, 0);
           lcd.print(">");
           
          phone.replace("+7","8");
          if(phone==COMMAND_PHONE){
             text.toUpperCase();
             lcd.print(text);
             if(text==String("RUN") || text==String("START") || text==String("1") ){
                curr_state=FORCE_GENERATOR_START;
                SendCommandComplite();
             }else if(text==String("STOP") || text==String("2") ){
                curr_state=FORCE_OSNOVNAYA_START;
                SendCommandComplite();
             }else if(text==String("OFF") ||text==String("NOPOWER")|| text==String("3") ){
                curr_state=FORCE_NOPOWER_START;
                SendCommandComplite();
             }if(text==String("AUTO") || text==String("4") ){
                curr_state=SET_OSNOVNAYA;
                SendCommandComplite();
             }else if(text==String("?") ||text==String("INFO")){
                 GetCurrInfo(&OtvetSMS);
                 sendTextMessage(COMMAND_PHONE,OtvetSMS);
             }else if(text==String("HELP")){
                 GetHelp(COMMAND_PHONE);
             }else{
                 sendTextMessage(COMMAND_PHONE,"Nevernaya kommanda");
             } 
          }else {
            strInUse=20;
            lcd.setCursor(0, 0);
            lcd.print(F("                "));
            lcd.setCursor(0, 0);
            lcd.print(F("HOMEP "));lcd.print(F("HE"));lcd.write(4);lcd.print(F("P"));lcd.print(F("AB."));
            delay(3000);
            lcd.setCursor(0, 0);
            lcd.print(F("                "));
            lcd.setCursor(0, 0);
            lcd.print(phone);
          }
      }
  }
  ClearSerial();
  }//end gsm_flag processing
  
  ProcessFunc();
  printCurState();
  ClearSerial();  
  delay(1000);
  
   
}





