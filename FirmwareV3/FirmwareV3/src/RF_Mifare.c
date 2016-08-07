#include <asf.h>

#include "Pinos.h"
#include "Helper.h"
#include "Output.h"
#include "RF_Mifare.h"
#include "RF_Mifare_Util.h"
#include "RC522_Mifare.h"
#include "RC522_ErrCode.h"

volatile uint8_t erro=0;
volatile uint8_t dado=0xff;

struct i2c_master_module i2c_master_instance;

void mifare_i2c_init(void)
{
	struct i2c_master_config config_i2c_master;
	i2c_master_get_config_defaults(&config_i2c_master);
	
	config_i2c_master.buffer_timeout = 65535;
	config_i2c_master.pinmux_pad0 = PINMUX_PA22C_SERCOM3_PAD0;
	config_i2c_master.pinmux_pad1 = PINMUX_PA23C_SERCOM3_PAD1;
	
	while (i2c_master_init(&i2c_master_instance, SERCOM3, &config_i2c_master) != STATUS_OK);
}

void mifare_reset(void)
{
	ioport_set_pin_level(PIN_MIFARE_RST,0);
	delay_ms(100);
	ioport_set_pin_level(PIN_MIFARE_RST,1);
	delay_ms(100);
}

void mifare_activate_card(void)
{
	unsigned char bSAK, baATQ[2], uid[8], uid_length=255 /*nunca vai ser*/;

	Rc522RFReset(5);

	if (ActivateCard(ISO14443_3_REQA, baATQ, uid, &uid_length, &bSAK) == STATUS_SUCCESS)
	{
		go_output(((uint32_t)uid[1] << 16) + ((uint32_t)uid[2] << 8) + (uint32_t)uid[3]);
	}
}

void Mifare_Init(void) 
{
	mifare_i2c_init();
	mifare_reset();
		
	Rc522Init();
}

void Mifare_Run(void)
{
	while (1)
	{
		mifare_activate_card();
		wdt_reset_count();
	}
}