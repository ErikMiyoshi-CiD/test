#include <asf.h>

#include "helper.h"
#include "pinos.h"
#include "delay.h"
#include "RF_common.h"
#include "ioport.h"
#include "SerialOut.h"

TIPO_OUTPUT tipo_output;
WIEGAND_SIZE wiegand_size;

struct tc_module tc1_module;
struct tc_module tc5_module;
static uint8_t user_data_page[NVMCTRL_PAGE_SIZE];

static void ReadUserPage(void) {
	enum status_code error_code;
	
	do 
	{
		error_code = nvm_read_buffer(USER_INFO_ADD,user_data_page,NVMCTRL_PAGE_SIZE);
	} while (error_code == STATUS_BUSY);	
	wdt_reset_count();
}

static void WriteUserPage(void) {	
	enum status_code error_code;
	
	do
	{
		error_code = nvm_erase_row(USER_INFO_ADD);
	} while (error_code == STATUS_BUSY);
	wdt_reset_count();	
	
	do
	{
		error_code = nvm_write_buffer(USER_INFO_ADD,user_data_page,NVMCTRL_PAGE_SIZE);
	} while (error_code == STATUS_BUSY);
	wdt_reset_count();
}

static void WriteOUTP(uint8_t outp) {
	ReadUserPage();
	user_data_page[USER_INFO_POS_OUTP]=outp;
	WriteUserPage();
}

static void WriteWIEGANDSIZE(uint8_t wsize) {
	ReadUserPage();
	user_data_page[USER_INFO_POS_WIEGANDSIZE]=wsize;
	WriteUserPage();
}

static void WriteRFID(uint8_t rfid) {
	ReadUserPage();
	user_data_page[USER_INFO_POS_RFID]=rfid;
	WriteUserPage();
}

static uint8_t ReadRFID(void) {
	ReadUserPage();
	return user_data_page[USER_INFO_POS_RFID];
}

static uint8_t ReadOUTP(void) {
	ReadUserPage();
	return user_data_page[USER_INFO_POS_OUTP];
}

static uint8_t ReadWIEGANDSIZE(void) {
	ReadUserPage();
	
	//Trata o caso de nunca ter sido inicializado
	if (user_data_page[USER_INFO_POS_WIEGANDSIZE] == WIEGAND_66)
		return WIEGAND_66;
	else if (user_data_page[USER_INFO_POS_WIEGANDSIZE] == WIEGAND_34)
		return WIEGAND_34;
	else
		return WIEGAND_26;
}

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
	
	ioport_set_pin_dir(PIN_ASK_IN,        IOPORT_DIR_INPUT);
	ioport_set_pin_mode(PIN_ASK_IN,0);
	ioport_set_pin_dir(PIN_FSK_PSK_IN,    IOPORT_DIR_INPUT);
	ioport_set_pin_mode(PIN_FSK_PSK_IN,0);
	
	ioport_set_pin_dir(PIN_LED_GRN,       IOPORT_DIR_OUTPUT);
	ioport_set_pin_dir(PIN_LED_RED,       IOPORT_DIR_OUTPUT);
	
	ioport_set_pin_dir(PIN_FSK_PSK_AC_IN, IOPORT_DIR_INPUT);
	ioport_set_pin_mode(PIN_FSK_PSK_AC_IN,0);
	ioport_set_pin_dir(PIN_ASK_AC_IN,     IOPORT_DIR_INPUT);
	ioport_set_pin_mode(PIN_ASK_AC_IN,0);
	
	ioport_set_pin_dir(PIN_125KHZ,        IOPORT_DIR_OUTPUT);
	ioport_set_pin_dir(PIN_D0_TX_CLK,     IOPORT_DIR_OUTPUT);
	ioport_set_pin_dir(PIN_D1_DATA,       IOPORT_DIR_OUTPUT);
	ioport_set_pin_dir(PIN_CARD_PRES,     IOPORT_DIR_OUTPUT);
	
	//Led inpurt sem pull-up or down
	ioport_set_pin_dir(PIN_LED_INPUT, IOPORT_DIR_INPUT);
	ioport_set_pin_mode(PIN_LED_INPUT,0);
	
	//Mode select sem pull-up or down
	ioport_set_pin_dir(PIN_MS_BUZZ, IOPORT_DIR_INPUT);
	ioport_set_pin_mode(PIN_MS_BUZZ,0);
	
	//Mifare
	ioport_set_pin_dir(PIN_MIFARE_RST, IOPORT_DIR_OUTPUT);
	ioport_set_pin_dir(PIN_MIFARE_SCL, IOPORT_DIR_OUTPUT);
	ioport_set_pin_dir(PIN_MIFARE_SDA, IOPORT_DIR_INPUT);
	ioport_set_pin_mode(PIN_MIFARE_SDA,0);
	
	//Buzzer será reconfigurado na inicialização
	ioport_set_pin_dir(PIN_BUZZ, IOPORT_DIR_INPUT);
	ioport_set_pin_mode(PIN_BUZZ,0);
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
    config_wdt.clock_source   = GCLK_GENERATOR_2; //RTC
    config_wdt.timeout_period = WDT_PERIOD_8192CLK; //8s = reset
    /* Initialize and enable the Watchdog with the user settings */
    wdt_set_config(&config_wdt);
}

static void nvm_init(void)
{
	struct nvm_config config_nvm;
	
	nvm_get_config_defaults(&config_nvm);
	config_nvm.manual_page_write = false;
	nvm_set_config(&config_nvm);
	
	ReadUserPage();
	if (user_data_page[USER_INFO_POS_OUTP] == 0xff &&
		user_data_page[USER_INFO_POS_RFID] == 0xff &&
		user_data_page[USER_INFO_POS_WIEGANDSIZE] == 0xff)
	{
		DEBUG_PUTSTRING("Memoria zerada!\r\n");
		WriteOUTP(USER_INFO_WIE_OUTP);
		WriteRFID(USER_INFO_ASK_RFID);
		WriteWIEGANDSIZE(USER_INFO_WIEGAND26);
	}
	
	//Se nunca setamos o tamanho da Wiegand, setaremos agora
	if (user_data_page[USER_INFO_POS_WIEGANDSIZE] != WIEGAND_26 &&
		user_data_page[USER_INFO_POS_WIEGANDSIZE] != WIEGAND_34 && 
		user_data_page[USER_INFO_POS_WIEGANDSIZE] != WIEGAND_66)
	{
		if (ReadRFID()==USER_INFO_MIF_RFID)
			WriteWIEGANDSIZE(WIEGAND_34);
		else
			WriteWIEGANDSIZE(WIEGAND_26);
	}
}

void user_init(void){
	//Inicializa delays
	delay_init();
	
	//Inicializa WDT
	configure_wdt();
	wdt_reset_count();
	
#if SERIAL_DEBUG
	//Configura USART
	configure_usart();
	wdt_reset_count();
#endif
	
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
	const uint8_t nretries = 8;
	int i, val;
	
	wdt_reset_count();
	
	//Testa Mode com D0 = W26
	for (i=0;i<nretries;i++)
	{
		val=i % 2;
		ioport_set_pin_level(PIN_D0_TX_CLK,val);
		delay_ms(20);
			
		if (ioport_get_pin_level(PIN_MS_BUZZ) == val) //tem inversor 
			break;
	}
	if (i==nretries)
		return MODO_PROGRAMA_W26;
				
	//Testa Mode com D1 = W34
	for (i=0;i<nretries;i++)
	{
		val=i % 2;
		ioport_set_pin_level(PIN_D1_DATA,val);
		delay_ms(20);
			
		if (ioport_get_pin_level(PIN_MS_BUZZ) == val) //tem inversor
			break;
	}
	if (i==nretries)
		return MODO_PROGRAMA_W34;
		
	//Testa Mode com CardPres = ABA
	for (i=0;i<nretries;i++)
	{
		val=i % 2;
		ioport_set_pin_level(PIN_CARD_PRES,val);
		delay_ms(20);
			
		if (ioport_get_pin_level(PIN_MS_BUZZ) == val) //tem inversor
			break;
	}
	if (i==nretries)
		return MODO_PROGRAMA_ABA;
		
	//Testa LEDIN com D0 = W66
	for (i=0;i<nretries;i++)
	{
		val=i % 2;
		ioport_set_pin_level(PIN_D0_TX_CLK,val);
		delay_ms(20);
		
		if (ioport_get_pin_level(PIN_LED_INPUT) == val) //tem inversor
			break;
	}
	if (i==nretries)
		return MODO_PROGRAMA_W66;	
	
	return MODO_NORMAL;
}

void modo_leitor(void)
{
	switch(avaliar_modo_leitor())
	{
		case MODO_PROGRAMA_W26:
			WriteOUTP(USER_INFO_WIE_OUTP);
			WriteWIEGANDSIZE(WIEGAND_26);
			led_green();
			buzz_on();
			while(1)
				wdt_reset_count();
			break;
		case MODO_PROGRAMA_W34:
			WriteOUTP(USER_INFO_WIE_OUTP);
			WriteWIEGANDSIZE(WIEGAND_34);
			led_yellow();
			buzz_on();
			while(1)
				wdt_reset_count();
			break;	
		case MODO_PROGRAMA_W66:
			WriteOUTP(USER_INFO_WIE_OUTP);
			WriteWIEGANDSIZE(WIEGAND_66);
			led_off();
			buzz_on();
			while(1)
				wdt_reset_count();
			break;
		case MODO_PROGRAMA_ABA:
			WriteOUTP(USER_INFO_ABA_OUTP);
			WriteWIEGANDSIZE(WIEGAND_26); //ignorado
			led_red();
			buzz_on();
			while(1)
				wdt_reset_count();
			break;
		case MODO_NORMAL:
			//Lê tamanho Wiegand
			if (USER_INFO_WIEGAND34 == ReadWIEGANDSIZE()) 
				wiegand_size=WIEGAND_34;
			else if (USER_INFO_WIEGAND66 == ReadWIEGANDSIZE())
				wiegand_size=WIEGAND_66;
			else
				wiegand_size=WIEGAND_26;
				
			//Lê tipo saída
			if (USER_INFO_ABA_OUTP == ReadOUTP()) {
				tipo_output=OUTPUT_ABATRACK;
				ioport_set_pin_level(PIN_D0_TX_CLK,0); //inverted logic
				ioport_set_pin_level(PIN_D1_DATA,0); //inverted logic
				ioport_set_pin_level(PIN_CARD_PRES,0); //inverted logic
			}
			else {
				tipo_output=OUTPUT_WIEGAND;
				ioport_set_pin_level(PIN_D0_TX_CLK,0);//inverted logic
				ioport_set_pin_level(PIN_D1_DATA,0);//inverted logic
				ioport_set_pin_level(PIN_CARD_PRES,0); //inverted logic
			}
			
			//Pisca LEDs e termina inicialização
			led_red();
			delay_ms(300);
			led_green();
			buzz(250);
			led_idle();
			break;
	}
}

TIPO_LEITOR ler_tipo_leitor(void)
{
	switch (ReadRFID())
	{
	case USER_INFO_ASK_RFID:
		return TIPO_ASK;
	case USER_INFO_FSK_RFID:
		return TIPO_FSK;
	case USER_INFO_PSK_RFID:
		return TIPO_PSK;
	case USER_INFO_MIF_RFID:
		return TIPO_MIFARE;
	default:
		return TIPO_ASK;
	}
}