/*
 * adc1.h
 *
 */
 
#ifndef ADC1_H_
#define ADC1_H_

#include "MK60DZ10.h"
#include "adc16.h"

//Configuration
void ADC1_Init16b(void);
uint16 ADC1_Read16b(uint8 channelNumber);

//Calibration
uint_8 ADC_Cal(ADC_MemMapPtr adcmap);

#endif /* ADC1_H_ */
