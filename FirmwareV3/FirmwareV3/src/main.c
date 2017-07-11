#include <asf.h>
#include "helper.h"
#include "pinos.h"
#include "RF_common.h"
#include "RF_ASK.h"
#include "RF_Mifare.h"
#include "SerialOut.h"
#include "Output.h"

/*
 ATENÇÃO!!! O FIRMWARE NÃO PODE PASSAR DE 16KB para não entrar em 0x3F00 (USER_PAGE)!!!
 */

int main(void){		
	//1 - Inicializa o sistema e os clocks
	//2 - Verifica se estamos em modo de programação ou não
	//3 - Lê o modo atual de funcionamento na EEPROM (ASK/FSK/PSK/Mifare?)
	//4 - Configura de acordo
	//5 - Roda e demodula	
	
	system_init();
	
	user_init();

	wdt_reset_count();

	modo_leitor();
	
	switch(ler_tipo_leitor())
	{
		case TIPO_ASK:
			ASK_Init();
			wdt_reset_count();
			ASK_Run();
			break;
		case TIPO_FSK:
			break;
		case TIPO_PSK:
			break;
		case TIPO_MIFARE:
			Mifare_Init();
			wdt_reset_count();
			Mifare_Run();
		break;
	}
	
	NVIC_SystemReset(); //Se chegou aqui deu besteira*/
}
