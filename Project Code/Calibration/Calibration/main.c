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
#include "sensor.h"

/* GLOBAL VARIABLES */
volatile unsigned char temp = 0;
volatile unsigned char int2_counter;
volatile unsigned int ADC_counter;
volatile unsigned char is_first = 1;
volatile unsigned char item_counter;
volatile unsigned char item_adc_ready;
volatile unsigned char reset_flag;



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
	
	init_pwm();
	change_pwm_speed(60);
	init_dc_moter();
	run_dc_motor();
	

	sei();

	while(1){			
		PORTL = 0x10;

		while(OR){ // when object is at the reflective sensor
			start_conversion();
			PORTL = 0x20;
	
			if(ADC_result_flag){
				if(ADC_result < ADC_curr_min){
					ADC_curr_min = ADC_result;
				}
				//ADCSRA |= _BV(ADSC);
			}
			
			ADC_result_flag = 0;
			item_adc_ready = 1;
		}
		
		//ADCSRA &= ~_BV(ADSC);
		
		if(item_adc_ready){
			disable_adc();
			item_counter += 1;

			if(item_counter == 1){
				ADC_min_min = ADC_curr_min;
				ADC_max_min = ADC_curr_min;
			} else {
				if(ADC_min_min > ADC_curr_min){
					ADC_min_min = ADC_curr_min;
				}
				if(ADC_max_min < ADC_curr_min){
					ADC_max_min = ADC_curr_min;
				}
			}

			LCDWriteIntXY(0,0,item_counter,3);
			LCDWriteIntXY(5,0,ADC_counter,5);
			LCDWriteIntXY(0,1,ADC_min_min,4);
			LCDWriteIntXY(5,1,ADC_max_min,4);
			LCDWriteIntXY(10,1,ADC_curr_min,4);
			
			item_adc_ready = 0;
			ADC_counter = 0;

		}

		if(kill_flag){
			brake_dc_motor();
		}
		
		if(reset_flag){
			ADC_max_min = 0;
			ADC_min_min = 1023;
			ADC_curr_min = 1023;
			item_counter = 0;
			ADC_counter = 0;
		}

				
	}
}


/***** Interrupt Service Routines *****/
EMPTY_INTERRUPT(BADISR_vect);


ISR(ADC_vect){ //ADC conversion done
	ADC_result = (ADCL >> 6) + (ADCH << 2);
	ADC_result_flag = 1;
	ADC_counter += 1;
}


ISR(INT4_vect){ // Kill switch
	mTimer(25);
	kill_flag = 1;
	while((PINE & 0x02) == 0x00);
	mTimer(25);	
}

ISR(INT5_vect){ // Kill switch
	mTimer(25);
	reset_flag = 1;
	while((PINE & 0x01) == 0x00);
	mTimer(25);
}

ISR(INT2_vect){ // OR sensor is logic high when object in
	enable_adc();
	start_conversion();
}