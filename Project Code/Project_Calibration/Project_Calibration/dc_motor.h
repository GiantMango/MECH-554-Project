#ifndef DC_MOTOR_H_
#define DC_MOTOR_H_

#define DC_MOTOR_PORT   PORT(B)
#define DC_MOTOR_MASK   0x0F

#define DC_BRAKE	      0x0F
#define DC_DISABLE	    0x00
#define DC_FORWARD      0x0B
#define DC_BACKWARD     0x0E

#define SWITCH1         (PINE & 0x01)
#define SWITCH2         (PINE & 0x02)

volatile unsigned int kill_flag;

void init_dc_motor();
void disable_dc_motor();
void run_dc_motor();
void brake_dc_motor();

#endif /* DC_MOTOR_H_ */