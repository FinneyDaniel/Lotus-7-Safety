/*=============================================================================
 Copyright Enarka India Pvt Ltd (EIPL) All Rights Reserved.
 All trademarks are owned by Enarka India Private Limited
 ============================================================================ */

/*==============================================================================
 @file  main1.c
 @author JOTHI RAMESH
 @date 13-Jan-2022

 @brief Description
 ==============================================================================*/

/*==============================================================================
 Includes
 ==============================================================================*/

#include "F2837xS_device.h"
#include "F2837xS_Examples.h"
#include "F28x_Project.h"
#include "init_system.h"
#include "scheduler.h"
#include "safety_lib.h"
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

//
// Function Prototypes
//
//
/*==============================================================================
 Local Variables
 ==============================================================================*/

/*==============================================================================
 Local Constants
 ==============================================================================*/

/*=============================================================================
 @brief infinite loop for the main where tasks are executed is defined here

 @param void
 @return void
 ==============================================
 ============================== */
int main(void)
{    INIT_fnSystem(); // System Initialization done here

    while (1)
    {
        safety_fnslotAll();
    }
}

/*==============================================================================
 End of File
 ==============================================================================*/
