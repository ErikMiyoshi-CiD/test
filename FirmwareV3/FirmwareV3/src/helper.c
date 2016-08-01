#include <asf.h>

#include "helper.h"
#include "pinos.h"
#include "delay.h"
#include "RF_common.h"
#include "ioport.h"
#include "SerialOut.h"


#undef ENABLE

TIPO_OUTPUT tipo_output;

struct tc_module tc1_module;
struct tc_module tc5_module;
static uint8_t user_data_page[NVMCTRL_PAGE_SIZE];

//Esta função inicializa o TC1 WO[1] --> Pino do 125kHz da antena
void Init125khz(void){
	struct tc_config tc1_config;
	
	tc_reset(&tc1_module);
	tc_get_config_defaults(&tc1_config);
	tc1_config.wave_generation = TC_WAVE_GENERATION_MATCH_FREQ;
	tc1_config.counter_size = TC_COUNTER_SIZE_16BIT;
	tc1_config.counter_16_bit.compare_capture_channel[0] = 384/2 - 1;
	tc1_config.pwm_channel[0].enabled = true;
	tc1_config.pwm_channel[0].pin_out = PIN_PA07F_TC1_WO1;
	tc1_config.pwm_channel[0].pin_mux = MUX_PA07F_TC1_WO1;
	tc_init(&tc1_module, TC1, &tc1_config);
	tc_enable(&tc1_module);
}

void buzzer_clock_init(void)
{
	struct tc_config tc5_config;
	
	tc_reset(&tc5_module);
	tc_get_config_defaults(&tc5_config);
	tc5_config.wave_generation = TC_WAVE_GENERATION_MATCH_FREQ;
	tc5_config.counter_size = TC_COUNTER_SIZE_16BIT;
	tc5_config.counter_16_bit.compare_capture_channel[0] = 12000 / 2 - 1;
	tc5_config.pwm_channel[1].enabled = true;
	tc5_config.pwm_channel[1].pin_out = PIN_PA25F_TC5_WO1;
	tc5_config.pwm_channel[1].pin_mux = MUX_PA25F_TC5_WO1;
	tc_init(&tc5_module, TC5, &tc5_config);
}

void buzz(uint32_t tempo){
	tc_enable(&tc5_module);
	delay_ms(tempo);
	tc_disable(&tc5_module);
}

void buzz_on(void)
{
	tc_enable(&tc5_module);
}

void buzz_off(void)
{
	tc_disable(&tc5_module);
}

static void pin_configure(void)
{
	ioport_init();
	
	ioport_set_pin_dir(PIN_ASK_IN, IOPORT_DIR_INPUT);
	ioport_set_pin_dir(PIN_FSK_PSK_IN, IOPORT_DIR_INPUT);
	
	ioport_set_pin_dir(PIN_LED_GRN, IOPORT_DIR_OUTPUT);
	ioport_set_pin_dir(PIN_LED_RED, IOPORT_DIR_OUTPUT);
	
	ioport_set_pin_dir(PIN_FSK_PSK_AC_IN, IOPORT_DIR_INPUT);
	ioport_set_pin_dir(PIN_ASK_AC_IN, IOPORT_DIR_INPUT);
	ioport_set_pin_dir(PIN_125KHZ, IOPORT_DIR_OUTPUT);
	ioport_set_pin_dir(PIN_D0_TX_CLK, IOPORT_DIR_OUTPUT);
	ioport_set_pin_dir(PIN_D1_DATA, IOPORT_DIR_OUTPUT);
	ioport_set_pin_dir(PIN_CARD_PRES, IOPORT_DIR_OUTPUT);
	ioport_set_pin_dir(PIN_LED_INPUT, IOPORT_DIR_INPUT);
	ioport_set_pin_dir(PIN_MIFARE_RST, IOPORT_DIR_OUTPUT);
	ioport_set_pin_dir(PIN_MS_BUZZ, IOPORT_DIR_INPUT);
	ioport_set_pin_dir(PIN_MIFARE_SDA, IOPORT_DIR_INPUT);
	ioport_set_pin_dir(PIN_MIFARE_SCL, IOPORT_DIR_OUTPUT);
	ioport_set_pin_dir(PIN_BUZZ, IOPORT_DIR_OUTPUT);
}


void ok_feedback(void){
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

static void configure_wdt(void)
{
    /* Create a new configuration structure for the Watchdog settings and fill
     * with the default module settings. */
    struct wdt_conf config_wdt;
    wdt_get_config_defaults(&config_wdt);
    /* Set the Watchdog configuration settings */
    config_wdt.always_on      = false;
    config_wdt.clock_source   = GCLK_GENERATOR_1;
    config_wdt.timeout_period = WDT_PERIOD_16384CLK;
    /* Initialize and enable the Watchdog with the user settings */
    wdt_set_config(&config_wdt);
}

static void nvm_init(void)
{
	struct nvm_config config_nvm;
	nvm_get_config_defaults(&config_nvm);
	nvm_set_config(&config_nvm);
}

void user_init(void){
	//Inicializa WDT
	configure_wdt();
	wdt_reset_count();
	
	//Inicializa delays
	delay_init();
	wdt_reset_count();
	
	//Configura USART
	configure_usart();
	wdt_reset_count();
	
	//Configure pinos
	pin_configure();
	wdt_reset_count();
	
	//Inicializa buzzer
	buzzer_clock_init();
	wdt_reset_count();
	
	//Inicializa NVM
	nvm_init();
	
	wdt_reset_count();
}

MODO_LEITOR avaliar_modo_leitor(void)
{
	//Se estivermos em modo programação, o pino 
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
#warning XXX
	//uint16_t temp=0;
	uint16_t temp=*(volatile uint16_t *)USER_INFO_ADD;	
	temp=0;

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
				temp |= ((uint16_t)'W') << 8;
				led_green();	
			}
			else
			{
				tipo_output=OUTPUT_ABATRACK;
				temp |= ((uint16_t)'A') << 8;
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
#warning SSS
	//uint8_t tipo=(*(volatile uint16_t *)USER_INFO_ADD & 0xFF);
	uint8_t tipo='A';
	
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