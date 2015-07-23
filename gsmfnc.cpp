#include "gsmfnc.h"
#include <Arduino.h>
#include <string.h>

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

String getLine(uint8_t w) {
   int8_t n=w;
   uint8_t nb=0;
   int i=0,k=0;
   char currSymb=0,currSymb_last=0;
   String currStr="";
   
   
    while(n>0) {
      n--;
      while(Serial.available()) {
        //delay(3);
        currSymb_last=currSymb;
        currSymb=Serial.read();
        if( (currSymb=='\r' && currSymb_last=='\n') ||
            (currSymb=='\n' && currSymb_last=='\r') || 
            (currSymb==10 && currSymb_last==13) ||
            (currSymb==13 && currSymb_last==10)
        ){
          return currStr;
        }
     
        if(currSymb=='\n')continue;
        if(currSymb==13)continue;
        if(currSymb==10)continue;
        if(currSymb=='\r')continue;
        currStr+=String(currSymb);
        
      }
      //delay(100);
    }
    return currStr;
}


String send(const String msg,String *ans, char echo) {
  String s="";
  Serial.println(msg);  
  Serial.flush();
  getLine(10);
  getLine(10);
  if(ans)ans[0]=getLine(10);
    else getLine(10);
  getLine(10);
  s=getLine(10);
  ClearSerial();
  return s;
}

void updateOper(String *s) {
  
  if(s->substring(0,6)==String("+COPS:"))
  {
    lcd.setCursor(0, 0);
    lcd.print(s->substring(12,s->length()-1));
  }
}

void updateSignal(String *s) {
  
  if(s->substring(0,5)==String("+CSQ:"))
  {
    lcd.setCursor(14, 0);
    lcd.print(s->substring(6,8));
  }
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
  String str="";
  int i=0,j=0;
  ClearSerial();
  Serial.println("AT");
  Serial.flush();
  wait(300);
  str=Serial.readString();
  ClearSerial();
  Serial.println("ATD*100#");
  Serial.flush();
  delay(5000);
  wait(300);
  str=Serial.readString();
  i=str.indexOf("Balans:");
  j=str.indexOf("r",i+1);
  ans[0]=str.substring(i+7,j);
  if(i<0)return -1;
  return 1;
}

char getTextMessage(String *phone,String *text){  
  String s="";
  char res=0;
  unsigned short i,j;
  /*s1="AT+CMGL=\"ALL\"\r";
  Serial.print(s1);  
  Serial.flush();
  s=Serial.readString();
  s.replace("\n","");
  s.replace("\r","");
  lcd.setCursor(0, 0);
  lcd.print("                ");
  lcd.setCursor(0, 0);
  lcd.print(s.substring(s.length()-14,s.length()));
  */
  s="";
  Serial.print("AT+CMGR=1\r");
  Serial.flush();
  wait(300);
  s=Serial.readString();
  
  //s.replace("\n","#");
  //s.replace("\r","#");
  s.trim();
  
  /*lcd.setCursor(0, 0);
   lcd.print("                ");
   lcd.setCursor(0, 0);
   lcd.print(">");
   lcd.print(s);
   delay(3000);
  */   
     
  s=s.substring(12,s.length());
  if(s.indexOf("ERROR")>=0)return -1;
  if(s.indexOf("+CMGR:")>=0) {
     
     
    i=s.indexOf(",\"");
    j=s.indexOf("\",",i);
    phone[0]=s.substring(i+2,j);
    
   /*lcd.setCursor(0, 0);
   lcd.print("                ");
   lcd.setCursor(0, 0);
   lcd.print(">");
   lcd.print(phone[0]);
   delay(3000);
    */
    
    i=s.indexOf("\r\n");
    //i=s.indexOf("\r\n",i+2);
    j=s.indexOf("\r\n",i+2);
    text[0]=s.substring(i+2,j);
    s="";
   /*lcd.setCursor(0, 0);
   lcd.print("                ");
   lcd.setCursor(0, 0);
   lcd.print(">");
   lcd.print(text[0]);
   delay(3000);
   */
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
