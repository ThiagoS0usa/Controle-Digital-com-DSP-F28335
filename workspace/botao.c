/*
 * botao.c
 *
 *  Created on: 14 de abr de 2021
 *      Author: evert
 */
#include "botao.h"
#include "DSP28x_Project.h"
//#include "DSP2833x_Gpio.h"

bool BP(DB* db){ bool pressionado = false;
	if (db->osc) {
		if (TO(&(db->dt)))
			db->osc = false;
	} else { //sem oscilação, ok
		Uint16 pino_lido = le_pino(db->pino);
		if (pino_lido != db->est) { //houve borda ?
			db->osc = 1;
			RT(&(db->dt));
			if (pino_lido == 0)
			pressionado = true;
		}
		db->est = pino_lido; //inicia_re(&reles[0]);
	}
	return pressionado;
}//função BP

void Config_botao(int16 pino, DB * db, Uint16 tempo_ms){ //anti_bouncing em ms
	db->pino = pino;
	db->dt.tempo = (Uint32)(tempo_ms * 150150U);
	db->dt.to = false;
	db->est = 1; db->osc = false;
	prepara_pino(0,pino);//prepara_pino como entrada
}

void prepara_pino(int16 pino, Uint16 modo){//0 entrada, modo 1 para saída,
	Uint16 mod = pino/32; Uint16 resto = pino%32; Uint32 valor = ((Uint32)1<<resto);
	EALLOW;
	switch (mod) {
	case 0:
		if (modo){
			GpioCtrlRegs.GPADIR.all |= valor; //configurar saída para LED
			GpioCtrlRegs.GPAPUD.all |= valor; //desabilitanto pull-up
		}
		else {
			GpioCtrlRegs.GPADIR.all &= ~valor; //configurar para entrada de botao
			GpioCtrlRegs.GPAPUD.all &= ~valor; //com pull-up habilitado
		}
		break;
	case 1:
		if (modo) {
			GpioCtrlRegs.GPBDIR.all |= valor; //configurar saída para LED
			GpioCtrlRegs.GPBPUD.all |= valor; //desabilitanto pull-up
		} else {
			GpioCtrlRegs.GPBDIR.all &= ~valor; //configurar para entrada de botao
			GpioCtrlRegs.GPBPUD.all &= ~valor; //com pull-up habilitado
		}
		break;
	case 2:
		if (modo) {
			GpioCtrlRegs.GPCDIR.all |= valor; //configurar saída para LED
			GpioCtrlRegs.GPCPUD.all |= valor; //desabilitanto pull-up
		} else {
			GpioCtrlRegs.GPCDIR.all &= ~valor; //configurar para entrada de botao
			GpioCtrlRegs.GPCPUD.all &= ~valor; //com pull-up habilitado
		}
		break;
	}
	EDIS;
}

Uint16 le_pino(Uint16 pino){
	Uint16 mod = pino/32; Uint16 resto = pino%32; Uint32 valor = ((Uint32)1<<resto);
	Uint32 est;
	switch (mod) {
	case 0:
		est = GpioDataRegs.GPADAT.all & valor;
		break;
	case 1:
		est = GpioDataRegs.GPBDAT.all & valor;
		break;
	case 2:
		est = GpioDataRegs.GPCDAT.all & valor;
		break;
	}

	if (est)
		return 1;
	else
		return 0; //retorna o estado do pino, se entrada ou saída
}
