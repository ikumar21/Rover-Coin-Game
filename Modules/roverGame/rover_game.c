#include "rover_game.h"
#include "main.h"
#include "EEPROM.h"
#include "joystick.h"
#include "levels.h"

//Rover starting Location
#define ROVER_STRT_X 50
#define ROVER_STRT_Y 50

//Distance between point of edge and arrow:
#define X_ARROW_DIST 2
//Location of start
#define Y_LOC_START 127
//Location of Finish Box
#define X_LOC_FINISH 13
#define Y_LOC_FINISH 55

//Location of EXIT Box LVL
#define Y_LOC_EXIT_LVL Y_LOC_START

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
Coin_Obj coinsObj[MAX_NUM_COINS];

//Joystick:
int16_t joystickVal[2] = {0,0};

//Button:
uint8_t buttonPressed = false;

//Cursor Location [row, col]
uint8_t crsLoc[2] = {0, 0};
uint8_t prevCrsLoc[2]= {0, 0};

//Font Text:
extern uint8_t font8x8_basic[128][8];
Text_Info font8by8 = {0,0,NULL,0,'\0',DISP_BLACK, BackgroundColorPixel, 7,7,objPixelSpace, (uint8_t *)font8x8_basic, 128, 8};


//Rover Game State
enum Game_Status curState = TITLE_SCREEN;
enum Game_Status prevState = FINISHED; //Should be different at start to signal state change


//Game Level Data
uint8_t gameLevel = 0;
uint8_t beatLevel;
typedef struct{
  uint8_t qckTimes[10];
  uint32_t maxlvlDone;
} Lvl_Data_E;
union Lvl_E_U{
  Lvl_Data_E lvlsData;
  uint8_t dataArr[14];
};
union Lvl_E_U eepromLvlData;

//Reset EEPROM to all levels locked (except 1) and all game times to MAX
union Lvl_E_U resetEEPROMData = {.lvlsData = {{0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
                                               0xFF, 0xFF, 0xFF, 0xFF, 0xFF}, 1}};
volatile uint8_t resetEEPROM = false;

//Current Game Level parameters:
uint16_t coinGenTime[2];
uint8_t maxCoins;
uint8_t maxGameTime;
uint16_t targetScore;

//Lock Info
Obj_Disp lockObj;
uint32_t lockColors[4] = {0xF8F8F8,0x0C0C0C,0x3C3C3C,DISP_BLACK};
uint8_t lockColorType[9][8] = {
{0,1,1,1,1,1,1,0},
{1,1,0,0,0,0,1,1},
{1,0,0,0,0,0,0,1},
{1,0,0,0,0,0,0,1},
{2,2,2,2,2,2,2,2},
{2,2,2,3,3,2,2,2},
{2,2,2,3,3,2,2,2},
{2,2,2,3,3,2,2,2},
{2,2,2,2,2,2,2,2}};
uint8_t *lockColorTypes[2] = {(uint8_t *)lockColorType, (uint8_t *)lockColorType};

//Init States:
void InitTitle();
void InitLevel();
void InitRunning();
void InitFinished();

//Continuously running function for state:
void RunTitle();
void RunLevel();
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
  for(uint8_t i = 1; i<=MAX_NUM_COINS;i++){
    gameObjects[i].dObj=&dispObjects[i];
    gameObjects[i].objType=COIN;
    dispObjects[i].objType=COIN;
    gameObjects[i].coinObj=&coinsObj[i-1];
    gameObjects[i].active=false;
  }
  
    //Initialize object data based on object type and angle
    for(uint8_t i = 0; i<=numObjs;i++){
        Game_Obj *gObj = &gameObjects[i];
        Obj_Disp *obj = gObj->dObj;

        //First correct angle (default to 0 if angle not (0-315); multiple of 45))
        if(obj->angle>315 || obj->angle %45 != 0)
            obj->angle = 0;

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
  uint8_t didCoinControl = true;
  Obj_Disp *dObj = gObj->dObj;
  if(gObj->objType==ROVER){
    RoverController(gObj, joystickVal[0],joystickVal[1]);
  }else if (gObj->objType==COIN){
    ;
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
  writeRightArrowRecOutline(127-X_ARROW_DIST-47, Y_LOC_START, 37, 47, 11, DISP_BLACK); 
  
  //Reset EEPROM if wanted:
  if(resetEEPROM){
    resetEEPROM=false;
    EWriteData(resetEEPROMData.dataArr, 14, 0);
  }
}

void InitLevel(){
  const uint8_t *selLvlStr = "Select Level";
  const uint8_t *exitStr = "EXIT";
  setBackground();
  
  
  
  //Put Select Level String
  font8by8.x=22; font8by8.y=5; font8by8.numChrs=12;
  font8by8.msg= (uint8_t*)selLvlStr;
  writeText(&font8by8);
  
  //Display a bar underneath Select Level
  writeRectangle(22, 12, 84,1, 0x0); 
  
  //Draw EXIT string and its left arrow box
  font8by8.x=X_ARROW_DIST+11; font8by8.y=Y_LOC_START+2; font8by8.numChrs=4;
  font8by8.msg= (uint8_t*)exitStr;
  writeText(&font8by8);
  writeLeftArrowRecOutline(X_ARROW_DIST, Y_LOC_START, 30, 40, 11, DISP_BLACK);
  
  //Read Level data in EEPROM
  EReadData(eepromLvlData.dataArr, 14, 0);
  
  uint8_t curLvl =0;
  
  //Set up Lock
  lockObj.width=8; lockObj.height=9;
  lockObj.objColors=lockColors; lockObj.objColorTypes=lockColorTypes;
  //Display rectangles and level number/locks
  for(uint8_t row = 0; row<3; row++){
    for(uint8_t col = 0; col<4;col++){
      curLvl = row*4+col+1;
      if(curLvl>=11){ //Skip if levels 11 and onwards
        break;
      }
      //If level is less than max lvl done ->put number, otherwise put lock
      if(curLvl <=eepromLvlData.lvlsData.maxlvlDone){
        //Write Number
        if(curLvl/10){ //Is it 2 digits
          DisplayNumber(curLvl, 2, 9+col*32, 30+row*25 );
        }else{
          DisplayNumber(curLvl, 1, 13+col*32, 30+row*25 );
        }
        
      }else{
        SetObjectColor(objPixelSpace,&lockObj);
        setColumnRowRange(12+col*32,12+col*32+lockObj.width-1,29+row*25 ,29+row*25+lockObj.height-1);
        writeColorArray(objPixelSpace,lockObj.width*lockObj.height);

      }
      writeRectangleOutline(6+col*32, 27+row*25, 20, 13, DISP_BLACK);
    }
  }
  
  //set cursor location to 0,0 and gameLevel to 0
  crsLoc[0]=0; crsLoc[1]=0; prevCrsLoc[0] = 0; prevCrsLoc[1]=0;gameLevel=0;
  
  
  
  
  
}

void InitRunning(){
  const uint8_t *timeStr = "TIME:";
  const uint8_t *scoreStr = "SCORE:";
  const uint8_t *targetStr = "TARGET:";
  
  setBackground();
  //Set score
  prevScore=999; 
  roverGameScore=0;
  
  InitializeObjDisp(MAX_NUM_COINS+1);
  
  roverTime = -1; //-1 notes timer start
  numActiveCoins = 0; //0 active coins at start
  timeToGen = 0; //Start game with one coin
  
  //Update Level Parameters:
  coinGenTime[0]=allLvlInfo[gameLevel-1]->coinGenTime[0];
  coinGenTime[1]=allLvlInfo[gameLevel-1]->coinGenTime[1];
  maxCoins = allLvlInfo[gameLevel-1]->maxCoins;
  maxGameTime = allLvlInfo[gameLevel-1]->maxGameTime;
  targetScore = allLvlInfo[gameLevel-1]->targetScore;
  
  //Display a bar underneath score and time
  writeRectangle(0, 8, 128,1, 0x0); 
  
  //Put Time, Score, Target string
  font8by8.x=1; font8by8.y=1; font8by8.numChrs=5;font8by8.msg= (uint8_t*)timeStr;
  writeText(&font8by8);
  font8by8.x=64; font8by8.y=1; font8by8.numChrs=6;font8by8.msg= (uint8_t*)scoreStr;
  writeText(&font8by8);
  font8by8.x=29; font8by8.y=153; font8by8.numChrs=7;font8by8.msg= (uint8_t*)targetStr;
  writeText(&font8by8);
  DisplayNumber(targetScore, 3, 77, 153);
  
  //Put Target Box
  writeRectangle(27, 151, 1, 9,  DISP_BLACK );
  writeRectangle(98, 151, 1, 9,  DISP_BLACK );
  writeRectangle(27, 151, 72, 1,  DISP_BLACK );
}

void InitFinished(){
  
//  setBackground();
   
  const uint8_t finStr[15] = "!!LEVEL BEAT!!";
  const uint8_t urTimeStr[11] = "YOUR TIME:";
  const uint8_t bestTimeStr[11] = "BEST TIME:";
  const uint8_t maxTimeStr[11] = "MAX  TIME:";
  const uint8_t urScoreStr[12] = "YOUR SCORE:";
  const uint8_t targetStr[12] = "THE TARGET:";
  const uint8_t failedStr[14] = "FAILED LEVEL!";
  const uint8_t *exitStr = "EXIT";
  const uint8_t *levelsStr = "LEVELS";
  const uint8_t *retryStr = "RETRY";
  const uint8_t *nextStr = "NEXT";
  
  beatLevel = targetScore <=roverGameScore;

  //Create White Box with black border:
  writeRectangle(X_LOC_FINISH, Y_LOC_FINISH,128-X_LOC_FINISH*2, 40+10*beatLevel, DISP_WHITE);
  writeRectangleOutline(X_LOC_FINISH, Y_LOC_FINISH, 128-X_LOC_FINISH*2, 40+10*beatLevel, DISP_BLACK);

  //Write Text depending on if level is beat or not:
  font8by8.x=X_LOC_FINISH+2; font8by8.y=Y_LOC_FINISH+2; font8by8.numChrs=14-!beatLevel;
  font8by8.msg= beatLevel ? (uint8_t*)finStr : (uint8_t*)failedStr;
  writeText(&font8by8);
  font8by8.x=X_LOC_FINISH+2; font8by8.y=Y_LOC_FINISH+2+9*2; font8by8.numChrs=10+!beatLevel;
  font8by8.msg= beatLevel ? (uint8_t*)urTimeStr : (uint8_t*)urScoreStr;
  writeText(&font8by8);
  font8by8.x=X_LOC_FINISH+2; font8by8.y=Y_LOC_FINISH+2+9*3; font8by8.numChrs=10+!beatLevel;
  font8by8.msg= beatLevel ? (uint8_t*)bestTimeStr : (uint8_t*)targetStr;
  writeText(&font8by8);
  
  uint8_t bestTime = MIN(maxGameTime-roverTime, eepromLvlData.lvlsData.qckTimes[gameLevel-1]);
  //Write Times or scores:
  if(beatLevel){
    font8by8.x=X_LOC_FINISH+2; font8by8.y=Y_LOC_FINISH+2+9*4; font8by8.numChrs=10;
    font8by8.msg= (uint8_t*)maxTimeStr;
    writeText(&font8by8);
    DisplayNumber(maxGameTime, 3, X_LOC_FINISH+78-beatLevel*2, Y_LOC_FINISH+2+9*4);
    DisplayNumber(maxGameTime-roverTime, 3, X_LOC_FINISH+78-beatLevel*2, Y_LOC_FINISH+2+9*2);
    DisplayNumber(bestTime, 3, X_LOC_FINISH+78-beatLevel*2, Y_LOC_FINISH+2+9*3);
  }else{
    DisplayNumber(roverGameScore, 3, X_LOC_FINISH+78-beatLevel*2, Y_LOC_FINISH+2+9*2);
    DisplayNumber(targetScore, 3, X_LOC_FINISH+78-beatLevel*2, Y_LOC_FINISH+2+9*3);
  }
  
  //Draw Level string and its left arrow box
  font8by8.x=X_ARROW_DIST+11; font8by8.y=Y_LOC_EXIT_LVL-100; font8by8.numChrs=6;
  font8by8.msg= (uint8_t*)levelsStr;
  writeText(&font8by8);
  writeLeftArrowRecOutline(X_ARROW_DIST, Y_LOC_EXIT_LVL-102, 44, 54, 11, DISP_BLACK);
  
  //Draw EXIT string and its left arrow box
  font8by8.x=X_ARROW_DIST+11; font8by8.y=Y_LOC_EXIT_LVL+2; font8by8.numChrs=4;
  font8by8.msg= (uint8_t*)exitStr;
  writeText(&font8by8);
  writeLeftArrowRecOutline(X_ARROW_DIST, Y_LOC_EXIT_LVL, 30, 40, 11, DISP_BLACK);
  
  //Draw Retry string and it box
  font8by8.x=64-20+2; font8by8.y=Y_LOC_EXIT_LVL+2; font8by8.numChrs=5;
  font8by8.msg= (uint8_t*)retryStr;
  writeText(&font8by8);
  writeRectangleOutline(64-20, Y_LOC_EXIT_LVL, 5*7+4, 11, DISP_BLACK);
  
  if(beatLevel){
    //Update EEPROM if level beat
    eepromLvlData.lvlsData.maxlvlDone = MAX(gameLevel+1, eepromLvlData.lvlsData.maxlvlDone);
    eepromLvlData.lvlsData.qckTimes[gameLevel-1] = bestTime; 
    EWriteData(eepromLvlData.dataArr, 14, 0);
  }
  //Draw Next Str and its right arrow box if level available
  if(eepromLvlData.lvlsData.maxlvlDone>=gameLevel+1){
    writeRightArrowRecOutline(127-X_ARROW_DIST-40, Y_LOC_START, 30, 40, 11, DISP_BLACK); 
    font8by8.x=127-X_ARROW_DIST-40+2; font8by8.y=Y_LOC_START+2; font8by8.numChrs=4;
    font8by8.msg= (uint8_t*)nextStr;
    writeText(&font8by8);
  }

  //Set crs Loc to either Next or Retry
  crsLoc[0] = 1;
  crsLoc[1] = beatLevel + 1;
  prevCrsLoc[0] = crsLoc[0];
  prevCrsLoc[1]= crsLoc[1];
  
}

void RunTitle(){
  const uint8_t *strtStr = "START"; 
  static uint8_t changeColorCounter = 49;
  static uint32_t curColor = DISP_YELLOW;
  
  //Check if button pressed (user wants to start):
  UpdateButton();
  if(buttonPressed){
    buttonPressed=false;
    curState=SELECT_LEVEL;
    return;
  }
  
  //Change color every 500 ms
  changeColorCounter=changeColorCounter>=49? 0: changeColorCounter+1; //Increment
  if(changeColorCounter!=0) 
    return;
  
  //Alternate between yellow and white
  curColor=curColor==DISP_YELLOW? DISP_BLACK:DISP_YELLOW; 
  font8by8.colorRGB=curColor;
  //Write START to display
  font8by8.x=127-X_ARROW_DIST-47+2; font8by8.y=Y_LOC_START+2; font8by8.numChrs=5;
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
  //Set up Coins:
  CoinController(gameObjects);
  
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
  DisplayTime(&roverTime, maxGameTime, 36, 1);
  //Change state to finished if time is done or target score reached:
  if(roverTime==0 || roverGameScore>=targetScore){
    curState=FINISHED;
  }
}

void RunLevel(){
  const uint8_t *exitStr = "EXIT";
  static uint8_t updateCrsCounter = 29;
  static uint8_t changeColorCounter = 39;
  static uint32_t curColor = DISP_BLACK;
  static uint8_t dirCrs = 0;
  static uint8_t justChangedCrs = false;
  
  //Get Cursor movement; gives 0 if no movement
  dirCrs=getJoyDirCom(); //Right-1, Up-2, Left-3, Down-4
  
  //Change cursor loc if movement and not recently changed
  uint8_t legalMoveRight = (dirCrs==1) && crsLoc[0]!=3 && gameLevel+1<=eepromLvlData.lvlsData.maxlvlDone;
  uint8_t legalMoveUp =  (dirCrs==2) && crsLoc[0]!=0;
  uint8_t legalMoveLeft = (dirCrs==3) && crsLoc[0]!=3 && gameLevel!=1;
  uint8_t legalMoveDown = (dirCrs==4) && crsLoc[0]!=3;
  uint8_t legalMove = legalMoveRight || legalMoveUp || legalMoveLeft || legalMoveDown;
  
  if(!justChangedCrs && legalMove){
    justChangedCrs=true; updateCrsCounter=0;
    //Set color to yellow first:
    changeColorCounter=255;
    curColor=DISP_BLACK;
    uint8_t atExit = crsLoc[0]==3 && crsLoc[1]==0;
    uint8_t goExit = (gameLevel+4>eepromLvlData.lvlsData.maxlvlDone);
    switch (dirCrs){
      case 1://Joystick Right
        //Either go right one col or go next row, first col
        crsLoc[0]=(gameLevel%4==0)*(1)+crsLoc[0];
        crsLoc[1]=(gameLevel%4!=0)*(crsLoc[1]+1);
        break;
      case 2://Joystick Up
        //Go to max level done or up one row
        crsLoc[0]=atExit ? (eepromLvlData.lvlsData.maxlvlDone-1)/4 : (crsLoc[0]-1); 
        crsLoc[1]=atExit ? ((eepromLvlData.lvlsData.maxlvlDone-1)%4): crsLoc[1];
        break;
      case 3://Joystick Left
        //Either go left one col or go up one row, last col
        crsLoc[0]=((gameLevel-1)%4==0)*(-1)+crsLoc[0];
        crsLoc[1]=((gameLevel-1)%4!=0)*(crsLoc[1]-4)+3;
        break;
      case 4://Joystick Down
        //Go to EXIT or down one row 
        crsLoc[0]=!goExit*(crsLoc[0]+1)+goExit*3;
        crsLoc[1]=!goExit*crsLoc[1];
        break;
    }
  }
  //Update game level based on cursor location
  gameLevel=crsLoc[0]*4+crsLoc[1]+1;
  
  
  //Wait 300 ms before allowing a change in cursor location
  if(justChangedCrs){
    updateCrsCounter=updateCrsCounter>=29? 0: updateCrsCounter+1; //Increment
    justChangedCrs=!(updateCrsCounter==0);
  }
  
  uint8_t onExit = crsLoc[0] ==3;
  //Check if button pressed (user wants to exit or start):
  UpdateButton();
  if(buttonPressed){
    buttonPressed=false;
    //Either Exit or go run game at level
    curState= onExit*TITLE_SCREEN + !onExit*RUNNING_GAME;
    return;
  }
  
  //Change color every 400 ms
  changeColorCounter=changeColorCounter>=39? 0: changeColorCounter+1; //Increment
  if(changeColorCounter!=0) 
    return;

  //Write num/exit to display
  //Alternate between yellow and Black
  curColor=curColor==DISP_GREEN? DISP_BLACK:DISP_GREEN; 
  font8by8.colorRGB=curColor;
  if(onExit){//Draw EXIT string 
    font8by8.x=X_ARROW_DIST+11; font8by8.y=Y_LOC_START+2; font8by8.numChrs=4;
    font8by8.msg= (uint8_t*)exitStr;
    writeText(&font8by8);
  }else{//Draw number 
    if(gameLevel/10){ //Is it 2 digits
      DisplayNumber(gameLevel, 2, 9+crsLoc[1]*32, 30+crsLoc[0]*25 );
    }else{
      DisplayNumber(gameLevel, 1, 13+crsLoc[1]*32, 30+crsLoc[0]*25 );
    }
  }

  //Change color back to black since most text is black
  font8by8.colorRGB = DISP_BLACK;
  
  //Set previous string/number to black if cursor moved
  if(prevCrsLoc[0]!=crsLoc[0] || prevCrsLoc[1]!=crsLoc[1]){
    if(prevCrsLoc[0]==3){//Draw EXIT string 
    font8by8.x=X_ARROW_DIST+11; font8by8.y=Y_LOC_START+2; font8by8.numChrs=4;
    font8by8.msg= (uint8_t*)exitStr;
    writeText(&font8by8);
  }else{//Draw number 
    uint8_t prevGameLevel = prevCrsLoc[0]*4+prevCrsLoc[1]+1;
    if(prevGameLevel/10){ //Is it 2 digits
      DisplayNumber(prevGameLevel, 2, 9+prevCrsLoc[1]*32, 30+prevCrsLoc[0]*25 );
    }else{
      DisplayNumber(prevGameLevel, 1, 13+prevCrsLoc[1]*32, 30+prevCrsLoc[0]*25 );
    }
  }
    prevCrsLoc[0]=crsLoc[0]; prevCrsLoc[1]=crsLoc[1];  
  }
  
}

void RunFinished(){
  const uint8_t *exitStr = "EXIT";
  const uint8_t *levelsStr = "LEVELS";
  const uint8_t *retryStr = "RETRY";
  const uint8_t *nextStr = "NEXT";
  static uint8_t updateCrsCounter = 29;
  static uint8_t changeColorCounter = 39;
  static uint32_t curColor = DISP_BLACK;
  static uint8_t dirCrs = 0;
  static uint8_t justChangedCrs = false;
  
  //Font prop of levels, exit, retry, next
  static uint8_t fontX[4] = {X_ARROW_DIST+11, X_ARROW_DIST+11, 64-20+2, 127-X_ARROW_DIST-40+2};
  static uint8_t fontY[4] = {Y_LOC_EXIT_LVL-100, Y_LOC_EXIT_LVL+2, Y_LOC_EXIT_LVL+2, Y_LOC_EXIT_LVL+2};
  static uint8_t fontN[5] = {6, 4, 5, 4, 4};
  uint8_t *fontM[4] = {(uint8_t *)levelsStr, (uint8_t *)exitStr, (uint8_t *)retryStr, (uint8_t *)nextStr};
  
  //Get Cursor movement; gives 0 if no movement
  dirCrs=getJoyDirCom(); //Right-1, Up-2, Left-3, Down-4
  
  //Change cursor loc if movement and not recently changed
  uint8_t nextLvlAval = eepromLvlData.lvlsData.maxlvlDone>=gameLevel+1;
  uint8_t legalMoveRight = (dirCrs==1) && crsLoc[1]!=2 && (nextLvlAval || crsLoc[1]!=1);
  uint8_t legalMoveUp =  (dirCrs==2) && crsLoc[0]!=0;
  uint8_t legalMoveLeft = (dirCrs==3) && !(crsLoc[0]==0 && crsLoc[1]==0);
  uint8_t legalMoveDown = (dirCrs==4) && crsLoc[0]==0;
  uint8_t legalMove = legalMoveRight || legalMoveUp || legalMoveLeft || legalMoveDown;
  
  if(!justChangedCrs && legalMove){
    justChangedCrs=true; updateCrsCounter=0;
    //Set color to yellow first:
    changeColorCounter=255;
    curColor=DISP_BLACK;
    switch (dirCrs){
      case 1://Joystick Right
        //Either go right one col or go next row, first col
        crsLoc[1]= crsLoc[0]==0 ? 0 : crsLoc[1]+1;
        crsLoc[0]= crsLoc[0]==0 ? 1 : crsLoc[0];
        break;
      case 2://Joystick Up
        //Go to levels
        crsLoc[0]=0; 
        crsLoc[1]=0;
        break;
      case 3://Joystick Left
        //Either go left one col or go up one row, last col
        crsLoc[0]=crsLoc[1]==0 ? 0 : crsLoc[0] ;
        crsLoc[1]=crsLoc[1]==0 ? 0 : crsLoc[1]-1 ;
        break;
      case 4://Joystick Down
        //Go to EXIT  
        crsLoc[0]=1;
        crsLoc[1]=0;
        break;
    }
  }  
  
  //Wait 300 ms before allowing a change in cursor location
  if(justChangedCrs){
    updateCrsCounter=updateCrsCounter>=29? 0: updateCrsCounter+1; //Increment
    justChangedCrs=!(updateCrsCounter==0);
  }
  
  //Check if button pressed:
  UpdateButton();
  if(buttonPressed){
    buttonPressed=false;
    //Either Exit, go to select levels, or go run game at some level
    if(crsLoc[0]==1 && crsLoc[1]==0){ //Exit
      curState = TITLE_SCREEN;
    
    }else if (crsLoc[0]==0 && crsLoc[1]==0){ //Go to Select Level
      curState = SELECT_LEVEL;
    
    }else{ //Retry or run game at next level
      gameLevel = crsLoc[1]==1 ? gameLevel : gameLevel+1;
      curState = RUNNING_GAME;
    
    }
    
    return;
  }
  
  //Change color every 400 ms
  changeColorCounter=changeColorCounter>=39? 0: changeColorCounter+1; //Increment
  if(changeColorCounter!=0) 
    return;

  //Write num/exit to display
  //Alternate between green and Black
  curColor=curColor==DISP_GREEN? DISP_BLACK:DISP_GREEN; 
  font8by8.colorRGB=curColor;
  
  uint8_t sIdx = crsLoc[0]+crsLoc[1];
  font8by8.x = fontX[sIdx]; font8by8.y=fontY[sIdx]; font8by8.numChrs=fontN[sIdx];
  font8by8.msg= fontM[sIdx];
  writeText(&font8by8);

  //Change color back to black since most text is black
  font8by8.colorRGB = DISP_BLACK;
  
  //Set previous string/number to black if cursor moved
  if(prevCrsLoc[0]!=crsLoc[0] || prevCrsLoc[1]!=crsLoc[1]){
    sIdx = prevCrsLoc[0]+prevCrsLoc[1];
    font8by8.x = fontX[sIdx]; font8by8.y=fontY[sIdx]; font8by8.numChrs=fontN[sIdx];
    font8by8.msg= fontM[sIdx];
    writeText(&font8by8);
    prevCrsLoc[0]=crsLoc[0]; prevCrsLoc[1]=crsLoc[1];  
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
    case SELECT_LEVEL:
      InitLevel();
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
    case SELECT_LEVEL:
      RunLevel();
      break;
    case RUNNING_GAME:
      RunGamePlay();
      break;
    case FINISHED:
      RunFinished();
      break;
  }
  
}
