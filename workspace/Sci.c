//###########################################################################
//
// FILE:	DSP2833x_Sci.c
//
// TITLE:	DSP2833x SCI Initialization & Support Functions.
//
//###########################################################################
// $TI Release: F2833x/F2823x Header Files and Peripheral Examples V142 $
// $Release Date: November  1, 2016 $
// $Copyright: Copyright (C) 2007-2016 Texas Instruments Incorporated -
//             http://www.ti.com/ ALL RIGHTS RESERVED $
//###########################################################################

//
// Included Files
//
#include "DSP2833x_Device.h"     // DSP2833x Headerfile Include File
#include "DSP2833x_Examples.h"   // DSP2833x Examples Include File

void scia_loopback_init(void);
void scia_fifo_init(void);
void scia_xmit(int a);

//
// InitSci - This function initializes the SCI(s) to a known state.
//
void InitSci(void) {
	InitSciaGpio();
	// Initialize SCI-A
	scia_fifo_init();	   // Initialize the SCI FIFO
	scia_loopback_init();  // Initialize SCI for digital loop back

	//

	//
	// Initialize SCI-B
	//

	//
	// Initialize SCI-C
	//
}

//
// InitSciGpio - This function initializes GPIO to function as SCI-A, SCI-B, or
// SCI-C
//
// Each GPIO pin can be configured as a GPIO pin or up to 3 different
// peripheral functional pins. By default all pins come up as GPIO
// inputs after reset.  
// 
// Caution: 
// Only one GPIO pin should be enabled for SCITXDA/B operation.
// Only one GPIO pin shoudl be enabled for SCIRXDA/B operation. 
// Comment out other unwanted lines.
//
void InitSciGpio() {
	InitSciaGpio();
//#if DSP28_SCIB
//    InitScibGpio();
//#endif // if DSP28_SCIB
//
//#if DSP28_SCIC
//    InitScicGpio();
//#endif // if DSP28_SCIC
}

//
// InitSciaGpio - This function initializes GPIO pins to function as SCI-A pins
//
void InitSciaGpio() {
	EALLOW;

	//
	// Enable internal pull-up for the selected pins
	// Pull-ups can be enabled or disabled disabled by the user.
	// This will enable the pullups for the specified pins.
	//
	GpioCtrlRegs.GPAPUD.bit.GPIO28 = 0;  // Enable pull-up for GPIO28 (SCIRXDA)
	GpioCtrlRegs.GPAPUD.bit.GPIO29 = 0;	 // Enable pull-up for GPIO29 (SCITXDA)

	//
	// Set qualification for selected pins to asynch only
	// Inputs are synchronized to SYSCLKOUT by default.
	// This will select asynch (no qualification) for the selected pins.
	//
	GpioCtrlRegs.GPAQSEL2.bit.GPIO28 = 3;  // Asynch input GPIO28 (SCIRXDA)

	//
	// Configure SCI-A pins using GPIO regs
	// This specifies which of the possible GPIO pins will be SCI functional
	// pins.
	//
	GpioCtrlRegs.GPAMUX2.bit.GPIO28 = 1;   // Configure GPIO28 to SCIRXDA
	GpioCtrlRegs.GPAMUX2.bit.GPIO29 = 1;   // Configure GPIO29 to SCITXDA

	EDIS;
}

#if DSP28_SCIB
//
// InitScibGpio - This function initializes GPIO pins to function as SCI-B pins
//
void InitScibGpio() {
	EALLOW;

	//
	// Enable internal pull-up for the selected pins
	// Pull-ups can be enabled or disabled disabled by the user.
	// This will enable the pullups for the specified pins.
	// Comment out other unwanted lines.
	//
	//GpioCtrlRegs.GPAPUD.bit.GPIO9 = 0;  //Enable pull-up for GPIO9  (SCITXDB)
	//GpioCtrlRegs.GPAPUD.bit.GPIO14 = 0; //Enable pull-up for GPIO14 (SCITXDB)
	GpioCtrlRegs.GPAPUD.bit.GPIO18 = 0;	 //Enable pull-up for GPIO18 (SCITXDB)
	//GpioCtrlRegs.GPAPUD.bit.GPIO22 = 0; //Enable pull-up for GPIO22 (SCITXDB)

	//GpioCtrlRegs.GPAPUD.bit.GPIO11 = 0; //Enable pull-up for GPIO11 (SCIRXDB)
	//GpioCtrlRegs.GPAPUD.bit.GPIO15 = 0; //Enable pull-up for GPIO15 (SCIRXDB)
	GpioCtrlRegs.GPAPUD.bit.GPIO19 = 0;	 //Enable pull-up for GPIO19 (SCIRXDB)
	//GpioCtrlRegs.GPAPUD.bit.GPIO23 = 0; //Enable pull-up for GPIO23 (SCIRXDB)

	//
	// Set qualification for selected pins to asynch only
	// This will select asynch (no qualification) for the selected pins.
	// Comment out other unwanted lines.
	//
	//GpioCtrlRegs.GPAQSEL1.bit.GPIO11 = 3;  // Asynch input GPIO11 (SCIRXDB)
	//GpioCtrlRegs.GPAQSEL1.bit.GPIO15 = 3;  // Asynch input GPIO15 (SCIRXDB)
	GpioCtrlRegs.GPAQSEL2.bit.GPIO19 = 3;  // Asynch input GPIO19 (SCIRXDB)
	//GpioCtrlRegs.GPAQSEL2.bit.GPIO23 = 3;  // Asynch input GPIO23 (SCIRXDB)

	//
	// Configure SCI-B pins using GPIO regs
	// This specifies which of the possible GPIO pins will be SCI functional
	// pins.
	// Comment out other unwanted lines.
	//
	//GpioCtrlRegs.GPAMUX1.bit.GPIO9 = 2;  //Configure GPIO9 to SCITXDB
	//GpioCtrlRegs.GPAMUX1.bit.GPIO14 = 2; //Configure GPIO14 to SCITXDB
	GpioCtrlRegs.GPAMUX2.bit.GPIO18 = 2;  //Configure GPIO18 to SCITXDB
	//GpioCtrlRegs.GPAMUX2.bit.GPIO22 = 3; //Configure GPIO22 to SCITXDB

	//GpioCtrlRegs.GPAMUX1.bit.GPIO11 = 2;  //Configure GPIO11 for SCIRXDB
	//GpioCtrlRegs.GPAMUX1.bit.GPIO15 = 2;  //Configure GPIO15 for SCIRXDB
	GpioCtrlRegs.GPAMUX2.bit.GPIO19 = 2;   //Configure GPIO19 for SCIRXDB
	//GpioCtrlRegs.GPAMUX2.bit.GPIO23 = 3;  //Configure GPIO23 for SCIRXDB

	EDIS;
}
#endif // if DSP28_SCIB 

#if DSP28_SCIC
//
// InitScicGpio - This function initializes GPIO pins to function as SCI-C pins
//
void InitScicGpio() {
	EALLOW;

	//
	// Enable internal pull-up for the selected pins
	// Pull-ups can be enabled or disabled disabled by the user.
	// This will enable the pullups for the specified pins.
	//
	GpioCtrlRegs.GPBPUD.bit.GPIO62 = 0;  // Enable pull-up for GPIO62 (SCIRXDC)
	GpioCtrlRegs.GPBPUD.bit.GPIO63 = 0;	 // Enable pull-up for GPIO63 (SCITXDC)

	//
	// Set qualification for selected pins to asynch only
	// Inputs are synchronized to SYSCLKOUT by default.
	// This will select asynch (no qualification) for the selected pins.
	//
	GpioCtrlRegs.GPBQSEL2.bit.GPIO62 = 3;  // Asynch input GPIO62 (SCIRXDC)

	//
	// Configure SCI-C pins using GPIO regs
	// This specifies which of the possible GPIO pins will be SCI functional
	// pins.
	//
	GpioCtrlRegs.GPBMUX2.bit.GPIO62 = 1;   // Configure GPIO62 to SCIRXDC
	GpioCtrlRegs.GPBMUX2.bit.GPIO63 = 1;   // Configure GPIO63 to SCITXDC

	EDIS;
}
#endif // if DSP28_SCIC 

// Test 1,SCIA  DLB, 8-bit word, baud rate 0x000F, default, 1 STOP bit, no parity
void scia_loopback_init() {
	// Note: Clocks were turned on to the SCIA peripheral
	// in the InitSysCtrl() function

	SciaRegs.SCICCR.all = 0x0007;   // 1 stop bit,  No loopback
									// No parity,8 char bits,
									// async mode, idle-line protocol
	SciaRegs.SCICTL1.all = 0x0003;  // enable TX, RX, internal SCICLK,
									// Disable RX ERR, SLEEP, TXWAKE
	SciaRegs.SCICTL2.all = 0x0003;
	SciaRegs.SCICTL2.bit.TXINTENA = 1;
	SciaRegs.SCICTL2.bit.RXBKINTENA = 1;
	SciaRegs.SCIHBAUD = 0x0000;
	SciaRegs.SCILBAUD = 0x000F;
	SciaRegs.SCICCR.bit.LOOPBKENA = 1; // Enable loop back
	SciaRegs.SCICTL1.all = 0x0023;     // Relinquish SCI from Reset
}

// Transmit a character from the SCI'
void scia_xmit(int a) {
	SciaRegs.SCITXBUF = a;
}

// Initialize the SCI FIFO
void scia_fifo_init() {
	SciaRegs.SCIFFTX.all = 0xE040;
	SciaRegs.SCIFFRX.all = 0x204f;
	SciaRegs.SCIFFCT.all = 0x0;
}

//
// End of file
//

