// YLE-Volume-Mixer
// For ATmega48/88, 4MHz clock
// 19/05/18 Eleken rev_c1815@yahoo.co.jp

#include "main.h"
#include "BD3460.h"

/* EEPROM */
uint8_t eep_dummy __attribute__((section(".eeprom")));
uint8_t eep_area[sizeof(sEEPSettings)] __attribute__((section(".eeprom")));

#define BD3460_ADDR 0x42

static void LoadEEP(sEEPSettings* pEEPData){ // EEPROM -> RAM
	uint8_t i;
	uint8_t* p = (uint8_t*)pEEPData;
	for(i=0; i<sizeof(sEEPSettings); i++){
		uint8_t a = eeprom_read_byte(&(eep_area[i]));
		if(a != 0xFF) p[i] = a;
	}
}
static void SaveEEP(const sEEPSettings* pEEPData){ // RAM -> EEPROM 
	uint8_t i;
	uint8_t* p = (uint8_t*)pEEPData;
	for(i=0; i<sizeof(sEEPSettings); i++){
		eeprom_write_byte(&(eep_area[i]), p[i]);
	}
}


volatile uint16_t g_curADCValue;
volatile uint8_t g_ledSegment[4];

#define TOUCH_SW 0

static uint16_t GetSWState(void)
{
	uint16_t a = PINC;
	uint8_t b2 = PIND;
	// PIND: swap PD2 thru PD7
	uint8_t b = 0;
	if(b2 & 0x04) b |= 0x20;
	if(b2 & 0x08) b |= 0x10;
	if(b2 & 0x10) b |= 0x08;
	if(b2 & 0x20) b |= 0x04;
	if(b2 & 0x40) b |= 0x02;
	if(b2 & 0x80) b |= 0x01;
	uint16_t state = (b & 0x3F) | (a << 6);
	
	return ~state;
}

// SREG
void OutputSreg(uint16_t d)
{
	uint8_t i;
	for(i=0; i<16; i++){
		if(d&0x8000) SBI(PORTC, PC0);
		else CBI(PORTC, PC0);
		SBI(PORTC, PC1);
		CBI(PORTC, PC1);
		d = d << 1;
	}
	SBI(PORTC, PC2);
	CBI(PORTC, PC2);
	CBI(PORTC, PC3);
}

uint8_t g_ledFont[10] PROGMEM = 
{
	0b00111111, 0b00000110, 0b01011011, 0b01001111, 0b01100110, 
	0b01101101, 0b01111101, 0b00100111, 0b01111111, 0b01101111
};

// Timer0 Interrput (2.0 ms)
ISR(TIMER0_OVF_vect)
{
	PORTB ^= 0x01;
	static uint8_t digit;
	
	digit++;
	if(digit>3) digit = 0;
	uint16_t sregData = (0x0200 << digit) | g_ledSegment[digit];
	sregData = ~sregData; // com.A
	OutputSreg(sregData);
}

#define ADC_BUF_SIZE 16
// ADC Interrput
ISR(ADC_vect)
{
	PORTB ^= 0x02;
	static uint16_t adcBuf[ADC_BUF_SIZE];
	uint16_t d = ADC;
	
	uint8_t i;
	for(i=0; i<ADC_BUF_SIZE-1; i++){
		adcBuf[i] = adcBuf[i+1];
	}
	adcBuf[ADC_BUF_SIZE-1] = d;
	
	uint16_t ret = 0;
	for(i=0; i<ADC_BUF_SIZE; i++){
		ret += adcBuf[i];
	}
	ret /= ADC_BUF_SIZE;
	
	g_curADCValue = ret;
	
}

#define GetLEDFont(n) pgm_read_byte(&g_ledFont[n])
static void Init(void)
{
	//OSCCAL = 176; // [Note] this value should be adjusted by reading CAL value,,, 176 is an example value
	
	DDRC  = 0b00001111; // PC0-3: Output, PC4-5: Input
	PORTC = 0b00111000; 
	
	DDRD  = 0b00000010; // SW0-5, TX, RX
	PORTD = 0b11111101;
	
	DDRB  = 0b11110011; // PB2, PB3: SDA, SCL
	PORTB = 0b11110001;
	
	// ADC
	ADMUX = 0b01000111; // ADC7, VREF = AVCC
	ADCSRA = 0b10101111; // ADPS = 1/128 (63 kHz), ADIE = 1
	ADCSRA |= 0x40; // start adc
	
	// Timer0: 2 ms (8MHz / 64 / 256)
	TCCR0A = 0b00000000;
	TCCR0B = 0b00000011;
	TIMSK0 = 0b00000001; // TOIE0 enable
	
	
	// Timer1: CTC, output OC1A
//	TCCR1A = 0b01000000; // Toggle OC1A on compare match
//	TCCR1B = 0b00001000; // stop timer here
//	OCR1A  = 0; // OCR1A = OSC/freq
	
	g_ledSegment[0] = GetLEDFont(0);
	g_ledSegment[1] = GetLEDFont(1);
	g_ledSegment[2] = GetLEDFont(2);
	g_ledSegment[3] = GetLEDFont(3);
	
	BD3460_Init(BD3460_ADDR);
}


int main(void)
{
	uint8_t i;
	// wait for power up
	for(i=0; i<100; i++) _delay_ms(2);
	
	Init();
	sei();
	
	uint8_t lastValue = 0;
	while(1)
	{
		// ADC value -> LED
		/*
		g_ledSegment[0] = GetLEDFont(g_curADCValue / 1000);
		g_ledSegment[1] = GetLEDFont((g_curADCValue / 100) % 10);
		g_ledSegment[2] = GetLEDFont((g_curADCValue / 10) % 10);
		g_ledSegment[3] = GetLEDFont(g_curADCValue % 10);
		*/
		
		_delay_ms(10);
		uint8_t value = g_curADCValue / 4;
		if(value > BD3460_ATT_MAX) value = BD3460_ATT_MUTE;
		if(value < BD3460_ATT_MIN) value = BD3460_ATT_MIN;
		// Attenuator value -> LED
		if(value == BD3460_ATT_MUTE){
			g_ledSegment[0] = g_ledSegment[1] = g_ledSegment[2] = g_ledSegment[3] = 0b01000000; // -
		}
		else if(value > BD3460_ATT_0DB) {
			g_ledSegment[0] = 0; // -
			g_ledSegment[1] = 0b01000000; // -
			g_ledSegment[2] = GetLEDFont((value - BD3460_ATT_0DB) / 10);
			g_ledSegment[3] = GetLEDFont((value - BD3460_ATT_0DB) % 10);
		}else{
			g_ledSegment[0] = 0; // -
			g_ledSegment[1] = 0;
			g_ledSegment[2] = GetLEDFont((BD3460_ATT_0DB - value) / 10);
			g_ledSegment[3] = GetLEDFont((BD3460_ATT_0DB - value) % 10);
		}
		
		if(value != lastValue)
		{
			uint8_t ch;
			uint8_t ret = 0;
			for(ch=0; ch<6; ch++)
			{
				ret |= BD3460_SendVolume(BD3460_ADDR, ch, value);
			}
			if(ret) {
				// Display "Err"
				_delay_ms(100);
				g_ledSegment[0] = 0;
				g_ledSegment[1] = 0b01111001;
				g_ledSegment[2] = 0b01010000;
				g_ledSegment[3] = 0b01010000;
				_delay_ms(100);
			}
		}
	}
	
	
	return 0;
}
