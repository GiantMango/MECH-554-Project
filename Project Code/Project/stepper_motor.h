#ifndef STEPPER_MOTOR_H_
#define STEPPER_MOTOR_H_

#define STEPPER_MOTOR_PORT		PORT(C)

#define EN1			              PA5
#define L1			              PA4
#define L2			              PA3
#define EN2			              PA2
#define L3			              PA1
#define L4			              PA0   

// |    |    | EN1 | L1 | L2 | EN2 | L3 | L4 |
#define STEP1                 (STEPPER_MOTOR_PORT = (1<<EN1 | 1<<L1))
#define STEP2                 (STEPPER_MOTOR_PORT = (1<<EN2 | 1<<L3))
#define STEP3                 (STEPPER_MOTOR_PORT = (1<<EN1 | 1<<L2))
#define STEP4                 (STEPPER_MOTOR_PORT = (1<<EN2 | 1<<L4))

#define STEP05                (STEPPER_MOTOR_PORT = (1<<EN1 | 1<<L1 | 1<<EN2 | 1<<L4))
#define STEP15                (STEPPER_MOTOR_PORT = (1<<EN1 | 1<<L1 | 1<<EN2 | 1<<L3))
#define STEP25                (STEPPER_MOTOR_PORT = (1<<EN1 | 1<<L2 | 1<<EN2 | 1<<L3))
#define STEP35                (STEPPER_MOTOR_PORT = (1<<EN1 | 1<<L2 | 1<<EN2 | 1<<L4))

#define STEPS                 (STEP1, STEP2, STEP3, STEP4)
#define HALF_STEPS            (STEP05, STEP1, STEP15, STEP2, STEP25, STEP3, STEP35, STEP4)
#define INIT_STEP             2

#define DEG90                 50
#define DEG180                100

/* GLOBAL VARIABLES */
volatile int current_step;

void init_stepper_motor();
void StepperMotor_CW(int num_steps);
void StepperMotor_CCW(int num_steps);

#endif /* STEPPER_MOTOR_H_ */