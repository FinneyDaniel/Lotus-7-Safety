/*=============================================================================
Copyright Enarka India Pvt Ltd (EIPL) All Rights Reserved.
All trademarks are owned by Enarka India Private Limited
============================================================================ */

/*==============================================================================
 @file  eep.h
 @author JOTHI RAMESH
 @date 06-Jun-2022

 @brief Description
==============================================================================*/

#ifndef MODULE_APPLICATION_EEP_INC_EEP_H_
#define MODULE_APPLICATION_EEP_INC_EEP_H_

/*==============================================================================
 Includes
==============================================================================*/

#include <stdint.h>
#include "F2837xS_device.h"
#include "F2837xS_Examples.h"
#include "F2837xS_device.h"
/*==============================================================================
 Defines
==============================================================================*/
#define EEP_mADC_A                             (1U)
#define EEP_mADC_B                             (2U)
#define EEP_mCJC_mV                            (3U)

#define EEP_mRESET_ALL                           (0U)
#define EEP_mQUERY                               (1U)
#define EEP_mRESET                               (0xAAU)

#define EEP_mWRITE                             (1U)
#define EEP_mREAD                              (2U)
#define EEP_mIDLE                              (0U)

#define EEP_mWRITE_MODE                        (0x10)
#define EEP_mREAD_MODE                         (0x01)
#define EEP_mIDLE_MODE                         (0x11)

#define EEP_mSUCCESS                           (1U)
#define MAX_BUFFER_SIZE                        (14U)

#define EEP_mCAN_FLASH_ADDR                                   (0x120U)
#define EEP_mCAN_ERROR_ADDR                                   (0x122U)
#define EEP_mCAN_LENGTH_ADDR                                  (0x124U)
#define EEP_mCAN_CRC32_ADDR                                   (0x128U)

#define EEP_mCHANNEL_ID1                            (0x01)
#define EEP_mCHANNEL_ID2                            (0x02)
#define EEP_mCHANNEL_ID3                            (0x03)
#define EEP_mCHANNEL_ID4                            (0x04)
#define EEP_mCHANNEL_ID5                            (0x05)
#define EEP_mCHANNEL_ID6                            (0x06)
#define EEP_mCHANNEL_ID7                            (0x07)
#define EEP_mCHANNEL_ID8                            (0x08)

/*==============================================================================
 Enums
==============================================================================*/

/*==============================================================================
 Structures
==============================================================================*/
typedef struct EEP_zMSG_BUFF
{
    uint16_t msgStatus;
    uint16_t slaveAddr;                  // Slave address tied to the message.
    uint16_t numBytes;                   // Number of valid bytes in message.
    uint16_t memoryHighAddr;             // EEPROM address of data associated
                                         // with message (high byte).
    uint16_t memoryLowAddr;              // EEPROM address of data associated
                                         // with message (low byte).
    uint16_t msgBuffer[MAX_BUFFER_SIZE]; // Array holding message data.
}EEP_tzMSG_BUFF;

typedef struct EEP_zBOOTBUFF
{
    int32 Calibrated_gain;
    int32 Calibrated_offset;
    int32 Calibrated_sign;
    int32 update_Calibrated_gain;
    int32 update_Calibrated_sign;
    int32 update_Calibrated_offset;

} EEP_tzBOOTBUFF;


extern EEP_tzMSG_BUFF transaction_I2CMsg;
/*==============================================================================
 Macros
==============================================================================*/

/*==============================================================================
 Extern/Public Function Prototypes
==============================================================================*/
extern void idle(void);
extern void EEP_fnResetAll(uint16_t uiInputType);
extern void eep_fnResetVal(uint16_t uiChannelnum, uint16_t adc_Sel, uint16_t uiaddress);
extern void eep_fnCalibWrite_TI(uint16_t *msgData, uint16_t uiadrs);
extern void eep_fnCalibWrite_AI(uint16_t *msgData, uint16_t uiadrs);
extern void eep_fnBootvalues(void);
extern void eep_fnCaliboffset_CJC(uint16_t *msgData);
extern bool EEP_fnWrite(EEP_tzMSG_BUFF  *pTx_msg, uint16_t ui_addr, uint16_t *pdata, uint16_t uilen);
/*==============================================================================
 Extern/Public Variables
==============================================================================*/

extern uint16_t uieepData[8];
extern uint16_t uiRxMsgBuffer[7];
/*==============================================================================
 Extern/Public Constants
==============================================================================*/


/*==============================================================================
 Extern/Public Function Prototypes
==============================================================================*/

/*==============================================================================
 Extern/Public Variables
==============================================================================*/

/*==============================================================================
 Extern/Public Constants
==============================================================================*/


#endif /* MODULE_APPLICATION_EEP_INC_EEP_H_ */
