#include <avr/io.h>
#include "lcd.h"

/****** PWM ******/

void init_pwm(int num){
	/* Step 1: Set Timer 0 to Fast PWM mode */
	TCCR0A |= (_BV(WGM01) | _BV(WGM00));	// OCRA0 updated at TOP | TOV Flag set on MAX | TOP = 0xFF
	
	TIMSK0 |= _BV(OCIE0A);					// can change OCR0A value in ISR if needed
	
	/* Step 3: Set the compare match output mode to clear on a compare match */
	TCCR0A |= _BV(COM0A1);					// non-inverting mode
	
	/* Step 4: Set the PWM frequency to 3.9 kHz */
	TCCR0B |= _BV(CS01);					// prescaler = 8  -----> 8MHz / 8 / 256 ~= 3.9kHz (Timer0 is a 8 bit timer, page 120)
	
	/* Step 5: Set OCR0A to determine duty cycle */
	OCR0A = num;
	
	/* Step 6: set OC0A as output (page 76) */
	DDRB |= _BV(DDB7);						// OC0A: PORTB pin 7 is the PWM output A for Timer/Counter 0
}