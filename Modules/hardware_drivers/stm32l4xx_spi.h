#ifndef __STM32L4XX_SPI_H
#define __STM32L4XX_SPI_H

#include "main.h"

enum SPI_Status{
    SUCESSFUL,
    IN_USE 
};

typedef struct SPI_Handle_Data{
  GPIO_TypeDef *GPIOx;
  uint8_t sckPin;
  uint8_t mosiPin;
 
  //Alternate function selection:
  uint8_t sckAF; //SPI1_SCK: 5 (PA1)
  uint8_t mosiAF; //SPI1_MOSI: 5 (PA7)
  
  uint8_t baudDiv; //0: 2, 1: 4, ..., 7: 256 
  
}SPI_HandleData;


void Initialize_SPI(SPI_HandleData *spiHandle);
void SPI_Transmit_Poll(SPI_HandleData *spiInfo, uint8_t data[], uint32_t numBytes);

#endif