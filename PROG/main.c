/*
 * main.c
 *
 * Created: 8/6/2022 3:13:30 PM
 *  Author: guigur
 */ 

#include <xc.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <util/twi.h>

uint8_t rows[] = { 0x02, 0x04, 0x08 };
uint8_t cols[] = { 0xe0, 0xd0, 0xb0, 0x70 };

typedef union cg {
	uint8_t brg[3];
	uint32_t col;
} colorGrad_t;

//uint32_t color = 0xFF0000; //BRG
uint8_t colorA[] = { 0x00, 0xff, 0xff };
#define NEOONE 	PORTB.OUT = 0xff;PORTB.OUT = 0xff;PORTB.OUT = 0xff;PORTB.OUT = 0xff;PORTB.OUT = 0xff;PORTB.OUT = 0xff;PORTB.OUT = 0x00; PORTB.OUT = 0x00; PORTB.OUT = 0x00;
#define NEOZERO PORTB.OUT = 0xff;PORTB.OUT = 0xff;PORTB.OUT = 0xff;PORTB.OUT = 0x00;PORTB.OUT = 0x00;PORTB.OUT = 0x00; PORTB.OUT = 0x00; PORTB.OUT = 0x00;

//#define NEOONE 	PORTB.OUT = 0xff;PORTB.OUT = 0xff;PORTB.OUT = 0xff;PORTB.OUT = 0xff;PORTB.OUT = 0xff;PORTB.OUT = 0xff;PORTB.OUT = 0xff;PORTB.OUT = 0x00;PORTB.OUT = 0x00;PORTB.OUT = 0x00;PORTB.OUT = 0x00;PORTB.OUT = 0x00;PORTB.OUT = 0x00;
//#define NEOZERO PORTB.OUT = 0xff;PORTB.OUT = 0xff;PORTB.OUT = 0xff;PORTB.OUT = 0xff;PORTB.OUT = 0x00;PORTB.OUT = 0x00;PORTB.OUT = 0x00;PORTB.OUT = 0x00;PORTB.OUT = 0x00;PORTB.OUT = 0x00;PORTB.OUT = 0x00;PORTB.OUT = 0x00;

void d(uint16_t val)
{
	for(uint16_t i = 0; i < val; i++) {}
}

void ringAnnim()
{
		//col => e d b 7 MSB
		//row => 2 4 8 LSB
		for (uint8_t r = 0; r < sizeof(rows); r++)
		{
			for (uint8_t c = 0; c < sizeof(cols); c++)
			{
				PORTA.OUT = rows[r] + cols[c];
				_delay_ms(1000);
			}

		}
}

void neopix(uint32_t color)
{
	for(uint8_t i = 0; i < 24; i++)
	{
		if (color & 0x01)
		{
			NEOONE;
		}
		else
		{
			NEOZERO;
		}
		color = color >> 1;
	}
}

ISR(TCA0_OVF_vect)
{
	TCA0.SINGLE.INTFLAGS = TCA_SINGLE_OVF_bm;
	PORTB.OUT = 0;
	PORTB.OUT = 0xff;
}

void timerInit()
{
	TCA0.SINGLE.INTCTRL = 0 << TCA_SINGLE_CMP0_bp
	| 0 << TCA_SINGLE_CMP1_bp
	| 0 << TCA_SINGLE_CMP2_bp
	| 1 << TCA_SINGLE_OVF_bp;
	
	TCA0.SINGLE.PER = 1;
	
	TCA0.SINGLE.CTRLA = TCA_SINGLE_CLKSEL_DIV1_gc
	| 1 << TCA_SINGLE_ENABLE_bp;	
}

typedef enum  { REDO, GREENO, BLUEO } colorStates_t;
typedef struct { colorStates_t s; uint8_t hex; } colorChan_t;
	
colorStates_t state = BLUEO;
//uint8_t dir = 1;
colorGrad_t c;
	

colorStates_t stateMinus(colorStates_t currentState)
{
	if (currentState == REDO)
		return (BLUEO);
	else if (currentState == GREENO) 
		return (REDO);
	else if (currentState == BLUEO) 
		return (GREENO);
}

void colorRoutine(colorChan_t *cc)
{
	//on estime red
	if (state == stateMinus(cc->s)) //BLUEO)
	{
		if (cc->hex >= 255)
		{
			state = cc->s;
		}
		else
		{
			cc->hex = cc->hex + 1;
		}
	}
	else if (state == cc->s)
	{
		cc->hex = cc->hex - 1;
	}
}
colorChan_t ccred = { REDO, 0 };
	colorChan_t ccblue = { BLUEO, 0 };
	colorChan_t ccgreen = { GREENO, 0 };
void colorMania()
{
	

	colorRoutine(&ccred);
	colorRoutine(&ccblue);
	colorRoutine(&ccgreen);
	c.brg[0] = ccblue.hex;
	c.brg[1] = ccred.hex;
	c.brg[2] = ccgreen.hex;

	// ccred.hex, ccgreen.hex};
}

int main(void)
{
	CCP = 0xD8;
	CLKCTRL.MCLKCTRLB = 0x00;

	//PORTA.DIR = 0xff;
	PORTB.DIR = 0xff;
	timerInit();
	
	//sei();

	
	while(1)
    {
		neopix(c.col);
		//neopix(0xff0000);
       //ringAnnim();
	  //neopix();
	  		colorMania();

	  _delay_ms(50);


	}
	return (0);
}
