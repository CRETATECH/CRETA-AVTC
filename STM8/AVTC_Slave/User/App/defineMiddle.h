#ifndef MIDDLE_H
#define MIDDLE_H

#include "eeprom.h"

#define REG_ADDR            0x30
#define REG_OUTPUT          0x10
#define REG_INPUT           0x20
#define REG_ID              0xF0
#define REG_HARDWARE        0xF1
#define REG_VERSION         0xF2
#define REG_STATUS          0xF3

#define REG_ADDR_EEPROM     EEPROM_START_ADDR + 4
#define REG_ID_EEPROM       EEPROM_START_ADDR + 0
#define REG_HARDWARE_EEPROM EEPROM_START_ADDR + 1
#define REG_VERSION_EEPROM  EEPROM_START_ADDR + 2
#define REG_STATUS_EEPROM   EEPROM_START_ADDR + 3

#define SERIAL_FUNC_READ    0x01
#define SERIAL_FUNC_WRITE   0x02
#define SERIAL_FUNC_ERROR   0x03

#endif