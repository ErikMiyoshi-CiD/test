#include <asf.h>
#include "config_board.h"
#include "ASK_Decode.h"

int main (void)
{
	Inicializa_Placa();
	
	Inicializa_GPIO();
	
	Inicializa_USART();

	Liga_125kHz();

	Configura_TC5();
	
	TC5_Set_CountUp_Mode();

	Liga_TC5();
	
	ASK_Pin_Config();
	
	while(1)
	{
	}
}

ISR(PORTD_INT_vect)
{
	ASK_Decoding();
	Clear_PORTD_Int_Flag();
}