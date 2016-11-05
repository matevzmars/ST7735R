/*
TI C2000 Piccolo LaunchPad example

This example demonstrates how to show text on 1.8'' TFT LCD screen.

Written by Matevž Marš, november 2016
*/

#include "ST7735R.h"

int pins[3]={AA2,AB2,AB4}; 
/* Connect pins as shown below:
VCC  -> 3.3 V
GND  -> GND
CS   -> AA2
RES  -> AB2
A0   -> AB4
SDA  -> P16
SCK  -> P17
LED  -> 3.3 V
*/

ST7735R screen(pins,0);

void setup(){
  screen.begin();
}

void loop(){
    screen.writeWord('Hello',0,0,0x0000,0xFFFF,1);
    screen.writeWord('Hello',0,8,0x0000,0xFFFF,2);
    screen.writeWord('Hello',0,24,0x0000,0xFFFF,3);    
    screen.writeWord('Hello',0,48,0x0000,0xFFFF,4);
    screen.writeWord('Hello',0,80,0x0000,0xFFFF,5);   
}
