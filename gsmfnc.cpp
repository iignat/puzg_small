#include "gsmfnc.h"
#include <Arduino.h>
#include <string.h>

String ClearSerial() {
  String currStr="";  
  char a;
  while(Serial.available())
     a=Serial.read();
     currStr+=a;
  return currStr;
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
      delay(100);
      
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
    send("AT+CMGF=1\r");
   
    send("AT + CMGS = \""+phnum+"\"\r");
    //delay(100);
    // Пишем текст сообщения
    send(msg);
    // Отправляем Ctrl+Z, обозначая, что сообщение готово
    Serial.print((char)26);
    ClearSerial();
//    send("\26");
}

char balans(String *ans) {
//  return 0;
  int i=0,j=0,k=0;
  char res=0;
  String s="";
  Serial.println("ATD*100#");  
  Serial.flush();
  while(i<10) {
    delay(1000);
    s=getLine(10);
    
    if(s.substring(0,8)==String("+CUSD: 4")){
      res=-1;
      break;
    }else if(s.substring(0,5)==String("+CUSD")){
      j=s.indexOf(",\"");
      k=s.indexOf("\",",j+1);
      ans[0]=s.substring(j+2,k-1);
      res=1;
      break;
    }
    i++;
  }
  ClearSerial();
  return res;
}

char getTextMessage(String *phone,String *text){  
  String s="",s1="";
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
  
  Serial.print("AT+CMGR=1\r");
  s=Serial.readString();
  //s.replace("\n","#");
  //s.replace("\r","#");
  s.trim();
  s=s.substring(12,s.length());
  if(s.indexOf("ERROR")>=0)return -1;
  if(s.indexOf("+CMGR:")>=0) {
    
    
    i=s.indexOf(",\"");
    j=s.indexOf("\",",i);
    phone[0]=s.substring(i+2,j);
    i=s.indexOf("\r\n");
    //i=s.indexOf("\r\n",i+2);
    j=s.indexOf("\r\n",i+2);
    text[0]=s.substring(i+2,j);
  
    /*lcd.setCursor(0, 0);
    lcd.print("                ");
    lcd.setCursor(0, 0);
    lcd.print(phone[0]);
    delay(3000);
  
    lcd.setCursor(0, 0);
    lcd.print("                ");
    lcd.setCursor(0, 0);
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
