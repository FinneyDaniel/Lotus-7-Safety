/*=============================================================================
 Copyright Enarka India Pvt Ltd (EIPL) All Rights Reserved.
 All trademarks are owned by Enarka India Private Limited
 ============================================================================ */

/*==============================================================================
 @file  canb_syscntrlnode.c
 @author JOTHI RAMESH
 @date 01-jul-2022
 @brief Description
 ==============================================================================*/

/*==============================================================================
 Includes
 ==============================================================================*/

#include <state_machine.h>
#include "F28x_Project.h"              // Standard libraries headers
#include "F2837xS_Device.h"
#include "inc/hw_types.h"
#include "inc/hw_memmap.h"
#include "inc/hw_can.h"
#include "hal/driverlib/can.h"
#include "cana_defs.h"
#include "canb_defs.h"
#include "scheduler.h"
#include "oi_version.h"
/*==============================================================================
 Defines
 ==============================================================================*/

/*==============================================================================
 Enums
 ==============================================================================*/

/*==============================================================================
 Structures
 ==============================================================================*/

CIRC_BUF_DEF(uiRxbufferSS, 20);
CANB_tzSSREGS CANB_tzSSRegs;

/*==============================================================================
 Macros
 ==============================================================================*/

/*==============================================================================
 Local Function Prototypes
 ==============================================================================*/

void CANB_fnRXevent(void);
void CANB_fnTask(void);
void canbTX_fnSS(void);
void CANB_fnTx(void);
/*==============================================================================
 Local Variables
 ==============================================================================*/

uint16_t uirxPrcsMsgSS[8] = { 0 };
uint16_t uirxMsgSS[8] = { 0 };
uint16_t uiCANtxMsgDataSS[8] = { 0 };

uint32_t u32msgIDB = 0;
uint16_t uiDataLengthB = 0;
/*==============================================================================
 Local Constants
 ==============================================================================*/

/*=============================================================================
 @Function name : void CAN_fnRXevent(void)
 @brief:  function to receive messages over CAN B .
 function called in (isr.c \cpu_timer0_isr\CANB_fnRXevent) 100usec
 @param void
 @return void
 ============================================================================ */
void CANB_fnRXevent(void)
{

    if (CAN_IsMessageReceived(CANB_BASE, CAN_mMAILBOX_20))
    {
        CAN_readMessage(CANB_BASE, CAN_mMAILBOX_20, uirxMsgSS); //SS -> MS

        can_fnEnquedata(&uiRxbufferSS, uirxMsgSS, CanbRegs.CAN_IF2ARB.bit.ID,
                        CanbRegs.CAN_IF2MCTL.bit.DLC);
    }

}

/*=============================================================================
 @Function name : void CANB_fnTask(void)
 @brief : processing the System safety messages.
 Function called in (isr.c \ cpu_timer1_isr\CANB_fnTask) 10msec
 @param :void
 @return :void
 ============================================================================ */
void CANB_fnTask(void)
{
    uint32_t ui32temp1;
    /* ============================================================================ */
    can_fndequedata(&uiRxbufferSS, uirxPrcsMsgSS, &u32msgIDB, &uiDataLengthB);
    //processing received messages of SYSTEM SAFETY SS
    ui32temp1 = (u32msgIDB & 0x00000F00);
    CANB_tzSSRegs.uiUnitID = (uint16_t) ((ui32temp1 & 0x00000F00) >> 8);

    can_fnmsgPrcsSS(uirxPrcsMsgSS);
    /* ============================================================================ */
}
/*=============================================================================
 @Function name : void can_fnmsgProcess(void)
 @brief   : function to Process Master Safety messages over CAN B
 Function called in (isr.c \cpu_timer1_isr\CANB_fnTask \can_fnmsgPrcsSS) 10msec
 @param: void
 @return :void
 ============================================================================ */
void can_fnmsgPrcsSS(uint16_t *msgDataSS)
{
   if (CANB_tzSSRegs.RxCntSS != msgDataSS[0])
    {
        CANB_tzSSRegs.RxCntSS = msgDataSS[0];
        CANB_tzSSRegs.StartCmd = msgDataSS[1];
        CANB_tzSSRegs.Start_LHC = msgDataSS[2];
        CANB_tzSSRegs.btSSComStart = true;
        CANB_tzSSRegs.SSComFailCnt = 0;
        CANB_tzSSRegs.SSComfail = 0;

    }

    CANB_tzSSRegs.SSComFailCnt++;
    if (CANB_tzSSRegs.SSComFailCnt >= CANA_mCOM_FAILCNT)
    {
        CANB_tzSSRegs.btSSComStart = false;
        CANB_tzSSRegs.SSComFailCnt = CANA_mCOM_FAILCNT;
        CANB_tzSSRegs.SSComfail = 1;
    }
    if(CANB_tzSSRegs.StartCmd==2)
    {
        STAT_tzStateMac.Next_st = SHUT_DOWN;
    }

}
/*=============================================================================
 @Function name : void canbTX_fnSS(void)
 @brief  :function to sending the MP messages to SS over CAN B
 Function called (scheduler.c \CANB_fnTx\ canbTX_fnSS) in 50msec
 @param: void
 @return :void
 ============================================================================ */
void canbTX_fnSS(void)
{
    // Master Safety to System Safety

    CANB_tzSSRegs.TxCntSS++;
    CANB_tzSSRegs.TxCntSS++;
    if (CANB_tzSSRegs.TxCntSS >= 255)
    {
        CANB_tzSSRegs.TxCntSS = 0;
    }
  //  CANA_tzIORegs.uiUnitID =1;
    CAN_setupMessageObject(
    CANB_BASE,
                           CAN_mMAILBOX_1,
                           (CANA_tzIORegs.uiUnitID << 8) | CANB_mTX_MSSS_MSGID1,
                           CAN_MSG_FRAME_EXT, CAN_MSG_OBJ_TYPE_TX, 0,
                           CAN_MSG_OBJ_NO_FLAGS,
                           CAN_mLEN8);

    uiCANtxMsgDataSS[0] = CANB_tzSSRegs.TxCntSS;

    uiCANtxMsgDataSS[1] = STAT_tzStateMac.Present_st;
    uiCANtxMsgDataSS[2] = 0;

    uiCANtxMsgDataSS[3] = 0;
    uiCANtxMsgDataSS[4] = 0;

    uiCANtxMsgDataSS[5] = 0;

    uiCANtxMsgDataSS[6] = 0;
    uiCANtxMsgDataSS[7] = 0;

    CAN_sendMessage(CANB_BASE, CAN_mMAILBOX_1, CAN_mLEN8, uiCANtxMsgDataSS);

}
/*=============================================================================
 @Function name : void canbTX_fnSS(void)
 @brief  :function to sending the MP messages to SS over CAN B
 Function called in (scheduler.c \ CANB_fnTx) 50msec
 @param: void
 @return :void
 ============================================================================ */
void CANB_fnTx(void)
{
    // Master Safety to System Safety
    canbTX_fnSS();                    //function called in every 50ms slot

}

/*==============================================================================
 End of File
 ==============================================================================*/

