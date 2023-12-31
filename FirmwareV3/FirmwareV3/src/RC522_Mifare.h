/* /////////////////////////////////////////////////////////////////////////////////////////////////
//                     Copyright (c) Philips Semiconductors
//
//         All rights are reserved. Reproduction in whole or in part is
//        prohibited without the written consent of the copyright owner.
//    Philips reserves the right to make changes without notice at any time.
//   Philips makes no warranty, expressed, implied or statutory, including but
//   not limited to any implied warranty of merchantability or fitness for any
//  particular purpose, or that the use will not infringe any third party patent,
//   copyright or trademark. Philips must not be liable for any loss or damage
//                            arising from its use.
///////////////////////////////////////////////////////////////////////////////////////////////// */ 
#ifndef __MFRC522_MIFARE_H__
#define __MFRC522_MIFARE_H__

/* \ingroup mifare
 *  \name Commands
 *  Command definitions for Mifare operation. These command bytes comply to the MIFARE
 *  specification and serve as one parameter for the MIFARE transaction commands defined
 *  within the scope of this implementation.
 */
/*@{*/
#define MIFARE_AUTHENT_A            ((unsigned char)0x60)   /* AUTHENT A command. */
#define MIFARE_AUTHENT_B            ((unsigned char)0x61)   /* AUTHENT B command. */
#define MIFARE_READ                 ((unsigned char)0x30)   /* READ command. */
#define MIFARE_WRITE                ((unsigned char)0xA0)   /* WRITE 16 bytes command. */
#define MIFARE_WRITE4               ((unsigned char)0xA2)   /* WRITE 4 bytes command. */
#define MIFARE_INCREMENT            ((unsigned char)0xC1)   /* INCREMENT command. */
#define MIFARE_DECREMENT            ((unsigned char)0xC0)   /* DECREMENT command.  */
#define MIFARE_RESTORE              ((unsigned char)0xC2)   /* RESTORE command. */
#define MIFARE_TRANSFER             ((unsigned char)0xB0)   /* TRANSFER command. */
#define MIFARE_NOCOMMAND            ((unsigned char)0x00)   /* VOID command (no MIFARE command). */


/* \if use_html
 *  \name Control Behaviour
 *  \ingroup mfrd
 *   Control behaviour of the protocol upon reply:
 *   OR-ing of values allows the protocol to be prepared for multiple reply types to create a
 *   positive result (e.g. some cards return an ACK, some nothing upon certain commands).
 *  \endif
 */
/*@{*/
#define MIFARE_EXPECT_TIMEOUT       ((unsigned char)0x01)   /*!< Tells the library to expect a timeout. */
#define MIFARE_EXPECT_ACK           ((unsigned char)0x02)   /*!< Let the library expect an Acknowledge response. */
#define MIFARE_EXPECT_DATA          ((unsigned char)0x04)   /*!< The library shall expect data. */
/*!
 * (Not-) Acknowledge:
 * When a MF PICC returns ACK, a logical AND with the mask yields a value other than 0. Otherwise,
 * in case of NACK, the mask zeroes the byte (or nibble respectively).
 */
#define MIFARE_ACK_MASK             ((unsigned char)0x0A)           /* */

/* Definitions for Request command. */
#define REQUEST_BITS                0x07
#define ATQA_LENGTH                 0x02

/* Request codes */
/*! \ingroup mifare
    \brief Request code for all devices. */
#define ISO14443_3_WUPA             0x52
/*! \ingroup mifare
    \brief Request code only for idle devices. */
#define ISO14443_3_REQA             0x26

/* Definitions for lower Anticollision / Select functions. */
#define BITS_PER_BYTE               0x08
#define UPPER_NIBBLE_SHIFT          0x04
#define COMPLETE_UID_BITS           0x28
#define NVB_MIN_PARAMETER           0x20
#define NVB_MAX_PARAMETER           0x70

/*Command byte definitions for Anticollision/Select functions.*/
#define SELECT_CASCADE_LEVEL_1      0x93
#define SELECT_CASCADE_LEVEL_2      0x95
#define SELECT_CASCADE_LEVEL_3      0x97
#define MAX_CASCADE_LEVELS          0x03
#define SINGLE_UID_LENGTH           0x20
#define CASCADE_BIT                 0x04

/* Definitions for Select functions. */
#define SAK_LENGTH                  0x01

/* Command and Parameter byte definitions for HaltA function. */
#define HALTA_CMD                   0x50
#define HALTA_PARAM                 0x00
#define HALTA_CMD_LENGTH            0x02




/*************************************************
Function:       SetPowerDown
Description:
     set the rc522 enter or exit power down mode
Parameter:
     ucFlag     0   --  exit power down mode
                !0  --  enter power down mode
**************************************************/
/*! \ingroup mifare
    \param[in] ucFlag    0   --  exit power down mode
                        !0  --  enter power down mode
    \brief Sets the rc522 enter or exit power down mode.
 */
extern void SetPowerDown(unsigned char ucFlag);

/*************************************************
Function:       Rc522Init
Description:
     initialize rc522 as a mifare reader
Parameter:
     NONE
**************************************************/
/*! \ingroup mifare
    \brief Initializees rc522 as a mifare reader.
 */
extern void Rc522Init(void);

void Rc522RFReset(unsigned char ms);


/*************************************************
Function:       ChangeRc522BaudRate
Description:
     Changes the serial speed of the RC522.
     Note that the speed of the host interface (UART on PC) has to be also set to the
     appropriate one.
Parameter:
     baudrate   new baudrate for rc522
Return:
     short      status of implement
**************************************************/
/*! \ingroup mifare
    \param[in] baudrate   new baudrate for rc522
    \return status of implementation
    \brief
     Changes the serial speed of the RC522.
     Note that the speed of the host interface (UART on PC) has to be also set to the
     appropriate one.
 */


extern short SetTimeOut(unsigned int uiMicroSeconds);

/*************************************************
Function:       Request
Description:
     REQA, request to see if have a ISO14443A card in the field
Parameter:
     req_code   command code(ISO14443_3_REQALL or ISO14443_3_REQIDL)
     atq        the buffer to save the answer to request from the card
Return:
     short      status of implement
**************************************************/
/*! \ingroup mifare
    \param[in]  req_code   command code(ISO14443_3_REQALL or ISO14443_3_REQIDL)
    \param[out] atq        the buffer to save the answer to request from the card
    \return status of implementation
    \brief REQA, request to see if have a ISO14443A card in the field
 */
extern short Request(unsigned char req_code, unsigned char *atq);

/*************************************************
Function:       AnticollSelect
Description:
     selecte a card to response the following command
     NOTE: this founction is used internal only, and cannot call by application program
Parameter:
     sel_code   command code
     snr        buffer to store the card UID
     sak        the byte to save the ACK from card
Return:
     short      status of implement
**************************************************/
/*! \ingroup mifare
    \param[in]  bcnt
    \param[in] *snr        buffer to store the card UID
    \return status of implementation
    \brief Select a card to response the following command
    \note This founction is used internal only, and cannot call by application program
 */

extern short CascAnticoll(unsigned char sel_code, 
                          unsigned char bitcount,
                          unsigned char *snr);

extern short Select(unsigned char sel_code, unsigned char *snr, unsigned char *sak);



/*************************************************
Function:       Authentication
Description:
     authentication the password for a sector of mifare card
Parameter:
     auth_mode  specify key A or key B -- MIFARE_AUTHENT_A or MIFARE_AUTHENT_A
     key        the buffer stored the key(6 bytes)
     snr        the buffer stored the selected card's UID
     addr       the block address of a sector
Return:
     short      status of implement
    \ingroup mifare
    \param[in]  auth_mode  specify key A or key B -- MIFARE_AUTHENT_A or MIFARE_AUTHENT_A
    \param[in]  key        the buffer stored the key(6 bytes)
    \param[in]  snr        the buffer stored the selected card's UID
    \param[in]  addr       the block address of a sector
    \return status of implementation
    \brief Authentication the password for a sector of mifare card.
 */
extern short Authentication(unsigned char auth_mode,unsigned char *key, unsigned char *snr, unsigned char block);

/*************************************************
Function:       Read
Description:
     read 16 bytes data from a block
Parameter:
     addr       the address of the block
     _data      the buffer to save the 16 bytes data
Return:
     short      status of implement
**************************************************/
/*! \ingroup mifare
    \param[in]  addr       the address of the block
    \param[out] _data      the buffer to save the 16 bytes data
    \return status of implementation
    \brief read 16 bytes data from a block.
 */
extern short Read(unsigned char addr, unsigned char *_data);

/*************************************************
Function:       Write
Description:
     write 16 bytes data to a block
Parameter:
     addr       the address of the block
     _data      the data to write
Return:
     short      status of implement
**************************************************/
/*! \ingroup mifare
    \param[in]  addr       the address of the block
    \param[in]  _data      the buffer to save the 16 bytes data
    \return status of implementation
    \brief write 16 bytes data to a block
 */
extern short Write( unsigned char addr, unsigned char *_data);


/*************************************************
Function:       ValueOper
Description:
     block value operation function, increment or decrement the block value
     and transfer to a block
Parameter:
     OperMode   MIFARE_INCREMENT or MIFARE_DECREMENT
     addr       the address of the block
     value      the value to be increment or decrement
     trans_addr the address to save the resulet of increment or decrement
Return:
     short      status of implement
**************************************************/
/*! \ingroup mifare
    \param[in]  value_mode MIFARE_INCREMENT or MIFARE_DECREMENT
    \param[in]  addr       the address of the block
    \param[in]  value      the value to be increment or decrement
    \param[in]  trans_addr the address to save the resulet of increment or decrement
    \return status of implementation
    \brief
     block value operation function, increment or decrement the block value
    and transfer to a block
 */
extern short ValueOper(unsigned char value_mode, unsigned char addr, unsigned char *value, unsigned char trans_addr);

/*************************************************
Function:       InitBlock
Description:
     initialize a block value
Parameter:
     addr       the address of the block
     value      the value to be initialized, 4 bytes buffer
Return:
     short      status of implement
**************************************************/
/*! \ingroup mifare
    \param[in]  addr       the address of the block
    \param[in]  value      the value to be initialized, 4 bytes buffer
    \return status of implementation
    \brief initialize a block value
 */
extern short InitBlock(unsigned char addr,unsigned char *value);

/*************************************************
Function:       HaltA
Description:
     halt the current selected card
Parameter:
     NONE
Return:
     short      status of implement
**************************************************/
/*! \ingroup mifare
    \return status of implementation
    \brief halt the current selected card
 */
extern short HaltA(void);



#endif

