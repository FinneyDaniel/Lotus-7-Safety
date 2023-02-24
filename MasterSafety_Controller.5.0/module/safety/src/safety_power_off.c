/*=============================================================================
 Copyright Enarka India Pvt Ltd (EIPL) All Rights Reserved.
 All trademarks are owned by Enarka India Private Limited
 ============================================================================ */

/*==============================================================================
 @file  safety_power_off.c
 @author JOTHI RAMESH
 @date 26-Jan-2022

 @brief Description:function called at any safety functions conditions fails
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

void safety_fnPower_off(microelectronic_fault_t fault);

/*==============================================================================
 Local Variables
 ==============================================================================*/
uint16_t ui16TxFlt_safety[8] = { 0 };
/*==============================================================================
 Local Constants
 ==============================================================================*/

/*=============================================================================
 @function name: void safety_fnPower_off(microelectronic_fault_t fault)
 @brief : function called at any safety functions conditions fails
 @param :(microelectronic_fault_t fault)
 @return :void
 ============================================================================ */
void safety_fnPower_off(microelectronic_fault_t fault)
{

//    __disable_interrupts();

    /* send the TURN off DIO commands to IO card */
    // Make all outputs off
//    CANA_tzDO[0][CANA_mLPC30_IO].all=0x00; //PURGE FANS
//    CANA_tzDO[0][CANA_mLPC31_IO].all=0x00;//CTR101 PSU CONT,CTR102 VFD CONT
//    CANA_tzDO[1][0].all=0x00;  // WSV,SV401,SV402,CTR101
//    CANA_tzDO[1][1].all=0x00;  //DRYER VALVES
//    CANA_fnCmdsForDigOPs(CANA_tzIORegs.uiUnitID, 3, 0, &CANA_tzDO[0][0]); //LPCIO-1
//    CANA_fnCmdsForDigOPs(CANA_tzIORegs.uiUnitID, 3, 1, &CANA_tzDO[0][1]); //LPCIO-2
//    CANA_fnCmdsForDigOPs(CANA_tzIORegs.uiUnitID, 1, 0, &CANA_tzDO[1][0]); //LHCIO-1
//    CANA_fnCmdsForDigOPs(CANA_tzIORegs.uiUnitID, 1, 1, &CANA_tzDO[1][1]); //LHCIO-2
    CAN_setupMessageObject(CANA_BASE, CAN_mMAILBOX_30, CANA_mTX_SFTY_FLT_MSGID1,
                            CAN_MSG_FRAME_EXT, CAN_MSG_OBJ_TYPE_TX, 0,
                            CAN_MSG_OBJ_NO_FLAGS,
                            CANA_mEIGHT_BYTE);

    ui16TxFlt_safety[0] = fault& 0xFF;
    ui16TxFlt_safety[1] = (fault >> 8) & 0xFF;
    ui16TxFlt_safety[2] = 0x00;
    ui16TxFlt_safety[3] = 0x00;
    ui16TxFlt_safety[4] = 0x00;
    ui16TxFlt_safety[5] = 0x00;
    ui16TxFlt_safety[6] = 0x00;
    ui16TxFlt_safety[7] = 0x00;

    CAN_sendMessage(CANA_BASE, CAN_mMAILBOX_30, CANA_mEIGHT_BYTE,
                ui16TxFlt_safety);
    if (fault >= MEF_LAST_FAULT)
        fault = MEF_UNKNOWN;

}

/*==============================================================================
 End of File
 ==============================================================================*/
