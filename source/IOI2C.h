#ifndef IOI2C_H
#define IOI2C_H

uint8_t IOI2C_WriteByte(uint8_t d);
uint8_t IOI2C_ReadByte(uint8_t* p, uint8_t ack);

void IOI2C_SendDummyClock();
void IOI2C_SendStartBit(void);
void IOI2C_SendStopBit(void);
void IOI2C_SendReadConditionBit(void);

#endif
