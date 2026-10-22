/*
 *  osciloscopioDesl_primo.c
 *  Created on: 9 de julho de 2021
 *  Author: Everton Reis
 *  Permite otimizar a memória para o armazenamento dos pontos
 */
#include "DSP2833x_Device.h" //para aceitar os tipos de dados redefinidos Uint16 e int16, int32 etc;
#include <stdlib.h>
    Uint16 trigger = 0, //indica que não houve captura e está a procura do evento. Se 1, encontrou o sinal de interesse
                ix = 4, //Nível horizontal do trigger
				nivel_trigger = 4000;//Nível vertical do trigger repare q no teste contempla a igualdade, mas pode ser borda de subida, descida, etc.
	Uint16 i_wrap;
	Uint16 num_pontos=1009, num_canais=2, automatico = 0, source_trigger=0, borda_descida = 0,
		   inicializar = 1, falha_aloc = 0;//parametros globais no arquivo e que devem estar disponiveis no arquivo principal com "extern"
    enum Etapas {armazenar, formatar, stop, set} etapa = set;
    Uint16 **matriz = NULL, **canal=NULL, **mptr_fila = NULL, **mfila = NULL;//quero ter vetor de ponteiros tb
//protótipos das funções neste arquivo
void criar_matriz();// só pra tirar o warnning "funtion "criar_matriz" declared"//aloca espaço para todos os pontos de todos os canais
void gereNum_primo();
void plot();

void plot() {
	static int16 inc, al; //variavel static dentro da função, CAPture mais sinais até completar a metade do vetor
	static Uint16 i = 0, j=0, cap = 0;
	Uint16 seleciona_canal;//variavel auxiliar para selecionar canais para preenchimento dos pontos
    switch(etapa) {
    case armazenar:
    	if (i >= num_pontos) //corrige possíveis acessos fora dos limites do vetor
    	        i = i - num_pontos;
    	for (seleciona_canal=0; seleciona_canal < num_canais; seleciona_canal++){
    		matriz[seleciona_canal][i] = *canal[seleciona_canal];//armazena o sinal de interesse na linha da matriz
    	}
    	if (trigger == 0) {//se é para triggar algum evento
            if ((matriz[source_trigger][i] >= nivel_trigger)^borda_descida) {// criterio por edge(borda). Se borda_descida = 1; vai pegar a borda de descida
                al = (int16)(i - ix);
                if (al>=0) {
                	trigger = 1;
                	cap = ix;
                }
            }//fim teste nivelvdo trigger
        } else {//se triggou, conte até armazenar o final do vetor
            cap++;//contando
        }
        i++;//indexador para armazenamento
        if (cap >= num_pontos) {
            	etapa = formatar;
                j = 0;//a partir daqui até na linha onde se tem "i_wrap=al", inicializa tudo para poder formatar
                inc = -1;
                for (seleciona_canal = 0; seleciona_canal < num_canais; ++seleciona_canal) {
					mptr_fila[seleciona_canal] = mfila[seleciona_canal];//endereço
					*mptr_fila[seleciona_canal] = matriz[seleciona_canal][al];//conteúdo
					mptr_fila[seleciona_canal]++;
				}
        	    i_wrap = al;
        }
        break;
    case formatar:
        if (j++ < num_pontos) {//percorra todos os pontos do vetor
			i_wrap += num_pontos - al;//oti
			if (i_wrap >= num_pontos) i_wrap -= num_pontos;//se passar do limite do vetor, ache o indice adequado

			for (seleciona_canal = 0; seleciona_canal < num_canais; ++seleciona_canal) {//faz a formatação para esquerda
				*mptr_fila[seleciona_canal] = matriz[seleciona_canal][i_wrap];
				mptr_fila[seleciona_canal] += inc;
				matriz[seleciona_canal][i_wrap] = *mptr_fila[seleciona_canal];
			}
			inc = -inc;
        } else etapa = stop;
        break;
    case stop: //etapas visiveis ao usuário.
        if (trigger == 0) {//se trigger nulo, fique armazenando os pontos
            etapa = armazenar;
            i = 0; cap=0;
        }
        else if (automatico) {//
			static int32 divfreq_sw = 0;
			if (divfreq_sw++ > (int32)((Uint32)150 * (Uint16)num_pontos)) {//divisor de frequencia por software
				trigger = 0; divfreq_sw = 0;
				nivel_trigger +=10;//DEON
			} //divisor_freq_sw
		} //else_if
        break;
    case set:
    	if (inicializar) {//vai fazer uma única vez assim que inicializar for !=0
			inicializar = 0;
			gereNum_primo(); //gere número primo para que o algoritmo de deslocamento (etapa formatar) funcione
			if (matriz != NULL) { // se a matriz foi criada antes, elimine-a e todos os outros ponteiros auxiliares
				Uint16 aux1;
				for (aux1 = 0; aux1 < num_canais; aux1++) {
					free(matriz[aux1]);
					matriz[aux1] = NULL;
					free(mfila[aux1]);
					mfila[aux1] = NULL;
				}
				free(canal);free(matriz);free(mptr_fila);free(mfila); //libera a memória reservada para esses 4 ponteiros
				matriz = NULL;canal = NULL;mptr_fila = NULL;mfila = NULL; //e os inicializo com NULL
			} // fim se a matriz foi criada
			criar_matriz(); //criar matriz
			if (falha_aloc) //falhou a alocação
				break; //saia imediatamente
			else //não houve falha na alocação, ótimo
			{
				etapa = armazenar;
				static Uint16 cont = 1;
				if (cont) {
					cont = 0; //só carrega uma vez só, logo após a primeira inicialização
					canal[0] = &AdcMirror.ADCRESULT0; //na prática essa atribuição vai ser na janela EXPRESSION
					canal[1] = &AdcMirror.ADCRESULT1;
				}
			}
		} //if_inicializar
    } // fim do switch
}//retorne

void criar_matriz(){//cria a matriz de pontos e os ponteiros auxiliares
	Uint16 i;//para o laço for a seguir
	matriz = ((Uint16 **) malloc(num_canais * sizeof(Uint16*)));//inicializa o ponteiro duplo matriz com um endereço estimável !=NULL
	canal = ((Uint16 **) malloc(num_canais * sizeof(Uint16*)));//" para o ponteiro duplo canal
	mfila = ((Uint16 **) malloc(num_canais * sizeof(Uint16*)));//" " ... mfila
	mptr_fila = ((Uint16 **) malloc(num_canais * sizeof(Uint16*)));//cada mptr_fila[i] será um ponteiro

	if ( (matriz == NULL) || (canal == NULL) || (mfila == NULL) || (mptr_fila == NULL)) {//se houver falla na alocação,
		falha_aloc = 1; return;//sinalize e saia imediatamente
	}
	//canal[i] acessivel no "expression view"
	for (i = 0; i < num_canais; i++) { //criação da matriz toda de fato
		matriz[i] = (Uint16 *) malloc(num_pontos * sizeof(Uint16));//minha matriz[i] tem que ter num_pontos colunas
		mfila[i] = (Uint16 *) malloc(2 * sizeof(Uint16));//meu vetor mfila[i] tem que ter apenas 2 colunas
		if ((matriz[i] == NULL) || (mfila[i] == NULL)){//se falhou a criação do vetor de duas posições,
			falha_aloc = 1;//sinalize
			return;// e saia imediatamente
		}
	}
}
//função necessária para que o meu algortimo de deslocamento funcione corretamente
void gereNum_primo() {	Uint16 j;//para o laço for a seguir
    (num_pontos & 1) ? : num_pontos++; //
    volta:
    for (j = 3; j < num_pontos; j += 2) { //num_pontos aqui tem que ser sempre ímpar
        if (( num_pontos % j) == 0) {
        	num_pontos += 2;//avança para um número maior de pontos, até que seja ímpar e primo
            goto volta;
        }
    }
}
