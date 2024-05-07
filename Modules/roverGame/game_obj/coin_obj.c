#include "coin_obj.h"
#include "rover_math.h"
#include "rover_game.h"
//All objects:
uint8_t numActiveCoins = 0;
uint32_t coinColors[13] = {0xF8F8F8,0xFC0000,0xFC7A00,0xFCFC00,0x7AFC00, 
                            0x00FC00,0x00FC7A,0x00FCFC,0x007AFC,
                            0x0000FC,0x7A00FC,0xFC00FC,0xFC007A};
//Level properties from rover_game.c
extern uint16_t coinGenTime[2];
extern uint8_t maxCoins;


//How long to wait to generate coin
uint16_t timeToGen = 0; 
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

void CoinController(Game_Obj gObjAll[]){
  
  //Don't do anything if coin doesn't need to be generated
  if(timeToGen!=0){
    timeToGen--;
    return;
  }
  
  //Don't generate coins if max coins already
  if(numActiveCoins==maxCoins){
    return;
  }
  //Determine time to generate next coin, divide by 10 (func called very 10 ms)
  timeToGen = getRandNum(coinGenTime[0], coinGenTime[1])/10;
  
  //Find non-active coin to make active
  Game_Obj *newCoin = NULL;
  for(uint8_t i = 0; i< MAX_NUM_OBJ;i++){
    if(gObjAll[i].objType==COIN && gObjAll[i].active==false){
      newCoin = &gObjAll[i];
      break;
    }
  }
  //Then randomly place it in new position

  newCoin->active=true;
  numActiveCoins++;
  //Ensure the coin isn't placed where rover or another coin is
  uint8_t noIntersection = false;
  while(!noIntersection){
    noIntersection=true;
    newCoin->dObj->xLoc=getRandNum(5, 105);
    newCoin->dObj->yLoc = getRandNum(10,135);
    for(uint8_t i = 0; i<MAX_NUM_OBJ;i++){
      //See if object is active and don't compare against itself
      if(gObjAll[i].active==true && newCoin!=&gObjAll[i]){
        noIntersection = noIntersection  && !collisionThere(&gObjAll[i], newCoin);
      }
    }
  }
  
  newCoin->dObj->prevXLoc=newCoin->dObj->xLoc;
  newCoin->dObj->prevYLoc=newCoin->dObj->yLoc;
  newCoin->coinObj->currentColor=getRandNum(1,12);
  newCoin->coinObj->colorChangeCounter=0;
  
}

void deactivateCoin(Game_Obj *gObj){
  gObj->active=false;
  Obj_Disp *dObj = gObj->dObj;
  numActiveCoins--;
  //Fill in coin with background:
  BackgroundFillBox(dObj->xLoc,dObj->xLoc+dObj->width-1, dObj->yLoc, dObj->yLoc+dObj->height-1, 1);
  
}


