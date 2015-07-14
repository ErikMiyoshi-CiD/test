/*
 * helper.c
 *
 * Created: 08/11/2013 18:27:43
 *  Author: Albert
 */ 

#include "helper.h"
#include "pinos.h"
#include "delay.h"
#include "RF_common.h"

/* ----------- GLOBALS start ----------- */
TIPO_OUTPUT tipo_output;
/* ------------ GLOBALS end ------------ */

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

void ioport_set_pin_input(uint8_t pino){
	//Configura e muda o estado de uma determinada porta
	PORT->Group[0].PINCFG[pino].reg = 0x2;
	PORT->Group[0].DIRCLR.reg = (1 << pino);
	PORT->Group[0].CTRL.bit.SAMPLING = (1 << pino);
}

uint8_t ioport_get_pin_level(uint8_t pino){
	if (PORT->Group[0].IN.reg & (1 << pino))
		return 1;
	else
		return 0;
}

void system_flash_set_waitstates(uint8_t wait_states)
{
	NVMCTRL->CTRLB.bit.RWS = wait_states; 
}

int system_gclk_is_syncing(void)
{
	if (GCLK->STATUS.reg & GCLK_STATUS_SYNCBUSY) 
		return 1;
	return 0;
}

void ClockInit(){
	int i;
	/* Workaround for errata 10558 */
	/*SYSCTRL = System Controller (Pg 127) */
	SYSCTRL->INTFLAG.reg = /*SYSCTRL_INTFLAG_BOD12RDY |*/ SYSCTRL_INTFLAG_BOD33RDY |
		/*SYSCTRL_INTFLAG_BOD12DET |*/ SYSCTRL_INTFLAG_BOD33DET |
		SYSCTRL_INTFLAG_DFLLRDY;
	
	/* Configura os wait states de acesso à memória */
	system_flash_set_waitstates(0);
	
	//Inicializar XOSC de 8MHz
	SYSCTRL->XOSC.bit.AMPGC = 1;
	SYSCTRL->XOSC.bit.ONDEMAND = 0;
	SYSCTRL->XOSC.bit.RUNSTDBY = 0;
	SYSCTRL->XOSC.bit.STARTUP = 0xF;
	SYSCTRL->XOSC.bit.XTALEN = 1;
	SYSCTRL->XOSC.reg |= SYSCTRL_XOSC_ENABLE;
	while(SYSCTRL->PCLKSR.bit.XOSCRDY==0);
	
	//SYSCTRL->DFLLCTRL.reg = /*DISABLE_QL=0|DISABLE_CHILL=0|ONDEMAND=0|LOSELOCK=0|DONOTTRACK=0*/ 0b0100 /*closed,enable,reserved*/;
	//while (!(SYSCTRL->PCLKSR.reg & SYSCTRL_PCLKSR_DFLLRDY));
	//SYSCTRL->DFLLMUL.bit.CSTEP = (0x1f / 4);
	//SYSCTRL->DFLLMUL.bit.FSTEP = (0xff / 4);
	//SYSCTRL->DFLLMUL.bit.MUL = 6;
	///* Write full configuration to DFLL control register */
	//SYSCTRL->DFLLCTRL.reg =  /*DISABLE_QL=0|DISABLE_CHILL=0|ONDEMAND=0|LOSELOCK=0|DONOTTRACK=0*/ 0b0110 /*closed,enable,reserved*/;
	//while (!(SYSCTRL->PCLKSR.reg & SYSCTRL_PCLKSR_DFLLRDY));	
	//while (!(SYSCTRL->PCLKSR.reg & SYSCTRL_PCLKSR_DFLLLCKF));
	
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
		if (i==6)
			GCLK->GENDIV.reg  = 0x2000+i;
		else
			GCLK->GENDIV.reg  = 0x0100+i;
		while (system_gclk_is_syncing()) {/* Wait for synchronization */};
			
		if (i==6) //GCLK para WDT
			GCLK->GENCTRL.reg  = i | GCLK_SOURCE_OSCULP32K << GCLK_GENCTRL_SRC_Pos | GCLK_GENCTRL_GENEN;
		else
			GCLK->GENCTRL.reg  = i | GCLK_SOURCE_XOSC << GCLK_GENCTRL_SRC_Pos | GCLK_GENCTRL_GENEN;
	}
	
	//Set TC gclock=0
	for (i=0x13; i<=0x16;i++)
	{
		while (system_gclk_is_syncing()) {/* Wait for synchronization */};
		*((uint8_t*)&GCLK->CLKCTRL.reg) = (uint8_t)i;
		GCLK->CLKCTRL.reg |= GCLK_CLKCTRL_CLKEN;
	}
	
	//Set SERCOM3 Gclock=0
	for (i=0x0C; i<=0x12;i++)
	{
		while (system_gclk_is_syncing()) {/* Wait for synchronization */};
		*((uint8_t*)&GCLK->CLKCTRL.reg) = (uint8_t)i;
		GCLK->CLKCTRL.reg |= GCLK_CLKCTRL_CLKEN;
	}
	
	//Set WDT Gclock=0
	for (i=0x01; i<=0x01;i++)
	{
		while (system_gclk_is_syncing()) {/* Wait for synchronization */};
		*((uint8_t*)&GCLK->CLKCTRL.reg) = (uint8_t)i;
		GCLK->CLKCTRL.reg |= (0x600 | GCLK_CLKCTRL_CLKEN);
	}
		
	//Set CPU, APBA, APBB and APBC clocks
	PM->CPUSEL.reg = (uint32_t)0;
	PM->APBASEL.reg = (uint32_t)0;
	PM->APBBSEL.reg = (uint32_t)0;
	PM->APBCSEL.reg = (uint32_t)0;
}


//Esta função inicializa o TC1 WO[1] --> Pino do 125kHz da antena
void Init125khz(void){
	//Associa o pino de 125kHz ao TC1
	PORT->Group[0].PINCFG[7].bit.PMUXEN = 1; //PA07
	PORT->Group[0].PMUX[3].bit.PMUXO= 0x5; //Periférico F = TC2/WO[0]
	
	//Habilita o clock do TC1
	PM->APBCMASK.reg |= PM_APBCMASK_TC1;
	
	//Reseta o TC1
	TC1->COUNT16.CTRLA.bit.SWRST = 1;
	while (TC1->COUNT16.CTRLA.bit.SWRST!=0);
	
	//Configura o TC1
	TC1->COUNT16.CTRLA.reg = TC_CTRLA_WAVEGEN_MFRQ; //GCLK/1, não roda em STBY, MFRQ, COUNT16, Disable
	TC1->COUNT16.CTRLBSET.reg = 0; //Count up infinitamente
	TC1->COUNT16.CTRLC.reg = 0; //Sem capture/Compare e não inverte nenhuma saída
	TC1->COUNT16.CC[0].reg = 31; //Valor para 8MHz é 32 (para 48MHz é 384/2)
	
	//Habilita o TC1
	TC1->COUNT16.CTRLA.bit.ENABLE = 1;
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
	
	//Não vamos habilitar o TC5 agora. Caso contrário o buzzer já começaria a tocar
	//TC5->COUNT16.CTRLA.bit.ENABLE = 1;
}

void buzz(uint32_t tempo){
	/* Apita o buzzer em busy wait por ? segundo */
	TC5->COUNT16.CTRLA.bit.ENABLE = 1;
	delay_ms(tempo);
	TC5->COUNT16.CTRLA.bit.ENABLE = 0;
}

void buzz_on(void)
{
	TC5->COUNT16.CTRLA.bit.ENABLE = 1;
}

void buzz_off(void)
{
	TC5->COUNT16.CTRLA.bit.ENABLE = 0;
}

void ok_feedback(void){
	/* Fornece feedback visual (led) e de áudio(buzzer) positivo*/
	ioport_set_pin_level(PIN_LED_RED,1);
	ioport_set_pin_level(PIN_LED_GRN,0);
	ioport_set_pin_level(PIN_LED_RED,0);
	ioport_set_pin_level(PIN_LED_GRN,1);
}

void led_idle(void){
	ioport_set_pin_level(PIN_LED_RED,1);
	led_green();
}

void led_yellow(void){
	ioport_set_pin_level(PIN_LED_RED,0);
	ioport_set_pin_level(PIN_LED_GRN,0);
}

void led_green(void){
	ioport_set_pin_level(PIN_LED_GRN,0);	
}

void led_red(void){
	ioport_set_pin_level(PIN_LED_RED,0);
}

void led_off(void){
	ioport_set_pin_level(PIN_LED_RED,1);
	ioport_set_pin_level(PIN_LED_GRN,1);
}

void pin_configure(void)
{
	ioport_set_pin_input(PIN_ASK_IN);
	ioport_set_pin_input(PIN_B0);
	ioport_set_pin_input(PIN_B1);
	ioport_set_pin_input(PIN_B2);
}

void wdt_init(void)
{
	PM->APBAMASK.reg |= PM_APBAMASK_WDT;
	
	//WDT->CTRL.bit.ALWAYSON=1;
	while (WDT->STATUS.reg & WDT_STATUS_SYNCBUSY);
	WDT->CTRL.bit.WEN=0; //sem window
	while (WDT->STATUS.reg & WDT_STATUS_SYNCBUSY);
	WDT->CONFIG.bit.PER=0x9; //4s ou explode!
	while (WDT->STATUS.reg & WDT_STATUS_SYNCBUSY);
	WDT->INTENCLR.reg = 0xFF;
	while (WDT->STATUS.reg & WDT_STATUS_SYNCBUSY);
	WDT->CTRL.bit.ENABLE=1;
}

void wdt_reset(void)
{
	while (WDT->STATUS.reg & WDT_STATUS_SYNCBUSY);
	WDT->CLEAR.reg=0xA5;
}

void system_init(void){
	/* Inicializa clock */
	ClockInit();
	//Habilita IO Port
	PM->APBBMASK.reg |= PM_APBBMASK_PORT;	
	//Configura pinos
	pin_configure();
	//Inicializa delays
	delay_init();
	//Inicializa buzzer
	buzzer_clock_init();
	//Inicializa WDT
	wdt_init();
}

MODO_LEITOR avaliar_modo_leitor(void)
{
	return MODO_NORMAL;
}

void modo_leitor(void)
{
	uint16_t temp=*(volatile uint16_t *)USER_INFO_ADD;	

	switch(avaliar_modo_leitor())
	{
		case MODO_NORMAL:
			led_green();
			led_idle();
		break;
	}
}

void programa_config (uint16_t dados)
{
		NVMCTRL->ADDR.reg=USER_INFO_ADD/4;
		NVMCTRL->CTRLA.reg = NVMCTRL_CTRLA_CMD_ER | NVMCTRL_CTRLA_CMDEX_KEY;
		while (NVMCTRL->INTFLAG.bit.READY==0);
		
		NVMCTRL->CTRLA.reg = NVMCTRL_CTRLA_CMD_PBC | NVMCTRL_CTRLA_CMDEX_KEY;
		while (NVMCTRL->INTFLAG.bit.READY==0);
		
		NVMCTRL->ADDR.reg=USER_INFO_ADD;
		*(volatile uint16_t *)USER_INFO_ADD=dados;
		
		NVMCTRL->CTRLA.reg = NVMCTRL_CTRLA_CMD_WP | NVMCTRL_CTRLA_CMDEX_KEY;
		while (NVMCTRL->INTFLAG.bit.READY==0);
}

TIPO_LEITOR ler_tipo_leitor(void)
{
	uint8_t b0,b1,b2;
	b0 = ioport_get_pin_level(PIN_B0);
	b1 = ioport_get_pin_level(PIN_B1);
	b2 = ioport_get_pin_level(PIN_B2);
	if(b0 == 0 && b1 == 0 && b2 == 0)
		return TIPO_ASK;
	else if(b0 == 1 && b1 == 0 && b2 == 0)
		return TIPO_MIFARE;
	else
		return SEM_LEITOR;
}