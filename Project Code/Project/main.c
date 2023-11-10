/*
 * MECH 458 Mechatronics: Project
 *
 * Name: Taylor Liao
 * Name: Gavin Angman	
 */ 

#include <avr/io.h>
#include <avr/interrupt.h>
#include "lcd.h"
#include "mTimer.h"
#include "adc.h"
#include "pwm.h"
#include "dc_motor.h"
#include "LinkedQueue.h"
#include "sensor.h"
#include "stepper_motor.h"

/* GLOBAL VARIABLES */
volatile unsigned char temp;
volatile unsigned char ADC_counter;



int main(void)
{
	/* Disable global interrupt */
	cli();
		
	/* Initialize main clock: 8 MHz */
	CLKPR = 0x80;
	CLKPR = 0x01;
	
	/* Set PORTC as output to display ADC result */
	DDRL = 0xFF;
	DDRA = 0x00;	// for buttons


	InitLCD(LS_BLINK|LS_ULINE);
	LCDClear();	
	init_adc();
	free_running_adc();
	current_step = 2;

	sei();

	while(1){			
		LCDWriteIntXY(0,0,OR,1);	
		
		while(!OR){
			ADCSRA &= ~_BV(ADSC);
		}
		
		if(ADC_result_flag){
			ADC_result_flag = 0x00;
		}
		
		LCDWriteIntXY(0,1,ADC_min_min,4);
		LCDWriteIntXY(5,1,ADC_max_min,4);
		LCDWriteIntXY(10,1,ADC_result,4);
				
	}
}


/***** Interrupt Service Routines *****/
EMPTY_INTERRUPT(BADISR_vect);

ISR(INT2_vect){ //
	ADCSRA |= _BV(ADSC); // Execute ADC when rising edge 
}

ISR(ADC_vect){ //ADC conversion done
	ADC_result = (ADCL >> 6) + (ADCH << 2);
	if(ADC_result < ADC_max_min){
		ADC_max_min = ADC_result;
	}
	if(ADC_result < ADC_curr_min){
		ADC_curr_min = ADC_result;
	}
	if(ADC_max_min < ADC_min_min){
		ADC_min_min = ADC_max_min;
	}
	ADC_result_flag = 1;
	ADC_counter += 1;
}