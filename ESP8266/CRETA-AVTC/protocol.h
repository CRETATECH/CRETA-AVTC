#ifndef PROTOCOL_H
#define PROTOCOL_H

enum {
  FUNC_READ  = 0x01,
  FUNC_WRITE = 0x02,
};

void protocolDataProcess(uint8_t * dataIn, int len);

#endif /* PROTOCOL_H */
