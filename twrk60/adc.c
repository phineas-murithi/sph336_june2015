/*
 * adc1.c
 *
 */
 
#include "adc1.h"


// Configure ADC1 16bit single ended mode
void ADC1_Init16b(void)
{
    SIM_SCGC3 |= SIM_SCGC3_ADC1_MASK; //Gives clock to ADC1
	
    ADC1_CFG1 = (ADC_CFG1_MODE(3) | ADC_CFG1_ADIV(2)); //16bit mode
	ADC1_SC1A = ADC_SC1_ADCH(31); //disable module

        /* Configure ADC pins as ADC Inputs manually */
        
        /*In Kinetis the ADC pins are ADC inputs by default */          
}

uint16 ADC1_Read16b(uint8 channelNumber)
{
	ADC1_SC1A = channelNumber & ADC_SC1_ADCH_MASK;	                /* Write to ADCSC1 to start conversion */
	while ((ADC1_SC2 & 0x80)); 				  	/* Wait if the conversion is in progress */
	while (!(ADC1_SC1A & 0x80)); 					/* Wait until the conversion is complete */
	return ADC1_RA; 
}

