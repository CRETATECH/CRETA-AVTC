/*******************************************************************************
  * @filename   : defineHardware.h
  * @author     : HgN
  * @last update: December 4th, 2017
  */
/******************************************************************************/

#ifndef DEFINE_HARDWARE_H
#define DEFINE_HARDWARE_H

/******************************************************************************/
/* INCLUDE */
/******************************************************************************/

/******************************************************************************/
/* PUBLIC TYPEDEFS */
/******************************************************************************/

/******************************************************************************/
/* PUBLIC DEFINES */
/******************************************************************************/
#define BUTTON_1            (uint8_t)0x00
#define BUTTON_2            (uint8_t)0x01
#define BUTTON_3            (uint8_t)0x02
#define WATER_SENSOR        (uint8_t)0x03

#define BUTTON_1_PORT       GPIO_PORTC
#define BUTTON_1_PIN        GPIO_PIN_5
#define BUTTON_2_PORT       GPIO_PORTC
#define BUTTON_2_PIN        GPIO_PIN_6
#define BUTTON_3_PORT       GPIO_PORTC
#define BUTTON_3_PIN        GPIO_PIN_7
#define WATER_SENSOR_PORT   GPIO_PORTD
#define WATER_SENSOR_PIN    GPIO_PIN_3

#endif