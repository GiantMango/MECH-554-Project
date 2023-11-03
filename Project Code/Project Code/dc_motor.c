#include <avr/io.h>
#include <avr/interrupt.h>
#include "dc_motor.h"
#include "lcd.h"

/* Initialize DC motor */
void init_dc_moter(){
	DDRB |= (_BV(DDB0) | _BV(DDB1) | _BV(DDB2) | _BV(DDB3));	// set as output pins to control DC motor
	PORTB = (PORTB & 0x10) | DC_Brake;							// motor driver enable pins	& brake to Vcc
	PORTB &= ~(_BV(PB0));  										// motor driving initialize	1 1 1 0
	PORTL = PORTB << 4;
}


/* Disable DC Motor */
void disable_dc_motor(){
	PORTB = (PORTB & 0x10) | DC_Disable;
	mTimer(5);
	PORTL = PORTB << 4;
	LCDClear();
	LCDWriteStringXY(0, 0, "Killed");
}