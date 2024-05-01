#ifndef __ROVER_GAME_H
#define __ROVER_GAME_H

#include "generic_game.h"
#include "coin_obj.h"
#include "rover_obj.h"

#define MAX_NUM_OBJ 60
//uint32_t objPixelSpace[200];
//Game_Obj gameObjects[15];
enum Game_Status{
  TITLE_SCREEN,
  SELECT_LEVEL,
  RUNNING_GAME,
  FINISHED,

};

//Joystick
extern int16_t joystickVal[2];

//Generic Game
extern void BackgroundFillBox(uint16_t xS, uint16_t xE, uint16_t yS, uint16_t yE, uint8_t objIndex);
extern void FillNewBackgroundObj();
extern void SetObjectColor(uint32_t *destColorArr, Obj_Disp *obj);

//Coin 
extern void deactivateCoin(Game_Obj *gObj);
extern uint16_t timeToGen;

void InitializeObjDisp(uint8_t numObjs);

void RunGamePlay();
void ObjectPositionCalc(Game_Obj *gObj);//Calculate new position of object
void DisplayObjectLoc(Game_Obj *gObj); //Display object in new place
void RefreshObjectDetails(Game_Obj *gObj); //Refresh object details from movement
void initRoverGame();
void DisplayScore();
void DisplayTime(int16_t *curTime, uint16_t maxTime, uint8_t x, uint8_t y);

#endif
