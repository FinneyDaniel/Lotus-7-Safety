/*=============================================================================
Copyright Enarka India Pvt Ltd (EIPL) All Rights Reserved.
All trademarks are owned by Enarka India Private Limited
============================================================================ */

/*==============================================================================
 @file  state_machine.h
 @author JOTHI RAMESH
 @date 06-Sep-2021
 @brief Description
==============================================================================*/

#ifndef MODULE_STATE_MACHINE_INC_STATE_MACHINE_H_
#define MODULE_STATE_MACHINE_INC_STATE_MACHINE_H_

/*==============================================================================
 Includes
==============================================================================*/

#include "F28x_Project.h"
#include <stdint.h>
#include <stdbool.h>
/*==============================================================================
 Defines
==============================================================================*/



/*==============================================================================
 Enums
==============================================================================*/

/*==============================================================================
 Structures
==============================================================================*/


typedef enum
{
    MP_STANDBY,
    MP_READY,
    MP_STACKCHECK,
    MP_STACKPOWER,
    MP_FAULT,
    MP_SAFE_SHUT_DOWN,
    MP_COMMISSION,
}STATE_enumVALMS;

typedef struct STAT_zSTATE_MPMAC
{
    STATE_enumVALMS Present_st;

}STAT_tzSTATE_MPMAC;

typedef enum
{
    STAND_BY,
    PURGE,
    IOPOWER,
    ARMED_POWER,
    FAULT,
    SHUT_DOWN,

}STATE_enumVAL;

typedef struct STAT_zSTATEMAC
{
    STATE_enumVAL Next_st;
    STATE_enumVAL Present_st;
    STATE_enumVAL Previous_st;
}STAT_tzSTATEMAC;


/*==============================================================================
 Macros
==============================================================================*/

/*==============================================================================
 Extern/Public Function Prototypes
==============================================================================*/

extern void STAT_fnFSMCheck(void);
extern void stat_fnInitState(void);
extern  void stat_IOReset(void);
/*==============================================================================
 Extern/Public Structures
==============================================================================*/

extern STAT_tzSTATEMAC STAT_tzStateMac;
extern STAT_tzSTATE_MPMAC STAT_tzStateMacMP;


/*==============================================================================
 Extern/Public Constants
==============================================================================*/


#endif /* MODULE_STATE_MACHINE_INC_STATE_MACHINE_H_ */
