/*=============================================================================
 Copyright Enarka India Pvt Ltd (EIPL) All Rights Reserved.
 All trademarks are owned by Enarka India Private Limited
 ============================================================================ */

/*==============================================================================
 @file  init_system.c
 @author JOTHI RAMESH
 @date 26-Jan-2022
 @brief Description System Initialization done here. Function called in main.c/init_system.c
 ==============================================================================*/

/*==============================================================================
 Includes
 ==============================================================================*/

#include "F2837xS_device.h"
#include "F2837xS_Examples.h"
#include "init_peripherals.h"
#include "cana_defs.h"
#include "isr.h"
#include "scheduler.h"
#include "F021_F2837xS_C28x.h"
#include "safety_lib.h"
#include "safety_lib_prv.h"
#include "state_machine.h"
/*==============================================================================
 Defines
 ==============================================================================*/
#define PUMPREQUEST *(unsigned long*)(0x00050024)

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
void INIT_fnSystem(void);
static void init_fnFlashSectors(void);
/*==============================================================================
 Local Variables
 ==============================================================================*/

/*==============================================================================
 Local Constants
 ==============================================================================*/
extern const fp_sch_slot_t psch_slots[NUM_mTIME_SLOTS];
/*=============================================================================
 @brief infinite loop for the main where tasks are executed is defined here

 @param void
 @return void
 ============================================================================ */
void INIT_fnSystem(void)
{

    //
    // Step 1. Initialize System Control:
    // PLL, WatchDog, enable Peripheral Clocks
    // This example function is found in the F2837xS_SysCtrl.c file.
    //
    InitSysCtrl();

    //
    // Step 2. Initialize GPIO:
    // This example function is found in the F2837xS_Gpio.c file and
    // illustrates how to set the GPIO to it's default state.
    // InitGpio();

    // Step 3. Clear all interrupts and initialize PIE vector table:
    // Disable CPU interrupts
    //
    DINT;

    //
    // Initialize the PIE control registers to their default state.
    // The default state is all PIE interrupts disabled and flags
    // are cleared.
    // This function is found in the F2837xS_PieCtrl.c file.
    //
    InitPieCtrl();

    //
    // Disable CPU interrupts and clear all CPU interrupt flags:
    //
    IER = 0x0000;
    IFR = 0x0000;

    //
    // Initialize the PIE vector table with pointers to the shell Interrupt
    // Service Routines (ISR).
    // This will populate the entire table, even if the interrupt
    // is not used in this example.  This is useful for debug purposes.
    // The shell ISR routines are found in F2837xS_DefaultIsr.c.
    // This function is found in F2837xS_PieVect.c.
    //
    InitPieVectTable();

    // Step 6: Initialize Peripherals

    INIT_fnPeripherals();
    SysCtl_setWatchdogMode(SYSCTL_WD_MODE_RESET);
     // SysCtl_setWatchdogPredivider(SYSCTL_WD_PREDIV_512);
    SysCtl_setWatchdogPrescaler(SYSCTL_WD_PRESCALE_8);

    ServiceDog();

    // Step 7: Initialize global variables

    //
    // Map ISR functions
    //
    EALLOW;
    PieVectTable.TIMER0_INT = &cpu_timer0_isr;
    PieVectTable.TIMER1_INT = &cpu_timer1_isr;

    EDIS;
    PieCtrlRegs.PIEIER1.bit.INTx7 = 1U;

    PieCtrlRegs.PIECTRL.bit.ENPIE = 1U;

    IER = M_INT1 | M_INT13;

    /**********************************************************************************/
    INIT_fnCpu_Timer();

    safety_fninit();

    stat_fnInitState();

    init_fnFlashSectors();

    // Scheduler variables initialization
    scheduler_init(NUM_mTIME_SLOTS, &psch_slots[0], &SCH_fnslot_all);

    INIT_fnStart_CPUtimers();

    EINT;

    // Enable Global interrupt INTM

    ERTM;

    /*********************************************************************************/
    // CAN MailBox Initialization

    INIT_fnCANAMailBox();

    INIT_fnCANBMailBox();
 //   SysCtl_enableWatchdog();
}

/*==============================================================================
 End of File
 ==============================================================================*/

/*=============================================================================
 @brief initFlashSectors - Initializes the flash API

 @param void
 @return void
 ============================================================================ */
static void init_fnFlashSectors(void)
{
    EALLOW;

    PUMPREQUEST = 0x5A5A0001;

    Fapi_StatusType oReturnCheck;
    oReturnCheck = Fapi_initializeAPI(F021_CPU0_W1_BASE_ADDRESS, 200);
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
 @brief Hardware reset

 @param void
 @return void
 ============================================================================ */
void Init_fnHWreset(void)
{
    volatile Uint16 temp;
    // Enable watchdog

    __eallow();
    temp = WdRegs.WDCR.all & 0x0007;
    WdRegs.WDCR.all = 0x0028 | temp;
    __eallow();

//        // Trigger watchdog timeout
    while (1)
        ;

}
/*==============================================================================
 End of File
==============================================================================*/
