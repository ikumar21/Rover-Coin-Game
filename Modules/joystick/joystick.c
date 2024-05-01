#include "joystick.h"

#define NUM_ADC_READS 2
#define AVG_COUNT 5

extern ADC_HandleTypeDef hadc1;
extern DMA_HandleTypeDef hdma_adc1;
extern uint8_t adcFlag; //Create adcFlag and put adcFlag = true; in dma interrupt

#define MATH_ABS(a) a<0?-a:a 

uint32_t adcResults[NUM_ADC_READS];
uint32_t finalADCResults[NUM_ADC_READS];


uint32_t allAdcResults[NUM_ADC_READS*AVG_COUNT];
uint8_t circularCounter = 0;

uint8_t arrayValid = false;

void runADC(uint32_t *finalADCResults){
  if(adcFlag == true){
    adcFlag = false;
    //Store ADC results in array to average
    for(uint8_t i = 0; i<NUM_ADC_READS; i++){
      allAdcResults[i*AVG_COUNT+circularCounter]=adcResults[i];
    }
    //Increment counter
    INCRE_CIRC_COUNTER(circularCounter, AVG_COUNT);
    
    //Finally enough data
    if(circularCounter==0)
      arrayValid=true;
      
    //Store avg result if enough data or store current value
    for(uint8_t i = 0; i<NUM_ADC_READS; i++){
      if(arrayValid){
        finalADCResults[i]=0;
        for(uint8_t j = 0; j<AVG_COUNT;j++){
          finalADCResults[i]+=allAdcResults[j+i*AVG_COUNT];
        }
          finalADCResults[i]=finalADCResults[i]/AVG_COUNT;
      }else{
        finalADCResults[i]= allAdcResults[circularCounter-1];
      }
    }
    //Start DMA again
    HAL_ADC_Start_DMA(&hadc1, adcResults, NUM_ADC_READS);
    
  }
  
  
}


void initADC(){
  HAL_ADC_Start_DMA(&hadc1, adcResults, NUM_ADC_READS);
}


void runJoystick(int16_t *joyVal){
  runADC(finalADCResults);
  joyVal[0]=finalADCResults[0]-2000;
  joyVal[1]=finalADCResults[1]-2000;
  
  //Create dead zone around joystick:
  if(joyVal[0]<100 && joyVal[0]>-100)
    joyVal[0]=0;
  if(joyVal[1]<100 && joyVal[1]>-100)
    joyVal[1]=0;
  
}

uint8_t getJoyDirCom(){
  runADC(finalADCResults);
  if(finalADCResults[0]>3000){
    return 1;
  }else if(finalADCResults[0]<1000){
    return 3;
  }else if(finalADCResults[1]>3000){
    return 2;
  }else if(finalADCResults[1]<1000){
    return 4;
  }else{
    return 0;
  }
  
  
  
}
//  float angle = normalized_atan2((float) finalADCResults[1]-2048.0, (float) finalADCResults[0]-2048.0);
//  *joyAngle = angle*90;
//
//}
//
//float normalized_atan2( float y, float x ){
//    static const uint32_t sign_mask = 0x80000000;
//    static const float b = 0.596227f;
//
//    // Extract the sign bits
//    uint32_t ux_s  = sign_mask & (uint32_t )x;
//    uint32_t uy_s  = sign_mask & (uint32_t )y;
//
//    // Determine the quadrant offset
//    float q = (float)( ( ~ux_s & uy_s ) >> 29 | ux_s >> 30 ); 
//
//    // Calculate the arctangent in the first quadrant
//    float bxy_a =  MATH_ABS( b * x * y );
//    float num = bxy_a + y * y;
//    float atan_1q =  num / ( x * x + bxy_a + num +0.0001);
//
//    // Translate it to the proper quadrant
//    uint32_t uatan_2q = (ux_s ^ uy_s) | (uint32_t )atan_1q;
//    return q + (float )uatan_2q;
//} 

