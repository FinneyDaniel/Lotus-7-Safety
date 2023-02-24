/*=============================================================================
 Copyright Enarka India Pvt Ltd (EIPL) All Rights Reserved.
 All trademarks are owned by Enarka India Private Limited
 ============================================================================ */

/*==============================================================================
 @file  safety_cancom.c
 @author    JOTHI RAMESH
 @date 26-Jan-2022

 @brief Description: CAN fail detection
 ==============================================================================*/

/*==============================================================================
 Includes
 ==============================================================================*/

#include "F28x_Project.h"
#include <stdint.h>
#include "safety_lib.h"
#include "safety_lib_prv.h"
#include "cana_defs.h"
#include "canb_defs.h"
#include "faultcheck.h"
#include "state_machine.h"
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
#define  RA_mFAILCNT    (50U)   //25sec (500U)
/*==============================================================================
 Local Function Prototypes
 ==============================================================================*/
void safety_cancom(void);
/*==============================================================================
 Local Variables
 ==============================================================================*/
uint16_t uiRA_state_failcnt = 0,uiRA_state_flag=0;
/*==============================================================================
 Local Constants
 ==============================================================================*/

/*=============================================================================
 @function name: void safety_cancom(void)
 @brief :CAN communication failure detection .
 function called in infinite loop (safety_fnslotAll/safety_cancom.c)

 @param: void
 @return: void
 ============================================================================ */

void safety_cancom(void)
{
    if(STAT_tzStateMac.Present_st >=2)
    {
    if ((CANA_tzMPRegs.MPComfail == 1) || (CANA_tzIOflags.LHC11Comfail == 1)
            || (CANA_tzIOflags.LHC10Comfail == 1)
            || (CANA_tzIOflags.LPC31Comfail == 1)
            || (CANA_tzIOflags.LPC30Comfail == 1)
            || (CANB_tzSSRegs.SSComfail == 1) || (bPurgeFanFault == 1)
            || (CANA_tzLHCIO1_AIFaultRegs.bit.PRT_102 == 1)
            || (CANA_tzLHCIO1_AIFaultRegs.bit.LVL_101 == 1)
            || (CANA_tzLHCIO2_AIFaultRegs.bit.PRT_401 == 1))
    {
        /*--------------------------LHC 10 CAN FAIL DETECTION--CAB-1 & IO - 0-------------------------*/
        uiRA_state_failcnt++;
        if (uiRA_state_failcnt >= RA_mFAILCNT)
        {
            uiRA_state_failcnt = RA_mFAILCNT;
            uiRA_state_failcnt = false;
            uiRA_state_flag = 1;
           // safety_fnPower_off(MEF_CANCOM);   //MEF_CANCOM
            STAT_tzStateMac.Next_st = SHUT_DOWN;
        }


    }
    else
    {
        uiRA_state_failcnt =0;
        uiRA_state_flag = 0;
    }
}
}
/*==============================================================================
 End of File
 ==============================================================================*/
