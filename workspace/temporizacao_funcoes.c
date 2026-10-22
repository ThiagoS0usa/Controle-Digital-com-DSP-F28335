#include "DSP28x_Project.h"
#include "temporizacao_funcoes.h"
//#define valor_atual_timer CpuTimer0Regs.TIM.all
#define valor_inicial_timer CpuTimer0Regs.TIM.all
#define AC //com auto reset

bool TO(DT * dt) { //função que retorna se estourou o tempo, TIME-OUT
    if (cronometro(dt, true) > (dt->tempo)) {// passou do tempo?
        dt->captimer = CpuTimer0Regs.TIM.all;//valor atual do timer
        #ifdef AC //autoclear
        return true; //com dt->to =false ainda
        #elif 
        dt->to = true;// só aqui que coloco como true
        #endif
    }
    return dt->to;
}
/*
 * Uint32 valor_atual_timer = CpuTimer0Regs.TIM.all;
    Uint32 delta_t = (Uint32)(dt->captimer - valor_atual_timer);//para timer decrescente
    if ((dt->captimer) <= valor_atual_timer) delta_t += CpuTimer0Regs.PRD.all;
 *
 * */

void RT(DT * dt) { //função que reseta a temporização
    dt->to = false;
    dt->captimer = valor_inicial_timer;
}

Uint32 cronometro(DT* dt, bool tic){//retornar o tempo de execução
	if (tic) {
		Uint32 valor_atual_timer = CpuTimer0Regs.TIM.all;
		Uint32 delta_t = (Uint32)(dt->captimer - valor_atual_timer); //para timer decrescente
		if ((dt->captimer) <= valor_atual_timer)
			delta_t += CpuTimer0Regs.PRD.all;
		return delta_t;
	} else {
		dt->captimer = valor_inicial_timer;
		return 0;
	}
}
