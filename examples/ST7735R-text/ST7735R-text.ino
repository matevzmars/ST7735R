/*
TI C2000 Piccolo LaunchPad example

This example demonstrates how to show text on 1.8'' TFT LCD screen.

Written by Matevž Marš, november 2016
*/

#include "ST7735R.h"
#include "SPI.h"

int pins[2]={AB2,AB4}; 
/* Connect pins as shown below:
VCC  -> 3.3 V
GND  -> GND
CS   -> GND
RES  -> AB2
A0   -> AB4
SDA  -> P16
SCK  -> P18
LED  -> 3.3 V
*/

ST7735R screen(pins,0);

void setup(){
  screen.begin();
}

void loop(){
    screen.writeWord("Hello",0,0,0xFFFF,0x0000,1);
    screen.writeWord("Hello",0,8,0xFFFF,0x0000,2);
    screen.writeWord("Hello",0,24,0xFFFF,0x0000,3);    
    screen.writeWord("Hello",0,48,0xFFFF,0x0000,4);
    screen.writeWord("Hello",0,80,0xFFFF,0x0000,5);   
}
