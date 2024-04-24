#include "EEPROM.h"

#define E_READ_INSTR 0x3
#define E_WRITE_INSTR 0x2
#define E_WREN_INSTR 0x6
#define E_WRSR_INSTR 0x1
#define E_RDSR_INSTR 0x5
#define E_WRDI_INSTR 0x4


#define false 0
#define true 1
uint8_t statReg = 0;
uint8_t zero = 0x0;
uint8_t adr[2];
//SCLK PA1, MOSI PA7, MISO PA6

#define E_SCK_PIN GPIOA, GPIO_PIN_11
#define E_MOSI_PIN GPIOA, GPIO_PIN_8
#define E_MISO_PIN GPIOB, GPIO_PIN_1
#define CS_EEPROM_PIN GPIOA, GPIO_PIN_12

void SendInstr(uint8_t instr);
void SPI_Transmit(uint8_t *pData, uint8_t numBytes);
void SPI_Receive(uint8_t *sData, uint8_t numBytes);
void ReadStatusReg();

void SendInstr(uint8_t instr){
  SPI_Transmit(&instr, 1);
}
void ReadStatusReg(){
  //Bring CS pin low
  HAL_GPIO_WritePin(CS_EEPROM_PIN, false);
  SendInstr(E_RDSR_INSTR);
  SPI_Receive(&statReg,1);
  HAL_GPIO_WritePin(CS_EEPROM_PIN, true);
}              
void InitEEPROM(){
  HAL_GPIO_WritePin(CS_EEPROM_PIN, false);//bring CS pin low
  SendInstr(E_WRSR_INSTR);
  //Send 0 to write status register to ensure nothing is write protected
  SPI_Transmit(&zero, 1);
  //Bring CS pin back to high
   HAL_GPIO_WritePin(CS_EEPROM_PIN, true);
}



void EWriteData(uint8_t *data, uint16_t numBytes, uint16_t addr){
  //Addr: 0 - 0x03FF
  //For now: max numBytes = 16, addr's 4 LSBs assumed to be 0000 (start of page)
  
  //Bring CS pin low
  HAL_GPIO_WritePin(CS_EEPROM_PIN, false);
  
  //Send Write Enable Instruction:
  SendInstr(E_WREN_INSTR);
  HAL_GPIO_WritePin(CS_EEPROM_PIN, true);
  
  //Send Write Instruction
  HAL_GPIO_WritePin(CS_EEPROM_PIN, false);
  SendInstr(E_WRITE_INSTR);
  
  //Send read addr:
  adr[0] = 0xFF & (addr>>8);
  adr[1] = 0xFF & addr;
  SPI_Transmit(adr, 2);
  
  //Send data:
  SPI_Transmit(data,numBytes);
  //Bring CS pin High
  HAL_GPIO_WritePin(CS_EEPROM_PIN, true);
  
  //Wait until write is finished before leaving:
  ReadStatusReg();
  while(statReg&0x1){
    ReadStatusReg();
  }
  
  //Bring CS pin High
  HAL_GPIO_WritePin(CS_EEPROM_PIN, true);
  
}

void EReadData(uint8_t *storeData, uint16_t numBytes, uint16_t addr){
  //Bring CS pin low
  HAL_GPIO_WritePin(CS_EEPROM_PIN, false);
  
  //Read Instruction:
  SendInstr(E_READ_INSTR);
  //Send read addr:
  adr[0] = 0xFF & (addr>>8);
  adr[1] = 0xFF & addr;
  SPI_Transmit(adr, 2);
  //Get Data:
  SPI_Receive(storeData, numBytes);
  
  //Bring CS pin low
  HAL_GPIO_WritePin(CS_EEPROM_PIN, true);
  
}

void SPI_Transmit(uint8_t *pData, uint8_t numBytes){
  for(uint8_t i = 0; i <numBytes;i++){
    for(int8_t j = 7; j >= 0; j--){
      HAL_GPIO_WritePin(E_MOSI_PIN, ((*pData)>>j)&0x1);
      HAL_GPIO_WritePin(E_SCK_PIN, true);
      HAL_GPIO_WritePin(E_SCK_PIN, false);
    }
    pData++;
  }
}

void SPI_Receive(uint8_t *sData, uint8_t numBytes){
  uint8_t data =0;
  for(uint8_t i = 0; i <numBytes;i++){
    for(int8_t j = 7; j >= 0; j--){
      HAL_GPIO_WritePin(E_SCK_PIN, true);
      data |= HAL_GPIO_ReadPin(E_MISO_PIN)<<j;
      HAL_GPIO_WritePin(E_SCK_PIN, false);
    }
    *sData++ = data;
    data = 0;
  }
}

