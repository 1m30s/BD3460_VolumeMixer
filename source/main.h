#ifndef _MAIN_H_
#define _MAIN_H_


#include <inttypes.h>
#include <stdlib.h>
#include <string.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/sleep.h>
#include <util/delay.h>
#include <avr/pgmspace.h>
#include <avr/eeprom.h>

// macros
#define SBI(port, bit)	(port |= _BV(bit))
#define CBI(port, bit)	(port &= ~_BV(bit))
#define NOP()	asm volatile("nop;")

//#define show_string_direct_pgm(text, a, b, c)  strcpy_P(tbuf, PSTR(text)); show_string_direct(tbuf, a, b, c)
//#define show_string_direct_pgm(text, a, b, c)  show_string_direct(text, a, b, c)

#define LB(x) ((x)&0x00FF)
#define HB(x) (((x)>>8)&0x00FF)

// Variables 
//extern volatile uint8_t g_swState ;

// Non-volatile settings
typedef struct {
	uint32_t curFreq1, curFreq2;
	uint8_t curPosX, curPosY;
}sEEPSettings; // g_eepSettings

#endif

