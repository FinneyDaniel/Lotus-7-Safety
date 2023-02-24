/*=============================================================================
Copyright Enarka India Pvt Ltd (EIPL) All Rights Reserved.
All trademarks are owned by Enarka India Private Limited
============================================================================ */

/*==============================================================================
 @file  hw_init.c
 @author OHMLAP0107   
 @date 06-Jan-2022

 @brief Description
==============================================================================*/

/*==============================================================================
 Includes
==============================================================================*/

#include <module/boot/inc/hw_init.h>
#include "F2837xS_device.h"
#include "F2837xS_Examples.h"
#include "F28x_Project.h"

#include <stdint.h>

#include "device.h"
/*==============================================================================
 Defines
==============================================================================*/
#define BOOT_CPU_RATE                10.00L
#define BOOT_PLL_LOCK_TIMEOUT        2000
#define BOOT_DELAY_US(A)            {\
                                        uiTime_count =(uint32_t)((((long double)(A * 1000.0)/(long double)BOOT_CPU_RATE) - 9)/5); \
                                        while(uiTime_count > 0) \
                                        {\
                                            uiTime_count--; \
                                        }\
                                    }\



#define I2C_WAIT_FOR(condition, timeout)        do { \
                                                    timeout_cnt = 0; \
                                                    while ((condition)) \
                                                    { \
                                                        timeout_cnt++; \
                                                        if (timeout_cnt >= (timeout)) \
                                                        { \
                                                            I2cbRegs.I2CMDR.all = 0; \
                                                            asm ("  NOP"); \
                                                            I2cbRegs.I2CMDR.all = 0x20; \
                                                            return false; \
                                                        } \
                                                    } \
                                                } while(0)
#define I2C_SW_TIMEOUT                          (5000000UL)

//
// Macro used for adding delay between 2 consecutive writes to CLKSRCCTL1
// register.
// Delay = 300 NOPs
//
#define BOOT_CLKSRCCTL1_DELAY  asm(" RPT #250 || NOP \n RPT #50 || NOP")
#define BOOT_TMR1SYSCLKCTR        0xF0000000
#define BOOT_TMR2INPCLKCTR        0x800
/*==============================================================================
 Enums
==============================================================================*/

/*==============================================================================
 Structures
==============================================================================*/

/*==============================================================================
 Macros
==============================================================================*/

//
// Macro used for adding delay between 2 consecutive writes to CLKSRCCTL1
// register.
// Delay = 300 NOPs
//
#define BOOT_CLKSRCCTL1_DELAY  asm(" RPT #250 || NOP \n RPT #50 || NOP")

/*==============================================================================
 Local Function Prototypes
==============================================================================*/
static void boot_fnWDDisable(void);
static void boot_fnFlashInit(void);
static void boot_fnPeripheral_clkInit(void);
static void boot_fnInitPLL(void);
static void boot_fnInitI2C(void);

bool boot_hw_eeprom_read(uint16_t addr, uint16_t *data);
bool boot_hw_eeprom_write(uint16_t addr, const uint16_t *data);
static void boot_fnInitI2C(void);
static void boot_fnDisablePeripheral_clk(void);
static void boot_fnDeInitI2C(void);
void boot_fnHW_deinit(void);
void boot_fnHWreset(void);

/*==============================================================================
 Local Variables
==============================================================================*/
static volatile uint32_t uiTime_count;
/*==============================================================================
 Local Constants
==============================================================================*/

/*=============================================================================
 @brief diables WD & check for external clock

 @param void
 @return void
 ============================================================================ */
void BOOT_fnInit(void)
{
    //
    // Disable the watchdog
    //
   boot_fnWDDisable();

    if (ClkCfgRegs.MCDCR.bit.MCLKSTS != 0)
    {
        // External clock is not available
        ESTOP0;
    }

}

/*=============================================================================
 @brief disable watchdog

 @param void
 @return void
 ============================================================================ */

static void boot_fnWDDisable(void)
{
    volatile Uint16 temp;

    __eallow();

    //
    // Grab the clock config so we don't clobber it
    //
    temp = WdRegs.WDCR.all & 0x0007;
    WdRegs.WDCR.all = 0x0068 | temp;
    __edis();
}

/*=============================================================================
 @brief initialize PLL

 @param void
 @return void
 ============================================================================ */
void BOOT_fnHwInit(void)
{
    boot_fnFlashInit(); // flash Initialization

    boot_fnInitPLL();   // PLL Initialization

    boot_fnPeripheral_clkInit(); // I2C Clock Enable

    boot_fnInitI2C(); // I2C Initialization

}

/*=============================================================================
 @brief initialize Peripheral clock

 @param void
 @return void
 ============================================================================ */
static void boot_fnPeripheral_clkInit(void)
{
    __eallow();

    CpuSysRegs.PCLKCR0.all = 0;
    CpuSysRegs.PCLKCR2.all = 0;
    CpuSysRegs.PCLKCR3.all = 0;
    CpuSysRegs.PCLKCR4.all = 0;

    CpuSysRegs.PCLKCR6.all = 0;
    CpuSysRegs.PCLKCR7.all = 0;
    CpuSysRegs.PCLKCR8.all = 0;
    CpuSysRegs.PCLKCR9.all = 0;
    CpuSysRegs.PCLKCR10.all = 0;

    CpuSysRegs.PCLKCR13.all = 0;
    CpuSysRegs.PCLKCR14.all = 0;
    CpuSysRegs.PCLKCR16.all = 0;

    CpuSysRegs.PCLKCR9.bit.I2C_B = 1;
    CpuSysRegs.PCLKCR10.bit.CAN_A = 0;

    __edis();

}

/*=============================================================================
 @brief initialize FLASH

 @param void
 @return void
 ============================================================================ */
#pragma CODE_SECTION(boot_fnFlashInit, "boot_flash_lib")
static void boot_fnFlashInit(void)
{
    __eallow();
    //
    // At reset bank and pump are in sleep
    // A Flash access will power up the bank and pump automatically
    //
    // Power up Flash bank and pump and this also sets the fall back mode of
    // flash and pump as active
    //
    Flash0CtrlRegs.FPAC1.bit.PMPPWR = 0x1;
    Flash0CtrlRegs.FBFALLBACK.bit.BNKPWR0 = 0x3;

    //
    // Disable Cache and prefetch mechanism before changing wait states
    //
    Flash0CtrlRegs.FRD_INTF_CTRL.bit.DATA_CACHE_EN = 0;
    Flash0CtrlRegs.FRD_INTF_CTRL.bit.PREFETCH_EN = 0;

    //
    // Set waitstates according to frequency
    //                CAUTION
    // Minimum waitstates required for the flash operating
    // at a given CPU rate must be characterized by TI.
    // Refer to the datasheet for the latest information.
    //
    //set based  on PLL clock
    Flash0CtrlRegs.FRDCNTL.bit.RWAIT = 0x3;

    //
    // Enable Cache and prefetch mechanism to improve performance
    // of code executed from Flash.
    //
    Flash0CtrlRegs.FRD_INTF_CTRL.bit.DATA_CACHE_EN = 1;
    Flash0CtrlRegs.FRD_INTF_CTRL.bit.PREFETCH_EN = 1;

    //
    // At reset, ECC is enabled. If it is disabled by application software
    // and if application again wants to enable ECC
    //
    Flash0EccRegs.ECC_ENABLE.bit.ENABLE = 0xA;

    __edis();

    //
    // Force a pipeline flush to ensure that the write to
    // the last register configured occurs before returning.
    //
    __asm(" RPT #7 || NOP");
}

/*=============================================================================
 @brief initialize PLL

 @param void
 @return void
 ============================================================================ */
static void boot_fnInitPLL(void)
{


    Uint16 SCSR, WDCR, WDWCR;
    Uint16 imult = 20, fmult = 0, divsel = 1;


    __eallow();
    ClkCfgRegs.CLKSRCCTL1.bit.XTALOFF=0;            // Turn on XTALOSC
    BOOT_CLKSRCCTL1_DELAY;
    ClkCfgRegs.CLKSRCCTL1.bit.OSCCLKSRCSEL = 1;     // Clk Src = XTAL
    __edis();


        Uint16 i;

        //
        // Bypass PLL and set dividers to /1
        //
        __eallow();

        ClkCfgRegs.SYSPLLCTL1.bit.PLLCLKEN = 0;

        //
        // Delay of at least 120 OSCCLK cycles required post PLL bypass
        //
        asm(" RPT #120 || NOP");

        ClkCfgRegs.SYSCLKDIVSEL.bit.PLLSYSCLKDIV = 0;

        //
        // Lock the PLL five times. This helps ensure a successful start.
        // Five is the minimum recommended number. The user can increase this
        // number according to allotted system initialization time.
        //
        for(i = 0; i < 10; i++)
        {
            //
            // Turn off PLL
            //
            ClkCfgRegs.SYSPLLCTL1.bit.PLLEN = 0;
            asm(" RPT #20 || NOP");

            //
            // Write multiplier, which automatically turns on the PLL
            //
            ClkCfgRegs.SYSPLLMULT.all = ((fmult << 8U) | imult);

            //
            // Wait for the SYSPLL lock counter
            //
            while(ClkCfgRegs.SYSPLLSTS.bit.LOCKS != 1)
            {
                //
                // Uncomment to service the watchdog
                //
                // ServiceDog();
            }
        }


    //
    // Set divider to produce slower output frequency to limit current increase
    //


     ClkCfgRegs.SYSCLKDIVSEL.bit.PLLSYSCLKDIV = divsel;


    //
    //      *CAUTION*
    // It is recommended to use the following watchdog code to monitor the PLL
    // startup sequence. If your application has already cleared the watchdog
    // SCRS[WDOVERRIDE] bit this cannot be done. It is recommended not to clear
    // this bit until after the PLL has been initiated.
    //

    //
    // Backup User Watchdog
    //
    SCSR = WdRegs.SCSR.all;
    WDCR = WdRegs.WDCR.all;
    WDWCR = WdRegs.WDWCR.all;

    //
    // Disable windowed functionality, reset counter
    //
    __eallow();
//    WdRegs.WDWCR.all = 0x0;
//    WdRegs.WDKEY.bit.WDKEY = 0x55;
//    WdRegs.WDKEY.bit.WDKEY = 0xAA;

    //
    // Disable global interrupts
    //
    //intStatus = __disable_interrupts();

    //
    // Configure for watchdog reset and to run at max frequency
    //
//    WdRegs.SCSR.all = 0x0;
//    WdRegs.WDCR.all = 0x28;

    //
    // This bit is reset only by power-on-reset (POR) and will not be cleared
    // by a WD reset
    //
    DevCfgRegs.SYSDBGCTL.bit.BIT_0 = 1;

    //
    // Enable PLLSYSCLK is fed from system PLL clock
    //
    ClkCfgRegs.SYSPLLCTL1.bit.PLLCLKEN = 1;

    //
    // Delay to ensure system is clocking from PLL prior to clearing status bit
    //
    asm(" RPT #20 || NOP");

    //
    // Service watchdog
    //
   // ServiceDog();

    //
    // Slip Bit Monitor and SYSCLK Frequency Check using timers
    // Re-lock routine for SLIP condition or if SYSCLK and CLKSRC timer counts
    // are off by +/- 10%.
    // At a minimum, SYSCLK check is performed.  Re lock attempt is carried out
    // if SLIPS bit is set. This while loop is monitored by watchdog.
    // In the event that the PLL does not successfully lock, the loop will be
    // aborted by watchdog reset.
    //
    __eallow();
    CpuSysRegs.TMR2CLKCTL.bit.TMR2CLKSRCSEL = 0x3;

    //
    // Clear bit
    //
    DevCfgRegs.SYSDBGCTL.bit.BIT_0 = 0;

    //
    // Restore user watchdog, first resetting counter
    //
//    WdRegs.WDKEY.bit.WDKEY = 0x55;
//    WdRegs.WDKEY.bit.WDKEY = 0xAA;
//
//    WDCR |= 0x28;                     // Setup WD key--KEY bits always read 0
//    WdRegs.WDCR.all = WDCR;
//    WdRegs.WDWCR.all = WDWCR;
//    WdRegs.SCSR.all = SCSR & 0xFFFE;  // Mask write to bit 0 (W1toClr)
//


    //
    // Set the divider to user value
    //
    ClkCfgRegs.SYSCLKDIVSEL.bit.PLLSYSCLKDIV = divsel;

    __edis();


}


/*=============================================================================
 @brief initialize Peripheral clock

 @param void
 @return void
 ============================================================================ */
static void boot_fnInitI2C(void)
{
    // Enable I2C Pins
    __eallow();

    GpioCtrlRegs.GPALOCK.all = 0x00000000;
     GpioCtrlRegs.GPBLOCK.all = 0x00000000;

    // Enable pull-ups
    GpioCtrlRegs.GPAPUD.bit.GPIO2 = 0;
    GpioCtrlRegs.GPAPUD.bit.GPIO3 = 0;

    // Make the inputs Async
    GpioCtrlRegs.GPAQSEL1.bit.GPIO2  = 3;
    GpioCtrlRegs.GPAQSEL1.bit.GPIO3 = 3;

    // Enable I2C operation on pins
//    GpioCtrlRegs.GPAMUX1.bit.GPIO2  = 6;
//    GpioCtrlRegs.GPAMUX1.bit.GPIO3 = 6;

    GpioCtrlRegs.GPAGMUX1.bit.GPIO2  = 1;
    GpioCtrlRegs.GPAMUX1.bit.GPIO2  = 2;
    GpioCtrlRegs.GPAGMUX1.bit.GPIO3 = 1;
    GpioCtrlRegs.GPAMUX1.bit.GPIO3 = 2;

//    GPIO_SetupPinMux(42, GPIO_MUX_CPU1, 6);
//    GPIO_SetupPinMux(43, GPIO_MUX_CPU1, 6);

    __edis();


    // Initialize I2C, I2C clock = 10MHz, I2C Bus Clock = 250KHz
    I2cbRegs.I2CMDR.all = 0;
    I2cbRegs.I2CPSC.all = 0x9;
    I2cbRegs.I2CCLKL = 10;
    I2cbRegs.I2CCLKH = 5;
    I2cbRegs.I2CIER.all = 0x00;

    // Enable I2C
    I2cbRegs.I2CMDR.all = 0x0020;

    // FIFO is not required
    I2cbRegs.I2CFFTX.all = 0x0000;
    I2cbRegs.I2CFFRX.all = 0x0000;
}


bool boot_hw_eeprom_read(uint16_t addr, uint16_t *data)
{
    volatile uint32_t timeout_cnt;

    I2C_WAIT_FOR(I2cbRegs.I2CMDR.bit.STP == 1, I2C_SW_TIMEOUT);

    I2cbRegs.I2CSAR.all = 0x50;

    I2cbRegs.I2CDXR.all = (addr >> 8) & 0xFF;
    I2cbRegs.I2CMDR.all = 0x66A0;
    I2C_WAIT_FOR(I2cbRegs.I2CSTR.bit.ARDY != 1, I2C_SW_TIMEOUT);

    I2cbRegs.I2CDXR.all = (addr >> 0) & 0xFF;
    I2C_WAIT_FOR(I2cbRegs.I2CSTR.bit.ARDY != 1, I2C_SW_TIMEOUT);

    I2cbRegs.I2CMDR.all = 0x2CA0;

    if (I2cbRegs.I2CSTR.bit.NACK == 1)
    {
        I2cbRegs.I2CMDR.bit.STP = 1;
        I2cbRegs.I2CSTR.all = (1U << 1);
        return false;
    }

    I2C_WAIT_FOR(I2cbRegs.I2CSTR.bit.RRDY != 1, I2C_SW_TIMEOUT);

    *data = I2cbRegs.I2CDRR.all;

    return true;
}

bool boot_hw_eeprom_write(uint16_t addr, const uint16_t *data)
{
    volatile uint32_t timeout_cnt;

    I2C_WAIT_FOR(I2cbRegs.I2CMDR.bit.STP == 1, I2C_SW_TIMEOUT);

    I2cbRegs.I2CSAR.all = 0x50;

    I2cbRegs.I2CDXR.all = (addr >> 8) & 0xFF;
    I2cbRegs.I2CMDR.all = 0x66A0;
    I2C_WAIT_FOR(I2cbRegs.I2CSTR.bit.ARDY != 1, I2C_SW_TIMEOUT);

    I2cbRegs.I2CDXR.all = (addr >> 0) & 0xFF;
    I2C_WAIT_FOR(I2cbRegs.I2CSTR.bit.ARDY != 1, I2C_SW_TIMEOUT);

    I2cbRegs.I2CDXR.all = *data;
    I2cbRegs.I2CMDR.all = 0x4EA0;
    I2C_WAIT_FOR(I2cbRegs.I2CSTR.bit.SCD == 0, I2C_SW_TIMEOUT);

    return true;
}

/*=============================================================================
 @brief  rests hardware

 @param void
 @return void
 ============================================================================ */
void boot_fnHWreset(void)
{
    volatile Uint16 temp;
    // Enable watchdog

        __eallow();
        temp = WdRegs.WDCR.all & 0x0007;
        WdRegs.WDCR.all = 0x0028 | temp;
        __eallow();

        // Trigger watchdog timeout
        while(1)
            ;

}


/*=============================================================================
 @brief Disables all peripheral clock

 @param void
 @return void
 ============================================================================ */
static void boot_fnDisablePeripheral_clk(void)
{
    __eallow();
    CpuSysRegs.PCLKCR0.all = 0;
        CpuSysRegs.PCLKCR2.all = 0;
        CpuSysRegs.PCLKCR3.all = 0;
        CpuSysRegs.PCLKCR4.all = 0;
        CpuSysRegs.PCLKCR6.all = 0;
        CpuSysRegs.PCLKCR7.all = 0;
        CpuSysRegs.PCLKCR8.all = 0;
        CpuSysRegs.PCLKCR9.all = 0;
        CpuSysRegs.PCLKCR10.all = 0;
        CpuSysRegs.PCLKCR13.all = 0;
        CpuSysRegs.PCLKCR14.all = 0;
        CpuSysRegs.PCLKCR16.all = 0;

        __edis();
}

/*=============================================================================
 @brief de-initialize hardware clock & peripherals

 @param void
 @return void
 ============================================================================ */
void boot_fnHW_deinit(void)
{
    boot_fnDeInitI2C();
    boot_fnDisablePeripheral_clk();
}

/*=============================================================================
 @brief de-initialize the I2C module

 @param void
 @return void
 ============================================================================ */
static void boot_fnDeInitI2C(void)
{
    // Reset
    I2cbRegs.I2CMDR.all = 0;

    // Disable I2C Pins
    __eallow();

    // Enable pull-ups
    GpioCtrlRegs.GPAPUD.bit.GPIO2 = 0;
    GpioCtrlRegs.GPAPUD.bit.GPIO3 = 0;

    // Make the inputs Async
    GpioCtrlRegs.GPAQSEL1.bit.GPIO2 = 0;
    GpioCtrlRegs.GPAQSEL1.bit.GPIO3 = 0;

    // Disable I2C operation on pins
    GpioCtrlRegs.GPAMUX1.bit.GPIO2 = 0;
    GpioCtrlRegs.GPAMUX1.bit.GPIO3 = 0;

    __edis();

}

/*==============================================================================
 End of File
==============================================================================*/

