#include "protocol.h"
#include "stdlib.h"
#include "string.h"

uint8_t serialGetFrame(frame_t* frame) {
    if(uartAvailable() > 5) {
        frame->addr = uartRead();
        frame->func = uartRead();
        frame->reg = uartRead();
        frame->num = uartRead();
        frame->data = (uint8_t*)malloc(frame->num);
        int i;
        for(i = 0; i < frame->num; i++) {
            frame->data[i] = uartRead();
        }
        frame->crc = (uint16_t)uartRead();
        frame->crc = frame->crc << 8;
        frame->crc |= (uint16_t)uartRead();
        return EXIT_SUCCESS;
    }
    else {
        return EXIT_FAILURE;
    }
}

void serialClearFrame(frame_t* frame) {
    frame->addr = 0xFF;
    frame->func = 0xFF;
    frame->reg = 0xFF;
    frame->num = 0;
    free(frame->data);
    frame->crc = 0;
}

void serialSendFrame(frame_t* frame) {
    uartWriteByte(frame->addr);
    uartWriteByte(frame->func);
    int i;
    for(i = 0; i < frame->num; i++) {
        uartWriteByte(frame->data[i]);
    }
    uartWriteByte((uint8_t)(frame->crc >> 8));
    uartWriteByte((uint8_t)(frame->crc >> 0));
}
