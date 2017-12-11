#ifndef TASK_H
#define TASK_H

#include "protocol.h"
#include "reg.h"
#include "led.h"
#include "device.h"
#include "button.h"
#include "ds18b20.h"
#include "ticker.h"
#include "defineApp.h"

void taskInit(void);
void taskSerialCmd();
void taskReg2Dev(void);
void taskDev2Reg(void);

#endif