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
void RcSetReg(unsigned char RegAddr, unsigned char RegVal)
{
	int ret;

//	ret = i2c_smbus_write_byte_data(global_client, RegAddr & 0x3F, RegVal);
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
	int ret=0;

	//ret = i2c_smbus_read_byte_data(global_client, RegAddr & 0x3F);

	return ret & 0xFF; 
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
