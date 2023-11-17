/*
 * MECH 458 Mechatronics: Project
 *
 * Name: Taylor Liao
 * Name: Gavin Angman	
 */ 

#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdlib.h>
#include <stdio.h>
#include "lcd.h"
#include "timer.h"
#include "adc.h"
#include "pwm.h"
#include "dc_motor.h"
#include "LinkedQueue.h"
#include "sensor.h"
#include "stepper_motor.h"

/* Material Reflection Bounds */
#define BLACK				0
#define STEEL				1
#define WHITE				2
#define ALUMINUM		3


#define ALUMINUM_STEEL_BOUND		100
#define STEEL_WHITE_BOUND				800
#define WHITE_BLACK_BOUND				898

#define TOTAL_ITEM								4

/* GLOBAL VARIABLES */
volatile unsigned char temp = 0;
volatile unsigned char int2_counter;
volatile unsigned int ADC_counter;
volatile unsigned char is_first = 1;
volatile unsigned char item_counter;
volatile unsigned char item_adc_ready;
volatile unsigned char reset_flag;
volatile unsigned char current_plate;
volatile unsigned char next_plate;
volatile unsigned char aluminum_counter;
volatile unsigned char steel_counter;
volatile unsigned char white_counter;
volatile unsigned char black_counter;
volatile unsigned char task_complete_flag;

/* default rotation */							  	//|  B  |  A  |  W  |  S  |
volatile signed char rotations[4][4] = {{0, DEG90, DEG180, NEG_DEG90}, // current plate black
																				 {NEG_DEG90, 0, DEG90, DEG180}, // current plate aluminum
																			 	 {DEG180, NEG_DEG90, 0 ,DEG90}, // current plate white
																			 	 {DEG90, DEG180, NEG_DEG90, 0}}; // current plate steel

/* Variable Declaration */
link *head;			/* The ptr to the head of the queue */
link *tail;			/* The ptr to the tail of the queue */
link *newLink;		/* A ptr to a link aggregate data type (struct) */
link *rtnLink;		/* same as the above */
element eTest;		/* A variable to hold the aggregate data type known as element */			


int main(void)
{
	/* Disable global interrupt */
	cli();
		
	/* Initialize main clock: 8 MHz */
	CLKPR = 0x80;
	CLKPR = 0x01;
	
	/* Set PORTC as output to display ADC result */
	DDRL = 0xF0;
	DDRA &= ~(_BV(PA6) | _BV(PA7)); // external switch interrupts

	
	/* Initialize LCD */
	InitLCD(LS_BLINK|LS_ULINE);
	LCDClear();
	
	/* Initialize ADC */
	init_adc();
	free_running_adc();

	/* Initialize DC Motor */
	init_pwm();
	change_pwm_speed(60);
	init_dc_motor();
	
	sei();

	/* Initialize Stepper Motor and Plate Position */
	init_stepper_motor();
	current_plate = BLACK;

	setup(&head, &tail);

	run_dc_motor();

	int pos = 0;
	int pos2 = 0;

	while(1){			

		PORTL = 0x10;

		/* EX HIGH: object not at exit */
		while(!EX){ // when something at the exit stop
			PORTL = 0x20;
			brake_dc_motor();			

			switch(head->e.itemMaterial){
				case(ALUMINUM):
					StepperMotor_Rotate(rotations[current_plate][ALUMINUM]);
					current_plate = ALUMINUM;
					break;
				
				case(STEEL):
					StepperMotor_Rotate(rotations[current_plate][STEEL]);
					current_plate = STEEL;
					break;
				
				case(WHITE):
					StepperMotor_Rotate(rotations[current_plate][WHITE]);
					current_plate = WHITE;
					break;
				
				case(BLACK):
					StepperMotor_Rotate(rotations[current_plate][BLACK]);
					current_plate = BLACK;
					break;
			}

			run_dc_motor();
			mTimer(500);
			item_counter += 1;

			LCDWriteIntXY(pos, 1, head->e.itemMaterial, 1);
			pos += 2;

			dequeue(&head, &tail, &rtnLink);
			free(rtnLink);

			while(item_counter == TOTAL_ITEM){
				disable_dc_motor();
				PORTL = 0xF0;
				mTimer(200);
				PORTL = 0x00;
				mTimer(200);
			}
		}
		

		while(OR){ // when object is at the reflective sensor
			start_conversion();
			PORTL = 0xF0;
	
			if(ADC_result_flag){
				if(ADC_result < ADC_curr_min){
					ADC_curr_min = ADC_result;
				}
				//ADCSRA |= _BV(ADSC);
			}
			
			ADC_result_flag = 0;
			item_adc_ready = 1;
		}

		if(item_adc_ready){
			initLink(&newLink); // creating a new link in the linked list

			disable_adc();

			if(ADC_curr_min >= WHITE_BLACK_BOUND){
				newLink->e.itemMaterial = BLACK; // 1
				black_counter += 1;
				LCDWriteStringXY(pos2, 0, "B");

			} else if(ADC_curr_min >= STEEL_WHITE_BOUND){
				newLink->e.itemMaterial = WHITE; // 3
				white_counter += 1;
				LCDWriteStringXY(pos2, 0, "W");

			} else if(ADC_curr_min >= ALUMINUM_STEEL_BOUND){
				newLink->e.itemMaterial = STEEL; // 2
				steel_counter += 1;
				LCDWriteStringXY(pos2, 0, "S");
				
			} else {
				newLink->e.itemMaterial = ALUMINUM; // 4
				aluminum_counter += 1;
				LCDWriteStringXY(pos2, 0, "A");
			}

			pos2 += 2;

			enqueue(&head, &tail, &newLink);

			
			// LCDWriteIntXY(0,0,item_counter,3);
			// LCDWriteIntXY(5,0,ADC_counter,5);
			// LCDWriteIntXY(12,0,newLink->e.itemMaterial, 1);
			// LCDWriteIntXY(0,1,ADC_min_min,4);
			// LCDWriteIntXY(5,1,ADC_max_min,4);
			LCDWriteIntXY(10,1,ADC_curr_min,4);
			
			item_adc_ready = 0;
			ADC_counter = 0;
			ADC_curr_min = 1023;
		}


		if(ADC_result_flag){
			ADC_result_flag = 0;
			
		}

		/* Stop dc motor */
		if(kill_flag){
			brake_dc_motor();
		}

		/* reset numbers */		
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
	if(ADC_result < ADC_curr_min){
		ADC_curr_min = ADC_result;
	}
	ADC_result_flag = 1;
	ADC_counter += 1;
}


ISR(INT4_vect){ // stop motor
	mTimer(25);
	kill_flag = 1;
	while((PINE & 0x02) == 0x00);
	mTimer(25);	
}

ISR(INT5_vect){
	mTimer(25);
	reset_flag = 1;
	while((PINE & 0x01) == 0x00);
	mTimer(25);	
}

ISR(INT2_vect){ // OR sensor is logic high when object in
	enable_adc();
	start_conversion();
}