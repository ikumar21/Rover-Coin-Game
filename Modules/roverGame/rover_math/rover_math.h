#ifndef __ROVER_MATH_H
#define __ROVER_MATH_H
#include <inttypes.h>

float SinSim(uint16_t angle);
float CosSim(uint16_t angle);
void MultMatrix(float* ans, float M[2][2], float *V);

uint32_t randSTM32L4x();

#endif