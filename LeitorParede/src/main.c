#include <asf.h>
#include "config_board.h"
#include "ASK_Decode.h"
#include "FSK_Decode.h"
#include "PSK_Decode.h"

int main (void)
{
	Inicializa_Placa();
	
	Inicializa_GPIO();
	
	//Inicializa_USART();

	Liga_125kHz();

	Configura_TC5();
	
	TC5_Set_CountUp_Mode();

	Liga_TC5();
	
	//ASK_Pin_Config();
	//FSK_Pin_Config();
	PSK_Pin_Config();
	
	while(1)
	{
	}
}

//ISR(PORTD_INT_vect)
//{
	//ASK_Decoding();
	//FSK_Decoding();
	//
	//Clear_PORTD_Int_Flag();
//}

ISR(PORTC_INT_vect)
{
	//PSK_Decoding();
	PSK_Decoding_2();
	Clear_PORTC_Int_Flag();
}