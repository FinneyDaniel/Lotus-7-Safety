/*=============================================================================
 Copyright Enarka India Pvt Ltd (EIPL) All Rights Reserved.
 All trademarks are owned by Enarka India Private Limited
 ============================================================================ */

/*==============================================================================
 @file  init_gpio.c
 @author JOTHI RAMESH
 @date 06-Sep-2021

 @brief Description:  peripherals gpio configuration done here
 ==============================================================================*/

/*==============================================================================
 Includes
 ==============================================================================*/

#include <module/application/init_gpio/inc/init_gpio.h>
#include "F28x_Project.h"
#include "device.h"

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

void GPIO_fnInit(void);
void gpio_fnInitSYS(void);
void gpio_fnInitPWM1(void);
void gpio_fnInitH_beat(void);
void gpio_fnInitCANA(void);
void gpio_fnInitCANB(void);
void gpio_fnInitI2CB(void);
/*==============================================================================
 Local Variables
 ==============================================================================*/

/*==============================================================================
 Local Constants
 ==============================================================================*/

/*=============================================================================
 @brief Initialization of GPIO's done here

 @param void
 @return void
 ============================================================================ */

void INIT_fnGPIO()
{
    gpio_fnInitSYS();                     // System GPIO Initialize
    gpio_fnInitH_beat();                  // heart beat of dsp
    gpio_fnInitCANA();
    gpio_fnInitCANB();
    gpio_fnInitI2CB();
}

/*=============================================================================
 @Function name : void gpio_fnInitSYS(void)
 @brief configures required GPIO's for System functionality
 @param void
 @return void
 ============================================================================ */

void gpio_fnInitSYS(void)
{
    EALLOW;

//    // ISR Execution check LED
    GpioCtrlRegs.GPCGMUX2.bit.GPIO85 = GPIO_mGPIOFUNCTION;  // Configure as GPIO
    GpioCtrlRegs.GPCDIR.bit.GPIO85 = 1;

    GpioCtrlRegs.GPCGMUX2.bit.GPIO86 = GPIO_mGPIOFUNCTION;  // Configure as GPIO
    GpioCtrlRegs.GPCDIR.bit.GPIO86 = 1;

    GpioCtrlRegs.GPCGMUX2.bit.GPIO92 = GPIO_mGPIOFUNCTION;  // Configure as GPIO  // isr chk
    GpioCtrlRegs.GPCDIR.bit.GPIO92 = 1;
    EDIS;
}

/*=============================================================================
 @Function name : void gpio_fnInitH_beat(void)
 @brief configures required GPIO's for PWM1
 @param void
 @return void
 ============================================================================ */

void gpio_fnInitH_beat(void)
{
    //configure GPIO11 as DSP_BEAT pin
    GPIO_setPadConfig(11, GPIO_PIN_TYPE_PULLUP);      // Enable pullup on GPIO11
    GPIO_setPinConfig(GPIO_11_GPIO11);                // GPIO11 = GPIO11
    GPIO_setDirectionMode(11, GPIO_DIR_MODE_OUT);    // GPIO11 = output
    GPIO_writePin(11, 0);                            // Load output latch
}

///*=============================================================================
// @Function name : void gpio_fnInitCANA(void)
// @brief configures GPIO's based on CANA functionality
//
// @param void
// @return void
// ============================================================================ */
void gpio_fnInitCANA(void)
{
GPIO_SetupPinMux(62, GPIO_MUX_CPU1, 6); //GPIO30 -  CANRXA
GPIO_SetupPinOptions(62, GPIO_INPUT, GPIO_ASYNC);
GPIO_SetupPinMux(63, GPIO_MUX_CPU1, 6); //GPIO31 - CANTXA
GPIO_SetupPinOptions(63, GPIO_OUTPUT, GPIO_PUSHPULL);
 }
//
///*=============================================================================
// @Function name : void gpio_fnInitCANB(void)
// @brief configures GPIO's based on CANB functionality
//
// @param void
// @return void
// ============================================================================ */
void gpio_fnInitCANB(void)
 {
GPIO_SetupPinMux(73, GPIO_MUX_CPU1, 5); //GPIO73 -  CANRXB
GPIO_SetupPinOptions(73, GPIO_INPUT, GPIO_ASYNC);
GPIO_SetupPinMux(20, GPIO_MUX_CPU1, 3); //GPIO20 - CANTXB
GPIO_SetupPinOptions(20, GPIO_OUTPUT, GPIO_PUSHPULL);//}
 }
///*=============================================================================
// @Function name : void gpio_fnInitCANB(void)
// @brief configures GPIO's based on CANB functionality
//
// @param void
// @return void
// ============================================================================ */
void gpio_fnInitI2CB(void)
 {
    GPIO_SetupPinMux(2, GPIO_MUX_CPU1, 6); //
    GPIO_SetupPinMux(3, GPIO_MUX_CPU1, 6); //
 }

/*==============================================================================
 End of File
 ==============================================================================*/
