/*=============================================================================
 Copyright Enarka India Pvt Ltd (EIPL) All Rights Reserved.
 All trademarks are owned by Enarka India Private Limited
 ============================================================================ */

/*==============================================================================
 @file  canaEvent.c
 @author JOTHI RAMESH
 @date 20-Jun-2022

 @brief Description
 ==============================================================================*/

/*==============================================================================
 Includes
 ==============================================================================*/

#include <stdint.h>
#include "F2837xS_device.h"
#include "F2837xS_Examples.h"
#include "F2837xS_device.h"
#include "inc/hw_types.h"
#include "inc/hw_memmap.h"
#include "inc/hw_can.h"
#include "cana_defs.h"
#include "canb_defs.h"
#include "hal/driverlib/can.h"
#include "state_machine.h"
#include "control_defs.h"
/*==============================================================================
 Defines
 ==============================================================================*/

/*==============================================================================
 Enums
 ==============================================================================*/

/*==============================================================================
 Structures
 ==============================================================================*/

CANA_tzIOREGS CANA_tzIORegs;
CANA_tzIOFLAGS CANA_tzIOflags;
CANA_tzTIMERIOREGS CANA_tzIOtimers;
CANA_tzMPREGS CANA_tzMPRegs;

CANA_tzAI_IOREGS CANA_tzAIData_IORegs[CANA_mTOTAL_IONODE];
CANA_tzDIFREQ_IOREGS CANA_tzAIDataFreq_IORegs[CANA_mTOTAL_IONODE];

CANA_tzTHERMAL_IOREGS CANA_tzThermal_IORegs[CANA_mTOTAL_IONODE];
CANA_tzAISENSOR_DATA CANA_tzAISensorData;
CANA_tzDISENSOR_DATA CANA_tzDISensorData;

CANA_tzTHERMOCOUPLE_DATA CANA_tzThermoCoupleData;

union CANA_tzDI_IOREGS CANA_tzLPCDI_IORegs[CANA_mTOTAL_LPCNODES],
        CANA_tzLHCDI_IORegs[CANA_mTOTAL_LHCNODES];
union CANA_tzAIFLT_IOREGS CANA_tzLPCAIFlt_IORegs[CANA_mTOTAL_IONODE],
        CANA_tzLHCAIFlt_IORegs[CANA_mTOTAL_IONODE];

union CANA_tzLPCIO1_DIFLT_IOREGS CANA_tzLPCIO1_DIFaultRegs;
union CANA_tzLPCIO2_DIFLT_IOREGS CANA_tzLPCIO2_DIFaultRegs;

union CANA_tzLHCIO1_DIFLT_IOREGS CANA_tzLHCIO1_DIFaultRegs;
union CANA_tzLHCIO2_DIFLT_IOREGS CANA_tzLHCIO2_DIFaultRegs;

union CANA_tzLPCIO1_AIFLT_IOREGS CANA_tzLPCIO1_AIFaultRegs;
union CANA_tzLPCIO2_AIFLT_IOREGS CANA_tzLPCIO2_AIFaultRegs;
union CANA_tzLPCIO2_AIFLT_IOREGS CANA_tzLPCIO2_AIFaultRegs;
union CANA_tzLHCIO1_AIFLT_IOREGS CANA_tzLHCIO1_AIFaultRegs;
union CANA_tzLHCIO2_AIFLT_IOREGS CANA_tzLHCIO2_AIFaultRegs;
union CANA_tzLPCIO1_AOFLT_IOREGS CANA_tzLPCIO1_AOFaultRegs;

union CANA_tzTHERMALFLT_IOREGS CANA_tzThermalFaultRegs;

CANA_tzDO_IOREGS CANA_tzSetDO_IORegs;

can_tzAnaOPParams CANA_tzAnaOPParams;

//can_tzDigOPParams CANA_tzDigOPParams;

CANA_tzDIG_OP CANA_tzDO[2][2];
CANA_tzANG_OP CANA_tzAO[2][2];

/*==============================================================================
 Macros
 ==============================================================================*/

CIRC_BUF_DEF(uiRxbufferLPCIO, 100);
CIRC_BUF_DEF(uiRxbufferLHCIO, 100);
CIRC_BUF_DEF(uiRxbufferMP, 50);

/*==============================================================================
 Local Function Prototypes
 ==============================================================================*/

void CANA_fnInitMBox(void);
void CANA_fnRXevent(void);
void CANA_fnTx(void);
void CANA_fnTask(void);
static void cana_fnmsgPrcsLPCIO(uint16_t uimsgID, uint16_t *msgData,
                                uint16_t uiNodeType);

static void cana_fnmsgPrcsLHCIO(uint16_t uimsgID, uint16_t *msgData,
                                uint16_t uiNodeType);

static void cana_fnmsgPrcsMP(uint16_t *msgDataMP);
void can_fnmsgPrcsSS(uint16_t *msgDataSS);

bool can_fnEnquedata(can_tzcirc_buff *ptr, uint16_t *data, uint32_t msgID,
                     uint16_t DLC);
bool can_fndequedata(can_tzcirc_buff *ptr, uint16_t *data, uint32_t *msgID,
                     uint16_t *DLC);
void CANA_fnCmdsForDigOPs(uint16_t ui16unitID, uint16_t ui16cab_ID,
                          uint16_t ui16nodeID, CANA_tzDIG_OP *ptrDigOP);

void CANA_fnCmdsForAnaOPVs(uint16_t ui16unitID, uint16_t ui16cab_ID,
                           uint16_t ui16nodeID, can_tzAnaOPParams *ptrAO_V);

void CANA_fnCmdsForAnaOPIs(uint16_t ui16unitID, uint16_t ui16cab_ID,
                           uint16_t ui16nodeID, can_tzAnaOPParams *ptrAO_I);

void CANA_fnMSTxCmds(uint16_t ui16CabiD, uint16_t NodeID,
                     CANA_tzDIG_OP *ptrDigAOP);

void CANA_fnMPTxCmds(uint16_t ui16CabiD, uint16_t NodeID,
                     CANA_tzANG_OP *ptrDigAOP);
static void CANA_fnMPRxCmdsProcess(uint16_t uiCabIDDO, uint16_t uiNodeIDDO,
                                   CANA_tzMPREGS *CANA_tzMPRegs);
void CAN_fnTurnoffIO(void);
void CANA_fnMPTxDOCmds(void);
/*==============================================================================
 Local Variables
 ==============================================================================*/

uint16_t uirxMsgLPCIO[8] = { 0 };
uint16_t uirxMsgLHCIO[8] = { 0 };
uint16_t uirxMsgMP[8] = { 0 };

uint16_t ui16txMsgDataIO[8] = { 0 };
uint16_t uirxPrcsMsgLPCIO[8] = { 0 };
uint16_t uirxPrcsMsgLHCIO[8] = { 0 };
uint16_t uirxPrcsMsgMP[8] = { 0 };

uint32_t u32msgID1 = 0, u32msgID2 = 0, u32msgID3 = 0;
uint16_t uiDataLength1 = 0, uiDataLength2 = 0, uiDataLength3 = 0;
uint16_t uiMsgtype = 0, uiNodeType = 0;
uint16_t uiCANtxMsgDataMP[8] = { 0 };
uint16_t uiCANtxMsgDataMP1[8] = { 0 };
uint16_t uiCANtxMsgDataMS[8] = { 0 };
uint16_t ui16CabID = 0, ui16prev_value = 0, ui16CabIDMP = 0;
uint16_t uiCabIDDO = 0, uiNodeIDDO = 0;
uint16_t ui16Cnt = 0;
uint16_t uiMPtoMS_Rxcnt = 0;

/*==============================================================================
 Local Constants
 ==============================================================================*/

/*=============================================================================
 @Function name : void can_fnEnquedata(void)
 @brief  function to add data to queue

 @param void
 @return void
 ============================================================================ */
bool can_fnEnquedata(can_tzcirc_buff *ptr, uint16_t *data, uint32_t msgID,
                     uint16_t DLC)
{
    int16_t i_count;

    i_count = ptr->i_head + 1;

    if (i_count >= ptr->i_maxlen)
    {
        i_count = 0;
    }

    if (i_count == ptr->i_tail)
        return false;  // buffer is full

    memcpy(ptr->canA_tzRxC_buffer[ptr->i_head].uiDataBuffer, data, 8);
    ptr->canA_tzRxC_buffer[ptr->i_head].u32_msgID = msgID;
    ptr->canA_tzRxC_buffer[ptr->i_head].i_DataLength = (uint16_t) DLC;
    ptr->i_head = i_count;

    return true;
}

/*=============================================================================
 @Function name : void can_fndequedata(void)
 @brief  function to Delete data from queue

 @param void
 @return void
 ============================================================================ */
bool can_fndequedata(can_tzcirc_buff *ptr, uint16_t *data, uint32_t *msgID,
                     uint16_t *DLC)
{
    int16_t i_count;

    if (ptr->i_head == ptr->i_tail)
    {
        return false; // buffer is empty
    }

    i_count = ptr->i_tail + 1;

    if (i_count >= ptr->i_maxlen)
    {
        i_count = 0;
    }

    memcpy(data, ptr->canA_tzRxC_buffer[ptr->i_tail].uiDataBuffer, 8);
    *msgID = ptr->canA_tzRxC_buffer[ptr->i_tail].u32_msgID;
    *DLC = ptr->canA_tzRxC_buffer[ptr->i_tail].i_DataLength;
    ptr->i_tail = i_count;

    return true;

}
/*=============================================================================
 @Function name : void CAN_fnRXevent(void)
 @brief  function to receive messages over CAN
 function called in (isr.c \cpu_timer0_isr\CANA_fnRXevent) 100usec
 @param void
 @return void
 ============================================================================ */
void CANA_fnRXevent(void)
{

    //any commands for AO/DO control
    if (CAN_IsMessageReceived(CANA_BASE, CAN_mMAILBOX_3))
    {
        CAN_readMessage(CANA_BASE, CAN_mMAILBOX_3, uirxMsgLPCIO); //LPC IO card

        can_fnEnquedata(&uiRxbufferLPCIO, uirxMsgLPCIO,
                        CanaRegs.CAN_IF2ARB.bit.ID,
                        CanaRegs.CAN_IF2MCTL.bit.DLC);
    }

    if (CAN_IsMessageReceived(CANA_BASE, CAN_mMAILBOX_4))
    {
        CAN_readMessage(CANA_BASE, CAN_mMAILBOX_4, uirxMsgLHCIO); //LHC IO card

        can_fnEnquedata(&uiRxbufferLHCIO, uirxMsgLHCIO,
                        CanaRegs.CAN_IF2ARB.bit.ID,
                        CanaRegs.CAN_IF2MCTL.bit.DLC);
    }

    if (CAN_IsMessageReceived(CANA_BASE, CAN_mMAILBOX_7))
    {
        CAN_readMessage(CANA_BASE, CAN_mMAILBOX_7, uirxMsgMP); //MP -> MS

        can_fnEnquedata(&uiRxbufferMP, uirxMsgMP, CanaRegs.CAN_IF2ARB.bit.ID,
                        CanaRegs.CAN_IF2MCTL.bit.DLC);
    }

}

/*=============================================================================
 @Function name : void CANA_fnTask(void)
 @brief  :function to receive messages over CAN
 function called in isr.c/ cpu_timer1_isr/CANA_fnTask @10msec
 @param :void
 @return :void
 ============================================================================ */
void CANA_fnTask(void)
{

    uint32_t ui32temp;
    /* ============================================================================ */
    //extracting msgID for individual messages of LPC
    while (can_fndequedata(&uiRxbufferLPCIO, uirxPrcsMsgLPCIO, &u32msgID1,
                           &uiDataLength1))
    {
        ui32temp = (u32msgID1 & 0x00F00F0F);
        CANA_tzIORegs.uiMsgtypeLPCIO =
                (uint16_t) ((ui32temp & 0x00F00000) >> 20);
        CANA_tzIORegs.uiUnitID = (uint16_t) ((ui32temp & 0x00000F00) >> 8);

        CANA_tzIORegs.uiNodeLPCIO = (uint16_t) (ui32temp & 0x0F);

        //processing received messages of LPC

        cana_fnmsgPrcsLPCIO(CANA_tzIORegs.uiMsgtypeLPCIO, uirxPrcsMsgLPCIO,
                            CANA_tzIORegs.uiNodeLPCIO);
    }
    /* ============================================================================ */
    //extracting msgID for individual messages of LHC
    while (can_fndequedata(&uiRxbufferLHCIO, uirxPrcsMsgLHCIO, &u32msgID2,
                           &uiDataLength2))
    {
        ui32temp = (u32msgID2 & 0x00F0000F);
        CANA_tzIORegs.uiMsgtypeLHCIO = (uint16_t) (ui32temp >> 20);
        CANA_tzIORegs.uiNodeLHCIO = (uint16_t) (ui32temp & 0x0F);

        cana_fnmsgPrcsLHCIO(CANA_tzIORegs.uiMsgtypeLHCIO, uirxPrcsMsgLHCIO,
                            CANA_tzIORegs.uiNodeLHCIO);
    }
    /* ============================================================================ */
    while (can_fndequedata(&uiRxbufferMP, uirxPrcsMsgMP, &u32msgID3,
                           &uiDataLength3))
    //processing received messages of MP
    {
        cana_fnmsgPrcsMP(uirxPrcsMsgMP);
    }

    /* ============================================================================ */

}
/*=============================================================================
 @Function name : void cana_fnmsgPrcsLPCIO(void)
 @brief : function to Process LPCIO messages over CAN
 function called in isr.c/ cpu_timer1_isr/CANA_fnTask/cana_fnmsgPrcsLPCIO @10msec
 @param :(uint16_t uiMsgtype, uint16_t *msgDataIO,
 uint16_t uiNodeType)
 @return: void
 ============================================================================ */
static void cana_fnmsgPrcsLPCIO(uint16_t uiMsgtype, uint16_t *msgDataIO,
                                uint16_t uiNodeType)

{
    if((uiMsgtype==IO_DI_MSGID)&&(uiNodeType==LPC_30))
    {
        if (CANA_tzIOtimers.RxCntLPC30 != msgDataIO[0])
                   {
                       CANA_tzIOtimers.RxCntLPC30 = msgDataIO[0];

                       if (msgDataIO[1] == msgDataIO[2])
                       {
                           CANA_tzLPCDI_IORegs[CANA_mLPC30_IO].all = msgDataIO[1];
                           CANA_tzLPCAIFlt_IORegs[CANA_mLPC30_IO].all = msgDataIO[5];

                           CANA_tzIOflags.btLPC30CommnStart = true;
                           CANA_tzIOflags.LPC30Comfail = 0;
                           CANA_tzIOtimers.LPC30ComFailCnt = 0;

                       }
                   }
    }
    if((uiMsgtype==IO_DI_MSGID)&&(uiNodeType==LPC_31))
    {
        if (CANA_tzIOtimers.RxCntLPC31 != msgDataIO[0])
        {
            CANA_tzIOtimers.RxCntLPC31 = msgDataIO[0];

            if (msgDataIO[1] == msgDataIO[2])
            {
                CANA_tzLPCDI_IORegs[CANA_mLPC31_IO].all = msgDataIO[1];
                CANA_tzLPCAIFlt_IORegs[CANA_mLPC31_IO].all = msgDataIO[5];

                CANA_tzIOflags.btLPC31CommnStart = true;
                CANA_tzIOflags.LPC31Comfail = 0;
                CANA_tzIOtimers.LPC31ComFailCnt = 0;
            }
        }
    }
    /*****************************************************************************************/
    if((uiMsgtype==IO_AIBLK1_MSGID)&&(uiNodeType==LPC_30))
    {
        CANA_tzAIData_IORegs[CANA_mLPC30_IO].AI0_Data = ((msgDataIO[0] << 8)
                | (msgDataIO[1])) * 0.001;
        CANA_tzAIData_IORegs[CANA_mLPC30_IO].AI1_Data = ((msgDataIO[2] << 8)
                | (msgDataIO[3])) * 0.001;
        CANA_tzAIData_IORegs[CANA_mLPC30_IO].AI2_Data = ((msgDataIO[4] << 8)
                | (msgDataIO[5])) * 0.001;
        CANA_tzAIData_IORegs[CANA_mLPC30_IO].AI3_Data = ((msgDataIO[6] << 8)
                | (msgDataIO[7])) * 0.001;
    }
    if((uiMsgtype==IO_AIBLK1_MSGID)&&(uiNodeType==LPC_31))
    {
        CANA_tzAIData_IORegs[CANA_mLPC31_IO].AI0_Data = ((msgDataIO[0] << 8)
                 | (msgDataIO[1])) * 0.001;
         CANA_tzAIData_IORegs[CANA_mLPC31_IO].AI1_Data = ((msgDataIO[2] << 8)
                 | (msgDataIO[3])) * 0.001;
         CANA_tzAIData_IORegs[CANA_mLPC31_IO].AI2_Data = ((msgDataIO[4] << 8)
                 | (msgDataIO[5])) * 0.001;
         CANA_tzAIData_IORegs[CANA_mLPC31_IO].AI3_Data = ((msgDataIO[6] << 8)
                 | (msgDataIO[7])) * 0.001;

         CANA_tzAISensorData.HYS_401 =
                 CANA_tzAIData_IORegs[CANA_mLPC31_IO].AI3_Data;
    }
    /*********************************************************************************************/
    if((uiMsgtype==IO_AIBLK2_MSGID)&&(uiNodeType==LPC_30))
    {
        CANA_tzAIData_IORegs[CANA_mLPC30_IO].AI4_Data = ((msgDataIO[0] << 8)
                | (msgDataIO[1])) * 0.001;
        CANA_tzAIData_IORegs[CANA_mLPC30_IO].AI5_Data = ((msgDataIO[2] << 8)
                | (msgDataIO[3])) * 0.001;
        CANA_tzAIData_IORegs[CANA_mLPC30_IO].AI6_Data = ((msgDataIO[4] << 8)
                | (msgDataIO[5])) * 0.001;
        CANA_tzAIData_IORegs[CANA_mLPC30_IO].AI7_Data = ((msgDataIO[6] << 8)
                | (msgDataIO[7])) * 0.001;
    }
    if((uiMsgtype==IO_AIBLK2_MSGID)&&(uiNodeType==LPC_31))
    {
        CANA_tzAIData_IORegs[CANA_mLPC31_IO].AI4_Data = ((msgDataIO[0] << 8)
                | (msgDataIO[1])) * 0.001;
        CANA_tzAIData_IORegs[CANA_mLPC31_IO].AI5_Data = ((msgDataIO[2] << 8)
                | (msgDataIO[3])) * 0.001;
        CANA_tzAIData_IORegs[CANA_mLPC31_IO].AI6_Data = ((msgDataIO[4] << 8)
                | (msgDataIO[5])) * 0.001;
        CANA_tzAIData_IORegs[CANA_mLPC31_IO].AI7_Data = ((msgDataIO[6] << 8)
                | (msgDataIO[7])) * 0.001;

        CANA_tzAISensorData.HYS_101 =
                CANA_tzAIData_IORegs[CANA_mLPC31_IO].AI4_Data;
        CANA_tzAISensorData.HYS_102 =
                CANA_tzAIData_IORegs[CANA_mLPC31_IO].AI5_Data;
        CANA_tzAISensorData.OXS_101 =
                CANA_tzAIData_IORegs[CANA_mLPC31_IO].AI6_Data;
        CANA_tzAISensorData.HYS_501 =
                CANA_tzAIData_IORegs[CANA_mLPC31_IO].AI7_Data;
    }
    /*********************************************************************************************/
    if((uiMsgtype==IO_DI1_FREQ_MSGID)&&(uiNodeType==LPC_30))
    {
        CANA_tzAIDataFreq_IORegs[CANA_mLPC31_IO].DI0_FreqData =
                ((msgDataIO[0] << 8) | (msgDataIO[1])) * 0.01;
        CANA_tzAIDataFreq_IORegs[CANA_mLPC31_IO].DI1_FreqData =
                ((msgDataIO[2] << 8) | (msgDataIO[3])) * 0.01;
        CANA_tzAIDataFreq_IORegs[CANA_mLPC31_IO].DI2_FreqData =
                ((msgDataIO[4] << 8) | (msgDataIO[5])) * 0.01;
        CANA_tzAIDataFreq_IORegs[CANA_mLPC31_IO].DI3_FreqData =
                ((msgDataIO[6] << 8) | (msgDataIO[7])) * 0.01;

        CANA_tzDISensorData.PURGE101 =
                CANA_tzAIDataFreq_IORegs[CANA_mLPC31_IO].DI0_FreqData;
        CANA_tzDISensorData.PURGE102 =
                CANA_tzAIDataFreq_IORegs[CANA_mLPC31_IO].DI1_FreqData;
        CANA_tzDISensorData.PURGE501 =
                CANA_tzAIDataFreq_IORegs[CANA_mLPC31_IO].DI2_FreqData;
        CANA_tzDISensorData.PURGE502 =
                CANA_tzAIDataFreq_IORegs[CANA_mLPC31_IO].DI3_FreqData;

    }
    /*********************************************************************************************/

    if((uiMsgtype==IO_DI2_FREQ_MSGID)&&(uiNodeType==LPC_31))
     {
        CANA_tzAIDataFreq_IORegs[CANA_mLPC31_IO].DI4_FreqData =
                ((msgDataIO[0] << 8) | (msgDataIO[1])) * 0.01;
        CANA_tzAIDataFreq_IORegs[CANA_mLPC31_IO].DI5_FreqData =
                ((msgDataIO[2] << 8) | (msgDataIO[3])) * 0.01;
        CANA_tzAIDataFreq_IORegs[CANA_mLPC31_IO].DI6_FreqData =
                ((msgDataIO[4] << 8) | (msgDataIO[5])) * 0.01;
        CANA_tzAIDataFreq_IORegs[CANA_mLPC31_IO].DI7_FreqData =
                ((msgDataIO[6] << 8) | (msgDataIO[7])) * 0.01;

        CANA_tzDISensorData.PURGE401 =
                CANA_tzAIDataFreq_IORegs[CANA_mLPC31_IO].DI4_FreqData;
        CANA_tzDISensorData.PURGE402 =
                CANA_tzAIDataFreq_IORegs[CANA_mLPC31_IO].DI5_FreqData;
     }

    /*-----------------------LPC 30 CAN FAIL DETECTION-----CAB-3 & IO - 0-----------------*/
    CANA_tzIOtimers.LPC30ComFailCnt++;
    if (CANA_tzIOtimers.LPC30ComFailCnt >= CANA_mCOM_FAILCNT)
    {
        CANA_tzIOtimers.LPC30ComFailCnt = CANA_mCOM_FAILCNT;
        CANA_tzIOflags.btLPC30CommnStart = false;
        CANA_tzIOflags.LPC30Comfail = 1;
    }
    /*-----------------------LPC 31 CAN FAIL DETECTION-----CAB-3 & IO - 1-----------------*/

    CANA_tzIOtimers.LPC31ComFailCnt++;
    if (CANA_tzIOtimers.LPC31ComFailCnt >= CANA_mCOM_FAILCNT)
    {
        CANA_tzIOtimers.LPC31ComFailCnt = CANA_mCOM_FAILCNT;
        CANA_tzIOflags.btLPC31CommnStart = false;
        CANA_tzIOflags.LPC31Comfail = 1;
    }
}

/*=============================================================================
 @Function name : void cana_fnmsgPrcsLHCIO(void)
 @brief : function to Process LPCIO messages over CAN
 function called in isr.c/ cpu_timer1_isr/CANA_fnTask/cana_fnmsgPrcsLHCIO @10msec
 @param :(uint16_t uiMsgtype, uint16_t *msgDataIO,
 uint16_t uiNodeType)

 @return: void
 ============================================================================ */
static void cana_fnmsgPrcsLHCIO(uint16_t uiMsgtype, uint16_t *msgDataLHCIO,
                                uint16_t uiNodeType)

{
    if((uiMsgtype==IO_DI_MSGID)&&(uiNodeType==LHC_10))
      {
        if (CANA_tzIOtimers.RxCntLHC10 != msgDataLHCIO[0])
        {
            CANA_tzIOtimers.RxCntLHC10 = msgDataLHCIO[0];

            if (msgDataLHCIO[1] == msgDataLHCIO[2])
            {
                CANA_tzLHCDI_IORegs[CANA_mLHC10_IO].all = msgDataLHCIO[1];
                CANA_tzIORegs.CJC[CANA_mLHC10_IO] = ((msgDataLHCIO[3] << 8)
                        | (msgDataLHCIO[4])) * 0.01;
                CANA_tzLHCAIFlt_IORegs[CANA_mLHC10_IO].all = msgDataLHCIO[5];
                CANA_tzIOflags.btLHC10CommnStart = true;
                CANA_tzIOflags.LHC10Comfail = 0;
                CANA_tzIOtimers.LHC10ComFailCnt = 0;

            }
        }
      }
    if((uiMsgtype==IO_DI_MSGID)&&(uiNodeType==LHC_11))
      {
        if (CANA_tzIOtimers.RxCntLHC11 != msgDataLHCIO[0])
        {
            CANA_tzIOtimers.RxCntLHC11 = msgDataLHCIO[0];

            if (msgDataLHCIO[1] == msgDataLHCIO[2])
            {
                CANA_tzLHCDI_IORegs[CANA_mLHC11_IO].all = msgDataLHCIO[1];
                CANA_tzLHCAIFlt_IORegs[CANA_mLHC11_IO].all = msgDataLHCIO[5];
                CANA_tzIOflags.btLHC11CommnStart = true;
                CANA_tzIOflags.LHC11Comfail = 0;
                CANA_tzIOtimers.LHC11ComFailCnt = 0;

            }
        }
      }
    /*****************************************************************************************/
    if((uiMsgtype==IO_AIBLK1_MSGID)&&(uiNodeType==LHC_10))
      {
        CANA_tzAIData_IORegs[CANA_mLHC10_IO].AI0_Data = ((msgDataLHCIO[0] << 8)
                 | (msgDataLHCIO[1])) * 0.001;
         CANA_tzAIData_IORegs[CANA_mLHC10_IO].AI1_Data = ((msgDataLHCIO[2] << 8)
                 | (msgDataLHCIO[3])) * 0.001;
         CANA_tzAIData_IORegs[CANA_mLHC10_IO].AI2_Data = ((msgDataLHCIO[4] << 8)
                 | (msgDataLHCIO[5])) * 0.001;
         CANA_tzAIData_IORegs[CANA_mLHC10_IO].AI3_Data = ((msgDataLHCIO[6] << 8)
                 | (msgDataLHCIO[7])) * 0.001;
      }
    if((uiMsgtype==IO_AIBLK1_MSGID)&&(uiNodeType==LHC_11))
      {
        CANA_tzAIData_IORegs[CANA_mLHC11_IO].AI0_Data = ((msgDataLHCIO[0] << 8)
                | (msgDataLHCIO[1])) * 0.001;
        CANA_tzAIData_IORegs[CANA_mLHC11_IO].AI1_Data = ((msgDataLHCIO[2] << 8)
                | (msgDataLHCIO[3])) * 0.001;
        CANA_tzAIData_IORegs[CANA_mLHC11_IO].AI2_Data = ((msgDataLHCIO[4] << 8)
                | (msgDataLHCIO[5])) * 0.001;
        CANA_tzAIData_IORegs[CANA_mLHC11_IO].AI3_Data = ((msgDataLHCIO[6] << 8)
                | (msgDataLHCIO[7])) * 0.001;

        CANA_tzAISensorData.PRT_402 =
                CANA_tzAIData_IORegs[CANA_mLHC11_IO].AI3_Data;
      }
    /*****************************************************************************************/
    if((uiMsgtype==IO_AIBLK2_MSGID)&&(uiNodeType==LHC_10))
      {
        CANA_tzAIData_IORegs[CANA_mLHC10_IO].AI4_Data = ((msgDataLHCIO[0] << 8)
                | (msgDataLHCIO[1])) * 0.001;
        CANA_tzAIData_IORegs[CANA_mLHC10_IO].AI5_Data = ((msgDataLHCIO[2] << 8)
                | (msgDataLHCIO[3])) * 0.001;
        CANA_tzAIData_IORegs[CANA_mLHC10_IO].AI6_Data = ((msgDataLHCIO[4] << 8)
                | (msgDataLHCIO[5])) * 0.001;
        CANA_tzAIData_IORegs[CANA_mLHC10_IO].AI7_Data = ((msgDataLHCIO[6] << 8)
                | (msgDataLHCIO[7])) * 0.001;

        CANA_tzAISensorData.LVL_101 =
                CANA_tzAIData_IORegs[CANA_mLHC10_IO].AI4_Data;
        CANA_tzAISensorData.PRT_101 =
                CANA_tzAIData_IORegs[CANA_mLHC10_IO].AI5_Data;
        CANA_tzAISensorData.PRT_102 =
                CANA_tzAIData_IORegs[CANA_mLHC10_IO].AI6_Data;
        CANA_tzAISensorData.COS_101 =
                CANA_tzAIData_IORegs[CANA_mLHC10_IO].AI7_Data;
      }
    if((uiMsgtype==IO_AIBLK2_MSGID)&&(uiNodeType==LHC_11))
       {
        CANA_tzAIData_IORegs[CANA_mLHC11_IO].AI4_Data = ((msgDataLHCIO[0] << 8)
                 | (msgDataLHCIO[1])) * 0.001;
         CANA_tzAIData_IORegs[CANA_mLHC11_IO].AI5_Data = ((msgDataLHCIO[2] << 8)
                 | (msgDataLHCIO[3])) * 0.001;
         CANA_tzAIData_IORegs[CANA_mLHC11_IO].AI6_Data = ((msgDataLHCIO[4] << 8)
                 | (msgDataLHCIO[5])) * 0.001;
         CANA_tzAIData_IORegs[CANA_mLHC11_IO].AI7_Data = ((msgDataLHCIO[6] << 8)
                 | (msgDataLHCIO[7])) * 0.001;

         CANA_tzAISensorData.PRT_401 =
                 CANA_tzAIData_IORegs[CANA_mLHC11_IO].AI5_Data;
         CANA_tzAISensorData.TE_401 =
                 CANA_tzAIData_IORegs[CANA_mLHC11_IO].AI6_Data;
         CANA_tzAISensorData.DPT_401 =
                 CANA_tzAIData_IORegs[CANA_mLHC11_IO].AI7_Data;

       }
    /*****************************************************************************************/

    if((uiMsgtype==IO_THERMAL_MSGID)&&(uiNodeType==LHC_10))
        {
            CANA_tzThermal_IORegs[CANA_mLHC10_IO].T0_Data = ((msgDataLHCIO[0] << 8)
                    | (msgDataLHCIO[1])) * 0.001;
            CANA_tzThermal_IORegs[CANA_mLHC10_IO].T1_Data = ((msgDataLHCIO[2] << 8)
                    | (msgDataLHCIO[3])) * 0.001;
            CANA_tzThermal_IORegs[CANA_mLHC10_IO].T2_Data = ((msgDataLHCIO[4] << 8)
                    | (msgDataLHCIO[5])) * 0.001;
            CANA_tzThermal_IORegs[CANA_mLHC10_IO].T3_Data = ((msgDataLHCIO[6] << 8)
                    | (msgDataLHCIO[7])) * 0.001;

            CANA_tzThermoCoupleData.TTC_101 =
                    CANA_tzThermal_IORegs[CANA_mLHC10_IO].T0_Data;
            CANA_tzThermoCoupleData.TTC_102 =
                    CANA_tzThermal_IORegs[CANA_mLHC10_IO].T1_Data;
            CANA_tzThermoCoupleData.TTC_301 =
                    CANA_tzThermal_IORegs[CANA_mLHC10_IO].T2_Data;
            CANA_tzThermoCoupleData.KTC_401 =
                    CANA_tzThermal_IORegs[CANA_mLHC10_IO].T3_Data;
        }

    /*--------------------------LHC 10 CAN FAIL DETECTION--CAB-1 & IO - 0-------------------------*/
    CANA_tzIOtimers.LHC10ComFailCnt++;
    if (CANA_tzIOtimers.LHC10ComFailCnt >= CANA_mCOM_FAILCNT)
    {
        CANA_tzIOtimers.LHC10ComFailCnt = CANA_mCOM_FAILCNT;
        CANA_tzIOflags.btLHC10CommnStart = false;
        CANA_tzIOflags.LHC10Comfail = 1;
    }
    /*--------------------------LHC 11 CAN FAIL DETECTION---CAB-1 & IO - 1------------------------*/
    CANA_tzIOtimers.LHC11ComFailCnt++;
    if (CANA_tzIOtimers.LHC11ComFailCnt >= CANA_mCOM_FAILCNT)
    {
        CANA_tzIOtimers.LHC11ComFailCnt = CANA_mCOM_FAILCNT;
        CANA_tzIOflags.btLHC11CommnStart = false;
        CANA_tzIOflags.LHC11Comfail = 1;
    }
}

/*=============================================================================
 @Function name : void cana_fnmsgPrcsMP(uint16_t *msgDataMP)
 @brief : function to Process LPCIO messages over CAN
 function called in isr.c/ cpu_timer1_isr/CANA_fnTask/cana_fnmsgPrcsMP @10msec
 @param :(uint16_t *msgDataMP)
 @return: void
 ============================================================================ */
static void cana_fnmsgPrcsMP(uint16_t *msgDataMP)
{
   if (CANA_tzMPRegs.RxCntMP != msgDataMP[0])
    {
        CANA_tzMPRegs.RxCntMP = msgDataMP[0];
        CANA_tzMPRegs.PresentStMP = msgDataMP[1];
        CANA_tzMPRegs.DOCmd = msgDataMP[2];
        uiCabIDDO = msgDataMP[3];
        uiNodeIDDO = msgDataMP[4];
        CANA_tzMPRegs.Hyd_Genstart = msgDataMP[5];
        CANA_tzLHCIO1_AIFaultRegs.all = msgDataMP[6];
        CANA_tzLHCIO2_AIFaultRegs.all = msgDataMP[7];
        CANA_tzMPRegs.MPComfail = 0;
        CANA_tzMPRegs.btMPComStart = true;
        CANA_tzMPRegs.MPComFailCnt = 0;

    }
    /*----------------------- MP FAIL DETECTION-- MP->MS-----------------------*/

    CANA_tzMPRegs.MPComFailCnt++;
    if (CANA_tzMPRegs.MPComFailCnt >= CANA_mCOM_FAILCNT)
    {
        CANA_tzMPRegs.btMPComStart = false;
        CANA_tzMPRegs.MPComfail = 1;
        CANA_tzMPRegs.MPComFailCnt = CANA_mCOM_FAILCNT;
    }
    /*-----------------------------------------------------------------------**/
    switch (CANA_tzMPRegs.PresentStMP)
    {
    case 0:
        STAT_tzStateMacMP.Present_st = MP_STANDBY;
        break;
    case 1:
        STAT_tzStateMacMP.Present_st = MP_READY;
        break;
    case 2:
        STAT_tzStateMacMP.Present_st = MP_STACKCHECK;
        break;
    case 3:
        STAT_tzStateMacMP.Present_st = MP_STACKPOWER;
        break;
    case 4:
        STAT_tzStateMacMP.Present_st = MP_FAULT;
        break;
    case 5:
        STAT_tzStateMacMP.Present_st = MP_SAFE_SHUT_DOWN;
        break;
    case 6:
        STAT_tzStateMacMP.Present_st = MP_COMMISSION;
        break;

    default:
        break;
    }
}
/*=============================================================================
 @Function name : void CANA_fnTx(void)
 @brief  :function to Process Master Safety to MP messages over CAN
 (scheduler.c \ SCH_fnslot_1\CANA_fnTx) 50msec
 Purge and contactor logics are called related commands are send to IOcard
 @param :void
 @return :void
 ============================================================================ */
void CANA_fnTx(void)
{
    /***************************************fn called in every 150ms******3*50ms = 150ms***********************/

    ++uiMPtoMS_Rxcnt;
    if (uiMPtoMS_Rxcnt > 2)
    {
        uiMPtoMS_Rxcnt = 0;
    }
    if (uiMPtoMS_Rxcnt == 1)
    {
        CANA_fnMPTxCmds(CANA_mLPC_CABID, CANA_mLPC30_IO,
                        &CANA_tzAO[0][CANA_mLPC30_IO]); //MS ->MP .. MP will consider CNT,State

        CANA_fnMPRxCmdsProcess(uiCabIDDO, uiNodeIDDO, &CANA_tzMPRegs); //fn called in every 110ms

    }
    if (uiMPtoMS_Rxcnt == 2)
    {
        CANA_fnMPTxDOCmds(); //MS ->MP .. digital output status
        uiMPtoMS_Rxcnt = 0;

    }
    /******************************************************************************************/
    switch (STAT_tzStateMac.Present_st)
    // Common Messages Irrespective of States
    {

    case STAND_BY:
        CAN_fnTurnoffIO();
        break;

    case PURGE:
        purge_fnFan();
        Monitor_fnFeedbacks();
        break;

    case IOPOWER:
        LHC_fnPower();
        break;

    case ARMED_POWER:
        LPC_fnContactor();
        break;

    case FAULT:
        //CAN_fnTurnoffIO();
        break;

    case SHUT_DOWN:
        //  CAN_fnTurnoffIO();
        break;

    default:
        break;
    }
}
/*=============================================================================
 @Function name : void CANA_fnCmdsForDigOPs(uint16_t ui16unitID, uint16_t ui16cab_ID,
 uint16_t ui16nodeID, CANA_tzDIG_OP *ptrDigOP)
 @brief  :function for giving Digital outputs commands to related IOcard
 @param void
 @return void
 ============================================================================ */
void CANA_fnCmdsForDigOPs(uint16_t ui16unitID, uint16_t ui16cab_ID,
                          uint16_t ui16nodeID, CANA_tzDIG_OP *ptrDigOP)
{
    CAN_setupMessageObject(
            CANA_BASE,
            CAN_mMAILBOX_8,
            (ui16unitID << 8) | (ui16cab_ID << 4) | (ui16nodeID)
                    | CANA_mTX_IOMSGID1,
            CAN_MSG_FRAME_EXT, CAN_MSG_OBJ_TYPE_TX, 0,
            CAN_MSG_OBJ_NO_FLAGS,
            CAN_mLEN8);

    CANA_tzIOtimers.TxCntIOCom++;

    if (ui16cab_ID == 3)
    {
        ui16CabID = 0; // filling LPC Cabinet array
    }
    else if (ui16cab_ID == 1)
    {
        ui16CabID = 1; // filling LHC Cabinet array
    }

    ui16txMsgDataIO[0] = CANA_tzIOtimers.TxCntIOCom;
    ui16txMsgDataIO[1] = ptrDigOP->all;
    ui16txMsgDataIO[2] = ptrDigOP->all;

    if (CANA_tzIOtimers.TxCntIOCom == 255)
    {
        CANA_tzIOtimers.TxCntIOCom = 0;
    }

    CAN_sendMessage(CANA_BASE, CAN_mMAILBOX_8, CAN_mLEN8, ui16txMsgDataIO);

}
/*=============================================================================
 @Function name : void CANA_fnCmdsForAnaOPVs(uint16_t ui16unitID, uint16_t ui16cab_ID,
 uint16_t ui16nodeID, can_tzAnaOPParams *ptrAOV)
 @brief  function for giving analog voltage output commands to related IOcard
 @param void
 @return void
 ============================================================================ */
void CANA_fnCmdsForAnaOPVs(uint16_t ui16unitID, uint16_t ui16cab_ID,
                           uint16_t ui16nodeID, can_tzAnaOPParams *ptrAOV)
{

    CAN_setupMessageObject(
            CANA_BASE,
            CAN_mMAILBOX_8,
            (ui16unitID << 8) | (ui16cab_ID << 4) | (ui16nodeID)
                    | CANA_mTX_IOMSGID3,
            CAN_MSG_FRAME_EXT, CAN_MSG_OBJ_TYPE_TX, 0,
            CAN_MSG_OBJ_NO_FLAGS,
            CAN_mLEN8);

    if (ui16cab_ID == 3)
    {
        ui16CabID = 0; // filling LPC Cabinet array
    }
    else if (ui16cab_ID == 1)
    {
        ui16CabID = 1; // filling LHC Cabinet array
    }

    ui16txMsgDataIO[0] = (ptrAOV->CANA_tzAOV[ui16CabID][ui16nodeID].AOV1) >> 8;
    ui16txMsgDataIO[1] = (ptrAOV->CANA_tzAOV[ui16CabID][ui16nodeID].AOV1);
    ui16txMsgDataIO[2] = (ptrAOV->CANA_tzAOV[ui16CabID][ui16nodeID].AOV2) >> 8;
    ui16txMsgDataIO[3] = (ptrAOV->CANA_tzAOV[ui16CabID][ui16nodeID].AOV2);
    ui16txMsgDataIO[4] = (ptrAOV->CANA_tzAOV[ui16CabID][ui16nodeID].AOV3) >> 8;
    ui16txMsgDataIO[5] = (ptrAOV->CANA_tzAOV[ui16CabID][ui16nodeID].AOV3);
    ui16txMsgDataIO[6] = (ptrAOV->CANA_tzAOV[ui16CabID][ui16nodeID].AOV4) >> 8;
    ui16txMsgDataIO[7] = (ptrAOV->CANA_tzAOV[ui16CabID][ui16nodeID].AOV4);

    CAN_sendMessage(CANA_BASE, CAN_mMAILBOX_8, CAN_mLEN8, ui16txMsgDataIO);
}
/*=============================================================================
 @Function name : void CANA_fnCmdsForAnaOPIs(uint16_t ui16unitID, uint16_t ui16cab_ID,
 uint16_t ui16nodeID, can_tzAnaOPParams *ptrAOI)
 @brief : function for giving analog current output commands to related IOcard
 @param :(uint16_t ui16unitID, uint16_t ui16cab_ID,
 uint16_t ui16nodeID, can_tzAnaOPParams *ptrAOI)
 @return :void
 ============================================================================ */
void CANA_fnCmdsForAnaOPIs(uint16_t ui16unitID, uint16_t ui16cab_ID,
                           uint16_t ui16nodeID, can_tzAnaOPParams *ptrAOI)
{

    CAN_setupMessageObject(
            CANA_BASE,
            CAN_mMAILBOX_8,
            (ui16unitID << 8) | (ui16cab_ID << 4) | (ui16nodeID)
                    | CANA_mTX_IOMSGID2,
            CAN_MSG_FRAME_EXT, CAN_MSG_OBJ_TYPE_TX, 0,
            CAN_MSG_OBJ_NO_FLAGS,
            CAN_mLEN8);

    if (ui16cab_ID == 3)
    {
        ui16CabID = 0; // filling LPC Cabinet array
    }
    else if (ui16cab_ID == 1)
    {
        ui16CabID = 1; // filling LHC Cabinet array
    }

    ui16txMsgDataIO[0] = (ptrAOI->CANA_tzAOI[ui16CabID][ui16nodeID].AOI1) >> 8;
    ui16txMsgDataIO[1] = (ptrAOI->CANA_tzAOI[ui16CabID][ui16nodeID].AOI1);
    ui16txMsgDataIO[2] = (ptrAOI->CANA_tzAOI[ui16CabID][ui16nodeID].AOI2) >> 8;
    ui16txMsgDataIO[3] = (ptrAOI->CANA_tzAOI[ui16CabID][ui16nodeID].AOI2);
    ui16txMsgDataIO[4] = (ptrAOI->CANA_tzAOI[ui16CabID][ui16nodeID].AOI3) >> 8;
    ui16txMsgDataIO[5] = (ptrAOI->CANA_tzAOI[ui16CabID][ui16nodeID].AOI3);
    ui16txMsgDataIO[6] = (ptrAOI->CANA_tzAOI[ui16CabID][ui16nodeID].AOI4) >> 8;
    ui16txMsgDataIO[7] = (ptrAOI->CANA_tzAOI[ui16CabID][ui16nodeID].AOI4);

    CAN_sendMessage(CANA_BASE, CAN_mMAILBOX_8, CAN_mLEN8, ui16txMsgDataIO);
}

/*=============================================================================
 @Function name : void CANA_fnMPTxCmds(uint16_t ui16CabiD, uint16_t NodeID,
 CANA_tzANG_OP *ptrDigAOP)
 @brief : function for sending the counter and states, commands to MS to MP
 @param :(uint16_t ui16CabiD, uint16_t NodeID,
 CANA_tzANG_OP *ptrDigAOP)
 @return :void
 ============================================================================ */

void CANA_fnMPTxCmds(uint16_t ui16CabiD, uint16_t NodeID,
                     CANA_tzANG_OP *ptrDigAOP)
{

    // Master Safety to Master Process

    CAN_setupMessageObject(
    CANA_BASE,
                           CAN_mMAILBOX_9,
                           CANA_mTX_MPMSGID1,
                           CAN_MSG_FRAME_EXT, CAN_MSG_OBJ_TYPE_TX, 0,
                           CAN_MSG_OBJ_NO_FLAGS,
                           CAN_mLEN8);

    CANA_tzMPRegs.TxCntMP++;
    if (CANA_tzMPRegs.TxCntMP >= 255)
    {
        CANA_tzMPRegs.TxCntMP = 0;
    }

    uiCANtxMsgDataMP[0] = CANA_tzMPRegs.TxCntMP;

    uiCANtxMsgDataMP[1] = CANA_tzMPRegs.ReadyCmd;

    uiCANtxMsgDataMP[2] = STAT_tzStateMac.Present_st;

    uiCANtxMsgDataMP[3] = ptrDigAOP->all;

    uiCANtxMsgDataMP[4] = MPFAULTS_tzMSMPRegs.all;

    uiCANtxMsgDataMP[5] = CANA_tzAO_VAL[0][CANA_mLPC30_IO].value.AVO1;

    uiCANtxMsgDataMP[6] = CANA_tzAO_VAL[0][CANA_mLPC30_IO].value.AVO2;
    uiCANtxMsgDataMP[7] = CANA_tzAO_VAL[0][CANA_mLPC30_IO].value.AVO3;

    CAN_sendMessage(CANA_BASE, CAN_mMAILBOX_9, CAN_mLEN8, uiCANtxMsgDataMP);

}
/*=============================================================================
 @Function name : void CANA_fnMPTxDOCmds(void)
 @brief : function for sending the digital output status to MS to MP
 @param : void
 @return :void
 ============================================================================ */

void CANA_fnMPTxDOCmds(void)
{

    // Master Safety to Master Process

    CAN_setupMessageObject(
    CANA_BASE,
                           CAN_mMAILBOX_9,
                           CANA_mTX_MPMSGID2,
                           CAN_MSG_FRAME_EXT, CAN_MSG_OBJ_TYPE_TX, 0,
                           CAN_MSG_OBJ_NO_FLAGS,
                           CAN_mLEN8);

    uiCANtxMsgDataMP1[0] = DOSTATUS_tzMSMPRegs.all;

    CAN_sendMessage(CANA_BASE, CAN_mMAILBOX_9, CAN_mLEN8, uiCANtxMsgDataMP1);

}

/*=============================================================================
 @Function name : void CANA_fnIOHrtBt(void)
 @brief : function for sending the counter to IO card & MP for Heart beat verification
 @param :void
 @return :void
 ============================================================================ */
void CANA_fnIOHrtBt(void)
{
    CANA_tzIOtimers.HrtbtCntIOCom++;

    switch (CANA_tzIOtimers.HrtbtCntIOCom)
    {
    case 1:

        CANA_fnCmdsForDigOPs(CANA_tzIORegs.uiUnitID, 3, 0, &CANA_tzDO[0][0]); //LPCIO-1
        break;

    case 2:

        CANA_fnCmdsForDigOPs(CANA_tzIORegs.uiUnitID, 3, 1, &CANA_tzDO[0][1]); //LPCIO-2
        break;

    case 3:

        CANA_fnCmdsForDigOPs(CANA_tzIORegs.uiUnitID, 1, 0, &CANA_tzDO[1][0]); //LHCIO-1
        break;

    case 4:

        CANA_fnCmdsForDigOPs(CANA_tzIORegs.uiUnitID, 1, 1, &CANA_tzDO[1][1]); //LHCIO-2
        //  CANA_tzIOtimers.HrtbtCntIOCom = 0;

        break;

    case 5:
        CANA_fnMPTxCmds(CANA_mLPC_CABID, CANA_mLPC30_IO,
                        &CANA_tzAO[0][CANA_mLPC30_IO]);  //MS ->MP
        CANA_tzIOtimers.HrtbtCntIOCom = 0;

        break;

    default:
        break;
    }
}
/*=============================================================================
 @Function name : static void CANA_fnMPRxCmdsProcess(uint16_t uiCabIDDO,
 uint16_t uiNodeIDDO, CANA_tzMPREGS *CANA_tzMPRegs)
 @brief : function to Process the Digital outputs which received from MP.

 @param :(uint16_t uiCabIDDO, uint16_t uiNodeIDDO, CANA_tzMPREGS *CANA_tzMPRegs)
 @return :void
 ============================================================================ */
static void CANA_fnMPRxCmdsProcess(uint16_t uiCabIDDO, uint16_t uiNodeIDDO,
                                   CANA_tzMPREGS *CANA_tzMPRegs)
{

    if((uiCabIDDO == 1)&&(uiNodeIDDO==0))
    {
        if ((CANA_tzMPRegs->DOCmd & 0x0001) == 0x0001)
         {
             CANA_tzMPRegs->TurnON_WSV = 1;
             CANA_tzDO[1][0].bit.DO0 = 0x1;
             CANA_fnCmdsForDigOPs(CANA_tzIORegs.uiUnitID, 1,
                                  0,
                                  &CANA_tzDO[1][0]); // Turn ON WSV

         }
         else if ((CANA_tzMPRegs->DOCmd & 0x0001) == 0x0000)
         {
             CANA_tzMPRegs->TurnON_WSV = 0;
             CANA_tzDO[1][0].bit.DO0 = 0x0;
                CANA_fnCmdsForDigOPs(CANA_tzIORegs.uiUnitID, 1,
                                     0,
                                    &CANA_tzDO[1][0]); // Turn OFF WSV

         }
        if ((CANA_tzMPRegs->DOCmd & 0x0002) == 0x0002)
        {
            CANA_tzMPRegs->TurnON_SV401 = 1;
            CANA_tzDO[1][0].bit.DO1 = 0x1;
            CANA_fnCmdsForDigOPs(CANA_tzIORegs.uiUnitID, 1,
                                 0,
                                 &CANA_tzDO[1][0]); // Turn ON SV401

        }
        else if ((CANA_tzMPRegs->DOCmd & 0x0002) == 0x0000)
        {
            CANA_tzMPRegs->TurnON_SV401 = 0;
            CANA_tzDO[1][0].bit.DO1 = 0x0;
            CANA_fnCmdsForDigOPs(CANA_tzIORegs.uiUnitID, 1,
                                 0,
                                 &CANA_tzDO[1][0]); // Turn OFF SV401

        }
        if ((CANA_tzMPRegs->DOCmd & 0x0004) == 0x0004)
        {
            CANA_tzMPRegs->TurnON_SV402 = 1;
            CANA_tzDO[1][0].bit.DO2 = 0x1;
            CANA_fnCmdsForDigOPs(CANA_tzIORegs.uiUnitID, 1,
                                 0,
                                 &CANA_tzDO[1][0]); // Turn ON SV402

        }
        else if ((CANA_tzMPRegs->DOCmd & 0x0004) == 0x0000)
        {
            CANA_tzMPRegs->TurnON_SV402 = 0;
            CANA_tzDO[1][0].bit.DO2 = 0x0;
            CANA_fnCmdsForDigOPs(CANA_tzIORegs.uiUnitID, 1,
                                 0,
                                 &CANA_tzDO[1][0]); // Turn OFF SV402

        }

    }
    /*********************************************************************************/
    if((uiCabIDDO == 1)&&(uiNodeIDDO==1))
     {
        if ((CANA_tzMPRegs->DOCmd & 0x0001) == 0x0001)
        {
            CANA_tzMPRegs->TurnON_SV1 = 1;
            CANA_tzDO[1][1].bit.DO0 = 0x1;
            CANA_fnCmdsForDigOPs(CANA_tzIORegs.uiUnitID, 1,
                                 1,
                                 &CANA_tzDO[1][1]); // Turn ON SV1
        }
        else if ((CANA_tzMPRegs->DOCmd & 0x0001) == 0x0000)
        {
            CANA_tzMPRegs->TurnON_SV1 = 0;
            CANA_tzDO[1][1].bit.DO0 = 0x0;
            CANA_fnCmdsForDigOPs(CANA_tzIORegs.uiUnitID, 1,
                                 1,
                                 &CANA_tzDO[1][1]); // Turn OFF SV1
        }
        /********************************************************************************/
        if ((CANA_tzMPRegs->DOCmd & 0x0002) == 0x0002)
        {
            CANA_tzMPRegs->TurnON_SV2 = 1;
            CANA_tzDO[1][1].bit.DO1 = 0x1;
            CANA_fnCmdsForDigOPs(CANA_tzIORegs.uiUnitID, 1,
                                 1,
                                 &CANA_tzDO[1][1]); // Turn ON SV2
        }
        else if ((CANA_tzMPRegs->DOCmd & 0x0002) == 0x0000)
        {
            CANA_tzMPRegs->TurnON_SV2 = 0;
            CANA_tzDO[1][1].bit.DO1 = 0x0;
            CANA_fnCmdsForDigOPs(CANA_tzIORegs.uiUnitID, 1,
                                 1,
                                 &CANA_tzDO[1][1]); // Turn OFF SV2
        }
        /********************************************************************************/

        if ((CANA_tzMPRegs->DOCmd & 0x0004) == 0x0004)
        {
            CANA_tzMPRegs->TurnON_SV3 = 1;
            CANA_tzDO[1][1].bit.DO2 = 0x1;
            CANA_fnCmdsForDigOPs(CANA_tzIORegs.uiUnitID, 1,
                                 1,
                                 &CANA_tzDO[1][1]); // Turn ON SV3
        }
        else if ((CANA_tzMPRegs->DOCmd & 0x0004) == 0x0000)
        {
            CANA_tzMPRegs->TurnON_SV3 = 0;
            CANA_tzDO[1][1].bit.DO2 = 0x0;
            CANA_fnCmdsForDigOPs(CANA_tzIORegs.uiUnitID, 1,
                                 1,
                                 &CANA_tzDO[1][1]); // Turn OFF SV3
        }
        /********************************************************************************/

        if ((CANA_tzMPRegs->DOCmd & 0x0008) == 0x0008)
        {
            CANA_tzMPRegs->TurnON_SV4 = 1;
            CANA_tzDO[1][1].bit.DO3 = 0x1;
            CANA_fnCmdsForDigOPs(CANA_tzIORegs.uiUnitID, 1,
                                 1,
                                 &CANA_tzDO[1][1]); // Turn ON SV4
        }
        else if ((CANA_tzMPRegs->DOCmd & 0x0008) == 0x0000)
        {
            CANA_tzMPRegs->TurnON_SV4 = 0;
            CANA_tzDO[1][1].bit.DO3 = 0x0;
            CANA_fnCmdsForDigOPs(CANA_tzIORegs.uiUnitID, 1,
                                 1,
                                 &CANA_tzDO[1][1]); // Turn OFF SV4
        }
        /********************************************************************************/

        if ((CANA_tzMPRegs->DOCmd & 0x0010) == 0x0010)
        {
            CANA_tzMPRegs->TurnON_SV5 = 1;
            CANA_tzDO[1][1].bit.DO4 = 0x1;
            CANA_fnCmdsForDigOPs(CANA_tzIORegs.uiUnitID, 1,
                                 1,
                                 &CANA_tzDO[1][1]); // Turn ON SV5
        }
        else if ((CANA_tzMPRegs->DOCmd & 0x0010) == 0x0000)
        {
            CANA_tzMPRegs->TurnON_SV5 = 0;
            CANA_tzDO[1][1].bit.DO4 = 0x0;
            CANA_fnCmdsForDigOPs(CANA_tzIORegs.uiUnitID, 1,
                                 1,
                                 &CANA_tzDO[1][1]); // Turn OFF SV5
        }
        /********************************************************************************/

        if ((CANA_tzMPRegs->DOCmd & 0x0020) == 0x0020)
        {
            CANA_tzMPRegs->TurnON_SV6 = 1;
            CANA_tzDO[1][1].bit.DO5 = 0x1;
            CANA_fnCmdsForDigOPs(CANA_tzIORegs.uiUnitID, 1,
                                 1,
                                 &CANA_tzDO[1][1]); // Turn ON SV6
        }
        else if ((CANA_tzMPRegs->DOCmd & 0x0020) == 0x0000)
        {
            CANA_tzMPRegs->TurnON_SV6 = 0;
            CANA_tzDO[1][1].bit.DO5 = 0x0;
            CANA_fnCmdsForDigOPs(CANA_tzIORegs.uiUnitID, 1,
                                 1,
                                 &CANA_tzDO[1][1]); // Turn OFF SV6
        }
        /********************************************************************************/

        if ((CANA_tzMPRegs->DOCmd & 0x0040) == 0x0040)
        {
            CANA_tzMPRegs->TurnON_SV7 = 1;
            CANA_tzDO[1][1].bit.DO6 = 0x1;
            CANA_fnCmdsForDigOPs(CANA_tzIORegs.uiUnitID, 1,
                                 1,
                                 &CANA_tzDO[1][1]); // Turn ON SV7
        }
        else if ((CANA_tzMPRegs->DOCmd & 0x0040) == 0x0000)
        {
            CANA_tzMPRegs->TurnON_SV7 = 0;
            CANA_tzDO[1][1].bit.DO6 = 0x0;
            CANA_fnCmdsForDigOPs(CANA_tzIORegs.uiUnitID, 1,
                                 1,
                                 &CANA_tzDO[1][1]); // Turn OFF SV7
        }
     }
/*----------------------------------------------------------------------------------*/
    if((uiCabIDDO == 3)&&(uiNodeIDDO==1))
      {
        if ((CANA_tzMPRegs->DOCmd & 0x000C) == 0x000C)
        {
            CANA_tzMPRegs->TurnON_VFDCONT = 1;
            CANA_tzDO[0][1].bit.DO2 = 0x1;
            CANA_tzDO[0][1].bit.DO3 = 0x1;

            CANA_fnCmdsForDigOPs(CANA_tzIORegs.uiUnitID, 3,
                                 1,
                                 &CANA_tzDO[0][1]); // Turn ON VFD CONTACTOR

        }
        else if ((CANA_tzMPRegs->DOCmd & 0x000C) == 0x0000)
        {
            CANA_tzMPRegs->TurnON_VFDCONT = 0;
            CANA_tzDO[0][1].bit.DO2 = 0x0;
            CANA_tzDO[0][1].bit.DO3 = 0x0;
            CANA_fnCmdsForDigOPs(CANA_tzIORegs.uiUnitID, 3,
                                 1,
                                 &CANA_tzDO[0][1]); // Turn OFF VFD CONTACTOR

        }
     }
}
/*=============================================================================
 @Function name : void CAN_fnTurnoffIO(void)
 @brief  function to receive messages over CAN
 function called in fault & standby states
 @param void
 @return void
 ============================================================================ */
void CAN_fnTurnoffIO(void)
{
    CANA_tzDO[0][CANA_mLPC30_IO].all = 0x00; //PURGE FANS
    CANA_tzDO[0][CANA_mLPC31_IO].all = 0x00; //CTR101 PSU CONT,CTR102 VFD CONT
    CANA_tzDO[1][0].all = 0x00;  // WSV,SV401,SV402,CTR101
    CANA_tzDO[1][1].all = 0x00;  //DRYER VALVES
    CANA_fnCmdsForDigOPs(CANA_tzIORegs.uiUnitID, 3, 0, &CANA_tzDO[0][0]); //LPCIO-1
    CANA_fnCmdsForDigOPs(CANA_tzIORegs.uiUnitID, 3, 1, &CANA_tzDO[0][1]); //LPCIO-2
    CANA_fnCmdsForDigOPs(CANA_tzIORegs.uiUnitID, 1, 0, &CANA_tzDO[1][0]); //LHCIO-1
    CANA_fnCmdsForDigOPs(CANA_tzIORegs.uiUnitID, 1, 1, &CANA_tzDO[1][1]); //LHCIO-2
}

/*==============================================================================
 End of File
 ==============================================================================*/

