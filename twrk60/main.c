#include "Mk60.h"
#include <gpio.h>
#define USEUART2
//#define USEUART3

#ifdef USEUART2
void __attribute__ ((interrupt, no_auto_psv)) _U2RXInterrupt(void) {
  LATAbits.LATA1 = 1;
  IFS1bits.U2RXIF = 0;
}
void __attribute__ ((interrupt, no_auto_psv)) _U2TXInterrupt(void) {
  LATAbits.LATA0 = 1;
  IFS1bits.U2TXIF = 0;
}
#endif

#ifdef USEUART3
void __attribute__ ((interrupt, no_auto_psv)) _U3RXInterrupt(void) {
  LATAbits.LATA1 = 1;
  IFS5bits.U3RXIF = 0;
}
void __attribute__ ((interrupt, no_auto_psv)) _U3TXInterrupt(void) {
  LATAbits.LATA0 = 1;
  IFS5bits.U3TXIF = 0;
}
#endif

void InitClock()
{
  CLKDIVbits.ROI =        0b0;
  CLKDIVbits.DOZE =     0b000;
  CLKDIVbits.DOZEN =      0b0;
  CLKDIVbits.FRCDIV =   0b000;
  CLKDIVbits.PLLPRE =       0; // Vorteiler von 2 => Eingang PLL = 4 MHZ
  PLLFBDbits.PLLDIV =    (58); // 4 * 60 = 240 MHZ
  CLKDIVbits.PLLPOST =      0; // Output / 2 => Fosc = 120 MHz => Fcy = 60 Mips

  while(OSCCONbits.LOCK != 1) {};          // Wait for PLL to lock
}


void InitUART2() {
  // This is an EXAMPLE, so brutal typing goes into explaining all bit sets

  // The HPC16 board has a DB9 connector wired to UART2, so we will
  // be configuring this port only
#ifdef USEUART2
  // configure U2MODE
  U2MODEbits.UARTEN = 0;  // Bit15 TX, RX DISABLED, ENABLE at end of func
  //U2MODEbits.notimplemented;  // Bit14
  U2MODEbits.USIDL = 0; // Bit13 Continue in Idle
  U2MODEbits.IREN = 0;  // Bit12 No IR translation
  U2MODEbits.RTSMD = 0; // Bit11 Simplex Mode
  //U2MODEbits.notimplemented;  // Bit10
  U2MODEbits.UEN = 0;   // Bits8,9 TX,RX enabled, CTS,RTS not
  U2MODEbits.WAKE = 0;  // Bit7 No Wake up (since we don't sleep here)
  U2MODEbits.LPBACK = 0;  // Bit6 No Loop Back
  U2MODEbits.ABAUD = 0; // Bit5 No Autobaud (would require sending '55')

  U2MODEbits.BRGH = 0;  // Bit3 16 clocks per bit period
  U2MODEbits.PDSEL = 0; // Bits1,2 8bit, No Parity
  U2MODEbits.STSEL = 0; // Bit0 One Stop Bit

  // Load a value into Baud Rate Generator.  Example is for 9600.
  // See section 19.3.1 of datasheet.
  //  U2BRG = (Fcy/(16*BaudRate))-1
  //  U2BRG = (37M/(16*9600))-1
  //  U2BRG = 240
  U2BRG = 32;  // 40Mhz osc, 9600 Baud

  // Load all values in for U1STA SFR
  U2STAbits.UTXISEL1 = 0; //Bit15 Int when Char is transferred (1/2 config!)
  U2STAbits.UTXINV = 0; //Bit14 N/A, IRDA config
  U2STAbits.UTXISEL0 = 0; //Bit13 Other half of Bit15
  //U2STAbits.notimplemented = 0; //Bit12
  U2STAbits.UTXBRK = 0; //Bit11 Disabled
  U2STAbits.UTXEN = 0;  //Bit10 TX pins controlled by periph
  U2STAbits.UTXBF = 0;  //Bit9 *Read Only Bit*
  U2STAbits.TRMT = 0; //Bit8 *Read Only bit*
  U2STAbits.URXISEL = 0;  //Bits6,7 Int. on character recieved
  U2STAbits.ADDEN = 0;  //Bit5 Address Detect Disabled
  U2STAbits.RIDLE = 0;  //Bit4 *Read Only Bit*
  U2STAbits.PERR = 0;   //Bit3 *Read Only Bit*
  U2STAbits.FERR = 0;   //Bit2 *Read Only Bit*
  U2STAbits.OERR = 0;   //Bit1 *Read Only Bit*
  U2STAbits.URXDA = 0;  //Bit0 *Read Only Bit*
#endif

#ifdef USEUART3
  // configure U2MODE
  U3MODEbits.UARTEN = 0;  // Bit15 TX, RX DISABLED, ENABLE at end of func
  //U3MODEbits.notimplemented;  // Bit14
  U3MODEbits.USIDL = 0; // Bit13 Continue in Idle
  U3MODEbits.IREN = 0;  // Bit12 No IR translation
  U3MODEbits.RTSMD = 0; // Bit11 Simplex Mode
  //U3MODEbits.notimplemented;  // Bit10
  U3MODEbits.UEN = 0;   // Bits8,9 TX,RX enabled, CTS,RTS not
  U3MODEbits.WAKE = 0;  // Bit7 No Wake up (since we don't sleep here)
  U3MODEbits.LPBACK = 0;  // Bit6 No Loop Back
  U3MODEbits.ABAUD = 0; // Bit5 No Autobaud (would require sending '55')

  U3MODEbits.BRGH = 0;  // Bit3 16 clocks per bit period
  U3MODEbits.PDSEL = 0; // Bits1,2 8bit, No Parity
  U3MODEbits.STSEL = 0; // Bit0 One Stop Bit

  // Load a value into Baud Rate Generator.  Example is for 9600.
  // See section 19.3.1 of datasheet.
  //  U2BRG = (Fcy/(16*BaudRate))-1
  //  U2BRG = (37M/(16*9600))-1
  //  U2BRG = 240
  U3BRG = 32;  // 40Mhz osc, 9600 Baud

  // Load all values in for U1STA SFR
  U3STAbits.UTXISEL1 = 0; //Bit15 Int when Char is transferred (1/2 config!)
  U3STAbits.UTXINV = 0; //Bit14 N/A, IRDA config
  U3STAbits.UTXISEL0 = 0; //Bit13 Other half of Bit15
  //U3STAbits.notimplemented = 0; //Bit12
  U3STAbits.UTXBRK = 0; //Bit11 Disabled
  U3STAbits.UTXEN = 0;  //Bit10 TX pins controlled by periph
  U3STAbits.UTXBF = 0;  //Bit9 *Read Only Bit*
  U3STAbits.TRMT = 0; //Bit8 *Read Only bit*
  U3STAbits.URXISEL = 0;  //Bits6,7 Int. on character recieved
  U3STAbits.ADDEN = 0;  //Bit5 Address Detect Disabled
  U3STAbits.RIDLE = 0;  //Bit4 *Read Only Bit*
  U3STAbits.PERR = 0;   //Bit3 *Read Only Bit*
  U3STAbits.FERR = 0;   //Bit2 *Read Only Bit*
  U3STAbits.OERR = 0;   //Bit1 *Read Only Bit*
  U3STAbits.URXDA = 0;  //Bit0 *Read Only Bit*
#endif
  IPC7 = 0x4400;  // Mid Range Interrupt Priority level, no urgent reason
#ifdef USEUART2
  IFS1bits.U2TXIF = 0;  // Clear the Transmit Interrupt Flag
  IEC1bits.U2TXIE = 1;  // Enable Transmit Interrupts
  IFS1bits.U2RXIF = 0;  // Clear the Recieve Interrupt Flag
  IEC1bits.U2RXIE = 1;  // Enable Recieve Interrupts


  RPOR9bits.RP101R = 3;   //RF5 as U2TX

  U2MODEbits.UARTEN = 1;  // And turn the peripheral on

  U2STAbits.UTXEN = 1;
#endif
#ifdef USEUART3
  IFS5bits.U3TXIF = 0;  // Clear the Transmit Interrupt Flag
  IEC5bits.U3TXIE = 1;  // Enable Transmit Interrupts
  IFS5bits.U3RXIF = 0;  // Clear the Recieve Interrupt Flag
  IEC5bits.U3RXIE = 1;  // Enable Recieve Interrupts


  RPOR9bits.RP101R = 0x001C;   //RF5 as U3TX

  U3MODEbits.UARTEN = 1;  // And turn the peripheral on

  U3STAbits.UTXEN = 1;
#endif

}
void InitPorts() {

  TRISAbits.TRISA0 = 0; // LED 1

  TRISDbits.TRISD5 = 1;
  TRISAbits.TRISA1 = 0; // LED 2

  TRISFbits.TRISF5 = 0; // RF5 output - U2TX pin
  TRISFbits.TRISF4 = 1; // RF4 input - U2RX pin
#ifdef USEUART2
  RPINR19bits.U2RXR = 0b1100100; // RF4 pin - RP100 assigned to U2RX function
#endif

#ifdef USEUART3
  RPINR27bits.U3RXR = 0b1100100; // RF4 pin - RP100 assigned to U3RX function
#endif
}
int main(void)
{
  InitClock();
  InitUART2();
  InitPorts();
  LATAbits.LATA0 = 0;
  LATAbits.LATA1 = 0;
    int block = 0;
    while (1)
    {
      if ((PORTDbits.RD5 != 0) && ( block == 0))
      {
        block = 1;
#ifdef USEUART2
        U2TXREG = 'N';
#endif
#ifdef USEUART3
        U3TXREG = 'N';
#endif
      };
      if (PORTDbits.RD5 == 0)
      {
        block = 0;
      }
}

}
