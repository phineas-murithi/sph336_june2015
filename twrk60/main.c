/*
 * File: main.c
 *
 * Copyright (c) 2015 David Muriuki
 * see the LICENCE file
 */

#include "main.h"

int main(void){	
	//whether to blink LED2
	uint8_t blink=1;
	//temporally UART data holder
	uint8_t byte=0;

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
			else uartsend((uint8_t)byte); //echo back the data
			if(blink){toggle_LED2(); delay(); toggle_LED4(); delay();} //show activity on LED2 and LED4
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
