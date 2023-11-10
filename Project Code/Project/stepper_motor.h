#ifndef STEPPER_MOTOR_H_
#define STEPPER_MOTOR_H_

#define STEP1       0b00000011
#define STEP2       0b00011000
#define STEP3       0b00000101
#define STEP4       0b00101000
#define STEPS       (STEP1, STEP2, STEP3, STEP4)
#define INIT_STEP   2
#define DEG30       17
#define DEG60       33
#define DEG90       100
#define DEG180      200

/* GLOBAL VARIABLES */
volatile int current_step;
//volatile int STEP[4] = {STEP1, STEP2, STEP3, STEP4};

void init_stepper_motor();
void StepperMotor_CW(int num_steps);
void StepperMotor_CCW(int num_steps);


#endif /* STEPPER_MOTOR_H_ */