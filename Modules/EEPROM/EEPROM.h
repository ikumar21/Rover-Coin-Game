#include "main.h"

void InitEEPROM();
void EReadData(uint8_t *storeData, uint16_t numBytes, uint16_t addr);
void EWriteData(uint8_t *data, uint16_t numBytes, uint16_t addr);

