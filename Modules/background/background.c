#include "background.h"

uint8_t thickness = 4;
uint32_t checkerColors[2]= {DISP_WHITE, 0xFC3C7C};

uint32_t checkerBoard(uint16_t x, uint16_t y){
  uint8_t c = (y/thickness)%2==0;
  uint8_t r = (x/thickness)%2==0;
  uint8_t index = c^r;
  return checkerColors[index];
  
}