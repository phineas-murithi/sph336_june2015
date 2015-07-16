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

#endif /* ADC1_H_ */


