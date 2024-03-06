#include "rover_game.h"

#define MAX_NUM_OBJ 15
#define COIN_NUM_COLOR 12


uint32_t objPixelSpace[200];

//All Objects:
Game_Obj gameObjects[MAX_NUM_OBJ];
Obj_Disp dispObjects[MAX_NUM_OBJ];
Rover_Obj rov1Obj;
Coin_Obj coinsObj[4];
//Joystick:
int16_t joystickVal[2] = {0,0};

uint8_t coininterval=5;

uint8_t **allObjColorTypes[2]={roverColorTypes,coinColorTypes};
uint32_t *allObjColors[2]  = {roverColors, coinColors};

//Width in pixels of each object-> {RoverStraight, roverAngled, spiderStraight, spiderAngled,.. }
uint16_t objPxWidth[14] = {14,14,5,5,5,5,5,5,5,5,5,5,3,3};
uint16_t objPxHeight[14] = {14,14,5,5,5,5,5,5,5,5,5,5,3,3};

//Testing:
uint16_t roverAngle;
uint8_t leftTopCorner[2] = {50,50};



void InitializeObjDisp(uint8_t numObjs){
  //Init Rover:
  dispObjects[0].angle=roverAngle;

  dispObjects[0].xLoc=leftTopCorner[0];
  dispObjects[0].yLoc=leftTopCorner[1];
  dispObjects[0].objType=ROVER;
  
  gameObjects[0].dObj=&dispObjects[0];
  gameObjects[0].objType=ROVER;
  gameObjects[0].active=true;
  gameObjects[0].rovObj=&rov1Obj;
  gameObjects[0].rovObj->pT_G[0]=leftTopCorner[0]*100;
  gameObjects[0].rovObj->pT_G[1]=leftTopCorner[1]*100;
  
  //Init Coins
  for(uint8_t i = 1; i<5;i++){
    dispObjects[i].xLoc=i*20;
    dispObjects[i].yLoc=i*25+10;
    gameObjects[i].dObj=&dispObjects[i];
    gameObjects[i].objType=COIN;
    dispObjects[i].objType=COIN;
    gameObjects[i].coinObj=&coinsObj[i-1];
    gameObjects[i].active=true;
    gameObjects[i].coinObj->colorChangeCounter=0;
    gameObjects[i].coinObj->currentColor=0;
    
  }
  
    //Initialize object data based on object type and angle
    for(uint8_t i = 0; i<numObjs;i++){
        Game_Obj *gObj = &gameObjects[i];
        Obj_Disp *obj = gObj->dObj;

        //First correct angle (default to 0 if angle not (0-315); multiple of 45))
        if(obj->angle>315 || obj->angle %45 != 0)
            obj->angle = 0;
        
//        printf("%d\n",obj->angle);

        //Fix prev loc/angle 
        obj->prevXLoc=obj->xLoc;
        obj->prevYLoc=obj->yLoc;
        obj->prevAngle= obj->angle;

        //Fix height/width (pixels) of object
        uint8_t objIndex = ((obj->angle%90)!=0)+gObj->objType*2;
        obj->orWidth = objPxWidth[objIndex];
        obj->orHeight = objPxHeight[objIndex];

        //If angles are 90,270,135, or 315-> height and width are flipped
        if(obj->angle==90 || obj->angle==135 || obj->angle==270 || obj->angle==315){
            obj->width = obj->orHeight;
            obj->height = obj->orWidth;
        }else{
            obj->width = obj->orWidth;
            obj->height = obj->orHeight;
        }

        obj->prevWidth = obj->width;
        obj->prevHeight = obj->height;

        //Set colors the object contains
        obj->objColors=allObjColors[gObj->objType];//Doesn't change
        obj->objColorTypes=allObjColorTypes[obj->objType];//

        //Fix Health 
//        obj->health = 100;

    }


}

void UpdateObjectPosition(){
  for(uint8_t i = 0; i<MAX_NUM_OBJ; i++){
    Game_Obj *gObj = &gameObjects[i];
    //First see if object is active:
    if(gObj->active)
      ObjectPositionCalc(gObj);//Calculate new position of object
  }
  for(uint8_t i = 0; i<MAX_NUM_OBJ; i++){
    Game_Obj *gObj = &gameObjects[i];
    if(gObj->active)
      DisplayObjectLoc(gObj); //Display object in new place
  }
  for(uint8_t i = 0; i<MAX_NUM_OBJ; i++){
    Game_Obj *gObj = &gameObjects[i];
    if(gObj->active)
      FillNewBackgroundObj();//Fill new background gap caused by object moving
  }
  for(uint8_t i = 0; i<MAX_NUM_OBJ; i++){
    Game_Obj *gObj = &gameObjects[i];
    if(gObj->active)
      RefreshObjectDetails(gObj); //Refresh object details from movement
  }

}


void ObjectPositionCalc(Game_Obj *gObj){
  Obj_Disp *dObj = gObj->dObj;
  if(gObj->objType==ROVER){
    RoverController(gObj, joystickVal[0],joystickVal[1]);
  }else if (gObj->objType==COIN){
  }else{
    INCRE_CIRC_COUNTER(dObj->yLoc,160-11);
    INCRE_CIRC_COUNTER(dObj->xLoc,128-11);
  }

}

void DisplayObjectLoc(Game_Obj *gObj){
  Obj_Disp* dObj = gObj->dObj; 
  
  if(gObj->objType==COIN){ //Do Rainbow Color
      Coin_Obj *cObj = gObj->coinObj;
      INCRE_CIRC_COUNTER(cObj->colorChangeCounter,coininterval);
      
      if(cObj->colorChangeCounter==0){
        INCRE_CIRC_COUNTER(cObj->currentColor,COIN_NUM_COLOR);
        setCoinColor(dObj->objColorTypes[0] , dObj->objColorTypes[1], cObj->currentColor);
      
      }
  }

  SetObjectColor(objPixelSpace,dObj);
  uint8_t x = dObj->xLoc;
  uint8_t y = dObj->yLoc;
  setColumnRowRange(x,x+dObj->width-1,y ,y+dObj->height-1);
  writeColorArray(objPixelSpace,dObj->width*dObj->height);
  
}

void RefreshObjectDetails(Game_Obj *gObj){
  Obj_Disp* dObj = gObj->dObj;
  dObj->prevXLoc=dObj->xLoc;
  dObj->prevYLoc=dObj->yLoc;
  dObj->prevAngle=dObj->angle;
  
}

void initRoverGame(){
  //Set the background:
  for(uint16_t col = 0; col<128;col++){
    for(uint16_t row = 0; row<160;row++){
      objPixelSpace[row]=checkerBoard(row,col);
    }
      setColumnRowRange(col, col, 0,  159);
      writeColorArray(objPixelSpace, 160);
  }
}

  
