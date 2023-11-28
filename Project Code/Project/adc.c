/****** ADC ******/
#include <avr/io.h>
#include <avr/interrupt.h>

/* Initialize ADC in one conversion mode */
void init_adc(){	
	/* Config ADC: default ADC input is ADC0 in PORTF 0 (Single Conversion) */
	ADCSRA |= _BV(ADEN);					// enable ADC | ADEN: ADC Enable
	ADCSRA |= _BV(ADIE);					// enable interrupt of ADC | ADIE: ADC Interrupt enable (raised when conversion done)
	ADMUX |= _BV(ADLAR) | _BV(REFS0);		// ADLAR: ADC Left adjust result | REFS0: AVCC with external capacitor at AREF pin
	// Default ADC Clock Pre-scaler = 2 | ADC Clk = 8MHz / 2 = 4 MHz
} // end init_adc

/* Start ADC Conversion */
void start_conversion(){
	/* Initialize ADC and start one conversion at the beginning */
	ADCSRA |= _BV(ADSC);					// ADSC: ADC Start Conversion Bit
} // end start_adc

/* Stop ADC Conversion */
void stop_conversion(){
	ADCSRA &= ~(_BV(ADSC));		// clear ADSC
}// end stop_adc

void free_running_adc(){
	ADCSRA |= _BV(ADATE);					// ADATE: ADC Auto Trigger Enable
}

void disable_adc(){
	ADCSRA &= ~_BV(ADEN);
}

void enable_adc(){
	ADCSRA |= _BV(ADEN);
}