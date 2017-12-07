#include "task.h"
#include <stdlib.h>
#include <string.h>

frame_t frameTx;
frame_t frameRx;

void taskInit(void) {
    CLK_HSICmd(ENABLE);
    CLK_HSIPrescalerConfig(CLK_PRESCALER_HSIDIV1);
    CLK_SYSCLKConfig(CLK_PRESCALER_CPUDIV1);
    CLK_AdjustHSICalibrationValue(CLK_HSITRIMVALUE_0);

    serialInit();
    tickerInit();
    enableInterrupts();

    // ds18b20Init();
    buttonInit(BUTTON_1);
    buttonInit(BUTTON_2);
    buttonInit(BUTTON_3);
    buttonInit(WATER_SENSOR);

    serialClearFrame(&frameTx);
    serialClearFrame(&frameRx);

    regInit();
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

}

void taskDev2Reg(void) {
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
    //! Fake number
    regWrite(0x20, 0x0A);
    regWrite(0x21, 0x91);
}