#include "rover_game.h"

#define MAX_NUM_OBJ 15
#define COIN_NUM_COLOR 12

#define MAX(a,b) a>b? a:b
#define ABS_M(a) a<0? -a:a

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


void SetObjectColor(uint32_t *destColorArr, Obj_Disp *obj){
  //Function that sets the color array according to object and angle it is
  
  //First correct angle, width, height, and then choose right color array
  
  // correct angle (default to 90 if angle not (0-315); multiple of 45))
  if(obj->angle>315 || obj->angle %45 != 0)
    obj->angle = 90;
  //Fix height/width (pixels) of object
  uint8_t objIndex = ((obj->angle%90)!=0)+2*obj->objType;
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
  
  //Color depends on what angle object is
  uint8_t angleNotMul90 = (obj->angle%90)!=0;
  uint8_t *objColorType = obj->objColorTypes[angleNotMul90]; 
  
  //Apply transformations to rotate color matrice according to object's angle
  uint8_t dir = (obj->angle/45)/2;//0->0/45, 1->90,135 ...

  uint16_t width = obj->orWidth;
  uint16_t height = obj->orHeight;

  uint16_t k = 0; //Pixel num
  uint16_t start1 = (width-1)*(dir==1)+(height-1)*(dir==2); //Start of outer
  uint16_t start2 = (height-1)*(dir==3)+(width-1)*(dir==2); //start of inner
  int16_t end1 = width*(dir==3)+height*(dir==0)-1*(dir==1 || dir==2); //End of outer
  int16_t end2 = width*(dir==0)+height*(dir==1)-1*(dir==3 || dir==2);//End of inner
  int8_t change1 = 1-2*(dir==1 || dir==2); //Outer loop Either decrementing or incrementing
  int8_t change2 = 1-2*(dir>1);//Inner loop Either decrementing or incrementing
  uint16_t row = 0;
  uint16_t col = 0;
  for(int16_t i = start1;i!=end1;i+=change1){
    for(int16_t j = start2;j!=end2;j+=change2){
        row = i*(dir%2==0)+j*(dir%2);//See if inner or outer loop is row variable 
        col = i*(dir%2)+j*(dir%2==0);//See if inner or outer loop is col variable
        destColorArr[k]=objColorType[row*width+col];
//        printf("%d,%d,\n",i,j);
        k++; //Increase 
    }
  }
  
  //Now that the color type of each pixel is set. Replace color type with corresponding 18 bit color.
  for (uint16_t row = 0; row<obj->height;row++){
    for(uint16_t col = 0; col<obj->width;col++){
      uint16_t i = row*obj->width+col;
      if(destColorArr[i]==0){
        destColorArr[i]=BackgroundColorPixel(obj->xLoc+col,obj->yLoc+row, 0);
      }else{
        destColorArr[i]=obj->objColors[destColorArr[i]];
      }
    }
  
  }
//  for (uint8_t i=0;i<width*height;i++){
//    if(destColorArr[i]==0){
//      destColorArr[i]=BackgroundColorPixel(obj->xLoc,
//    }else{
//      
//      destColorArr[i]=obj->objColors[destColorArr[i]];
//    }
//  }

}

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

  
