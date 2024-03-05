#include "coin_obj.h"


uint32_t coinColors[13] = {0xF8F8F8,0xFC0000,0xFC7A00,0xFCFC00,0x7AFC00, 
                            0x00FC00,0x00FC7A,0x00FCFC,0x007AFC,
                            0x0000FC,0x7A00FC,0xFC00FC,0xFC007A};

uint8_t coin1[5][5] = {
{8,8,7,6,6},
{8,7,7,6,5},
{1,1,0,5,5},
{1,2,3,4,4},
{2,2,3,3,4}};


uint8_t orgCoin1[5][5] = {
{8,8,7,6,6},
{8,7,7,6,5},
{1,1,0,5,5},
{1,2,3,4,4},
{2,2,3,3,4}};

uint8_t *coinColorTypes[2]={(uint8_t *) coin1, (uint8_t *) orgCoin1};

void setCoinColor(uint8_t arr[], uint8_t orArr[], uint8_t color){
  for(uint8_t i = 0; i<25;i++){
    if(arr[i] !=0){
      arr[i]=(orArr[i]+color-1)%12+1;
    }
  }
}