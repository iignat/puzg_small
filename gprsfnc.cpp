
#include "gprsfnc.h"
#include <Arduino.h>
#include <string.h>
//#include "DHT.h"
#include "puzgfnc.h"

#define LOGIN "user"
#define PASS "asdf"
#define STDDELAY 300

//extern DHT dht;
//extern byte curr_state

void atcmdsend(String atcmd,short delayms,byte secho=1) {
  gsm.println(atcmd);
  gsm.flush();
  delay(delayms);
  if(secho!=1)Serial.println(atcmd);
  while( gsm.available() ) {
    if(secho==1){
          Serial.println("-------------------------------");
          Serial.println(gsm.readString());
   }else{
          gsm.read();
   }
  }
}

void gprsinit() {
  for (int k=0;k<10;k++)atcmdsend(F("AT"),1000);
  atcmdsend(F("AT+CMGF=1"),STDDELAY);
  atcmdsend(F("AT+CSCS=\"GSM\""),STDDELAY);
  atcmdsend(F("AT+CPMS=\"SM\""),STDDELAY);
  atcmdsend(F("AT+CNMI=1,1,0,0,0"),STDDELAY);
  atcmdsend(F("AT+CMGD=1,4"),STDDELAY);
  atcmdsend(F("AT+SAPBR=3,1,\"CONTYPE\",\"GPRS\""),STDDELAY);
  atcmdsend(F("AT+SAPBR=3,1,\"APN\",\"internet.mts.ru\""),STDDELAY);
  atcmdsend(F("AT+SAPBR=3,1,\"USER\",\"mts\""),STDDELAY);
  atcmdsend(F("AT+SAPBR=3,1,\"PWD\",\"mts\""),STDDELAY);
  atcmdsend(F("AT+SAPBR=1,1"),STDDELAY);
}

void gprssenddata(byte curr_state,float temp,float airhum) {
  char buff[200];
  char chr_temp[6];
  char chr_airhum[6];
  dtostrf(temp, 4, 2, chr_temp);
  dtostrf(airhum, 4, 2, chr_airhum);
  atcmdsend(F("AT+HTTPINIT"),STDDELAY);
  atcmdsend(F("AT+HTTPPARA=\"CID\",1"),STDDELAY);
  sprintf(buff,"AT+HTTPPARA=\"URL\",\"iignat1.vds.serverdale.com:8000/?login=%s&pass=%s&a=%d&b=%s&c=%s\"\0",LOGIN,PASS,curr_state,chr_temp,chr_airhum);
  atcmdsend(buff,STDDELAY);
  atcmdsend(F("AT+HTTPACTION=0"),STDDELAY);
  atcmdsend(F("AT+HTTPREAD"),STDDELAY);
  atcmdsend(F("AT+HTTPTERM"),STDDELAY);
}

void gprsupddata(byte curr_state,float temp,float airhum, byte c_step) {
  char buff[200];
  char chr_temp[6];
  char chr_airhum[6];

  switch (c_step) {
    case 1:
      atcmdsend(F("AT+HTTPINIT"),STDDELAY,0);
      break;
    case 2:
      atcmdsend(F("AT+HTTPPARA=\"CID\",1"),STDDELAY,0);
      break;
    case 3:
      dtostrf(temp, 4, 2, chr_temp);
      dtostrf(airhum, 4, 2, chr_airhum);
      sprintf(buff,"AT+HTTPPARA=\"URL\",\"iignat1.vds.serverdale.com:8000/?login=%s&pass=%s&a=%d&b=%s&c=%s\"\0",LOGIN,PASS,curr_state,chr_temp,chr_airhum);
      atcmdsend(buff,STDDELAY,0);    
    case 4:
      atcmdsend(F("AT+HTTPACTION=0"),STDDELAY,0);
      break;
    case 5:
      atcmdsend(F("AT+HTTPREAD"),STDDELAY,0);
      break;
    case 6:
      atcmdsend(F("AT+HTTPTERM"),STDDELAY,0);
      break;
  }
  
}
