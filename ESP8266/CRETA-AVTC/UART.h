#ifndef UART_H
#define UART_H

/*************************************************/
/*                  FUCTION PROTOTYPE            */
/*************************************************/
void UART_Init (void);
void UART_SendBuffer(uint8_t* buff, int len);
#endif /* UART_H */
