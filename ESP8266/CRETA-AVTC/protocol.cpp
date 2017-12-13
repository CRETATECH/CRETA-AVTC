/*************************************************/
/*                  INCLUDE                      */
/*************************************************/
#include <ESP8266WiFi.h>
#include "protocol.h"
#include <ArduinoJson.h>
#include "mqtt.h"
#include "UART.h"
#include "param.h"
#include "timer.h"
/*************************************************/
/*                  LOCAL  VARIABLE              */
/*************************************************/
String gFunc;
String gAddr;
String gData;
int regAddr_Send;
int timerTimeoutID;
int timerRecvdataID;
int timerFunc200mlID;
uint8_t FuncWaiting;
func_status_t func_rw_t;
func_status_t func_200ml_t;
func_status_t func_3min_t;

/*************************************************/
/*             FUNCTION PROTOTYPE                */
/*************************************************/
int parseJson(String pJson);
void createModBusBuffer (func_status_t *func_t, uint8_t* BuffOut, uint8_t* buffOutLen);
uint16_t calcuteCRC16 (uint8_t *pBuff, uint8_t pLen);
String protocolCreateJson (String pFunc, String pAddr, String pData);
void protocolTimeoutErrorProcess (void);
void protocolFunc200mlProcess (void);
/*************************************************/
/*                  MAIN FUNCTION                */
/*************************************************/
void protocolMqttDataProcess(uint8_t * dataIn, int len)
{
  uint8_t buffLen;
  uint8_t modbusBuff[20];
  String dataString = "";
  for (int i = 0; i < len; i++)
  {
    dataString += String((char)dataIn[i]);
  }
  if (parseJson(dataString) == 1)
  {
    if (gFunc == "101")
    {
      #ifdef DEBUG
        Serial.println("special func");
      #endif
      if (gAddr == "0101")
      {
        func_3min_t.isRunning = START_RUNNING;
        func_3min_t.isWaitingforRes = 1;
        func_3min_t.Addr = gAddr;
        func_3min_t.Data = "1";
        createModBusBuffer(&func_3min_t, &modbusBuff[0], &buffLen);
        #ifdef DEBUG
          Serial.println("Func 3min");
        #endif 
      }
      else if (gAddr == "0102")
      {
        func_200ml_t.isWaitingforRes = 1;
        func_200ml_t.isRunning = START_RUNNING;
        func_200ml_t.Addr = gAddr;
        func_200ml_t.Data = "1";
        createModBusBuffer(&func_200ml_t, &modbusBuff[0], &buffLen);
        #ifdef DEBUG
          Serial.println("Func 200ml");
        #endif         
      }
    }
    else if ((gFunc == "001") || (gFunc == "002"))
    {
      func_rw_t.Func = gFunc;
      func_rw_t.isRunning = ON_RUNNING;
      func_rw_t.isWaitingforRes = 1;
      func_rw_t.Addr = gAddr;
      func_rw_t.Data = gData;
      createModBusBuffer(&func_rw_t, &modbusBuff[0], &buffLen);
    }
    #ifdef DEBUG
      Serial.println("PROCESS: Send data to STM");
    #endif
    if (FuncWaiting == 1)
    {
      mqttPublish(protocolCreateJson("003", "", "002")); //ESP is busy
      return;
    }
    else
    {
      FuncWaiting = 1;
      UART_SendBuffer(modbusBuff, buffLen);
      /* create sw timer to handle timeout */
      timerTimeoutID = createSWTimer(2000, protocolTimeoutErrorProcess, (void*)0);
      runSWTimer(timerTimeoutID);
      /* create sw timer to handle uart recv data */
      timerRecvdataID = createSWTimer(50, protocolRecvUARTdataProcess, (void*)0);
      runSWTimer(timerRecvdataID);     
    }
  }
  else
    mqttPublish(protocolCreateJson("003", "", "001"));
}

void protocolRecvUARTdataProcess (void)
{
  String funcString;
  String addrString;
  String dataString;
  uint16_t dataInt;
  uint8_t recv_buff[20];
  int len;
  UART_Event();
  int recv_status = UART_ModbusCheck(recv_buff, &len);
  if (recv_status == RECV_OK)
  {
    deleteSWTimer(timerTimeoutID);
    FuncWaiting = 0;
    /* parse frame */
    if (recv_buff[FUNC_INDEX] != FUNC_ERROR)
    {
      if (func_rw_t.isWaitingforRes == 1)
      {
        if (recv_buff[FUNC_INDEX] == FUNC_READ)
          funcString = "002";
        else if (recv_buff[FUNC_INDEX] == FUNC_WRITE)
          funcString = "001";
        addrString = func_rw_t.Addr;
        
        if (recv_buff[ID_INDEX] == 0x01)
        {
          if (recv_buff[2] == 0x00)  //data byte
            dataString = "0";
          else if (recv_buff[2] == 0x64)
            dataString = "1";
        }
        else if (recv_buff[0] == 0x02)
        {
          if (addrString == "0401")
          {
            if (recv_buff[3] == 0x00) // data byte
              dataString = "0";
            else if (recv_buff[3] == 0x64)
              dataString = "1";
          }
          else if (addrString = "0201")
          {
            dataInt = (uint16_t)(recv_buff[2]<<8) + recv_buff[3];
            dataString = (String)(dataInt / 100);
            dataString += ".";
            int temp = dataInt % 100;
            if (temp < 10)
              dataString += "0";
            dataString += (String)(temp);
          } 
        }
        func_rw_t.isRunning = NOT_RUNNING;
        func_rw_t.isWaitingforRes = 0;
      }
      else if (func_200ml_t.isWaitingforRes == 1)
      {
        if (func_200ml_t.isRunning == START_RUNNING)
        {
          #ifdef DEBUG
            Serial.println("Func 200ml, On Running");
          #endif
          func_200ml_t.isRunning = ON_RUNNING;
          func_200ml_t.isWaitingforRes = 0;
          funcString = "101";
          addrString = "0102";
          dataString = "1";
          #ifdef DEBUG
            Serial.println("Func 200ml, Start timer");
          #endif
          timerFunc200mlID =  createSWTimer(6000, protocolFunc200mlProcess, (void*)0);
          runSWTimer(timerFunc200mlID); 
        }
        else if(func_200ml_t.isRunning == ON_RUNNING)
        {
          #ifdef DEBUG
            Serial.println("Func 200ml, Finish Running");
          #endif          
          func_200ml_t.isRunning = NOT_RUNNING;
          func_200ml_t.isWaitingforRes = 0;
          funcString = "101";
          addrString = "0102";
          dataString = "2";          
        }
      }
      else if (func_3min_t.isWaitingforRes == 1)
      {
        func_3min_t.isRunning = ON_RUNNING;
        func_3min_t.isWaitingforRes = 0;
        funcString = "101";
        addrString = "0101";
        dataString = "1";
      }
    }
    else
    {
      /* TODO: Error proc */ 
    }
    mqttPublish(protocolCreateJson(funcString, addrString, dataString));
  }
  else
  {
    int timerID = createSWTimer(50, protocolRecvUARTdataProcess, (void*)0);
    runSWTimer(timerID);
  }
}

void protocolFunc200mlProcess (void)
{
  #ifdef DEBUG
    Serial.println("200ml Handle");
  #endif
  uint8_t buffOut[10];
  uint8_t len;
  func_200ml_t.Data = "0";
  createModBusBuffer(&func_200ml_t, buffOut, &len);
  if (FuncWaiting == 1)
  {
    /* TODO: wating for resent */
  }
  else
  {
    UART_SendBuffer(buffOut, len);
    FuncWaiting = 1;
    func_200ml_t.isWaitingforRes = 1;
    /* create sw timer to handle timeout */
    timerTimeoutID = createSWTimer(2000, protocolTimeoutErrorProcess, (void*)0);
    runSWTimer(timerTimeoutID);
    /* create sw timer to handle uart recv data */
    timerRecvdataID = createSWTimer(50, protocolRecvUARTdataProcess, (void*)0);
    runSWTimer(timerRecvdataID);
  }
}

void protocolTimeoutErrorProcess (void)
{
  #ifdef DEBUG
    Serial.println("ERROR: timeout");
  #endif
  UART_ResetBuffer();
  deleteSWTimer(timerRecvdataID);
  mqttPublish(protocolCreateJson("003", "", "004"));
  FuncWaiting = 0;
  func_rw_t.isWaitingforRes = 0;
  func_200ml_t.isWaitingforRes = 0;
  func_3min_t.isWaitingforRes = 0;
}


void createModBusBuffer (func_status_t *func_t, uint8_t* BuffOut, uint8_t* buffOutLen)
{
  int funcInt = func_t->Func.toInt();
  int addrInt = func_t->Addr.toInt();
  int slaveID = addrInt / 100;
  uint16_t crc;    // swapped, low byte first
  /* Get slave ID */
  if (slaveID == 1) // relay in slave 1
    BuffOut[ID_INDEX] = 0x01; 
  else if (slaveID == 2 || slaveID == 4)
    BuffOut[ID_INDEX] = 0x02;
  if ((func_t->Func == "001") || (func_t->Func == "101"))
  {
    BuffOut[FUNC_INDEX] = FUNC_WRITE;
    BuffOut[REG_INDEX] = (0x10 | (addrInt % 100)) - 1; // Reg Addr, -1 because start from 0
    BuffOut[NUM_OF_REG_INDEX] = 0x01;                         // Number of Reg
    if (func_t->Data == "1")
      BuffOut[4] = 0x64;
    else if (func_t->Data == "0")
      BuffOut[4] = 0x00;
    crc = calcuteCRC16(BuffOut, 5);
    BuffOut[5] = (uint8_t)(crc >> 8);
    BuffOut[6] = crc & 0xFF;
    *buffOutLen = 7;
  }
  else if (func_t->Func == "002")
  {
    BuffOut[1] = FUNC_READ;
    if (slaveID == 2 || slaveID == 4)
    {
      if (func_t->Addr == "0201")
        BuffOut[REG_INDEX] = 0x22;
      else if (func_t->Addr == "0401")
        BuffOut[REG_INDEX] = 0x20;
      BuffOut[NUM_OF_REG_INDEX] = 0x02;
    }
    else if (slaveID == 1)
    {
      BuffOut[REG_INDEX] = (0x10 | (addrInt % 100)) - 1;
      BuffOut[NUM_OF_REG_INDEX] = 0x01;                   // Number of Reg
    }
    crc = calcuteCRC16(BuffOut, 4);
    BuffOut[4] = (uint8_t)(crc >> 8);
    BuffOut[5] = crc & 0xFF;
    *buffOutLen = 6;
  }
}

/**
 * @brief       create Json to prepare for publishing
 * @param       pFunc, pAddr, pData
 * @retval      String json
 *              
 */
String protocolCreateJson (String pFunc, String pAddr, String pData)
{
  
   String _stringout = "{\"USER\" : \"AVTC"  + Get_macID() + "\", \"FUNC\" : \"" + pFunc + "\", \"ADDR\" : \"" + pAddr + "\", \"DATA\" : \"" + pData + "\"}";
   return _stringout;
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

void protocolFuncStatusInit(void)
{
  func_200ml_t.Func = "101";
  func_3min_t.Func = "101";
  func_200ml_t.Addr = "0102";
  func_3min_t.Addr = "0101";
  
  func_rw_t.isRunning = NOT_RUNNING;
  func_200ml_t.isRunning = NOT_RUNNING;
  func_3min_t.isRunning = NOT_RUNNING;
  
  func_rw_t.isWaitingforRes = 0;
  func_200ml_t.isWaitingforRes = 0;
  func_3min_t.isWaitingforRes = 0;
}

