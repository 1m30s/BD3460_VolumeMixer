// BD3460 Controller
// (c) 2019 Eleken



#include "main.h"
#include "BD3460.h"
#include "IOI2C.h"





uint8_t BD3460_I2CSend(uint8_t addr, uint8_t reg, uint8_t value)
{
	uint8_t ret = 0;
	IOI2C_SendStartBit();
	
	ret |= IOI2C_WriteByte((addr<<1));
	ret |= IOI2C_WriteByte(reg);
	ret |= IOI2C_WriteByte(value);
	
	IOI2C_SendStopBit();
	
	return ret;
}
// I2C Read might not be supported

void BD3460_Init(uint8_t addr)
{
	IOI2C_SendDummyClock();

	BD3460_I2CSend(addr, 1, 0);
}

uint8_t BD3460_SendVolume(uint8_t addr, uint8_t ch, uint8_t attValue)
{
	if(ch > 5) ch = 5;
	if(attValue > BD3460_ATT_MAX) attValue = BD3460_ATT_MUTE;
	else if(attValue < BD3460_ATT_MIN) attValue = BD3460_ATT_MUTE;

	if(BD3460_I2CSend(addr, 0x28+ch, attValue))
	{
		IOI2C_SendDummyClock();
		return 1;
	}
	return 0;
}


