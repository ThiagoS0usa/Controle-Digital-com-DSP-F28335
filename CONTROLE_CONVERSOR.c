//##############################Descrição#############################################
//Este código implementa um controle digital simples de um conversor boost clássico
//mapeamento de pinos
//GPIO63,65,66,67,69,71,73 como GPIO normal. GPIO75 -> LED
//EPWM1A e EPWM1B. Pino de TZ1 ativado para forçar PWM1A e B em zero
//###########################################################################
// $Release Date: 17/02/2021, Autor: Everton Reis
//###########################################################################
#include "DSP28x_Project.h"
#include "io.h" //arquivo gerado pelo muxpin tool para selecionar a função dos pinos
#include <stdlib.h>
//#include "lib_osciloscopio.h"
#include "temporizacao_funcoes.h"
#include "botao.h"
#include "SFRA_includes.h"// para incluir todos os header files e as variáveis globais
float Duty_pu_DC = 0.35f;//valor para teste inicial de razao ciclica para varredura em frequencia em malha aberta
//===========================protótipos das funções===================================
void setup_DSP();//configura o hardware do DSP e inicializa variáveis
void InitTripZone();
void HRPWM2_Config(Uint16 period);
void DAC(Uint16 periodo);
void InitPWM();
__interrupt void adc_isr(void);
__interrupt void wakeint_isr(void);
__interrupt void cpu_timer0_isr(void);
__interrupt void cpu_timer1_isr(void);
__interrupt void cpu_timer2_isr(void);
//void sweep(Uint16 ad_result,float duty_ref, bool tipo);

Uint32 LoopCount, ConversionCount, var, inicial_time, WakeCount = 0, deon = 0;
float tensao, corrente, erro, kiL=0.1540f, kVc= 0.0129f, kerro= 0.0161f;
float duty = 0.5, duty_teste = 0.55, ref = 16.0f, integrando = 0;

Uint16 periodo = 20, dutyfine = 1, liga_controle = 4, ser = 1, SendChar = 0, testar = 0;//testar botão
Uint16 sdata, rdata, ErrorCount, ReceivedChar, tempo_exe, sinal;
char kmpc=1;

//parametro ."tempo" não pode passar o range de contagem do timer0
DT dt1 = {.tempo=149000000UL,.to=false};//C99 inicialização designada do C99 para o objeto dt1
DT dt2 = {.tempo=140000000UL,.to=false};//note que: .captimer e .to não foram inicializados
DB bot, bot1;// = {{15015015,0,false},0,0};
//DEON
//#define TAM 503
DT dtc = {.tempo=149000000UL,.to=false};
extern Uint16 num_pontos,num_canais,automatico,source_trigger,trigger,borda_descida,inicializar,falha_aloc;
extern void plot();
extern enum Etapas etapa;
enum EtapasControle {malhaAberta, SFRA, SFRA_MF,confParametros, malhaFechada} acionamento = SFRA;
main() {

	RT(&dt1);	RT(&dt2);
	Config_botao(46, &bot, 100);//Inicializando com essas funções
	Config_botao(47, &bot1, 100);//100ms de tempo de debouncing aproximadamente
	prepara_pino(39,1);
	setup_DSP();

	iniciar_SFRAeGUI();//SFRA
	for (;;) {
		if (initializationFlag == 1) {//SFRA
			SFRA_F_INIT(&SFRA1);
			initializationFlag = 0;
			SFRA1.start = 1;
		}
		//static Uint16 grt;
		if (BP(&bot)) {
			trigger = 0;
		}

		if (TO(&dt1)){ GpioDataRegs.GPBTOGGLE.bit.GPIO61 = 1; }//prepara_pino(63,1);
		//if (TO(&dt2)) GpioDataRegs.GPCTOGGLE.bit.GPIO65 = 1;
	} // fim do for
} // fim da main

__interrupt void adc_isr(void) { // a cada término de uma sequência, que é iniciado pelo período do PWM
//GpioDataRegs.GPBCLEAR.bit.GPIO48 = 1;// tic, acende led,no caso do LED da placa, nivel baixo faz acender
	float constante_conversao = 3.0f / 4096.0f;//temos também um offset de 24mV a mais na medição
	tensao = 21.64f*((AdcMirror.ADCRESULT0) * constante_conversao - 24e-3f - 1.5f); //(AdcMirror.ADCRESULT0) * constante_conversao - 24e-3f; *tensão = ADCMIRROR.ADCRESULT0
	//tensao = (AdcMirror.ADCRESULT0) * constante_conversao;//<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
	corrente = -12.65f*((AdcMirror.ADCRESULT1) * constante_conversao - 24e-3f - 1.5f);//Voltage2[ConversionCount]
//==============================================================================
//TODO controle, ponha as suas equações de controle digital aqui
//==============================================================================
// fim divisor de frequência por SW
	static float refde = 0.8f;
	float Vout1_Read;
	static float filtrado = 0.0f;
	filtrado = 4.5693805E-005f*tensao + 0.99995431f*filtrado;
	static uint16_t ver_filtro;
	ver_filtro = (Uint16)100*filtrado;
	if (filtrado > 22.0f){
		acionamento = SFRA;
		duty = 0.0f;
		EPwm1Regs.CMPA.half.CMPA = (Uint16) (duty * (long) (BOOST_PWM_PERIOD));
	}
	switch (acionamento) {//escolher o tipo de acionamento do conversor
	case malhaFechada:
	if (++LoopCount > 21) {//div_sw por 22, => 1 ms
		LoopCount = 0;
		//tensao = ((tensao)/21.64f + 1.5f)/3.0f;//Valor em P.U(float32) (AdcMirror.ADCRESULT0)*(1.0f) / (4096.0f);//((tensao/(21.64f)) + 1.5f)/(3.0f);
		tensao = AdcMirror.ADCRESULT0;
		ref=20.0f;//1.2f;
		erro = ref - tensao;
		duty = (kerro * integrando -kiL * corrente - kVc * tensao);
		integrando = integrando + erro;
		if (duty > 0.85f)
			duty = 0.85f;
		else if (duty < 0.0f)
			duty = 0.0f; //pra testar o duty fixo de 40% //EPwm1Regs.CMPA.half.CMPA = (Uint16) (duty * 3420.0);
	}//FIM DO CONTROLE
	EPwm1Regs.CMPA.half.CMPA = (Uint16) (duty * (long) (BOOST_PWM_PERIOD));
	break;
	case SFRA://SFRA para malha aberta (sem o controlador)
		Vout1_Read = (float32)AdcMirror.ADCRESULT0 /(4096.0);
		float Duty_pu_DC = 0.5f;
		float Duty_pu = SFRA_F_INJECT(Duty_pu_DC);
		EPwm1Regs.CMPA.half.CMPA = ((long) (BOOST_PWM_PERIOD)) * Duty_pu;//Update PWM value
		SFRA_F_COLLECT(&Duty_pu, &Vout1_Read);
		break;
	case SFRA_MF:
		if (BP(&bot1)) if (refde > 0.81f) refde = 0.8; else refde = 0.83;
		Vout1_Read = (float32) (AdcMirror.ADCRESULT0)/(4096.0f);//valor em P.U //((tensao / (21.64f)) + 1.5f) / (3.0f);
		float ref = SFRA_F_INJECT(refde); //esta funcão nao altera o seu parametro de entrada
		static Uint16 refe;
		refe = (Uint16) (ref * 5000);
		if (++LoopCount > 21) { //div_sw por 22, => 1 ms
			LoopCount = 0;
			//erro = ref - Vout1_Read; //Vout1_Read em P.U
			erro = ref - Vout1_Read;
			duty = (64.92f * kerro * integrando - kiL * corrente
					- kVc * tensao);//ou o ganho: Vout1_Read * 64.92f
			integrando = integrando + erro;
			if (duty > 0.85f)
				duty = 0.85f;
			else if (duty < 0.0f)
				duty = 0.0f; //pra testar o duty fixo de 40% //EPwm1Regs.CMPA.half.CMPA = (Uint16) (duty * 3420.0);
		}
		EPwm1Regs.CMPA.half.CMPA = ((long) (BOOST_PWM_PERIOD)) * duty;//Update PWM value
		SFRA_F_COLLECT(&duty, &Vout1_Read);//&duty
		break;
	case confParametros: //ajustar os parametros
		duty = 0; integrando = 0;
		if (kmpc) { // vai o MPC
			kiL = 0.1540f; //0.1540    0.0129   -0.0161
			kVc = 0.0129f;
			kerro = 0.0161f;
		} else { //vai o LQI
			kiL = 0.6911f;
			kVc = 0.0334f;
			kerro = 0.0571;
		} // fim senao, vai o LQI
		break;
	default:

		break;
	}
//==============================================================================
//               Roda a assistência de osciloscópio
//==============================================================================
	cronometro(&dtc, false); //tic .all
	plot();
	tempo_exe = (Uint16) cronometro(&dtc, true);

	AdcRegs.ADCTRL2.bit.RST_SEQ1 = 1;         // // Reinitialize for next ADC sequence:Reset SEQ1,
	AdcRegs.ADCST.bit.INT_SEQ1_CLR = 1;       // Clear INT SEQ1 bit,
	PieCtrlRegs.PIEACK.all = PIEACK_GROUP1;   // Acknowledge interrupt to PIE.
	static Uint32 v = 0;
	if (++v >= 200000){
		v = 0;//GpioDataRegs.GPBTOGGLE.bit.GPIO63 = 1;
	}//GpioDataRegs.GPBSET.bit.GPIO48 = 1;//toc, apaga led
}

//interrupções auxiliares
__interrupt void cpu_timer0_isr(void) {//1  segundo
	//GpioDataRegs.GPBTOGGLE.bit.GPIO61 = 1;
	//DEON
//	if (trigger) {
//		CpuTimer0.InterruptCount++;
//		if (CpuTimer0.InterruptCount > 40) {
//			trigger = 0;
//			CpuTimer0.InterruptCount = 0;
//		}
//	}
	// Acknowledge this interrupt to receive more interrupts from group 1
	PieCtrlRegs.PIEACK.all = PIEACK_GROUP1;
}

__interrupt void cpu_timer1_isr(void) {//5  milisegundo
	//EALLOW;
	CpuTimer1.InterruptCount++;
	//GpioDataRegs.GPBTOGGLE.bit.GPIO63 = 1;
	// The CPU acknowledges the interrupt.
	//EDIS;
}

__interrupt void cpu_timer2_isr(void) {//1 milisegundo
	EALLOW;
	CpuTimer2.InterruptCount++;
	//background loop do SFRA a cada 1 ms
	SFRA_F_BACKGROUND(&SFRA1);
	SerialCommsTimer++;//SFRA
	static char cont = 0;//divisor de frequência por software
	if (++cont >= 5) {
		cont = 0;
		SerialHostComms();//rodar isto a 5 ms
	}//GpioDataRegs.GPCTOGGLE.bit.GPIO65 = 1;
	EDIS;// The CPU acknowledges the interrupt.
}

__interrupt void wakeint_isr(void) { // a cada 4,37 ms
	WakeCount++;
	if (WakeCount > 100) {
		GpioDataRegs.GPCTOGGLE.bit.GPIO69 = 1;
		WakeCount = 0;
	}

// Acknowledge this interrupt to get more from group 1
	PieCtrlRegs.PIEACK.all = PIEACK_GROUP1;
}

void InitTripZone() {
	EALLOW;
	//TRIP ZONE, configurações necessárias. Com resistor de pull-up ativado e sem filtro(asynch) e
	GpioCtrlRegs.GPAPUD.bit.GPIO12 = 0;    // Enable pull-up on GPIO12 (TZ1)
	GpioCtrlRegs.GPAQSEL1.bit.GPIO12 = 3;  // Asynch input GPIO12 (TZ1)
	EPwm1Regs.TZSEL.bit.OSHT1 = 1; //One-shot TZ1 select
	// What do we want the TZ1 and TZ2 to do?
	EPwm1Regs.TZCTL.bit.TZA = TZ_FORCE_LO;
	EPwm1Regs.TZCTL.bit.TZB = TZ_FORCE_LO;
	// Enable TZ interrupt
	EPwm1Regs.TZEINT.bit.OST = 1;
	EDIS;
}

void InitGpio() {
	EALLOW;
	//escolha a função do pino, default GPIO. A ferramenta PIN MUX TOOL nos auxilia e em muito nisso
	GpioCtrlRegs.GPAMUX1.all = GPAMUX1_VALUE; // GPIO functionality GPIO0-GPIO15
	GpioCtrlRegs.GPAMUX2.all = GPAMUX2_VALUE; // GPIO functionality GPIO16-GPIO31
	GpioCtrlRegs.GPBMUX1.all = GPBMUX1_VALUE; // GPIO functionality GPIO32-GPIO39
	GpioCtrlRegs.GPBMUX2.all = GPBMUX2_VALUE; // GPIO functionality GPIO48-GPIO63
	GpioCtrlRegs.GPCMUX1.all = GPCMUX1_VALUE; // GPIO functionality GPIO64-GPIO79
	GpioCtrlRegs.GPCMUX2.all = GPCMUX2_VALUE; // GPIO functionality GPIO80-GPIO95
	//<<<<< escolha a direção do pino GP{A,B,C}{DIR}
	//Barra de LEDs smds
	//GpioCtrlRegs.GPBDIR.bit.GPIO46 = 0;//configurar para entrada de botao
	//GpioCtrlRegs.GPBPUD.bit.GPIO46 = 0;//com pull-up
	GpioCtrlRegs.GPBDIR.bit.GPIO61 = 1;
	GpioCtrlRegs.GPBDIR.bit.GPIO63 = 1;
	GpioCtrlRegs.GPCDIR.bit.GPIO65 = 1;
	GpioCtrlRegs.GPCDIR.bit.GPIO67 = 1;
	GpioCtrlRegs.GPCDIR.bit.GPIO69 = 1;
	GpioCtrlRegs.GPCDIR.bit.GPIO71 = 1;
	GpioCtrlRegs.GPCDIR.bit.GPIO73 = 1;

	//GpioCtrlRegs.GPADIR.all = 0x0000;      // GPIO0-GPIO31 are inputs
	//GpioCtrlRegs.GPBDIR.all = 0x0000;      // GPIO32-GPIO63 are inputs
	//GpioCtrlRegs.GPCDIR.all = 0x0000;      // GPI064-GPIO95 are inputs
	//<<<<< o que fazer com o pino
	GpioCtrlRegs.GPCDIR.bit.GPIO75 = 1;	//saida para O LED ON-BOARD!!!!
	GpioDataRegs.GPCCLEAR.bit.GPIO73 = 1;	// aterra para habilitar os leds
	GpioDataRegs.GPBSET.bit.GPIO61 = 1;	// 65 a 75 estou pondo em '1' para ativar todos os LEDs
	GpioDataRegs.GPBSET.bit.GPIO63 = 1;
	GpioDataRegs.GPCSET.bit.GPIO65= 1;
	GpioDataRegs.GPCSET.bit.GPIO67= 1;
	GpioDataRegs.GPCSET.bit.GPIO69 = 1;
	GpioDataRegs.GPCSET.bit.GPIO71 = 1;
	GpioDataRegs.GPCCLEAR.bit.GPIO75 = 1;	// ligando o LED na placa
	EDIS;
}

void HRPWM2_Config(Uint16 period)
{
// ePWM2 register configuration with HRPWM
// ePWM2A toggle low/high with MEP control on Rising edge

	EPwm2Regs.TBCTL.bit.PRDLD = TB_IMMEDIATE;	        // set Immediate load
	EPwm2Regs.TBPRD = period-1;		                    // PWM frequency = 1 / period
	EPwm2Regs.CMPA.half.CMPA = period / 2;	            // set duty 50% initially
    EPwm2Regs.CMPA.half.CMPAHR = (1 << 8);              // initialize HRPWM extension
	EPwm2Regs.CMPB = period / 2;	                    // set duty 50% initially
	EPwm2Regs.TBPHS.all = 0;
	EPwm2Regs.TBCTR = 0;

	EPwm2Regs.TBCTL.bit.CTRMODE = TB_COUNT_UP;
	EPwm2Regs.TBCTL.bit.PHSEN = TB_DISABLE;		         // ePWM2 is the Master
	EPwm2Regs.TBCTL.bit.SYNCOSEL = TB_SYNC_DISABLE;
	EPwm2Regs.TBCTL.bit.HSPCLKDIV = TB_DIV1;
	EPwm2Regs.TBCTL.bit.CLKDIV = TB_DIV1;

	EPwm2Regs.CMPCTL.bit.LOADAMODE = CC_CTR_ZERO;
	EPwm2Regs.CMPCTL.bit.LOADBMODE = CC_CTR_ZERO;
	EPwm2Regs.CMPCTL.bit.SHDWAMODE = CC_SHADOW;
	EPwm2Regs.CMPCTL.bit.SHDWBMODE = CC_SHADOW;

	EPwm2Regs.AQCTLA.bit.ZRO = AQ_SET;                  // PWM toggle low/high
	EPwm2Regs.AQCTLA.bit.CAU = AQ_CLEAR;
	EPwm2Regs.AQCTLB.bit.ZRO = AQ_CLEAR;
	EPwm2Regs.AQCTLB.bit.CBU = AQ_SET;

	EALLOW;
	EPwm2Regs.HRCNFG.all = 0x0;
	EPwm2Regs.HRCNFG.bit.EDGMODE = HR_REP;                //MEP control on Rising edge
	EPwm2Regs.HRCNFG.bit.CTLMODE = HR_CMP;
	EPwm2Regs.HRCNFG.bit.HRLOAD  = HR_CTR_ZERO;

	EDIS;

}

void DAC(Uint16 period){
	EPwm2Regs.TBPRD = period-1;
	float Valor_flutuante = (duty * period);                //0.54*10 =5.4
	Uint16 inteiro = (Uint16) (Valor_flutuante);
	EALLOW;
	EPwm2Regs.CMPA.half.CMPA = inteiro;	  //duty*periodo
	EPwm2Regs.CMPA.half.CMPAHR =  ((Uint16)((Valor_flutuante - inteiro)*37 + 1.5f) << 8);  // parte fracionária
	//EPwm2Regs.CMPA.all = ((Uint32)EPwm2Regs.CMPA.half.CMPA << 16) + (DutyFine << 8);
	EDIS;
}

void iniciar_SFRAeGUI(void){
	SFRA1.amplitude = 0.3;
	//Specify the length of SFRA
	SFRA1.Vec_Length = SFRA_FREQ_LENGTH;
	//Specify the SFRA ISR Frequency
	SFRA1.ISR_Freq = SFRA_ISR_FREQ;
	//Specify the Start Frequency of the SFRA analysis
	SFRA1.Freq_Start = SFRA_FREQ_START;
	//Specify the Frequency Step
	SFRA1.Freq_Step = FREQ_STEP_MULTIPLY;
	//Assign array location to Pointers in the SFRA object
	SFRA1.FreqVect = FreqVect;
	SFRA1.GH_MagVect = OL_MagVect;
	SFRA1.GH_PhaseVect = OL_PhaseVect;
	SFRA1.H_MagVect = Plant_MagVect;
	SFRA1.H_PhaseVect = Plant_PhaseVect;
	SFRA_F_INIT(&SFRA1);

	//SFRA gui
	CommsOKflg = 0;
	SerialCommsTimer = 0;
	//"Set" variables
	// assign GUI Buttons to desired flag addresses
	varSetBtnList[0] = (int16*) &initializationFlag;
	//"Get" variables
	//---------------------------------------
	// assign a GUI "getable" parameter address
	varGetList[0] = (int16*) &(SFRA1.Vec_Length);
	varGetList[1] = (int16*) &(SFRA1.status);
	varGetList[2] = (int16*) &(SFRA1.FreqIndex);
	//"Setable" variables
	//"Setable" variables
	//----------------------------------------
	// assign GUI "setable" by Text parameter address
	dataSetList[0] = (Uint32*) &(SFRA1.Freq_Start);
	dataSetList[1] = (Uint32*) &(SFRA1.amplitude);
	dataSetList[2] = (Uint32*) &(SFRA1.Freq_Step);// assign GUI "setable" by Text parameter address dataSetList[0] = (Uint32*)&(SFRA1.Freq_Start); dataSetList[1] = (Uint32*)&(SFRA1.amplitude); dataSetList[2] = (Uint32*)&(SFRA1.Freq_Step);

	// assign a GUI "getable" parameter array address
	arrayGetList[0] = (int32*) FreqVect;
	arrayGetList[1] = (int32*) OL_MagVect;
	arrayGetList[2] = (int32*) OL_PhaseVect;
	arrayGetList[3] = (int32*) Plant_MagVect;
	arrayGetList[4] = (int32*) Plant_PhaseVect;
	arrayGetList[5] = (int32*) &(SFRA1.Freq_Start);
	arrayGetList[6] = (int32*) &(SFRA1.amplitude);
	arrayGetList[7] = (int32*) &(SFRA1.Freq_Step);

	InitSciaGpio(); //para o SFRA
	SCIA_Init(37500000, 57600);
}

void setup_DSP(){
	InitSysCtrl();
	InitGpio(); //<<<<<<<<<<<<<<<<< para configurar a função do pino
	InitTripZone();

	DINT;
	InitPieCtrl();
	IER = 0x0000; // Disable CPU interrupts and clear all CPU interrupt flags:
	IFR = 0x0000;

	InitPieVectTable();
	EALLOW;
	// Interrupts that are used in this example are re-mapped to
	// This is needed to write to EALLOW protected register
	PieVectTable.ADCINT = &adc_isr;
	PieVectTable.WAKEINT = &wakeint_isr;
	PieVectTable.TINT0 = &cpu_timer0_isr;
	PieVectTable.XINT13 = &cpu_timer1_isr;
	PieVectTable.TINT2 = &cpu_timer2_isr;
	EDIS;

	// Step 4. Initialize all the Device Peripherals:
	InitAdc();
	InitCpuTimers();
	InitSpi();
	//InitSci();
	InitPWM(); // função criada por mim, inicializa PWM
	EALLOW;
	GpioCtrlRegs.GPAMUX1.bit.GPIO2 = 1;   // Configure GPIO2 as EPWM2A
	EDIS;

	// 150MHz CPU Freq, 1 second Period (in uSeconds)
	ConfigCpuTimer(&CpuTimer0, 150, 1E6);
	ConfigCpuTimer(&CpuTimer1, 150, 5000); // in a slow task (5ms), run SerialHostComms()
	ConfigCpuTimer(&CpuTimer2, 150, 1000); //para rodar a 1 ms o SerialCommsTimer++;

	CpuTimer0Regs.TCR.all = 0x4000; // Use write-only instruction to set TSS bit = 0
	CpuTimer1Regs.TCR.all = 0x4000; // Use write-only instruction to set TSS bit = 0
	CpuTimer2Regs.TCR.all = 0x4000; // Use write-only instruction to set TSS bit = 0

	//ServiceDog();

	// User specific code, enable interrupts:  Enable ADCINT in PIE
	PieCtrlRegs.PIEIER1.bit.INTx6 = 1;  // Enable Global interrupt INTM
	PieCtrlRegs.PIEIER1.bit.INTx8 = 1;  // Enable PIE Group 1 INT8
	PieCtrlRegs.PIEIER1.bit.INTx7 = 1; // Enable TINT0 in the PIE: Group 1 interrupt 7
	PieCtrlRegs.PIEIER2.bit.INTx1 = 1; // Enable EPWM INTn in the PIE: Group 2 interrupt 1-3

	IER |= M_INT1;
	IER |= M_INT2; // para habilitar a interrupção do grupo 2
	IER |= M_INT13;
	IER |= M_INT14;
	EINT;
	// Enable Global interrupt INTM
	ERTM;
	// Enable Global realtime interrupt DBGM

	ConversionCount = 0;
	LoopCount = 0;
	ErrorCount = 0;
	var = 0;
	sdata = 0;
	EALLOW;
	SysCtrlRegs.PCLKCR0.bit.TBCLKSYNC = 0;
	EDIS;
	//HRPWM2_Config(periodo); //EPWM2A com 7.5 MHz
	EALLOW;
	SysCtrlRegs.PCLKCR0.bit.TBCLKSYNC = 1;
	EDIS;
}

//void sweep(Uint16 ad_result,float duty_ref_dc, bool tipo){//0 duty, 1 para Ref
//float Vout1_Read = (float32) (ad_result)/(4096.0f);//((tensao/(21.64f)) + 1.5f)/(3.0f);
//float duty_ref = SFRA_F_INJECT(duty_ref_dc);//esta funcão nao altera o seu parametro de entrada
//static float duty;
//	if (tipo){
//		if (++LoopCount > 21) { //div_sw por 22, => 1 ms
//			LoopCount = 0;
//			static Uint16 refe;
//			refe = (Uint16) (duty_ref * 5000);
//			erro = duty_ref - Vout1_Read; //Vout1_Read em P.U
//			duty = (3.0f * kerro * integrando); //-kiL * corrente - kVc * tensao +
//			integrando = integrando + erro;
//			if (duty > 0.85f)
//				duty = 0.85f;
//			else if (duty < 0)
//				duty = 0.0f; //pra testar o duty fixo de 40% //EPwm1Regs.CMPA.half.CMPA = (Uint16) (duty * 3420.0);
//		} //FIM DO CONTROLE
//	}
//	EPwm1Regs.CMPA.half.CMPA = ((long) (BOOST_PWM_PERIOD)) * duty_ref; // * Duty_pu; //Update PWM value
//	SFRA_F_COLLECT(&duty_ref, &Vout1_Read);
//}
/*if (vai){
	if (ptr == NULL){
		ptr = (Uint16*) malloc(num_pontos * sizeof(Uint16));
		if (ptr==NULL) break;
		for (var = 0; var < num_pontos; ++var) { //gerando minha rampa de teste
			ptr[var] = var;
		}
		vai = 0;
	}else{
		free(ptr); ptr =NULL;
	}
}
*/
