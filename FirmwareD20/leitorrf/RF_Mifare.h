/*
 * RF_Mifare.h
 *
 * Created: 2014-02-26 12:53:10 PM
 *  Author: Alb
 */ 


#ifndef RF_MIFARE_H_
#define RF_MIFARE_H_

#define I2C_TIMEOUT 1000
#define I2C_ADDRESS (0x28)

enum _i2c_transfer_direction {
	_I2C_TRANSFER_WRITE = 0,
	_I2C_TRANSFER_READ = 1,
};

int mifare_i2c_master_address_response(void);
void mifare_i2c_wait_for_bus(void);

void mifare_reset(void);
void mifare_activate_card(void);

void Mifare_Init(void);
void Mifare_Run(void);

#endif /* RF_MIFARE_H_ */