
#include <Arduino.h>
#include "puzgfnc.h"
#define LOW 1
#define HIGH 0
#define UNKNOWN_STATE 100

byte curr_state=UNDEF_STATE;
byte zapusk_delay=0;
byte starts_try_num=0;
byte force_generator_cnt=0;
byte force_osnovnaya_cnt=0;
byte ostanov_generatora_cnt=0;
byte old_generator=UNKNOWN_STATE;
byte new_generator=UNKNOWN_STATE;
byte old_osnovnaya=UNKNOWN_STATE;
byte new_osnovnaya=UNKNOWN_STATE;
byte f_osnovnaya=UNKNOWN_STATE;
byte f_generator=UNKNOWN_STATE;

void pins_init() {
  digitalWrite(GENERATOR_SWCH,LOW);
  digitalWrite(OSNOVNAYA_SWCH,LOW);
  digitalWrite(GENERATOR_ON,LOW);
  digitalWrite(GENERATOR_OFF,LOW);
}

void generator_off(){
  if(curr_state!=OSTANOV_GENERATORA) {
    curr_state=OSTANOV_GENERATORA;
    ostanov_generatora_cnt=0;
  }
  ostanov_generatora_cnt++;
  if(ostanov_generatora_cnt>OSTANOV_GENERATORA1_CNT) {
    //delay(OSNOVNAYA_GENERATOR_SWH_DELAY);
    digitalWrite(GENERATOR_SWCH,LOW);    
    digitalWrite(OSNOVNAYA_SWCH,LOW);
  }
  if(ostanov_generatora_cnt>OSTANOV_GENERATORA2_CNT) {
    digitalWrite(GENERATOR_OFF,HIGH);
    digitalWrite(GENERATOR_ON,LOW);
    delay(1000);
    digitalWrite(GENERATOR_OFF,LOW);
  }
}

void generator_on(){
  if(curr_state==OSHIBKA_ZAPUSKA)return;
  curr_state=ZAPUSK_GENERATORA;
  starts_try_num++;
  
  digitalWrite(OSNOVNAYA_SWCH,LOW);
  digitalWrite(GENERATOR_SWCH,LOW);
  
  digitalWrite(GENERATOR_ON,HIGH);
  digitalWrite(GENERATOR_OFF,LOW);
  delay(DIZEL_STARTER_TIME);
  digitalWrite(GENERATOR_ON,LOW);
}

void ProcessFunc() {
  
  new_osnovnaya=digitalRead(OSNOVNAYA);
  new_generator=digitalRead(GENERATOR);
  
  if(old_osnovnaya!=UNKNOWN_STATE && old_generator!=UNKNOWN_STATE) {
      if(old_osnovnaya==new_osnovnaya && new_generator==old_generator) {
        f_osnovnaya=old_osnovnaya;
        f_generator=old_generator;
      }else {
        old_osnovnaya=new_osnovnaya;
        old_generator=new_generator;
      }
  }else{
    f_osnovnaya=old_osnovnaya=new_osnovnaya;
    f_generator=old_generator=new_generator;
  }
  
  if(f_osnovnaya==1 && f_generator==0){
      digitalWrite(GENERATOR_SWCH,LOW);
      if(curr_state!=SET_OSNOVNAYA) {
        delay(OSNOVNAYA_GENERATOR_SWH_DELAY);
      }            
      
      digitalWrite(OSNOVNAYA_SWCH,HIGH);
      
      curr_state=SET_OSNOVNAYA;
      starts_try_num=0;
            
  }else if(f_osnovnaya==0 && f_generator==1){
     digitalWrite(OSNOVNAYA_SWCH,LOW); 
     if(curr_state!=SET_GENERATORA) {
        delay(OSNOVNAYA_GENERATOR_SWH_DELAY);
      }
      
      digitalWrite(GENERATOR_SWCH,HIGH);      
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

