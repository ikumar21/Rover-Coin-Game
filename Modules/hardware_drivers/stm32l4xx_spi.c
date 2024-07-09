//SPI1: SCK - PA1, MOSI - PA7

#define SPIX SPI1

#include "main.h"
#include "stm32l4xx_spi.h"

uint8_t testData = 0xEE;

void Initialize_SPI_Ports(){
  //Initialize clock for GPIO port a 
  RCC->AHB2ENR |= 0x1;
  //Wait two clock cycles:
  __asm volatile("NOP");
  __asm volatile("NOP");
  
  //Enable SPI clock
  if(SPIX==SPI1){
    RCC->APB2ENR |= 0x1<<12;
  }else{ //SPI3
    RCC->APB1ENR1 |= 0x1<<15;
  }
}

void Initialize_SPI(SPI_HandleData *spiInfo){
  
  //Initialize ports
  Initialize_SPI_Ports();
  
  GPIO_TypeDef *GPIOS = spiInfo->GPIOx;
  
  //Set MOSI, SCK to SPI1 (GPIO alternate function)
  //Set MOSI, SCK to GPIO mode to AF
  GPIOS->MODER &= ~( (0x3<<(spiInfo->mosiPin<<1))  | (0x3<<(spiInfo->sckPin<<1)) );
  GPIOS->MODER |= (0x2<<(spiInfo->mosiPin<<1))  | (0x2<<(spiInfo->sckPin<<1));
  
  //SET SCK PIN to AF selection (SPIx_SCK)
  GPIOS->AFR[spiInfo->sckPin>7] &= ~(0xF<<((spiInfo->sckPin%8)<<2));
  GPIOS->AFR[spiInfo->sckPin>7] |= (spiInfo->sckAF << ((spiInfo->sckPin%8)<<2));

  //SET MOSI PIN to AF selection (SPIx_MOSI)
  GPIOS->AFR[spiInfo->mosiPin>7] &= ~(0xF<<((spiInfo->mosiPin%8)<<2));
  GPIOS->AFR[spiInfo->mosiPin>7] |= (spiInfo->mosiAF << ((spiInfo->mosiPin%8)<<2));
  
  //Set baud rate
  SPIX->CR1 &= ~(0x7<<3);
  SPIX->CR1 |= (spiInfo->baudDiv<<3);
  
  //Set CPOL/CPHA to 0
  SPIX->CR1 &= ~(0x3);
 
  //Configure SPI as simplex, transmit only (half-duplex, constant transmit)
  SPIX->CR1 |= 0x3<<14;
    
  //Configure SPI NSS as software
  SPIX->CR1 |= 0x1<<9;
  SPIX->CR1 |= 0x1<<8;
  
  //Configure device as master
  SPIX->CR1 |= 0x1<<2;
  
  //Set 8 bit SPI data:
  SPIX->CR2 &=  ~(0xF << 8);
  SPIX->CR2 |=  (0x7 << 8);
  
  //Allow 8 bit only SPI:
  SPIX->CR2 |= 0x1<<12;
  
  //Enable SPI
  SPIX->CR1 |= 0x1<<6;
}

void SPI_Transmit_Poll(SPI_HandleData *spiInfo, uint8_t data[], uint32_t numBytes){
 
  uint32_t i = 0;
  while(i < numBytes){
    if(((SPIX->SR>>1) & 0x1)){ //If not busy, send data
      if(i+1==numBytes){ //Only one byte
        *((volatile uint8_t *)&SPIX->DR) = data[i++]; 
      }else{ //Put 2 bytes of data in TX FIFO 
        SPIX->DR = data[i+1] <<8 | data[i];
        i++;i++;
      }
    }
  }
  
  while((SPIX->SR>>11) &0x3){};
}

