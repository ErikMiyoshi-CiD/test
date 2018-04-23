#include "stm32f0xx_hal.h"

#include "helper.h"
#include "main.h"
#include "RF_common.h"
#include "SerialOut.h"

#include "gpio.h"
#include "iwdg.h"
#include "tim.h"

TIPO_OUTPUT tipo_output;
WIEGAND_SIZE wiegand_size;
TIPO_LEITOR tipo_leitor;

static uint8_t user_info_array[USER_INFO_SIZE];

static void ReadUserPage(void) {
	user_info_array[USER_INFO_POS_RFID] = *(uint16_t*) (USER_INFO_ADD + 4 * (USER_INFO_POS_RFID + 1));
	user_info_array[USER_INFO_POS_OUTP] = *(uint16_t*) (USER_INFO_ADD + 4 * (USER_INFO_POS_OUTP + 1));
	user_info_array[USER_INFO_POS_WIEGANDSIZE] = *(uint16_t*) (USER_INFO_ADD + 4 * (USER_INFO_POS_WIEGANDSIZE + 1));
	
	HAL_IWDG_Refresh(&hiwdg);
}

static void WriteUserPage(void) {
	FLASH_EraseInitTypeDef pEraseInit;
	uint32_t PageError;
	
	pEraseInit.TypeErase = FLASH_TYPEERASE_PAGES;
	pEraseInit.PageAddress = USER_INFO_ADD;
	pEraseInit.NbPages = 1;
	
	HAL_FLASH_Unlock();
	
	__HAL_FLASH_CLEAR_FLAG(FLASH_FLAG_EOP | FLASH_FLAG_PGERR | FLASH_FLAG_WRPERR);
	HAL_FLASHEx_Erase(&pEraseInit, &PageError);
	
	HAL_FLASH_Program(FLASH_TYPEPROGRAM_HALFWORD, USER_INFO_ADD + 4 * (USER_INFO_POS_RFID + 1), (uint16_t) user_info_array[USER_INFO_POS_RFID]);
	HAL_FLASH_Program(FLASH_TYPEPROGRAM_HALFWORD, USER_INFO_ADD + 4 * (USER_INFO_POS_OUTP + 1), (uint16_t) user_info_array[USER_INFO_POS_OUTP]);
	HAL_FLASH_Program(FLASH_TYPEPROGRAM_HALFWORD, USER_INFO_ADD + 4 * (USER_INFO_POS_WIEGANDSIZE + 1), (uint16_t) user_info_array[USER_INFO_POS_WIEGANDSIZE]);
	
	HAL_FLASH_Lock();
	
	HAL_IWDG_Refresh(&hiwdg);
}

static void WriteOUTP(uint8_t outp) {
	ReadUserPage();
	user_info_array[USER_INFO_POS_OUTP] = outp;
	WriteUserPage();
}

static void WriteWIEGANDSIZE(uint8_t wsize) {
	ReadUserPage();
	user_info_array[USER_INFO_POS_WIEGANDSIZE] = wsize;
	WriteUserPage();
}

// Alterado para ler pinos B0..2 do módulo RFID
// Não faz mais sentido ter WriteRFID agora...
static uint8_t ReadRFID(void) {
	uint8_t rfidType = 0;
	
	if (HAL_GPIO_ReadPin(B0_GPIO_Port, B0_Pin) == GPIO_PIN_SET)
		rfidType |= (1<<0);
	if (HAL_GPIO_ReadPin(B1_GPIO_Port, B1_Pin) == GPIO_PIN_SET)
		rfidType |= (1<<1);
	if (HAL_GPIO_ReadPin(B2_GPIO_Port, B2_Pin) == GPIO_PIN_SET)
		rfidType |= (1<<2);
	
	if (rfidType == USER_INFO_MIF_TYPE) {
		return USER_INFO_MIF_RFID;
	} 
	
	if (rfidType == USER_INFO_ASK_TYPE) {
		return USER_INFO_ASK_RFID;
	} 
	
	return USER_INFO_NUL_RFID;
}

static uint8_t ReadOUTP(void) {
	ReadUserPage();
	return user_info_array[USER_INFO_POS_OUTP];
}

static uint8_t ReadWIEGANDSIZE(void) {
	ReadUserPage();
	
	//Trata o caso de nunca ter sido inicializado
	if (user_info_array[USER_INFO_POS_WIEGANDSIZE] == WIEGAND_66)
		return WIEGAND_66;
	else if (user_info_array[USER_INFO_POS_WIEGANDSIZE] == WIEGAND_34)
		return WIEGAND_34;
	else
		return WIEGAND_26;
}

void Init125khz(void){
	MX_TIM16_Init();
	__HAL_TIM_SET_COMPARE(&htim16,TIM_CHANNEL_1, 192);
	
	HAL_TIM_PWM_Start(&htim16, TIM_CHANNEL_1);
}

void buzzer_clock_init(void)
{
	MX_TIM3_Init();
	__HAL_TIM_SET_COMPARE(&htim3,TIM_CHANNEL_2, 6000);
}

void buzz(uint32_t tempo){
	HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_2);
	HAL_Delay(tempo);
	HAL_TIM_PWM_Stop(&htim3, TIM_CHANNEL_2);
}

void buzz_on(void)
{
	HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_2);
}

void buzz_off(void)
{
	HAL_TIM_PWM_Stop(&htim3, TIM_CHANNEL_2);
}

static void pin_configure(void)
{
	MX_GPIO_Init();
}


void ok_feedback(void){
	HAL_GPIO_WritePin(LED_RED_GPIO_Port, LED_RED_Pin, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(LED_GRN_GPIO_Port, LED_GRN_Pin, GPIO_PIN_SET);
	buzz(300);
	HAL_GPIO_WritePin(LED_RED_GPIO_Port, LED_RED_Pin, GPIO_PIN_SET);
	HAL_GPIO_WritePin(LED_GRN_GPIO_Port, LED_GRN_Pin, GPIO_PIN_RESET);
}

void led_idle(void){
	led_red();
}

void led_yellow(void){
	HAL_GPIO_WritePin(LED_RED_GPIO_Port, LED_RED_Pin, GPIO_PIN_SET);
	HAL_GPIO_WritePin(LED_GRN_GPIO_Port, LED_GRN_Pin, GPIO_PIN_SET);
}

void led_green(void){
	HAL_GPIO_WritePin(LED_RED_GPIO_Port, LED_RED_Pin, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(LED_GRN_GPIO_Port, LED_GRN_Pin, GPIO_PIN_SET);
}

void led_red(void){
	HAL_GPIO_WritePin(LED_RED_GPIO_Port, LED_RED_Pin, GPIO_PIN_SET);
	HAL_GPIO_WritePin(LED_GRN_GPIO_Port, LED_GRN_Pin, GPIO_PIN_RESET);
}

void led_off(void){
	HAL_GPIO_WritePin(LED_RED_GPIO_Port, LED_RED_Pin, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(LED_GRN_GPIO_Port, LED_GRN_Pin, GPIO_PIN_RESET);
}

static void configure_wdt(void)
{
    MX_IWDG_Init();
}

static void nvm_init(void)
{
	ReadUserPage();
	if (user_info_array[USER_INFO_POS_OUTP] == 0xff &&
			user_info_array[USER_INFO_POS_WIEGANDSIZE] == 0xff)
	{
		DEBUG_PUTSTRING("Memoria zerada!\r\n");
		WriteOUTP(USER_INFO_WIE_OUTP);
		WriteWIEGANDSIZE(USER_INFO_WIEGAND26);
	}
	
	//Se nunca setamos o tamanho da Wiegand, setaremos agora
	if (user_info_array[USER_INFO_POS_WIEGANDSIZE] != WIEGAND_26 &&
			user_info_array[USER_INFO_POS_WIEGANDSIZE] != WIEGAND_34 && 
			user_info_array[USER_INFO_POS_WIEGANDSIZE] != WIEGAND_66)
	{
		if (ReadRFID() == USER_INFO_MIF_RFID)
			WriteWIEGANDSIZE(WIEGAND_34);
		else
			WriteWIEGANDSIZE(WIEGAND_26);
	}
}

void user_init(void){
	//Inicializa WDT
	configure_wdt();
	HAL_IWDG_Refresh(&hiwdg);
	
	//Configura USART
	configure_usart();
	HAL_IWDG_Refresh(&hiwdg);
	
	//Configure pinos
	pin_configure();
	HAL_IWDG_Refresh(&hiwdg);
	
	//Inicializa buzzer
	buzzer_clock_init();
	HAL_IWDG_Refresh(&hiwdg);
	
	//Inicializa NVM
	nvm_init();
	HAL_IWDG_Refresh(&hiwdg);
}

MODO_LEITOR avaliar_modo_leitor(void)
{
	const uint8_t nretries = 8;
	int i, val;
	
	HAL_IWDG_Refresh(&hiwdg);
	
	//Testa Mode com D0 = W26
	for (i = 0; i < nretries; i++)
	{
		val = i % 2;
		HAL_GPIO_WritePin(D0_TX_CLK_GPIO_Port, D0_TX_CLK_Pin, val);
		HAL_Delay(20);
			
		if (HAL_GPIO_ReadPin(MS_BUZZ_GPIO_Port, MS_BUZZ_Pin) == val) //tem inversor 
			break;
	}
	if (i == nretries)
		return MODO_PROGRAMA_W26;
				
	//Testa Mode com D1 = W34
	for (i = 0; i < nretries; i++)
	{
		val = i % 2;
		HAL_GPIO_WritePin(D1_DATA_GPIO_Port, D1_DATA_Pin, val);
		HAL_Delay(20);
			
		if (HAL_GPIO_ReadPin(MS_BUZZ_GPIO_Port, MS_BUZZ_Pin) == val) //tem inversor
			break;
	}
	if (i == nretries)
		return MODO_PROGRAMA_W34;
		
	//Testa Mode com CardPres = ABA
	for (i = 0; i < nretries; i++)
	{
		val = i % 2;
		HAL_GPIO_WritePin(CARD_PRES_GPIO_Port, CARD_PRES_Pin, val);
		HAL_Delay(20);
			
		if (HAL_GPIO_ReadPin(MS_BUZZ_GPIO_Port, MS_BUZZ_Pin) == val) //tem inversor
			break;
	}
	if (i == nretries)
		return MODO_PROGRAMA_ABA;
		
	//Testa LEDIN com D0 = W66
	for (i = 0; i < nretries; i++)
	{
		val = i % 2;
		HAL_GPIO_WritePin(D0_TX_CLK_GPIO_Port, D0_TX_CLK_Pin, val);
		HAL_Delay(20);
		
		if (HAL_GPIO_ReadPin(LED_INPUT_GPIO_Port, LED_INPUT_Pin) == val) //tem inversor
			break;
	}
	if (i == nretries)
		return MODO_PROGRAMA_W66;	
	
	//Testa LEDIN com D1 = RS232
	for (i = 0; i < nretries; i++)
	{
		val = i % 2;
		HAL_GPIO_WritePin(D1_DATA_GPIO_Port, D1_DATA_Pin, val);
		HAL_Delay(20);
		
		if (HAL_GPIO_ReadPin(LED_INPUT_GPIO_Port, LED_INPUT_Pin) == val) //tem inversor
			break;
	}
	if (i == nretries)
		return MODO_PROGRAMA_RS232;	
	
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
				HAL_IWDG_Refresh(&hiwdg);
		break;
		
		case MODO_PROGRAMA_W34:
			WriteOUTP(USER_INFO_WIE_OUTP);
			WriteWIEGANDSIZE(WIEGAND_34);
			led_yellow();
			buzz_on();
			while(1)
				HAL_IWDG_Refresh(&hiwdg);
		break;	
			
		case MODO_PROGRAMA_W66:
			WriteOUTP(USER_INFO_WIE_OUTP);
			WriteWIEGANDSIZE(WIEGAND_66);
			led_off();
			buzz_on();
			while(1)
				HAL_IWDG_Refresh(&hiwdg);
		break;
			
		case MODO_PROGRAMA_ABA:
			WriteOUTP(USER_INFO_ABA_OUTP);
			WriteWIEGANDSIZE(WIEGAND_26); //ignorado
			led_red();
			buzz_on();
			while(1)
				HAL_IWDG_Refresh(&hiwdg);
		break;
			
		case MODO_PROGRAMA_RS232:
			WriteOUTP(USER_INFO_RS232_OUTP);
			WriteWIEGANDSIZE(WIEGAND_26); //ignorado
			led_red();
			buzz_on();
			while(1) {
				HAL_GPIO_TogglePin(LED_RED_GPIO_Port, LED_RED_Pin);
				
				HAL_Delay(200);
								
				HAL_IWDG_Refresh(&hiwdg);
			}
		break;
			
		case MODO_NORMAL:
			//Lê tamanho Wiegand
			if (USER_INFO_WIEGAND34 == ReadWIEGANDSIZE()) 
				wiegand_size = WIEGAND_34;
			else if (USER_INFO_WIEGAND66 == ReadWIEGANDSIZE())
				wiegand_size = WIEGAND_66;
			else
				wiegand_size = WIEGAND_26;
				
			//Lê tipo saída
			if (USER_INFO_ABA_OUTP == ReadOUTP()) {
				tipo_output = OUTPUT_ABATRACK;
				HAL_GPIO_WritePin(D0_TX_CLK_GPIO_Port, D0_TX_CLK_Pin, GPIO_PIN_RESET); //inverted logic
				HAL_GPIO_WritePin(D1_DATA_GPIO_Port, D1_DATA_Pin, GPIO_PIN_RESET); //inverted logic
				HAL_GPIO_WritePin(CARD_PRES_GPIO_Port, CARD_PRES_Pin, GPIO_PIN_RESET); //inverted logic
			} else if (USER_INFO_WIE_OUTP == ReadOUTP()) {
				tipo_output = OUTPUT_WIEGAND;
				HAL_GPIO_WritePin(D0_TX_CLK_GPIO_Port, D0_TX_CLK_Pin, GPIO_PIN_RESET); //inverted logic
				HAL_GPIO_WritePin(D1_DATA_GPIO_Port, D1_DATA_Pin, GPIO_PIN_RESET); //inverted logic
				HAL_GPIO_WritePin(CARD_PRES_GPIO_Port, CARD_PRES_Pin, GPIO_PIN_RESET); //inverted logic
			} else {
				tipo_output = OUTPUT_RS232;
				HAL_GPIO_WritePin(D0_TX_CLK_GPIO_Port, D0_TX_CLK_Pin, GPIO_PIN_SET); //inverted logic
				HAL_GPIO_WritePin(D1_DATA_GPIO_Port, D1_DATA_Pin, GPIO_PIN_RESET); //inverted logic
				HAL_GPIO_WritePin(CARD_PRES_GPIO_Port, CARD_PRES_Pin, GPIO_PIN_RESET); //inverted logic
			}
			
			//Pisca LEDs e termina inicialização
			led_red();
			HAL_Delay(300);
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
		tipo_leitor = TIPO_ASK;
		return TIPO_ASK;
	
	case USER_INFO_FSK_RFID:
		tipo_leitor = TIPO_FSK;
		return TIPO_FSK;
	
	case USER_INFO_PSK_RFID:
		tipo_leitor = TIPO_PSK;
		return TIPO_PSK;
	
	case USER_INFO_MIF_RFID:
		tipo_leitor = TIPO_MIFARE;
		return TIPO_MIFARE;
	
	case USER_INFO_NUL_RFID:
		tipo_leitor = TIPO_NUL;
		return TIPO_NUL;
	
	default:
		return TIPO_NUL;
	}
}
