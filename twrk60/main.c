/*
 * File: main.c
 *
 * Copyright (c) 2015 David Muriuki
 * see the LICENCE file
 */

#include "main.h"
uint8_t bits;
uint8_t byte=0x0;

void display(uint8_t byte)
{
	//reading the 4 bits into an array
	char bits[4];
    	int i = 0;
	for (i=0; i<4; i++) 
	{
	 bits[i] = (byte >> i) & 1;
	}
	//mask for the 4 LEDS on a 32-bit register
	uint32_t BYTE_LEDS;
	BYTE_LEDS = bits[0]<<11 | bits[1]<<28 | bits[2]<<29 | bits[3]<<10;
	
	GPIOA->PDOR.word_reg = ~ BYTE_LEDS;
}

int main(void){	
	//whether to blink LED2
	//uint8_t blink=1;
	//temporally UART data holder

	//initialize system
	SystemInit();

	//initialize UART5 with 8-N-1 settings, 57600 baudrate
	init_uart(UART5_BASE_PTR,periph_clk_khz,57600);

	//clear all interrupts and enable interrupts
	nvic->ICPR[2] |= 1 << (87 & 0x1F); //Clear Pending Interrupts
	nvic->ISER[2] |= 1 << (87 & 0x1F); //Enable Interrupts
	
	//initialize GPIO ports
	gpio_init();

	//Loop forever
	while(1)
	{
		//use polling method to echo back data when available
		if(data_available()){ 
			byte = uart_read();
			if(byte==0xD) puts((uint8_t *)"\r\n"); //send new line character
			
			//0 to 9 ascii code in hexadecimal 
			else if ((byte >= 0x30) && (byte <= 0x39)) 
			{byte = byte - 0x30; display(byte);}
			
			//small case letters a to f ascii code in hexadecimal
			else if ((byte >= 0x61) && (byte <= 0x66))
			{byte = byte - 0x57; display(byte);}
			  
			 else {display(byte);}
		}
	}
}

/*
	brief  Silly delay
*/
void delay(void)
{
  volatile unsigned int i,j;

  for(i=0; i<1000; i++)
  {
	for(j=0; j<300; j++)
      __asm__("nop");
  }
}

