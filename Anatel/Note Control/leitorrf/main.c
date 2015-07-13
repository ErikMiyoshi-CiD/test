/*
 * leitorrf.c
 *
 * Created: 2013-09-12 11:33:23 AM
 *  Author: Albert
 */ 
#include "sam.h"
#include "helper.h"
#include "pinos.h"
#include "RF_common.h"
#include "RF_ASK.h"
#include "RF_Mifare.h"
#include "delay.h"

int main(void){		
	//1 - Inicializa o sistema e os clocks
	//2 - Verifica se estamos em modo de programa��o ou n�o
	//3 - L� o modo atual de funcionamento na EEPROM (ASK/FSK/PSK/Mifare?)
	//4 - Configura de acordo
	//5 - Roda e demodula	
	
	system_init();

	modo_leitor();
	
	while(1)
	{
		switch(ler_tipo_leitor())
		{
			case TIPO_ASK:
			ASK_Init();
			ASK_Run();
			break;
			case TIPO_FSK:
			break;
			case TIPO_PSK:
			break;
			case TIPO_MIFARE:
			Mifare_Init();
			Mifare_Run();
			break;
			case SEM_LEITOR:
			led_off();
			delay_s(1);
			led_idle();
			break;
		}
	}
	NVIC_SystemReset(); //Se chegou aqui deu besteira
}
