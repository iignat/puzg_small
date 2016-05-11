
#include <Arduino.h>
#include "puzgfnc.h"

byte curr_state=0;
byte zapusk_delay=0;
byte starts_try_num=0;
byte force_generator_cnt=0;
byte force_osnovnaya_cnt=0;
byte ostanov_generatora_cnt=0;

void generator_off(){
  if(curr_state!=OSTANOV_GENERATORA) {
    curr_state=OSTANOV_GENERATORA;
    ostanov_generatora_cnt=0;
  }
  ostanov_generatora_cnt++;
  if(ostanov_generatora_cnt>OSTANOV_GENERATORA1_CNT) {
    digitalWrite(GENERATOR_SWCH,LOW);
    delay(OSNOVNAYA_GENERATOR_SWH_DELAY);
    digitalWrite(OSNOVNAYA_SWCH,HIGH);
  }
  if(ostanov_generatora_cnt>OSTANOV_GENERATORA2_CNT) {
    digitalWrite(GENERATOR_OFF,HIGH);
    delay(1000);
    digitalWrite(GENERATOR_OFF,LOW);
  }
}

void generator_on(){
  if(curr_state==OSHIBKA_ZAPUSKA)return;
  curr_state=ZAPUSK_GENERATORA;
  starts_try_num++;
  
  digitalWrite(OSNOVNAYA_SWCH,LOW);
  digitalWrite(GENERATOR_ON,HIGH);
  delay(6000);
  digitalWrite(GENERATOR_ON,LOW);
}

void ProcessFunc() {
  byte f_osnovnaya=0,f_generator=0;
  f_osnovnaya=digitalRead(OSNOVNAYA);
  f_generator=digitalRead(GENERATOR);
  
  if(curr_state==FORCE_OSNOVNAYA){  
    if(f_generator==1 && force_osnovnaya_cnt<FORCE_OSNOVNAYA_SHTD_CNT) {
      digitalWrite(GENERATOR_OFF,HIGH);
      digitalWrite(OSNOVNAYA_SWCH,HIGH);
        delay(1000);
      digitalWrite(GENERATOR_OFF,LOW);
      force_osnovnaya_cnt++;
    }else force_osnovnaya_cnt=0;
     
    digitalWrite(GENERATOR_SWCH,LOW);    
    delay(OSNOVNAYA_GENERATOR_SWH_DELAY);
    digitalWrite(OSNOVNAYA_SWCH,HIGH);
    
    return;
    
  }else if(curr_state==FORCE_NOPOWER){
  
    if(f_generator==1 && force_osnovnaya_cnt<FORCE_OSNOVNAYA_SHTD_CNT) {
      digitalWrite(GENERATOR_OFF,HIGH);
      digitalWrite(OSNOVNAYA_SWCH,HIGH);
        delay(1000);
      digitalWrite(GENERATOR_OFF,LOW);
      force_osnovnaya_cnt++;
    }else force_osnovnaya_cnt=0;
    
    digitalWrite(OSNOVNAYA_SWCH,LOW);
    digitalWrite(GENERATOR_SWCH,LOW);
    return;
  }else if(curr_state==FORCE_GENERATOR){  
    if(f_generator==0 && force_generator_cnt<FORCE_GENERATOR_START_CNT) {
      digitalWrite(GENERATOR_ON,HIGH);
      delay(6000);  
      digitalWrite(GENERATOR_ON,LOW);
      force_generator_cnt++;
    }else force_generator_cnt=0;
    
    digitalWrite(OSNOVNAYA_SWCH,LOW);
    delay(OSNOVNAYA_GENERATOR_SWH_DELAY);
    digitalWrite(GENERATOR_SWCH,HIGH);
    return;
  }
  
  force_osnovnaya_cnt=0;
  force_generator_cnt=0;
  
  if(f_osnovnaya==1 && f_generator==0){
      if(curr_state!=SET_OSNOVNAYA) {
        digitalWrite(GENERATOR_SWCH,LOW);
        delay(OSNOVNAYA_GENERATOR_SWH_DELAY);
        digitalWrite(OSNOVNAYA_SWCH,HIGH);
      }
      curr_state=SET_OSNOVNAYA;
      starts_try_num=0;
      
      
  }else if(f_osnovnaya==0 && f_generator==1){
      if(curr_state!=SET_GENERATORA) {
        digitalWrite(OSNOVNAYA_SWCH,LOW);
        delay(OSNOVNAYA_GENERATOR_SWH_DELAY);
        digitalWrite(GENERATOR_SWCH,HIGH);
      }
      curr_state=SET_GENERATORA;
      starts_try_num=0;
  } 
   
  if(curr_state==ZAPUSK_GENERATORA){
    zapusk_delay++;
    if(zapusk_delay>=10 && starts_try_num>START_NUM){
      curr_state=OSHIBKA_ZAPUSKA;
      zapusk_delay=0;
      starts_try_num=0;
    }else if(zapusk_delay>=30) {
      generator_on();
      zapusk_delay=0;
    }
  }else if(f_osnovnaya==0 && f_generator==0 && curr_state==SET_OSNOVNAYA){
    curr_state=PROPALA_OSNOVNAYA_SET;
    zapusk_delay=0;
  }else if(f_osnovnaya==0 && f_generator==0 && curr_state==PROPALA_OSNOVNAYA_SET){
    zapusk_delay++;
    if(zapusk_delay>30){
      zapusk_delay=0;
      generator_on();
    }
  }else if(f_osnovnaya==0 && f_generator==0 && curr_state!=OSHIBKA_ZAPUSKA)generator_on();
  
  if(f_osnovnaya==1 && f_generator==1)generator_off();
  
}
