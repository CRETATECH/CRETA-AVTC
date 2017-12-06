/**
 * @brief: project CRETA - AVTC, use ESP8266 to connect to server and
 *         communicate with Slave
 * @author: nghiaphung
 */
 /*************************************************/
/*                  INCLUDE                      */
/*************************************************/
#include <ESP8266WiFi.h>
#include "param.h"
#include "UART.h"
#include "state.h"
#include "device.h"
#include "mqtt.h"
#include "timer.h"
#include "protocol.h"
/*************************************************/
/*             FUNCTION PROTOTYPE                */
/*************************************************/
void hwConfig (void);
void mqttConfig(void);
/*************************************************/
/*                  MAIN FUNCTION                */
/*************************************************/

uint8_t cac[6];


void setup() {
  hwConfig();
  mqttConfig();
  Timer_Init();
}

void loop() {
  stateMachine();
}

/**
 * @brief:  config hardware module
 * @detail: setup GPIO, UART 8E1
 */
void hwConfig (void)
{
  UART_Init();
  Button_Init();
  Led_Init();
}

void mqttConfig(void)
{
  mqtt_Init();
}

