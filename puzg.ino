// include the library code:
#include <LiquidCrystal.h>
#include "puzgfnc.h"
#include "gsmfnc.h"

#define BALANS_UPDATE_INTERVAL 3600 //sec
#define COMMAND_PHONE "+79166076723"

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

/*void GetHelp(String *text){
  text[0]="";
  text[0]+="? - Zapros tekushego sostoyaniya\r\n";
  text[0]+="START - Prinuditelniy zapusk generatora\r\n";
  text[0]+="STOP - Prinuditelnaya rabota ot osnovnoy seti\r\n";
  text[0]+="NOPOWER - Prinuditelnoe otkluchenie elektrichestva\r\n";
  text[0]+="AUTO - Snyatie prinuditelnogo regima, perekhod na avtonat\r\n";
}*/

void GetCurrInfo(String *text){
    const String s1="PRINUDITELNO ";
    const String s2=" GENERATORA";
    
    byte f_osnovnaya=0,f_generator=0;
    f_osnovnaya=digitalRead(OSNOVNAYA);
    f_generator=digitalRead(GENERATOR);  
  
    if(balans(text)!=1)text[0]="Oshibka zaprosa balansa\r\n";
    else text[0]+="\r\n";
    text[0]+="Osnovnaya set':";
    if(f_osnovnaya==1)text[0]+="NORMA";
    else text[0]+="OTKLUCHENA";
    text[0]+="\r\n";
    
    text[0]+="Generator:";
    if(f_generator==1)text[0]+="NORMA";
    else text[0]+="OTKLUCHEN";
    text[0]+="\r\n";
    text[0]+="Regim raboty:";
    switch(curr_state){
      case 0:text[0]+="NORMA";break;
      case 1:text[0]+=s2.substring(0,s2.length()-1);break;
      case 2:text[0]+=("ZAPUSK"+s2);break;
      case 3 :text[0]+=("OSHIBKA ZAPUSKA")+s2;break;
      case 4 :text[0]+=("OTKLUCHENIE")+s2;break;
      case 5:text[0]+="PROPALA OSNOVNAYA SET'";break;
      case 6:text[0]+=(s1+"GENERATOR");break;
      case 7:text[0]+=(s1+"OSNOVNAYA SET'");break;
      case 8:text[0]+=(s1+"OTKLUCHENIE ENERGII");break;
      default:text[0]+="NEIZVESTNIY REGIM";break;  
    }  
    text[0]+="\r\n";
    
}

void printCurState() {
   lcd.setCursor(0, 1);
   switch(curr_state) {
   
     case SET_OSNOVNAYA:{
         lcd.print("HOPMA ");
         if(balance_cnt==0){
           if(balans(&blns_str)==1)
              //lcd.print(blns_str.substring(blns_str.indexOf(":")+1,blns_str.indexOf("r")));
              lcd.print(blns_str);
           else {
              lcd.print(blns_str);
              balance_cnt=-100;
           }
           lcd.print("PY");lcd.write(byte(2));lcd.print("     ");
         }else if(balance_cnt<0)lcd.print(balance_cnt);
         else lcd.print(blns_str);
         balance_cnt++;
         if (balance_cnt>BALANS_UPDATE_INTERVAL)balance_cnt=0;  
         break;
     }
     case SET_GENERATORA:        lcd.print("PE3EPB          ");break;
     case ZAPUSK_GENERATORA:     lcd.print("CTAPT... #");lcd.print(starts_try_num);lcd.print("      ");break;
     case OSHIBKA_ZAPUSKA:       lcd.print("O");lcd.write(byte(0));lcd.write(byte(1));lcd.write(byte(2));lcd.print("KA CTAPTA   ");break;
     case OSTANOV_GENERATORA:    lcd.print("OCTAHOBKA...    ");break;
     case PROPALA_OSNOVNAYA_SET: lcd.print("HET HA");lcd.write(byte(4));lcd.print("P");lcd.write(byte(5));lcd.write(byte(6));lcd.print("EH");lcd.write(byte(1));lcd.write(byte(5));lcd.print("   ");break;
   } 
}

void setup() {
  byte i=10;
  // set up the LCD's number of columns and rows:
  lcd.begin(16, 2);
  lcd.clear();
  analogWrite(24,30);
  lcd.setCursor(0, 0);
  lcd.print("Start ...");
  Serial.begin(57600);
  //Serial.begin(19200);
  //Serial.begin(9600);
  
  Serial.println("AT");
  Serial.flush();
  delay(300);
  
  //send("ATD*111*6*2#");
  Serial.println("AT+CMGF=1");
  Serial.flush();
  delay(300);
  
  Serial.println("AT+CSCS=\"GSM\"");
  Serial.flush();
  delay(300);
  
  Serial.println("AT+CPMS=\"SM\"");
  Serial.flush();
  delay(300);
  
  Serial.println("AT+CNMI=1,1,0,0,0");
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
  
}

uint8_t c_step=0;
byte strInUse=0;

void SendCommandComplite() {
    sendTextMessage(String(COMMAND_PHONE),"Kommanda vipolnena");
}

void loop() {

  byte val=0;
  String s="";
  String OtvetSMS="";
  String res="";
  String phone="",text="";
  unsigned int i=0;
  
  //lcd.clear();
/*  
  */
  //lcd.setCursor(0, 0);
  if(curr_state==SET_OSNOVNAYA || curr_state==SET_GENERATORA || curr_state==FORCE_GENERATOR ||  curr_state==FORCE_OSNOVNAYA || curr_state==FORCE_NOPOWER || curr_state==OSHIBKA_ZAPUSKA) {
    if(strInUse==0) {
      if(signal_oper_update_cnt==0) updateOper();
          else if(signal_oper_update_cnt==1) updateSignal();
      signal_oper_update_cnt++;
      if(signal_oper_update_cnt==30)signal_oper_update_cnt=0;    
    }else {
      strInUse--;
      if(strInUse==0) {
        lcd.setCursor(0, 0);
        lcd.print("                ");
        signal_oper_update_cnt=0;
      }
    }
    ClearSerial();
  }
  
  if(curr_state==OSHIBKA_ZAPUSKA && start_err_notification==0) {
    start_err_notification++;
    GetCurrInfo(&OtvetSMS);
    sendTextMessage(String(COMMAND_PHONE),OtvetSMS);
  }
  phone=" ";
  text=" ";
  if(curr_state==SET_OSNOVNAYA || curr_state==SET_GENERATORA || curr_state==FORCE_GENERATOR ||  curr_state==FORCE_OSNOVNAYA || curr_state==FORCE_NOPOWER || curr_state==OSHIBKA_ZAPUSKA) {
    val=getTextMessage(&phone,&text);  
      strInUse=20;
      if(val==-1){
          lcd.setCursor(0, 0);
          lcd.print("                ");
          lcd.setCursor(0, 0);
          lcd.print("SMS HE ");lcd.write(4);lcd.print("P");lcd.write(1);lcd.print("H");lcd.write(5);lcd.print("TO");
      }else if(val==1) {
           lcd.setCursor(0, 0);
           lcd.print("                ");
           lcd.setCursor(0, 0);
           lcd.print(">");
           
          
          if(phone==String(COMMAND_PHONE)){
             text.toUpperCase();
             lcd.print(text);
             if(text==String("RUN") || text==String("START") || text==String("1") ){
                curr_state=FORCE_GENERATOR;
                SendCommandComplite();
             }else if(text==String("STOP") || text==String("2") ){
                curr_state=FORCE_OSNOVNAYA;
                SendCommandComplite();
             }else if(text==String("OFF") ||text==String("NOPOWER")|| text==String("3") ){
                curr_state=FORCE_NOPOWER;
                SendCommandComplite();
             }if(text==String("AUTO") || text==String("4") ){
                curr_state=SET_OSNOVNAYA;
                SendCommandComplite();
             }else if(text==String("?") ||text==String("INFO")){
                 GetCurrInfo(&OtvetSMS);
                 sendTextMessage(String(COMMAND_PHONE),OtvetSMS);
             }else if(text==String("HELP")){
                 //GetHelp(&OtvetSMS);
                 SendCommandComplite();
                 //sendTextMessage(String(COMMAND_PHONE),OtvetSMS);
             }else{
                 sendTextMessage(String(COMMAND_PHONE),"Nevernaya kommanda");
             } 
          }else {
            lcd.setCursor(0, 0);
            lcd.print("                ");
            lcd.setCursor(0, 0);
            lcd.print("HOMEP ");lcd.print("HE");lcd.write(4);lcd.print("P");lcd.print("AB.");
            delay(3000);
            lcd.setCursor(0, 0);
            lcd.print("                ");
            lcd.setCursor(0, 0);
            lcd.print(phone);
          }
      }
  }
  ClearSerial();
  
  ProcessFunc();
  printCurState();
  ClearSerial();
//  val=digitalRead(31);
//  for(i=16;i<=22;i++){
//      digitalWrite(i,val);
//      if(digitalRead(i))lcd.print("1");
//        else lcd.print("0");
//  }
  
  delay(1000);
  
   
}





