/*
 * leitorrf.c
 *
 * Created: 2013-09-12 11:33:23 AM
 *  Author: Albert
 */ 

#include "sam.h"

static inline void system_flash_set_waitstates(uint8_t wait_states)
{
	NVMCTRL->CTRLB.bit.RWS = wait_states;
}

static inline int system_gclk_is_syncing(void)
{
	if (GCLK->STATUS.reg & GCLK_STATUS_SYNCBUSY) {
		return 1;
	}

	return 0;
}

void ClockInit()
{
	int i;
	/* Workaround for errata 10558 */
	SYSCTRL->INTFLAG.reg = SYSCTRL_INTFLAG_BOD12RDY | SYSCTRL_INTFLAG_BOD33RDY |
		SYSCTRL_INTFLAG_BOD12DET | SYSCTRL_INTFLAG_BOD33DET |
		SYSCTRL_INTFLAG_DFLLRDY;
	
	/* Configura os wait states de acesso à memória */
	system_flash_set_waitstates(0);
	
	//Inicializar OSC8M
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



//Esta função inicializa o TC2 WO[0] --> Pino do 125kHz da antena
void Init125khz(void)
{
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
	TC2->COUNT16.CTRLBSET.reg = 0; //Count up infinitamente
	TC2->COUNT16.CTRLC.reg = 0; //Sem capture/Compare e não inverte nenhuma saída 
	TC2->COUNT16.CC[0].reg = 31; //Valor para 8MHz é 32 (para 48MHz é 384/2)
		
	//Habilita o TC2
	TC2->COUNT16.CTRLA.bit.ENABLE = 1;
}

volatile uint32_t a;

int main(void)
{
    /* Initialize the SAM system */
	//Esse system Init parece inútil e errado
    //SystemInit();
	/* Inicializa os clocks */
	ClockInit();
	Init125khz();

	TC2->COUNT16.COUNT.reg = 22;
	a=TC2->COUNT16.COUNT.reg;

    while (1) 
    {
        //TODO:: Please write your application code 
		a=TC2->COUNT16.COUNT.reg;
    }
}
