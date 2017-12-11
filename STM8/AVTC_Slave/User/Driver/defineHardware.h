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

#define BUTTON_1_PORT       GPIO_PORTD
#define BUTTON_1_PIN        GPIO_PIN_2
#define BUTTON_2_PORT       GPIO_PORTC
#define BUTTON_2_PIN        GPIO_PIN_6
#define BUTTON_3_PORT       GPIO_PORTC
#define BUTTON_3_PIN        GPIO_PIN_7
#define WATER_SENSOR_PORT   GPIO_PORTC
#define WATER_SENSOR_PIN    GPIO_PIN_4

#define LED_1               (uint8_t)0x00
#define LED_2               (uint8_t)0x01
#define LED_3               (uint8_t)0x02

#define LED_1_PORT          GPIO_PORTC
#define LED_1_PIN           GPIO_PIN_5
#define LED_2_PORT          GPIO_PORTC
#define LED_2_PIN           GPIO_PIN_6
#define LED_3_PORT          GPIO_PORTC
#define LED_3_PIN           GPIO_PIN_7

#define DEVICE_1            (uint8_t)0x00
#define DEVICE_2            (uint8_t)0x01
#define DEVICE_3            (uint8_t)0x02
#define DEVICE_4            (uint8_t)0x03

#define DEVICE_1_PORT        GPIO_PORTC
#define DEVICE_1_PIN         GPIO_PIN_4
#define DEVICE_2_PORT        GPIO_PORTC
#define DEVICE_2_PIN         GPIO_PIN_3
#define DEVICE_3_PORT        GPIO_PORTD
#define DEVICE_3_PIN         GPIO_PIN_2
#define DEVICE_4_PORT        GPIO_PORTD
#define DEVICE_4_PIN         GPIO_PIN_3


#define DS18B20_PORT_INIT   GPIO_PORTB
#define DS18B20_PORT        GPIOB
#define DS18B20_PIN         GPIO_PIN_5

#endif