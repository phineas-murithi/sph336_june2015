#include <GenericTypeDefs.h>
#include <plib.h>

// -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-= SYSTEM CONFIGURATION REGISTER SETTINGS -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-= //
#pragma config FPLLMUL = MUL_20, FPLLIDIV = DIV_2, FPLLODIV = DIV_1, FWDTEN = OFF
#pragma config POSCMOD = HS, FNOSC = PRIPLL, FPBDIV = DIV_1

// -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-= SYSTEM DEFINES -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-= //
#define SYS_FREQ					(80000000UL) 
#define GetSystemClock()			(SYS_FREQ) 
#define GetPeripheralClock()		(GetSystemClock() / (1 << OSCCONbits.PBDIV)) 
#define GetInstructionClock()		(GetSystemClock()) 
#define DESIRED_BAUDRATE			(9600)      //The desired BaudRate 115200

// Defined States:
#define STATE_1						(1)
#define STATE_2						(2)
#define STATE_3						(3)

#define U1_BUF_LENGTH				(3)
#define U3_BUF_LENGTH				(256)

// -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-= GLOBAL VARIABLES -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-= //
unsigned int CRC_POLYNOM = 0x8408; // 0xA001; 
unsigned int CRC_PRESET = 0xFFFF; 
int pbClk;

unsigned char data [] = {0x06, 0xFF, 0x66, 0x00, 0xCD, 0xBF}; //sending this data from PIC 
unsigned int cnt1 = sizeof(data); 
unsigned char data1 [] = "TAG DETECTED"; //SEND NUMERIC VALUES????????
unsigned int cnt2 = sizeof(data1); 

// Flags:
BOOL tracker = FALSE;
BOOL reader = FALSE; // Added for RFID tag detection

// UART3 Related Variables:
unsigned int u3BufIndex = 0;
volatile char u3RxByte;
char u3RxBuf[U3_BUF_LENGTH] = {0};

// State Machine & UART1 Related Variables:
unsigned int u1RxState = STATE_1;
volatile char u1RxByte;
char u1RxBuf[U1_BUF_LENGTH] = {0};

// -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-= FUNCTION PROTOTYPES -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-= //
void portInit(void);
void WriteString(const char *string, const int len); 
void UART3_Configure(void);
void UART1_configure(void);
void DelayMs(unsigned int msec);

// -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-= MAIN PROGRAM -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-= //
int main(void) 
{ 
	pbClk = SYSTEMConfig(SYS_FREQ, SYS_CFG_WAIT_STATES | SYS_CFG_PCACHE);
	
// System Initializations:
	portInit();
	UART1_Configure();
	UART3_Configure();

// DO NOT CONFIGURE INTERRUPTS ANY MORE OR ENABLE THEM EITHER.  THIS TAKES CARE OF EVERYTHING FOR YOU.  
	INTEnableSystemMultiVectoredInt();

	while (1)
	{ 
		if(!PORTEbits.RE7)
		{ // IF EXTRA SWITCH IS PRESSED, SEND A STRING THROUGH THE EXPORT 
			WriteString(data, cnt1); 
			mPORTEToggleBits(BIT_8); // TURN ON red LIGHT     
			DelayMs(100); 
			mPORTEToggleBits(BIT_8); // TURN OFF red LIGHT 
		} 
		if(reader)  //RF Tag Detected
		{
// Send string to server here via UART3 somewhere in here because the string received via UART1 is completed
			WriteString(u1RxBuf,3);

			DelayMs(100);
			mPORTEToggleBits(BIT_9); // TURN ON BLUE LIGHT				
			DelayMs(50);
			mPORTEToggleBits(BIT_9); // TURN OFF BLUE LIGHT
			reader = FALSE;
		}
		if(tracker)
		{ // DATA COMING THROUGH THE EXPORT COMPONENT 
			DBPRINTF("IN:%02X\n",u3RxByte);
			switch(u3RxByte)
			{
				case 'A':
					mPORTBClearBits(BIT_8); // BUZZER OFF 
					mPORTBToggleBits(BIT_1); // TURN ON TL LIGHT     
					DelayMs(100); 
					mPORTBToggleBits(BIT_1); // TURN OFF TL LIGHT 
					DelayMs(100); 
					break; 
				case 'B':
					mPORTBToggleBits(BIT_1); // TURN ON TL LIGHT 
					mPORTBToggleBits(BIT_0); //TURN ON WL LIGHT 
					mPORTBToggleBits(BIT_8); // BUZZER ON 

					DelayMs(50); 
					mPORTBToggleBits(BIT_8); // BUZZER OFF 
					mPORTBToggleBits(BIT_1); // TURN OFF TL LIGHT 
					mPORTBToggleBits(BIT_0); //TURN OFF WL LIGHT       

					DelayMs(50);     
					break; 
				case 'C':
					mPORTBClearBits(BIT_8); // BUZZER OFF
					mPORTBSetBits(BIT_1); // TURN ON TL LIGHT
					break; 
				case 'D':
					mPORTBToggleBits(BIT_0); //TURN ON WL LIGHT
					DelayMs(50);
					mPORTBToggleBits(BIT_0); //TURN OFF WL LIGHT
					DelayMs(50);
					mPORTBSetBits(BIT_1); // TURN ON TL LIGHT
					mPORTBSetBits(BIT_8); // BUZZER ON 
					break; 
				case 'E':
					mPORTBClearBits(BIT_8); // BUZZER OFF
					mPORTBToggleBits(BIT_0); //TURN ON WL LIGHT
					mPORTBToggleBits(BIT_1); // TURN ON TL LIGHT
					DelayMs(50);
					mPORTBToggleBits(BIT_0); //TURN OFF WL LIGHT
					//DelayMs(50);
					mPORTBToggleBits(BIT_1); // TURN OFF TL LIGHT
					DelayMs(50);
					break;
				case 'F':
					mPORTBClearBits(BIT_8); // BUZZER OFF
					mPORTBToggleBits(BIT_0); //TURN ON WL LIGHT
					DelayMs(50);
					mPORTBToggleBits(BIT_0); //TURN OFF WL LIGHT
					//DelayMs(50);
					mPORTBSetBits(BIT_1); // TURN ON TL LIGHT
					DelayMs(50);
					break;
				default: 
					mPORTBClearBits(BIT_1); // TURN OFF TL LIGHT 
					mPORTBClearBits(BIT_0); //TURN OFF WL LIGHT       
					mPORTBClearBits(BIT_8); // BUZZER OFF     
					break; 
			}
			u3BufIndex = 0;		// Might want to do this or you will run into problems by the time 'u3RxBuf' has over 256 bytes...
			tracker = FALSE;	// Why was this commented out here?  You need to actually clear this flag in the code?  
		}
	} 
	return 0;
	//tracker = 0;  			// You are never going to reach here.  This is nonessential code.  
}

// ---- I/O PIN CONFIGURATION .. PINS USED: 3(TDM SW.), 4(RFID SW.), 32(BUZZER), 18(RD. LIGHT), 19(BL. LIGHT), 20(GR. LIGHT) ----
void portInit(void)
{
	mPORTBClearBits(BIT_11 | BIT_12 | BIT_13| BIT_14 | BIT_15); // DIP SWITCH BITS
	mPORTBSetPinsDigitalIn(BIT_11 | BIT_12 | BIT_13| BIT_14 | BIT_15);

	mPORTEClearBits(BIT_7); // EXTRA SWITCH
  	mPORTESetPinsDigitalIn(BIT_7);

	mPORTEClearBits(BIT_8 | BIT_9); // RGB R AND B RESP.
	mPORTESetPinsDigitalOut(BIT_8 | BIT_9);

	mPORTBClearBits(BIT_0 | BIT_1); // WL AND TL
	mPORTBSetPinsDigitalOut(BIT_0 | BIT_1);

	mPORTBClearBits(BIT_8); // BUZZER
	mPORTBSetPinsDigitalOut(BIT_8);
}

void UART3_Configure(void)
{
	unsigned int pbclk = 8000000;

// Clear all UART3 control registers
	U3STA = 0;
	U3MODE = 0;

    U3BRG = (pbclk / 16 / DESIRED_BAUDRATE - 1);
	U3STAbits.UTXEN = 1;
	U3STAbits.URXEN = 1;

	IFS1bits.U3RXIF = 0;		// Clear receiver interrupt flag
	IPC7bits.U3IP = 3;			// Set priority
	IPC7bits.U3IS = 3;			// Set sub-priority
	IEC1bits.U3RXIE = 1;		// Enable receiver interrupt

	U3MODEbits.UARTEN = 1;
}

void UART1_Configure(void)
{
// UART1 CONTROL REGISTERS
	U1STA = 0;
	U1MODE = 0;

	U1BRG = (pbclk / 16 / DESIRED_BAUDRATE - 1);
	U1STAbits.UTXEN = 1;
	U1STAbits.URXEN = 1;

// You do not need this.  'INTEnableSystemMultiVectoredInt()' in 'main' takes care of this for you
	//INTConfigureSystem(INT_SYSTEM_CONFIG_MULT_VECTOR);

	IFS0bits.U1RXIF = 0;
	IPC6bits.U1IP = 3;
	IPC6bits.U1IS = 2; // NO NEED FOR NOW!!
	IEC0bits.U1RXIE = 1;

	U3MODEbits.UARTEN = 1;

// EVERYTHING IN HERE IS NON-ESSENTIAL - LEAVE COMMENTED
/*
	U1STAbits.URXISEL = 0; // interrupt with each character received
// enable interrupts
    //INTEnableInterrupts(); // ALSO NEED !
	U1MODEbits.STSEL = 0; //bit0, one stop bit
	U1MODEbits.PDSEL = 0; //bit1, 2 8 bit, no parity 
	U1STAbits.UTXEN = 1; 
	U1STAbits.URXEN = 1; 
	U1MODEbits.UARTEN = 1;
	U1STAbits.OERR = 0;
*/
}

// Sending from PIC to server as below: 
void WriteString(const char *string, const int len)
{
	unsigned int i;

	for(i = 0;i < len;i++)
	{
		while(!U3STAbits.TRMT);
		U3TXREG = string[i];
		// put delay here if you like, but for what reason I wonder?  Following a certain protocol?
	}
}

// THE DELAY FUNCTION:
// This is to be used to ensure a delay of 5ms between the commands sent to the reader 
void DelayMs(unsigned int msec)
{
    unsigned int tWait, tStart;

    tWait = (80000000 / 2000) * msec;
    tStart = ReadCoreTimer();
    while((ReadCoreTimer() - tStart) < tWait);

} //END OF DelayMs(unsigned int msec)

// -=-=-=-=-=-=-=-=-=-=-=-=-=-=-= INTERRUPT HANDLERS -=-=-=-=-=-=-=-=-=-=-=-=-=-=-= //

// See below...
/*
// UART 1 INTERRUPT HANDLER
void __ISR(_UART_1_VECTOR, ipl3) IntUart1Handler(void) { 

	if(IFS0bits.U1RXIF)
	{
		unsigned int cnt4 = 0;
		int i;

		for(i = 1;i < 4;i++)
		{
			str = U1RXREG;											// ESSENTIAL OPERATION! 
			//	buf[bufptr++] = str; // buf COMES AS buf[]="..."		// What do you do if bufptr > sizeof(buf)???  This a potential source of failure
																		// here I want to store the workers Tag number but 'str' is not of sufficient length and buf[] = "..."??????? Any other better way to store the long received string?
			//	reader = 1; 

			// read received byte here... 
			//int state = 1;

			switch(i) 
			{ 
				case 1: 
					buf[bufptr++] = str; 
					//state = 2;
					cnt4 = sizeof(str);
					WriteString(str, cnt4);  //I tried this thing that what ever it receives immidiatly sends to server through UART3 But it does not work??
					break; 
				case 2: 
					buf[bufptr++] = str; // here contents of buf[] = "..." remains unchanged
				   // state = 3; 
					break; 
			
				case 3: 
					buf[bufptr++] = str; 
					//state = 1;        // reset state machine 
					bufptr = 0;            // maybe reset buffer index here, or not and use as a received byte counter in main-line processing??? 
					reader = 1;            // set flag to signal main loop to process data in 'buf' 
					break; 
				default: 
					//state = 1; 
					break; 
			} 
		//IFS0bits.U1RXIF = 0;
		}//END OF FOR LOOP	
	} // END OF IF ...		
	IFS0bits.U1RXIF = 0;	

} //END OF INTERRUPT HANDLER 
*/

/*
The first byte that comes into U1RX FIFO will hit STATE_1, then the next byte (interrupt) will hit STATE_2, and so on.  Do not wait for 
	the string to be constructed inside here.  Let this ISR signal to the mainline that a string is ready to be processed.  
BTW, do not check if RXIF is set.  The reason that you enter this function is because U1RXIF is set.  
*/
// UART 1 INTERRUPT HANDLER
void __ISR(_UART_1_VECTOR, ipl3) IntUart1Handler(void)
{
	u1RxByte = U1RXREG;
    switch(u1RxState)
    {
        case STATE_1:
			u1RxBuf[0] = u1RxByte;
			u1RxState = STATE_2;
            break;
        case STATE_2:
            u1RxBuf[1] = u1RxByte;
			u1RxState = STATE_3;
            break;
        case STATE_3:
			u1RxBuf[2] = u1RxByte;
			u1RxState = STATE_1;
			reader = TRUE;					// String is ready to be processed in 'main'
			break; 
        default:
            state = STATE_1;
            break;
    }
	IFS0bits.U1RXIF = 0;
} //END OF INTERRUPT HANDLER

// UART3 ISR HANDLER:
/*
There is no reason to have a buffer really in this interrupt because in the main function you are switching only a single byte
 and based on that byte ('A', 'B', 'C', etc.) you do something.  You are not looking at a buffer ever in this project.  I will
 leave it in here, but it is not really needed the way this source code is structured.  
*/
void __ISR(_UART_3_VECTOR, ipl3) IntUart3Handler(void)
{
// Essential operation
	u3RxByte = U3RXREG;
	
	//buf[bufptr++] = str; // buf COMES AS buf[]="..."		// What do you do if bufptr > sizeof(buf)???  This a potential source of failure 
															// here I checked the length of bufptr < sizof(buf) even though buf[] = "..."  ??????? problem !
// This is what I was saying about making sure that you check this or you will eventually access memory that is unallocated.  Where is 'bufptr' reset???
	if(u3RxBuf < U3_BUF_LENGTH)
	{
		u3RxBuf[u3BufIndex++] = u3Rxbyte;
		tracker = TRUE;
	}
	IFS1bits.U3RXIF = 0; 			// ALWAYS DO THIS AT THE END OF THIS ISR NO MATTER WHAT!
} //END OF INTERRUPT HANDLER 
