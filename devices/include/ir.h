/*
 * adc.h
 *
 *  Created on: Oct 21, 2026
 *      Author: gapelaez
 */

#ifndef ADC_H_
#define ADC_H_



#endif /* ADC_H_ */

void adc_ir_init(void);
int adc_ir_read(void);
float fix(int input);
void calibrate_ir();
float perform_dist_measure(int angle);
