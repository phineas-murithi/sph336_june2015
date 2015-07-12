/*
phineas tutorial 1
 * File: main.c
 *
 * Copyright (c) 2015 David Muriuki
 * see the LICENCE file
 */
#include "main.h"
int main(void){	
	//whether to blink LED2
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
		if(data_available())
		{ 
			byte = uart_read();
			if(byte==0xD)
			{ 
				puts((uint8_t *)"\r\n"); 
				//send new line character
			}
			else if ((byte >= 0x30) && (byte <= 0x39)) 
			{
				byte = byte - 0x30; 
				display(byte);
			
			//small case letters a to f ascii code in hexadecimal
			}
			else if ((byte >= 0x61) && (byte <= 0x66))
			{
				byte = byte-0x57;
				display(byte);
				}
				else if((byte >= 0x41) && (byte <= 0x46))
				{
					byte = byte-0x37; 
					display(byte);
				}
				else{}
				
					void display (unit8_t data)
					{char bit0,bit1,bit2,bit3;unit30_t LEDS;
					bit0 = data>>0&1;bit1 = data>>1&1;
					bit2 =data>>2&1;bit3 = data>>3&1;
					LEDS=bit0<<11 |bit1<<28 |bit2<<29 |bit3<<10;
					/*declare LEDs mask*/
				
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

