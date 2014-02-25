/*
 * leitorrf.c
 *
 * Created: 2013-09-12 11:33:23 AM
 *  Author: Albert
 */ 

#include "sam.h"
#include "helper.h"
#include "Wiegand.h"
#include "ABATK2.h"
#include "pinos.h"
#include "RF_common.h"
#include "RF_ASK.h"

int main(void){		
	//1 - Inicializa o sistema e os clocks
	//2 - L� o modo atual de funcionamento na EEPROM (ASK/FSK/PSK/Mifare?)
	//3 - Configura de acordo
	//4 - Roda e demodula	
	
	system_init();
	
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
			break;
	}
	NVIC_SystemReset(); //Se chegou aqui deu besteira
}
