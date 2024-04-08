#include "coin_obj.h"
#include "rover_math.h"
#include "rover_game.h"
//All objects:
extern Game_Obj gameObjects[MAX_NUM_OBJ];
uint8_t numActiveCoins = 0;
uint32_t coinColors[13] = {0xF8F8F8,0xFC0000,0xFC7A00,0xFCFC00,0x7AFC00, 
                            0x00FC00,0x00FC7A,0x00FCFC,0x007AFC,
                            0x0000FC,0x7A00FC,0xFC00FC,0xFC007A};

uint8_t coin1[5][5] = {
{8,8,7,6,6},
{8,7,7,6,5},
{1,1,0,5,5},
{1,2,3,4,4},
{2,2,3,3,4}};


uint8_t orgCoin1[5][5] = {
{8,8,7,6,6},
{8,7,7,6,5},
{1,1,0,5,5},
{1,2,3,4,4},
{2,2,3,3,4}};

uint8_t *coinColorTypes[2]={(uint8_t *) coin1, (uint8_t *) orgCoin1};

void setCoinColor(uint8_t arr[], uint8_t orArr[], uint8_t color){
  for(uint8_t i = 0; i<25;i++){
    if(arr[i] !=0){
      arr[i]=(orArr[i]+color-1)%12+1;
    }
  }
}

void CoinController(Game_Obj *gObj){
  //Don't do anything if active already
  if(gObj->active==true)
    return;
  //If not active, generate coin randomly (less coins->more likely new coin appears)
  uint8_t makeActive = getRandNum(0,1000)<(MAX_NUM_COINS+1-numActiveCoins);
  //Then randomly place it in new position
  if(makeActive){
    gObj->active=true;
    numActiveCoins++;
    //Ensure the coin isn't placed where rover or another coin is
    uint8_t noIntersection = false;
    while(!noIntersection){
      noIntersection=true;
      gObj->dObj->xLoc=getRandNum(5, 105);
      gObj->dObj->yLoc = getRandNum(10,135);
      for(uint8_t i = 0; i<MAX_NUM_OBJ;i++){
        //See if object is active and don't compare against itself
        if(gameObjects[i].active==true && gObj!=&gameObjects[i]){
          noIntersection = noIntersection  && !collisionThere(&gameObjects[i], gObj);
        }
      }
    }
    
    gObj->dObj->prevXLoc=gObj->dObj->xLoc;
    gObj->dObj->prevYLoc=gObj->dObj->yLoc;
    gObj->coinObj->currentColor=getRandNum(1,12);
    gObj->coinObj->colorChangeCounter=0;
  } 
}

void deactivateCoin(Game_Obj *gObj){
  gObj->active=false;
  Obj_Disp *dObj = gObj->dObj;
  numActiveCoins--;
  //Fill in coin with background:
  BackgroundFillBox(dObj->xLoc,dObj->xLoc+dObj->width-1, dObj->yLoc, dObj->yLoc+dObj->height-1, 1);
  
}


