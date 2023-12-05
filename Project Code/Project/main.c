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

#define ALUMINUM_STEEL_BOUND		100
#define STEEL_WHITE_BOUND				700
#define WHITE_BLACK_BOUND				898 //WHITE upper is 880 lower is 824

#define TOTAL_ITEM							48

// #define CALIBRATION
#define CALIBRATION_MODE				0
#define WORK_MODE								1

#ifdef CALIBRATION
#   define DC_MOTOR_SPEED 70
#		define MODE						0
#else
#   define DC_MOTOR_SPEED 125
#		define MODE						1
#endif

/* Global Variable */
volatile unsigned char STATE = 0;
volatile unsigned char in_OR_flag = 0;
unsigned char ramp_down_flag = 0;
unsigned char pause_flag = 0;

unsigned char INT0_counter = 0; // counts enters OR
unsigned char INT1_counter = 0; // counts EX
unsigned char INT4_counter = 0; // pause
unsigned char INT5_counter = 0; // ramp down
unsigned int  ADC_counter = 0;
unsigned char item_counter = 0;
unsigned char BUCKET_counter = 0;

/* Dropped Item Counter */
unsigned char aluminum_counter = 0;
unsigned char steel_counter = 0;
unsigned char white_counter = 0;
unsigned char black_counter = 0;

/* Sorted Item Counter */
unsigned char  queue_aluminum_counter = 0;
unsigned char  queue_steel_counter = 0;
unsigned char  queue_white_counter = 0;
unsigned char  queue_black_counter = 0;

volatile unsigned char current_plate;
volatile unsigned char current_part;
volatile unsigned int	 current_reading;



/* default rotation */							  //|  B  |  A  |  W  |  S  |
volatile signed char rotations[4][4] = {{0, DEG90, DEG180, NEG_DEG90}, 	// current plate black
																				{NEG_DEG90, 0, DEG90, DEG180}, 	// current plate aluminum
																			 	{DEG180, NEG_DEG90, 0 ,DEG90}, 	// current plate white
																			 	{DEG90, DEG180, NEG_DEG90, 0}}; // current plate steel

void categorize();
void calibration();

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

	/* Initialize DC Motor */
	init_pwm(DC_MOTOR_SPEED);
	init_dc_motor();

	/* Initialize Stepper Motor and Plate Position */
	init_stepper_motor();
	current_plate = BLACK;

	setup(&head, &tail);

	if(MODE){
		LCDWriteStringXY(0, 0, "A");
		LCDWriteStringXY(3, 0, "S");
		LCDWriteStringXY(6, 0, "W");
		LCDWriteStringXY(9, 0, "B");
	} else{
		free_running_adc();
	}

	sei();	// Note this sets the Global Enable for all interrupts

	goto POLLING_STAGE;

	POLLING_STAGE:
		PORTL = 0x10;

		if(!pause_flag){
			run_dc_motor();	
		}
		

		if(MODE){
			if(in_OR_flag){
				start_conversion();
				if(!OR){
					categorize();
				}
			}
		} else {
			if(!OR && in_OR_flag){
				PORTL = 0x70;
				disable_adc();
				stop_conversion();
				calibration();
			}
		}
		
		if(item_counter == TOTAL_ITEM){
			mTimer(200);
			STATE = 5;
		}

		if(ramp_down_flag && (size(&head, &tail) == 0)){
			STATE = 3;
		}

		switch(STATE){
			case (0) :
				goto POLLING_STAGE;
				break;	//not needed but syntax is correct
			case (2) :
				goto BUCKET_STAGE;
				break;
			case (3) :
				goto RAMP_DOWN;
				break;
			case (4) :
				goto PAUSE;
				break;
			case (5) :
				goto END;
			default :
				goto POLLING_STAGE;
		}//switch STATE
	

	BUCKET_STAGE:
		STATE = 0;
		PORTL = 0x40;
		BUCKET_counter += 1;
		if(in_OR_flag){
			start_conversion();
			if(!OR){
				categorize();
			}
		}
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
		if(in_OR_flag){
			start_conversion();
			if(!OR){
				categorize();
			}
		}
		
		dequeue(&head, &tail, &rtnLink);
		free(rtnLink);

		item_counter += 1;

		if(MODE){
			LCDWriteIntXY(0, 1, aluminum_counter, 2);
			LCDWriteIntXY(3, 1, steel_counter, 2);
			LCDWriteIntXY(6, 1, white_counter, 2);
			LCDWriteIntXY(9, 1, black_counter, 2);
			if(INT4_counter != 0){
				LCDWriteIntXY(14,1, size(&head, &tail), 2);
			}
		}

		goto POLLING_STAGE;

	
	RAMP_DOWN:
		PORTL = 0xB0;
		mTimer(200);
		brake_dc_motor();
		LCDWriteIntXY(0, 1, aluminum_counter, 2);
		LCDWriteIntXY(3, 1, steel_counter, 2);
		LCDWriteIntXY(6, 1, white_counter, 2);
		LCDWriteIntXY(9, 1, black_counter, 2);

		aluminum_counter = 0;
		steel_counter = 0;
		white_counter = 0;
		black_counter = 0;
		ADC_curr_min = 1023;
		ADC_counter = 0;
		INT0_counter = 0;
		INT1_counter = 0;
		INT4_counter = 0;
		INT5_counter = 0;
		ramp_down_flag = 0;
		goto END;


	PAUSE:
		PORTL = 0xC0;
		brake_dc_motor();

		if(MODE){
			LCDWriteIntXY(0, 1, aluminum_counter, 2);
			LCDWriteIntXY(3, 1, steel_counter, 2);
			LCDWriteIntXY(6, 1, white_counter, 2);
			LCDWriteIntXY(9, 1, black_counter, 2);
			LCDWriteStringXY(14,0,"US");
			LCDWriteIntXY(14,1, size(&head, &tail), 2);
		}

		STATE = 0;
		goto POLLING_STAGE;


	END:
		brake_dc_motor();
		disable_adc();
		disable_dc_motor();
		cli();

	return(0);
}


/* Subroutines */
void categorize(){
	initLink(&newLink);

	if(ADC_curr_min >= WHITE_BLACK_BOUND){
		newLink->e.itemMaterial = BLACK; // 1
	} else if(ADC_curr_min >= STEEL_WHITE_BOUND){
		newLink->e.itemMaterial = WHITE; // 3
	} else if(ADC_curr_min >= ALUMINUM_STEEL_BOUND){
		newLink->e.itemMaterial = STEEL; // 2
	} else {
		newLink->e.itemMaterial = ALUMINUM; // 4
	}
	
	enqueue(&head, &tail, &newLink);

	ADC_curr_min = 1023;
	ADC_counter = 0;
	in_OR_flag = 0;
}

void calibration(){
	if(ADC_curr_min > ADC_max_min){
		ADC_max_min = ADC_curr_min;
	}

	if(ADC_min_min > ADC_curr_min){
		ADC_min_min = ADC_curr_min;
	}
	in_OR_flag = 0;
	LCDWriteIntXY(0,0,ADC_min_min,4);
	LCDWriteIntXY(5,0,ADC_max_min,4);
	LCDWriteIntXY(10,0,ADC_curr_min,4);
	LCDWriteIntXY(10,1,INT0_counter,2);
	ADC_curr_min = 1023;
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
	enable_adc();
	start_conversion();
	INT0_counter += 1;
}

ISR(INT1_vect){ //catch EX falling edge
	INT1_counter += 1;
	STATE = 2; // bucket stage
}

/* Switches INT */
ISR(INT4_vect){
	mTimer(20);
	if(SWITCH1 == 0){
		INT4_counter += 1;
		if(INT4_counter%2 == 1){
			pause_flag = 1;
		} else {
			pause_flag = 0;
		}
		STATE = 4; // PAUSE
		while(SWITCH1 == 0);
		mTimer(20);
	}
}

ISR(INT5_vect){ // RAMP DOWN BUTTON
	mTimer(20);
	if(SWITCH2 == 0){
		INT5_counter += 1;
		while(SWITCH2 == 0);
		mTimer(20);
		intTimer();
	}
}

ISR(TIMER3_COMPA_vect){
	ramp_down_flag = 1;
}