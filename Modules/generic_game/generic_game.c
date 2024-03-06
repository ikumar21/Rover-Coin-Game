#include "generic_game.h"
#define MAX_NUM_OBJ 15

#define ABS_M(a) a<0? -a:a
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

void SetObjectColor(uint32_t *destColorArr, Obj_Disp *obj){
  //Function that sets the color array according to object and angle it is
  
  //First correct angle, width, height, and then choose right color array
  
  // correct angle (default to 90 if angle not (0-315); multiple of 45))
  if(obj->angle>315 || obj->angle %45 != 0)
    obj->angle = 90;
  //Fix height/width (pixels) of object
  uint8_t objIndex = ((obj->angle%90)!=0)+2*obj->objType;
  obj->orWidth = obj->width;//objPxWidth[objIndex];
  obj->orHeight = obj->height;//objPxHeight[objIndex];

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


uint8_t inFrame(Game_Obj *gObj1, Game_Obj *gObj2){
  //Returns if object is in frame of another
  //Faster if gObj2 is smaller than gObj1
  Obj_Disp *dObj1= gObj1->dObj; Obj_Disp *dObj2 = gObj2->dObj;
  uint8_t toLeft = dObj1->xLoc+dObj1->width>=dObj2->xLoc; //Object 1 is to left of object 2 (no intersection)
  uint8_t toRight = dObj2->xLoc+dObj2->width>=dObj1->xLoc; //Object 1 is to right of object 2 (no intersection)
  uint8_t toTop = dObj1->yLoc+dObj1->height>=dObj2->yLoc; //Object 1 is to top of object 2 (no intersection)
  uint8_t toBottom = dObj2->yLoc+dObj2->height>=dObj1->yLoc; //Object 1 is to bottom of object 2 (no intersection)
  return !(toLeft || toRight || toTop || toBottom);
  
}

uint8_t collisionThere(Game_Obj *gObj1, Game_Obj *gObj2){
  //Detects if two objects are intersecting ->returns true or false
  Obj_Disp *dObj1= gObj1->dObj; Obj_Disp *dObj2 = gObj2->dObj;
  
  if(inFrame(gObj1,gObj2)){
    return false; //No intersection if they aren't in frame
  }
  //Start looking at right/down most left/top edge of objects:
  uint16_t xStart = MAX(dObj1->xLoc, dObj2->xLoc);
  uint16_t yStart = MAX(dObj1->yLoc, dObj2->yLoc);
  //End looking at left/top most right/bottom edge of objects:
  uint16_t xEnd = MIN(dObj1->xLoc+dObj1->width, dObj2->xLoc+dObj2->width)-1;
  uint16_t yEnd = MIN(dObj1->yLoc+dObj1->height, dObj2->yLoc+dObj2->height)-1;
  
  uint8_t collision = false;
  //Get right color array according to angle
  //Go through intersection of objects and make sure overlapped values aren't background
  uint8_t *colorArr1 = dObj1->objColorTypes[(dObj1->angle%90)!=0];
  uint8_t *colorArr2 = dObj2->objColorTypes[(dObj2->angle%90)!=0];
  for(uint16_t x = xStart; x<=xEnd;x++){
    if(collision)
      break;
    for(uint16_t y = yStart; y<=yEnd; y++){
      //row*width+col;
      uint8_t colorType1 = colorArr1[(x-dObj1->xLoc)*dObj1->width+y-dObj1->yLoc];
      uint8_t colorType2 = colorArr2[(x-dObj2->xLoc)*dObj2->width+y-dObj2->yLoc];
      //If both pixels aren't background-> collision
      if(colorType1!=0 && colorType2!=0){
        collision = true;
        break;
      }
    }
  }
  return collision;
  
     
  
  
}