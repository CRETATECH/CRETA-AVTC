#include "protocol.h"
#include "stdlib.h"
#include "string.h"

uint16_t uCaclCrcByte(uint16_t currentCRC, uint8_t byte);

void serialInit(void) {
    uartInit(9600);
}

uint8_t serialGetFrame(frame_t* frame) {
    if(uartAvailable()) {
        uint16_t crc = 0xFFFF;
        uint16_t timeout = 0xFFFF;
        while((uartAvailable() == 0) & (timeout != 0)) timeout--;
        if(timeout == 0) return EXIT_FAILURE;
        frame->addr = uartRead();
        crc = uCaclCrcByte(crc, frame->addr);
        while((uartAvailable() == 0) & (timeout != 0)) timeout--;
        if(timeout == 0) return EXIT_FAILURE;
        frame->func = uartRead();
        crc = uCaclCrcByte(crc, frame->func);
        while((uartAvailable() == 0) & (timeout != 0)) timeout--;
        if(timeout == 0) return EXIT_FAILURE;
        frame->reg = uartRead();
        crc = uCaclCrcByte(crc, frame->reg);
        while((uartAvailable() == 0) & (timeout != 0)) timeout--;
        if(timeout == 0) return EXIT_FAILURE;
        frame->num = uartRead();
        crc = uCaclCrcByte(crc, frame->num);
        int i;
        if(frame->func == SERIAL_FUNC_WRITE) {
            frame->data = (uint8_t*)malloc(frame->num);
            for(i = 0; i < frame->num; i++) {
                while((uartAvailable() == 0) & (timeout != 0)) timeout--;
                if(timeout == 0) return EXIT_FAILURE;
                frame->data[i] = uartRead();
                crc = uCaclCrcByte(crc, frame->data[i]);
            }
        }
        while((uartAvailable() == 0) & (timeout != 0)) timeout--;
        if(timeout == 0) return EXIT_FAILURE;
        frame->crc = (uint16_t)uartRead();
        frame->crc = frame->crc << 8;
        while((uartAvailable() == 0) & (timeout != 0)) timeout--;
        if(timeout == 0) return EXIT_FAILURE;
        frame->crc |= (uint16_t)uartRead();
        if(crc == frame->crc)
            return EXIT_SUCCESS;
        else
            return EXIT_FAILURE;
    }
    else {
        return EXIT_FAILURE;
    }
}

void serialClearFrame(frame_t* frame) {
    frame->addr = 0xFF;
    frame->func = 0xFF;
    frame->reg = 0xFF;
    frame->num = 0xFF;
    free(frame->data);
    frame->crc = 0xFFFF;
}

void serialSendFrame(frame_t* frame) {
    uint16_t crc = 0xFFFF;
    uartWriteByte(frame->addr);
    crc = uCaclCrcByte(crc, frame->addr);
    uartWriteByte(frame->func);
    crc = uCaclCrcByte(crc, frame->func);
    int i;
    for(i = 0; i < frame->num; i++) {
        uartWriteByte(frame->data[i]);
        crc = uCaclCrcByte(crc, frame->data[i]);
    }
    frame->crc = crc;
    uartWriteByte((uint8_t)(frame->crc >> 8));
    uartWriteByte((uint8_t)(frame->crc >> 0));
}

uint16_t uCaclCrcByte(uint16_t currentCRC, uint8_t byte) {
    uint16_t nextCRC = currentCRC ^ (uint16_t)byte;
    uint8_t count;
    for(count = 0; count < 8; count++) {
        if((nextCRC & 0x0001) != 0) {
            nextCRC >>= 1;
            nextCRC ^= 0xA001;
        }
        else {
            nextCRC >>= 1;
        }
    }
    return nextCRC;
}
