/*=============================================================================
 Copyright Enarka India Pvt Ltd (EIPL) All Rights Reserved.
 All trademarks are owned by Enarka India Private Limited
 ============================================================================ */

/*==============================================================================
 @file  boot_main.c
 @author OHMLAP0107   
 @date 31-Dec-2021

 @brief Description
 ==============================================================================*/

/*==============================================================================
 Includes
 ==============================================================================*/

#include "boot_main.h"
#include "hw_init.h"
#include "F2837xS_device.h"
#include "F2837xS_Examples.h"
#include <stdint.h>
#include "eep.h"
#include "hw_init.h"
#include "boot_main.h"
#include "boot_flash.h"


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
static void boot_fnCopyFlashToRAM(void);
static void boot_fnRead_CANflashcmd(void);

/*==============================================================================
 Local Variables
 ==============================================================================*/

uint32_t count = 0;
int32_t uiCheck = 0;
uint16_t CANfalshRequest = 0;
static uint16_t cmd = 0;
static uint16_t err = 0;

uint16_t I2Csuccess = 0;

/*==============================================================================
 Local Constants
 ==============================================================================*/

/*=============================================================================
 @brief the boot main where tasks are executed is defined here

 @param void
 @return void
 ============================================================================ */
void boot_main(void)
{

    // basic hardware initialization
    BOOT_fnInit();

    //copy flash to RAM
    boot_fnCopyFlashToRAM();

    //Hardware initialization
    BOOT_fnHwInit();

    boot_fnRead_CANflashcmd();

    if (is_can_flash_cmd_received())
    {



        if(boot_fnFlashprocess())
        {
            boot_fnHWreset();
        }
    }

    boot_fnHW_deinit();


}

/*=============================================================================
 @brief to copy boot code from flash to RAM

 @param void
 @return void
 ============================================================================ */
static void boot_fnCopyFlashToRAM(void)
{
    extern uint32_t boot_flash_code_run_start;
    extern uint32_t boot_flash_code_load_start;
    extern uint32_t boot_flash_code_size;
    uint16_t *p_dst = (uint16_t*) &boot_flash_code_run_start;
    uint16_t *p_src = (uint16_t*) &boot_flash_code_load_start;
    uint32_t size = (uint32_t) &boot_flash_code_size;

    while (size)
    {
        *p_dst = *p_src;
        ++p_dst;
        ++p_src;
        --size;
    }

    //extern uint32_t boot_cinit_start;
    //boot_copy_in((COPY_TABLE *)&boot_cinit_start);
}

/*=============================================================================
 @brief Read can-flash command & error command in eeprom address

 @param void
 @return void
 ============================================================================ */
static void boot_fnRead_CANflashcmd(void)
{
    if (boot_hw_eeprom_read(EEP_mCAN_FLASH_ADDR, &cmd) == false)
    {
        cmd = 0xFF;
    }

    if (boot_hw_eeprom_read(EEP_mCAN_ERROR_ADDR, &err) == false)
    {
        err = 0xFF;
    }
}

/*=============================================================================
 @brief To check new firmware availability

 @param void
 @return void
 ============================================================================ */
bool is_can_flash_cmd_received(void)
{
    if (cmd == BOOT_mCAN_NEW_FIRMWARE)
        return true;
    else
        return false;
}
/*==============================================================================
 End of File
 ==============================================================================*/
