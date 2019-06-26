#ifndef BK1080_H
#define BK1080_H


uint8_t BK1080_WriteRegister(uint8_t reg, uint16_t value);
uint8_t BK1080_ReadRegister(uint8_t reg, uint16_t* pValue);
uint8_t BK1080_Init();
uint8_t BK1080_SetVolume(uint8_t vol);
uint16_t BK1080_GetStatus();
uint16_t BK1080_GetReadChannel();
uint8_t BK1080_SetChannel(uint16_t channel);

#endif