/*=============================================================================
Copyright Enarka India Pvt Ltd (EIPL) All Rights Reserved.
All trademarks are owned by Enarka India Private Limited
============================================================================ */

/*==============================================================================
 @file  safety_defs.h
 @author JOTHI RAMESH
 @date 26-Jan-2022

 @brief Description
==============================================================================*/

#ifndef MODULE_SAFETY_INC_SAFETY_LIB_PRV_H_
#define MODULE_SAFETY_INC_SAFETY_LIB_PRV_H_

/*==============================================================================
 Includes
==============================================================================*/

#include "F28x_Project.h"
#include <stdint.h>

/*==============================================================================
 Defines
==============================================================================*/
#define REG_mTEST_PATTERN_STUCK_AT_FAULT     (0xAAAAU)
#define N_REG_mTEST_PATTERN_STUCK_AT_FAULT   (0x5555U)
#define REG_mTEST_PATTERN_DC_FAULT           (0x0000U)
#define N_REG_mTEST_PATTERN_DC_FAULT         (0xFFFFU)

#define UNIQUE_ID                            (0x1806U)
// register test
#define   SIG_CPU_REG_TEST              0xA029U
#define   SIG_FPU_REG_TEST              0xA02AU
#define   SIG_VCU_REG_TEST              0xA02BU
// Test pass fail
#define TEST_mPASSED     1U
#define TEST_mFAILED     0U
#define LIB_mTEST_VCU_TEST_ERROR    0
#define LIB_mTEST_FPU_TEST_ERROR    0
#define LIB_mTEST_CPU_TEST_ERROR    0
#define VCU_mERROR_CODE             0xCC
#define FPU_mERROR_CODE             0xCC
#define CPU_mERROR_CODE             0xCC

#define   JUMP_mTO_FAILSAFE       0

#define CRC_mEEP_ADDRESS                (0x64)
#define CRC_mEEP_LENGTH                 (12U)
#define CRC_mEEP_ID                     (0xDD)
#define CRC_mMEM_SIZE                   (0xC000)
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
 Extern/Public Function Prototypes
==============================================================================*/
void safety_fnLog_Monitoring_init(void);
uint16_t calc_crc_buff(uint16_t crc, const uint16_t *data, uint16_t len);
extern bool eep_read_word(uint16_t address, uint16_t *data, uint16_t bytes);
extern bool eep_write_word(uint16_t address, uint16_t *data, uint16_t bytes);
void safety_fnNv_mem_crc_init(void);
/*==============================================================================
 Extern/Public Variables
==============================================================================*/

/*==============================================================================
 Extern/Public Constants
==============================================================================*/


#endif /* MODULE_SAFETY_INC_SAFETY_LIB_PRV_H_ */
