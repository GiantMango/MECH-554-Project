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

volatile unsigned char aluminum_counter = 0;
volatile unsigned char steel_counter = 0;
volatile unsigned char white_counter = 0;
volatile unsigned char black_counter = 0;

volatile unsigned char current_plate;
volatile unsigned char current_part;
volatile unsigned int	 current_reading;
volatile signed char extra_steps;
volatile unsigned char extra_plate;
volatile unsigned char extra_piece;

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
	init_pwm(70);
	init_dc_motor();

	/* Initialize Stepper Motor and Plate Position */
	init_stepper_motor();
	current_plate = BLACK;

	setup(&head, &tail);

	// LCDWriteStringXY(0, 0, "A");
	// LCDWriteStringXY(3, 0, "S");
	// LCDWriteStringXY(6, 0, "W");
	// LCDWriteStringXY(9, 0, "B");

	sei();


	goto POLLING_STAGE; // STATE = 0

	// POLLING STATE
	POLLING_STAGE:
		PORTL = 0x10;

		run_dc_motor();

		LCDWriteIntXY(8,0,INT1_counter,2);

		// LCDWriteIntXY(0, 1, aluminum_counter, 2);
		// LCDWriteIntXY(3, 1, steel_counter, 2);
		// LCDWriteIntXY(6, 1, white_counter, 2);
		// LCDWriteIntXY(9, 1, black_counter, 2);

		if(!OR && in_OR_flag){
			PORTL = 0x70;
			disable_adc();
			stop_conversion();
			
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

			// LCDWriteIntXY(10,1,ADC_counter,5);
			// LCDWriteIntXY(5,0,ADC_curr_min,4);

			ADC_curr_min = 1023;
			ADC_counter = 0;
			in_OR_flag = 0;
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


	REFLECTIVE_STAGE: // STATE = 1
		PORTL = 0x20;

		LCDWriteIntXY(10,0,INT0_counter,2);

		enable_adc();
		start_conversion();
		
		//Reset the state variable
		STATE = 0;
		goto POLLING_STAGE;
	

	BUCKET_STAGE:  // STATE = 2
		PORTL = 0x40;

		LCDWriteIntXY(14,0,INT1_counter,2);

		brake_dc_motor();

		LCDWriteIntXY(0,0,head->e.itemMaterial,1);
		LCDWriteIntXY(0,1,head->next->e.itemMaterial,1);

		extra_steps = 0;
		extra_plate = 0;
		extra_piece = 0;

// /* default rotation */							  //|  B  |  A  |  W  |  S  |
// volatile signed char rotations[4][4] = {{0, DEG90, DEG180, NEG_DEG90}, 	// current plate black
// 																				{NEG_DEG90, 0, DEG90, DEG180}, 	// current plate aluminum
// 																			 	{DEG180, NEG_DEG90, 0 ,DEG90}, 	// current plate white
// 																			 	{DEG90, DEG180, NEG_DEG90, 0}}; // current plate steel

		if((head->e.itemMaterial-1) == head->next->e.itemMaterial){
			same_dir_flag = 1;
			same_cw_flag = 1;
			extra_steps = NEG_DEG90;
			extra_piece = 1;
		} else if ((head->e.itemMaterial+1) == head->next->e.itemMaterial){
			same_dir_flag = 1;
			same_ccw_flag = 1;
			extra_steps = DEG90;
			extra_piece = 1;
		}

		switch(head->e.itemMaterial){
			case(ALUMINUM):
				StepperMotor_Rotate(rotations[current_plate][ALUMINUM] + extra_steps);
				aluminum_counter += 1;
				current_plate = (ALUMINUM + extra_plate) % 4;
				break;
			
			case(STEEL):
				StepperMotor_Rotate(rotations[current_plate][STEEL] + extra_steps);
				steel_counter += 1;
				current_plate = (STEEL + extra_plate) % 4;
				break;
			
			case(WHITE):
				StepperMotor_Rotate(rotations[current_plate][WHITE] + extra_steps);
				white_counter += 1;
				current_plate = (WHITE + extra_plate) % 4;
				break;
			
			case(BLACK):
				StepperMotor_Rotate(rotations[current_plate][BLACK] + extra_steps);
				black_counter += 1;
				current_plate = (BLACK + extra_plate) % 4;
				break;
		}

		run_dc_motor();

		// LCDWriteIntXY(0,0,STATE, 1);
		
		dequeue(&head, &tail, &rtnLink);
		free(rtnLink);

		if(same_dir_flag){
			dequeue(&head, &tail, &rtnLink);
			free(rtnLink);
			PORTL = 0xC0;
			mTimer(1000);
		}

		item_counter += extra_piece + 1;
		same_dir_flag = 0;
		same_cw_flag = 0;
		same_ccw_flag = 0;

		//Reset the state variable
		STATE = 0;
		goto POLLING_STAGE;


	

	RESET: // STATE = 4
		PORTL = 0xF0;

		brake_dc_motor();

		/* Resetting all counters */
		clearQueue(&head, &tail);
		setup(&head, &tail);
		ADC_curr_min = 1023;
		ADC_counter = 0;
		aluminum_counter = 0;
		steel_counter = 0;
		black_counter = 0;
		white_counter = 0;
		INT1_counter = 0;
		INT0_counter = 0;


	END: // STATE = 5
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
	INT0_counter += 1;
	STATE = 1;
}

ISR(INT1_vect){ //catch EX falling edge
	INT1_counter += 1;
	STATE = 2; // bucket stage
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
