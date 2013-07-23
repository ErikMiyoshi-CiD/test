#include <asf.h>
#include "config_board.h"
#include "ASK_Decode.h"
#include "FSK_Decode.h"
#include "PSK_Decode.h"

int main (void)
{
	Inicializa_Placa();
	
	Inicializa_GPIO();
	
	Inicializa_USART();

	Liga_125kHz();

	Configura_TC5();
	
	TC5_Set_CountUp_Mode();

	Liga_TC5();
	
	#ifdef ASK
		ASK_Pin_Config();
	#endif
	
	#ifdef FSK
		FSK_Pin_Config();
	#endif
	
	#ifdef PSK
	PSK_Pin_Config();
	#endif
	
	while(1)
	{
	}
}

#ifndef	PSK
	ISR(PORTD_INT_vect)
	{
		#ifdef ASK
			ASK_Decoding();
		#endif
		
		#ifdef FSK
			FSK_Decoding();
		#endif
		
		Clear_PORTD_Int_Flag();
	}
#endif

#ifdef PSK
	ISR(PORTC_INT_vect)
	{
		ioport_toggle_pin(CARD_PRES);
		PSK_Decoding();
		ioport_toggle_pin(CARD_PRES);
		Clear_PORTC_Int_Flag();
	}
#endif
