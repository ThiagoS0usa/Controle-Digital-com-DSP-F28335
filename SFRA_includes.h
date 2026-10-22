/*
 * SFRA_includes.h
 *
 *  Created on: 26 de mai de 2021
 *      Author: evert
 */
#ifndef SFRA_INCLUDES_H_
#define SFRA_INCLUDES_H_

#define MATH_TYPE 1 //1 para FLOAT_MATH
#include "SFRA_F_Include.h"

#define BOOST_PWM_PERIOD 3408  //estou definindo aqui o valor que gera a freq_chavea de 22k
#define SFRA_ISR_FREQ 22000
#define SFRA_FREQ_START 100
#define SFRA_FREQ_LENGTH 100

//MUDEI AQUI PARA UM TESTE
//SFRA step Multiply = 10^(1/No of steps per decade(40))
#define FREQ_STEP_MULTIPLY (float)1.059253

SFRA_F SFRA1;
//extern to access tables in ROM
extern long FPUsinTable[];
float32 Plant_MagVect[SFRA_FREQ_LENGTH];
float32 Plant_PhaseVect[SFRA_FREQ_LENGTH];
float32 OL_MagVect[SFRA_FREQ_LENGTH];
float32 OL_PhaseVect[SFRA_FREQ_LENGTH];
float32 FreqVect[SFRA_FREQ_LENGTH];
//function prototype list
void SCIA_Init(long SCI_VBUS_CLOCKRATE, long SCI_BAUDRATE);//SFRA protótipo
void SerialHostComms();//SFRA protótipo

//criadas por mim para facilitar
void iniciar_SFRAeGUI(void);//iniciar SFRA e dar suporte a janela SFRA GUI
// Global variables
int16 SerialCommsTimer; //SFRA
int16 CommsOKflg; //SFRA
//Flag for reinitializing FRA variables
int16 initializationFlag; //SFRA
int16 *varSetTxtList[16]; //16 textbox controlled variables
int16 *varSetBtnList[16]; //16 button controlled variables
int16 *varSetSldrList[16]; //16 slider controlled variables
int16 *varGetList[16]; //16 variables sendable to GUI
int32 *arrayGetList[16]; //16 arrays sendable to GUI
Uint32 *dataSetList[16]; //16 32-bit textbox or label controlled variables

#endif /* SFRA_INCLUDES_H_ */
