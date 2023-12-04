#include <avr/io.h>
#include <avr/interrupt.h>
#include "dc_motor.h"
#include "lcd.h"
#include "timer.h"



/* Initialize DC motor */
void init_dc_motor(){
	// Enable switch button interrupt
	EIMSK |= (_BV(INT4) | _BV(INT5));
	EICRB |= (_BV(ISC41)| _BV(ISC51));		// falling edge trigger

	// GPIO
	DDRE &= ~(_BV(PE1) | _BV(PE0)); // used as input

	DDRB |= (_BV(DDB0) | _BV(DDB1) | _BV(DDB2) | _BV(DDB3));	// set as output pins to control DC motor
	//PORTB = (PORTB & 0xF0) | DC_Brake;							// motor driver enable pins	& brake to Vcc
	brake_dc_motor();
}

/* Disable DC Motor */
void disable_dc_motor(){
	DC_MOTOR_PORT = (DC_MOTOR_PORT & ~DC_MOTOR_MASK) | DC_DISABLE;
	//PORTB = (PORTB & 0xF0) | DC_Disable;
	mTimer(5);
}

void run_dc_motor(){
	DC_MOTOR_PORT = (DC_MOTOR_PORT & ~DC_MOTOR_MASK) | DC_FORWARD;
	//PORTB &= ~(_BV(PB0));  										// motor driving initialize	1 1 1 0
}

void brake_dc_motor(){
	DC_MOTOR_PORT = (DC_MOTOR_PORT & ~DC_MOTOR_MASK) | DC_BRAKE;
}