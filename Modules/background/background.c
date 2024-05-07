#include "background.h"
#include "rover_game.h"
#define MAX_DISP_Y 160
uint8_t thickness = 4;
uint32_t checkerColors[2]= {DISP_WHITE, 0xFC3C7C};
uint32_t gradColors[2] = {DISP_BLUE, DISP_RED};


uint32_t checkerBoard(uint16_t x, uint16_t y, uint8_t width);
uint32_t gradientVertical(uint16_t x, uint16_t y);
uint32_t runningBackGround(uint16_t x, uint16_t y); 
extern enum Game_Status curState;

uint32_t BackgroundColorPixel(uint16_t x,uint16_t y){ 
  switch(curState){
    case TITLE_SCREEN:
      return gradientVertical(x,y);
      break;
    case SELECT_LEVEL:
      return DISP_WHITE;
      break;
    case RUNNING_GAME:
      return runningBackGround(x, y);
      break;
    case FINISHED:
      if(y<50 || y>110)
        return checkerBoard(x,y-9, thickness);
      return DISP_WHITE;
      break;
  }
  return 0;
}

uint32_t runningBackGround(uint16_t x, uint16_t y){
  if(y<9){ //Where current time and current score goes
    return 0x7CBCFC;
  }
  
  if(y>=152 && (x>=27 && x<99) ){ //Where Target score goes
    return 0x7CBCFC;
  }
  
  return checkerBoard(x, y, thickness);
  
  

}

uint32_t checkerBoard(uint16_t x, uint16_t y, uint8_t width){
  uint8_t c = (y/width)%2==0;
  uint8_t r = (x/width)%2==0;
  uint8_t index = c^r;
  return checkerColors[index];
  
}
uint32_t gradientVertical(uint16_t x, uint16_t y){
  uint8_t rValue = (0xFF & gradColors[0]>>16) + (y-160)*((0xFF & gradColors[0]>>16)-(0xFF & gradColors[1]>>16))/160;
  uint8_t gValue = (0xFF & gradColors[0]>>8) + (y-160)*((0xFF & gradColors[0]>>8)-(0xFF & gradColors[1]>>8))/160;
  uint8_t bValue = (0xFF & gradColors[0]) - (160-y)*((0xFF & gradColors[0])-(0xFF & gradColors[1]))/160;
  return (0xFC &rValue) <<16 | (0xFC &gValue)<<8 | (0xFC &bValue);
  
}

