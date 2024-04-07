#include "rover_game.h"

#define MAX_NUM_OBJ 15
#define COIN_NUM_COLOR 12
#define COIN_INTV 5

#define ROVER_STRT_X 50
#define ROVER_STRT_Y 50

//Score and Time
uint16_t roverGameScore = 0;
int16_t roverTime = 0;

//All Objects:
uint32_t objPixelSpace[200];
Game_Obj gameObjects[MAX_NUM_OBJ];
Obj_Disp dispObjects[MAX_NUM_OBJ];
Rover_Obj rov1Obj;
Coin_Obj coinsObj[5];

//Joystick:
int16_t joystickVal[2] = {0,0};
extern void runJoystick(int16_t *joyVal);

//Colors
uint8_t **allObjColorTypes[2]={roverColorTypes,coinColorTypes};
uint32_t *allObjColors[2]  = {roverColors, coinColors};

//Width in pixels of each object-> {RoverStraight, roverAngled, spiderStraight, spiderAngled,.. }
uint16_t objPxWidth[14] = {14,14,5,5,5,5,5,5,5,5,5,5,3,3};
uint16_t objPxHeight[14] = {14,14,5,5,5,5,5,5,5,5,5,5,3,3};

//Font Text:
extern uint8_t font8x8_basic[128][8];
uint8_t msgDisp[20]="Hello";
Text_Info font8by8 = {0,0,msgDisp,0,'\0',DISP_BLACK, BackgroundColorPixel, 7,7,objPixelSpace, (uint8_t *)font8x8_basic, 128, 8};


//Rover Game State
enum Game_Status curState = TITLE_SCREEN;
enum Game_Status prevState = FINISHED; //Should be different at start to signal state change
uint8_t gameMode = 0; //0->easy, 1->medium, 2->hard

//Init States:
void InitTitle();
void InitMode();
void InitRunning();
void InitFinished();


void setBackground(){
  //Set the background:
  for(uint16_t col = 0; col<128;col++){
    for(uint16_t row = 0; row<160;row++){
      objPixelSpace[row]=BackgroundColorPixel(col,row);
    }
      setColumnRowRange(col, col, 0,  159);
      writeColorArray(objPixelSpace, 160);
  }
}

void InitializeObjDisp(uint8_t numObjs){  
  writeRectangle(0, 8, 128,1, 0x0); //Display a bar underneath score and time
  roverTime = -1; //-1 notes start of game
  //Init Rover:
  dispObjects[0].angle=0;
  

  dispObjects[0].xLoc=ROVER_STRT_X;
  dispObjects[0].yLoc=ROVER_STRT_Y;
  dispObjects[0].objType=ROVER;
  
  gameObjects[0].dObj=&dispObjects[0];
  gameObjects[0].objType=ROVER;
  gameObjects[0].active=true;
  gameObjects[0].rovObj=&rov1Obj;
  gameObjects[0].rovObj->pT_G[0]=ROVER_STRT_X*100;
  gameObjects[0].rovObj->pT_G[1]=ROVER_STRT_Y*100;
  
  //Init Coins
  for(uint8_t i = 1; i<6;i++){
//    dispObjects[i].xLoc=i*20;
//    dispObjects[i].yLoc=i*25+10;
    gameObjects[i].dObj=&dispObjects[i];
    gameObjects[i].objType=COIN;
    dispObjects[i].objType=COIN;
    gameObjects[i].coinObj=&coinsObj[i-1];
//    gameObjects[i].active=true;
//    gameObjects[i].coinObj->colorChangeCounter=0;
//    gameObjects[i].coinObj->currentColor=0;
    
  }
  
    //Initialize object data based on object type and angle
    for(uint8_t i = 0; i<=numObjs;i++){
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

void InitializeStateChange(){
  //Initialize state only if state change
  if(prevState==curState){
    return;
  }
  prevState = curState;
  setBackground();
  switch(curState){
    case TITLE_SCREEN:
      InitTitle();
      break;
    case SELECT_MODE:
      InitMode();
      break;
    case RUNNING_GAME:
      InitRunning();
      break;
    case FINISHED:
      InitFinished();
      break;
  }
}

void RunRoverGame(){
  //Initialize State if state change
  InitializeStateChange();
  switch(curState){
    case TITLE_SCREEN:
      break;
    case SELECT_MODE:
      break;
    case RUNNING_GAME:
      RunGamePlay();
      break;
    case FINISHED:
      break;
  }
  

}

void RunGamePlay(){
  DisplayScore();
  DisplayTime();
  runJoystick(joystickVal);
  
  for(uint8_t i = 0; i<MAX_NUM_OBJ; i++){
    Game_Obj *gObj = &gameObjects[i];
//    //First see if object is active:
//    if(gObj->active)
      ObjectPositionCalc(gObj);//Calculate new position of object
  }
  //See if any collisions 
  for(uint8_t i = 0; i<MAX_NUM_OBJ; i++){
    Game_Obj *gObj = &gameObjects[i];
    if(gObj->objType==COIN && gObj->active==true){
      //If rover hits coin, increase score and make coin unactive
      if(collisionThere(&gameObjects[i], &gameObjects[0])){
         roverGameScore++;
         deactivateCoin(gObj);
      }
    }
  }
  //Display objects in new place
  for(uint8_t i = 0; i<MAX_NUM_OBJ; i++){
    Game_Obj *gObj = &gameObjects[i];
    if(gObj->active)
      DisplayObjectLoc(gObj); 
  }
  //Fill in new background gap caused by object moving
  FillNewBackgroundObj();
  //Refresh object details from movement
  for(uint8_t i = 0; i<MAX_NUM_OBJ; i++){
    Game_Obj *gObj = &gameObjects[i];
    if(gObj->active)
      RefreshObjectDetails(gObj);
  }

}


void ObjectPositionCalc(Game_Obj *gObj){
  Obj_Disp *dObj = gObj->dObj;
  if(gObj->objType==ROVER){
    RoverController(gObj, joystickVal[0],joystickVal[1]);
  }else if (gObj->objType==COIN){
    CoinController(gObj);
  }else{
    INCRE_CIRC_COUNTER(dObj->yLoc,160-11);
    INCRE_CIRC_COUNTER(dObj->xLoc,128-11);
  }

}

void DisplayObjectLoc(Game_Obj *gObj){
  Obj_Disp* dObj = gObj->dObj; 
  
  if(gObj->objType==COIN){ //Do Rainbow Color
      Coin_Obj *cObj = gObj->coinObj;
      //Change color every coinInterval*10 ms 
      INCRE_CIRC_COUNTER(cObj->colorChangeCounter,COIN_INTV);
      if(cObj->colorChangeCounter==0){
        INCRE_CIRC_COUNTER(cObj->currentColor,COIN_NUM_COLOR);
      }
      setCoinColor(dObj->objColorTypes[0] , dObj->objColorTypes[1], cObj->currentColor);
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


void DisplayScore(){
  static uint16_t prevScore = 999;
  if(prevScore==roverGameScore)
    return;
  //Only display score if it changed
  prevScore=roverGameScore;
  font8by8.x=64; font8by8.y=1;
  font8by8.numChrs=9; //Num of characters of string
  msgDisp[0]='S';msgDisp[1]='c';msgDisp[2]='o';msgDisp[3]='r'; msgDisp[4]='e';msgDisp[5]=':';
  msgDisp[6]= roverGameScore/100+48;
  msgDisp[7]= (roverGameScore/10)%10+48;
  msgDisp[8]= (roverGameScore)%10+48;
  writeText(&font8by8);

}

void DisplayTime(){
  static uint8_t decrementTime = 0;
  uint8_t displayTime=false;
  if(roverTime==-1){ //Start of Game
    decrementTime=0;
    roverTime=60;
    displayTime=true;
  }
  //Once it reaches 99->1000 ms has passed
  if(decrementTime==99){
    roverTime--;
    displayTime=true;
  }
  //Increment Counter
  decrementTime=decrementTime>=99? 0: decrementTime+1; 
  if(!displayTime){
    return;
  }
  //Display Time:
  font8by8.x=1; font8by8.y=1;
  font8by8.numChrs=7; //Num of characters of string
  msgDisp[0]='T';msgDisp[1]='i';msgDisp[2]='m';msgDisp[3]='e'; msgDisp[4]=':';
  msgDisp[5]=(roverTime/10)%10+48;
  msgDisp[6]=(roverTime)%10+48;
  writeText(&font8by8);
}
  
void InitTitle(){
  const uint8_t *titleName = "ROVER COIN DASH";
  const uint8_t *byStr = "BY";
  const uint8_t *name = "ISHAN KUMAR";
  //Put Rover title
  font8by8.x=12; font8by8.y=50; font8by8.numChrs=15;
  font8by8.msg= (uint8_t*)titleName;
  writeText(&font8by8);
  
  //Put BY
  font8by8.x=57; font8by8.y=85; font8by8.numChrs=2;
  font8by8.msg= (uint8_t*)byStr;
  writeText(&font8by8);
  
  //Put my own name
  font8by8.x=25; font8by8.y=100; font8by8.numChrs=11;
  font8by8.msg= (uint8_t*)name;
  writeText(&font8by8);
}

void InitMode(){
}

void InitRunning(){
  InitializeObjDisp(6);
}

void InitFinished(){
}