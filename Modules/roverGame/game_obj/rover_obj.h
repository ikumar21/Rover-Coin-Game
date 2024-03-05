#ifndef __ROVER_OBJ_H
#define __ROVER_OBJ_H

#include "generic_game.h"

//typedef struct Game_Obj Game_Obj;
//typedef struct Obj_Disp Obj_Disp;

struct Rover_Obj{
    float pA_R[2];//Additional vector to be added in Rover frame
    int16_t pT_R[2];//Total vector that's been added in Rover frame
    float pA_G[2];//Additional vector to be added in global frame
    int16_t pT_G[2];//(Total) Vector position in global frame
    
    
};

//Colors:
extern uint32_t roverColors[7];
extern uint8_t roverStraightColorType[14][14];
extern uint8_t roverAngledColorType[14][14];
extern uint8_t *roverColorTypes[2];

extern void RoverLimitLoc(int16_t *x, int16_t *y);
extern void RoverController(Game_Obj *gObj, int16_t joyX, int16_t joyY);
#endif