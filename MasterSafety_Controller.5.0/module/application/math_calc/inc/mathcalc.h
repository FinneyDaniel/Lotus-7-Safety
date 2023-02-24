/*=============================================================================
Copyright Enarka India Pvt Ltd (EIPL) All Rights Reserved.
All trademarks are owned by Enarka India Private Limited
============================================================================ */

/*==============================================================================
 @file  mathcalc.h
 @author JOTHI RAMESH
 @date 06-Sep-2021

 @brief Description
==============================================================================*/

#ifndef MODULE_MATH_CALC_INC_MATHCALC_H_
#define MODULE_MATH_CALC_INC_MATHCALC_H_

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
#include "mathcalc.h"

/*==============================================================================
 Defines
==============================================================================*/

#define MATH_mLVL_RANGE                 (16.0f)
#define MATH_mLVL_MIN                   (4.0f)

#define MATH_mHYS_RANGE                 (-16.0f)
#define MATH_mHYS_MIN                   (4.0f)
#define MATH_mOXS_MIN                   (4.0f)



#define MATH_mKTC_C1                    (0.0563f)
#define MATH_mKTC_C2                    (-0.5112f)
#define MATH_mKTC_C3                    (25.489f)
#define MATH_mKTC_C4                    (0.1386f)

#define MATH_mTTC_C1                    (0.0439f)
#define MATH_mTTC_C2                    (-0.8404f)
#define MATH_mTTC_C3                    (26.079f)
#define MATH_mTTC_C4                    (0.1605f)

#define MATH_mPRT_MIN                    (4.0f)
#define MATH_mPRT_RANGE                  (16.0f)
#define MATH_mPRT_LRV                     (0.0f)

#define MATH_mCOS_MIN                    (4.0f)
#define MATH_mCOS_RANGE                  (16.0f)
#define MATH_mCOS_SPAN                   (19.98f)
#define MATH_mCOS_LRV                    (0.02f)

#define MATH_mRTDGAIN                        (9.34891f)
#define MATH_mRTDOFFSET                      (41.90f)


// CJC TO mV conversion
#define MATH_mCJC_KTC_C1                 (-1.0e-7)
#define MATH_mCJC_KTC_C2                 (3.0e-5)
#define MATH_mCJC_KTC_C3                 (0.0394)
#define MATH_mCJC_KTC_C4                 (-0.0008)

#define MATH_mCJC_TTC_C1                 (-3.0e-8)
#define MATH_mCJC_TTC_C2                 (5.0e-5)
#define MATH_mCJC_TTC_C3                 (0.0386)
#define MATH_mCJC_TTC_C4                 (-0.0029)
/*==============================================================================
 Enums
==============================================================================*/

/*==============================================================================
 Structures
==============================================================================*/

typedef struct MATHCONV_zREGS
{
    float32_t AISensorLVL101;
    float32_t AISensorEBV801;
    float32_t AISensorHYS101;
    float32_t AISensorHYS501;
    float32_t AISensorHYS401;
    float32_t AISensorHYS102;
    float32_t TempSensorTTC101;
    float32_t TempSensorTTC102;
    float32_t TempSensorTTC301;
    float32_t TempSensorKTC401;
    float32_t AISensorPRT101;
    float32_t AISensorPRT102;
    float32_t AISensorPRT401;
    float32_t AISensorPRT1;
    float32_t AISensorCOS101;
    float32_t AISensorOXS101;
    float32_t AISensorTE401;



}MATHCONV_tzREGS;

/*==============================================================================
 Macros
==============================================================================*/

/*==============================================================================
 Extern/Public Function Prototypes
==============================================================================*/

extern void MATH_fnCalc(void);

/*==============================================================================
 Extern/Public Variables
==============================================================================*/

extern MATHCONV_tzREGS MATHConvtzRegs;

/*==============================================================================
 Extern/Public Constants
==============================================================================*/


#endif /* MODULE_MATH_CALC_INC_MATHCALC_H_ */
