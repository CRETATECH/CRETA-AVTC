#include "task.h"
#include <stdlib.h>
#include <string.h>

#define SLAVE_RELAY

frame_t frameTx;
frame_t frameRx;

void taskInit(void) {
    //! Set clock 16MHz
    CLK_HSICmd(ENABLE);
    CLK_HSIPrescalerConfig(CLK_PRESCALER_HSIDIV1);
    CLK_SYSCLKConfig(CLK_PRESCALER_CPUDIV1);
    CLK_AdjustHSICalibrationValue(CLK_HSITRIMVALUE_0);

    //! Init serial, ticker
    serialInit();
    tickerInit();
    enableInterrupts();

    //! Init led
    ledInit(LED_1);
    ledInit(LED_2);
    ledInit(LED_3);

#ifdef SLAVE_SENSOR
    ds18b20Init();
    buttonInit(WATER_SENSOR);
#endif
#ifdef SLAVE_RELAY
    deviceInit(DEVICE_1);
    deviceInit(DEVICE_2);
    deviceInit(DEVICE_3);
    deviceInit(DEVICE_4);
#endif

    //! Clear serial frame
    serialClearFrame(&frameTx);
    serialClearFrame(&frameRx);

    regInit();
    //! Set sensor slave address 0x02
#ifdef SLAVE_SENSOR
    regWrite(0x30, 0x02);
#endif
#ifdef SLAVE_RELAY
    regWrite(0x30, 0x01);
#endif
}

void taskSerialCmd() {
    uint8_t count;
    if(EXIT_SUCCESS != serialGetFrame(&frameRx)) {
        return;
    }
    //! Check Addr
    if(frameRx.addr != regRead(REG_ADDR)) {
        return;
    }
    //! Get function
    if(frameRx.func == SERIAL_FUNC_READ) {
        frameTx.addr = frameRx.addr;
        frameTx.func = frameRx.func;
        frameTx.num = frameRx.num;
        frameTx.data = (uint8_t*)malloc(frameTx.num);
        for(count = 0; count < frameRx.num; count++) {
            frameTx.data[count] = regRead(frameRx.reg + count);
        }
    }
    else if(frameRx.func == SERIAL_FUNC_WRITE) {
        frameTx.addr = frameRx.addr;
        frameTx.func = frameRx.func;
        frameTx.num = frameRx.num;
        frameTx.data = (uint8_t*)malloc(frameTx.num);
        for(count = 0; count < frameRx.num; count++) {
            regWrite(frameRx.reg + count, frameRx.data[count]);
            frameTx.data[count] = regRead(frameRx.reg + count);
        }
    }
    serialSendFrame(&frameTx);
    serialClearFrame(&frameTx);
    serialClearFrame(&frameRx);
}

void taskReg2Dev(void) {
#ifdef SLAVE_SENSOR
#endif
#ifdef SLAVE_RELAY
    uint8_t count = 0;
    uint8_t relay[4] = {
        DEVICE_1,
        DEVICE_2,
        DEVICE_3,
        DEVICE_4
    };
    for(count = 0; count < 4; count++) {
        if(regRead(0x10 + count) == 0x64) {
            deviceOn(relay[count]);
        }
        else if(regRead(0x10 + count) == 0x00) {
            deviceOff(relay[count]);
        }
    }
#endif
}

void taskDev2Reg(void) {
#ifdef SLAVE_SENSOR
    //! Update water sensor
    if(GPIO_HIGH == buttonReadLevel(WATER_SENSOR)) {
        regWrite(0x22, 0x00);
        regWrite(0x23, 0x64);
    }
    else {
        regWrite(0x22, 0x00);
        regWrite(0x23, 0x00);
    }
    //! Update DS18B20
    float t = ds18b20ReadTemp();
    if(200 != t) {
        uint16_t reg_t = (uint16_t)(t * 100.0);
        regWrite(0x20, (uint8_t)(reg_t >> 8));
        regWrite(0x21, (uint8_t)(reg_t >> 0));
    }
#endif
#ifdef SLAVE_RELAY
    //! Update relay status
    uint8_t count = 0;
    uint8_t relay[4] = {
        DEVICE_1,
        DEVICE_2,
        DEVICE_3,
        DEVICE_4
    };
    for(count = 0; count < 4; count++) {
        relay[count] = deviceRead(relay[count]);
    }
    for(count = 0; count < 4; count++) {
        if(relay[count] == GPIO_LOW) {
            regWrite(0x10 + count, 0x64);
        }
        else {
            regWrite(0x10 + count, 0x00);
        }
    }
#endif
}