/*=============================================================================
 Copyright Enarka India Pvt Ltd (EIPL) All Rights Reserved.
 All trademarks are owned by Enarka India Private Limited
 ============================================================================ */

/*==============================================================================
 @file  isr.c
 @author JOTHI RAMESH
 @date 26-Jan-2022

 @brief Description
 ==============================================================================*/

/*==============================================================================
 Includes
 ==============================================================================*/

#include "F28x_Project.h"              // Standard libraries headers
#include "F2837xS_Device.h"
#include "safety_lib.h"
#include "safety_lib_prv.h"
#include "isr.h"
#include "scheduler.h"
#include "cana_defs.h"
#include "canb_defs.h"
#include "state_machine.h"
/*==============================================================================
 Defines
 ==============================================================================*/
#define CPU_mtimerT0_T1mincnt   (95U)
#define CPU_mtimerT0_T1maxcnt   (105U)
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

/*==============================================================================
 Local Variables
 ==============================================================================*/

bool bl_clock_OKflg = 0;
uint16_t uiResetTimer = 0;
uint16_t uiLEDCount = 0;
uint16_t uiHearbtcntFlag=0,uiHearbtCount=0;
uint16_t ui16fsmCounter = 0;

/*==============================================================================
 Local Constants
 ==============================================================================*/

/*=============================================================================
 @brief infinite loop for the main where tasks are executed is defined here

 @param void
 @return void
 Description : cpu_timer0_isr - CPU Timer0 ISR with interrupt counter
 ============================================================================ */
interrupt void cpu_timer0_isr(void)  //100usec
{
    CpuTimer0Regs.TCR.bit.TIF = 1;
    CpuTimer0.InterruptCount++;
    if (CpuTimer1.InterruptCount == 1)
    {
        CpuTimer1.InterruptCount = 0;
        bl_clock_OKflg = 0;
    }
/**************************CAN AUTO BUS OFF ******************************/
    if (CanaRegs.CAN_CTL.bit.Init == 1)
      {
          EALLOW;
          CanaRegs.CAN_CTL.bit.ABO = 1;
          DELAY_US(0.1);
          EDIS;
      }

      if (CanbRegs.CAN_CTL.bit.Init == 1)
      {
          EALLOW;
          CanbRegs.CAN_CTL.bit.ABO = 1;
          DELAY_US(0.1);
          EDIS;
      }
/*************************************************************************/
    ui16fsmCounter++;

    if (ui16fsmCounter > 20)  // FSM occurs once every 2ms (100us * 20 = 2ms)
    {
        STAT_fnFSMCheck();
        ui16fsmCounter = 0;
    }
/*******************************************************************************/
    CANA_fnRXevent();  // Receiving the CANA from IOcard & MP using circular buffer
    CANB_fnRXevent();  // Receiving the CANB from IOcard & MP using circular buffer

    // Acknowledge this interrupt to receive more interrupts from group 1
     PieCtrlRegs.PIEACK.all = PIEACK_GROUP1;
}
/*=============================================================================
 @brief infinite loop for the main where tasks are executed is defined here

 @param void
 @return void
 Description : cpu_timer1_isr - CPU Timer1 ISR
               Compare T0 cnt with T1 cnt. T0cnt 100times more than T1cnt
 ============================================================================ */
interrupt void cpu_timer1_isr(void)  // 10msec
{

    CpuTimer1.InterruptCount++;
    if (CpuTimer1.InterruptCount == 1)
    {
        if ((CpuTimer0.InterruptCount > CPU_mtimerT0_T1mincnt)
                && (CpuTimer0.InterruptCount < CPU_mtimerT0_T1maxcnt))
        {
            bl_clock_OKflg = 1;
            CpuTimer0.InterruptCount = 0;
        }
        else
        {
            safety_fnPower_off(MEF_TIMING);
        }
    }
/**************************************************************************/
    scheduler_task();
/**************************************************************************/

    CANA_fnTask();  //processing the CAN A RX message
    CANB_fnTask();  //processing the CAN B RX message
    CAN_fnFltRst();
    CANA_fnTx();
/**************************************************************************/
   uiLEDCount++;                 //for testing purpose

    if (uiLEDCount > 200)
    {
        uiLEDCount = 0;
  //      CANA_fnTx();   //TX function to MP and IO cards
    }

    if (uiLEDCount <= 100)
    {
        GpioDataRegs.GPCSET.bit.GPIO92 = 1;
    }
    if ((uiLEDCount >= 101) && (uiLEDCount <= 200))
    {
        GpioDataRegs.GPCCLEAR.bit.GPIO92 = 1;
    }
/********************************************************************/

    CpuTimer1Regs.TCR.bit.TIF = 1;
    EINT;

}

/*==============================================================================
 End of File
 ==============================================================================*/
