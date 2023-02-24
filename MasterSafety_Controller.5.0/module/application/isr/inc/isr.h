/*=============================================================================
Copyright Enarka India Pvt Ltd (EIPL) All Rights Reserved.
All trademarks are owned by Enarka India Private Limited
============================================================================ */

/*==============================================================================
 @file  isr.h
 @author JOTHI RAMESH
 @date 14-Jan-2022

 @brief Description
==============================================================================*/

#ifndef MODULE_APPLICATION_ISR_INC_ISR_H_
#define MODULE_APPLICATION_ISR_INC_ISR_H_

/*==============================================================================
 Includes
==============================================================================*/

#include <stdint.h>
#include "driverlib.h"


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
 Extern/Public Function Prototypes
==============================================================================*/

extern interrupt void cpu_timer0_isr(void);
extern interrupt void cpu_timer1_isr(void);

/*==============================================================================
 Extern/Public Variables
==============================================================================*/
extern uint16_t uiResetTimer,uiHearbtcntFlag,uiHearbtCount;
/*==============================================================================
 Extern/Public Constants
==============================================================================*/


#endif /* MODULE_APPLICATION_ISR_INC_ISR_H_ */