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
#include "hal/driverlib/can.h"
#include <stdint.h>
#include "mathcalc.h"
#include "control_defs.h"
#include "faultcheck.h"
#include "state_machine.h"
#include "scheduler.h"
#include "cana_defs.h"
#include "canb_defs.h"
/*==============================================================================
 Defines
 ==============================================================================*/

uint16_t faultSettimer1[16] = { 0 };
uint16_t faultResettimer1[16] = { 0 };

uint16_t faultSettimer2[16] = { 0 };
uint16_t faultResettimer2[16] = { 0 };

uint16_t faultSettimer3[16] = { 0 };
uint16_t faultResettimer3[16] = { 0 };

uint16_t faultSettimer4[16] = { 0 };
uint16_t faultResettimer4[16] = { 0 };

uint16_t faultSettimer5[16] = { 0 };
uint16_t faultResettimer5[16] = { 0 };

uint16_t LPCIO1FaultRegs[16] = { 0 };
uint16_t LPCIO2FaultRegs[16] = { 0 };
uint16_t LHCIO1FaultRegs[16] = { 0 };
uint16_t LHCIO2FaultRegs[16] = { 0 };

/*==============================================================================
 Enums
 ==============================================================================*/

/*==============================================================================
 Structures
 ==============================================================================*/
union CONTROL_FLTS_REG CONTROLtzFaultRegs;
/*==============================================================================
 Macros
 ==============================================================================*/
// LPC Faults
#define FAULT_mPURFAN101_102       (0U)
#define FAULT_mPURFAN501_502       (1U)
#define FAULT_mPURFAN401_402       (2U)

#define FAULT_mDOS301              (3U)
#define FAULT_mDOS303              (4U)

#define FAULT_mHYS101              (8U)
#define FAULT_mHYS501              (9U)
#define FAULT_mHYS401              (10U)
#define FAULT_mHYS102_SHUTDOWN     (11U)
#define FAULT_mHYS102_RAMPDOWN     (12U)
#define FAULT_mOXS101_SHUTDOWN     (13U)
#define FAULT_mOXS101_RAMPDOWN     (14U)
#define FAULT_mVFD_PMPFAIL         (15U)

// LHC Faults

#define FAULT_mDOS101_103          (0U)
#define FAULT_mDOS301_303          (1U)

#define FAULT_mLVL101              (2U)
#define FAULT_mPRT101              (3U)
#define FAULT_mPRT102              (4U)
#define FAULT_mLGT101_402          (5U)

#define FAULT_mPRT401              (8U)
#define FAULT_mPRT402              (9U)

//delay fault set & clear

#define DLY_mFLTSET_PF            (18000U)  //600*50ms = 30sec  [18000x50ms = 900sec]
#define DLY_mFLTCLEAR_PF          (30U)  //600*50ms = 30sec  [18000x50ms = 900sec]
//in hydrogen generation state
#define DLY_mFLTSET_PF1            (20U)  //100*50ms = 5sec  [18000x50ms = 900sec]
#define DLY_mFLTCLEAR_PF1          (20U)  //50*50ms = 2.5sec  [18000x50ms = 900sec]

#define DLY_mFLTSET_DOS           (20U)  //20*50ms = 1sec  [18000x50ms = 900sec]
#define DLY_mFLTCLEAR_DOS         (20U)  //20*50ms = 1sec  [18000x50ms = 900sec]

/**************************FAULT SET & RECOVERY*************************/
#define FAULT_mPRT102SET              (0.5f)
#define FAULT_mPRT401SET              (32.0f)
#define FAULT_mLVL101LOWSET           (60.0f)
#define FAULT_mLVL101HISET            (90.0f)
#define FAULT_mPRT102RESET            (1.15f)
#define FAULT_mPRT401RESET            (30.0f)
#define FAULT_mLVL101RESET            (62.0f)
#define FAULT_mLVL101RESET1            (93.0f)

/*==============================================================================
 Local Function Prototypes
 ==============================================================================*/

static bool faultLPCIO1_fnSet(uint16_t ui16DIFaultType, uint16_t timerValueSet);
static bool faultLPCIO1_fnReset(uint16_t ui16DIFaultType,
                                uint16_t timerValueReset);

static bool faultLHCIO1_fnSet(uint16_t ui16DIFaultType1,
                              uint16_t timerValueSet1);
static bool faultLHCIO1_fnReset(uint16_t ui16DIFaultType1,
                                uint16_t timerValueReset1);

/*==============================================================================
 Local Variables
 ==============================================================================*/
bool bPurgeFanFault = 0, bDOS_Fault = 0, dos_senflt = 0;

void faultCheck(void)
{
    if ((CANA_tzMPRegs.PresentStMP >= 3))
    //       if((CANA_tzMPRegs.Hyd_Genstart==0)&&(CANA_tzMPRegs.PresentStMP>=3))
    {
        if (MATHConvtzRegs.AISensorPRT102 < FAULT_mPRT102SET)
        {
            CANA_tzLHCIO1_AIFaultRegs.bit.PRT_102 = faultLHCIO1_fnSet(
            FAULT_mPRT102,
                                                                      20);
        }

        if (MATHConvtzRegs.AISensorPRT401 >= FAULT_mPRT401SET)
        {
            CANA_tzLHCIO2_AIFaultRegs.bit.PRT_401 = faultLHCIO1_fnSet(
            FAULT_mPRT401,
                                                                      20);
        }

        if ((MATHConvtzRegs.AISensorLVL101 <= FAULT_mLVL101LOWSET)
                || (MATHConvtzRegs.AISensorLVL101 > FAULT_mLVL101HISET))
        {
            CANA_tzLHCIO1_AIFaultRegs.bit.LVL_101 = faultLHCIO1_fnSet(
            FAULT_mLVL101,
                                                                      20);
        }
        if ((CANA_tzLHCDI_IORegs[CANA_mLHC10_IO].bit.DI_bit2 == 0x01)
                || (CANA_tzLHCDI_IORegs[CANA_mLHC10_IO].bit.DI_bit3 == 0x01))
        {
            CANA_tzLHCIO1_DIFaultRegs.bit.LGT101_402 = faultLHCIO1_fnSet(
            FAULT_mLGT101_402,
                                                                         20);
        }

    }
    //  else
    // {
    if (MATHConvtzRegs.AISensorPRT102 > FAULT_mPRT102RESET)
    {
        CANA_tzLHCIO1_AIFaultRegs.bit.PRT_102 = faultLHCIO1_fnReset(
        FAULT_mPRT102,
                                                                    20);
    }
    if (MATHConvtzRegs.AISensorPRT401 <= FAULT_mPRT401RESET)
    {
        CANA_tzLHCIO2_AIFaultRegs.bit.PRT_401 = faultLHCIO1_fnReset(
        FAULT_mPRT401,
                                                                    20);
    }
    if ((MATHConvtzRegs.AISensorLVL101 >= FAULT_mLVL101RESET)&&(MATHConvtzRegs.AISensorLVL101 <=FAULT_mLVL101RESET1))
    {
        CANA_tzLHCIO1_AIFaultRegs.bit.LVL_101 = faultLHCIO1_fnReset(
        FAULT_mLVL101,
                                                                    20);
    }
    if ((CANA_tzLHCDI_IORegs[CANA_mLHC10_IO].bit.DI_bit2 == 0x0)
            && (CANA_tzLHCDI_IORegs[CANA_mLHC10_IO].bit.DI_bit3 == 0x0))
    {
        CANA_tzLHCIO1_DIFaultRegs.bit.LGT101_402 = faultLHCIO1_fnReset(
        FAULT_mLGT101_402,
                                                                       20);
    }
    // }
    //  }
    /***************************************************************/

    // LPC Cabinet DI Faults
    if (CANA_tzLPCDI_IORegs[CANA_mLPC30_IO].bit.DI_bit3 == 0x01)
    {
        CANA_tzLPCIO1_DIFaultRegs.bit.DOS_301 = faultLPCIO1_fnSet(
                FAULT_mDOS301,
                DLY_mFLTSET_DOS);
    }
    else if (CANA_tzLPCDI_IORegs[CANA_mLPC30_IO].bit.DI_bit3 == 0x0)
    {
        CANA_tzLPCIO1_DIFaultRegs.bit.DOS_301 = faultLPCIO1_fnReset(
                FAULT_mDOS301,
                DLY_mFLTCLEAR_DOS);
    }

    if (CANA_tzLPCDI_IORegs[CANA_mLPC30_IO].bit.DI_bit4 == 0x01)
    {
        CANA_tzLPCIO1_DIFaultRegs.bit.DOS_303 = faultLPCIO1_fnSet(
                FAULT_mDOS303,
                DLY_mFLTSET_DOS);
    }
    else if (CANA_tzLPCDI_IORegs[CANA_mLPC30_IO].bit.DI_bit4 == 0x0)
    {
        CANA_tzLPCIO1_DIFaultRegs.bit.DOS_303 = faultLPCIO1_fnReset(
                FAULT_mDOS303,
                DLY_mFLTCLEAR_DOS);
    }

    // LHC Cabinet DI Faults

    if ((CANA_tzLHCDI_IORegs[CANA_mLHC10_IO].bit.DI_bit0 == 0x01)
            || (CANA_tzLHCDI_IORegs[CANA_mLHC10_IO].bit.DI_bit1 == 0x01))
    {
        CANA_tzLHCIO1_DIFaultRegs.bit.DOS101_103 = faultLHCIO1_fnSet(
                FAULT_mDOS101_103,
                DLY_mFLTSET_DOS);
    }
    else if ((CANA_tzLHCDI_IORegs[CANA_mLHC10_IO].bit.DI_bit0 == 0x0)
            && (CANA_tzLHCDI_IORegs[CANA_mLHC10_IO].bit.DI_bit1 == 0x0))
    {
        CANA_tzLHCIO1_DIFaultRegs.bit.DOS101_103 = faultLHCIO1_fnReset(
                FAULT_mDOS101_103,
                DLY_mFLTCLEAR_DOS);
    }

    /*************************************PURGE FAN SPEED****in all STATE except STANDBY*******************************/
    //    if ((STAT_tzStateMac.Present_st > 0)
    //            && ((CANA_tzMPRegs.Hyd_Genstart == 0)
    //                    && (CANA_tzMPRegs.PresentStMP < 3)))
    if ((STAT_tzStateMac.Present_st >= 3) && ((CANA_tzMPRegs.PresentStMP < 3)))
    {
        if ((CANA_tzDISensorData.PURGE101 < uiTachFreqHighLmt)
                || (CANA_tzDISensorData.PURGE102 < uiTachFreqHighLmt))

        {
            CANA_tzLPCIO1_AOFaultRegs.bit.PUR_FAN101_102 = faultLPCIO1_fnSet(
                    FAULT_mPURFAN101_102, DLY_mFLTSET_PF);
        }

        else
        {
            CANA_tzLPCIO1_AOFaultRegs.bit.PUR_FAN101_102 = faultLPCIO1_fnReset(
                    FAULT_mPURFAN101_102,
                    DLY_mFLTCLEAR_PF);
        }
    }
    /*'''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''*/
    if ((STAT_tzStateMac.Present_st >= 3) && ((CANA_tzMPRegs.PresentStMP < 3)))
    {
        if ((CANA_tzDISensorData.PURGE501 < uiTachFreqHighLmt)
                || (CANA_tzDISensorData.PURGE502 < uiTachFreqHighLmt))

        {
            CANA_tzLPCIO1_AOFaultRegs.bit.PUR_FAN501_502 = faultLPCIO1_fnSet(
                    FAULT_mPURFAN501_502, DLY_mFLTSET_PF);
        }
        else
        {
            CANA_tzLPCIO1_AOFaultRegs.bit.PUR_FAN501_502 = faultLPCIO1_fnReset(
                    FAULT_mPURFAN501_502,
                    DLY_mFLTCLEAR_PF);
        }
    }
    /*'''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''*/
    //   if((STAT_tzStateMac.Present_st > 0)&&((CANA_tzMPRegs.Hyd_Genstart==0)&&(CANA_tzMPRegs.PresentStMP<3)))
    if ((STAT_tzStateMac.Present_st >= 3) && ((CANA_tzMPRegs.PresentStMP < 3)))

    {
        if ((CANA_tzDISensorData.PURGE401 < uiTachFreqHighLmt)
                || (CANA_tzDISensorData.PURGE402 < uiTachFreqHighLmt))
        {
            CANA_tzLPCIO1_AOFaultRegs.bit.PUR_FAN401_402 = faultLPCIO1_fnSet(
                    FAULT_mPURFAN401_402, DLY_mFLTSET_PF);
        }

        else
        {
            CANA_tzLPCIO1_AOFaultRegs.bit.PUR_FAN401_402 = faultLPCIO1_fnReset(
                    FAULT_mPURFAN401_402, 10);
        }
    }
    /****************** fault check  when Hydrogen generated condition ************************************/
    if (CANA_tzMPRegs.PresentStMP >= 3)
    {
        if ((STAT_tzStateMac.Present_st > 0)
                && ((CANA_tzMPRegs.Hyd_Genstart == 0)))
        {
            if ((CANA_tzDISensorData.PURGE101 < uiTachFreqLowLmt)
                    || (CANA_tzDISensorData.PURGE102 < uiTachFreqLowLmt))
            {
                CANA_tzLPCIO1_AOFaultRegs.bit.PUR_FAN101_102 =
                        faultLPCIO1_fnSet(
                        FAULT_mPURFAN101_102,
                                          DLY_mFLTSET_PF1);
            }
            else
            {
                CANA_tzLPCIO1_AOFaultRegs.bit.PUR_FAN101_102 =
                        faultLPCIO1_fnReset(
                        FAULT_mPURFAN101_102,
                                            10);
            }
        }
        /*'''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''*/

        if ((STAT_tzStateMac.Present_st > 0)
                && ((CANA_tzMPRegs.Hyd_Genstart == 0)))
        {
            if ((CANA_tzDISensorData.PURGE501 < uiTachFreqLowLmt)
                    || (CANA_tzDISensorData.PURGE502 < uiTachFreqLowLmt))
            {
                CANA_tzLPCIO1_AOFaultRegs.bit.PUR_FAN501_502 =
                        faultLPCIO1_fnSet(
                        FAULT_mPURFAN501_502,
                                          DLY_mFLTSET_PF1);
            }
            else
            {
                //CANA_tzLPCIO1_AOFaultRegs.bit.PUR_FAN501_502 = 0;
                CANA_tzLPCIO1_AOFaultRegs.bit.PUR_FAN501_502 =
                        faultLPCIO1_fnReset(
                        FAULT_mPURFAN501_502,
                                            DLY_mFLTCLEAR_PF1);
            }
        }
        /*'''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''*/

        if ((STAT_tzStateMac.Present_st > 0)
                && ((CANA_tzMPRegs.Hyd_Genstart == 0)))
        {
            if ((CANA_tzDISensorData.PURGE401 < uiTachFreqLowLmt)
                    || (CANA_tzDISensorData.PURGE402 < uiTachFreqLowLmt))
            {
                CANA_tzLPCIO1_AOFaultRegs.bit.PUR_FAN401_402 =
                        faultLPCIO1_fnSet(
                        FAULT_mPURFAN401_402,
                                          DLY_mFLTSET_PF1);
            }

            else
            {
                // CANA_tzLPCIO1_AOFaultRegs.bit.PUR_FAN401_402 = 0;
                CANA_tzLPCIO1_AOFaultRegs.bit.PUR_FAN401_402 =
                        faultLPCIO1_fnReset(
                        FAULT_mPURFAN401_402,
                                            DLY_mFLTCLEAR_PF1);
            }
        }

    }
    /*,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,*/
    if ((CANA_tzLPCIO1_AOFaultRegs.bit.PUR_FAN101_102 == 1)
            || (CANA_tzLPCIO1_AOFaultRegs.bit.PUR_FAN501_502 == 1)
            || (CANA_tzLPCIO1_AOFaultRegs.bit.PUR_FAN401_402 == 1))
    // if((CANA_tzLPCIO1_AOFaultRegs.bit.PUR_FAN501_502 == 1))
    {
        bPurgeFanFault = 1;
       // STAT_tzStateMac.Next_st = SHUT_DOWN;
        //  CANB_tzSSRegs.StartCmd =0;
    }
    else
    {
        bPurgeFanFault = 0;
    }
    /*,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,*/
    if (dos_senflt == 1) //flag set for bench level test. in auto testing condition not required.
    {
        if ((CANA_tzLHCIO1_DIFaultRegs.bit.DOS101_103 == 0)
                || (CANA_tzLPCIO1_DIFaultRegs.bit.DOS_301 == 0)
                || (CANA_tzLPCIO1_DIFaultRegs.bit.DOS_303 == 0))
        {
            bDOS_Fault = 1;
            //    STAT_tzStateMac.Next_st = FAULT;
        }
        else
        {
            bDOS_Fault = 0;
        }
    }
//    MPFAULTS_tzMSMPRegs.bit.DOS301_303 = ((CANA_tzLPCIO1_DIFaultRegs.bit.DOS_301 == 0)||(CANA_tzLPCIO1_DIFaultRegs.bit.DOS_303 == 0))?1:0;

}
/****************************************************************************************/
static bool faultLPCIO1_fnSet(uint16_t ui16DIFaultType, uint16_t timerValueSet)
{
    faultSettimer1[ui16DIFaultType]++;
    if (faultSettimer1[ui16DIFaultType] >= timerValueSet)
    {
        faultSettimer1[ui16DIFaultType] = timerValueSet;
        faultResettimer1[ui16DIFaultType] = 0;
        LPCIO1FaultRegs[ui16DIFaultType] = 1;
    }
    return LPCIO1FaultRegs[ui16DIFaultType];

}

static bool faultLPCIO1_fnReset(uint16_t ui16DIFaultType,
                                uint16_t timerValueReset)
{
    faultResettimer1[ui16DIFaultType]++;
    if (faultResettimer1[ui16DIFaultType] >= timerValueReset)
    {
        faultResettimer1[ui16DIFaultType] = timerValueReset;
        faultSettimer1[ui16DIFaultType] = 0;
        LPCIO1FaultRegs[ui16DIFaultType] = 0;
    }

    return LPCIO1FaultRegs[ui16DIFaultType];
}

//static bool faultLPCIO2_fnSet(uint16_t ui16DIFaultType, uint16_t timerValueSet)
//{
//    faultSettimer2[ui16DIFaultType]++;
//    if (faultSettimer2[ui16DIFaultType] >= timerValueSet)
//    {
//        faultSettimer2[ui16DIFaultType] = timerValueSet;
//        faultResettimer2[ui16DIFaultType] = 0;
//        LPCIO2FaultRegs[ui16DIFaultType] = 1;
//    }
//    return LPCIO2FaultRegs[ui16DIFaultType];
//
//}
//
//static bool faultLPCIO2_fnReset(uint16_t ui16DIFaultType,
//                                uint16_t timerValueReset)
//{
//    faultResettimer2[ui16DIFaultType]++;
//    if (faultResettimer2[ui16DIFaultType] >= timerValueReset)
//    {
//        faultResettimer2[ui16DIFaultType] = timerValueReset;
//        faultSettimer2[ui16DIFaultType] = 0;
//        LPCIO2FaultRegs[ui16DIFaultType] = 0;
//    }
//
//    return LPCIO1FaultRegs[ui16DIFaultType];
//}

static bool faultLHCIO1_fnSet(uint16_t ui16DIFaultType, uint16_t timerValueSet)
{
    faultSettimer3[ui16DIFaultType]++;
    if (faultSettimer3[ui16DIFaultType] >= timerValueSet)
    {
        faultSettimer3[ui16DIFaultType] = timerValueSet;
        faultResettimer3[ui16DIFaultType] = 0;
        LHCIO1FaultRegs[ui16DIFaultType] = 1;
    }
    return LHCIO1FaultRegs[ui16DIFaultType];

}

static bool faultLHCIO1_fnReset(uint16_t ui16DIFaultType,
                                uint16_t timerValueReset)
{
    faultResettimer3[ui16DIFaultType]++;
    if (faultResettimer3[ui16DIFaultType] >= timerValueReset)
    {
        faultResettimer3[ui16DIFaultType] = timerValueReset;
        faultSettimer3[ui16DIFaultType] = 0;
        LHCIO1FaultRegs[ui16DIFaultType] = 0;
    }

    return LHCIO1FaultRegs[ui16DIFaultType];
}

//static bool faultLHCIO2_fnSet(uint16_t ui16DIFaultType, uint16_t timerValueSet)
//{
//    faultSettimer4[ui16DIFaultType]++;
//    if (faultSettimer4[ui16DIFaultType] >= timerValueSet)
//    {
//        faultSettimer4[ui16DIFaultType] = timerValueSet;
//        faultResettimer4[ui16DIFaultType] = 0;
//        LHCIO2FaultRegs[ui16DIFaultType] = 1;
//    }
//    return LHCIO2FaultRegs[ui16DIFaultType];
//
//}
//
//static bool faultLHCIO2_fnReset(uint16_t ui16DIFaultType,
//                                uint16_t timerValueReset)
//{
//    faultResettimer4[ui16DIFaultType]++;
//    if (faultResettimer4[ui16DIFaultType] >= timerValueReset)
//    {
//        faultResettimer4[ui16DIFaultType] = timerValueReset;
//        faultSettimer4[ui16DIFaultType] = 0;
//        LHCIO2FaultRegs[ui16DIFaultType] = 0;
//    }
//
//    return LHCIO2FaultRegs[ui16DIFaultType];
//}
//
//static bool faultLPCAO_IO1_fnSet(uint16_t ui16DIFaultType, uint16_t timerValueSet)
//{
//    faultSettimer5[ui16DIFaultType]++;
//    if (faultSettimer5[ui16DIFaultType] >= timerValueSet)
//    {
//        faultSettimer5[ui16DIFaultType] = timerValueSet;
//        faultResettimer5[ui16DIFaultType] = 0;
//        LPCIO1FaultRegs[ui16DIFaultType] = 1;
//    }
//    return LPCIO1FaultRegs[ui16DIFaultType];
//
//}
//
//static bool faultLPCAO_IO1_fnReset(uint16_t ui16DIFaultType,
//                                uint16_t timerValueReset)
//{
//    faultResettimer5[ui16DIFaultType]++;
//    if (faultResettimer5[ui16DIFaultType] >= timerValueReset)
//    {
//        faultResettimer5[ui16DIFaultType] = timerValueReset;
//        faultSettimer5[ui16DIFaultType] = 0;
//        LPCIO1FaultRegs[ui16DIFaultType] = 0;
//    }
//
//    return LPCIO1FaultRegs[ui16DIFaultType];
//}

/*==============================================================================
 End of File
 ==============================================================================*/
