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
volatile unsigned char steps[4] = {STEP1, STEP2, STEP3, STEP4};
//volatile unsigned char half_steps[8] = {STEP05, STEP1, STEP15, STEP2, STEP25, STEP3, STEP35, STEP4};

volatile unsigned int accel_speed[ACCEL_TOTAL_STEPS] = {20, 19, 18, 17, 16,
																												15, 14, 13, 12, 11,
																												10, 9, 8};
volatile unsigned int decel_speed[DECCEL_TOTAL_STEPS] = {9, 11, 13, 15, 17, 20};


/* Initialize Stepper Motor to Black */
void init_stepper_motor(){

	DDRA |= (_BV(PA0) | _BV(PA1) | _BV(PA2) | _BV(PA3) | _BV(PA4) | _BV(PA5));

	PORTL = 0xF0;
	int i = 0;

	while(HE){ // HE: active low
		STEPPER_MOTOR_PORT = (STEPPER_MOTOR_PORT & ~STEPPER_MOTOR_MASK) | steps[i % 4];
		mTimer(15);
		i += 1;
	}

	current_step = i % 4;
}


void StepperMotor_Rotate(int num_steps){
		if(num_steps > 0){
			StepperMotor_CCW(num_steps);
		} else if (num_steps < 0) {
			StepperMotor_CW(-num_steps);
		} // do nothing if num_steps = 0
}



void StepperMotor_CW(int num_steps){ // 12  24  60
    int i = 0;
		int j = 0;
    while(i < num_steps){
				STEPPER_MOTOR_PORT = (STEPPER_MOTOR_PORT & ~STEPPER_MOTOR_MASK) | steps[(current_step+i)%4];
				// uTimer(15000);
				// mTimer(15);

				/* Acceleration and Deceleration Profile */
				if(num_steps <= 2*ACCEL_TOTAL_STEPS){
						if(i <= num_steps/2){ 
							mTimer(accel_speed[i]);
						} else if (i > num_steps/2 ) {
							mTimer(decel_speed[num_steps - i - 1]);
						}
				} else {
						if(i < ACCEL_TOTAL_STEPS){ 
							mTimer(accel_speed[i]);
						} else if (i > (num_steps - DECCEL_TOTAL_STEPS - 1)) {
							mTimer(decel_speed[j]);
							j += 1;
						} else {
							mTimer(accel_speed[ACCEL_TOTAL_STEPS - 1]);
						}
				}
				/* End of Accel/Deccel Profile */
				
				i += 1;
    }

		if(((num_steps - i) < 15) && !start_dc_motor_flag){
			run_dc_motor();
			start_dc_motor_flag = 1;
		}
		
		current_step = (current_step+i)%4;

}


void StepperMotor_CCW(int num_steps){
    int i = 0;
    while(i < num_steps){
        STEPPER_MOTOR_PORT = (STEPPER_MOTOR_PORT & ~STEPPER_MOTOR_MASK) | steps[3 - (current_step+i)%4];
				//uTimer(15000);
				// mTimer(15);
				/* Acceleration and Deceleration Profile */
				if(num_steps <= 2*ACCEL_TOTAL_STEPS){
						if(i <= num_steps/2){ 
							mTimer(accel_speed[i]);
						} else if (i > num_steps/2 ) {
							mTimer(accel_speed[num_steps - i - 1]);
						}
				} else {
						if(i < ACCEL_TOTAL_STEPS){ 
							mTimer(accel_speed[i]);
						} else if (i > (num_steps - ACCEL_TOTAL_STEPS-1)) {
							mTimer(accel_speed[num_steps - i - 1]);
						} else {
							mTimer(accel_speed[ACCEL_TOTAL_STEPS-1]);
						}
				}
				/* End of Accel/Deccel Profile */

				i += 1;
		}

		if(((num_steps - i) < 15) && !start_dc_motor_flag){
			run_dc_motor();
			start_dc_motor_flag = 1;
		}

		current_step = 3 - (current_step+i)%4;
}



