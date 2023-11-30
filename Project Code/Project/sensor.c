/*
 * sensor.c
 *
 * Created: 2023/11/3 上午 11:12:27
 *  Author: subwa
 */ 

/*
 * HE signal: sensor is under the plate, when black is aligned it's low
 * EX signal: low when piece is at exit
 * RL signal: analog (reflection from the object)
 * OR signal: high when a piece is at the reflective sensor
 */

#include <avr/io.h>
#include <avr/interrupt.h>
#include "sensor.h"
#include "timer.h"

void init_sensors(){
  // OR
  EIMSK |= (_BV(INT0));
  EICRA |= (_BV(ISC01) | _BV(ISC00)); // rising edge

  // EX
  EIMSK |= (_BV(INT1));		
	EICRA |= (_BV(ISC11));  // falling edge

  // GPIO
  DDRB &= ~(_BV(DDB6) | _BV(DDB5) | _BV(DDB4));
}