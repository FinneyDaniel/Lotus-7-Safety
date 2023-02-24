/*=============================================================================
 Copyright Enarka India Pvt Ltd (EIPL) All Rights Reserved.
 All trademarks are owned by Enarka India Private Limited
 ============================================================================ */

/*==============================================================================
 @file  safety_nv_mem_crc_test.c
 @author JOTHI RAMESH
 @date 26-Jan-2022

 @brief Description
 ==============================================================================*/

/*==============================================================================
 Includes
 ==============================================================================*/

#include "F28x_Project.h"
#include <stdint.h>
#include "safety_lib.h"
#include "safety_lib_prv.h"
#include "eep.h"
#include "oi_version.h"
#include "cana_defs.h"
/*==============================================================================
 Defines
 ==============================================================================*/
#define NV_MEM_EEPROM_ADDR  (0x0500U)
#define NV_MEM_CRC_ID       (0x7EE7U)
#define NV_MEM_EEPROM_LEN   (6U)
#define NUM_1KB_BLOCKS      (64U)
#define BLOCK_SIZE          (1024U)

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
void safety_fnNv_mem_crc_init(void);
void safety_nv_mem_crc_test(void);
uint32_t crc_fnCalculate(void);
/*==============================================================================
 Local Variables
 ==============================================================================*/
uint32_t u32Mem_CRC = 0;
uint32_t u32Mem_CRC_inv = 0;
/*==============================================================================
 Local Constants
 ==============================================================================*/

/*=============================================================================
 @function name: void safety_fnNv_mem_crc_init(void)
 @brief:All safety related function are called here. called @ safety_init.c/safety_fnNv_mem_crc_init
        @main loop
 @param :void
 @return: void
 ============================================================================ */
void safety_fnNv_mem_crc_init(void)
{
    // Read CRC data from EEPROM
    uint16_t uiData[12];
    if (!eep_read_word(CRC_mEEP_ADDRESS, uiData,
    CRC_mEEP_LENGTH))

        safety_fnPower_off(MEF_I2C);

    if ((uiData[0] == CRC_mEEP_ID) && (uiData[1] == FW_MAJOR)
            && (uiData[2] == FW_MINOR) && (uiData[3] == FW_BUILD))
    {

        u32Mem_CRC = ((uint32_t) uiData[4] << 24) | ((uint32_t) uiData[5] << 16)
                | (uiData[6] << 8) | (uiData[7] << 0);

        u32Mem_CRC_inv = ((uint32_t) uiData[8] << 24)
                | ((uint32_t) uiData[9] << 16) | (uiData[10] << 8) | uiData[11];
        if (u32Mem_CRC != ~u32Mem_CRC_inv)
        {
            safety_fnPower_off(MEF_CRC);
            //crc mismatch
        }
    }
    else
    {
        u32Mem_CRC = crc_fnCalculate();
        u32Mem_CRC_inv = ~u32Mem_CRC;
        uiData[0] = CRC_mEEP_ID;
        uiData[1] = FW_MAJOR;
        uiData[2] = FW_MINOR;
        uiData[3] = FW_BUILD;
        uiData[4] = (u32Mem_CRC >> 24) & 0xFF;
        uiData[5] = (u32Mem_CRC >> 16) & 0xFF;
        uiData[6] = (u32Mem_CRC >> 8) & 0xFF;
        uiData[7] = (u32Mem_CRC >> 0) & 0xFF;
        uiData[8] = (u32Mem_CRC_inv >> 24) & 0xFF;
        uiData[9] = (u32Mem_CRC_inv >> 16) & 0xFF;
        uiData[10] = (u32Mem_CRC_inv >> 8) & 0xFF;
        uiData[11] = (u32Mem_CRC_inv >> 0) & 0xFF;

        if (!eep_write_word(CRC_mEEP_ADDRESS, uiData,
        CRC_mEEP_LENGTH))

        {
            // power off
            safety_fnPower_off(MEF_I2C);
        }
    }
}
/*=============================================================================
 @function name: void safety_nv_mem_crc_test(void)
 @brief:All safety related function are called here. called @ safety_init.c/safety_nv_mem_crc_test
        @infinite loop
 @param :void
 @return: void
 ============================================================================ */

void safety_nv_mem_crc_test(void)
{

    uint32_t crc;
    uint32_t crc_inv;

    crc = crc_fnCalculate();
    crc_inv = ~crc;

    if ((crc != u32Mem_CRC) && (crc_inv != u32Mem_CRC_inv))
    {
        safety_fnPower_off(MEF_CRC);
        //CRC fail
    }
}
/*=============================================================================
 @function name: void crc_fnCalculate(void)
 @brief:All safety related function are called here. called @ safety_init.c/safety_nv_mem_crc_test/crc_fnCalculate
        @infinite loop
 @param :void
 @return: void
 ============================================================================ */

uint32_t crc_fnCalculate(void)
{
    uint16_t *p_flash = (uint16_t*) 0x088000UL;
    uint32_t u32CheckSum_safety = 0xFFFFFFFFUL;

    u32CheckSum_safety = can_fncomputeCRC(u32CheckSum_safety, p_flash,
    CRC_mMEM_SIZE);

    return u32CheckSum_safety;

}

/*==============================================================================
 End of File
 ==============================================================================*/
