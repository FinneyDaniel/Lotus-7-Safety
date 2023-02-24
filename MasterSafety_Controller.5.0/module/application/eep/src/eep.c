/*=============================================================================
 Copyright Enarka India Pvt Ltd (EIPL) All Rights Reserved.
 All trademarks are owned by Enarka India Private Limited
 ============================================================================ */

/*==============================================================================
 @file  eep.c
 @author JOTHI RAMESH
 @date 19-JUN-2022

 @brief Description
 ==============================================================================*/

/*==============================================================================
 Includes
 ==============================================================================*/

#include "eep.h"
#include "F28x_Project.h"              // Standard libraries headers
#include "F2837xS_Device.h"
#include "math.h"
#include <stdint.h>

/*==============================================================================
 Defines
 ==============================================================================*/

/*==============================================================================
 Enums
 ==============================================================================*/

/*==============================================================================
 Structures
 ==============================================================================*/
EEP_tzMSG_BUFF transaction_I2CMsg;
EEP_tzMSG_BUFF *currentMsgPtr;
EEP_tzBOOTBUFF eep_tzBootBuf;
/*==============================================================================
 Macros
 ==============================================================================*/
volatile Uint16 timeout_count;
#define I2C_DELAY           5000
#define I2C_WAIT_UNTIL(x)   { \
                                timeout_count = 0; \
                                while((x)) \
                                { \
                                    timeout_count++; \
                                    if (timeout_count >= I2C_DELAY) \
                                        { \
                                        return 0; \
                                        } \
                                } \
                            } \

/******************************************************************************/
volatile Uint16 timeout_count1;
#define I2C_DELAY1           5000
#define I2C_WAIT_UNTIL1(x)   { \
                                timeout_count1 = 0; \
                                while((x)) \
                                { \
                                    timeout_count1++; \
                                    if (timeout_count1 >= I2C_DELAY1) \
                                        { \
                                        return 0; \
                                        } \
                                } \
                            } \
/*==============================================================================
 Local Function Prototypes
==============================================================================*/
void (*eep_fnEvent)(void);
static bool eep_fnWrite(EEP_tzMSG_BUFF *WriteMsgPtr);
static bool eep_fnRead(EEP_tzMSG_BUFF *ReadMsgPtr);
static void eep_fnWritetask(void);
static void eep_fnReadtask(void);
bool eep_read_word(uint16_t address, uint16_t *data, uint16_t bytes);
bool eep_write_word(uint16_t address, uint16_t *data, uint16_t bytes);

/*==============================================================================
 Local Variables
 ==============================================================================*/
bool bt_busyflag;
uint16_t uiRW_eep = 0;
uint16_t uiTxMsgBuffer[7] = { 0 };
uint16_t uiRxMsgBuffer[7] = { 0 };
uint16_t uieepData[8] = { 0 };

/*==============================================================================
 Local Constants
 ==============================================================================*/

/*=============================================================================
 @brief eep_fnWrite : write function for i2c bus

 @param EEP_tzMSG_BUFF *WriteMsgPtr
 @return uint16_t (0) on SUCCES
 ============================================================================ */
static bool eep_fnWrite(EEP_tzMSG_BUFF *WriteMsgPtr)
{
    uint16_t i = 0;

    I2C_WAIT_UNTIL(I2cbRegs.I2CMDR.bit.STP == 1);

    I2cbRegs.I2CSAR.all = WriteMsgPtr->slaveAddr;

//
// Check if bus busy
//
    I2C_WAIT_UNTIL(I2cbRegs.I2CSTR.bit.BB == 1);

    I2cbRegs.I2CCNT = WriteMsgPtr->numBytes + 2;

    I2cbRegs.I2CDXR.all = WriteMsgPtr->memoryHighAddr;
    I2cbRegs.I2CDXR.all = WriteMsgPtr->memoryLowAddr;

    for (i = 0; i < WriteMsgPtr->numBytes; i++)
    {
        I2cbRegs.I2CDXR.all = WriteMsgPtr->msgBuffer[i];
    }

    //
    // Send start as master transmitter
    //
    I2cbRegs.I2CMDR.all = 0x6E20;

    I2C_WAIT_UNTIL(I2cbRegs.I2CSTR.bit.SCD == 0);

    return true;

}

/*=============================================================================
 @brief eep_fnRead : read from eeprom

 @param uiNumBytes, data
 @return void
 ============================================================================ */
static bool eep_fnRead(EEP_tzMSG_BUFF *ReadMsgPtr)
{
    uint16_t i = 0;

    I2C_WAIT_UNTIL(I2cbRegs.I2CMDR.bit.STP == 1);

    I2cbRegs.I2CSAR.all = ReadMsgPtr->slaveAddr;

    // If we are in the the address setup phase, send the address without a
    // stop condition.
    //
    // Check if bus busy
    I2C_WAIT_UNTIL(I2cbRegs.I2CSTR.bit.BB == 1);

    I2cbRegs.I2CCNT = 2;

    I2cbRegs.I2CDXR.all = ReadMsgPtr->memoryHighAddr;
    I2cbRegs.I2CDXR.all = ReadMsgPtr->memoryLowAddr;

    I2cbRegs.I2CMDR.all = 0x6620;

    I2C_WAIT_UNTIL(I2cbRegs.I2CSTR.bit.ARDY != 1);

    if (I2cbRegs.I2CSTR.bit.NACK == 1)
    {
        I2cbRegs.I2CMDR.bit.STP = 1;
        I2cbRegs.I2CSTR.all = (1U << 1);
        return false;
    }

    I2cbRegs.I2CCNT = ReadMsgPtr->numBytes;

    I2cbRegs.I2CMDR.all = 0x6C20;

    if (I2cbRegs.I2CSTR.bit.NACK == 1)
    {
        I2cbRegs.I2CMDR.bit.STP = 1;
        I2cbRegs.I2CSTR.all = (1U << 1);
        return false;
    }

    I2C_WAIT_UNTIL(I2cbRegs.I2CSTR.bit.SCD == 0);

    DELAY_US(5000);

    for (i = 0; i < ReadMsgPtr->numBytes; i++)
    {
        ReadMsgPtr->msgBuffer[i] = I2cbRegs.I2CDRR.all;
    }

    return true;
}

/*=============================================================================
 @Function Name : bool EEP_fnWrite(EEP_tzMSG_BUFF  *pTx_msg, uint16_t ui_addr, uint16_t *pdata, uint16_t uilen)
 function called in scheduler.c / canaFlashEvent.c / EEP_fnWrite @50msec
 @brief read function for i2c bus

 @param:(EEP_tzMSG_BUFF  *pTx_msg, uint16_t ui_addr, uint16_t *pdata, uint16_t uilen)
 @return :bool
 ============================================================================ */
bool EEP_fnWrite(EEP_tzMSG_BUFF *pTx_msg, uint16_t ui_addr, uint16_t *pdata,
                 uint16_t uilen)
{
    bool bt_status = false;
    bt_busyflag = false;
    uint16_t i = 0;

    //if((!bt_busyflag) && (pTx_msg->msgStatus == EEP_mIDLE_MODE))
    {
        bt_busyflag = true;

        pTx_msg->msgStatus = EEP_mWRITE_MODE;
        pTx_msg->slaveAddr = 0x50;
        pTx_msg->memoryHighAddr = (ui_addr >> 8) & 0xFF;
        pTx_msg->memoryLowAddr = ui_addr & 0xFF;
        pTx_msg->numBytes = uilen;
        for (i = 0; i < uilen; i++)
        {
            pTx_msg->msgBuffer[i] = pdata[i];
        }
//        memcpy(pTx_msg->msgBuffer,pdata,sizeof(pdata));
//        *pTx_msg->msgBuffer = *pdata;
        uiRW_eep = EEP_mWRITE;
        eep_fnEvent = eep_fnWritetask;
        currentMsgPtr = pTx_msg;

        (*eep_fnEvent)();
        bt_status = true;
    }

    return bt_status;
}

/*=============================================================================
 @Function Name : uint16_t I2C_fnReadData(struct I2CMsg *msg)
 @brief read function for i2c bus

 @param struct I2CMsg *msg
 @return uint16_t (0) on SUCCES
 ============================================================================ */
bool EEP_fnRead(EEP_tzMSG_BUFF *pRx_msg, uint16_t ui_addr, uint16_t *pdata,
                uint16_t uilen)
{
    bool bt_status = false;
    bt_busyflag = false;
    uint16_t i = 0;

    //if((!bt_busyflag) && (pRx_msg->msgStatus == EEP_mIDLE_MODE))
    {
        bt_busyflag = true;
        pRx_msg->msgStatus = EEP_mREAD_MODE;
        pRx_msg->slaveAddr = 0x50;
        pRx_msg->memoryHighAddr = (ui_addr >> 8) & 0xFF;
        pRx_msg->memoryLowAddr = ui_addr & 0xFF;
        pRx_msg->numBytes = uilen;

        uiRW_eep = EEP_mREAD;
        eep_fnEvent = eep_fnReadtask;
        currentMsgPtr = pRx_msg;

        (*eep_fnEvent)();

        pRx_msg = currentMsgPtr;

        for (i = 0; i < uilen; i++)
        {
            pdata[i] = pRx_msg->msgBuffer[i];
        }

        bt_status = true;
    }

    return bt_status;

}

/*=============================================================================
 @Function Name : void eep_fnWritetask(void)
 @brief write task for EEPROM

 @param void
 @return void
 ============================================================================ */
static void eep_fnWritetask(void)
{
    uint16_t Error;
    if (uiRW_eep == EEP_mWRITE)
    {
        if (currentMsgPtr->msgStatus == EEP_mWRITE_MODE)
        {
            Error = eep_fnWrite(currentMsgPtr);

            if (Error == EEP_mSUCCESS)
            {
                eep_fnEvent = idle;
                (*eep_fnEvent)();
            }

        }
        DELAY_US(5000);
    }
}

/*=============================================================================
 @Function Name : uint16_t eep_fnReadtask(void)
 @brief read task for EEPROM

 @param void
 @return void
 ============================================================================ */
static void eep_fnReadtask(void)
{
    uint16_t Error;

    if (uiRW_eep == EEP_mREAD)
    {
        if (currentMsgPtr->msgStatus == EEP_mREAD_MODE)
        {
            Error = eep_fnRead(currentMsgPtr);

            if (Error == EEP_mSUCCESS)
            {
                eep_fnEvent = idle;
                (*eep_fnEvent)();
            }

        }
        DELAY_US(5000);
    }
}

/*=============================================================================
 @Function Name : void idle(void)
 @brief idle mode of EEPROM functions

 @param void
 @return void
 ============================================================================ */
void idle(void)
{
    bt_busyflag = false;
    uiRW_eep = EEP_mIDLE;
    transaction_I2CMsg.msgStatus = EEP_mIDLE_MODE;
}

/*=============================================================================
 @function name:bool eep_write_word(uint16_t address, uint16_t *data, uint16_t bytes)
 @brief:eeprom read function for safety_CRC test. called @infinite loop
 @param :(uint16_t address, uint16_t *data, uint16_t bytes)
 @return: void
 ============================================================================ */
bool eep_read_word(uint16_t address, uint16_t *data, uint16_t bytes)
{
    uint16_t i = 0;

    I2C_WAIT_UNTIL1(I2cbRegs.I2CMDR.bit.STP == 1);

    I2cbRegs.I2CSAR.all = 0x50;

    // If we are in the the address setup phase, send the address without a
    // stop condition.
    //
    // Check if bus busy
    I2C_WAIT_UNTIL1(I2cbRegs.I2CSTR.bit.BB == 1);

    I2cbRegs.I2CCNT = 2;

    I2cbRegs.I2CDXR.all = (address >> 8) & 0xFF;
    I2cbRegs.I2CDXR.all = (address >> 0) & 0xFF;

    I2cbRegs.I2CMDR.all = 0x6620;

    I2C_WAIT_UNTIL1(I2cbRegs.I2CSTR.bit.ARDY != 1);

    if (I2cbRegs.I2CSTR.bit.NACK == 1)
    {
        I2cbRegs.I2CMDR.bit.STP = 1;
        I2cbRegs.I2CSTR.all = (1U << 1);
        return false;
    }

    I2cbRegs.I2CCNT = bytes;

    I2cbRegs.I2CMDR.all = 0x6C20;

    if (I2cbRegs.I2CSTR.bit.NACK == 1)
    {
        I2cbRegs.I2CMDR.bit.STP = 1;
        I2cbRegs.I2CSTR.all = (1U << 1);
        return false;
    }

    I2C_WAIT_UNTIL1(I2cbRegs.I2CSTR.bit.SCD == 0);

    DELAY_US(5000);

    for (i = 0; i < bytes; i++)
    {
        data[i] = I2cbRegs.I2CDRR.all;
    }

    return true;

}
/*=============================================================================
 @function name:bool eep_write_word(uint16_t address, uint16_t *data, uint16_t bytes)
 @brief:eeprom write function for safety_CRC test. called @infinite loop
 @param :(uint16_t address, uint16_t *data, uint16_t bytes)
 @return: void
 ============================================================================ */

bool eep_write_word(uint16_t address, uint16_t *data, uint16_t bytes)
{
    uint16_t i = 0;

    I2C_WAIT_UNTIL1(I2cbRegs.I2CMDR.bit.STP == 1);

    I2cbRegs.I2CSAR.all = 0x50;

    //
    // Check if bus busy
    //
    I2C_WAIT_UNTIL1(I2cbRegs.I2CSTR.bit.BB == 1);

    I2cbRegs.I2CCNT = bytes + 2;

    I2cbRegs.I2CDXR.all = (address >> 8) & 0xFF;
    I2cbRegs.I2CDXR.all = (address >> 0) & 0xFF;

    for (i = 0; i < bytes; i++)
    {
        I2cbRegs.I2CDXR.all = data[i];
    }

    //
    // Send start as master transmitter
    //
    I2cbRegs.I2CMDR.all = 0x6E20;
    DELAY_US(5000);

    I2C_WAIT_UNTIL1(I2cbRegs.I2CSTR.bit.SCD == 0);

    return true;
}

/*==============================================================================
 End of File
 ==============================================================================*/
