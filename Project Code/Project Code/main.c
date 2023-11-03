/*
 * MECH 458 Mechatronics: Project
 *
 * Name: Taylor Liao
 * Name: Gavin Angman	
 *
 */ 

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include "lcd.h"
#include "mTimer.h"
#include "adc.h"
#include "pwm.h"
#include "dc_motor.h"

/* GLOBAL VARIABLES */
volatile unsigned int ramp_up_flag;
volatile unsigned int ramp_down_flag;

volatile unsigned int Vin;
volatile int current_dir;
volatile int state = 0;
volatile int ramp_counter = 0;

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
	EIMSK |= _BV(INT1) | _BV(INT0);			// enable INT1
	EICRA |= _BV(ISC11) | _BV(ISC01);		// falling edge trigger
	
	//Initialize LCD module
	InitLCD(LS_BLINK|LS_ULINE);

	//Clear the screen
	LCDClear();
	
	/* Generate 30% duty cycle PWM signal */
	init_pwm();
	
	/* PORTB 0b 0 0 0 0 ENa ENb INa INb */
	init_dc_moter();
	
	init_adc();
	free_running_adc();
	start_adc();
	
	sei();
	/*
	LCDWriteStringXY(0,0,"Counter");
	LCDWriteIntXY(8,0,counter,3);
	*/
	
    while(1)
    {		
		if(ramp_up_flag){
			if (ramp_counter == 255){
				ramp_counter = 0;
			} else {
				ramp_counter += 1;
			}
			ramp_up_flag = 0;
			LCDWriteIntXY(8, 0, ramp_counter, 3);
		}
		
		if(ramp_down_flag){
			if (ramp_counter == 0){
				ramp_counter = 0;
			} else {
				ramp_counter -= 1;
			}
			ramp_down_flag = 0;
			LCDWriteIntXY(8, 0, ramp_counter, 3);
		}

		
		/*
        if(ADC_result_flag){
			// Vin = ADC * Vref / 1024
			ADC_result_flag = 0x00; // ADC 0 - 255
			change_pwm_speed(ADC_result);
			display_pwm();
			LCDWriteIntXY(12, 1, ADC_result, 3);
		}
		*/
    }
}


/***** Interrupt Service Routines *****/
EMPTY_INTERRUPT(BADISR_vect);

/*
ISR(INT0_vect){ // Kill switch
	mTimer(25);
	kill_flag = 1;
	while((PINA & 0x02) == 0x00);
	mTimer(25);	
	cli();
}

ISR(INT1_vect){ // switch direction
	mTimer(25);
	switch_direction_flag = 1;
	while((PINA & 0x01) == 0x00);
	mTimer(25);
}
*/

ISR(INT0_vect){ // Ramp Up
	mTimer(25);
	ramp_up_flag = 1;
	while((PINA & 0x02) == 0x00);
	mTimer(25);
}

ISR(INT1_vect){ // Ramp Down
	mTimer(25);
	ramp_down_flag = 1;
	while((PINA & 0x01) == 0x00);
	mTimer(25);
}


ISR(INT2_vect){ // Active HIGH starts AD conversion
	ADCSRA |= _BV(ADSC); // Execute ADC when rising edge 
}

ISR(ADC_vect){ // Triggered when ADC is done
	ADC_result = ADCH; // MSB 8 bit
	ADC_result_flag = 1;
}


