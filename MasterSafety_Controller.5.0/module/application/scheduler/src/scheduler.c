/*=============================================================================
 Copyright Enarka India Pvt Ltd (EIPL) All Rights Reserved.
 All trademarks are owned by Enarka India Private Limited
 ============================================================================ */

/*==============================================================================

 @file  scheduler.c
 @author JOTHI RAMESH
 @date 26-Jan-2022

 @brief Description

 ==============================================================================*/

/*==============================================================================
 Includes
 ==============================================================================*/

#include "scheduler.h"
#include "F28x_Project.h"
#include <stdint.h>
#include "cana_defs.h"
#include <stdint.h>
#include "canb_defs.h"

#include "isr.h"
#include "safety_lib.h"
#include "safety_lib_prv.h"
#include "mathcalc.h"
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

/*==============================================================================
 Macros
 ==============================================================================*/

/*==============================================================================
 Local Function Prototypes
 ==============================================================================*/
static void SCH_fnslot_0(void);
static void SCH_fnslot_1(void);
static void SCH_fnslot_2(void);
static void SCH_fnslot_3(void);
static void SCH_fnslot_4(void);

void SCH_fnslot_all(void);

const fp_sch_slot_t psch_slots[NUM_mTIME_SLOTS] = { SCH_fnslot_0, SCH_fnslot_1,
                                                    SCH_fnslot_2, SCH_fnslot_3,
                                                    SCH_fnslot_4, };

/*==============================================================================
 Local Variables
 ==============================================================================*/
uint16_t ui16txMsgDataPSU[8] = { 0 };
uint16_t i2cChk = 0;
uint16_t i2cChkRead = 0;
static const fp_sch_slot_t pevery_sch_slot;
static const fp_sch_slot_t pevery_sch_slot_cmp;
static int16_t icurrent_slot;
extern const fp_sch_slot_t psch_slots[NUM_mTIME_SLOTS];
uint16_t xx = 0, yy = 0;
/*==============================================================================
 Local Constants
 ==============================================================================*/

/*=============================================================================
 @brief infinite loop for the main where tasks are executed is defined here

 @param void
 @return void
 ============================================================================ */

//-----------------------------------Event Drivers---------------------------//
void scheduler_init(uint16_t slots, const fp_sch_slot_t *psch_slots,
                    const fp_sch_slot_t SCH_fnslot_all)
{
    icurrent_slot = 0;
    fp_sch_slot_t *p_every_sch_slot;
    p_every_sch_slot = (fp_sch_slot_t*) &pevery_sch_slot;
    *p_every_sch_slot = SCH_fnslot_all;
    p_every_sch_slot = (fp_sch_slot_t*) &pevery_sch_slot_cmp;
    *p_every_sch_slot = SCH_fnslot_all;
}
/*=============================================================================*/

void scheduler_task(void)
{
    icurrent_slot++;
    if (icurrent_slot >= NUM_mTIME_SLOTS)
    {
        icurrent_slot = 0;

    }

    if (icurrent_slot < NUM_mTIME_SLOTS)
    {
        psch_slots[icurrent_slot]();
    }
    if (pevery_sch_slot != NULL)
        pevery_sch_slot();
}
/*=============================================================================*/

void SCH_fnslot_0(void)               //function called in every 50ms
{
    safety_fnlog_monitoring_chk();
    Monitor_fnFeedbacks();
    safety_fnLog_monitoring_slot_exe(0);
}
/*=============================================================================*/

void SCH_fnslot_1(void)          //function called in every 50ms
{
    CANA_FlashEvent();
   // CANA_fnTx();   //TX function to MP and IO cards
    safety_fnLog_monitoring_slot_exe(1);
}
/*=============================================================================*/

void SCH_fnslot_2(void)        //function called in every 50ms
{
    faultCheck();
    MATH_fnCalc();
    safety_fnLog_monitoring_slot_exe(2);
}
/*=============================================================================*/

void SCH_fnslot_3(void)         //function called in every 50ms
{
    safety_fnLog_monitoring_slot_exe(3);
    MSMP_fnFaultstatusupdate(); //updating the faults and digital output status to send MS -> MP
    CANB_fnTx();   //TX function to MS and SS
}
/*=============================================================================*/

void SCH_fnslot_4(void)       //function called in every 50ms
{
    safety_fnLog_monitoring_slot_exe(4);
    ++uiHearbtCount;
    if (uiHearbtCount == 15) //15 * 50ms =750ms * 6 cases in heartbeat function = 4500ms=4.5sec
    {
        uiHearbtCount = 0;
        CANA_fnIOHrtBt();   //heart beat status sent to IO card & MP
    }
    else
    {
        uiHearbtcntFlag = 0;
    }
    /************************************************/
    safety_cancom(); //safety RA state
}
/*=============================================================================*/
void SCH_fnslot_all(void)
{
    yy++;                    //function called in every 10ms
}

/*==============================================================================
 End of File
 ==============================================================================*/
