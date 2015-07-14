#ifndef GSMFNC_H
#define GSMFNC_H
#include <string.h>
#include <LiquidCrystal.h>

extern LiquidCrystal lcd;

String ClearSerial();
String getLine(uint8_t w);

String send(const String msg,String *ans=0, char echo=1);

void updateOper(String *s);
void updateSignal(String *s);
void printError(String *s);
void sendTextMessage(String phnum,String msg);
char getTextMessage(String *phone,String *text);
char balans(String *ans);

void ErrorMsg(char b=1);

#endif GSMFNC_H
