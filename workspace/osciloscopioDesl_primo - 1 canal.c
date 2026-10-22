/*
 *  osciloscopioDesl_primo.c
 *  Created on: 9 de julho de 2021
 *  Author: Everton Reis
 *  Permite otimizar a memória para o armazenamento dos pontos
 */
#include "DSP2833x_Device.h" //para aceitar os tipos de dados redefinidos Uint16 e int16, int32 etc;
#include <stdlib.h>
    Uint16 trigger = 0, //indica que não houve captura e está a procura do evento. Se 1, encontrou o sinal de interesse
                ix = 4; //Nível horizontal do trigger
    Uint16 nivel_trigger = 100;//Nível vertical repare q no teste contempla a igualdade, mas pode ser borda de subida, descida, etc.
    Uint16 fila[2], i_wrap;
    Uint16 num_canais=1;//DEON
    int16 num_pontos=503;//DEON
    extern Uint16 rampa[];//DEON
    //parametros globais no arquivo e que devem estar disponiveis no arquivo principal com "extern"
    enum Etapas {armazenar, formatar, stop, set} etapa = set;
    Uint16 automatico = 0, source_trigger=0, borda_descida = 0; Uint16 inicializar = 1, falha_aloc = 0;
    Uint16 **matriz = NULL; Uint16 **canal=NULL; //

void criar_matriz();// só pra tirar o warnning "funtion "criar_matriz" declared"//aloca espaço para todos os pontos de todos os canais

void plot() {
	static int16 j = 0,i = 0, cap = 0, inc, al; //variavel static dentro da função, CAPture mais sinais até completar a metade do vetor
	Uint16 *ptr_fila;
	Uint16 seleciona_canal;//variavel auxiliar para selecionar canais para preenchimento dos pontos
	if (i >= num_pontos) //corrige possíveis acessos fora dos limites do vetor
        i = i - num_pontos;
    else if (i < 0)
        i = i + num_pontos;
    switch(etapa) {
    case armazenar:
    	for (seleciona_canal=0; seleciona_canal < num_canais; seleciona_canal++){
    		matriz[seleciona_canal][i] = rampa[i];//DEON *canal[seleciona_canal]; armazena o sinal de interesse na linha da matriz
    	}
    	if (trigger == 0) {
            if ((matriz[source_trigger][i] >= nivel_trigger)^borda_descida) {// criterio por edge(borda). Se borda_descida = 1; vai pegar a borda de descida
                al = (int16)(i - ix);
                if (al>=0) {
                	trigger = 1;
                	cap = ix;
                }
            }//fim teste nivelvdo trigger
        } else { //se triggou, conte até armazenar o final do vetor
            cap++; //contando
        }
        i++; // indexador para armazenamento
        if (cap >= num_pontos) {
            	etapa = formatar;
                j = 0;
                inc = -1;//
        	    ptr_fila = fila; // ponteiro recebe o endereço do vetor fila ou &fila[0]
        	    *ptr_fila = matriz[0/*seleciona_canal*/][al];// fila[0] = primeiro parametro
        	    ptr_fila++;
        	    i_wrap = al;
        }
        break;
    case formatar:
        if (j++ < num_pontos) {//percorra todos os pontos do vetor
			i_wrap += num_pontos - al;
			if (i_wrap >= num_pontos) i_wrap -= num_pontos;//se passar do limite do vetor, ache o indice adequado
			*ptr_fila = matriz[0/*seleciona_canal*/][i_wrap];
			ptr_fila += inc;
			matriz[0/*seleciona_canal*/][i_wrap] = *ptr_fila;
			inc = -inc;
        } else etapa = stop;
        break;
    case stop: //etapas visiveis ao usuário. Quando stop=automatico = 0;
        if (trigger == 0) {
            etapa = armazenar;
            i = 0; cap=0;
        }
        else if (automatico) {
			static int32 divfreq_sw = 0;
			if (divfreq_sw++ > (int32)((Uint32)150 * (Uint16)num_pontos)) {
				trigger = 0; divfreq_sw = 0;
				nivel_trigger +=10;//DEON
			} //divisor_freq_sw
		} //else_if
        break;
    case set:
    	if (inicializar) {
    			inicializar = 0;
    			if (matriz != NULL) {// se a matriz foi criada antes, elimine-a
    				Uint16 aux1;
    				for (aux1 = 0; aux1 < num_canais; aux1++) {
						free(matriz[aux1]); matriz[aux1] = NULL;
					}
    				free(canal); free(matriz);
    				matriz = NULL; canal = NULL;
    			}// fim se a matriz foi criada
    			criar_matriz();
    			if (falha_aloc)
    				break;
    			else //não houve falha na alocação
    			etapa = armazenar;
    			canal[0]= rampa; //&AdcMirror.ADCRESULT0;//EPwm1Regs.CMPA.half.CMPA;//// na prática essa atribuição vai ser na janela EXPRESSION
    			//canal[1]= &AdcMirror.ADCRESULT1;
    		}//if_inicializar
    } // fim do switch
}//retorne

void criar_matriz(){
	//Uint16 **vet_ponteiros_duplo_aux[]={NULL, NULL};//
	Uint16 i;
	matriz = ((Uint16 **) malloc(num_canais * sizeof(Uint16*)));
	canal = ((Uint16 **) malloc(num_canais * sizeof(Uint16*)));

	if ( (matriz == NULL) || (canal == NULL) ) {
		falha_aloc = 1; return;
	}
	//canal[i] acessivel no "expression view"
	for (i = 0; i < num_canais; i++) { //criação da matriz toda de fato
		matriz[i] = (Uint16 *) malloc(num_pontos * sizeof(Uint16));
		if (matriz[i] == NULL){
			falha_aloc = 1;
			return;
		}
	}
}
