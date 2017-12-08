#include "ds18b20.h"

inline void ds18b20WriteHigh(void);
inline void ds18b20WriteLow(void);
inline uint8_t ds18b20Read(void);
void ds18b20Reset(void);
inline void ds18b20Delay(uint32_t pUs);
void ds18b20WriteBit(uint8_t bit);
uint8_t ds18b20ReadBit(void);
void ds18b20WriteByte(uint8_t byte);
uint8_t ds18b20ReadByte(void);

void ds18b20Init() {
    ds18b20Reset();
    ds18b20WriteByte(0xCC);
    ds18b20WriteByte(0x4E);
    ds18b20WriteByte(0x64);
    ds18b20WriteByte(0x00);
    ds18b20WriteByte(0x7F);
}

float ds18b20ReadTemp() {
    ds18b20Reset();
    ds18b20WriteByte(0xCC);
    ds18b20WriteByte(0x44);
    while(RESET == ds18b20ReadBit());
    ds18b20Reset();
    ds18b20WriteByte(0xCC);
    ds18b20WriteByte(0xBE);
    uint8_t temp1 = ds18b20ReadByte();
    uint8_t temp2 = ds18b20ReadByte();
    ds18b20Reset();
    uint16_t temp = (uint16_t)(temp2 << 8) | (uint16_t)temp1;
    if(temp == 0xFFFF) return 200.0;
    else {
        return temp/16.0;
    }
}

void ds18b20Reset(void) {
    GPIOD->ODR |= (uint8_t)GPIO_PIN_3;
    ds18b20Delay(2);
    GPIOD->ODR &= (uint8_t)(~GPIO_PIN_3);
    ds18b20Delay(120);
    GPIOD->ODR |= (uint8_t)GPIO_PIN_3;
    ds18b20Delay(10);
    while(ds18b20Read() == GPIO_HIGH);
    while(ds18b20Read() == GPIO_LOW);
}

void ds18b20WriteByte(uint8_t byte) {
    int count = 0;
    for(count = 0; count < 8; count++) {
        ds18b20WriteBit(byte & 0x01);
        byte >>= 1;
    }
}

uint8_t ds18b20ReadByte(void) {
    int count = 0;
    int byte = 0;
    for(count = 0; count < 8; count++) {
        byte >>= 1;
        if(ds18b20ReadBit() != RESET)
            byte |= 0x80;
    }
    return byte;
}

uint8_t ds18b20ReadBit(void) {
    uint8_t bit;
    GPIOD->ODR |= (uint8_t)GPIO_PIN_3;
    ds18b20Delay(1);
    GPIOD->ODR &= (uint8_t)(~GPIO_PIN_3);
    int pUs = 2; while(pUs--) asm("NOP");
    GPIOD->ODR |= (uint8_t)GPIO_PIN_3;
    pUs = 3; while(pUs--) asm("NOP");
    bit = ((BitStatus)(GPIOD->IDR & (uint8_t)GPIO_PIN_3));
    ds18b20Delay(30);
    return bit;
}

void ds18b20WriteBit(uint8_t bit) {
    GPIOD->ODR |= (uint8_t)GPIO_PIN_3;
    ds18b20Delay(1);
    GPIOD->ODR &= (uint8_t)(~GPIO_PIN_3);
    ds18b20Delay(1);
    if(bit != 0x00)
        GPIOD->ODR |= (uint8_t)GPIO_PIN_3;
    else if(bit == 0x01)
        GPIOD->ODR &= (uint8_t)~GPIO_PIN_3;
    ds18b20Delay(13);
    GPIOD->ODR |= (uint8_t)GPIO_PIN_3;
}

void ds18b20WriteHigh() {
    GPIOD->ODR |= (uint8_t)GPIO_PIN_3;
}

void ds18b20WriteLow() {
    GPIOD->ODR &= (uint8_t)(~GPIO_PIN_3);
}

void ds18b20Delay(uint32_t pUs) {
    while(pUs--) asm("NOP");
}

uint8_t ds18b20Read(void) {
    return ((BitStatus)(GPIOD->IDR & (uint8_t)GPIO_PIN_3));
}