//###########################################################################
//
// FILE:	DSP2833x_Adc.c
//
// TITLE:	DSP2833x ADC Initialization & Support Functions.
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

//
// Defines
//
#define ADC_usDELAY  5000L

//
// InitAdc - This function initializes ADC to a known state.
//
void InitAdc(void) {
	extern void DSP28x_usDelay(Uint32 Count);
	//
	//                          *IMPORTANT*
	// The ADC_cal function, which  copies the ADC calibration values from
	// TI reserved OTP into the ADCREFSEL and ADCOFFTRIM registers, occurs
	// automatically in the Boot ROM. If the boot ROM code is bypassed during
	// the debug process, the following function MUST be called for the ADC to
	// function according to specification. The clocks to the ADC MUST be
	// enabled before calling this function. See the device data manual and/or
	// the ADC Reference Manual for more information.
	//
	EALLOW;
	SysCtrlRegs.PCLKCR0.bit.ADCENCLK = 1;
	ADC_cal();
	EDIS;
	//
	// To powerup the ADC the ADCENCLK bit should be set first to enable
	// clocks, followed by powering up the bandgap, reference circuitry, and
	// ADC core. Before the first conversion is performed a 5ms delay must be
	// observed after power up to give all analog circuits time to power up
	// and settle
	//

	//
	// Please note that for the delay function below to operate correctly the
	// CPU_RATE define statement in the DSP2833x_Examples.h file must
	// contain the correct CPU clock period in nanoseconds.
	//
	AdcRegs.ADCTRL1.bit.ACQ_PS = 0xf;
	AdcRegs.ADCTRL1.bit.SEQ_CASC = 0;        // 0 Non-Cascaded Mode
	AdcRegs.ADCTRL3.bit.ADCCLKPS = 3; //para dividir por 2*3 o clock HSPCLK          0x00E0 Power up bandgap/reference/ADC circuits
	AdcRegs.ADCTRL3.bit.ADCBGRFDN = 3; //Liga o bandgap e referência
	AdcRegs.ADCTRL3.bit.SMODE_SEL = 0; //modo sequencial
	AdcRegs.ADCTRL3.bit.ADCPWDN = 1; //Liga o AD core
	//DELAY_US(ADC_usDELAY);         // Delay before converting ADC channels
	Uint16 temp = 7500;
	while(temp--);
	// Configure ADC
	AdcRegs.ADCMAXCONV.all = 0x0001;       	// Setup 2 conv's on SEQ1
	AdcRegs.ADCCHSELSEQ1.bit.CONV00 = 0x1; 	// Setup ADCINA3 as 1st SEQ1 conv.
	AdcRegs.ADCCHSELSEQ1.bit.CONV01 = 0x2; 	// Setup ADCINA2 as 2nd SEQ1 conv.
	//AdcRegs.ADCCHSELSEQ1.bit.CONV02 = 0x5;	//terceira sequencia - A5
	//AdcRegs.ADCCHSELSEQ1.bit.CONV03 = 0x1; // quarta " , A1
//	AdcRegs.ADCCHSELSEQ2.bit.CONV04 = 0x7;		//quinta " A7
//	AdcRegs.ADCCHSELSEQ2.bit.CONV05 = 0x4;		//sexta " A4
//	AdcRegs.ADCCHSELSEQ2.bit.CONV06 = 0x6;		//setima " A6
//	AdcRegs.ADCCHSELSEQ2.bit.CONV07 = 0x0;		//oitava " A0

	AdcRegs.ADCTRL2.bit.EPWM_SOCA_SEQ1 = 1;	// Enable SOCA from ePWM to start SEQ1
	AdcRegs.ADCTRL2.bit.INT_ENA_SEQ1 = 1;	// Enable SEQ1 interrupt (every EOS)
}

//
// End of file
//

