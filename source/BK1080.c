#// BK1080 Controller
// (c) 2019 Eleken



#include "main.h"
#include "IOI2C.h"

#define BK1080_ID 0x80

// Note that BK1080's I2C is not complient to the I2C format
uint8_t BK1080_WriteRegister(uint8_t reg, uint16_t value)
{
	IOI2C_SendStartBit();
	
	IOI2C_WriteByte(BK1080_ID);
	IOI2C_WriteByte((reg<<1));
	IOI2C_WriteByte(value >> 8);
	IOI2C_WriteByte(value);
	
	IOI2C_SendStopBit();
	return 0;
}

uint8_t BK1080_ReadRegister(uint8_t reg, uint16_t* pValue)
{
	uint8_t value;
	IOI2C_SendStartBit();
	
	IOI2C_WriteByte(BK1080_ID);
	IOI2C_WriteByte((reg<<1) | 1);
	IOI2C_ReadByte(&value, 1);
	(*pValue) = ((uint16_t)value) << 8;
	IOI2C_ReadByte(&value, 0);
	(*pValue) |= value;
	
	IOI2C_SendStopBit();
	return 0;
}

#define SYSCONF1_DEFAULT 0x0800
#define SYSCONF2_DEFAULT 0x0050

uint8_t BK1080_Init()
{
	BK1080_WriteRegister(0x02, 0x0001); // power up

	BK1080_WriteRegister(0x04, SYSCONF1_DEFAULT);
	// channel space and band
	BK1080_WriteRegister(0x05, SYSCONF2_DEFAULT);
	return 0;
}


// vol: 0x00(mute) - 0x0F (max)
uint8_t BK1080_SetVolume(uint8_t vol)
{
	BK1080_WriteRegister(0x05, SYSCONF2_DEFAULT | (vol & 0x0F));
	return 0;
}

// b14: Seek/Tune Complet
// b8:  Stereo
// b7-0: RSSI (Signal strength)
uint16_t BK1080_GetStatus()
{
	uint16_t val;
	BK1080_ReadRegister(0x0A, &val);
	return val;
}
// Freq = (channel)*0.1 + 76MHz
uint16_t BK1080_GetReadChannel()
{
	uint16_t val;
	BK1080_ReadRegister(0x0B, &val);
	return val & 0x03FF;
}
// Freq = (channel)*0.1 + 76MHz
uint8_t BK1080_SetChannel(uint16_t channel)
{
	BK1080_WriteRegister(0x03, channel);
	BK1080_WriteRegister(0x03, channel | 0x8000);
	
	return 0;
}

