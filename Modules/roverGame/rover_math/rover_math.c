#include "rover_math.h"
#include "main.h"

uint32_t getRandNum(uint32_t min, uint32_t max){
  return min + randSTM32L4x() / (0xFFFFFFFF / (max - min + 1) + 1);
}

uint32_t randSTM32L4x(){
  //Enable RNG
  RNG->CR |= 0x1<<2;
  uint32_t statusReg = RNG->SR;
  //Keep waiting until data is ready, no faulty sequence, and RNG clock is fast enough 
  while((RNG->SR>>5)&0x3 || !(RNG->SR&0x1)){
    RNG->SR&=~(0x3); //Reset error interrupt statuses
    RNG->SR&=~(0x1); //Reset DRDY->data ready
  };
  //Disable RNG:
  RNG->CR &= ~(0x1<<2);
  return RNG->DR;
}

float SinSim(uint16_t angle){
  uint8_t O = angle%2;//Odd
  uint8_t N = angle>180; //Negative
  uint8_t A = angle==90 || angle ==270;
    
  return (N*-1+!N)*(0.7071*O+A);
}

float CosSim(uint16_t angle){
  uint8_t O = angle%2;//Odd
  uint8_t N = angle>90 && angle<270; //Negative
  uint8_t B = angle==0 || angle ==180;
    
  return (N*-1+!N)*(0.7071*O+B);
}

void MultMatrix(float* ans, float M[2][2], float *V){
  ans[0]=M[0][0]*V[0]+M[0][1]*V[1];
  ans[1]=M[1][0]*V[0]+M[1][1]*V[1];
}