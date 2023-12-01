// READ EVERY SINGLE WORD IN THIS PIECE OF CODE...IF YOU DON'T YOU WILL NOT UNDERSTAND THIS!!!!!!!
// READ EVERY SINGLE WORD IN THIS PIECE OF CODE...IF YOU DON'T YOU WILL NOT UNDERSTAND THIS!!!!!!!
// READ EVERY SINGLE WORD IN THIS PIECE OF CODE...IF YOU DON'T YOU WILL NOT UNDERSTAND THIS!!!!!!!
// READ EVERY SINGLE WORD IN THIS PIECE OF CODE...IF YOU DON'T YOU WILL NOT UNDERSTAND THIS!!!!!!!
// READ EVERY SINGLE WORD IN THIS PIECE OF CODE...IF YOU DON'T YOU WILL NOT UNDERSTAND THIS!!!!!!!
// READ EVERY SINGLE WORD IN THIS PIECE OF CODE...IF YOU DON'T YOU WILL NOT UNDERSTAND THIS!!!!!!!

// Open up the document in START -> WinAVR -> AVR LibC -> User Manual -> avr/interrupt.h
// Chapter 15, in Full Manual... THIS HAS A LOT OF IMPORTANT INFO...I have mentioned this at least 3 times!!!

// For those that are still having major problems, I've seen about 1/3 of the class with major problems in
// code structure. If you are still having major problems with your code, it's time to do a VERY quick overhaul.
// I've provided a skeleton structure with an example using two input capture interrupts on PORTDA0 and A3
// Please try this in the debugger.

// Create a watch variable on STATE. To do this right click on the variable STATE and then
// Add Watch 'STATE'. You can see how the variable changes as you click on PINDA0 or PINDA3. Note that the interrupt
// catches a rising edge. You modify this to suit your needs.

#include <avr/interrupt.h>
#include <avr/io.h>
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

#define ALUMINUM_STEEL_BOUND		200
#define STEEL_WHITE_BOUND				700
#define WHITE_BLACK_BOUND				915

#define TOTAL_ITEM							48

/* Global Variable */
volatile unsigned char STATE = 0;
volatile unsigned char in_OR_flag = 0;

volatile unsigned char INT0_counter = 0; // counts enters OR
volatile unsigned char INT1_counter = 0; // counts EX
volatile unsigned char INT5_counter = 0; // counts leaves OR
volatile unsigned int  ADC_counter = 0;
volatile unsigned char item_counter = 0;
volatile unsigned char BUCKET_counter = 0;

volatile unsigned char aluminum_counter = 0;
volatile unsigned char steel_counter = 0;
volatile unsigned char white_counter = 0;
volatile unsigned char black_counter = 0;

volatile unsigned char ADC_max_min;

/* default rotation */							  //|  B  |  A  |  W  |  S  |
volatile signed char rotations[4][4] = {{0, DEG90, DEG180, NEG_DEG90}, 	// current plate black
																				{NEG_DEG90, 0, DEG90, DEG180}, 	// current plate aluminum
																			 	{DEG180, NEG_DEG90, 0 ,DEG90}, 	// current plate white
																			 	{DEG90, DEG180, NEG_DEG90, 0}}; // current plate steel

void categorize();

link *head;			/* The ptr to the head of the queue */
link *tail;			/* The ptr to the tail of the queue */
link *newLink;		/* A ptr to a link aggregate data type (struct) */
link *rtnLink;		/* same as the above */
element eTest;		/* A variable to hold the aggregate data type known as element */			
/* End of Glabal Variables*/


int main(int argc, char *argv[]){
	CLKPR = 0x80;
	CLKPR = 0x01;		//  sets system clock to 8MHz

	STATE = 0;

	cli();		// Disables all interrupts
	
	DDRL = 0xF0; // for LED

	/* Initialize LCD */
	InitLCD(LS_BLINK|LS_ULINE);
	LCDClear();
	
	/* Sensor */
	init_sensors();

	/* Initialize ADC */
	init_adc();
	free_running_adc();

	/* Initialize DC Motor */
	init_pwm(100);
	init_dc_motor();

	/* Initialize Stepper Motor and Plate Position */
	init_stepper_motor();
	current_plate = BLACK;

	setup(&head, &tail);

	// LCDWriteStringXY(0, 0, "A");
	// LCDWriteStringXY(3, 0, "S");
	// LCDWriteStringXY(6, 0, "W");
	// LCDWriteStringXY(9, 0, "B");

	sei();	// Note this sets the Global Enable for all interrupts

	goto POLLING_STAGE;

	// POLLING STATE
	POLLING_STAGE:
		PORTL = 0x10;
		// LCDWriteIntXY(0,0,STATE, 1);
		// LCDWriteIntXY(10,0,INT2_counter,2);
		// LCDWriteIntXY(14,0,INT3_counter,2);

		run_dc_motor();

		// LCDWriteIntXY(0, 1, aluminum_counter, 2);
		// LCDWriteIntXY(3, 1, steel_counter, 2);
		// LCDWriteIntXY(6, 1, white_counter, 2);
		// LCDWriteIntXY(9, 1, black_counter, 2);

		if(!OR && in_OR_flag){
			PORTL = 0x70;
			disable_adc();
			stop_conversion();
			categorize();
		}

		if(item_counter == TOTAL_ITEM){
			mTimer(200);
			STATE = 5;
			goto END;
		}

		switch(STATE){
			case (0) :
				goto POLLING_STAGE;
				break;	//not needed but syntax is correct
			case (4) :
				goto RESET;
				break;
			case (5) :
				goto END;
			default :
				goto POLLING_STAGE;
		}//switch STATE
		

	RESET:
		PORTL = 0xF0;

		brake_dc_motor();

		/* Resetting all counters */
		clearQueue(&head, &tail);
		setup(&head, &tail);
		ADC_curr_min = 1023;
		ADC_counter = 0;
		item_counter = 0;
		aluminum_counter = 0;
		steel_counter = 0;
		black_counter = 0;
		white_counter = 0;
		INT1_counter = 0;
		INT0_counter = 0;


	END:
		disable_adc();
		disable_dc_motor();
		cli();

		for(int i = 0; i < 10; i++){
			PORTL = 0xF0;
			mTimer(500);
			PORTL = 0x00;
			mTimer(500);
		}

	return(0);
}


/* Subroutines */

/* Interrupt Service Routine*/
EMPTY_INTERRUPT(BADISR_vect);

ISR(ADC_vect){ //ADC conversion done
	ADC_result = (ADCL >> 6) + (ADCH << 2);
	if(ADC_result < ADC_curr_min){
		ADC_curr_min = ADC_result;
	}

	ADC_counter += 1;
	in_OR_flag = 1;
}

/* Sensor INT */
ISR(INT0_vect){ // OR sensor is logic high when object in
	enable_adc();
	start_conversion();
	INT0_counter += 1;
	LCDWriteIntXY(10,0,INT0_counter,2);
}

/* Switches INT */
ISR(INT4_vect){
	mTimer(25);
	while(!SWITCH1);
	mTimer(25);
	STATE = 4; // reset
}

// ISR(INT5_vect){ // catch OR rising edge
// 	INT5_counter += 1;
// 	STATE = 3; 
// }
