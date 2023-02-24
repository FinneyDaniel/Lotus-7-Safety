///*=============================================================================
//Copyright Enarka India Pvt Ltd (EIPL) All Rights Reserved.
//All trademarks are owned by Enarka India Private Limited
//============================================================================ */
//
///*==============================================================================
// @file  mathcalc.c
// @author JOTHI RAMESH
// @date 03-jul-2022
//
// @brief Description
//==============================================================================*/
//
///*==============================================================================
// Includes
//==============================================================================*/
#include <state_machine.h>
#include "F28x_Project.h"
#include <stdint.h>
#include <stdbool.h>
#include "inc/hw_can.h"
#include "inc/hw_ints.h"
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "hal/driverlib/can.h"
#include "hal/driverlib/debug.h"
#include "hal/driverlib/interrupt.h"
#include "math.h"
#include "isr.h"
#include "canb_defs.h"
#include "cana_defs.h"
#include "mathcalc.h"

///*==============================================================================
// Defines
//==============================================================================*/
//
///*==============================================================================
// Enums
//==============================================================================*/
//
///*==============================================================================
// Structures
//==============================================================================*/

MATHCONV_tzREGS MATHConvtzRegs;

///*==============================================================================
// Macros
//==============================================================================*/
//
///*==============================================================================
// Local Function Prototypes
//==============================================================================*/

void MATH_fnCalc(void);


///*==============================================================================
// Local Variables
//==============================================================================*/

float32_t LccControl = 70.0;

///*==============================================================================
// Local Constants
//==============================================================================*/
//
///*=============================================================================
// @brief infinite loop for the main where tasks are executed is defined here
//
// @param void
// @return void
// ============================================================================ */
inline float32 math_fnLVLmAtoPercent(float32 LVL_mA)
{
    return (((LVL_mA - MATH_mLVL_MIN) / MATH_mLVL_RANGE) * 100);

}

inline float32 math_fnHYSmAtoPercent(float32 HYS_mA, float32 HYSoffset)
{
    return (((HYS_mA - MATH_mHYS_MIN - HYSoffset) / 16) * 100);

}

inline float32 math_fnOXSmAtoPercent(float32 OXS_mA, float32 OXSoffset)
{
    return (((OXS_mA - MATH_mOXS_MIN - OXSoffset) / 16) * 25);
}

inline float32 math_fnKTCmVtoTemp(float32 KTC_mV)
{
    return (MATH_mKTC_C1 * (KTC_mV) * (KTC_mV) * (KTC_mV))
            + (MATH_mKTC_C2 * (KTC_mV) * (KTC_mV)) + (MATH_mKTC_C3 * KTC_mV)
            + MATH_mKTC_C4;
}

inline float32 math_fnTTCmVtoTemp(float32 TTC_mV)
{
    return (MATH_mTTC_C1 * (TTC_mV) * (TTC_mV) * (TTC_mV))
            + (MATH_mTTC_C2 * (TTC_mV) * (TTC_mV)) + (MATH_mTTC_C3 * TTC_mV)
            + MATH_mTTC_C4;
}

inline float32 math_fnPRTmAtoBar(float32 PRT_mV, float32 SPAN, float32 offset)
{
    return (((PRT_mV - offset) / MATH_mPRT_RANGE) * SPAN) + MATH_mPRT_LRV;
}

inline float32 math_fnCOSmAtouSpercm(float32 COS_mV)
{
    return (((COS_mV - MATH_mCOS_MIN) / MATH_mCOS_RANGE) * MATH_mCOS_SPAN)
            + MATH_mCOS_LRV;
}


inline float32 math_fnRTDmAtoTemp(float32 RTD_mA)
{
    return ((RTD_mA * MATH_mRTDGAIN) - MATH_mRTDOFFSET);
}


//inline float32 math_fnDPmAtoTemp(float32 DP_mA)
//{
//    return (((DP_mA -  MATH_mDP_MIN)* MATH_mDP_GAIN) - MATH_mDP_TEMPOFFSET);
//}

// CJC TO mV conversion

inline float32 math_fnKTCCJCtomV(float32 CJC_temp)
{
    return (MATH_mCJC_KTC_C1 * (CJC_temp) * (CJC_temp) * (CJC_temp))
            + (MATH_mCJC_KTC_C2 * (CJC_temp) * (CJC_temp))
            + (MATH_mCJC_KTC_C3 * CJC_temp) + MATH_mCJC_KTC_C4;
}

inline float32 math_fnTTCCJCtomV(float32 CJC_temp)
{
    return (MATH_mCJC_TTC_C1 * (CJC_temp) * (CJC_temp) * (CJC_temp))
            + (MATH_mCJC_TTC_C2 * (CJC_temp) * (CJC_temp))
            + (MATH_mCJC_TTC_C3 * CJC_temp) + MATH_mCJC_TTC_C4;
}

void MATH_fnCalc(void)
{
//
////    CANA_tzAISensorData.DPT_401 = limitAnalogSensorData(
////            CANA_tzAISensorData.DPT_401);
//    CANA_tzAISensorData.HYS_101 = limitAnalogSensorData(
//            CANA_tzAISensorData.HYS_101);
//    CANA_tzAISensorData.HYS_102 = limitAnalogSensorData(
//            CANA_tzAISensorData.HYS_102);
//    CANA_tzAISensorData.HYS_501 = limitAnalogSensorData(
//            CANA_tzAISensorData.HYS_501);
//    CANA_tzAISensorData.HYS_401 = limitAnalogSensorData(
//            CANA_tzAISensorData.HYS_401);
//    CANA_tzAISensorData.LVL_101 = limitAnalogSensorData(
//            CANA_tzAISensorData.LVL_101);
////    CANA_tzAISensorData.PRT_101 = limitAnalogSensorData(
////            CANA_tzAISensorData.PRT_101);
////    CANA_tzAISensorData.PRT_102 = limitAnalogSensorData(
////            CANA_tzAISensorData.PRT_102);
////    CANA_tzAISensorData.PRT_401 = limitAnalogSensorData(
////            CANA_tzAISensorData.PRT_401);
////    CANA_tzAISensorData.PRT_402 = limitAnalogSensorData(
////            CANA_tzAISensorData.PRT_402);
////    CANA_tzAISensorData.TE_401 = limitAnalogSensorData(
////            CANA_tzAISensorData.TE_401);
//    CANA_tzAISensorData.COS_101 = limitAnalogSensorData(
//            CANA_tzAISensorData.COS_101);
//    CANA_tzAISensorData.OXS_101 = limitAnalogSensorData(
//            CANA_tzAISensorData.OXS_101);

    MATHConvtzRegs.AISensorLVL101 = math_fnLVLmAtoPercent(
            CANA_tzAISensorData.LVL_101);

    // MATHConvtzRegs.AISensorEBV801 = math_fnLVLmAtoPercent(f32EBVOutvalue);

    MATHConvtzRegs.AISensorHYS101 = math_fnHYSmAtoPercent(
            CANA_tzAISensorData.HYS_101, 0.0);

    MATHConvtzRegs.AISensorHYS501 = math_fnHYSmAtoPercent(
            CANA_tzAISensorData.HYS_501, 0.0);

    MATHConvtzRegs.AISensorHYS401 = math_fnHYSmAtoPercent(
            CANA_tzAISensorData.HYS_401, 0.0);

    MATHConvtzRegs.AISensorHYS102 = math_fnHYSmAtoPercent(
            CANA_tzAISensorData.HYS_102, 0);

    MATHConvtzRegs.AISensorOXS101 = math_fnOXSmAtoPercent(
            CANA_tzAISensorData.OXS_101, 0);

    if (CANA_tzThermoCoupleData.TTC_101 > 64.0)
    {
        CANA_tzThermoCoupleData.TTC_101 = 0;
    }

    if (CANA_tzThermoCoupleData.TTC_102 > 64.0)
    {
        CANA_tzThermoCoupleData.TTC_102 = 0;
    }

    if (CANA_tzThermoCoupleData.TTC_301 > 64.0)
    {
        CANA_tzThermoCoupleData.TTC_301 = 0;
    }

    if (CANA_tzThermoCoupleData.KTC_401 > 64.0)
    {
        CANA_tzThermoCoupleData.KTC_401 = 0;
    }

    MATHConvtzRegs.TempSensorTTC101 = math_fnTTCmVtoTemp(
            math_fnTTCCJCtomV(CANA_tzIORegs.CJC[CANA_mLHC10_IO])
                    + CANA_tzThermoCoupleData.TTC_101);
    MATHConvtzRegs.TempSensorTTC102 = math_fnTTCmVtoTemp(
            math_fnTTCCJCtomV(CANA_tzIORegs.CJC[CANA_mLHC10_IO])
                    + CANA_tzThermoCoupleData.TTC_102);

    MATHConvtzRegs.TempSensorTTC301 = math_fnTTCmVtoTemp(
            math_fnTTCCJCtomV(CANA_tzIORegs.CJC[CANA_mLHC10_IO])
                    + CANA_tzThermoCoupleData.TTC_301);

    MATHConvtzRegs.TempSensorKTC401 = math_fnKTCmVtoTemp(
            math_fnKTCCJCtomV(CANA_tzIORegs.CJC[CANA_mLHC10_IO])
                    + CANA_tzThermoCoupleData.KTC_401);

    MATHConvtzRegs.AISensorPRT101 = math_fnPRTmAtoBar(
            CANA_tzAISensorData.PRT_101, 4, 4.0);

    MATHConvtzRegs.AISensorPRT102 = math_fnPRTmAtoBar(
            CANA_tzAISensorData.PRT_102, 4, 4.0);

    MATHConvtzRegs.AISensorPRT401 = math_fnPRTmAtoBar(
            CANA_tzAISensorData.PRT_401, 40, 4.0);

    MATHConvtzRegs.AISensorPRT1 = math_fnPRTmAtoBar(
            CANA_tzAISensorData.PRT1, 40, 4.0);

    MATHConvtzRegs.AISensorCOS101 = math_fnCOSmAtouSpercm(
            CANA_tzAISensorData.COS_101);

    MATHConvtzRegs.AISensorTE401 = math_fnRTDmAtoTemp(
            CANA_tzAISensorData.TE_401);

}

//
///*==============================================================================
// End of File
//==============================================================================*/
