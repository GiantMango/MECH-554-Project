﻿#ifndef ADC_H_
#define ADC_H_

/* GLOBAL VARIABLES */
volatile unsigned int ADC_result;
volatile unsigned char ADC_result_flag;
volatile unsigned int ADC_min_min = 1023;
volatile unsigned int ADC_max_min = 1023;
volatile unsigned int ADC_curr_min = 1023;

/* FUNCTIONS */
void init_adc();
void start_adc();
void stop_adc();
void free_running_adc();

#endif /* ADC_H_ */