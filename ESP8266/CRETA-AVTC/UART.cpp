#include <ESP8266WiFi.h>
#include "UART.h"
void UART_Init (void)
{
  Serial.begin(9600, SERIAL_8E1);
}

void serialEvent(void)
{
  while (Serial.available())
  {
    uint8_t inChar = (uint8_t)Serial.read();
  }
}

