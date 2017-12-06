#ifndef PROTOCOL_H
#define PROTOCOL_H

#include "uart.h"
#include "defineMiddle.h"

typedef struct {
    uint8_t addr;
    uint8_t func;
    uint8_t reg;
    uint8_t num;
    uint8_t* data;
    uint16_t crc;
} frame_t;

uint8_t serialGetFrame(frame_t* frame);
void serialClearFrame(frame_t* frame);
void serialSendFrame(frame_t* frame);

#endif