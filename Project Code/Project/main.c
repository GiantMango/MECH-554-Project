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
volatile unsigned char STATE;
volatile unsigned char in_exit_counter;
volatile unsigned char INT2_counter;
volatile unsigned int ADC_counter;
volatile unsigned char item_counter;

volatile unsigned char aluminum_counter;
volatile unsigned char steel_counter;
volatile unsigned char white_counter;
volatile unsigned char black_counter;

volatile unsigned char current_plate;

volatile unsigned char item_adc_ready;
volatile unsigned char at_exit_flag;
volatile unsigned char reset_flag;
volatile unsigned char task_complete_flag;
volatile unsigned char ready_to_drop_flag;

/* default rotation */							  //|  B  |  A  |  W  |  S  |
volatile signed char rotations[4][4] = {{0, DEG90, DEG180, NEG_DEG90}, 	// current plate black
																				{NEG_DEG90, 0, DEG90, DEG180}, 	// current plate aluminum
																			 	{DEG180, NEG_DEG90, 0 ,DEG90}, 	// current plate white
																			 	{DEG90, DEG180, NEG_DEG90, 0}}; // current plate steel

link *head;			/* The ptr to the head of the queue */
link *tail;			/* The ptr to the tail of the queue */
link *newLink;		/* A ptr to a link aggregate data type (struct) */
link *rtnLink;		/* same as the above */
element eTest;		/* A variable to hold the aggregate data type known as element */			
/* End of Glabal Variables*/

/* Prototype */
void init_INT();


int main(int argc, char *argv[]){
	CLKPR = 0x80;
	CLKPR = 0x01;		//  sets system clock to 8MHz

	STATE = 0;

	cli();		// Disables all interrupts
	
	DDRD = 0b11110000;	// Going to set up INT2 & INT3 on PORTD
	DDRC = 0xFF;		// just use as a display


	// Set up the Interrupt 0,3 options
	//External Interrupt Control Register A - EICRA (pg 110 and under the EXT_INT tab to the right
	// Set Interrupt sense control to catch a rising edge
	EICRA |= _BV(ISC21) | _BV(ISC20);
	EICRA |= _BV(ISC31) | _BV(ISC30);

	//	EICRA &= ~_BV(ISC21) & ~_BV(ISC20); /* These lines would undo the above two lines */
	//	EICRA &= ~_BV(ISC31) & ~_BV(ISC30); /* Nice little trick */


	// See page 112 - EIFR External Interrupt Flags...notice how they reset on their own in 'C'...not in assembly
	EIMSK |= 0x0C;

	/* Initialize LCD */
	InitLCD(LS_BLINK|LS_ULINE);
	LCDClear();
	
	/* Initialize ADC */
	init_adc();
	//free_running_adc();

	/* Initialize DC Motor */
	init_pwm(70);
	init_dc_motor();

	/* Initialize Stepper Motor and Plate Position */
	init_stepper_motor();
	current_plate = BLACK;

	setup(&head, &tail);

	run_dc_motor();

	// Enable all interrupts
	init_INT();
	sei();	// Note this sets the Global Enable for all interrupts

	goto POLLING_STAGE;

	// POLLING STATE
	POLLING_STAGE:
		PORTC |= 0xF0;	// Indicates this state is active
		run_dc_motor();
		switch(STATE){
			case (0) :
				goto POLLING_STAGE;
				break;	//not needed but syntax is correct
			case (1) :
				goto REFLECTIVE_STAGE;
				break;
			case (2) :
				goto BUCKET_STAGE;
				break;
			case (4) :
				goto RESET;
				break;
			case (5) :
				goto END;
			default :
				goto POLLING_STAGE;
		}//switch STATE

	REFLECTIVE_STAGE:
		// Do whatever is necessary HERE
		ADC_curr_min = 1023;
		start_conversion();
		PORTC = 0x01; // Just output pretty lights know you made it here
		//Reset the state variable
		STATE = 0;
		goto POLLING_STAGE;
	
	BUCKET_STAGE:
		// Do whatever is necessary HERE
		brake_dc_motor();

		switch(head->e.itemMaterial){
			case(ALUMINUM):
				StepperMotor_Rotate(rotations[current_plate][ALUMINUM]);
				aluminum_counter += 1;
				current_plate = ALUMINUM;
				break;
			
			case(STEEL):
				StepperMotor_Rotate(rotations[current_plate][STEEL]);
				steel_counter += 1;
				current_plate = STEEL;
				break;
			
			case(WHITE):
				StepperMotor_Rotate(rotations[current_plate][WHITE]);
				white_counter += 1;
				current_plate = WHITE;
				break;
			
			case(BLACK):
				StepperMotor_Rotate(rotations[current_plate][BLACK]);
				black_counter += 1;
				current_plate = BLACK;
				break;
		}

		run_dc_motor();
		mTimer(200);

		dequeue(&head, &tail, &rtnLink);
		free(rtnLink);

		PORTC = 0x04;
		//Reset the state variable
		STATE = 0;
		goto POLLING_STAGE;
	
	RESET:
		PORTC = 0x08;
		STATE = 0;

		/* Resetting all counters */
		ADC_curr_min = 1023;
		ADC_counter = 0;
		aluminum_counter = 0;
		steel_counter = 0;
		black_counter = 0;
		white_counter = 0;

		brake_dc_motor();

		goto POLLING_STAGE;

	END:
		// The closing STATE ... how would you get here?
		PORTC = 0xF0;	// Indicates this state is active
		// Stop everything here...'MAKE SAFE'
		disable_adc();
		disable_dc_motor();
		cli();

	return(0);

}

void init_INT(){
	/* Rising Edge Triggers */
	EIMSK |= (_BV(INT2));									// enable INT2 for OR
	EICRA |= (_BV(ISC21) | _BV(ISC20));
	
	/* Falling Edge Triggers */
	EIMSK |= (_BV(INT4) | _BV(INT5));			// enable INT4 and 5 for reset
	EICRB |= (_BV(ISC51) | _BV(ISC41));
}

/* Interrupt Service Routine*/
ISR(ADC_vect){ //ADC conversion done
	ADC_result = (ADCL >> 6) + (ADCH << 2);
	if(ADC_result < ADC_curr_min){
		ADC_curr_min = ADC_result;
	}
	ADC_result_flag = 1;
	ADC_counter += 1;

	// if OR still high -> object still at the sensor
	if(PINB & 0x02 == 0x02){
		start_conversion();
	} else { // object exits the sensor -> enqueue
		initLink(&newLink);
		if(ADC_curr_min >= WHITE_BLACK_BOUND){
			newLink->e.itemMaterial = BLACK; // 1
			// LCDWriteStringXY(pos2, 0, "B");
		} else if(ADC_curr_min >= STEEL_WHITE_BOUND){
			newLink->e.itemMaterial = WHITE; // 3
			// LCDWriteStringXY(pos2, 0, "W");
		} else if(ADC_curr_min >= ALUMINUM_STEEL_BOUND){
			newLink->e.itemMaterial = STEEL; // 2
			// LCDWriteStringXY(pos2, 0, "S");
		} else {
			newLink->e.itemMaterial = ALUMINUM; // 4
			// LCDWriteStringXY(pos2, 0, "A");
		}			
		enqueue(&head, &tail, &newLink);
		ADC_counter = 0;
	}
}

/* Sensor INT */
ISR(INT2_vect){ // catch OR rising edge
	/* Toggle PORTC bit 2 */
	INT2_counter += 1;
	STATE = 1;
}

ISR(INT3_vect){ //catch EX falling edge
	STATE = 2; // bucket stage
}

/* Switches INT */
ISR(INT4_vect){
	/* Toggle PORTC bit 3 */
	mTimer(25);
	while((PINE & 0x01) == 0x00);
	mTimer(25);
	STATE = 4; // reset
}

ISR(INT5_vect){
	/* Toggle PORTC bit 3 */
	mTimer(25);
	while((PINE & 0x01) == 0x00);
	mTimer(25);
	STATE = 4; // reset
}

// If an unexpected interrupt occurs (interrupt is enabled and no handler is installed,
// which usually indicates a bug), then the default action is to reset the device by jumping
// to the reset vector. You can override this by supplying a function named BADISR_vect which
// should be defined with ISR() as such. (The name BADISR_vect is actually an alias for __vector_default.
// The latter must be used inside assembly code in case <avr/interrupt.h> is not included.
EMPTY_INTERRUPT(BADISR_vect);
