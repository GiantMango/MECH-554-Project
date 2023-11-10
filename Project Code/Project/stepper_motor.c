#include <avr/io.h>
#include <avr/interrupt.h>
#include "stepper_motor.h"
#include "mTimer.h"
#include "sensor.h"
#include "lcd.h"

/* Initialize Stepper Motor to Black */
void init_stepper_motor(){
	while(HE){
		// TODO: ADD initialize stepper motor rotating function with half step commands
		StepperMotor_CW(1);
	}
}

// TODO: Optimize StepperMotor_CW and CCW
void StepperMotor_CCW(int num){
	for (int i = num; i > 0; i--){
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