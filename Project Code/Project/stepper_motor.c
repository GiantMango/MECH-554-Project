#include <avr/io.h>
#include <avr/interrupt.h>
#include "stepper_motor.h"
#include "timer.h"
#include "sensor.h"
#include "dc_motor.h"
#include "lcd.h"

/* GLOBAL VARIABLES */
volatile unsigned char start_dc_motor_flag;
volatile unsigned char current_step;
volatile unsigned char temp_step;
volatile unsigned char steps[4] = {STEP1, STEP2, STEP3, STEP4};

volatile unsigned char accel_speed[ACCEL_TOTAL_STEPS] = {20, 19, 18, 17, 16,
																												15, 14, 13, 12, 11,
																												10,	9, 8, 7};
volatile unsigned char decel_speed[DECCEL_TOTAL_STEPS] = {8, 9, 10, 11,
																												12, 14, 16, 18, 20};

volatile unsigned char ind[4][4] = {{0, 3, 2, 1},
																	  {1, 0, 3, 2},
																	  {2, 1, 0, 3},
																	  {3, 2, 1, 0}};


/* Initialize Stepper Motor to Black */
void init_stepper_motor(){
	DDRA |= (_BV(PA0) | _BV(PA1) | _BV(PA2) | _BV(PA3) | _BV(PA4) | _BV(PA5));
	PORTL = 0xF0;
	int i = 0;

	while(HE){ // HE: active low
		temp_step = i % 4;
		STEPPER_MOTOR_PORT = (STEPPER_MOTOR_PORT & ~STEPPER_MOTOR_MASK) | steps[temp_step];
		mTimer(18);
		i += 1;
	}
	current_step = temp_step;
}


void StepperMotor_Rotate(int num_steps){
	if(num_steps > 0){
		StepperMotor_CCW(num_steps);
	} else if (num_steps < 0) {
		StepperMotor_CW(-num_steps);
	} // do nothing if num_steps = 0
}


void StepperMotor_CW(int num_steps){ //50 100
	int j = 0;
	for(int i = 0; i < num_steps; i++){
		temp_step = (current_step + i + 1) % 4;
		STEPPER_MOTOR_PORT = (STEPPER_MOTOR_PORT & ~STEPPER_MOTOR_MASK) | steps[temp_step];

		// mTimer(18);

		/* Acceleration and Deceleration Profile */
		if((i) < ACCEL_TOTAL_STEPS){ 
			mTimer(accel_speed[i]);
		} else if ((i) > (num_steps - DECCEL_TOTAL_STEPS - 1)) {
			mTimer(decel_speed[j]);
			j += 1;
		} else {
			mTimer(accel_speed[ACCEL_TOTAL_STEPS - 1]);
		}
		/* End of Accel/Deccel Profile */

	}
	
	current_step = temp_step;

}

void StepperMotor_CCW(int num_steps){
	int j = 0;
	for(int i = 0; i < num_steps; i++){
		temp_step = ind[current_step][(i+1)%4];
		STEPPER_MOTOR_PORT = (STEPPER_MOTOR_PORT & ~STEPPER_MOTOR_MASK) | steps[temp_step];
		
		// mTimer(18);

		// /* Acceleration and Deceleration Profile */
		if((i) < ACCEL_TOTAL_STEPS){ 
			mTimer(accel_speed[i]);
		} else if ((i) > (num_steps - DECCEL_TOTAL_STEPS - 1)) {
			mTimer(decel_speed[j]);
			j += 1;
		} else {
			mTimer(accel_speed[ACCEL_TOTAL_STEPS - 1]);
		}
		/* End of Accel/Deccel Profile */			

	}

	current_step = temp_step;

}