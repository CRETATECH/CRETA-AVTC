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
};

/*************************************************/
/*                  FUCTION PROTOTYPE            */
/*************************************************/
void protocolMqttDataProcess(uint8_t * dataIn, int len);
uint16_t calcuteCRC16 (uint8_t *pBuff, uint8_t pLen);
void protocolRecvUARTdataProcess (void);

#endif /* PROTOCOL_H */
