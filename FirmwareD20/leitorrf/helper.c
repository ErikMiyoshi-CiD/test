/*
 * helper.c
 *
 * Created: 08/11/2013 18:27:43
 *  Author: Luiz
 */ 

#include "helper.h"

void ioport_set_pin_level(uint8_t pino, int estado){
	//Configura e muda o estado de uma determinada porta
	PORT->Group[0].PINCFG[pino].reg = 0;
	PORT->Group[0].DIRSET.reg = (1 << pino);
	if(estado){
		PORT->Group[0].OUTSET.reg = ( 1 << pino);
		return;
	}
	PORT->Group[0].OUTCLR.reg = (1 << pino);
}

void delay_us(volatile int tempo){
	while(tempo){tempo--;}
	/*
	escreve valor no timer
	somar tempo de delay
	comparar com o valor atual até ser menor
	*/
}

/* static inline */void system_flash_set_waitstates(uint8_t wait_states) /* Para cada combinação de
frequência do barramento de dados (AHB) e voltagem existe um wait state que otimiza o acesso à 
memória não volátil (flash) (Pg 587). O wait state define como será feita a comunicação no 
barramento de dados. */
{
	NVMCTRL->CTRLB.bit.RWS = wait_states; /* NVMCTRL = "Non-Volatile Memory Controller" - Pg 264
	(*NVMCTRL).
	CTRLB.bit.RWS = Control B (Bits do registrador responsáveis por definir o wait state) (Pg 268)
	Tudo indica que esse comando escreve no registrador certo o wait state da memória flash */
}

/* static inline */int system_gclk_is_syncing(void)/*Essa função serve para verificar a flag de
sincronização de registrador, ou seja, vai retornar 1 se está acontecendo sincronização e 0 se não
está. Ela deve ser necessária para fazer com que outras funções que necessitem acessar 
registradores que necessitam de sincronização não acabem tentando acessar o que não podem na hora
errada, ou algo assim... */
{
	if (GCLK->STATUS.reg & GCLK_STATUS_SYNCBUSY) {/*GCLK = "Generic Clock" (Pg 78), serve para
		receber diversos clocks externos e distribuir diferentes clocks internos onde for necessário
		(sempre menores ou iguais aos da fonte). Pode ser encarado como um centro de distribuição
		de clocks: recebe clocks em containers, daí os empacota no tamanho conveniente e manda pra
		quem precisa. */
		return 1;
	}

	return 0;
}

void ClockInit(){
	int i;
	/* Workaround for errata 10558 */
	/*SYSCTRL = System Controller (Pg 127) */
	SYSCTRL->INTFLAG.reg = SYSCTRL_INTFLAG_BOD12RDY | SYSCTRL_INTFLAG_BOD33RDY |
		SYSCTRL_INTFLAG_BOD12DET | SYSCTRL_INTFLAG_BOD33DET |
		SYSCTRL_INTFLAG_DFLLRDY;
	
	/* Configura os wait states de acesso à memória */
	system_flash_set_waitstates(0);
	
	//Inicializar OSC8M (8MHz Internal Oscillator)
	SYSCTRL->OSC8M.bit.PRESC = 0;
	SYSCTRL->OSC8M.bit.ONDEMAND = 1;
	SYSCTRL->OSC8M.bit.RUNSTDBY = 0;
	SYSCTRL->OSC8M.reg |= SYSCTRL_OSC8M_ENABLE;
	
	///TODO: Inicializar Oscilador Externo de 8/16MHz
	///TODO: Inicializar DFLL para 48MHz
	
	
	//Inicializa GCLK
	PM->APBAMASK.reg |= PM_APBAMASK_GCLK;

	/* Software reset the module to ensure it is re-initialized correctly */
	GCLK->CTRL.reg = GCLK_CTRL_SWRST;
	while (GCLK->CTRL.reg & GCLK_CTRL_SWRST) {/* Wait for reset to complete */};
		
	for (i=7;i>=0;i--)
	{
		/* Select the correct generator */
		*((uint8_t*)&GCLK->GENDIV.reg) = (uint8_t)i;
		/* Write the new generator configuration */
		while (system_gclk_is_syncing()) {/* Wait for synchronization */};
		GCLK->GENDIV.reg  = 1;
		while (system_gclk_is_syncing()) {/* Wait for synchronization */};
		GCLK->GENCTRL.reg = (GCLK_SOURCE_OSC8M << GCLK_GENCTRL_SRC_Pos) | GCLK_GENCTRL_GENEN;
	}
	
	for (i=0x13; i<=0x16;i++)
	{
		while (system_gclk_is_syncing()) {/* Wait for synchronization */};
		*((uint8_t*)&GCLK->CLKCTRL.reg) = (uint8_t)i;
		GCLK->CLKCTRL.reg |= GCLK_CLKCTRL_CLKEN;
	}
	
	//Set CPU, APBA and APBB clocks
	PM->CPUSEL.reg = (uint32_t)0;
	PM->APBASEL.reg = (uint32_t)0;
	PM->APBBSEL.reg = (uint32_t)0;
	PM->APBCSEL.reg = (uint32_t)0;
}


//Esta função inicializa o TC1 WO[1] --> Pino do 125kHz da antena
void Init125khz_NOVO(void){
	//Associa o pino de 125kHz ao TC1
	PORT->Group[0].PINCFG[7].bit.PMUXEN = 1; //PA07
	PORT->Group[0].PMUX[3].bit.PMUXO= 0x5; //Periférico F = TC2/WO[0]
	
	//Habilita o clock do TC1
	PM->APBCMASK.reg |= PM_APBCMASK_TC1;
	
	//Reseta o TC1
	TC1->COUNT16.CTRLA.bit.SWRST = 1;
	while (TC1->COUNT16.CTRLA.bit.SWRST!=0);
	
	//Configura o TC2
	TC1->COUNT16.CTRLA.reg = TC_CTRLA_WAVEGEN_MFRQ; //GCLK/1, não roda em STBY, MFRQ, COUNT16, Disable
	TC1->COUNT16.CTRLBSET.reg = 0; //Count up infinitamente
	TC1->COUNT16.CTRLC.reg = 0; //Sem capture/Compare e não inverte nenhuma saída
	TC1->COUNT16.CC[0].reg = 31; //Valor para 8MHz é 32 (para 48MHz é 384/2)
	
	//Habilita o TC1
	TC1->COUNT16.CTRLA.bit.ENABLE = 1;
}


//Esta função inicializa o TC2 WO[0] --> Pino do 125kHz da antena
void Init125khz(void){
	//Associa o pino de 125kHz ao TC2
	PORT->Group[0].PINCFG[0].bit.PMUXEN = 1;
	PORT->Group[0].PMUX[0].bit.PMUXE= 0x5; //Periférico F = TC2/WO[0]
	
	//Habilita o clock do TC2
	PM->APBCMASK.reg |= PM_APBCMASK_TC2;
		
	//Reseta o TC2
	TC2->COUNT16.CTRLA.bit.SWRST = 1;
	while (TC2->COUNT16.CTRLA.bit.SWRST!=0);
	
	//Configura o TC2
	TC2->COUNT16.CTRLA.reg = TC_CTRLA_WAVEGEN_MFRQ; //GCLK/1, não roda em STBY, MFRQ, COUNT16, Disable
	TC2->COUNT16.CTRLBSET.reg = 0; //Count up infinitament e
	TC2->COUNT16.CTRLC.reg = 0; //Sem capture/Compare e não inverte nenhuma saída 
	TC2->COUNT16.CC[0].reg = 31; //Valor para 8MHz é 32 (para 48MHz é 384/2)
		
	//Habilita o TC2
	TC2->COUNT16.CTRLA.bit.ENABLE = 1;
}

void buzzer_clock_init(void){
	//Associa o pino do Buzzer ao TC5
	PORT->Group[0].PINCFG[25].bit.PMUXEN = 1;
	PORT->Group[0].PMUX[12].bit.PMUXO = 0x5; //Periférico F = TC5/WO[1]

	//Habilita o clock do TC5
	PM->APBCMASK.reg |= PM_APBCMASK_TC5;
	
	//Reseta o TC5
	TC5->COUNT16.CTRLA.bit.SWRST = 1;
	while (TC5->COUNT16.CTRLA.bit.SWRST!=0);
	
	//Configura o TC5
	TC5->COUNT16.CTRLA.reg = TC_CTRLA_WAVEGEN_MFRQ; //GCLK/1, não roda em STBY, MFRQ, COUNT16, Disable
	TC5->COUNT16.CTRLBSET.reg = 0; //Count up infinitamente
	TC5->COUNT16.CTRLC.reg = 0; //Sem capture/Compare e não inverte nenhuma saída
	TC5->COUNT16.CC[0].reg = 1000; ////Valor de topo do counter (aqui que alteramos a freq!!!!)
	
	//Habilita o TC5
	//TC5->COUNT16.CTRLA.bit.ENABLE = 1;
}

void buzz(void){
	/* Apita o buzzer em busy wait por ? segundo */
	TC5->COUNT16.CTRLA.bit.ENABLE = 1;
	delay_us(300000);
	TC5->COUNT16.CTRLA.bit.ENABLE = 0;
}

void ok_feedback(void){
	/* Fornece feedback visual (led) e de áudio(buzzer) positivo*/
	
	ioport_set_pin_level(RED_LED,0);
	ioport_set_pin_level(GREEN_LED,1);
	buzz();
	ioport_set_pin_level(RED_LED,1);
	ioport_set_pin_level(GREEN_LED,0);
	
}

void system_init(void){
	/* Inicializa os clocks */
	ClockInit();
	//Init125khz();
	Init125khz_NOVO();
	//buzzer_clock_init();
	//Habilita o clock do TC2 e do TC5
	PM->APBBMASK.reg |= PM_APBBMASK_PORT;
	//ok_feedback();
}
