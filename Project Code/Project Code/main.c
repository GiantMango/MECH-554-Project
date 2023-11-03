/*
 * MECH 458 Mechatronics: Project
 *
 * Name: Taylor Liao
 * Name: Gavin Angman	
 *
 */ 
#include <avr/io.h>
#include <avr/interrupt.h>
#include "lcd.h"
#include "mTimer.h"
#include "adc.h"
#include "pwm.h"
#include "dc_motor.h"
#include "LinkedQueue.h"

/* GLOBAL VARIABLES */


int main(void)
{
	/* Disable global interrupt */
	cli();
		
	/* Initialize main clock: 8 MHz */
	CLKPR = 0x80;
	CLKPR = 0x01;
	
	/* Set PORTC as output to display ADC result */
	DDRL = 0xFF;
	DDRA = 0x00;
	
	/* Init Interrupt */
	
	//Initialize LCD module
	InitLCD(LS_BLINK|LS_ULINE);
	LCDClear();
	
	init_pwm();
	
	init_dc_moter();
	init_adc();
	free_running_adc();
	start_adc();
	
	sei();
	
	while(1)
	{				
	
	}
}


/***** Interrupt Service Routines *****/
EMPTY_INTERRUPT(BADISR_vect);

ISR(INT2_vect){ // Active HIGH starts AD conversion
	ADCSRA |= _BV(ADSC); // Execute ADC when rising edge 
}

ISR(ADC_vect){ // Triggered when ADC is done
	ADC_result = ADCH; // MSB 8 bit
	ADC_result_flag = 1;
}


