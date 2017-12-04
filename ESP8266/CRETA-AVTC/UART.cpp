/*************************************************/
/*                  INCLUDE                      */
/*************************************************/
#include <ESP8266WiFi.h>
#include "UART.h"
/*************************************************/
/*                  LOCAL  VARIABLE              */
/*************************************************/

/*************************************************/
/*             FUNCTION PROTOTYPE                */
/*************************************************/

/*************************************************/
/*                  MAIN FUNCTION                */
/*************************************************/
void UART_Init (void)
{
  Serial.begin(9600, SERIAL_8E1);
}

void UART_SendBuffer(uint8_t* buff, int len)
{
  for (int i = 0; i< len; i++)
    Serial.write(buff[i]);
}

void serialEvent(void)
{
  while (Serial.available())
  {
    uint8_t inChar = (uint8_t)Serial.read();
  }
}

