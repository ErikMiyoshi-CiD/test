#include <asf.h>

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

void RcSetReg(uint8_t RegAddr, uint8_t RegVal)
{
	struct i2c_master_packet p;
	uint8_t data[2] = {RegAddr,RegVal};
	
	p.address=I2C_ADDRESS;
	p.data = data;
	p.data_length = 2;
	p.ten_bit_address=false;
	p.high_speed=false;
	
	i2c_master_write_packet_wait(&i2c_master_instance,&p);
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
	struct i2c_master_packet write_p;
	struct i2c_master_packet read_p;
	uint8_t read_data;
	
	write_p.address=I2C_ADDRESS;
	write_p.data = &RegAddr;
	write_p.data_length = 1;
	write_p.ten_bit_address=false;
	write_p.high_speed=false;
	
	read_p.address=I2C_ADDRESS;
	read_p.data = &read_data;
	read_p.data_length=1;
	read_p.ten_bit_address=false;
	read_p.high_speed=false;
		
	i2c_master_write_packet_wait(&i2c_master_instance,&write_p);
	i2c_master_read_packet_wait(&i2c_master_instance,&read_p);
	
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
