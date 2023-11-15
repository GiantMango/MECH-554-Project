/*
 * sensor.c
 *
 * Created: 2023/11/3 上午 11:12:27
 *  Author: subwa
 */ 

/*
 * HE signal: sensor is under the plate, when black is aligned it's low
 * EX signal: when piece is at exit it's low
 * RL signal: analog (reflection from the object)
 * OR signal: when a piece is at the reflective sensor
 */

#include <avr/io.h>
#include <avr/interrupt.h>
#include "sensor.h"
#include "mTimer.h"

void init_sensors(){

  DDRB &= ~(_BV(DDB6) | _BV(DDB5) | _BV(DDB4));
  
}