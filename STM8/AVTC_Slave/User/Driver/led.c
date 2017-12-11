/*******************************************************************************
  * @filename   : led.c
  * @author     : HgN
  * @last update: December 11th, 2017
  */
/******************************************************************************/

/******************************************************************************/
/* INCLUDE */
/******************************************************************************/
#include "led.h"
#include <stdlib.h>

/******************************************************************************/
/* LOCAL TYPEDEFS */
/******************************************************************************/

/******************************************************************************/
/* LOCAL DEFINES */
/******************************************************************************/

/******************************************************************************/
/* PUBLIC VARIABLES */
/******************************************************************************/
uint16_t _ledPort[3] = {
    LED_1_PORT,
    LED_2_PORT,
    LED_3_PORT
};

uint8_t _ledPin[3] = {
    LED_1_PIN,
    LED_2_PIN,
    LED_3_PIN
};
/******************************************************************************/
/* LOCAL FUNCTION PROTOTYPES */
/******************************************************************************/

/******************************************************************************/
/* PUBLIC FUNCTIONS */
/******************************************************************************/
/**
 * @brief       Init led
 * @param       Led number
 * @reval       None
 */
void ledInit(uint8_t pLed) {
    uint16_t vPort = _ledPort[pLed];
    uint8_t vPin = _ledPin[pLed];
    gpioPinMode(vPort, vPin, GPIO_OUTPUT);
}

/**
 * @brief       Turn led on
 * @param       Led number
 * @reval       None
 */
void ledOn(uint8_t pLed) {
    uint16_t vPort = _ledPort[pLed];
    uint8_t vPin = _ledPin[pLed];
    gpioWritePin(vPort, vPin, GPIO_LOW);
}

/**
 * @brief       Turn led off
 * @param       Led number
 * @reval       None
 */
void ledOff(uint8_t pLed) {
    uint16_t vPort = _ledPort[pLed];
    uint8_t vPin = _ledPin[pLed];
    gpioWritePin(vPort, vPin, GPIO_HIGH);
}
/**
 * @brief       Read device status
 * @param       Device number
 * @reval       GPIO_LOW/GPIO_HIGH
 */
uint8_t ledRead(uint8_t pLed) {
    uint16_t vPort = _ledPort[pLed];
    uint8_t vPin = _ledPin[pLed];
    return gpioReadPin(vPort, vPin);
}