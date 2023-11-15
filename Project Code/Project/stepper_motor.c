#include <avr/io.h>
#include <avr/interrupt.h>
#include "stepper_motor.h"
#include "mTimer.h"
#include "sensor.h"
#include "lcd.h"

/* GLOBAL VARIABLES */
volatile unsigned char current_step;
volatile unsigned char steps[4] = {STEP1, STEP2, STEP3, STEP4};
volatile unsigned char half_steps[8] = {STEP05, STEP1, STEP15, STEP2, STEP25, STEP3, STEP35, STEP4};

/* Initialize Stepper Motor to Black */
void init_stepper_motor(){

	DDRA |= (_BV(PA0) | _BV(PA1) | _BV(PA2) | _BV(PA3) | _BV(PA4) | _BV(PA5));

	int temp = 0;

	while(!HE){ // HE: active low
		STEPPER_MOTOR_PORT = (STEPPER_MOTOR_PORT & ~STEPPER_MOTOR_MASK) | steps[temp % 4];
		temp += 1;
	}

	current_step = INIT_STEP;
}


void StepperMotor_CW (int num_steps){
    while(num_steps > 0){
        STEPPER_MOTOR_PORT = (STEPPER_MOTOR_PORT & ~STEPPER_MOTOR_MASK) | steps[num_steps%4];
        num_steps -= 1;
    }
}

// TODO: Optimize StepperMotor_CW and CCW
void StepperMotor_CCW(int num_steps){
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


