#ifndef __DISPLAYST7735_H
#define __DISPLAYST7735_H

#include "main.h"

//Display Commands
#define DISP_COM_CASET 0x2A
#define DISP_COM_RASET 0x2B
#define DISP_COM_RAMWR 0x2C
#define DISP_COM_DISPON 0x29
#define DISP_COM_SLPOUT 0x11
#define DISP_COM_SWRESET 0x1
#define DISP_COM_MADCTL 0x36


//Colors
#define DISP_BLACK 0x0
#define DISP_BLUE 0xFC
#define DISP_GREEN 0xFC00
#define DISP_RED 0xFC0000
#define DISP_WHITE 0xFCFCFC
#define DISP_PURPLE 0xFC00FC
#define DISP_YELLOW 0xFCFC00
#define DISP_AQUA 0x00FCFC
#define DISP_L_BROWN 0xCD853F


//Offsets
#define DISP_ROW_OFFSET 1
#define DISP_COL_OFFSET 2


typedef struct Text_Info Text_Info;
//Text Info Struct:
//writeText(uint16_t x, uint16_t y, char *msg, uint8_t nChrs, uint8_t tChr, uint32_t colorRGB, void *bGndFc(uint16_t, uint16_t))

struct Text_Info{
  uint16_t x;
  uint16_t y;
  uint8_t *msg;
  uint8_t numChrs;
  uint8_t termChr;
  uint32_t colorRGB;
  uint32_t (*BGndFnc)(uint16_t, uint16_t);
  uint8_t cWidth;
  uint8_t cHeight;  
  uint32_t *colorArr;
  uint8_t *fontArr;
  uint8_t fRows;
  uint8_t fCols;
};


void setColumnRowRange(uint8_t columnStart, uint8_t columnEnd, uint8_t rowStart, uint8_t rowEnd);

void setColor(uint32_t colorRGB, uint16_t pixelCount);

void initDisplay();

void writeDisplayCommand(uint8_t displayCommand);

void colorRGB32To8(uint32_t colorRGB, uint8_t *rgbArr);

void writeRectangle(uint8_t leftX, uint8_t leftY, uint8_t width, uint8_t height, uint32_t colorRGB);

void writePixel(uint8_t xCoor, uint8_t yCoor, uint32_t colorRGB);

void drawLine(int16_t xCoor1, int16_t yCoor1, int16_t xCoor2, int16_t yCoor2, uint32_t colorRGB );



void writeColorArray(uint32_t *colorArr, uint16_t size);
void writeText(Text_Info *tInfo);

#endif
