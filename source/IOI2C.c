
#include "main.h"
#include "IOI2C.h"


#define SCL_Down() (DDRB |= (1<<3))
#define SDA_Down() (DDRB |= (1<<2))
#define SDA_Release() (DDRB &= ~(1<<2))
#define GetSCLState() (PINB & (1<<3))
#define GetSDAState() (PINB & (1<<2))

#define IOI2C_Wait() (_delay_us(10))

static void SCL_Release()
{
	DDRB &= ~(1<<3);
	while(GetSCLState() == 0);
}

void IOI2C_SendDummyClock()
{
	uint8_t i;
	for(i=0; i<16; i++){
		SCL_Down();
		IOI2C_Wait();
		SCL_Release();
		IOI2C_Wait();
	}
}

void IOI2C_SendStartBit(void)
{
	SDA_Down();
	IOI2C_Wait();
	SCL_Down();
	IOI2C_Wait();
	SDA_Release();
	IOI2C_Wait();
}
void IOI2C_SendStopBit(void)
{
	SCL_Down();
	IOI2C_Wait();
	SDA_Down();
	IOI2C_Wait();
	SCL_Release();
	IOI2C_Wait();
	SDA_Release();
	IOI2C_Wait();
}
void IOI2C_SendReadConditionBit(void)
{
	SCL_Down();
	IOI2C_Wait();

	SDA_Release();
	IOI2C_Wait();
	SCL_Release();
	IOI2C_Wait();

	SDA_Down();
	IOI2C_Wait();
	SCL_Down();
	IOI2C_Wait();
}
// return 1 if NACK received
uint8_t IOI2C_WriteByte(uint8_t d)
{
	uint8_t i;
	SCL_Down();
	IOI2C_Wait();

	for(i=0; i<8; i++){
		if(d & 0x80){
			SDA_Release();
		}else{
			SDA_Down();
		}
		IOI2C_Wait();
		SCL_Release();
		IOI2C_Wait();
		SCL_Down();
		d = d << 1;
		IOI2C_Wait(); // [19/01/18] added this line
	}
	uint8_t ret = 0;
	// ack/nack
	SDA_Release();
	IOI2C_Wait();
	SCL_Release();
	IOI2C_Wait();
	if(GetSDAState()) ret = 1;
	IOI2C_Wait();

	return ret;
}

uint8_t IOI2C_ReadByte(uint8_t* p, uint8_t ack)
{
	uint8_t d = 0;
	uint8_t i;
	SCL_Down();
	IOI2C_Wait();
	SDA_Release();
	IOI2C_Wait();

	for(i=0; i<8; i++){
		d = d << 1;
		SCL_Release();
		IOI2C_Wait();
		if(GetSDAState()){
			d |= 0x01;
		}
		SCL_Down();
		IOI2C_Wait();
	}
	// ack/nack
	if(ack) SDA_Down(); // send ack
	else SDA_Release();
	IOI2C_Wait();

	SCL_Release();
	IOI2C_Wait();

	*p = d;
	
	return 0;
}
