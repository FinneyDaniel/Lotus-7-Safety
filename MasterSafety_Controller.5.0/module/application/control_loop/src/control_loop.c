/*=============================================================================
 Copyright Enarka India Pvt Ltd (EIPL) All Rights Reserved.
 All trademarks are owned by Enarka India Private Limited
 ============================================================================ */

/*==============================================================================
 @file  control_waterloop.c
 @author JOTHI RAMESH
 @date 10/07/2022

 @brief Description: Purge Fans, LPC Contactor turn ON/OFF . monitor the purge fan frequency
 ==============================================================================*/

/*==============================================================================
 Includes
 ==============================================================================*/

#include <stdint.h>
#include "F28x_Project.h"
#include <stdint.h>
#include <stdbool.h>

#include "math.h"
#include "isr.h"
#include "scheduler.h"
#include "cana_defs.h"
#include "canb_defs.h"
#include "state_machine.h"
#include "mathcalc.h"
/*==============================================================================
 Defines
 ==============================================================================*/

/*==============================================================================
 Enums
 ==============================================================================*/

/*==============================================================================
 Structures
 ==============================================================================*/
CANA_tzANG_OP CANA_tzAO_VAL[2][2];
union CANA_tzDO_STATUS_MSMP DOSTATUS_tzMSMPRegs;
union CANA_tzMP_FAULTS MPFAULTS_tzMSMPRegs;
/*==============================================================================
 Macros
 ==============================================================================*/

/*==============================================================================
 Local Function Prototypes
 ==============================================================================*/

void purge_fnFan(void);
void LHC_fnPower(void);
void Monitor_fnFeedbacks(void);
void MSMP_fnFaultstatusupdate(void);
/*==============================================================================
 Local Variables
 ==============================================================================*/

uint16_t uiTachFreqLowLmt=30;  //10hz*60 = 600RPM //when hyd start
uint16_t uiTachFreqHighLmt=50; //50*20 = 1000RPM [ 1hz= 60RPM]   for testing 150hz given 150 x 60 =9000RPM
uint16_t uiFanSpeed_AOVval=80;
uint16_t uiPurgestate_delay=6000;
bool bPurge_complete =0,biopower_complete=0;
/*==============================================================================
 Local Constants
 ==============================================================================*/

/*=============================================================================
 @function name: void purge_fnFan(void)
 @brief: Purge Fan turn ON/OFF condition added here. called @10ms
         (isr3.c \ CANA_fnTx called @purge state) 10msec
 @param :void
 @return: void
 ============================================================================ */
void purge_fnFan(void)
{
    CANA_tzIOtimers.TxCntPurgeFans++;

    if(CANA_tzIOtimers.TxCntPurgeFans > uiPurgestate_delay)  //6000 *10ms = 60sec
    {
        CANA_tzIOtimers.TxCntPurgeFans = uiPurgestate_delay;
        bPurge_complete=1;
    }
    else
    {
        bPurge_complete=0;
    }

    if (CANA_tzIOtimers.TxCntPurgeFans == 50) //500msec
    {
        CANA_tzDO[0][CANA_mLPC30_IO].bit.DO0 = 0x1;
        CANA_fnCmdsForDigOPs(CANA_tzIORegs.uiUnitID, CANA_mLPC_CABID, CANA_mLPC30_IO,
                             &CANA_tzDO[0][CANA_mLPC30_IO]); // Turn ON PURGE FAN 101&102
     }
    else if (CANA_tzIOtimers.TxCntPurgeFans == 100) //1sec
    {
        CANA_tzDO[0][CANA_mLPC30_IO].bit.DO1 = 0x1;
        CANA_fnCmdsForDigOPs(CANA_tzIORegs.uiUnitID, CANA_mLPC_CABID, CANA_mLPC30_IO,
                             &CANA_tzDO[0][CANA_mLPC30_IO]); // Turn ON PURGE FAN 501&502
    }

    else if (CANA_tzIOtimers.TxCntPurgeFans == 150) // 1.5sec
    {
        CANA_tzDO[0][CANA_mLPC30_IO].bit.DO2 = 0x1;
        CANA_fnCmdsForDigOPs(CANA_tzIORegs.uiUnitID, CANA_mLPC_CABID, CANA_mLPC30_IO,
                             &CANA_tzDO[0][CANA_mLPC30_IO]); // Turn ON PURGE FAN 401&402

    }

}
/*=============================================================================
 @function name: void LHC_fnPower(void)
 @brief: giving output cmd of LHC AC / DC. called @10ms
         ((isr3.c \ CANA_fnTx called @IOpower state) 10msec
 @param :void
 @return: void
 ============================================================================ */
void LHC_fnPower(void)
{
    CANA_tzIOtimers.TxCntLHCpower++;

    if(CANA_tzIOtimers.TxCntLHCpower > 200)    //200 * 10ms = 2sec for complete the IOPOWER
    {
        CANA_tzIOtimers.TxCntLHCpower = 200;
        biopower_complete=1;
    }
    else
    {
        biopower_complete=0;
    }
    if (CANA_tzIOtimers.TxCntLHCpower == 50)
    {
        CANA_tzDO[0][CANA_mLPC30_IO].bit.DO3 = 0x1;
        CANA_tzDO[0][CANA_mLPC30_IO].bit.DO4 = 0x1;

        CANA_fnCmdsForDigOPs(CANA_tzIORegs.uiUnitID, CANA_mLPC_CABID, CANA_mLPC30_IO,
                             &CANA_tzDO[0][CANA_mLPC30_IO]); // Turn ON LHC -DC  (3-0)
     }
    if (CANA_tzIOtimers.TxCntLHCpower == 100)
    {
        CANA_tzDO[0][CANA_mLPC30_IO].bit.DO5 = 0x1;
        CANA_tzDO[0][CANA_mLPC30_IO].bit.DO6 = 0x1;

         CANA_fnCmdsForDigOPs(CANA_tzIORegs.uiUnitID, CANA_mLPC_CABID, CANA_mLPC30_IO,
                              &CANA_tzDO[0][CANA_mLPC30_IO]); // Turn ON LHC -AC (3-0)
    }

 }
/*=============================================================================
 @function name: void LPC_fnContactor(void)
 @brief: LPC contactor turn ON/OFF condition added here. called @10ms
         (isr3.c \ CANA_fnTx called  @ARMED state) 10msec
 @param :void
 @return: void
 ============================================================================ */
void LPC_fnContactor(void)
{
    CANA_tzIOtimers.TxCntLPCCONT++;

    if(CANA_tzIOtimers.TxCntLPCCONT > 100)
    {
        CANA_tzIOtimers.TxCntLPCCONT = 1;
        CANA_tzMPRegs.ReadyCmd = 1;    // 100 * 10ms = 1sec delay for MP READY cmd after completing the ARMED POWER
    }

    if (CANA_tzIOtimers.TxCntLPCCONT == 50) //500msec
    {
        CANA_tzDO[0][CANA_mLPC31_IO].bit.DO0 = 0x1;
        CANA_tzDO[0][CANA_mLPC31_IO].bit.DO1 = 0x1;

        CANA_fnCmdsForDigOPs(CANA_tzIORegs.uiUnitID, CANA_mLPC_CABID, CANA_mLPC31_IO,
                             &CANA_tzDO[0][CANA_mLPC31_IO]); // Turn ON LPC CONTACTOR- CTR301  // LPC CAB-3(0) - IOCARD -1
    }
 }

/*=============================================================================
 @function name: void Monitor_fnFeedbacks(void)
 @brief: monitoring the purge fan frequency. called @10ms
         (isr3.c \ CANA_fnTx called @PURGE state) 10msec  and also function called in scheduler.c\slot0-50msec
 @param :void
 @return: void
 ============================================================================ */
/**************** SPEED CONTROL OF PURGE FANS *******************************/
void Monitor_fnFeedbacks(void)
{
    if(CANA_tzMPRegs.PresentStMP >= 3)
    {
        uiFanSpeed_AOVval=100;
    }
    else
    {
        uiFanSpeed_AOVval = 80;
    }

    if (CANA_tzLPCIO1_AOFaultRegs.bit.PUR_FAN101_102 == 0)
    {

        CANA_tzAO[0][CANA_mLPC30_IO].bit.AVO1 = 0x1;   // PURGE FAN ON 101 & 102
        CANA_tzAO_VAL[0][CANA_mLPC30_IO].value.AVO1=uiFanSpeed_AOVval;    // PURGE FAN 101 & 102 SPEED 10v * 10
       // CANA_fnMPTxCmds(CANA_mLPC_CABID,CANA_mLPC30_IO,&CANA_tzAO[0][CANA_mLPC30_IO]);  // LPC CAB-3(0) - IOCARD -0
    }
    else
    {
        CANA_tzAO[0][CANA_mLPC30_IO].bit.AVO1 = 0x0;   // PURGE FAN OFF 101 & 102
        CANA_tzAO_VAL[0][CANA_mLPC30_IO].value.AVO1=0;    // PURGE FAN 101 & 102 SPEED 10v * 10
      //  CANA_fnMPTxCmds(CANA_mLPC_CABID,CANA_mLPC30_IO,&CANA_tzAO[0][CANA_mLPC30_IO]);  // LPC CAB-3(0) - IOCARD -0
     }
  /****************************************************************************/
    if(CANA_tzLPCIO1_AOFaultRegs.bit.PUR_FAN501_502 == 0)
     {
        CANA_tzAO[0][CANA_mLPC30_IO].bit.AVO2 = 0x1;// PURGE FAN ON 501 & 502
        CANA_tzAO_VAL[0][CANA_mLPC30_IO].value.AVO2=uiFanSpeed_AOVval;// PURGE FAN 501 & 502 SPEED 10v * 10
       // CANA_fnMPTxCmds(CANA_mLPC_CABID,CANA_mLPC30_IO,&CANA_tzAO[0][CANA_mLPC30_IO]);   // LPC CAB-3(0) - IOCARD -0
     }
    else
    {
        CANA_tzAO[0][CANA_mLPC30_IO].bit.AVO2 = 0x0;// PURGE FAN off 501 & 502
        CANA_tzAO_VAL[0][CANA_mLPC30_IO].value.AVO2=0;// PURGE FAN 501 & 502 SPEED 10v * 10
       // CANA_fnMPTxCmds(CANA_mLPC_CABID,CANA_mLPC30_IO,&CANA_tzAO[0][CANA_mLPC30_IO]);   // LPC CAB-3(0) - IOCARD -0
    }
    /****************************************************************************/
    if(CANA_tzLPCIO1_AOFaultRegs.bit.PUR_FAN401_402 == 0)
     {
        CANA_tzAO[0][CANA_mLPC30_IO].bit.AVO3 = 0x1;   // PURGE FAN 401 & 402
        CANA_tzAO_VAL[0][CANA_mLPC30_IO].value.AVO3=uiFanSpeed_AOVval;  // PURGE FAN 401 & 402 SPEED 10v * 10
      //  CANA_fnMPTxCmds(CANA_mLPC_CABID,CANA_mLPC30_IO,&CANA_tzAO[0][CANA_mLPC30_IO]);  // LPC CAB-3(0) - IOCARD -0
     }
    else
    {
        CANA_tzAO[0][CANA_mLPC30_IO].bit.AVO3 = 0x0;   // PURGE FAN OFF 401 & 402
        CANA_tzAO_VAL[0][CANA_mLPC30_IO].value.AVO3=0;  // PURGE FAN 401 & 402 SPEED 10v * 10
       // CANA_fnMPTxCmds(CANA_mLPC_CABID,CANA_mLPC30_IO,&CANA_tzAO[0][CANA_mLPC30_IO]);  // LPC CAB-3(0) - IOCARD -0

    }
}
/*=============================================================================
 @function name: void MSMP_fnFaultstatusupdate(void)
 @brief: updating the faults and digital output status to send MS -> MP.
         monitoring the purge fan frequency. called @50ms
         (scheduler.c \ SCH_fnslot_3\MSMP_fnFaultstatusupdate called  ) 50msec
 @param :void
 @return: void
 ============================================================================ */
void MSMP_fnFaultstatusupdate(void)
{
    /*''''''''''''Digital output status update '''''''''''''''''''''''''''''''''''''''*/
    DOSTATUS_tzMSMPRegs.bit.purg101_102 = CANA_tzDO[0][CANA_mLPC30_IO].bit.DO0;
    DOSTATUS_tzMSMPRegs.bit.purg501_502 = CANA_tzDO[0][CANA_mLPC30_IO].bit.DO1;
    DOSTATUS_tzMSMPRegs.bit.purg401_402 = CANA_tzDO[0][CANA_mLPC30_IO].bit.DO2;
    DOSTATUS_tzMSMPRegs.bit.LHC_dc = ((CANA_tzDO[0][CANA_mLPC30_IO].bit.DO3)&&(CANA_tzDO[0][CANA_mLPC30_IO].bit.DO4));
    DOSTATUS_tzMSMPRegs.bit.LHC_ac = ((CANA_tzDO[0][CANA_mLPC30_IO].bit.DO5)&&(CANA_tzDO[0][CANA_mLPC30_IO].bit.DO6));
    DOSTATUS_tzMSMPRegs.bit.PSU_CONT=CANA_tzDO[0][CANA_mLPC31_IO].bit.DO0;
    /*''''''''''''faults update '''''''''''''''''''''''''''''''''''''''*/
    MPFAULTS_tzMSMPRegs.bit.Tpurg101_102 = CANA_tzLPCIO1_AOFaultRegs.bit.PUR_FAN101_102;
    MPFAULTS_tzMSMPRegs.bit.Tpurg501_502 = CANA_tzLPCIO1_AOFaultRegs.bit.PUR_FAN501_502;
    MPFAULTS_tzMSMPRegs.bit.Tpurg401_402 = CANA_tzLPCIO1_AOFaultRegs.bit.PUR_FAN401_402;
    MPFAULTS_tzMSMPRegs.bit.DOS101_103= CANA_tzLHCIO1_DIFaultRegs.bit.DOS101_103;
    MPFAULTS_tzMSMPRegs.bit.DOS301_303= ((CANA_tzLPCIO1_DIFaultRegs.bit.DOS_301 == 0)||(CANA_tzLPCIO1_DIFaultRegs.bit.DOS_303 == 0))?1:0;
    MPFAULTS_tzMSMPRegs.bit.LVL_101=CANA_tzLHCIO1_AIFaultRegs.bit.LVL_101;
    MPFAULTS_tzMSMPRegs.bit.PRT_102= CANA_tzLHCIO1_AIFaultRegs.bit.PRT_102;
    MPFAULTS_tzMSMPRegs.bit.PRT_401= CANA_tzLHCIO2_AIFaultRegs.bit.PRT_401;
}
/*==============================================================================
 End of File
 ==============================================================================*/
