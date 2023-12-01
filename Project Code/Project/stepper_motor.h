#ifndef STEPPER_MOTOR_H_
#define STEPPER_MOTOR_H_

#define STEPPER_MOTOR_PORT		PORT(A)
#define STEPPER_MOTOR_MASK    0x3F

// |Bit7|Bit6|Bit5 |Bit4|Bit3 |Bit2|Bit1|Bit0|
// |    |    | EN1 | L1 | L2 | EN2 | L3 | L4 |
#define STEP1                 0b00110101
#define STEP2                 0b00110110
#define STEP3                 0b00101110
#define STEP4                 0b00101101

#define NEG_DEG90             -50
#define NEG_DEG180            -100
#define DEG90                 50
#define DEG180                100

#define ACCEL_TOTAL_STEPS     14
#define DECCEL_TOTAL_STEPS    8


void init_stepper_motor();
void StepperMotor_Rotate(int num_steps);
void StepperMotor_CW(int num_steps);
void StepperMotor_CCW(int num_steps);
void ramp(int i, int j, int num_steps);

#endif /* STEPPER_MOTOR_H_ */