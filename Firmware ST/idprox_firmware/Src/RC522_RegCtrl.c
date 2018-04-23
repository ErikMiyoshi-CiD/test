#include "stm32f0xx_hal.h"

#include "RF_Mifare.h"
#include "RC522_ErrCode.h"
#include "RC522_RegCtrl.h"

#include "i2c.h"

/*************************************************
Function:       RcSetReg
Description:
Write data to register of RC522
Parameter:
RegAddr       The address of the regitster
RegVal        The value to be writen
Return:
None
 **************************************************/
void RcSetReg(uint8_t RegAddr, uint8_t RegVal)
{
	uint8_t data[2] = {RegAddr, RegVal};
	
	HAL_I2C_Master_Transmit(&hi2c1, I2C_ADDRESS << 1, data, 2, 10000);
}

/*************************************************
Function:       qRcGetReg
Description:
Write data to register of RC522
Parameter:
RegAddr       The address of the regitster to be readed
Return:
The value of the specify register
 **************************************************/
unsigned char RcGetReg(unsigned char RegAddr)
{
	uint8_t read_data;
	
	HAL_I2C_Master_Transmit(&hi2c1, I2C_ADDRESS << 1, &RegAddr, 1, 10000);
	
	HAL_I2C_Master_Receive(&hi2c1, I2C_ADDRESS << 1, &read_data, 1, 10000);
	
	return read_data;
}

/*************************************************
Function:       RcModifyReg
Description:
Change some bits of the register
Parameter:
RegAddr       The address of the regitster
ModifyVal        The value to change to, set or clr?
MaskByte      Only the corresponding bit '1' is valid,
Return:
None
 **************************************************/
void RcModifyReg(unsigned char RegAddr, unsigned char ModifyVal, unsigned char MaskByte)
{
	unsigned char RegVal;
	RegVal = RcGetReg(RegAddr);

	if(ModifyVal)
		RegVal |= MaskByte;
	else
		RegVal &= (~MaskByte);

	RcSetReg(RegAddr, RegVal);
}
