/*=============================================================================
 Copyright Enarka India Pvt Ltd (EIPL) All Rights Reserved.
 All trademarks are owned by Enarka India Private Limited
 ============================================================================ */

/*==============================================================================
 @file  boot_flash.c
 @author Swathi BK
 @date 18-Jan-2022

 @brief Description
 ==============================================================================*/

/*==============================================================================
 Includes
 ==============================================================================*/

#include <stdint.h>
#include "F2837xS_device.h"
#include "F2837xS_Examples.h"
#include "boot_main.h"
#include "boot_flash.h"
#include "hw_init.h"
#include "F021_F2837xS_C28x.h"
#include "F021.h"
#include "eep.h"
/*==============================================================================
 Defines
 ==============================================================================*/

/*==============================================================================
 Enums
 ==============================================================================*/

/*==============================================================================
 Structures
 ==============================================================================*/
Fapi_StatusType boot_ReturnCheck;
Fapi_FlashStatusWordType boot_FlashStatusWord;
// Fapi_FlashStatusType boot_FlashStatus;
/*==============================================================================
 Macros
 ==============================================================================*/

/*==============================================================================
 Local Function Prototypes
 ==============================================================================*/
static bool boot_fnEraseFlash(void);
static void boot_fnFlashSectors(void);
static bool boot_fnProgarm_Flash(void);
static uint32_t boot_fncomputecodeCRC(uint32_t seed, const uint16_t *data,
                                      uint32_t len);
static bool boot_fnProgarm_Flash(void);
#define PUMPREQUEST *(unsigned long*)(0x00050024)

/*==============================================================================
 Local Variables
 ==============================================================================*/

/*==============================================================================
 Local Constants
 ==============================================================================*/
uint16_t u16CRCdata[8] = { 0 };
uint32_t u32CRCread = 0;
uint32_t u32CRCcalculated = 0;
uint32_t Boot_checksum = 0;
//#pragma DATA_SECTION(u32bootcrc_table, "boot_buffer")

const uint32_t u32bootcrc_table[256] = { 0x00000000, 0x30850FF5, 0x610A1FEA,
                                         0x518F101F, 0xC2143FD4, 0xF2913021,
                                         0xA31E203E, 0x939B2FCB, 0x159615F7,
                                         0x25131A02, 0x749C0A1D, 0x441905E8,
                                         0xD7822A23, 0xE70725D6, 0xB68835C9,
                                         0x860D3A3C, 0x2B2C2BEE, 0x1BA9241B,
                                         0x4A263404, 0x7AA33BF1, 0xE938143A,
                                         0xD9BD1BCF, 0x88320BD0, 0xB8B70425,
                                         0x3EBA3E19, 0x0E3F31EC, 0x5FB021F3,
                                         0x6F352E06, 0xFCAE01CD, 0xCC2B0E38,
                                         0x9DA41E27, 0xAD2111D2, 0x565857DC,
                                         0x66DD5829, 0x37524836, 0x07D747C3,
                                         0x944C6808, 0xA4C967FD, 0xF54677E2,
                                         0xC5C37817, 0x43CE422B, 0x734B4DDE,
                                         0x22C45DC1, 0x12415234, 0x81DA7DFF,
                                         0xB15F720A, 0xE0D06215, 0xD0556DE0,
                                         0x7D747C32, 0x4DF173C7, 0x1C7E63D8,
                                         0x2CFB6C2D, 0xBF6043E6, 0x8FE54C13,
                                         0xDE6A5C0C, 0xEEEF53F9, 0x68E269C5,
                                         0x58676630, 0x09E8762F, 0x396D79DA,
                                         0xAAF65611, 0x9A7359E4, 0xCBFC49FB,
                                         0xFB79460E, 0xACB0AFB8, 0x9C35A04D,
                                         0xCDBAB052, 0xFD3FBFA7, 0x6EA4906C,
                                         0x5E219F99, 0x0FAE8F86, 0x3F2B8073,
                                         0xB926BA4F, 0x89A3B5BA, 0xD82CA5A5,
                                         0xE8A9AA50, 0x7B32859B, 0x4BB78A6E,
                                         0x1A389A71, 0x2ABD9584, 0x879C8456,
                                         0xB7198BA3, 0xE6969BBC, 0xD6139449,
                                         0x4588BB82, 0x750DB477, 0x2482A468,
                                         0x1407AB9D, 0x920A91A1, 0xA28F9E54,
                                         0xF3008E4B, 0xC38581BE, 0x501EAE75,
                                         0x609BA180, 0x3114B19F, 0x0191BE6A,
                                         0xFAE8F864, 0xCA6DF791, 0x9BE2E78E,
                                         0xAB67E87B, 0x38FCC7B0, 0x0879C845,
                                         0x59F6D85A, 0x6973D7AF, 0xEF7EED93,
                                         0xDFFBE266, 0x8E74F279, 0xBEF1FD8C,
                                         0x2D6AD247, 0x1DEFDDB2, 0x4C60CDAD,
                                         0x7CE5C258, 0xD1C4D38A, 0xE141DC7F,
                                         0xB0CECC60, 0x804BC395, 0x13D0EC5E,
                                         0x2355E3AB, 0x72DAF3B4, 0x425FFC41,
                                         0xC452C67D, 0xF4D7C988, 0xA558D997,
                                         0x95DDD662, 0x0646F9A9, 0x36C3F65C,
                                         0x674CE643, 0x57C9E9B6, 0xC8DF352F,
                                         0xF85A3ADA, 0xA9D52AC5, 0x99502530,
                                         0x0ACB0AFB, 0x3A4E050E, 0x6BC11511,
                                         0x5B441AE4, 0xDD4920D8, 0xEDCC2F2D,
                                         0xBC433F32, 0x8CC630C7, 0x1F5D1F0C,
                                         0x2FD810F9, 0x7E5700E6, 0x4ED20F13,
                                         0xE3F31EC1, 0xD3761134, 0x82F9012B,
                                         0xB27C0EDE, 0x21E72115, 0x11622EE0,
                                         0x40ED3EFF, 0x7068310A, 0xF6650B36,
                                         0xC6E004C3, 0x976F14DC, 0xA7EA1B29,
                                         0x347134E2, 0x04F43B17, 0x557B2B08,
                                         0x65FE24FD, 0x9E8762F3, 0xAE026D06,
                                         0xFF8D7D19, 0xCF0872EC, 0x5C935D27,
                                         0x6C1652D2, 0x3D9942CD, 0x0D1C4D38,
                                         0x8B117704, 0xBB9478F1, 0xEA1B68EE,
                                         0xDA9E671B, 0x490548D0, 0x79804725,
                                         0x280F573A, 0x188A58CF, 0xB5AB491D,
                                         0x852E46E8, 0xD4A156F7, 0xE4245902,
                                         0x77BF76C9, 0x473A793C, 0x16B56923,
                                         0x263066D6, 0xA03D5CEA, 0x90B8531F,
                                         0xC1374300, 0xF1B24CF5, 0x6229633E,
                                         0x52AC6CCB, 0x03237CD4, 0x33A67321,
                                         0x646F9A97, 0x54EA9562, 0x0565857D,
                                         0x35E08A88, 0xA67BA543, 0x96FEAAB6,
                                         0xC771BAA9, 0xF7F4B55C, 0x71F98F60,
                                         0x417C8095, 0x10F3908A, 0x20769F7F,
                                         0xB3EDB0B4, 0x8368BF41, 0xD2E7AF5E,
                                         0xE262A0AB, 0x4F43B179, 0x7FC6BE8C,
                                         0x2E49AE93, 0x1ECCA166, 0x8D578EAD,
                                         0xBDD28158, 0xEC5D9147, 0xDCD89EB2,
                                         0x5AD5A48E, 0x6A50AB7B, 0x3BDFBB64,
                                         0x0B5AB491, 0x98C19B5A, 0xA84494AF,
                                         0xF9CB84B0, 0xC94E8B45, 0x3237CD4B,
                                         0x02B2C2BE, 0x533DD2A1, 0x63B8DD54,
                                         0xF023F29F, 0xC0A6FD6A, 0x9129ED75,
                                         0xA1ACE280, 0x27A1D8BC, 0x1724D749,
                                         0x46ABC756, 0x762EC8A3, 0xE5B5E768,
                                         0xD530E89D, 0x84BFF882, 0xB43AF777,
                                         0x191BE6A5, 0x299EE950, 0x7811F94F,
                                         0x4894F6BA, 0xDB0FD971, 0xEB8AD684,
                                         0xBA05C69B, 0x8A80C96E, 0x0C8DF352,
                                         0x3C08FCA7, 0x6D87ECB8, 0x5D02E34D,
                                         0xCE99CC86, 0xFE1CC373, 0xAF93D36C,
                                         0x9F16DC99,

};
/*=============================================================================
 @brief checks for new firmware & programs bank0 from bank1

 @param void
 @return void
 ============================================================================ */
bool boot_fnFlashprocess(void)
{
    bool bt_status = false;
    bool bt_done = false;
    uint16_t uidata, i = 0;

    // if (is_can_flash_cmd_received())
    {
        //initialize Flash
        boot_fnFlashSectors();

        for (i = 0; i < 4; i++)
        {
            boot_hw_eeprom_read(EEP_mCAN_CRC32_ADDR + i, &u16CRCdata[i]);
        }

        u32CRCread = (((uint32_t) u16CRCdata[0]) << 24)
                | (((uint32_t) u16CRCdata[1]) << 16)
                | ((u16CRCdata[2]) << 8) & 0xFF00 | u16CRCdata[3];

//        //calculate checksum
//        Boot_checksum = 0xFFFFFFFFUL;
//        Boot_checksum = boot_fncomputecodeCRC(Boot_checksum,
//                                              (uint16_t*) 0x0C8000UL,
//                                              BOOTm_FLASH_SIZE);

        uint16_t *uisrcLoc = (uint16_t*) 0x0C8000UL;
        Boot_checksum = 0xFFFFFFFFUL;
        Boot_checksum = boot_fncomputecodeCRC(Boot_checksum, uisrcLoc,
        BOOTm_FLASH_SIZE);

        if (u32CRCread == Boot_checksum)
        {
            //write to i2c flash started
            uidata = 0xFF;
            boot_hw_eeprom_write(BOOT_mCAN_FLASH_ADDR, &uidata);

            //erase APP sector
            bt_status = boot_fnEraseFlash();

            //clear the flag for new firmware
            uidata = 0xFF;
            boot_hw_eeprom_write(BOOT_mCAN_FLASH_ADDR, &uidata);
        }

        else
        {
            uidata = BOOT_mCAN_CRC_MISMATCH;
            boot_hw_eeprom_write(BOOT_mCAN_FLASH_ADDR, &uidata);
//            bt_status = false;
        }

    }

    if (bt_status)
    {

        //copy from TEMP to APP memory

        if (boot_fnProgarm_Flash())
        {
            //calculate checksum
            uint16_t *uisrcLoc = (uint16_t*) 0x088000UL;
            Boot_checksum = 0xFFFFFFFFUL;
            Boot_checksum = boot_fncomputecodeCRC(Boot_checksum, uisrcLoc,
            BOOTm_FLASH_SIZE);

            if (u32CRCread == Boot_checksum)
            {
                uidata = 0xFF;
                boot_hw_eeprom_write(BOOT_mCAN_ERROR_ADDR, &uidata);
                bt_done = true;
            }
            else
            {
                uidata = BOOT_mCAN_CRC_MISMATCH;
                boot_hw_eeprom_write(BOOT_mCAN_ERROR_ADDR, &uidata);
//                bt_done = false;
            }

        }

//        return bt_done;
    }

    return bt_done;
}
/*=============================================================================
 @brief to erase bank0

 @param void
 @return void
 ============================================================================ */
static bool boot_fnEraseFlash(void)
{
    uint16_t uiSectorCount = 0;
    uint32_t ui32sectorAddress;

    ui32sectorAddress = 0x088000; // COPY TP TEMP SPACE
//    ui32sectorSize = (CAN_mAPP_FLASH_LEN / 2) - 1;

    while (uiSectorCount < 7)
    {

        //
        // Execute an erase operation using the Flash API
        //
//        oReturnCheck = 0;
        boot_ReturnCheck = Fapi_issueAsyncCommandWithAddress(
                Fapi_EraseSector, (uint32_t*) ui32sectorAddress);

        //
        // Wait until the Flash State Machine is ready
        //
        while (Fapi_checkFsmForReady() != Fapi_Status_FsmReady)
        {
        }

        //
        // Perform a blank check on the recently erased sector to ensure it was erased
        //
//        oReturnCheck = 0;
        boot_ReturnCheck = Fapi_doBlankCheck((uint32_t*) ui32sectorAddress,
                                             0x4000, &boot_FlashStatusWord);

        //
        // Check the Flash API status for an error
        //
        if (boot_ReturnCheck != Fapi_Status_Success)
        {
            return false;
        }
        else
        {
            ui32sectorAddress += 0x008000;
            uiSectorCount++;
        }

    }

    return true;

}

/*=============================================================================
 @brief initFlashSectors - Initializes the flash API

 @param void
 @return void
 ============================================================================ */
static void boot_fnFlashSectors(void)
{
//    InitFlash_Bank0();
    EALLOW;

    PUMPREQUEST = 0x5A5A0002;

    Fapi_StatusType oReturnCheck;
    oReturnCheck = Fapi_initializeAPI(F021_CPU0_W0_BASE_ADDRESS, 200);
    if (oReturnCheck != Fapi_Status_Success)
    {
        //
        // Error code will be in the status parameter
        //
        __asm("    ESTOP0");
    }
    oReturnCheck = Fapi_setActiveFlashBank(Fapi_FlashBank0);
    if (oReturnCheck != Fapi_Status_Success)
    {
        //
        // Error code will be in the status parameter
        //
        __asm("    ESTOP0");
    }

    EDIS;
}

/*=============================================================================
 @brief to program flash

 @param DestAdrs: destination,uiBuff : source,uiLen : size in words
 @return void
 ============================================================================ */
static bool boot_fnProgarm_Flash(void)
{
    uint32_t DestAdrs = 0x088000;
    uint32_t uiLen = BOOTm_FLASH_SIZE;
    uint16_t *uisrcBuff = (uint16_t*) 0x0C8000;

    uint16_t uiWord;

    if (uiLen < 8)
        uiWord = (uint16_t) uiLen;
    else
        uiWord = 8;

//    oReturnCheck = 0;

    while (uiLen > 0)
    {

        boot_ReturnCheck = Fapi_issueProgrammingCommand((uint32_t*) DestAdrs,
                                                        uisrcBuff, uiWord, 0, 0,
                                                        Fapi_AutoEccGeneration);

        //
        // Wait until the Flash program operation is over
        //
        while (Fapi_checkFsmForReady() == Fapi_Status_FsmBusy)
            ;

        //
        // Check for an error
        //
        if (boot_ReturnCheck != Fapi_Status_Success)
        {

            return false;
        }

        //
        // Read FMSTAT register contents to know the status of FSM after
        // program command to see if there are any program operation related errors
        //
        while (Fapi_getFsmStatus())
            ;
        DestAdrs += (uiWord);
        uisrcBuff += (uiWord);
        uiLen -= uiWord;
        if (uiLen < 8)
        {
            uiWord = uiLen;
        }
    }

    return true;

}

/*=============================================================================
 @Function Name :can_fncomputeCRC
 @brief to compute CRC-32
 @param crc_accum : checksum,data_blk_ptr,data_blk_size
 @return void
 ============================================================================ */
static uint32_t boot_fncomputecodeCRC(uint32_t seed, const uint16_t *data,
                                      uint32_t len)
{
    uint16_t byte;
    uint32_t crc = seed;
    uint32_t i;

    for (i = 0; i < len; i++)
    {
        byte = (data[i] >> 8) & 0xFFU;
        crc = u32bootcrc_table[(crc ^ byte) & 0xFFU] ^ (crc >> 8);

        byte = data[i] & 0xFFU;
        crc = u32bootcrc_table[(crc ^ byte) & 0xFFU] ^ (crc >> 8);
    }

    return crc ^ 0xFFFFFFFFUL;

}
/*==============================================================================
 End of File
 ==============================================================================*/
