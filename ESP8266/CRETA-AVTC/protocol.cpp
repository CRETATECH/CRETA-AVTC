#include <ESP8266WiFi.h>
#include "protocol.h"
#include <ArduinoJson.h>
#include "mqtt.h"

int parseJson(String pJson);

String gFunc;
String gAddr;
String gData;

void protocolDataProcess(uint8_t * dataIn, int len)
{
  String dataString = "";
  for (int i = 0; i < len; i++)
  {
    dataString += dataIn[i];
  }
  if (parseJson(dataString) == 1)
  void createModBusBuffer (uint8_t* BuffOut)
}


/**
 * @brief       parse Json
 * @param       pJson
 * @retval      0: parse failed
 *              1: parse success
 */
int parseJson(String pJson)
{
  DynamicJsonBuffer _jsonBuffer;
  JsonObject& root = _jsonBuffer.parseObject(pJson);
  if (!root.success())
  {
    return 0;
  } 
  String _b      = root["FUNC"];
  gFunc = _b;
  String _c      = root["ADDR"];
  gAddr = _c;
  String _d      = root["DATA"];
  gData = _d;
  return 1; 
}

void createModBusBuffer (uint8_t* BuffOut)
{
  int funcInt = gFunc.toInt();
  int addrInt = gAddr.toInt();
  int slaveID = addrInt / 100;
  uint16_t crc;    // swapped, low byte first
  /* Get slave ID */
  if (slaveID == 1) // relay in slave 1
    BuffOut[0] = 0x01; 
  else if (slaveID == 2 || slaveID == 4)
    BuffOut[0] = 0x02;
  if (gFunc == "001")
  {
    BuffOut[1] = FUNC_WRITE;
    BuffOut[2] = 0x10 | (addrInt % 100); // Reg Addr
    BuffOut[3] = 0x01;                   // Number of Reg
    crc = calcuteCRC16(BuffOut, 4);
    BuffOut[4] = (uint8_t)(crc >> 8);
    BuffOut[5] = crc & 0xFF;
  }
  else if (gFunc == "0x02")
  {
    BuffOut[1] = FUNC_READ;
    if (slaveID == 2 || slaveID == 4)
      BuffOut[2] = 0x20 | (addrInt % 100);
    else if (slaveID == 1)
      BuffOut[2] = 0x10 | (addrInt % 100);
    BuffOut[3] = 0x01;                   // Number of Reg
    crc = calcuteCRC16(BuffOut, 4);
    BuffOut[4] = (uint8_t)(crc >> 8);
    BuffOut[5] = crc & 0xFF;
  }  
}


uint16_t calcuteCRC16 (uint8_t *pBuff, uint8_t pLen)
{
  uint16_t crc = 0xFFFF;
  for (int pos = 0; pos < pLen; pos++)
  {
    crc ^= (uint16_t) pBuff[pos]; // XOR byte into least sig. byte of crc
    for (int i = 0; i < 8; i++)   // Loop over each bit
    {
      if ((crc & 0x0001) != 0 )   // If the LSB is set
      {
        crc >>= 1;                
        crc ^= 0xA001;
      }
      else
        crc >>= 1;
    }
  }
  // Note, this number has low and high bytes swapped, so use it accordingly (or swap bytes)
  return crc;
}
