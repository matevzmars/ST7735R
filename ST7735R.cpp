/* 
ST7735R.cpp - Library for ST7735R 1.8'' TFT display
Created by Matevž Marš, november 2016
Released into the public domain
*/

#include "Energia.h"
#include "ST7735R.h"
#include "SPI.h"
#include "pins_energia.h"

#define anSET 		GpioDataRegs.AIOSET.all
#define anCLEAR 	GpioDataRegs.AIOCLEAR.all
#define digSET		GpioDataRegs.GPASET.all
#define digCLEAR	GpioDataRegs.GPACLEAR.all


ST7735R::ST7735R(int pins[3], int mode){
	for(int i=0;i<3;i++){
		_pins[i] = pins[i];
	}

	_mode = mode;	
}

void ST7735R::digitalHigh(int pin){
	int _gpio_number = pin_mapping[pin];	
	if(_gpio_number & 0x8000){
		anSET = (1 << (_gpio_number - 0x8000));
	}
	else{
		digSET = (1 << _gpio_number);
	}
}

void ST7735R::digitalLow(int pin){
	int _gpio_number = pin_mapping[pin];	
	if(_gpio_number & 0x8000){
		anCLEAR = (1 << (_gpio_number - 0x8000));
	}
	else{
		digCLEAR = (1 << _gpio_number);
	}
}

void ST7735R::begin(void){
	for(int i=0;i<3;i++){
		pinMode(_pins[i],OUTPUT);
	}
	digitalHigh(_pins[0]);
	digitalLow(_pins[1]);
	SPI.begin();
	SPI.setDataMode(SPI_MODE1);

	delay(1);
	digitalHigh(_pins[1]);
	delay(150);
	
	_writeCommand(SLPOUT); //don't sleep
	delay(150); 
	
	switch(_mode){ //flip image: currently not working
		case 0:
			_height=160;
			_width=128;
		case 1:
			_writeCommand(MADCTL);
			_writeData(FLIP_R);
			delay(10);
			_height=128;
			_width=160;
		case 2:
			_writeCommand(MADCTL);
			_writeData(FLIP_180);
			delay(10);
			_height=160;
			_width=128;
		case 3:
			_writeCommand(MADCTL);
			_writeData(FLIP_L);
			delay(10);
			_height=128;
			_width=160;
	}
	
	_writeCommand(DISPON);  // turn display on
	delay(10);

	_writeCommand(COLMOD);  // request bit per pixel change
	_writeData(0x05);  // 16 bit per pixel
	delay(10);
	
	fillScreen(0xFFFF); //set white background
}

void ST7735R::_writeCommand(int c){
	digitalLow(_pins[2]);
	digitalLow(_pins[0]);
	SPI.transfer(c);
	digitalHigh(_pins[0]);
}

void ST7735R::_writeData(int c){
	digitalHigh(_pins[2]);
	digitalLow(_pins[0]);
	SPI.transfer(c);
	digitalHigh(_pins[0]);
}

void ST7735R::_setAddressWindow(int x0, int y0, int x1, int y1){
	_writeCommand(CASET); //column addr set
	_writeData(0x00);
	_writeData(x0); //xstart
	_writeData(0x00);
	_writeData(x1); //xend

	_writeCommand(RASET); //row addr set
	_writeData(0x00);
	_writeData(y0); //ystart
	_writeData(0x00);
	_writeData(y1); //yend 
}

int ST7735R::color(int red, int green, int blue){
	return ((red & 0xF8 << 8)) | ((green & 0xFC) << 3) | (blue >> 3);
}

void ST7735R::fillRect(int x, int y, int w, int h, int color){
	if((x < 0) ||(x >= _width) || (y < 0) || (y >= _height)) return;
	if((x + w - 1) >= _width)  w = _width  - x;
	if((y + h - 1) >= _height) h = _height - y;
  
   _setAddressWindow(x, y, x+w-1, y+h-1);
   
	int hi = color >> 8;
	int lo = color & 0x0F;
   
	_writeCommand(RAMWR);
   
	digitalHigh(_pins[2]); 
	digitalLow(_pins[0]); 
   
	for(y=h;y>0;y--){
		for(x=w;x>0;x--){
			SPI.transfer(hi);  
			SPI.transfer(lo);
		}	
	}
	
	digitalHigh(_pins[0]);
}

void ST7735R::fillScreen(int color){
	fillRect(0, 0, _width, _height, color);
}

void ST7735R::drawPixel(int x, int y, int color){
	if((x < 0) ||(x >= _width) || (y < 0) || (y >= _height)) return;
	_setAddressWindow(x,y,x+1,y+1);
   
	_writeCommand(RAMWR);
   
	digitalHigh(_pins[2]); 
	digitalLow(_pins[0]); 
	SPI.transfer(color >> 8);
	SPI.transfer(color & 0x0F);
	digitalHigh(_pins[0]);
}

int ST7735R::_writeCharacter(char c, int x, int y, int b, int col, int size){
	switch(c){
		case ' ':
			fillRect(x,y,2*size,7*size,b);
			return 2*size;     
		case 'A':
			fillRect(x,y,6*size,7*size,b);
			fillRect(x+2*size,y,3*size,size,col);
			fillRect(x+size,y+size,size,6*size,col);
			fillRect(x+5*size,y+size,size,6*size,col);
			fillRect(x+2*size,y+4*size,3*size,size,col);
			return 6*size;
		case 'B':
			fillRect(x,y,6*size,7*size,b);
			fillRect(x+2*size,y,3*size,size,col);
			fillRect(x+2*size,y+3*size,3*size,size,col);
			fillRect(x+2*size,y+6*size,3*size,size,col);
			fillRect(x+size,y,size,7*size,col);
			fillRect(x+5*size,y+size,size,2*size,col);
			fillRect(x+5*size,y+4*size,size,2*size,col);
			return 6*size;
		case 'C':
			fillRect(x,y,6*size,7*size,b);
			fillRect(x+size,y+size,size,5*size,col);
			fillRect(x+2*size,y,3*size,size,col);
			fillRect(x+2*size,y+6*size,3*size,size,col);
			fillRect(x+5*size,y+size,size,size,col);
			fillRect(x+5*size,y+5*size,size,size,col);
			return 6*size;
		case 'D':
			fillRect(x,y,6*size,7*size,b);     
			fillRect(x+size,y,size,7*size,col);
			fillRect(x+2*size,y,3*size,size,col);
			fillRect(x+2*size,y+6*size,3*size,size,col);
			fillRect(x+5*size,y+size,size,5*size,col);
			return 6*size;   
		case 'E':
			fillRect(x,y,6*size,7*size,b);
			fillRect(x+size,y,size,7*size,col);
			fillRect(x+2*size,y,4*size,size,col);
			fillRect(x+2*size,y+6*size,4*size,size,col);
			fillRect(x+2*size,y+3*size,3*size,size,col);
			return 6*size;
		case 'F':
			fillRect(x,y,6*size,7*size,b);
			fillRect(x+size,y,size,7*size,col);
			fillRect(x+2*size,y,4*size,size,col);
			fillRect(x+2*size,y+3*size,3*size,size,col);
			return 6*size;
		case 'G':
			fillRect(x,y,6*size,7*size,b);
			fillRect(x+size,y+size,size,5*size,col);
			fillRect(x+2*size,y,3*size,size,col);
			fillRect(x+2*size,y+6*size,3*size,size,col);
			fillRect(x+5*size,y+size,size,size,col);
			fillRect(x+5*size,y+3*size,size,3*size,col);
			fillRect(x+4*size,y+3*size,size,size,col);
			return 6*size;
		case 'H':
			fillRect(x,y,6*size,7*size,b);
			fillRect(x+size,y,size,7*size,col);
			fillRect(x+5*size,y,size,7*size,col);
			fillRect(x+2*size,y+3*size,3*size,size,col);
			return 6*size;
		case 'I':
			fillRect(x,y,6*size,7*size,b);
			fillRect(x+3*size,y+size,size,5*size,col);
			fillRect(x+size,y,5*size,size,col);
			fillRect(x+size,y+6*size,5*size,size,col);
			return 6*size;
		case 'J':
			fillRect(x,y,6*size,7*size,b);
			fillRect(x+5*size,y,size,6*size,col);
			fillRect(x+2*size,y,3*size,size,col);
			fillRect(x+size,y+4*size,size,2*size,col);
			fillRect(x+2*size,y+6*size,3*size,size,col);
			return 6*size;
		case 'K':
			fillRect(x,y,6*size,7*size,b);
			fillRect(x+size,y,size,7*size,col);
			fillRect(x+2*size,y+3*size,size,size,col);
			fillRect(x+3*size,y+2*size,size,size,col);
			fillRect(x+3*size,y+4*size,size,size,col);
			fillRect(x+4*size,y+size,size,size,col);
			fillRect(x+4*size,y+5*size,size,size,col);
			fillRect(x+5*size,y,size,size,col);
			fillRect(x+5*size,y+6*size,size,size,col);
			return 6*size;
		case 'L':
			fillRect(x,y,6*size,7*size,b);
			fillRect(x+size,y,size,7*size,col);
			fillRect(x+2*size,y+6*size,4*size,size,col);
			return 6*size;
		case 'M':
			fillRect(x,y,6*size,7*size,b);
			fillRect(x+size,y,size,7*size,col);
			fillRect(x+2*size,y+size,size,size,col);
			fillRect(x+3*size,y+2*size,size,size,col);
			fillRect(x+4*size,y+size,size,size,col);
			fillRect(x+5*size,y,size,7*size,col);
			return 6*size;
		case 'N':
			fillRect(x,y,6*size,7*size,b);
			fillRect(x+size,y,size,7*size,col);
			fillRect(x+2*size,y+2*size,size,size,col);
			fillRect(x+3*size,y+3*size,size,size,col);
			fillRect(x+4*size,y+4*size,size,size,col);
			fillRect(x+5*size,y,size,7*size,col);
			return 6*size;
		case 'O':
			fillRect(x,y,6*size,7*size,b);
			fillRect(x+size,y+size,size,5*size,col);
			fillRect(x+2*size,y,3*size,size,col);
			fillRect(x+2*size,y+6*size,3*size,size,col);
			fillRect(x+5*size,y+size,size,5*size,col);
			return 6*size;
		case 'P':
			fillRect(x,y,6*size,7*size,b);
			fillRect(x+size,y,size,7*size,col);
			fillRect(x+2*size,y,3*size,size,col);
			fillRect(x+2*size,y+3*size,3*size,size,col);
			fillRect(x+5*size,y+size,size,2*size,col);
			return 6*size;
		case 'Q':
			fillRect(x,y,6*size,7*size,b);
			fillRect(x+size,y+size,size,5*size,col);
			fillRect(x+2*size,y,3*size,size,col);
			fillRect(x+2*size,y+6*size,3*size,size,col);
			fillRect(x+5*size,y+size,size,5*size,col);
			fillRect(x+4*size,y+5*size,size,size,col);
			fillRect(x+3*size,y+4*size,size,size,col);
			return 6*size;
		case 'R':
			fillRect(x,y,6*size,7*size,b);
			fillRect(x+size,y,size,7*size,col);
			fillRect(x+2*size,y,3*size,size,col);
			fillRect(x+2*size,y+3*size,3*size,size,col);
			fillRect(x+5*size,y+size,size,2*size,col);
			fillRect(x+5*size,y+4*size,size,3*size,col);
			return 6*size;
		case 'S':
			fillRect(x,y,6*size,7*size,b);
			fillRect(x+size,y+size,size,2*size,col);
			fillRect(x+2*size,y,3*size,size,col);
			fillRect(x+2*size,y+3*size,3*size,size,col);
			fillRect(x+5*size,y+4*size,size,2*size,col);
			fillRect(x+2*size,y+6*size,3*size,size,col);
			fillRect(x+5*size,y+size,size,size,col);
			fillRect(x+size,y+5*size,size,size,col);
			return 6*size;
		case 'T':
			fillRect(x,y,6*size,7*size,b);
			fillRect(x+size,y,5*size,size,col);
			fillRect(x+3*size,y+size,size,6*size,col);
			return 6*size;
		case 'U':
			fillRect(x,y,6*size,7*size,b);
			fillRect(x+size,y,size,6*size,col);
			fillRect(x+5*size,y,size,6*size,col);
			fillRect(x+2*size,y+6*size,3*size,size,col);
			return 6*size;
		case 'V':
			fillRect(x,y,6*size,7*size,b);
			fillRect(x+size,y,size,5*size,col);
			fillRect(x+5*size,y,size,5*size,col);
			fillRect(x+2*size,y+5*size,size,size,col);
			fillRect(x+3*size,y+6*size,size,size,col);
			fillRect(x+4*size,y+5*size,size,size,col);
			return 6*size;
		case 'W':
			fillRect(x,y,6*size,7*size,b);
			fillRect(x+size,y,size,6*size,col);
			fillRect(x+5*size,y,size,6*size,col);
			fillRect(x+2*size,y+6*size,size,size,col);
			fillRect(x+3*size,y+4*size,size,2*size,col);
			fillRect(x+4*size,y+6*size,size,size,col);
			return 6*size;
		case 'X':
			fillRect(x,y,6*size,7*size,b);
			fillRect(x+size,y,size,2*size,col);
			fillRect(x+size,y+5*size,size,2*size,col);
			fillRect(x+5*size,y,size,2*size,col);
			fillRect(x+5*size,y+5*size,size,2*size,col);
			fillRect(x+2*size,y+2*size,size,size,col);
			fillRect(x+2*size,y+4*size,size,size,col);
			fillRect(x+3*size,y+3*size,size,size,col);
			fillRect(x+4*size,y+2*size,size,size,col);
			fillRect(x+4*size,y+4*size,size,size,col);
			return 6*size;
		case 'Y':
			fillRect(x,y,6*size,7*size,b);
			fillRect(x+size,y,size,3*size,col);
			fillRect(x+5*size,y,size,3*size,col);
			fillRect(x+2*size,y+3*size,size,size,col);
			fillRect(x+3*size,y+4*size,size,3*size,col);
			fillRect(x+4*size,y+3*size,size,size,col);
			return 6*size;
		case 'Z':
			fillRect(x,y,6*size,7*size,b);
			fillRect(x+size,y,5*size,size,col);
			fillRect(x+5*size,y+size,size,size,col);
			fillRect(x+4*size,y+2*size,size,size,col);
			fillRect(x+3*size,y+3*size,size,size,col);
			fillRect(x+2*size,y+4*size,size,size,col);
			fillRect(x+size,y+5*size,size,size,col);
			fillRect(x+size,y+6*size,5*size,size,col);
			return 6*size;
		case 'a':
			fillRect(x,y,6*size,7*size,b);
			fillRect(x+2*size,y+2*size,4*size,size,col);
			fillRect(x+size,y+3*size,size,3*size,col);
			fillRect(x+5*size,y+3*size,size,3*size,col);
			fillRect(x+2*size,y+6*size,4*size,size,col);
			return 6*size;
		case 'b':
			fillRect(x,y,6*size,7*size,b);
			fillRect(x+2*size,y+2*size,3*size,size,col);
			fillRect(x+size,y,size,7*size,col);
			fillRect(x+5*size,y+3*size,size,3*size,col);
			fillRect(x+2*size,y+6*size,3*size,size,col);
			return 6*size;
		case 'c':
			fillRect(x,y,6*size,7*size,b);
			fillRect(x+2*size,y+2*size,3*size,size,col);
			fillRect(x+size,y+3*size,size,3*size,col);
			fillRect(x+5*size,y+3*size,size,size,col);
			fillRect(x+5*size,y+5*size,size,size,col);
			fillRect(x+2*size,y+6*size,3*size,size,col);
			return 6*size;
		case 'd':
			fillRect(x,y,6*size,7*size,b);
			fillRect(x+2*size,y+2*size,3*size,size,col);
			fillRect(x+size,y+3*size,size,3*size,col);
			fillRect(x+5*size,y,size,7*size,col);
			fillRect(x+2*size,y+6*size,3*size,size,col);
			return 6*size;
		case 'e':
			fillRect(x,y,6*size,7*size,b);
			fillRect(x+2*size,y+2*size,3*size,size,col);
			fillRect(x+size,y+3*size,size,3*size,col);
			fillRect(x+2*size,y+6*size,3*size,size,col);
			fillRect(x+5*size,y+3*size,size,size,col);
			fillRect(x+3*size,y+4*size,3*size,size,col);
			return 6*size;
		case 'f':
			fillRect(x,y,6*size,8*size,b);
			fillRect(x+3*size,y+2*size,size,6*size,col);
			fillRect(x+4*size,y+size,size,size,col);
			fillRect(x+2*size,y+4*size,size,size,col);
			fillRect(x+4*size,y+4*size,size,size,col);
			return 6*size;
		case 'g':
			fillRect(x,y,6*size,8*size,b);
			fillRect(x+2*size,y+2*size,3*size,size,col);
			fillRect(x+size,y+3*size,size,2*size,col);
			fillRect(x+5*size,y+3*size,size,2*size,col);
			fillRect(x+2*size,y+5*size,3*size,size,col);
			fillRect(x+5*size,y+6*size,size,size,col);
			fillRect(x+2*size,y+7*size,3*size,size,col);
			return 6*size;
		case 'h':
			fillRect(x,y,6*size,7*size,b);
			fillRect(x+size,y,size,7*size,col);
			fillRect(x+2*size,y+3*size,3*size,size,col);
			fillRect(x+5*size,y+4*size,size,3*size,col);
			return 6*size;
		case 'i':
			fillRect(x,y,2*size,7*size,b);
			fillRect(x+size,y+size,size,size,col);
			fillRect(x+size,y+3*size,size,4*size,col);
			return 2*size;
		case 'j':
			fillRect(x,y,6*size,8*size,b);
			fillRect(x+5*size,y+size,size,size,col);
			fillRect(x+5*size,y+3*size,size,4*size,col);
			fillRect(x+size,y+6*size,size,size,col);
			fillRect(x+2*size,y+7*size,3*size,size,col);
			return 6*size;
		case 'k':
			fillRect(x,y,5*size,7*size,b);
			fillRect(x+size,y,size,7*size,col);
			fillRect(x+2*size,y+4*size,size,size,col);
			fillRect(x+3*size,y+3*size,size,size,col);
			fillRect(x+3*size,y+5*size,size,size,col);
			fillRect(x+4*size,y+2*size,size,size,col);
			fillRect(x+4*size,y+6*size,size,size,col);
			return 5*size;
		case 'l':
			fillRect(x,y,4*size,7*size,b);
			fillRect(x+size,y,size,6*size,col);
			fillRect(x+2*size,y+6*size,2*size,size,col);
			return 4*size;
		case 'm':
			fillRect(x,y,6*size,7*size,b);
			fillRect(x+2*size,y+2*size,size,size,col);
			fillRect(x+4*size,y+2*size,size,size,col);
			fillRect(x+size,y+2*size,size,5*size,col);
			fillRect(x+3*size,y+3*size,size,4*size,col);
			fillRect(x+5*size,y+3*size,size,4*size,col);
			return 6*size;
		case 'n':
			fillRect(x,y,6*size,7*size,b);
			fillRect(x+size,y+2*size,size,5*size,col);
			fillRect(x+2*size,y+2*size,3*size,size,col);
			fillRect(x+5*size,y+3*size,size,4*size,col);
			return 6*size;
		case 'o':
			fillRect(x,y,6*size,7*size,b);
			fillRect(x+2*size,y+2*size,3*size,size,col);
			fillRect(x+size,y+3*size,size,3*size,col);
			fillRect(x+5*size,y+3*size,size,3*size,col);
			fillRect(x+2*size,y+6*size,3*size,size,col);
			return 6*size;
		case 'p':
			fillRect(x,y,6*size,8*size,b);
			fillRect(x+size,y+2*size,size,6*size,col);
			fillRect(x+2*size,y+2*size,3*size,size,col);
			fillRect(x+2*size,y+6*size,3*size,size,col);
			fillRect(x+5*size,y+3*size,size,3*size,col);
			return 6*size;
		case 'q':
			fillRect(x,y,6*size,7*size,b);
			fillRect(x+2*size,y+2*size,3*size,size,col);
			fillRect(x+size,y+3*size,size,3*size,col);
			fillRect(x+5*size,y+3*size,size,3*size,col);
			fillRect(x+2*size,y+6*size,3*size,size,col);
			fillRect(x+4*size,y+5*size,size,size,col);
			fillRect(x+3*size,y+4*size,size,size,col);
			return 6*size;
		case 'r':
			fillRect(x,y,6*size,7*size,b);
			fillRect(x+size,y+3*size,size,4*size,col);
			fillRect(x+2*size,y+2*size,3*size,size,col);
			fillRect(x+5*size,y+3*size,size,size,col);
			return 6*size;
		case 's':
			fillRect(x,y,6*size,7*size,b);
			fillRect(x+2*size,y+size,3*size,size,col);
			fillRect(x+size,y+2*size,size,size,col);
			fillRect(x+5*size,y+2*size,size,size,col);
			fillRect(x+2*size,y+3*size,2*size,size,col);
			fillRect(x+3*size,y+4*size,2*size,size,col);
			fillRect(x+size,y+5*size,size,size,col);
			fillRect(x+5*size,y+5*size,size,size,col);
			fillRect(x+2*size,y+6*size,3*size,size,col);
			return 6*size;
		case 't':
			fillRect(x,y,5*size,7*size,b);
			fillRect(x+2*size,y,size,6*size,col);
			fillRect(x+size,y+2*size,size,size,col);
			fillRect(x+3*size,y+2*size,size,size,col);
			fillRect(x+3*size,y+6*size,2*size,size,col);
			return 5*size;
		case 'u':
			fillRect(x,y,6*size,7*size,b);
			fillRect(x+size,y+2*size,size,4*size,col);
			fillRect(x+5*size,y+2*size,size,4*size,col);
			fillRect(x+2*size,y+6*size,3*size,size,col);
			return 6*size;
		case 'v':
			fillRect(x,y,6*size,7*size,b);
			fillRect(x+size,y+2*size,size,3*size,col);
			fillRect(x+5*size,y+2*size,size,3*size,col);
			fillRect(x+2*size,y+5*size,size,size,col);
			fillRect(x+4*size,y+5*size,size,size,col);
			fillRect(x+3*size,y+6*size,size,size,col);
			return 6*size;
		case 'w':
			fillRect(x,y,6*size,7*size,b);
			fillRect(x+size,y+2*size,size,4*size,col);
			fillRect(x+5*size,y+2*size,size,4*size,col);
			fillRect(x+2*size,y+6*size,size,size,col);
			fillRect(x+4*size,y+6*size,size,size,col);
			fillRect(x+3*size,y+5*size,size,size,col);
			return 6*size;
		case 'x':
			fillRect(x,y,6*size,7*size,b);
			fillRect(x+size,y+2*size,size,size,col);
			fillRect(x+5*size,y+2*size,size,size,col);
			fillRect(x+2*size,y+3*size,size,size,col);
			fillRect(x+4*size,y+3*size,size,size,col);
			fillRect(x+3*size,y+4*size,size,size,col);
			fillRect(x+2*size,y+5*size,size,size,col);
			fillRect(x+4*size,y+5*size,size,size,col);
			fillRect(x+size,y+6*size,size,size,col);
			fillRect(x+5*size,y+6*size,size,size,col);
			return 6*size;
		case 'y':
			fillRect(x,y,6*size,8*size,b);
			fillRect(x+size,y+2*size,size,3*size,col);
			fillRect(x+5*size,y+2*size,size,5*size,col);
			fillRect(x+2*size,y+5*size,3*size,size,col);
			fillRect(x+2*size,y+7*size,3*size,size,col);
			return 6*size;
		case 'z':
			fillRect(x,y,6*size,7*size,b);
			fillRect(x+size,y+2*size,5*size,size,col);
			fillRect(x+4*size,y+3*size,size,size,col);
			fillRect(x+3*size,y+4*size,size,size,col);
			fillRect(x+2*size,y+5*size,size,size,col);
			fillRect(x+size,y+6*size,5*size,size,col);
			return 6*size;
		case '1':
			fillRect(x,y,6*size,7*size,b);
			fillRect(x+size,y,2*size,size,col);
			fillRect(x+3*size,y,size,6*size,col);
			fillRect(x+size,y+6*size,5*size,size,col);
			return 6*size;
		case '2':
			fillRect(x,y,6*size,7*size,b);
			fillRect(x+2*size,y,3*size,size,col);
			fillRect(x+size,y+size,size,size,col);
			fillRect(x+5*size,y+size,size,2*size,col);
			fillRect(x+4*size,y+3*size,size,size,col);
			fillRect(x+3*size,y+4*size,size,size,col);
			fillRect(x+2*size,y+5*size,size,size,col);
			fillRect(x+size,y+6*size,5*size,size,col);
			return 6*size;
		case '3':
			fillRect(x,y,6*size,7*size,b);
			fillRect(x+2*size,y,3*size,size,col);
			fillRect(x+size,y+size,size,size,col);
			fillRect(x+5*size,y+size,size,2*size,col);
			fillRect(x+3*size,y+3*size,2*size,size,col);
			fillRect(x+5*size,y+4*size,size,2*size,col);
			fillRect(x+size,y+5*size,size,size,col);
			fillRect(x+2*size,y+6*size,3*size,size,col);
			return 6*size;
		case '4':
			fillRect(x,y,6*size,7*size,b);
			fillRect(x+4*size,y,size,7*size,col);
			fillRect(x+3*size,y+size,size,size,col);
			fillRect(x+2*size,y+2*size,size,size,col);
			fillRect(x+size,y+3*size,size,size,col);
			fillRect(x+size,y+4*size,3*size,size,col);
			fillRect(x+5*size,y+4*size,size,size,col);
			return 6*size;
		case '5':
			fillRect(x,y,6*size,7*size,b);
			fillRect(x+size,y,5*size,size,col);
			fillRect(x+size,y+size,size,size,col);
			fillRect(x+size,y+2*size,4*size,size,col);
			fillRect(x+5*size,y+3*size,size,3*size,col);
			fillRect(x+size,y+5*size,size,size,col);
			fillRect(x+2*size,y+6*size,3*size,size,col);
			return 6*size;
		case '6':
			fillRect(x,y,6*size,7*size,b);
			fillRect(x+2*size,y,3*size,size,col);
			fillRect(x+size,y+size,size,5*size,col);
			fillRect(x+2*size,y+3*size,3*size,size,col);
			fillRect(x+5*size,y+4*size,size,2*size,col);
			fillRect(x+2*size,y+6*size,3*size,size,col);
			return 6*size;
		case '7':
			fillRect(x,y,6*size,7*size,b);
			fillRect(x+size,y,5*size,size,col);
			fillRect(x+5*size,y+size,size,2*size,col);
			fillRect(x+4*size,y+3*size,size,size,col);
			fillRect(x+3*size,y+4*size,size,3*size,col);
			return 6*size;
		case '8':
			fillRect(x,y,6*size,7*size,b);
			fillRect(x+2*size,y,3*size,size,col);
			fillRect(x+size,y+size,size,2*size,col);
			fillRect(x+5*size,y+size,size,2*size,col);
			fillRect(x+2*size,y+3*size,3*size,size,col);
			fillRect(x+size,y+4*size,size,2*size,col);
			fillRect(x+5*size,y+4*size,size,2*size,col);
			fillRect(x+2*size,y+6*size,3*size,size,col);
			return 6*size;
		case '9':
			fillRect(x,y,6*size,7*size,b);
			fillRect(x+2*size,y,3*size,size,col);
			fillRect(x+size,y+size,size,2*size,col);
			fillRect(x+5*size,y+size,size,5*size,col);
			fillRect(x+2*size,y+3*size,3*size,size,col);
			fillRect(x+size,y+5*size,size,size,col);
			fillRect(x+2*size,y+6*size,3*size,size,col);
			return 6*size;
		case '0':
			fillRect(x,y,6*size,7*size,b);
			fillRect(x+2*size,y,3*size,size,col);
			fillRect(x+size,y+size,size,5*size,col);
			fillRect(x+5*size,y+size,size,5*size,col);
			fillRect(x+2*size,y+6*size,3*size,size,col);
			return 6*size;
		case '!':
			fillRect(x,y,2*size,7*size,b);
			fillRect(x+size,y,size,5*size,col);
			fillRect(x+size,y+6*size,size,size,col);
			return 2*size;
		case '?':
			fillRect(x,y,6*size,7*size,b);
			fillRect(x+2*size,y,3*size,size,col);
			fillRect(x+size,y+size,size,size,col);
			fillRect(x+5*size,y+size,size,2*size,col);
			fillRect(x+4*size,y+3*size,size,size,col);
			fillRect(x+3*size,y+4*size,size,size,col);
			fillRect(x+3*size,y+6*size,size,size,col);
			return 6*size;
		case ':':
			fillRect(x,y,2*size,7*size,b);
			fillRect(x+size,y+2*size,size,size,col);
			fillRect(x+size,y+4*size,size,size,col);
			return 2*size; 
		case '.':
			fillRect(x,y,2*size,7*size,b);
			fillRect(x+size,y+6*size,size,size,col);
			return 2*size;
		case ',':
			fillRect(x,y,2*size,7*size,b);
			fillRect(x+size,y+5*size,size,2*size,col);
			return 2*size;
	}
	return 0;
}

void ST7735R::drawArrow(char c, int x, int y, int b, int col, int size){
	switch(c){
		case 'R': //right arrow
			fillRect(x,y,9*size,9*size,b);
			fillRect(x+7*size,y+4*size,size,size,col);
			fillRect(x+6*size,y+3*size,size,3*size,col);
			fillRect(x+5*size,y+2*size,size,5*size,col);
			fillRect(x+4*size,y+size,size,7*size,col);
			fillRect(x+size,y+3*size,3*size,3*size,col); 
			break;
		case 'L': //left arrow
			fillRect(x,y,9*size,9*size,b);
			fillRect(x+size,y+4*size,size,size,col);
			fillRect(x+2*size,y+3*size,size,3*size,col);
			fillRect(x+3*size,y+2*size,size,5*size,col);
			fillRect(x+4*size,y+size,size,7*size,col);
			fillRect(x+5*size,y+3*size,3*size,3*size,col); 
			break;
		case 'U': //up arrow
			fillRect(x,y,9*size,9*size,b);
			fillRect(x+4*size,y+size,size,size,col);
			fillRect(x+3*size,y+2*size,3*size,size,col);
			fillRect(x+2*size,y+3*size,5*size,size,col);
			fillRect(x+size,y+4*size,7*size,size,col);
			fillRect(x+3*size,y+5*size,3*size,3*size,col); 
			break;
		case 'D': //down arrow
			fillRect(x,y,9*size,9*size,b);
			fillRect(x+4*size,y+7*size,size,size,col);
			fillRect(x+3*size,y+6*size,3*size,size,col);
			fillRect(x+2*size,y+5*size,5*size,size,col);
			fillRect(x+size,y+4*size,7*size,size,col);
			fillRect(x+3*size,y+size,3*size,3*size,col); 
			break;
	} 
}

int ST7735R::writeWord(String str, int x, int y, int b, int col, int size){
	int a;
	
	for(int i=0;i<str.length();i++){
		a=_writeCharacter(str[i],x,y,b,col,size);
		x=x+a;   
	} 
	return x;
}

void ST7735R::drawHeart(int x, int y, int b, int col, int size){
	fillRect(x,y,9*size,7*size,b);
	fillRect(x+size,y,2*size,size,col);
	fillRect(x+6*size,y,2*size,size,col);
	fillRect(x,y+size,4*size,2*size,col);
	fillRect(x+5*size,y+size,4*size,2*size,col);
	fillRect(x+4*size,y+2*size,size,size,col);
	fillRect(x+size,y+3*size,7*size,size,col);
	fillRect(x+2*size,y+4*size,5*size,size,col);
	fillRect(x+3*size,y+5*size,3*size,size,col);
	fillRect(x+4*size,y+6*size,size,size,col); 
}

void ST7735R::drawCircle(int x, int y, int r, int e, int rot){
	
}

void ST7735R::fillCircle(int x, int y, int r, int e, int rot, int color){
	
}

void ST7735R::drawArc(int x, int y, int r, int e, int rot, int s, int f){
	
}

void ST7735R::fillArc(int x, int y, int r, int e, int rot, int s, int f, int color){
	
}



