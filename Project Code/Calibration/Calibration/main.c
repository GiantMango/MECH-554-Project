#include <avr/io.h>
#include <avr/interrupt.h>
#include "lcd.h"
#include "mTimer.h"
#include "dc_motor.h"
#include "sensor.h"

int main(void)
{
	/* Disable global interrupt */
	cli();
		
	/* Initialize main clock: 8 MHz */
	CLKPR = 0x80;
	CLKPR = 0x01;

	InitLCD(LS_BLINK|LS_ULINE);
	LCDClear();	
	init_adc();
	free_running_adc();

	sei();

	while(1){			
		LCDWriteIntXY(0,0,OR,1);	
		
		while(!OR){
			ADCSRA &= ~_BV(ADSC);
            if(ADC_curr_min > ADC_max_min){
                ADC_max_min = ADC_curr_min;
            }
            if (ADC_curr_min < ADC_min_min){
                ADC_min_min = ADC_curr_min;
            }

            LCDWriteIntXY(0,1,ADC_min_min,4);
		    LCDWriteIntXY(5,1,ADC_max_min,4);
		    LCDWriteIntXY(10,1,ADC_result,4);
            ADC_curr_min = 1023;
		}
		
		if(ADC_result_flag){
			ADC_result_flag = 0x00;
		}
		

				
	}
}

ISR(INT2_vect){ //Execute ADC when OR rising edge
	ADCSRA |= _BV(ADSC);
}

ISR(ADC_vect){ //ADC conversion done
	ADC_result = (ADCL >> 6) + (ADCH << 2);
	if(ADC_result < ADC_max_min){
		ADC_max_min = ADC_result;
	}
	if(ADC_result < ADC_curr_min){
		ADC_curr_min = ADC_result;
	}
	if(ADC_max_min < ADC_min_min){
		ADC_min_min = ADC_max_min;
	}
	ADC_result_flag = 1;
	ADC_counter += 1;
}