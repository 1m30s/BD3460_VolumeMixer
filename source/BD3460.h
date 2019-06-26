#ifndef BD3460_H
#define BD3460_H


// primal function

uint8_t BD3460_I2CSend(uint8_t addr, uint8_t reg, uint8_t value);
uint8_t BD3460_I2CReceive(uint8_t addr, uint8_t reg, uint8_t* pValue);

void BD3460_Init(uint8_t addr);

#define BD3460_ATT_MUTE 0xFF
#define BD3460_ATT_MAX  0b11001111 // -79 dB
#define BD3460_ATT_0DB  0b10000000 // 0
#define BD3460_ATT_MIN  0b01101001 // +23 dB
// 1dB step

// ch: 0-5 (Front1, Front2, Rear1, Rear2, Sub1, Sub2)
uint8_t BD3460_SendVolume(uint8_t addr, uint8_t ch, uint8_t attValue);

#endif
