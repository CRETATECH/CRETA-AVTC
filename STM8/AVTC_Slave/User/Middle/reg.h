#ifndef REG_H
#define REG_H

#include "eeprom.h"
#include "defineMiddle.h"

void regInit(void);
void regWrite(uint8_t pAddr, uint8_t pData);
uint8_t regRead(uint8_t pAddr);


#endif