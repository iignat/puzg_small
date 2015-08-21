#include "gsmfnc.h"
#include <Arduino.h>
#include <string.h>

String readBuff="";

void ClearSerial() {
  while(Serial.available())Serial.read();
}


void wait(const unsigned int w) {
  unsigned int i=0;
  while(!Serial.available() && i<w){
    delay(10);
    i++;
  }
}

void updateOper() {
  byte i=0;
  byte k=0;
  Serial.println("AT+COPS?");
  Serial.flush();
  readBuff=Serial.readString();
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
  Serial.println("AT+CSQ");
  Serial.flush();
  readBuff=Serial.readString();
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
    Serial.println("AT+CMGF=1");
    Serial.flush();
    delay(300);
    Serial.println("AT+CMGS=\""+phnum+"\"");
    Serial.flush();
    delay(300);
    //delay(100);
    // Пишем текст сообщения
    Serial.println(msg);
    Serial.flush();
    delay(300);
    // Отправляем Ctrl+Z, обозначая, что сообщение готово
    Serial.print((char)26);
    ClearSerial();
//    send("\26");
}

int balans(String *ans) {
  int i=0,j=0;
  ClearSerial();
  Serial.println("AT");
  Serial.flush();
  wait(300);
  readBuff=Serial.readString();
  ClearSerial();
  Serial.println("ATD*100#");
  Serial.flush();
  delay(5000);
  wait(300);
  readBuff=Serial.readString();
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
  Serial.print("AT+CMGR=1\r");
  Serial.flush();
  wait(300);
  readBuff=Serial.readString();
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
   
    Serial.println("AT+CMGD=1,4");  
    Serial.flush();
    delay(200);
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
