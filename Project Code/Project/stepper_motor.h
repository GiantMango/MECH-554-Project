#ifndef STEPPER_MOTOR_H_
#define STEPPER_MOTOR_H_

#define STEPPER_MOTOR_PORT		PORT(A)
#define STEPPER_MOTOR_MASK    0x3F

// |    |    | EN1 | L1 | L2 | EN2 | L3 | L4 |
#define STEP1                 0b00110000
#define STEP2                 0b00000110
#define STEP3                 0b00101000
#define STEP4                 0b00000101

#define STEP05                0b00110101
#define STEP15                0b00110110
#define STEP25                0b00101110
#define STEP35                0b00101101

#define INIT_STEP             2

#define DEG90                 50
#define DEG180                100

void init_stepper_motor();
void StepperMotor_CW(int num_steps);
void StepperMotor_CCW(int num_steps);

#endif /* STEPPER_MOTOR_H_ */