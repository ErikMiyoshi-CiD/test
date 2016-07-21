#include <asf.h>

#include "helper.h"
#include "pinos.h"
#include "delay.h"
#include "RF_common.h"

TIPO_OUTPUT tipo_output;

//Esta fun��o inicializa o TC1 WO[1] --> Pino do 125kHz da antena
void Init125khz(void){
	//Associa o pino de 125kHz ao TC1
	PORT->Group[0].PINCFG[7].bit.PMUXEN = 1; //PA07
	PORT->Group[0].PMUX[3].bit.PMUXO= 0x5; //Perif�rico F = TC2/WO[0]
	
	//Habilita o clock do TC1
	PM->APBCMASK.reg |= PM_APBCMASK_TC1;
	
	//Reseta o TC1
	TC1->COUNT16.CTRLA.bit.SWRST = 1;
	while (TC1->COUNT16.CTRLA.bit.SWRST!=0);
	
	//Configura o TC1
	TC1->COUNT16.CTRLA.reg = TC_CTRLA_WAVEGEN_MFRQ; //GCLK/1, n�o roda em STBY, MFRQ, COUNT16, Disable
	TC1->COUNT16.CTRLBSET.reg = 0; //Count up infinitamente
	TC1->COUNT16.CTRLC.reg = 0; //Sem capture/Compare e n�o inverte nenhuma sa�da
	TC1->COUNT16.CC[0].reg = 31; //Valor para 8MHz � 32 (para 48MHz � 384/2)
	
	//Habilita o TC1
	//TC1->COUNT16.CTRLA.bit.ENABLE = 1;
}

void buzzer_clock_init(void){
	//Associa o pino do Buzzer ao TC5
	PORT->Group[0].PINCFG[25].bit.PMUXEN = 1;
	PORT->Group[0].PMUX[12].bit.PMUXO = 0x5; //Perif�rico F = TC5/WO[1]

	//Habilita o clock do TC5
	PM->APBCMASK.reg |= PM_APBCMASK_TC5;
	
	//Reseta o TC5
	TC5->COUNT16.CTRLA.bit.SWRST = 1;
	while (TC5->COUNT16.CTRLA.bit.SWRST!=0);
	
	//Configura o TC5
	TC5->COUNT16.CTRLA.reg = TC_CTRLA_WAVEGEN_MFRQ; //GCLK/1, n�o roda em STBY, MFRQ, COUNT16, Disable
	TC5->COUNT16.CTRLBSET.reg = 0; //Count up infinitamente
	TC5->COUNT16.CTRLC.reg = 0; //Sem capture/Compare e n�o inverte nenhuma sa�da
	TC5->COUNT16.CC[0].reg = 1000; ////Valor de topo do counter (aqui que alteramos a freq!!!!)
	
	//N�o vamos habilitar o TC5 agora. Caso contr�rio o buzzer j� come�aria a tocar
	//TC5->COUNT16.CTRLA.bit.ENABLE = 1;
}

void buzz(uint32_t tempo){
	/* Apita o buzzer em busy wait por ? segundo */
	//TC5->COUNT16.CTRLA.bit.ENABLE = 1;
	//delay_ms(tempo);
	//TC5->COUNT16.CTRLA.bit.ENABLE = 0;
}

void buzz_on(void)
{
	//TC5->COUNT16.CTRLA.bit.ENABLE = 1;
}

void buzz_off(void)
{
	//TC5->COUNT16.CTRLA.bit.ENABLE = 0;
}

void ok_feedback(void){
	/* Fornece feedback visual (led) e de �udio(buzzer) positivo*/
	ioport_set_pin_level(PIN_LED_RED,0);
	ioport_set_pin_level(PIN_LED_GRN,1);
	buzz(300);
	ioport_set_pin_level(PIN_LED_RED,1);
	ioport_set_pin_level(PIN_LED_GRN,0);
}

void led_idle(void){
	led_red();
}

void led_yellow(void){
	ioport_set_pin_level(PIN_LED_RED,1);
	ioport_set_pin_level(PIN_LED_GRN,1);
}

void led_green(void){
	ioport_set_pin_level(PIN_LED_RED,0);
	ioport_set_pin_level(PIN_LED_GRN,1);	
}

void led_red(void){
	ioport_set_pin_level(PIN_LED_RED,1);
	ioport_set_pin_level(PIN_LED_GRN,0);
}

void led_off(void){
	ioport_set_pin_level(PIN_LED_RED,0);
	ioport_set_pin_level(PIN_LED_GRN,0);
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
	//WDT->CTRL.bit.ENABLE=1;
}

void wdt_reset(void)
{
	while (WDT->STATUS.reg & WDT_STATUS_SYNCBUSY);
	WDT->CLEAR.reg=0xA5;
}

static void pin_configure(void)
{
	//ioport_set_pin_input(PIN_ASK_IN);
	//ioport_set_pin_input(PIN_LED_INPUT);
	//ioport_set_pin_input(PIN_MS_BUZZ);
}

void user_init(void){
	/* Inicializa clock */
	//ClockInit();
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
	//Se estivermos em modo programa��o, o pino 
	for (uint8_t i=0;i<16;i++)
	{
		int val=(i & 1);
		ioport_set_pin_level(PIN_D0_TX_CLK,val);
		delay_us(50);
		if (ioport_get_pin_level(PIN_MS_BUZZ) == val) //D0 tem inversor
			return MODO_NORMAL;
	}
	return MODO_PROGRAMACAO;
}

void modo_leitor(void)
{
	uint16_t temp=*(volatile uint16_t *)USER_INFO_ADD;	

	switch(avaliar_modo_leitor())
	{
		case MODO_PROGRAMACAO:
			temp = temp & 0xFF; //limpa o byte superior
			for (uint8_t i=0;i<5;i++)
			{
				led_red();
				delay_ms(300);
				led_green();
				buzz(300);
			}
			if (ioport_get_pin_level(PIN_LED_INPUT)==1)
			{
				tipo_output=OUTPUT_WIEGAND;	
				temp |= 'W' << 8;
				led_green();	
			}
			else
			{
				tipo_output=OUTPUT_ABATRACK;
				temp |= 'A' << 8;
				led_yellow();
			}
			programa_config(temp);
			while(1); //Espera o cara desligar
			break;
		case MODO_NORMAL:
			if ((temp & 0xFF00) >> 8 == 'A')
				tipo_output=OUTPUT_ABATRACK;
			else
				tipo_output=OUTPUT_WIEGAND;
			led_green();
			buzz(500);
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
	//Liga clock do NVM
	PM->APBBMASK.reg |= PM_APBBMASK_NVMCTRL;
	
	uint8_t tipo=(*(volatile uint16_t *)USER_INFO_ADD & 0xFF);
	switch (tipo)
	{
	case 'A':
		return TIPO_ASK;
	case 'F':
		return TIPO_FSK;
	case 'P':
		return TIPO_PSK;
	case 'M':
		return TIPO_MIFARE;
	default: //nunca foi programado
		programa_config(((uint16_t)'W' << 8) + 'A');
		return TIPO_ASK;
	}
}