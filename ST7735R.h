/* 
ST7735R.h - Library for ST7735R 1.8'' TFT display
Created by Matevž Marš, november 2016
Released into the public domain
*/

#ifndef ST7735R_h
#define ST7735R_h

#include "Energia.h"
#include "SPI.h"

#define SWRESET 	0x01
#define SLPIN   	0x10
#define SLPOUT  	0x11
#define PLTON   	0x12
#define NORON   	0x13
#define INVOFF  	0x20
#define INVON   	0x21
#define GAMSET  	0x26
#define DISPOFF 	0x28
#define DISPON  	0x29
#define CASET   	0x2a
#define RASET   	0x2b
#define RAMWR   	0x2c
#define RGBSET  	0x2d
#define RAMRD   	0x2e
#define PTLAR   	0x30
#define SCRLAR  	0x33
#define TEOFF   	0x34
#define TEON    	0x35
#define MADCTL  	0x36
#define VSCSAD  	0x37
#define IDMOFF  	0x38
#define IDMON   	0x39
#define COLMOD  	0x3a
#define RDID1   	0xda
#define RDID2   	0xdb
#define RDID3   	0xdc
#define FLIP_R  	0xA0 
#define FLIP_L  	0x60 
#define FLIP_180	0xC0  


class ST7735R{
	public:
		ST7735R(int pins[2], int mode);
		void begin(void);
		int color(int red, int green, int blue);
		void fillRect(int x, int y, int w, int h, int color);
		void fillScreen(int color);
		void drawPixel(int x, int y, int color);
		void drawArrow(char c, int x, int y, int b, int col, int size);
		int writeWord(String str, int x, int y, int b, int col, int size);
		void drawHeart(int x, int y, int b, int col, int size);
		void drawCircle(int x, int y, int r, int e, int rot);
		void fillCircle(int x, int y, int r, int e, int rot, int color);
		void drawArc(int x, int y, int r, int e, int rot, int s, int f);
		void fillArc(int x, int y, int r, int e, int rot, int s, int f, int color);
	private:
		int _pins[2];
		int _mode;
		int _width;
		int _height;
		void writeCommand(int c);
		void writeData(int c);
		void setAddressWindow(int x0, int y0, int x1, int y1);
		int writeCharacter(char c, int x, int y, int b, int col, int size);	
};


#endif
