#ifndef DC_MOTOR_H_
#define DC_MOTOR_H_

#define DC_Brake	0x0F
#define DC_Disable	0x00
#define DC_CW		0x0E
#define DC_CCW		0x0D
#define CW			1
#define CCW			0

volatile unsigned int switch_direction_flag;
volatile unsigned int kill_flag;

void init_dc_moter();
void switch_direction();
void disable_dc_motor();

#endif /* DC_MOTOR_H_ */