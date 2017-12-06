#ifndef UART_H
#define UART_H

/*************************************************/
/*                  FUCTION PROTOTYPE            */
/*************************************************/

enum
{
  RECV_OK = 0,
  RECV_NOTHING,
  RECV_NOT_OK,
}modebus_recv_frame_status_t;

void UART_Init (void);
void UART_SendBuffer(uint8_t* buff, int len);
void UART_Event (void);
int UART_ModbusCheck(uint8_t* buffOut, int* len);
void UART_ResetBuffer (void);

#endif /* UART_H */
