#ifndef ADC_H_
#define ADC_H_

/* GLOBAL VARIABLES */
volatile unsigned char ADC_result;
volatile unsigned int ADC_result_flag;

/* FUNCTIONS */
void init_adc();
void start_adc();
void stop_adc();
void free_running_adc();

#endif /* ADC_H_ */