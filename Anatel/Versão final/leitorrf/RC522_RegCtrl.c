#include "sam.h"
#include "RF_Mifare.h"
#include "RC522_ErrCode.h"
#include "RC522_RegCtrl.h"

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

volatile int mmm;

void RcSetReg(unsigned char RegAddr, unsigned char RegVal)
{
	mifare_i2c_wait_for_bus();
	while (SERCOM3->I2CM.STATUS.reg & SERCOM_I2CM_STATUS_SYNCBUSY);
	/* Set address and direction bit. Will send start command on bus. */
	SERCOM3->I2CM.ADDR.reg = (I2C_ADDRESS << 1) | _I2C_TRANSFER_WRITE;	
	/* Wait for response on bus. */
	mifare_i2c_wait_for_bus();	
	mifare_i2c_master_address_response();
	while (SERCOM3->I2CM.STATUS.reg & SERCOM_I2CM_STATUS_SYNCBUSY);
	SERCOM3->I2CM.DATA.reg = RegAddr;
	mifare_i2c_wait_for_bus();	
	while (SERCOM3->I2CM.STATUS.reg & SERCOM_I2CM_STATUS_SYNCBUSY);
	SERCOM3->I2CM.DATA.reg = RegVal;
	mifare_i2c_wait_for_bus();
	/* Stop command */
	while (SERCOM3->I2CM.STATUS.reg & SERCOM_I2CM_STATUS_SYNCBUSY);
	SERCOM3->I2CM.CTRLB.reg |= SERCOM_I2CM_CTRLB_CMD(3);
	while (SERCOM3->I2CM.STATUS.reg & SERCOM_I2CM_STATUS_SYNCBUSY);
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
	uint8_t dado;
	
	mifare_i2c_wait_for_bus();
	while (SERCOM3->I2CM.STATUS.reg & SERCOM_I2CM_STATUS_SYNCBUSY);
	/* Set address and direction bit. Will send start command on bus. */
	SERCOM3->I2CM.ADDR.reg = (I2C_ADDRESS << 1) | _I2C_TRANSFER_WRITE;	
	/* Wait for response on bus. */
	mifare_i2c_wait_for_bus();	
	mifare_i2c_master_address_response();
	while (SERCOM3->I2CM.STATUS.reg & SERCOM_I2CM_STATUS_SYNCBUSY);
	mifare_i2c_wait_for_bus();	
	SERCOM3->I2CM.DATA.reg = RegAddr;
	mifare_i2c_wait_for_bus();
	/* Stop command */
	while (SERCOM3->I2CM.STATUS.reg & SERCOM_I2CM_STATUS_SYNCBUSY);
	SERCOM3->I2CM.CTRLB.reg |= SERCOM_I2CM_CTRLB_CMD(3);
	
	/* LEITURA */
	mifare_i2c_wait_for_bus();
	while (SERCOM3->I2CM.STATUS.reg & SERCOM_I2CM_STATUS_SYNCBUSY);
	SERCOM3->I2CM.CTRLB.reg |= SERCOM_I2CM_CTRLB_ACKACT;
	/* Set address and direction bit. Will send start command on bus. */
	SERCOM3->I2CM.ADDR.reg = (I2C_ADDRESS << 1) | _I2C_TRANSFER_READ;	
	/* Wait for response on bus. */
	mifare_i2c_wait_for_bus();	
	mifare_i2c_master_address_response();
	dado=SERCOM3->I2CM.DATA.reg;
	/* Stop command */
	while (SERCOM3->I2CM.STATUS.reg & SERCOM_I2CM_STATUS_SYNCBUSY);
	SERCOM3->I2CM.CTRLB.reg |= SERCOM_I2CM_CTRLB_CMD(3);		
	while (SERCOM3->I2CM.STATUS.reg & SERCOM_I2CM_STATUS_SYNCBUSY);
	
	return dado;
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
