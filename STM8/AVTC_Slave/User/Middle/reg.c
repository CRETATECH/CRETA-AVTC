#include "reg.h"

struct {
    uint8_t id;
    uint8_t hardware;
    uint8_t version;
    uint8_t status;
} REG_RO;

struct {
    uint8_t output[16];
    uint8_t input[16];
} REG_RW;

struct {
    uint8_t address;
} REG_CONFIG;

void regInit(void) {
    eepromInit();
    eepromRead(REG_ID_EEPROM, &REG_RO.id, 1);
    eepromRead(REG_HARDWARE_EEPROM, &REG_RO.hardware, 1);
    eepromRead(REG_VERSION_EEPROM, &REG_RO.version, 1);
    eepromRead(REG_STATUS_EEPROM, &REG_RO.status, 1);
    int count;
    for(count = 0; count < 16; count++) {
        REG_RW.output[count] = 0xFF;
        REG_RW.input[count] = 0xFF;
    }
    eepromRead(REG_ADDR_EEPROM, &REG_CONFIG.address, 1);
}

void regWrite(uint8_t pAddr, uint8_t pData) {
    if((pAddr & 0xF0) == 0x10) {
        REG_RW.output[pAddr & 0x0F] = pData;
    }
    else if((pAddr & 0xF0) == 0x20) {
        REG_RW.input[pAddr & 0x0F] = pData;
    }
    else if(pAddr == 0x30) {
        REG_CONFIG.address = pData;
        eepromWrite(REG_ADDR_EEPROM, &REG_CONFIG.address, 1);
    }
    else if(pAddr == 0xF0) {
        REG_RO.id = pData;
        eepromWrite(REG_ID_EEPROM, &REG_RO.id, 1);
    }
    else if(pAddr == 0xF1) {
        REG_RO.hardware = pData;
        eepromWrite(REG_HARDWARE_EEPROM, &REG_RO.hardware, 1);
    }
    else if(pAddr == 0xF2) {
        REG_RO.version = pData;
        eepromWrite(REG_VERSION_EEPROM, &REG_RO.version, 1);
    }
    else if(pAddr == 0xF3) {
        REG_RO.status = pData;
        eepromWrite(REG_STATUS_EEPROM, &REG_RO.status, 1);
    }
}

uint8_t regRead(uint8_t pAddr) {
    if((pAddr & 0xF0) == 0x10) {
        return REG_RW.output[pAddr & 0x0F];
    }
    else if((pAddr & 0xF0) == 0x20) {
        return REG_RW.input[pAddr & 0x0F];
    }
    else if(pAddr == 0xF0) {
        return REG_RO.id;
    }
    else if(pAddr == 0xF1) {
        return REG_RO.hardware;
    }
    else if(pAddr == 0xF2) {
        return REG_RO.version;
    }
    else if(pAddr == 0xF3) {
        return REG_RO.status;
    }
    else if(pAddr == 0x30) {
        return REG_CONFIG.address;
    }
    else {
        return 0xFF;
    }
}