#include "gsmfnc.h"
#include <Arduino.h>
#include <string.h>



String readBuff="";

void ClearSerial() {
  while(gsm.available())gsm.read();
}


void wait(const unsigned int w) {
  unsigned int i=0;
  while(!gsm.available() && i<w){
    delay(10);
    i++;
  }
}

void updateOper() {
  byte i=0;
  byte k=0;
  gsm.println(F("AT+COPS?"));
  gsm.flush();
  readBuff=gsm.readString();
  if(readBuff.indexOf("+COPS:")>=0 && readBuff.indexOf("OK")>=0)
  {
    lcd.setCursor(0, 0);
    i=readBuff.indexOf(",\"");
    k=readBuff.indexOf("OK",i+1);
    //lcd.print(readBuff.substring(i+2,readBuff.length()-9));
    lcd.print(readBuff.substring(i+2,k-5));
    ErrorMsg(0);
  } else ErrorMsg(1);
  readBuff="";
  ClearSerial();
}

void updateSignal() {
  byte i=0,j=0;
  readBuff="";
  gsm.println(F("AT+CSQ"));
  gsm.flush();
  readBuff=gsm.readString();
  if(readBuff.indexOf("+CSQ:")>=0 && readBuff.indexOf("OK"))
  {
    i=readBuff.indexOf("+CSQ:");
    j=readBuff.indexOf(",",i+1);
    lcd.setCursor(14, 0);
    lcd.print(readBuff.substring(i+6,j));
    ErrorMsg(0);
  }else ErrorMsg(1);
  readBuff="";
  ClearSerial();
}

void printError(String *s) {
  
  if(s->substring(0,4)==String("+CME"))
  {
    lcd.setCursor(0, 1);
    lcd.print(s->substring(0));
  }
}

void sendTextMessage(String phnum,String msg) {
          
    // Устанавливает текстовый режим для SMS-сообщений
    gsm.println(F("AT+CMGF=1"));
    gsm.flush();
    delay(300);
    gsm.println("AT+CMGS=\""+phnum+"\"");
    gsm.flush();
    delay(300);
    //delay(100);
    // Пишем текст сообщения
    gsm.println(msg);
    gsm.flush();
    delay(300);
    // Отправляем Ctrl+Z, обозначая, что сообщение готово
    gsm.print((char)26);
    ClearSerial();
//    send("\26");
}

void SendAlertSMS(String phnum){
      // Устанавливает текстовый режим для SMS-сообщений
    gsm.println(F("AT+CMGF=1"));
    gsm.flush();
    delay(300);
    gsm.println("AT+CMGS=\""+phnum+"\"");
    gsm.flush();
    delay(300);
    
    // Пишем текст сообщения
  
    gsm.print(F("Propala osnovnaya set'\r\n"));
    gsm.print(F("Sistema v prinuditelnom regime\r\n"));
    gsm.print(F("Perekhod na generator NE VIPOLNEN\r\n"));
    gsm.print(F("Otpravte 1 dlya zapuska\r\n"));
    // Отправляем Ctrl+Z, обозначая, что сообщение готово
    gsm.print((char)26);
    gsm.flush();
    delay(300);
    ClearSerial();
}

void GetHelp(String phnum){
      // Устанавливает текстовый режим для SMS-сообщений
    gsm.println(F("AT+CMGF=1"));
    gsm.flush();
    delay(300);
    gsm.println("AT+CMGS=\""+phnum+"\"");
    gsm.flush();
    delay(300);
    
    // Пишем текст сообщения
  
    gsm.print(F("? - Zapros tekushego sostoyaniya\r\n"));
    gsm.print(F("START - Prinuditelniy zapusk generatora\r\n"));
    gsm.print(F("STOP - Prinuditelnaya rabota ot osnovnoy seti\r\n"));
    gsm.print((char)26);
    gsm.flush();
    delay(300);
    ClearSerial();
    gsm.println("AT+CMGS=\""+phnum+"\"");
    gsm.flush();
    delay(300);
    gsm.print(F("NOPOWER - Prinuditelnoe otkluchenie elektrichestva\r\n"));
    gsm.print(F("AUTO - Snyatie prinuditelnogo regima, perekhod na avtomat\r\n"));
    // Отправляем Ctrl+Z, обозначая, что сообщение готово
    gsm.print((char)26);
    gsm.flush();
    delay(300);
    ClearSerial();
}

int balans(String *ans) {
  short int i=0,j=0;
  ClearSerial();
  gsm.println(F("AT"));
  gsm.flush();
  wait(300);
  readBuff=gsm.readString();
  ClearSerial();
  gsm.println(F("ATD*100#"));
  gsm.flush();
  delay(5000);
  wait(300);
  readBuff=gsm.readString();
  i=readBuff.indexOf("Balans:");
  j=readBuff.indexOf("r",i+1);
  ans[0]=readBuff.substring(i+7,j);
  readBuff="";
  if(i<0)return -1;
  return 1;
}

char getTextMessage(String *phone,String *text){  
  char res=0;
  unsigned short i,j;
  readBuff="";
  gsm.println(F("AT+CMGR=1"));
  gsm.flush();
  wait(300);
  readBuff=gsm.readString();
  readBuff.trim();   
  readBuff=readBuff.substring(12,readBuff.length());
  if(readBuff.indexOf("ERROR")>=0)return -1;
  if(readBuff.indexOf("+CMGR:")>=0) {
    
    i=readBuff.indexOf(",\"");
    j=readBuff.indexOf("\",",i);
    phone[0]=readBuff.substring(i+2,j);
   
    i=readBuff.indexOf("\r\n");
    //i=readBuff.indexOf("\r\n",i+2);
    j=readBuff.indexOf("\r\n",i+2);
    text[0]=readBuff.substring(i+2,j);
    readBuff="";
    ClearSerial();   
   
    gsm.println(F("AT+CMGD=1,4"));  
    gsm.flush();
    delay(300);
    ClearSerial();
    res=1;
  }/*else {
    lcd.setCursor(0, 0);
    lcd.print("                ");
    lcd.setCursor(0, 0);
    lcd.print(s);
    delay(3000);
    
     
  }*/
 
  return res;
}

void ErrorMsg(char b) {
  lcd.setCursor(13, 0);
  if(b)lcd.print("!");
    else lcd.print(" ");
}


char getMasterNum(String * ans) {
  String NoMasterErrorMsg="No master";
  char i=0,k=0,j;
  ans[0]=NoMasterErrorMsg;
  
  gsm.println(F("AT+CPBR=1"));
  gsm.flush();
  delay(200);
  readBuff=gsm.readString();
    //lcd.setCursor(0,0);
    //lcd.print(readBuff.substring(10,readBuff.length()));
    //delay(10000);
  ClearSerial();
  if(readBuff.indexOf("ERROR")>=0)return -1;
  if(readBuff.indexOf("MASTER")>=0) {
    k=readBuff.indexOf(",\"");
    j=readBuff.indexOf("\",",k);
    ans[0]=readBuff.substring(k+2,j);
    return 0;
  }    
  return -1;  
}
