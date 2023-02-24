/*=============================================================================
 Copyright Enarka India Pvt Ltd (EIPL) All Rights Reserved.
 All trademarks are owned by Enarka India Private Limited
 ============================================================================ */

/*==============================================================================
 @file  state_machine.c
 @author JOTHI RAMESH
 @date 06-Sep-2021

 @brief Description
 ==============================================================================*/

/*==============================================================================
 Includes
 ==============================================================================*/

#include "F28x_Project.h"              // Standard libraries headers
#include "F2837xS_Device.h"
#include "math.h"
#include "stdlib.h"
#include "inc/hw_types.h"
#include "inc/hw_memmap.h"
#include "inc/hw_can.h"
#include "hal/driverlib/can.h"
#include "cana_defs.h"
#include "canb_defs.h"
#include "control_defs.h"
#include "state_machine.h"
#include "scheduler.h"
#include "faultcheck.h"
/*==============================================================================
 Defines
 ==============================================================================*/

/*==============================================================================
 Enums
 ==============================================================================*/

/*==============================================================================
 Structures
 ==============================================================================*/

/*==============================================================================
 Macros
 ==============================================================================*/

/*==============================================================================
 Local Function Prototypes
 ==============================================================================*/

void STAT_fnFSMCheck(void);

static void stat_fnFSMNextState(void);
void stat_IOReset(void);
/*==============================================================================
 Local Variables
 ==============================================================================*/
STAT_tzSTATEMAC STAT_tzStateMac;
STAT_tzSTATE_MPMAC STAT_tzStateMacMP;
/*==============================================================================
 Local Constants
 ==============================================================================*/

void stat_fnInitState(void)
{
    // Global Flags

    // State Variables Initialization
    STAT_tzStateMac.Next_st = STAND_BY;
    STAT_tzStateMac.Previous_st = STAND_BY;
    STAT_tzStateMac.Present_st = STAND_BY;
}
/*=============================================================================
 @brief infinite loop for the main where tasks are executed is defined here

 @param void
 @return void
 ============================================================================ */

void STAT_fnFSMCheck(void)
{
    //  Determine the Next State depending on the flags set/reset in the rest of the code

    stat_fnFSMNextState();

    switch (STAT_tzStateMac.Present_st)
    {
    case STAND_BY:
    {

        if (CANB_tzSSRegs.StartCmd == 0)
        {
            stat_fnInitState();

            STAT_tzStateMac.Next_st = STAND_BY;

            stat_IOReset();
        }

        //GOTO START -  LPC IO Comm OK & LPC DOS OK & System Safety Controller Comm OK
        //& Start LHC from System Safety Controller

//        else if ((CANB_tzSSRegs.StartCmd == 1)
//                && (CANA_tzIOflags.LPC31Comfail == 0)
//                && (CANA_tzIOflags.LPC30Comfail == 0)
//                && (CANB_tzSSRegs.SSComfail == 0)
//                && (CANA_tzLPCDI_IORegs[CANA_mLPC30_IO].bit.DI_bit3 == 0x0)
//                && (CANA_tzLPCDI_IORegs[CANA_mLPC30_IO].bit.DI_bit4 == 0x0))
//
        else if ((CANB_tzSSRegs.StartCmd == 1)
                      && (CANA_tzIOflags.LPC31Comfail == 0)&&(CANA_tzIOtimers.LPC30ComFailCnt<4)
                      && (CANA_tzIOflags.LPC30Comfail == 0)&&(CANA_tzIOtimers.LPC31ComFailCnt<4)
                      && (CANB_tzSSRegs.SSComfail == 0)
                      && (CANA_tzLPCDI_IORegs[CANA_mLPC30_IO].bit.DI_bit3 == 0x0)
                      && (CANA_tzLPCDI_IORegs[CANA_mLPC30_IO].bit.DI_bit4 == 0x0))

        {

            STAT_tzStateMac.Next_st = PURGE;
        }
        else if (CANB_tzSSRegs.StartCmd == 0)
        {
            STAT_tzStateMac.Next_st = STAND_BY;
        }
        break;
    }

    case PURGE:
    {
        if ((CANB_tzSSRegs.StartCmd == 1) && (bPurge_complete == 1))
        {

            STAT_tzStateMac.Next_st = IOPOWER;
        }

        if (CANB_tzSSRegs.StartCmd == 0)
        {
            STAT_tzStateMac.Next_st = STAND_BY;
        }
        break;
    }

    case IOPOWER:
    {
//        //GOTO READY ONLY IF LHC IO Comm OK & LHC DOS OK
        if ((CANA_tzIOflags.LHC11Comfail == 0)
                && (CANA_tzIOflags.LHC10Comfail == 0)
                && (CANA_tzLHCDI_IORegs[CANA_mLHC10_IO].bit.DI_bit0 == 0x0)
                && (CANA_tzLHCDI_IORegs[CANA_mLHC10_IO].bit.DI_bit1 == 0x0)
                && (biopower_complete == 1))
        {
            STAT_tzStateMac.Next_st = ARMED_POWER;
        }

        else if (CANB_tzSSRegs.StartCmd == 0)
        {
            STAT_tzStateMac.Next_st = STAND_BY;
            stat_IOReset();
        }

        break;
    }
    case ARMED_POWER:
    {
        if (CANB_tzSSRegs.StartCmd == 0)
        {
            STAT_tzStateMac.Next_st = STAND_BY;

            stat_IOReset();
        }
        else if((CANA_tzLHCIO1_AIFaultRegs.bit.LVL_101 == 1)|| (bPurgeFanFault == 1))
        {
            STAT_tzStateMac.Next_st = SHUT_DOWN;
        }
        else if ((CANA_tzLHCIO1_AIFaultRegs.bit.PRT_102 == 1)
                || (CANA_tzLHCIO2_AIFaultRegs.bit.PRT_401 == 1)
                || (COM_FAIL == 1))
        {
            STAT_tzStateMac.Next_st = FAULT;
        }

        break;
    }

    case FAULT:
    {
        if ((CANB_tzSSRegs.StartCmd == 0)
                || ((CANA_tzLHCIO2_AIFaultRegs.bit.PRT_401 == 0)
                       &&(CANA_tzLHCIO1_AIFaultRegs.bit.PRT_102 == 0)
                        && (CANA_tzLHCIO1_AIFaultRegs.bit.LVL_101 == 0)
                        && (COM_FAIL == 0)&&(bPurgeFanFault==0)))
        {
            STAT_tzStateMac.Next_st = STAND_BY;
            stat_IOReset();
        }
        else if ((CANB_tzSSRegs.StartCmd == 1)
                && (CANA_tzLHCIO2_AIFaultRegs.bit.PRT_401 == 0)
                && (CANA_tzLHCIO1_AIFaultRegs.bit.PRT_102 == 0)
                && (CANA_tzLHCIO1_AIFaultRegs.bit.LVL_101 == 0)
                && (COM_FAIL == 0)&&(bPurgeFanFault))
        {

            STAT_tzStateMac.Next_st = PURGE;
        }
        else
        {
            STAT_tzStateMac.Next_st = FAULT;
        }
        break;
    }
    case SHUT_DOWN:
    {
          break;
    }

    default:
        break;
    }
}
//#################################################################################################################
static void stat_fnFSMNextState()
{
    STAT_tzStateMac.Previous_st = STAT_tzStateMac.Present_st;
    STAT_tzStateMac.Present_st = STAT_tzStateMac.Next_st;
}

/*==============================================================================
 End of File
 ==============================================================================*/
void stat_IOReset(void)
{
    uint16_t ui16temp;

    CANA_tzIOtimers.TxCntPurgeFans = 0;
    CANA_tzIOtimers.TxCntLHCpower = 0;
    CANA_tzIOtimers.TxCntLPCCONT = 0;
    CANA_tzMPRegs.ReadyCmd = 0;
//    CANA_tzIOtimers.LHC10ComFailCnt=0;
//    CANA_tzIOtimers.LHC11ComFailCnt=0;
//    CANA_tzIOtimers.LPC30ComFailCnt=0;
//    CANA_tzIOtimers.LPC31ComFailCnt=0;
    bPurge_complete = 0;
    biopower_complete = 0;

    for (ui16temp = 0; ui16temp <= 3; ui16temp++)
    {

        // CANA_tzDI_IORegs[ui16temp].all = 0;

    }

}
