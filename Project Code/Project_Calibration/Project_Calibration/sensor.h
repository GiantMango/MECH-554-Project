#ifndef SENSOR_H_
#define SENSOR_H_

#define HE      (PINB & 0x40)
#define EX      (PINB & 0x20)
#define OR      (PINB & 0x10)

void init_sensors();

#endif /* SENSOR_H_ */
