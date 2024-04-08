#ifndef __GENERIC_GAME_H
#define __GENERIC_GAME_H
#include "background.h"
#include "displayST7735.h"

#define MAX(a,b) a>b? a :b
#define MIN(a,b) a>b? b :a

typedef struct Rover_Obj Rover_Obj;
typedef struct Coin_Obj Coin_Obj;
typedef struct Obj_Disp Obj_Disp;
typedef struct Game_Obj Game_Obj;

typedef enum  {
    ROVER=0, 
    COIN
}ObjectDispType;

struct Obj_Disp{
    uint16_t xLoc; //Upper Left Corner
    uint16_t yLoc; //Upper Left Corner
    uint16_t orWidth; //Original width (0 Degrees)
    uint16_t orHeight; //Original height (0 Degrees)
    uint16_t height; //Current height
    uint16_t width;
    int16_t angle; //default to 0 if angle isn't revelent for object

    uint16_t prevXLoc;
    uint16_t prevYLoc;
    uint16_t prevHeight;
    uint16_t prevWidth;
    uint16_t prevAngle;

    ObjectDispType objType;
    uint32_t *objColors;
    uint8_t **objColorTypes;
};

struct Game_Obj{
    Obj_Disp *dObj;
    ObjectDispType objType;
    Rover_Obj *rovObj;
    Coin_Obj *coinObj;
    uint8_t active;
    
};
void BackgroundFillBox(uint16_t xS, uint16_t xE, uint16_t yS, uint16_t yE, uint8_t objIndex);
void FillNewBackgroundObj();
void SetObjectColor(uint32_t *destColorArr, Obj_Disp *obj);
uint8_t collisionThere(Game_Obj *gObj1, Game_Obj *gObj2);

#endif