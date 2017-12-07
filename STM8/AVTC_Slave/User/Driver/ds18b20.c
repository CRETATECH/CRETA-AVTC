/*******************************************************************************
  * @filename   : ds18b20.c
  * @author     : HgN
  * @last update: December 4th, 2017
  */
/******************************************************************************/

/******************************************************************************/
/* INCLUDE */
/******************************************************************************/
#include "ds18b20.h"
#include <stdlib.h>
#include <stdbool.h>

/******************************************************************************/
/* LOCAL TYPEDEFS */
/******************************************************************************/

/******************************************************************************/
/* LOCAL DEFINES */
/******************************************************************************/

/******************************************************************************/
/* PUBLIC VARIABLES */
/******************************************************************************/

/******************************************************************************/
/* LOCAL FUNCTION PROTOTYPES */
/******************************************************************************/
void ds18b20Reset(void);
void ds18b20WriteByte(uint8_t pByte);
uint8_t ds18b20ReadByte(void);
inline void ds18b20SetOutput(void);
inline void ds18b20SetInput(void);
inline void ds18b20WriteLow(void);
inline void ds18b20WriteHigh(void);
inline uint8_t ds18b20Read(void);
inline void ds18b20DelayUs(uint32_t pUs);
/******************************************************************************/
/* PUBLIC FUNCTIONS */
/******************************************************************************/
void ds18b20Init() {
    ds18b20Reset();
    ds18b20WriteByte(0xCC);
    ds18b20WriteByte(0x44);
    int read = 0;
    while(read == 0) {
        ds18b20SetOutput();
        ds18b20WriteLow();
        ds18b20DelayUs(3);
        ds18b20SetInput();
        ds18b20DelayUs(3);
        if(GPIO_HIGH == ds18b20Read())
            read = 1;
        ds18b20DelayUs(8);
    }
}

float ds18b20ReadTemp() {
}


/******************************************************************************/
/* PRIVATE FUNCTIONS */
/******************************************************************************/
void ds18b20Reset(void) {
    ds18b20SetOutput();
    ds18b20WriteHigh();
    ds18b20WriteLow();
    ds18b20DelayUs(100);
    ds18b20SetInput();
    while(GPIO_LOW != ds18b20Read());
    while(GPIO_HIGH != ds18b20Read());
}

void ds18b20WriteByte(uint8_t pByte) {
    ds18b20SetOutput();
    uint8_t count = 0;
    for(count = 0; count < 8; count++) {
        ds18b20WriteHigh();
        ds18b20DelayUs(1);
        ds18b20WriteLow();
        ds18b20DelayUs(1);
        if((pByte & 0x01) == 0x01) {
            ds18b20WriteHigh();
        }
        ds18b20DelayUs(8);
        pByte = pByte>>1;
    }
}

uint8_t ds18b20ReadByte(void) {
    uint8_t count = 0;
    uint8_t byte = 0;
    uint32_t delay;
    for(count = 0; count < 8; count++) {
        ds18b20SetOutput();
        ds18b20WriteLow();
        delay = 2; while(delay--);
        ds18b20SetInput();
        byte = byte >> 1;
        delay = 8; while(delay--);
        if(GPIO_HIGH == ds18b20Read()) {
            byte |= 0x80;
        }
        delay = 100; while(delay--);
    }
    return byte;
}

void ds18b20SetOutput(void) {
    gpioPinMode(DS18B20_PORT, DS18B20_PIN, GPIO_OUTPUT);
}

void ds18b20SetInput(void) {
    gpioPinMode(DS18B20_PORT, DS18B20_PIN, GPIO_INPUT);
}

void ds18b20WriteLow(void) {
    gpioWritePin(DS18B20_PORT, DS18B20_PIN, GPIO_LOW);
}

void ds18b20WriteHigh(void) {
    gpioWritePin(DS18B20_PORT, DS18B20_PIN, GPIO_HIGH);
}

uint8_t ds18b20Read(void) {
    return gpioReadPin(DS18B20_PORT, DS18B20_PIN);
}

void ds18b20DelayUs(uint32_t pUs) {
    for(; pUs > 0; pUs--) {
        asm("NOP");
    }
}