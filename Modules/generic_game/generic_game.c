#include "generic_game.h"
#define MAX_NUM_OBJ 15
extern uint32_t objPixelSpace[200];
extern Game_Obj gameObjects[15];
extern Obj_Disp dispObjects[MAX_NUM_OBJ];
uint32_t BackgroundColorPixel(uint16_t x,uint16_t y,uint8_t objIndex){ 
 return checkerBoard(x,y);
}

void BackgroundFillBox(uint16_t xS, uint16_t xE, uint16_t yS, uint16_t yE, uint8_t objIndex){
  //Limit 200 pixels
  uint16_t numPix = (xE-xS+1)*(yE-yS+1);
  uint16_t i = 0;
  for(uint16_t row = yS; row<yE+1; row++){
    for(uint16_t col = xS; col<xE+1; col++){
      objPixelSpace[i++]= BackgroundColorPixel(col,row,objIndex);
    }
  }
  setColumnRowRange(xS,xE,yS,yE);
  writeColorArray(objPixelSpace,numPix);
}

void FillNewBackgroundObj(){
  //Fill up new background caused by moving objects
  for(uint8_t i = 0; i<MAX_NUM_OBJ; i++){
    Obj_Disp *obj  = &dispObjects[i];
    Game_Obj *gObj  = &gameObjects[i];
    //First see if object is active:
    if(gObj->active){
      //Draw at max two boxes to fill in background where object was 
      
      uint8_t drawB1 = obj->prevXLoc!=obj->xLoc;//Draw Box 1: If object shifted Left/Right
      uint8_t drawB2 =obj->prevYLoc!=obj->yLoc;//Draw Box 2: If object shifted UP/Down
      
      uint8_t R = obj->xLoc>obj->prevXLoc; //Object shifted Right
      uint8_t D = obj->prevYLoc<obj->yLoc; //Object shifted Up
      uint8_t NO = obj->prevYLoc>obj->yLoc+obj->height ||
                   obj->yLoc>obj->prevYLoc+obj->height || 
                   obj->xLoc>obj->prevXLoc+obj->width ||
                   obj->prevXLoc>obj->xLoc+obj->width; //No overlap between where object is/was
      
      if(NO){
        BackgroundFillBox(obj->prevXLoc,obj->prevXLoc+obj->width-1,
                          obj->prevYLoc,obj->prevYLoc+obj->height-1,i);
      }
      if(drawB1 && !NO){
        uint16_t box1StartX = obj->prevXLoc*R+(obj->xLoc+obj->width)*!R;
        uint16_t box1EndX = obj->xLoc*R+(obj->prevXLoc+obj->width)*!R-1;
        BackgroundFillBox(box1StartX,box1EndX,obj->prevYLoc,obj->prevYLoc+obj->height-1,i);
      }
      
      if(drawB2 && !NO){
        uint16_t box2StartY = obj->prevYLoc*D+(obj->yLoc+obj->height)*!D;   
        uint16_t box2EndY = obj->yLoc*D+(obj->prevYLoc+obj->height)*!D-1;
        BackgroundFillBox(obj->prevXLoc,obj->prevXLoc+obj->width-1,box2StartY,box2EndY,i);
      }   
    } 
  }
}



uint8_t inFrame(Game_Obj *gObj1, Game_Obj *gObj2){
  //Faster if gObj2 is smaller than gObj1
  Obj_Disp *dObj1= gObj1->dObj; Obj_Disp *dObj2 = gObj2->dObj;
  uint8_t toLeft = dObj1->xLoc+dObj1->width>=dObj2->xLoc; //Object 1 is to left of object 2 (no intersection)
  uint8_t toRight = dObj2->xLoc+dObj2->width>=dObj1->xLoc; //Object 1 is to right of object 2 (no intersection)
  uint8_t toTop = dObj1->yLoc+dObj1->height>=dObj2->yLoc; //Object 1 is to top of object 2 (no intersection)
  uint8_t toBottom = dObj2->yLoc+dObj2->height>=dObj1->yLoc; //Object 1 is to bottom of object 2 (no intersection)
  return !(toLeft || toRight || toTop || toBottom);
  
}