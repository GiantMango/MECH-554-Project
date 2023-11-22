#include <avr/io.h>

void mTimer(int count){
	int i;
	i = 0;

	TCCR1B |= (_BV(CS11) | _BV(WGM12)); // Setting bit 3 in the timer/counter 1 control register
	OCR1A = 0x03E8; // Set output compare register for 1000 ticks which equals to 1ms
	TCNT1 = 0x0000; // Sets initial value of timer counter to 0
	
	//TIMSK1 |= _BV(OCIE1A); // Setting bit 1 in the timer/counter 1 interrupt mask register
	TIFR1 |= _BV(OCF1A); // clear the interrupt flag in the bit 1 by setting the bit 1
	
	while(i < count){
		if((TIFR1 & 0x02) == 0x02){ // check if the TIFR1 interrupt in bit 1 is raised
			TIFR1 |= _BV(OCF1A); // clear the interrupt flag by written ONE to the bit
			i++;
		}
	}
	return;
}

void uTimer(int count){
	int i;
	i = 0;

	TCCR1B |= (_BV(CS11) | _BV(WGM12)); // Setting bit 3 in the timer/counter 1 control register
	OCR1A = 0x0001; // Set output compare register for 1000 ticks which equals to 1ms
	TCNT1 = 0x0000; // Sets initial value of timer counter to 0
	
	//TIMSK1 |= _BV(OCIE1A); // Setting bit 1 in the timer/counter 1 interrupt mask register
	TIFR1 |= _BV(OCF1A); // clear the interrupt flag in the bit 1 by setting the bit 1
	
	while(i < count){
		if((TIFR1 & 0x02) == 0x02){ // check if the TIFR1 interrupt in bit 1 is raised
			TIFR1 |= _BV(OCF1A); // clear the interrupt flag by written ONE to the bit
			i++;
		}
	}
	return;
}