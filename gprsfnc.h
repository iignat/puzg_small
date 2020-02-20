#include <Arduino.h>
#include <string.h>
#include <SoftwareSerial.h>

extern SoftwareSerial gsm;
void atcmdsend(String atcmd,short delayms,byte secho=1);
void gprsinit();
void gprssenddata(byte curr_state,float temp,float airhum);
void gprsupddata(byte curr_state,float temp,float airhum, byte c_step);
