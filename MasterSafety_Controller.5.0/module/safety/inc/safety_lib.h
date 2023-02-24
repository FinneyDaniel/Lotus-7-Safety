/*=============================================================================
 Copyright Enarka India Pvt Ltd (EIPL) All Rights Reserved.
 All trademarks are owned by Enarka India Private Limited
 ============================================================================ */

/*==============================================================================
 @file  safety_lib.h
 @author JOTHI RAMESH
 @date 26-Jan-2022

 @brief Description
 ==============================================================================*/

#ifndef MODULE_SAFETY_INC_SAFETY_LIB_H_
#define MODULE_SAFETY_INC_SAFETY_LIB_H_

/*==============================================================================
 Includes
 ==============================================================================*/

#include "F28x_Project.h"
#include <stdint.h>
#include <stdbool.h>
/*==============================================================================
 Defines
 ==============================================================================*/
// Use #define or #undef as per need
#undef MICROELECTRONIC_TESTING_ENABLE

typedef unsigned char safe_bool;
#define safe_true     (0x1U)
#define safe_false    (0x2U)

#define HW_mFAULT_STATUS                         GpioDataRegs.GPADAT.bit.GPIO22
#define HW_mFAULT_FB_STATUS                      GpioDataRegs.GPADAT.bit.GPIO23

#define HW_mFAULT                                (0U)
#define HW_mNO_FAULT                             (1U)

#define MAJ_VER                     (5U)
#define MIN_VER                     (2U)
#define DEV_VER                     (2001U)
#define NUM_mTIME_SLOTS             (5U)
#define COMMUNICATION_TIMEOUT       (60U) // 60 seconds
#define FAULT_TIMEOUT               (1U)  // seconds
/*==============================================================================
 Enums
 ==============================================================================*/
typedef enum
{
    MEF_UNKNOWN = 0,
    MEF_IO = 0x01U,
    MEF_FPU = 0x02U,
    MEF_CRC = 0x03U,
    MEF_CANCOM = 0x04U,
    MEF_CPU = 0x05U,
    MEF_TIMING = 0x06U,
    MEF_LOGICAL_MONITORING = 0x07U,
    MEF_WDOG = 0x08U,
    MEF_INVALID_ISR = 0x09U,
    MEF_ILLEGAL_INSTR = 0x0AU,
    MEF_INVALID_MEM = 0x0BU,
    MEF_I2C = 0x0CU,
    MEF_VCU = 0x0DU,
    MEF_LAST_FAULT = 0x0EU
} microelectronic_fault_t;

/*==============================================================================
 Structures
 ==============================================================================*/
extern uint16_t uiRA_state_flag;
/*==============================================================================
 Macros
 ==============================================================================*/

/*==============================================================================
 Extern/Public Function Prototypes
 ==============================================================================*/
extern void safety_fninit(void);
extern void safety_fnPower_off(microelectronic_fault_t fault);
extern void safety_fnlog_monitoring_chk(void);
extern void safety_fnLog_monitoring_slot_exe(uint16_t slot);
extern void sl_fnCPU_RegistersTest(void);
extern void sl_fnFPU_RegistersTest(void);
extern void sl_fnVCU_RegistersTest(void);
extern void safety_fnslotAll(void);
extern void safety_nv_mem_crc_test(void);
extern void safety_cancom(void);
/*==============================================================================
 Extern/Public Variables
 ==============================================================================*/

/*==============================================================================
 Extern/Public Constants
 ==============================================================================*/

#endif /* MODULE_SAFETY_INC_SAFETY_LIB_H_ */
