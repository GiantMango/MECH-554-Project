#include <avr/io.h>
#include <avr/interrupt.h>
#include "stepper_motor.h"
#include "mTimer.h"

void initStepperMotor(){
	int i = 0;
	
	/* Initialize with 90 steps */
	while (i < 90){
		PORTA = STEP1;
		mTimer(20);
		i += 1;
		PORTA = STEP2;
		mTimer(20);
		i += 1;
		PORTA = STEP3;
		mTimer(20);
		i += 1;
		PORTA = STEP4;
		mTimer(20);
		i += 1;
	}
	// now we are at step 2
	current_step = 2;
}

void StepperMotor_CCW(int num_steps){
	// stay at the current position
	// add 1: 2 -> 3
	// check if 3 is valid, if not zero, reassign it
	// then turn using PORTA
	for (int i = num_steps; i > 0; i--){
		switch (current_step)
		{
			case(1):
				PORTA = STEP2;
				current_step = 2;
				mTimer(20);
				break;
			case(2):
				PORTA = STEP3;
				current_step = 3;
				mTimer(20);
				break;
			case(3):
				PORTA = STEP4;
				current_step = 4;
				mTimer(20);
				break;
			case(4):
				PORTA = STEP1;
				current_step = 1;
				mTimer(20);
				break;
		}
		
	}
}

void StepperMotor_CW(int num_steps){
	// stay at the current position
	// minus 1: 2 -> 1
	// check if 1 is valid, if not zero, reassign it
	// then turn using PORTA
	for (int i = num_steps; i > 0; i--){
		switch (current_step)
		{
			case(1):
				PORTA = STEP4;
				current_step = 4;
				mTimer(20);
				break;
			case(2):
				PORTA = STEP1;
				current_step = 1;
				mTimer(20);
				break;
			case(3):
				PORTA = STEP2;
				current_step = 2;
				mTimer(20);
				break;
			case(4):
				PORTA = STEP3;
				current_step = 3;
				mTimer(20);
				break;
		}
	}
}