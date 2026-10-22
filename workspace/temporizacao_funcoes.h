#ifndef _TEMPORIZACAO_FUNCOES_H_
#define _TEMPORIZACAO_FUNCOES_H_
#include <stdbool.h>
#include "DSP28x_Project.h"
typedef struct DadosTemporizador {
	Uint32  tempo,captimer;//tempo = qtd de clks para o timer0
	bool to;//true se houver estouro do timer
} DT;
//HELP
//o parâmetro tempo de DT tem que ser menor do que CpuTimer0Regs.PRD.all
bool TO(DT* dt);//indica se houve ou não estouro do tempo
void RT(DT* dt);//Reseta a temporização
Uint32 cronometro(DT* dt, bool tic);
#endif
