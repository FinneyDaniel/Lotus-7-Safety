/*=============================================================================
 Copyright Enarka India Pvt Ltd (EIPL) All Rights Reserved.
 All trademarks are owned by Enarka India Private Limited
 ============================================================================ */

/*==============================================================================
 @file  canaFlashEvent.c
 @author DEEPTI.K
 @date 24-Jan-2022

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
#include "F021_F2837xS_C28x.h"
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

Fapi_StatusType oReturnCheck;
Fapi_FlashStatusWordType oFlashStatusWord;
Fapi_FlashStatusType oFlashStatus;

/*==============================================================================
 Macros
 ==============================================================================*/

static const uint32_t crc_table[256] = { 0x00000000, 0x30850FF5, 0x610A1FEA,
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

/*==============================================================================
 Local Function Prototypes
 ==============================================================================*/

void CANA_FlashEvent(void);
static bool can_fnFWupdate_cmd(void);
static bool can_fnEraseFlash(void);
static bool can_fnProgram_Flash(uint32_t DestAdrs, uint16_t *uiBuff,
                                uint16_t uiLen, uint16_t *uierr);
static void can_fnsendACK(uint16_t uiControlByte, uint16_t uiErr,
                          uint32_t u32chksum, uint16_t uiBL);
uint32_t can_fncomputeCRC(uint32_t seed, const uint16_t *data, uint32_t len);
void Init_fnHWreset(void);

/*==============================================================================
 Local Variables
 ==============================================================================*/

uint16_t ui16Rx1MsgDataFlash[8] = { 0 };
uint16_t ui16Rx2MsgDataFlash[8] = { 0 };

uint16_t ui16TxACKdataFlash[8] = { 0 };

uint16_t ui16btcmd = 0, ui16btStatus = 0, ui32SectorCount = 0,
        ui16ErrorCode = 0;
uint32_t ui32DataLength = 0;
uint32_t ui32Checksum = 0;
uint16_t uiFlashTxMsg[8] = { 0 };
uint16_t uiFlashRxbuf[8] = { 0 };
uint16_t uiFlashRxdatabuf[8] = { 0 };
uint16_t u32_DataBuffer[CAN_mBUFFER_SIZE];
uint32_t u32_DataSize = 0, uiframeCount = 0;
uint16_t uiErrorCode = 0;
uint16_t uiControlCmd = 0;
uint16_t uiI2Ccodeflag = 0, CANARead = 0;
volatile uint32_t timeout_cnt = 0;
uint16_t ui16PrevValue = 0x80;

uint32_t ui32CodeChecksum = 0;
uint16_t ui16BurstLen = 0, k, l;
uint16_t *p_buff;
uint32_t ui32dst_addr = 0x000C8000;

bool bt_pgm_done = false;
bool uiStatus, msgsts = false;
bool bt_toggle = false;
extern uint16_t i2cChkRead;

/*==============================================================================
 Local Constants
 ==============================================================================*/

/*=============================================================================
 @Function Name :void CANA_FlashEvent()
 @brief to check for any new message
 function called in scheduler.c / canaFlashEvent.c   @50msec
 @param :void
 @return :void
 ============================================================================ */

void CANA_FlashEvent()
{

    if (CAN_IsMessageReceived(CANA_BASE, CAN_mMAILBOX_1))
    {

        ui16btcmd = can_fnFWupdate_cmd();

        if (ui16btcmd)
        {

            //indicat   e eeprom flash process started
            uieepData[0] = 0x11;

            EEP_fnWrite(&transaction_I2CMsg, EEP_mCAN_ERROR_ADDR, &uieepData[0],
                        1);

            ui32DataLength = (((uint32_t) (ui16Rx1MsgDataFlash[3])) << 16)
                    | (ui16Rx1MsgDataFlash[2] << 8)
                    | (ui16Rx1MsgDataFlash[1] << 0);

            uieepData[0] = (uint16_t) (ui32DataLength & 0xFF);
            uieepData[1] = (uint16_t) ((ui32DataLength >> 8) & 0xFF);
            uieepData[2] = (uint16_t) ((ui32DataLength >> 16) & 0xFF);

            EEP_fnWrite(&transaction_I2CMsg, EEP_mCAN_LENGTH_ADDR,
                        &uieepData[0], 3);

            ui16btStatus = can_fnEraseFlash();

            if (ui16btcmd & ui16btStatus)
            {
                ui16ErrorCode = 0;
                ui32Checksum = 0xFFFFFFFFUL;
                ui32Checksum = can_fncomputeCRC(ui32Checksum,
                                                &ui16Rx1MsgDataFlash, 8);

                //clear the command
                uieepData[0] = 0xFF;
                EEP_fnWrite(&transaction_I2CMsg, EEP_mCAN_ERROR_ADDR,
                            &uieepData[0], 1);

            }
            else
            {
                ui16ErrorCode = CANA_mFLASH_WRITE_FAIL;
                uieepData[0] = ui16ErrorCode;
                EEP_fnWrite(&transaction_I2CMsg, EEP_mCAN_ERROR_ADDR,
                            &uieepData[0], 1);
            }

            //send acknowledgment

            can_fnsendACK(ui16Rx1MsgDataFlash[0], ui16ErrorCode, ui32Checksum,
                          0x20);

            do
            {
                //check for previous flash fail
            }
            while (!CAN_IsMessageReceived(CANA_BASE, CAN_mMAILBOX_1));

            p_buff = &u32_DataBuffer[0];

            do
            {

                CAN_readMessage(CANA_BASE, CAN_mMAILBOX_1, ui16Rx2MsgDataFlash);
                timeout_cnt = 0;

                if (ui16PrevValue == ui16Rx2MsgDataFlash[0])
                {
                    uiErrorCode = CANA_mFLASH_WRITE_FAIL;
                    bt_pgm_done = true;
                    break;
                }
                else
                {
                    ui16PrevValue = ui16Rx2MsgDataFlash[0];
                    if ((ui16PrevValue == 0x80) || (ui16PrevValue == 0x00))
                    {
                        *p_buff++ = ui16Rx2MsgDataFlash[1];
                        *p_buff++ = ui16Rx2MsgDataFlash[2];
                        *p_buff++ = ui16Rx2MsgDataFlash[3];
                        *p_buff++ = ui16Rx2MsgDataFlash[4];
                        *p_buff++ = ui16Rx2MsgDataFlash[5];
                        *p_buff++ = ui16Rx2MsgDataFlash[6];
                        *p_buff++ = ui16Rx2MsgDataFlash[7];

                        ui16BurstLen++;

                        if (p_buff >= &u32_DataBuffer[CAN_mBUFFER_SIZE])
                        {
                            p_buff = &u32_DataBuffer[0];

                            l = 0;
                            for (k = 0; k < CAN_mBUFFER_SIZE; k++)
                            {
                                u32_DataBuffer[l] = (u32_DataBuffer[k] << 8)
                                        | u32_DataBuffer[k + 1];
                                l++;
                                k++;
                            }

                            uiStatus = can_fnProgram_Flash(ui32dst_addr,
                                                           u32_DataBuffer,
                                                           CAN_mBUFFER_SIZE / 2,
                                                           &uiErrorCode);

                            if (uiStatus == 1)
                            {
                                ui32dst_addr += (CAN_mBUFFER_SIZE / 2);
                                ui32Checksum = can_fncomputeCRC(
                                        ui32Checksum, u32_DataBuffer,
                                        CAN_mBUFFER_SIZE);
                                uiErrorCode = 0;
                                can_fnsendACK(uiFlashRxdatabuf[0], uiErrorCode,
                                              ui32Checksum, (ui16BurstLen));
                                ui16BurstLen = 0;
                                uiframeCount++;
                            }
                            else
                            {
                                //   uiErrorCode = uiErrorCode;
                                bt_pgm_done = true;
//                            break;
                            }

                            if (ui32DataLength == (uiframeCount * 32 * 7))
                            {
                                bt_pgm_done = true;
                            }

                        }
                    }

                }

                uiErrorCode = 0; //????
                msgsts = CAN_IsMessageReceived(CANA_BASE, CAN_mMAILBOX_1);
                while (!msgsts)
                {
                    msgsts = CAN_IsMessageReceived(CANA_BASE, CAN_mMAILBOX_1);

                    if (++timeout_cnt >= (APPROX_1_SECOND_DELAY * 10))
                    {
                        uiErrorCode = CAN_mFLASH_ERR_COMM_TIMEOUT;
                        bt_pgm_done = true;
                        break;
                    }
                }

            }
            while (!bt_pgm_done);

            if (ui16ErrorCode != 0)
            {
                can_fnsendACK(uiFlashRxdatabuf[0], ui16ErrorCode, ui32Checksum,
                              0x20);
                //copy to I2C
                uieepData[0] = ui16ErrorCode;
                EEP_fnWrite(&transaction_I2CMsg, EEP_mCAN_ERROR_ADDR,
                            &uieepData[0], 1);
            }
            else
            {
                while (!CAN_IsMessageReceived(CANA_BASE, CAN_mMAILBOX_1))
                    ;
                CAN_readMessage(CANA_BASE, CAN_mMAILBOX_1, uiFlashRxdatabuf);
                if (uiFlashRxdatabuf[0] == 0xA0)
                {
                    //read checksum
                    ui32CodeChecksum = (((uint32_t) uiFlashRxdatabuf[7] << 24)
                            | ((uint32_t) uiFlashRxdatabuf[6] << 16)
                            | (uiFlashRxdatabuf[5] << 8)
                            | (uiFlashRxdatabuf[4] << 0));

                    uint16_t *uisrcLoc = (uint16_t*) 0x0C8000UL;
                    ui32Checksum = 0xFFFFFFFFUL;
                    ui32Checksum = can_fncomputeCRC(ui32Checksum, uisrcLoc,
                                                    ui32DataLength / 2);

                    if (ui32Checksum != ui32CodeChecksum)
                    {
                        uiErrorCode = CAN_mFLASH_CHKSUM_MISMATCH;
                        //COPY TO I2C
                        uieepData[0] = uiErrorCode;
                        EEP_fnWrite(&transaction_I2CMsg, EEP_mCAN_ERROR_ADDR,
                                    &uieepData[0], 1);
                        uiI2Ccodeflag = 0x00;
                    }
                    else
                    {
                        //write to I2C new code available
                        uiI2Ccodeflag = 0xAA;
                        uieepData[0] = ((uint32_t) ui32Checksum >> 24);
                        uieepData[1] = ((uint32_t) ui32Checksum >> 16) & 0xFF;
                        uieepData[2] = ((uint32_t) ui32Checksum >> 8) & 0xFF;
                        uieepData[3] = ((uint32_t) ui32Checksum >> 0) & 0xFF;

                        EEP_fnWrite(&transaction_I2CMsg, EEP_mCAN_CRC32_ADDR,
                                    &uieepData[0], 4);

                        DELAY_US(100);

//                        EEP_fnRead(&transaction_I2CMsg, EEP_mCAN_LENGTH_ADDR,
//                                   &uireadData[0], 4);
                        //reset
                    }

                    can_fnsendACK(uiFlashRxdatabuf[0], uiErrorCode,
                                  ui32Checksum, 0x20);
                }

            }

        }

        //keep some delay
        //write to I2C new code available
        if (uiI2Ccodeflag == 0xAA)
        {
            uieepData[0] = uiI2Ccodeflag;
            EEP_fnWrite(&transaction_I2CMsg, EEP_mCAN_FLASH_ADDR, &uieepData[0],
                        1);

            DELAY_US(10);

            //COPY TO I2C

            Init_fnHWreset();
        }
    }

}

/*=============================================================================
 @FUNCTION name: static bool can_fnEraseFlash(void)
 @brief :infinite loop for the main where tasks are executed is defined here
 function called in scheduler.c / canaFlashEvent.c / can_fnEraseFlash @50msec
 @param :void
 @return :bool
 ============================================================================ */
static bool can_fnEraseFlash(void)
{
    uint16_t ui32SectorCount = 0;
    uint32_t ui32sectorAddress;

    ui32sectorAddress = 0x0C8000; // COPY TP TEMP SPACE
//    ui32sectorSize = (CAN_mAPP_FLASH_LEN / 2) - 1;

    while (ui32SectorCount < 7)
    {

        //
        // Execute an erase operation using the Flash API
        //
//        oReturnCheck = 0;
        oReturnCheck = Fapi_issueAsyncCommandWithAddress(
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
        oReturnCheck = Fapi_doBlankCheck((uint32_t*) ui32sectorAddress, 0x4000,
                                         &oFlashStatusWord);

        //
        // Check the Flash API status for an error
        //
        if (oReturnCheck != Fapi_Status_Success)
        {
            return false;
        }
        else
        {
            ui32sectorAddress += 0x08000;
            ui32SectorCount++;
        }

    }

    return true;

}

/*=============================================================================
 @Function name :static bool can_fnFWupdate_cmd(void)
 @brief:  function called in scheduler.c / canaFlashEvent.c / can_fnFWupdate_cmd @50msec
 @param :void
 @return:bool
 ============================================================================ */
static bool can_fnFWupdate_cmd(void)
{
    CAN_readMessage(CANA_BASE, CAN_mMAILBOX_1, ui16Rx1MsgDataFlash);

    if (ui16Rx1MsgDataFlash[0] == 0x90)
    {
        return true;
    }
    else
    {
        return false;
    }
}

/*=============================================================================
 @function name  : static void can_fnsendACK(uint16_t uiControlByte, uint16_t uiErr,
 uint32_t u32chksum, uint16_t uiBL)
 @brief: infinite loop for the main where tasks are executed is defined here
 function called in scheduler.c / canaFlashEvent.c / can_fnsendACK  @50msec
 @param :(uint16_t uiControlByte, uint16_t uiErr,
 uint32_t u32chksum, uint16_t uiBL)
 @return: void
 ============================================================================ */
static void can_fnsendACK(uint16_t uiControlByte, uint16_t uiErr,
                          uint32_t u32chksum, uint16_t uiBL)
{

    uint16_t ui16ACK;

    CAN_setupMessageObject(CANA_BASE, CAN_mMAILBOX_2, 0x1F143000,
                           CAN_MSG_FRAME_EXT, CAN_MSG_OBJ_TYPE_TX, 0,
                           CAN_MSG_OBJ_NO_FLAGS,
                           CANA_mEIGHT_BYTE);

    if (bt_toggle == false)
    {
        ui16ACK = 0xB0;
    }
    else
    {
        ui16ACK = 0x30;
    }
    bt_toggle = !bt_toggle;

    if (uiErr == 0)
    {
        ui16TxACKdataFlash[0] = ui16ACK;
        ui16TxACKdataFlash[1] = uiBL;
        ui16TxACKdataFlash[2] = 0x00;
        ui16TxACKdataFlash[3] = 0x00;
        ui16TxACKdataFlash[4] = u32chksum & 0xFF;
        ui16TxACKdataFlash[5] = (u32chksum >> 8) & 0XFF;
        ui16TxACKdataFlash[6] = (u32chksum >> 16) & 0xFF;
        ui16TxACKdataFlash[7] = (u32chksum >> 24) & 0xFF;
    }
    else
    {
        ui16TxACKdataFlash[0] = CANA_mFLASH_ERRORBYTE;
        ui16TxACKdataFlash[1] = uiErr & 0xFF;
        ui16TxACKdataFlash[2] = (uiErr >> 8) & 0xFF;
        ui16TxACKdataFlash[3] = 0x00;
        ui16TxACKdataFlash[4] = 0x00;
        ui16TxACKdataFlash[5] = 0x00;
        ui16TxACKdataFlash[6] = 0x00;
        ui16TxACKdataFlash[7] = 0x00;
    }

    CAN_sendMessage(CANA_BASE, CAN_mMAILBOX_2, CANA_mEIGHT_BYTE,
                    ui16TxACKdataFlash);

}

uint32_t can_fncomputeCRC(uint32_t seed, const uint16_t *data, uint32_t len)
{
    uint16_t byte;
    uint32_t crc = seed;
    uint32_t i;

    for (i = 0; i < len; i++)
    {
        byte = (data[i] >> 8) & 0xFFU;
        crc = crc_table[(crc ^ byte) & 0xFFU] ^ (crc >> 8);

        byte = data[i] & 0xFFU;
        crc = crc_table[(crc ^ byte) & 0xFFU] ^ (crc >> 8);
    }

    return crc ^ 0xFFFFFFFFUL;
}
/*=============================================================================
 @FUNCTION name:static bool can_fnProgram_Flash(uint32_t DestAdrs, uint16_t *uiBuff,
 uint16_t uiLen, uint16_t *uierr)

 @brief :function called in scheduler.c / canaFlashEvent.c / can_fnProgram_Flash  @50msec
 @param :(uint32_t DestAdrs, uint16_t *uiBuff,
 uint16_t uiLen, uint16_t *uierr)
 @return :bool
 ============================================================================ */
static bool can_fnProgram_Flash(uint32_t DestAdrs, uint16_t *uiBuff,
                                uint16_t uiLen, uint16_t *uierr)
{
    uint16_t uiWord;

    if (uiLen < 8)
        uiWord = uiLen;
    else
        uiWord = 8;

    oFlashStatus = Fapi_getFsmStatus();
    //    oReturnCheck = 0;

    while (uiLen > 0)
    {
        oFlashStatus = 0;
        oReturnCheck = Fapi_issueProgrammingCommand((uint32*) DestAdrs, uiBuff,
                                                    uiWord, 0, 0,
                                                    Fapi_AutoEccGeneration);

        //
        // Wait until the Flash program operation is over
        //
        while (Fapi_checkFsmForReady() == Fapi_Status_FsmBusy)
            ;

        //
        // Check for an error
        //
        if (oReturnCheck != Fapi_Status_Success)
        {

            *uierr = CAN_mFLASH_WRITE_FAIL;
            return false;
        }
        else
        {
            *uierr = 0;
        }

        //
        // Read FMSTAT register contents to know the status of FSM after
        // program command to see if there are any program operation related errors
        //
        oFlashStatus = Fapi_getFsmStatus();
        while (oFlashStatus)
            ;

        DestAdrs += uiWord;
        uiBuff += uiWord;
        uiLen -= uiWord;
        if (uiLen < 8)
        {
            uiWord = uiLen;
        }
    }

    return true;

}

/*==============================================================================
 End of File
 ==============================================================================*/
