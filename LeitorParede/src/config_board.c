/*
 * config_board.h
 *
 * Created: 02/07/2013 10:21:07
 *  Author: Neto
 */ 
#include "config_board.h"
#include <asf.h>

usart_serial_options_t USART_SERIAL_OPTIONS = {
	.baudrate = USART_SERIAL_BAUDRATE,
	.charlength = USART_SERIAL_LENGTH,
	.paritytype = USART_SERIAL_PARITY,
	.stopbits = USART_SERIAL_STOP_BIT
};

//Inicia o gerador de frequência de 125kHz para a excitação da antena.

void Liga_125kHz(void)
{
	tc45_enable(&TCC4);
	tc45_set_wgm(&TCC4, TC45_WGMODE_NORMAL_gc);
	tc45_write_period(&TCC4, 31);
	tc45_enable_cc_channels(&TCC4,TC45_CCDCOMP);
	tc45_write_clock_source(&TCC4, TC45_CLKSEL_DIV4_gc);
}

void Conf_Buzzer(void) // Precisa entender melhor como setar a frequencia do tom através do "period". 
{
	xcl_enable(XCL_SYNCHRONOUS);
	xcl_port(PD);
	xcl_tc_type(BTC01);
	xcl_tc_mode(PWM);
}

void Liga_Buzzer(uint16_t period)
{
	xcl_tc_source_clock(DIV256);
	xcl_tc16_set_period(0x0000);
	xcl_tc16_set_count(period);
	xcl_enable_oc1();
}

// Desliga o buzzer da placa

void Desliga_Buzzer(void)
{
	xcl_tc_source_clock(OFF);
}

// Inicializa a placa. Habilita clock de 32MHz, interrupções, delay via software, etc.

void Inicializa_Placa(void)
{
	sysclk_init();
	pmic_init();
	pmic_enable_level(PMIC_LVL_LOW | PMIC_LVL_MEDIUM | PMIC_LVL_HIGH);
	delay_init(sysclk_get_cpu_hz());
	irq_initialize_vectors();
	cpu_irq_enable();
}

// Inicializa os pinos de entrada e saída da placa

void Inicializa_GPIO(void)
{
	ioport_init();
	ioport_set_pin_dir(LED_GREEN,IOPORT_DIR_OUTPUT);
	ioport_set_pin_dir(RELAY,IOPORT_DIR_OUTPUT);
	ioport_set_pin_dir(RFID_CLK,IOPORT_DIR_OUTPUT);
	ioport_set_pin_dir(CARD_PRES,IOPORT_DIR_OUTPUT);
	ioport_set_pin_dir(D1_DATA,IOPORT_DIR_OUTPUT);
	ioport_set_pin_dir(BUZZER,IOPORT_DIR_OUTPUT);
	ioport_set_pin_dir(USART_TX_PIN, IOPORT_DIR_OUTPUT);
	
	ioport_set_pin_dir(LED_IN,IOPORT_DIR_INPUT);
}

// Inicializa o pino para recepção do sinal de ASK filtrado pelos op-amps. Sensibilidade na borda de subida e descida
// e habilitando interrupção

void ASK_Pin_Config(void)
{
	ioport_set_pin_dir(ASK_FSK,IOPORT_DIR_INPUT);
	ioport_set_pin_sense_mode(ASK_FSK,IOPORT_SENSE_BOTHEDGES);
	PORTD.INTCTRL = PMIC_MEDLVLEN_bm;
	PORTD.INTMASK |= (1<<INTMASK2);
}

// Inicializa o pino para recepção do sinal de FSK filtrado pelos op-amps. Sensibilidade na borda de descida
// e habilitando interrupção

void FSK_Pin_Config(void)
{
	ioport_set_pin_dir(ASK_FSK,IOPORT_DIR_INPUT);
	ioport_set_pin_sense_mode(ASK_FSK,IOPORT_SENSE_FALLING);
	PORTD.INTCTRL = PMIC_MEDLVLEN_bm;
	PORTD.INTMASK |= (1<<INTMASK2);
}

void PSK_Pin_Config(void)
{
	ioport_set_pin_dir(PSK_PIN,IOPORT_DIR_INPUT);
	ioport_set_pin_sense_mode(PSK_PIN,IOPORT_SENSE_RISING);
	PORTC.INTCTRL = PMIC_MEDLVLEN_bm;
	PORTC.INTMASK |= (1<<INTMASK6);
}

// Limpa a flag de interrupção do pino de ASK e FSK

void Clear_PORTD_Int_Flag(void)
{
	PORTD.INTFLAGS |= (1<<INT2IF);
}

void Clear_PORTC_Int_Flag(void)
{
	PORTC.INTFLAGS |= (1<<INT6IF);
}

void Conf_Timer_XCL_16bits(void)
{
	xcl_enable(XCL_SYNCHRONOUS);
	xcl_port(PD);
	xcl_tc_type(TC16);
	xcl_tc_mode(XCL_TCMODE_NORMAL_gc);
}

void Liga_Timer_XCL_16bits(void)
{
	xcl_tc_source_clock(DIV1);
}

void Desliga_Timer_XCL_16bits(void)
{
	xcl_tc_source_clock(OFF);
}

uint16_t  Cont_Timer_XCL_16bits(void)
{
	uint16_t temp;
	
	Desliga_Timer_XCL_16bits();
	temp = (XCL.CNTH << 8) + XCL.CNTL;
	Liga_Timer_XCL_16bits();
	
	return temp;
}

// Inicia o contador TC5 em modo normal e com período de contagem máximo

void Configura_TC5(void)
{
	tc45_enable(&TCC5);
	tc45_set_wgm(&TCC5,TC45_WG_NORMAL);
	tc45_write_period(&TCC5, 0xFFFF); 
}

// Liga o contador TC5 selecionando a sua fonte de clock

void Liga_TC5(void)
{
	tc45_write_clock_source(&TCC5,TC45_CLKSEL_DIV1_gc);
}

// Desliga a fonte de clock do TC5 parando a contagem 

void Desliga_TC5(void)
{
	tc45_write_clock_source(&TCC5,TC45_CLKSEL_OFF_gc);
}

// Define o modo do contador TC5 para contar para cima

void TC5_Set_CountUp_Mode(void)
{
	TCC5.CTRLGCLR |= (1<<0);
}

// Recebe o valor do registrador do contador TC5

uint16_t Ler_Contagem_TC5(void)
{
	uint16_t cont;
	
	Desliga_TC5();
	cont=tc45_read_count(&TCC5);
	Liga_TC5();

	return cont;
}

// Reinicia o registrador de contagem do TC5

inline void Reinicia_Contagem_TC5(void)
{
	TCC5.CTRLGSET = (1<<3);
}

// Inicializa a USART para transmissao de dados do cartao

void Inicializa_USART(void)
{
	PORTD.REMAP |= PORT_USART0_bm;
	ioport_configure_pin(USART_TX_PIN, IOPORT_DIR_OUTPUT);
	
	usart_serial_init(USART_SERIAL, &USART_SERIAL_OPTIONS);
	PORTD.PIN7CTRL |= (1 << 6);
}