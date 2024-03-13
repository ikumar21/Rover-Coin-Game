#ifndef __COIN_OBJ_H
#define __COIN_OBJ_H
#include "generic_game.h"

//typedef struct Game_Obj Game_Obj;
//typedef struct Obj_Disp Obj_Disp;

struct Coin_Obj{
    uint8_t i;
    uint8_t colorChangeCounter;
    uint8_t currentColor;
  
};

//Colors:
extern uint32_t coinColors[13];
extern uint8_t coin1[5][5];
extern uint8_t *coinColorTypes[2];

extern void setCoinColor(uint8_t arr[], uint8_t orArr[], uint8_t color);
extern void CoinController(Game_Obj *gObj);
extern void deactivateCoin(Game_Obj *gObj);

#endif