/*
 * RF_Mifare.c
 *
 * Created: 2014-02-26 12:53:22 PM
 *  Author: Albert
 */ 
#include "sam.h"
#include "pinos.h"
#include "helper.h"
#include "delay.h"
#include "RF_Mifare.h"
#include "RF_Mifare_Util.h"
#include "RC522_Mifare.h"
#include "RC522_ErrCode.h"

uint8_t mifare_uid_length=255 /*nunca vai ser*/;

int mifare_i2c_master_address_response(void)
{
	/* Check for error and ignore bus-error; workaround for BUSSTATE stuck in
	 * BUSY */
	if (SERCOM3->I2CM.INTFLAG.reg & SERCOM_I2CM_INTFLAG_SB) {

		/* Clear write interrupt flag */
		SERCOM3->I2CM.INTFLAG.reg = SERCOM_I2CM_INTFLAG_SB;

		/* Check arbitration. */
		if (SERCOM3->I2CM.STATUS.reg & SERCOM_I2CM_STATUS_ARBLOST) {
			/* Return packet collision. */
			return -2;
		}
	/* Check that slave responded with ack. */
	} else if (SERCOM3->I2CM.STATUS.reg & SERCOM_I2CM_STATUS_RXNACK) {
		/* Slave busy. Issue ack and stop command. */
		SERCOM3->I2CM.CTRLB.reg |= SERCOM_I2CM_CTRLB_CMD(3);
		/* Return bad address value. */
		return -1;
	}
	return 0;
}

void mifare_i2c_wait_for_bus(void)
{
	uint16_t timeout_counter = 0;
	/* Wait for reply. */
	while (!(SERCOM3->I2CM.INTFLAG.reg & SERCOM_I2CM_INTFLAG_MB) &&
			!(SERCOM3->I2CM.INTFLAG.reg & SERCOM_I2CM_INTFLAG_SB)) {

		/* Check timeout condition. */
		if (++timeout_counter >= I2C_TIMEOUT) {
			return;
		}
	}
	return;
}

volatile uint8_t erro=0;
volatile uint8_t dado=0xff;

void mifare_i2c_init(void)
{
	//Ligar clock do sercom3
	PM->APBCMASK.reg |= PM_APBCMASK_SERCOM3;
	//PIN_MIFARE_SCL=23;
	PORT->Group[0].PINCFG[23].reg = 0x3; //output with readback
	PORT->Group[0].PMUX[11].bit.PMUXO = 0x2; //Periférico C
	//PIN_MIFARE_SDA=22;
	PORT->Group[0].PINCFG[22].reg = 0x3; //output with readback
	PORT->Group[0].PMUX[11].bit.PMUXE = 0x2; //Periférico C
	//Configura o módulo
	SERCOM3->I2CM.CTRLA.reg = SERCOM_I2CM_CTRLA_MODE_I2C_MASTER | (1 << SERCOM_I2CM_CTRLA_INACTOUT_Pos);	
	//Baud divide
	SERCOM3->I2CM.BAUD.reg=5; //400kHz
	//Habilita o módulo
	SERCOM3->I2CM.CTRLA.reg |= SERCOM_I2CM_CTRLA_ENABLE;
	while (SERCOM3->I2CM.STATUS.reg & SERCOM_I2CM_STATUS_SYNCBUSY);
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
	int i, new_card = 0;
	unsigned char bSAK, baATQ[2], uid[8], uid_length=255 /*nunca vai ser*/;
	short status;
	unsigned long long card;

	Rc522RFReset(5);

	if ((status = ActivateCard(ISO14443_3_REQA, baATQ, uid, &uid_length, &bSAK)) == STATUS_SUCCESS)
	{
		buzz(100);
		if (uid_length == mifare_uid_length)
		{
			for (i = 0; i < uid_length; i++)
			{
				// Byte diferente => cartão novo.
				if (uid[i] != buf[0][i])
				{
					new_card = 1;
					// Copia dados restantes.
					do {
						buf[0][i] = uid[i];
					} while (++i < uid_length);
					break;
				}
			}
		}
		else 
		{ 
			// Tamanho diferente => cartão novo.
			new_card = 1;
			mifare_uid_length = uid_length;
			for (i = 0; i < uid_length; i++)
				buf[0][i] = uid[i];
		}

		if (new_card)
			for (card = 0, i = 0; i < mifare_uid_length; i++)
				card |= (unsigned long long)buf[0][i] << 8 * (mifare_uid_length - i - 1);
	}
}

void Mifare_Init(void) 
{
	mifare_i2c_init();
	mifare_reset();
	
/*	
	while(1)
	{
		delay_ms(100);
		RcGetReg(0x1);		
	}
	*/
	
	Rc522Init();
}

void Mifare_Run(void)
{
	while (1)
	{
		mifare_activate_card();
	}
}