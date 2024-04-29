#include "displayST7735.h"
#include "font8x8_basic.h"

#define MIN(a,b) a>b? b:a;
extern SPI_HandleTypeDef hspi1;

uint8_t zeroNum = 0;

uint8_t speedColorArr[600];
uint32_t cArr[200];

uint8_t transmitArr[20];
uint8_t rowData;

void setColumnRowRange(uint8_t columnStart, uint8_t columnEnd, uint8_t rowStart, uint8_t rowEnd){
  //Account for display offsets
  columnStart+=DISP_COL_OFFSET;
  columnEnd+=DISP_COL_OFFSET;
  rowStart+=DISP_ROW_OFFSET;
  rowEnd+=DISP_ROW_OFFSET;
  
  
  //Send CASET Command(Setting column ranges)
  writeDisplayCommand(DISP_COM_CASET); 
  
  //Write column ranges
  transmitArr[0] = zeroNum;
  transmitArr[1] = columnStart;
  transmitArr[2] = zeroNum;
  transmitArr[3] = columnEnd;
  HAL_SPI_Transmit(&hspi1,transmitArr,4,1000);

  //Send RASET Command(Setting column ranges)
  writeDisplayCommand(DISP_COM_RASET); 
  
  //Write row ranges
  transmitArr[0] = zeroNum;
  transmitArr[1] = rowStart;
  transmitArr[2] = zeroNum;
  transmitArr[3] = rowEnd;
  HAL_SPI_Transmit(&hspi1,transmitArr,4,1000);

}


void setColor(uint32_t colorRGB, uint16_t pixelCount){
  
  //Send RAMWR command
  writeDisplayCommand(DISP_COM_RAMWR); 
  //Set RGB for all 
  uint8_t redColor=(colorRGB>>16)&0xFF;//Red color
  uint8_t greenColor=(colorRGB>>8)&0xFF;//Green color
  uint8_t blueColor=(colorRGB>>0)&0xFF;//Blue color
  
  //Set the color array of color for all 160 pixels or pixelCount (if smaller)
  uint8_t limitPixel = pixelCount<200 ? pixelCount:200; 
  for(uint32_t i = 0; i<limitPixel*3;i++){
    speedColorArr[i++]=redColor;
    speedColorArr[i++]=greenColor;
    speedColorArr[i]=blueColor;
  }
  
  //Send out pixel color every 160 pixels
  for (uint8_t i =0; i<pixelCount/200;i++){
    HAL_SPI_Transmit(&hspi1,speedColorArr,200*3,1000);
  }
  
  //Send out the rest of the pixel colors <160 pixels
  HAL_SPI_Transmit(&hspi1,speedColorArr,(pixelCount%200)*3,1000);
  
}

void writeDisplayCommand(uint8_t displayCommand){
  //Set DC Pin to Low to signal command
  HAL_GPIO_WritePin(DC_PIN, GPIO_PIN_RESET);
  //Send Command
  HAL_SPI_Transmit(&hspi1,&displayCommand,1,1000);
  
  //Set DC Pin to High for possible parameters
  HAL_GPIO_WritePin(DC_PIN, GPIO_PIN_SET);
}


void initDisplay(){
  
  //Select Display Device
  HAL_GPIO_WritePin(CS_PIN,GPIO_PIN_RESET);
  
  //Send software Reset command
  writeDisplayCommand(DISP_COM_SWRESET);
  HAL_Delay(150);
  
  //Send Sleep out command
  writeDisplayCommand(DISP_COM_SLPOUT);
  HAL_Delay(150);
  
  //Turn on Display
  writeDisplayCommand(DISP_COM_DISPON);
  
  //Make (0,0) left corner display
  writeDisplayCommand(DISP_COM_MADCTL);
  uint8_t madctlPara = 0xC0;
  HAL_SPI_Transmit(&hspi1,&madctlPara,1,1000);
}



void writeRectangle(uint8_t leftX, uint8_t leftY, uint8_t width, uint8_t height, uint32_t colorRGB){
  setColumnRowRange(leftX, leftX+width-1,leftY,leftY+height-1);
  setColor(colorRGB,width*height);
}
         
void writeRectangleOutline(uint8_t leftX, uint8_t leftY, uint8_t width, uint8_t height, uint32_t colorRGB){
  setColumnRowRange(leftX, leftX+width-1,leftY,leftY);
  setColor(colorRGB,width);
  setColumnRowRange(leftX, leftX+width-1,leftY+height-1,leftY+height-1);
  setColor(colorRGB,width);
  setColumnRowRange(leftX, leftX,leftY,leftY+height-1);
  setColor(colorRGB,height);
  setColumnRowRange(leftX+width-1, leftX+width-1,leftY,leftY+height-1);
  setColor(colorRGB,height);
  
}

void writeRightArrowRecOutline(uint8_t leftX, uint8_t leftY, uint8_t widthRec,uint8_t widthTotal, uint8_t height, uint32_t colorRGB){
  //Draw Start Shape:
  drawLine(leftX,leftY,leftX+widthRec-1, leftY, colorRGB); //Top Hor Line
  drawLine(leftX,leftY+height-1,leftX+widthRec-1, leftY+height-1, colorRGB); //Bottom Hor Line
  drawLine(leftX,leftY,leftX, leftY+height-1, colorRGB); //Left Vert. Line
  drawLine(leftX+widthRec-1,leftY,leftX+widthTotal-1, leftY+(height-1)/2, colorRGB); //Top Slanted Line
  drawLine(leftX+widthRec-1,leftY+height-1,leftX+widthTotal-1, leftY+(height-1)/2, colorRGB); //Bottom Slanted Line
}


void writeLeftArrowRecOutline(uint8_t leftX, uint8_t leftY, uint8_t widthRec,uint8_t widthTotal, uint8_t height, uint32_t colorRGB){
  //Draw Start Shape:
  drawLine(leftX+widthTotal-widthRec,leftY,leftX+widthTotal-1, leftY, colorRGB); //Top Hor Line
  drawLine(leftX+widthTotal-widthRec,leftY+height-1,leftX+widthTotal-1, leftY+height-1, colorRGB); //Bottom Hor Line
  drawLine(leftX+widthTotal-1,leftY,leftX+widthTotal-1, leftY+height-1, colorRGB); //Right Vert. Line
  drawLine(leftX+widthTotal-widthRec,leftY,leftX, leftY+(height-1)/2, colorRGB); //Top Slanted Line
  drawLine(leftX+widthTotal-widthRec,leftY+height-1,leftX, leftY+(height-1)/2, colorRGB); //Bottom Slanted Line
}





void writePixel(uint8_t xCoor, uint8_t yCoor, uint32_t colorRGB){
  setColumnRowRange(xCoor,xCoor,yCoor,yCoor);
  setColor(colorRGB,1);
}

void drawLine(int16_t xCoor1, int16_t yCoor1, int16_t xCoor2, int16_t yCoor2, uint32_t colorRGB ){
  
  int16_t yDif = yCoor2-yCoor1<0 ? yCoor1-yCoor2 : yCoor2-yCoor1;
  int16_t xDif = xCoor2-xCoor1<0 ? xCoor1-xCoor2 : xCoor2-xCoor1;
  
  
  if(yDif>xDif){//Find an x for each y pixel
    //Start with coordinate of lowest y coorndinate
    int16_t xStartPixelCoor = yCoor2>yCoor1?xCoor1:xCoor2;//Starting x coordinate
    int16_t yStartPixelCoor = yCoor2>yCoor1?yCoor1:yCoor2;//Starting y coordinate
    int16_t xEndPixelCoor = yCoor2>yCoor1?xCoor2:xCoor1;//Ending x coordinate
    int16_t yEndPixelCoor = yCoor2>yCoor1?yCoor2:yCoor1;//Ending y coordinate
    int16_t xPixelCoor =xStartPixelCoor;//Current x coordinate
    
    for(int16_t i = yStartPixelCoor;i<=yEndPixelCoor;i++){
      writePixel(xPixelCoor,i,colorRGB);
      xPixelCoor=xStartPixelCoor+(1+i-yStartPixelCoor)*(xEndPixelCoor-xStartPixelCoor)/(yEndPixelCoor-yStartPixelCoor);
    }
    
  }else{ //Find an y for each x pixel
    //Start with coordinate of lowest x coorndinate
    int16_t xStartPixelCoor = xCoor2>xCoor1?xCoor1:xCoor2;//Starting x coordinate
    int16_t yStartPixelCoor = xCoor2>xCoor1?yCoor1:yCoor2;//Starting y coordinate
    int16_t xEndPixelCoor = xCoor2>xCoor1?xCoor2:xCoor1;//Ending x coordinate
    int16_t yEndPixelCoor = xCoor2>xCoor1?yCoor2:yCoor1;//Ending y coordinate
    int16_t yPixelCoor =yStartPixelCoor;//Current y coordinate
    
    for(int16_t i = xStartPixelCoor;i<=xEndPixelCoor;i++){
      writePixel(i,yPixelCoor,colorRGB);
      yPixelCoor=yStartPixelCoor+(yEndPixelCoor-yStartPixelCoor)*(1+i-xStartPixelCoor)/(xEndPixelCoor-xStartPixelCoor);
    }
  }
}

void colorRGB32To8(uint32_t colorRGB, uint8_t *rgbArr){
  rgbArr[0]=(colorRGB>>16)&0xFF;//Red color
  rgbArr[1]=(colorRGB>>8)&0xFF;//Green color
  rgbArr[2]=(colorRGB>>0)&0xFF;//Blue color
  
}

void writeColorArray(uint32_t *colorArr, uint16_t pixelCount){
  //Send RAMWR command
  writeDisplayCommand(DISP_COM_RAMWR); 
  
  //Set the color array of color for all 160 pixels or pixelCount (if smaller)
  uint8_t limitPixel = pixelCount<200 ? pixelCount:200; 
  
  uint16_t pixelNum =0;
  for(uint32_t i = 0; i<pixelCount*3;i++){
    speedColorArr[i++]=(colorArr[pixelNum]>>16)&0xFF;//Red color
    speedColorArr[i++]=(colorArr[pixelNum]>>8)&0xFF;//Green color
    speedColorArr[i]=(colorArr[pixelNum]>>0)&0xFF;//Blue color
    pixelNum++;
  }
  
  //Send out pixel color every 160 pixels
  for (uint8_t i =0; i<pixelCount/200;i++){
    HAL_SPI_Transmit(&hspi1,speedColorArr,200*3,1000);
  }
  
  //Send out the rest of the pixel colors <160 pixels
  HAL_SPI_Transmit(&hspi1,speedColorArr,(pixelCount%200)*3,1000);
  
}

void writeText(Text_Info *tInfo){
  int16_t totalChrs = tInfo->numChrs;
  int16_t chrsRemain = totalChrs;
  uint8_t cWidth = tInfo->cWidth;
  while(chrsRemain>0){
    //Number of Chars to send at one time(color array can hold at max 200 pixels):
    uint8_t nChrs = MIN(200/(tInfo->cHeight*cWidth), chrsRemain);
    
    //Starting x pos. of data being written:
    uint8_t x = tInfo->x+(totalChrs-chrsRemain)*cWidth;
    //For each row:
    for(uint8_t row = 0; row<tInfo->cHeight; row++){
      //For each char. being sent:
      for(uint8_t c = 0; c<nChrs; c++){
        uint8_t charSending = tInfo->msg[totalChrs-chrsRemain+c];
        rowData = tInfo->fontArr[(uint32_t)charSending*tInfo->fCols+ row];
        //For the row, fill the colorArr with the correct pixel color in each col:
        for(uint8_t col = 0; col<cWidth;col++){
          if(rowData &0x01){//1->Pixel should be text color:
            tInfo->colorArr[nChrs*cWidth*row+c*cWidth+col]=tInfo->colorRGB;
          }else{ //0->Pixel should be background color
            tInfo->colorArr[nChrs*cWidth*row+c*cWidth+col]=tInfo->BGndFnc(x+c*cWidth+col ,tInfo->y+row);
          }
          rowData>>=1;
        }
      }
    }
    //Send the color data to display:
    setColumnRowRange(x, x+nChrs*cWidth-1, tInfo->y, tInfo->y+tInfo->cHeight-1);
    writeColorArray(tInfo->colorArr,nChrs*cWidth*tInfo->cHeight);
    chrsRemain-=nChrs;
  
  }
  
  
  
}

