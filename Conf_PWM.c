/*
 * Conf_PWM.c
 *
 *  Created on: 4 de abr de 2020
 *      Author: evert
 */
#include "DSP2833x_Device.h"
#include "DSP2833x_Examples.h"

void InitPWM() {
// Assumes ePWM1 clock is already enabled in InitSysCtrl();
	EALLOW;
	EPwm1Regs.ETSEL.bit.SOCAEN = 1;//<<<<modified Enable SOC on A group
	EPwm1Regs.ETSEL.bit.SOCASEL = ET_CTR_PRD; // Select SOC from from CPMA on upcount
	EPwm1Regs.ETPS.bit.SOCAPRD = 1;//<<<<modified Generate pulse on 1st event
	EPwm1Regs.CMPA.half.CMPA = 1368;//Set compare A value, para um duty de 40 %, não definido ainda o formato do PWM, ctrlQ, E, H
	EPwm1Regs.TBPRD = 3408;//3421;           // Set period for ePWM1 para 22kHz
	EPwm1Regs.TBCTL.bit.CTRMODE = TB_COUNT_UPDOWN; //forma de onda triangular
	EPwm1Regs.TBCTL.bit.CLKDIV = 0;
	EPwm1Regs.TBCTL.bit.HSPCLKDIV = 0; //rodar na velocidade máxima para obter maior precisão

	EPwm1Regs.CMPCTL.bit.SHDWAMODE = CC_SHADOW;
	EPwm1Regs.CMPCTL.bit.SHDWBMODE = CC_SHADOW;
	EPwm1Regs.CMPCTL.bit.LOADAMODE = CC_CTR_ZERO; // load on CTR = Zero
	EPwm1Regs.CMPCTL.bit.LOADBMODE = CC_CTR_ZERO; // load on CTR = Zero
	//EPwm1Regs.AQCTLA.bit.ZRO = AQ_SET;
	EPwm1Regs.AQCTLA.bit.CAU = AQ_CLEAR;
	EPwm1Regs.AQCTLA.bit.CAD = AQ_SET;

	EPwm1Regs.AQCTLB.bit.ZRO = AQ_SET;
	EPwm1Regs.AQCTLB.bit.CBU = AQ_CLEAR;
EDIS;
/*Dia 07/05 adicionei a configuração do PWM3 pra trigar o conversor A/D a 200 kHz
	EPwm3Regs.ETSEL.bit.SOCAEN = 1;     // Enable SOC on A group
	EPwm3Regs.ETSEL.bit.SOCASEL = ET_CTR_PRD; // Select SOC from from CPMA on upcount
	EPwm3Regs.ETPS.bit.SOCAPRD = 1;     // Generate pulse on 1st event
	EPwm3Regs.CMPA.half.CMPA = 188; //Set compare A value, para um duty de 50%, não definido ainda o formato do PWM, ctrlQ, E, H
	EPwm3Regs.TBPRD = 376;           // Set period for ePWM3 para 200kHz
	EPwm3Regs.TBCTL.bit.CTRMODE = TB_COUNT_UPDOWN; //forma de onda triangular
	EPwm3Regs.TBCTL.bit.CLKDIV = 0;
	EPwm3Regs.TBCTL.bit.HSPCLKDIV = 0; //rodar na velocidade máxima para obter maior precisão

	EPwm3Regs.CMPCTL.bit.SHDWAMODE = CC_SHADOW;
	//EPwm3Regs.CMPCTL.bit.SHDWBMODE = CC_SHADOW;
	EPwm3Regs.CMPCTL.bit.LOADAMODE = CC_CTR_ZERO; // load on CTR = Zero
	//EPwm3Regs.CMPCTL.bit.LOADBMODE = CC_CTR_ZERO; // load on CTR = Zero
	//EP3m1Regs.AQCTLA.bit.ZRO = AQ_SET;
	EPwm3Regs.AQCTLA.bit.CAU = AQ_CLEAR;
	EPwm3Regs.AQCTLA.bit.CAD = AQ_SET;

	//EPwm3Regs.AQCTLB.bit.ZRO = AQ_SET;
	//EPwm3Regs.AQCTLB.bit.CBU = AQ_CLEAR;
*/
}

