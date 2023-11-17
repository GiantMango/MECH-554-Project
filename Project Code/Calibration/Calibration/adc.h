#ifndef ADC_H_
#define ADC_H_

/* GLOBAL VARIABLES */
volatile unsigned int ADC_result;
volatile unsigned int ADC_min_min = 1023;
volatile unsigned int ADC_max_min = 0;
volatile unsigned int ADC_curr_min = 1023;
volatile unsigned char ADC_result_flag = 0;

/* FUNCTIONS */
void init_adc();
void start_conversion();
void stop_conversion();
void disable_adc();
void enable_adc();
void free_running_adc();

#endif /* ADC_H_ */