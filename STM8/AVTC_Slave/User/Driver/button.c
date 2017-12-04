/*******************************************************************************
  * @filename   : button.c
  * @author     : HgN
  * @last update: December 4th, 2017
  */
/******************************************************************************/

/******************************************************************************/
/* INCLUDE */
/******************************************************************************/
#include "button.h"
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
uint16_t _buttonPort[4] = {BUTTON_1_PORT,
                          BUTTON_2_PORT,
                          BUTTON_2_PORT,
                          WATER_SENSOR_PORT};
uint8_t _buttonPin[4] = {BUTTON_1_PIN,
                         BUTTON_2_PIN,
                         BUTTON_3_PIN,
                         WATER_SENSOR_PIN};
/******************************************************************************/
/* LOCAL FUNCTION PROTOTYPES */
/******************************************************************************/

/******************************************************************************/
/* PUBLIC FUNCTIONS */
/******************************************************************************/
/**
 * @brief       Init a button
 * @param       pPin
 * @retval      EXIT_SUCCESS
 *              EXIT_FAILURE
 */
int buttonInit(uint8_t pButton) {
    uint16_t vPort = _buttonPort[pButton];
    uint8_t vPin = _buttonPin[pButton];
    gpioPinMode(vPort, vPin, GPIO_INPUT);
    return EXIT_SUCCESS;
}

bool buttonReadPressed(uint8_t pButton) {
    uint16_t vPort = _buttonPort[pButton];
    uint8_t vPin = _buttonPin[pButton];
    static uint8_t vLastStatus[4] = {GPIO_HIGH, GPIO_HIGH, GPIO_HIGH, GPIO_HIGH};
    static uint32_t vLastPressed[4] = {0, 0, 0, 0};
    static bool vPressed[4] = {false, false, false, false};
    uint8_t reading = gpioReadPin(vPort, vPin);
    if(reading == GPIO_LOW) {
        if(reading != vLastStatus[pButton]) {
            vLastPressed[pButton] = tickerMillis();
        }
        else if((tickerMillis() - vLastPressed[pButton]) > 150) {
            if(vPressed[pButton] == false) {
                vPressed[pButton] = true;
                vLastStatus[pButton] = reading;
                return true;
            }
        }
    }
    else {
        vPressed[pButton] = false;
    }
    vLastStatus[pButton] = reading;
    return false;
}

uint8_t buttonReadLevel(uint8_t pButton) {
    uint16_t vPort = _buttonPort[pButton];
    uint8_t vPin = _buttonPin[pButton];
    static uint32_t vFilter = 0;
    static uint32_t vTime = 0;
    static uint8_t vLastStatus[4] = {GPIO_HIGH, GPIO_HIGH, GPIO_HIGH, GPIO_HIGH};
    uint8_t reading = gpioReadPin(vPort, vPin);
    if(reading != vLastStatus[pButton]) {
        if((tickerMillis() - vTime) > 100) {
            vFilter++;
            vTime = tickerMillis();
        }
    }
    else {
        vFilter = 0;
    }
    if(vFilter > 3) {
        vFilter = 0;
        vLastStatus[pButton] = reading;
    }
    return vLastStatus[pButton];
}