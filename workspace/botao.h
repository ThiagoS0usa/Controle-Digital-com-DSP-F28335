/*
 * botao.h
 *
 *  Created on: 14 de abr de 2021
 *      Author: evert
 */
#ifndef BOTAO_H_
#define BOTAO_H_
#include "temporizacao_funcoes.h"//pra incluir tão somente a definição de DT

typedef struct DadosBotao {
	DT dt;//temporizador para tempo de bouncing
	bool osc;//true se houver oscilacao
	Uint16 est;//estado anterior do pino
	Uint16 pino;
} DB;
//HELP
//o parâmetro ....
bool BP(DB* db);//indica Botão_pressionado
void Config_botao(int16 pino, DB * db, Uint16 tempo_ms);
void prepara_pino(int16 pino, Uint16 modo);//modo I/O, pino
Uint16 le_pino(Uint16 pino);//e estado do pino
//void BC(DT* dt);//Reseta a temporização

#endif /* BOTAO_H_ */
