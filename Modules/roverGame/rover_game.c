#include "rover_game.h"
#include "main.h"

#define MAX_NUM_OBJ 15

//COIN
#define COIN_NUM_COLOR 12
#define COIN_INTV 5
//Rover starting Location
#define ROVER_STRT_X 50
#define ROVER_STRT_Y 50
//Location of start
#define X_LOC_START 85
#define Y_LOC_START 127
//Location of Finish Box
#define X_LOC_FINISH 14
#define Y_LOC_FINISH 55

//Score and Time
uint16_t prevScore = 999;
uint16_t roverGameScore = 0;
int16_t roverTime = 0;
uint16_t highestScore = 0;

//All Objects:
uint32_t objPixelSpace[200];
Game_Obj gameObjects[MAX_NUM_OBJ];
Obj_Disp dispObjects[MAX_NUM_OBJ];
Rover_Obj rov1Obj;
Coin_Obj coinsObj[5];

//Joystick:
int16_t joystickVal[2] = {0,0};
extern void runJoystick(int16_t *joyVal);

//Button:
uint8_t buttonPressed = false;

//Font Text:
extern uint8_t font8x8_basic[128][8];
Text_Info font8by8 = {0,0,NULL,0,'\0',DISP_BLACK, BackgroundColorPixel, 7,7,objPixelSpace, (uint8_t *)font8x8_basic, 128, 8};


//Rover Game State
enum Game_Status curState = TITLE_SCREEN;
enum Game_Status prevState = FINISHED; //Should be different at start to signal state change
uint8_t gameMode = 0; //0->easy, 1->medium, 2->hard

//Init States:
void InitTitle();
void InitMode();
void InitRunning();
void InitFinished();

//Continuously running function for state:
void RunTitle();
void RunMode();
void RunGamePlay();
void RunFinished();

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
  //Colors
  static uint8_t **allObjColorTypes[2]={roverColorTypes,coinColorTypes};
  static uint32_t *allObjColors[2]  = {roverColors, coinColors};

  //Width in pixels of each object-> {RoverStraight, roverAngled, spiderStraight, spiderAngled,.. }
  static uint16_t objPxWidth[14] = {14,14,5,5,5,5,5,5,5,5,5,5,3,3};
  static uint16_t objPxHeight[14] = {14,14,5,5,5,5,5,5,5,5,5,5,3,3};
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

void UpdateButton(){
  static uint8_t buttonStatus = false;
  uint8_t curButton = HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_3);
  if(curButton & !buttonStatus)
    buttonPressed=true;
  buttonStatus=curButton;
  
  
    
}

void DisplayNumber(int16_t num, uint8_t numDig, uint16_t x, uint16_t y){
  //Max digits 5
  static uint8_t numStr[6] = "12345";
  
  font8by8.x=x; font8by8.y=y;font8by8.numChrs=numDig; font8by8.msg= numStr;
  for(int16_t i = numDig-1; i>=0;i--, num/=10){
    numStr[i]= num%10+48;
  }
  writeText(&font8by8);
}
void DisplayScore(){
  if(prevScore==roverGameScore)
    return;
  //Only display score if it changed
  prevScore=roverGameScore;
  DisplayNumber(roverGameScore,3,106, 1);
}

void DisplayTime(int16_t *curTime, uint16_t maxTime, uint8_t x, uint8_t y){
  static uint8_t decrementTime = 0;
  uint8_t displayTime=false;
  if(*curTime==-1){ //Start of Game
    decrementTime=0;
    *curTime=maxTime;
    displayTime=true;
  }
  //Once it reaches 99->1000 ms has passed
  if(decrementTime==99){
    *curTime=*curTime-1;
    displayTime=true;
  }
  //Increment Counter
  decrementTime=decrementTime>=99? 0: decrementTime+1; 
  if(!displayTime){
    return;
  }
  //Display Time:
  DisplayNumber(*curTime, 2, x, y);
 
}
  
void InitTitle(){
  const uint8_t *titleName = "ROVER COIN DASH";
  const uint8_t *byStr = "BY";
  const uint8_t *name = "ISHAN KUMAR";
  
  //Put Rover title
  setBackground();
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
  
  //Draw Start Shape:
  drawLine(X_LOC_START,Y_LOC_START,X_LOC_START+38, Y_LOC_START, DISP_BLACK); //Top Hor Line
  drawLine(X_LOC_START,Y_LOC_START+10,X_LOC_START+38, Y_LOC_START+10, DISP_BLACK); //Bottom Hor Line
  drawLine(X_LOC_START,Y_LOC_START,X_LOC_START, Y_LOC_START+10, DISP_BLACK); //Left Vert. Line
  drawLine(X_LOC_START+38,Y_LOC_START,X_LOC_START+43, Y_LOC_START+5, DISP_BLACK); //Top Slanted Line
  drawLine(X_LOC_START+38,Y_LOC_START+10,X_LOC_START+43, Y_LOC_START+5, DISP_BLACK); //Bottom Slanted Line
  
}

void InitMode(){
}

void InitRunning(){
  const uint8_t *timeStr = "Time:";
  const uint8_t *scoreStr = "Score:";
  
  setBackground();
  
  //Put Time, Score 
  font8by8.x=1; font8by8.y=1; font8by8.numChrs=5;font8by8.msg= (uint8_t*)timeStr;
  writeText(&font8by8);
  font8by8.x=64; font8by8.y=1; font8by8.numChrs=6;font8by8.msg= (uint8_t*)scoreStr;
  writeText(&font8by8);
  //Set score
  prevScore=999; 
  roverGameScore=0;
  
  InitializeObjDisp(6);
  
  
  
  
}

void InitFinished(){
  
  const uint8_t finStr[15] = "Game Finished!";
  const uint8_t urScore[12] = "Your Score:";
  const uint8_t highStr[15] = "Highest Score:";
  static uint8_t mainStr[13] = "Going Main:_";
  uint8_t bWidth = 128-X_LOC_FINISH*2;
  uint8_t bHeight = 60;
  //Create White Box:
  writeRectangle(X_LOC_FINISH, Y_LOC_FINISH,bWidth, bHeight, DISP_WHITE);
  //Create black border:
  drawLine(X_LOC_FINISH,Y_LOC_FINISH,X_LOC_FINISH+bWidth, Y_LOC_FINISH, DISP_BLACK); //Top Hor Line
  drawLine(X_LOC_FINISH,Y_LOC_FINISH+bHeight,X_LOC_FINISH+bWidth, Y_LOC_FINISH+bHeight, DISP_BLACK); //Bottom Hor Line
  drawLine(X_LOC_FINISH,Y_LOC_FINISH,X_LOC_FINISH, Y_LOC_FINISH+bHeight, DISP_BLACK); //Left Vert Line
  drawLine(X_LOC_FINISH+bWidth,Y_LOC_FINISH,X_LOC_FINISH+bWidth, Y_LOC_FINISH+bHeight, DISP_BLACK); //Right Vert Line
  
  //Write Text:
  font8by8.x=X_LOC_FINISH+2; font8by8.y=Y_LOC_FINISH+2; font8by8.numChrs=14;font8by8.msg= (uint8_t*)finStr;
  writeText(&font8by8);
  font8by8.x=X_LOC_FINISH+2; font8by8.y=Y_LOC_FINISH+2+9; font8by8.numChrs=11;font8by8.msg= (uint8_t*)urScore;
  writeText(&font8by8);
  font8by8.x=X_LOC_FINISH+2; font8by8.y=Y_LOC_FINISH+2+9*3; font8by8.numChrs=14;font8by8.msg= (uint8_t*)highStr;
  writeText(&font8by8);
  font8by8.x=X_LOC_FINISH+2; font8by8.y=Y_LOC_FINISH+2+9*5; font8by8.numChrs=12;font8by8.msg= (uint8_t*)mainStr;
  writeText(&font8by8);
  
  //Write Scores:
  highestScore=MAX(highestScore, roverGameScore);
  DisplayNumber(roverGameScore, 3, X_LOC_FINISH+2, Y_LOC_FINISH+2+9*2);
  DisplayNumber(highestScore, 3, X_LOC_FINISH+2, Y_LOC_FINISH+2+9*4);
  roverTime = -1;
  
}

void RunTitle(){
  const uint8_t *strtStr = "START"; 
  static uint8_t changeColorCounter = 29;
  static uint32_t curColor = DISP_YELLOW;
  
  //Check if button pressed (user wants to start):
  UpdateButton();
  if(buttonPressed){
    buttonPressed=false;
    curState=RUNNING_GAME;
    return;
  }
  
  changeColorCounter=changeColorCounter>=29? 0: changeColorCounter+1; //Increment
  if(changeColorCounter!=0) //Change color every 300 ms
    return;
  
  //Alternate between yellow and white
  curColor=curColor==DISP_YELLOW? DISP_BLACK:DISP_YELLOW; 
  font8by8.colorRGB=curColor;
  //Write START to display
  font8by8.x=X_LOC_START+2; font8by8.y=Y_LOC_START+2; font8by8.numChrs=5;
  font8by8.msg= (uint8_t*)strtStr;
  writeText(&font8by8);
  //Change color back to black since most text is black
  font8by8.colorRGB = DISP_BLACK;
}

void RunGamePlay(){
  //Calculate new position of object
  for(uint8_t i = 0; i<MAX_NUM_OBJ; i++){
    Game_Obj *gObj = &gameObjects[i];
//    //First see if object is active:
//    if(gObj->active)
      ObjectPositionCalc(gObj);
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
  runJoystick(joystickVal);
  DisplayScore();
  DisplayTime(&roverTime, 60, 36, 1);
  //Change state to finished if time is done:
  if(roverTime==55){
    curState=FINISHED;
    
  }
}

void RunMode(){
}

void RunFinished(){
  DisplayTime(&roverTime, 5, X_LOC_FINISH+2+7*11, Y_LOC_FINISH+2+9*5);
  //Change state to TITLE if time is done:
  if(roverTime==0){
    curState=TITLE_SCREEN;
  }

}

void InitializeStateChange(){
  //Initialize state only if state change
  if(prevState==curState){
    return;
  }
  prevState = curState;
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
      RunTitle();
      break;
    case SELECT_MODE:
      RunMode();
      break;
    case RUNNING_GAME:
      RunGamePlay();
      break;
    case FINISHED:
      RunFinished();
      break;
  }
  
}
