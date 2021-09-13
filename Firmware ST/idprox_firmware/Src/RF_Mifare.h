#ifndef RF_MIFARE_H_
#define RF_MIFARE_H_

#define I2C_ADDRESS (0x28)

void mifare_i2c_init(void);

void mifare_reset(void);
void mifare_activate_card(void);

void Mifare_Init(void);
void Mifare_Run(void);

extern struct i2c_master_module i2c_master_instance;

#endif /* RF_MIFARE_H_ */
