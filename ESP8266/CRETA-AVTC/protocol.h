#ifndef PROTOCOL_H
#define PROTOCOL_H

/*************************************************/
/*                  INCLUDE                      */
/*************************************************/

/*************************************************/
/*              EXTERN SYMBOL                    */
/*************************************************/
enum {
  FUNC_READ  = 0x01,
  FUNC_WRITE = 0x02,
  FUNC_ERROR = 0x03,
};

#define ID_INDEX          0
#define FUNC_INDEX        1
#define REG_INDEX         2
#define NUM_OF_REG_INDEX  3

typedef struct
{
  String Func;
  String Data;
  String Addr;
  uint8_t isRunning;
  uint8_t isWaitingforRes;
}func_status_t;

typedef enum
{
  START_RUNNING = 0,
  ON_RUNNING = 2,
  NOT_RUNNING = 1,
}func_running_status_t;

/*************************************************/
/*                  FUCTION PROTOTYPE            */
/*************************************************/
void protocolMqttDataProcess(uint8_t * dataIn, int len);
uint16_t calcuteCRC16 (uint8_t *pBuff, uint8_t pLen);
void protocolRecvUARTdataProcess (void);
void protocolFuncStatusInit(void);

#endif /* PROTOCOL_H */
