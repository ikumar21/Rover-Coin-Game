#include "rover_obj.h"
#include "rover_math.h"

//0xF8F8F8 is background color
uint32_t roverColors[7] = {0xF8F8F8,0x040404,0x9C281C,0x00CC00,0x1C2828,0xFCFC00,0xA80000};

uint8_t roverStraightColorType[14][14] = {
{0,0,0,0,0,0,0,0,0,0,0,0,0,0},
{0,0,1,1,1,0,0,0,1,1,1,0,0,0},
{0,0,1,1,1,0,0,0,1,1,1,0,0,0},
{0,6,6,6,6,6,6,6,6,6,6,6,0,0},
{0,6,6,6,6,6,6,6,6,6,6,5,5,0},
{0,6,6,2,2,2,2,6,6,6,6,5,5,0},
{0,6,6,2,2,3,3,3,3,6,6,6,6,0},
{0,6,6,2,2,3,3,3,3,6,6,6,6,0},
{0,6,6,2,2,2,2,6,6,6,6,5,5,0},
{0,6,6,6,6,6,6,6,6,6,6,5,5,0},
{0,6,6,6,6,6,6,6,6,6,6,6,0,0},
{0,0,1,1,1,0,0,0,1,1,1,0,0,0},
{0,0,1,1,1,0,0,0,1,1,1,0,0,0},
{0,0,0,0,0,0,0,0,0,0,0,0,0,0}};

uint8_t roverAngledColorType[14][14] = {
{0,0,0,0,0,0,1,1,6,5,5,0,0,0},
{0,0,0,0,0,1,1,1,6,5,5,0,0,0},
{0,0,0,0,0,1,1,6,6,6,6,0,0,0},
{0,0,0,0,0,6,6,6,6,6,6,6,5,5},
{0,0,0,0,6,6,6,6,3,6,6,6,5,5},
{0,1,1,6,6,6,6,3,3,3,6,6,6,6},
{1,1,1,6,6,2,3,3,3,6,6,6,1,1},
{1,1,6,6,6,2,3,3,6,6,6,1,1,1},
{6,6,6,6,6,2,2,2,6,6,6,1,1,0},
{0,6,6,6,6,6,6,6,6,6,0,0,0,0},
{0,0,6,6,6,6,6,6,6,6,0,0,0,0},
{0,0,0,6,6,6,6,1,1,0,0,0,0,0},
{0,0,0,0,6,6,1,1,1,0,0,0,0,0},
{0,0,0,0,0,6,1,1,0,0,0,0,0,0}};

uint8_t *roverColorTypes[2] = {(uint8_t *)roverStraightColorType, (uint8_t *)roverAngledColorType};

float roverJoyGain[2][2] = {{.002,0},{0,.03}};
//float roverJoyGain[2][2] = {{.002,0},{0,.04}};
void RoverLimitLoc(int16_t *x, int16_t *y){
  if(*x<0)
    *x=0;
  if(*x>11500)
    *x=11500;
  if(*y<0)
    *y=0;
  if(*y>14700)
    *y=14700;
}

void RoverController(Game_Obj *gObj, int16_t joyX, int16_t joyY){
  Obj_Disp *dObj = gObj->dObj;
  Rover_Obj *rObj = gObj->rovObj;
  
  float arr[2] = {joyX,joyY};
  
  //How much to move relative to rover frame due to joystick position
  MultMatrix(rObj->pA_R,roverJoyGain,arr);
//  rObj->pA_R[1]+=ABS_M(arr[0])*0.01; //Move forward even making a turn
  
  
  //Keep a running sum of relative movement in order to switch angles when perpendicular sum is large
  rObj->pT_R[1]+=rObj->pA_R[1];
  rObj->pT_R[0]+=rObj->pA_R[0];
  
  //Rotation matrix to change from rover to global frame
  uint16_t a = dObj->angle;
  float rotMat[2][2] = {{SinSim(a), CosSim(a)}, {CosSim(a), -SinSim(a)}};
  
  //Calulate how much to change positon in global frame, then calculate new global position
  MultMatrix(rObj->pA_G,rotMat,rObj->pA_R);
  rObj->pT_G[0]+=rObj->pA_G[0];
  rObj->pT_G[1]+=rObj->pA_G[1];
  
  
  //Limit where rover can be
  RoverLimitLoc(rObj->pT_G, & (rObj->pT_G[1]));
  
  dObj->xLoc=rObj->pT_G[0]/100;
  dObj->yLoc=rObj->pT_G[1]/100;

  //Change angle if movement in perpendicular direction (rover frame) enough, then reset values
  if(rObj->pT_R[0]>100 || rObj->pT_R[0]<-100){
    dObj->angle=dObj->prevAngle+(rObj->pT_R[0]<100)*(90)-45; //Add 45 if negative x, -45 if pos x dir.
    
    //Adjust angle to get to 0-359 range
    if(dObj->angle ==-45)
      dObj->angle=315;
    if(dObj->angle ==360)
      dObj->angle=0;
    
    rObj->pT_R[0]=0;
    rObj->pT_R[1]=0;
  }
}