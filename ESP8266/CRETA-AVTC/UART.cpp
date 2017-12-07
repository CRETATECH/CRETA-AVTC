/*************************************************/
/*                  INCLUDE                      */
/*************************************************/
#include <ESP8266WiFi.h>
#include "UART.h"
#include "protocol.h"
/*************************************************/
/*                  LOCAL  VARIABLE              */
/*************************************************/
uint8_t recvbuff[20];
uint8_t byteCount = 0;

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


/**
 * @brief: check hw serial buffer, must be called regularly
 */
void UART_Event (void)
{
  if (Serial.available())
  {
    while (Serial.available())
    {
      recvbuff[byteCount] = Serial.read();
      byteCount++;
    }
  }
}

/**
 * @brief: check if recv right Modbus frame
 * retval: 0: 
 */
int UART_ModbusCheck(uint8_t* buffOut, int* len)
{
  if (byteCount > 0)
  {
    if (byteCount >= 3) // check if have at least 4 byte in buffer
    {
      uint16_t crcReadFromBuff;
      uint16_t crcSwap;
      uint16_t crcCal;
      crcReadFromBuff = (uint16_t)(recvbuff[byteCount - 2] << 8);
      crcReadFromBuff += recvbuff[byteCount - 1];
      crcCal = calcuteCRC16(recvbuff, (byteCount - 2));
      if (crcReadFromBuff == crcCal)    // check if recv ok
      {
        memcpy(buffOut, recvbuff, byteCount);
        *len = byteCount;
        // reset hw buffer
        byteCount = 0;
        return RECV_OK;
      }
      else 
        return RECV_NOT_OK;
    }
    else 
      return RECV_NOT_OK;
  }
  else
    return RECV_NOTHING;

}

void UART_ResetBuffer (void)
{
  byteCount = 0;
}

