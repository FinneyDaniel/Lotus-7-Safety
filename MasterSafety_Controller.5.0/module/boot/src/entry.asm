;//###########################################################################
;//
;// FILE:  f28004x_codestartbranch.asm
;//
;// TITLE: Branch for redirecting code execution after boot.
;//
;// For these examples, code_start is the first code that is executed after
;// exiting the boot ROM code.
;//
;// The codestart section in the linker cmd file is used to physically place
;// this code at the correct memory location. This section should be placed
;// at the location the BOOT ROM will re-direct the code to. For example,
;// for boot to FLASH this code will be located at 0x80000.
;//
;// In addition, the example f28004x projects are setup such that the codegen
;// entry point is also set to the codestart label. This is done by linker
;// option -e in the project build options. When the debugger loads the code,
;// it will automatically set the PC to the "entry point" address indicated by
;// the -e linker option. In this case the debugger is simply assigning the PC,
;// it is not the same as a full reset of the device.
;//
;// The compiler may warn that the entry point for the project is other then
;//  _c_init00. _c_init00 is the C environment setup and is run before
;// main() is entered. The codestart code will re-direct the execution
;// to _c_init00 and thus there is no worry and this warning can be ignored.
;//
;//###########################################################################
;//
;//
;// $Copyright:
;// Copyright (C) 2021 Texas Instruments Incorporated - http://www.ti.com/
;//
;// Redistribution and use in source and binary forms, with or without 
;// modification, are permitted provided that the following conditions 
;// are met:
;// 
;//   Redistributions of source code must retain the above copyright 
;//   notice, this list of conditions and the following disclaimer.
;// 
;//   Redistributions in binary form must reproduce the above copyright
;//   notice, this list of conditions and the following disclaimer in the 
;//   documentation and/or other materials provided with the   
;//   distribution.
;// 
;//   Neither the name of Texas Instruments Incorporated nor the names of
;//   its contributors may be used to endorse or promote products derived
;//   from this software without specific prior written permission.
;// 
;// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS 
;// "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT 
;// LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
;// A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT 
;// OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, 
;// SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT 
;// LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
;// DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
;// THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT 
;// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE 
;// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
;// $
;//###########################################################################

***********************************************************************

	.global  _boot_main
	.global  _entry
	.global  __stack_boot
	.global _bootstrap_main

****************************************************************************
* Declare the stack.  Size is determined by the linker option -stack.  The *
* default value is 1K words.                                               *
****************************************************************************
__stack_boot:	.usect	".stack",0

****************************************************************************
*  INITIALIZE RESET VECTOR TO POINT AT _c_int00                            *
****************************************************************************
	.sect .reset
	.long _entry

	.text

****************************************************************************
* FUNCTION DEF : _entry                                                  *
*                                                                          *
****************************************************************************

_entry:	.asmfunc
****************************************************************************
*  INITIALIZE STACK POINTER.                                               *
****************************************************************************
	MOV  	SP,#__stack_boot		; set to beginning of stack space

****************************************************************************
* INITIALIZE STATUS BIT FIELDS *NOT* INITIALIZED AT RESET                  *
****************************************************************************
	SPM	0			; set product shift to 0

****************************************************************************
* SET C28x MODES                                                           *
****************************************************************************
    C28OBJ                          ; select C28x object mode
    C28ADDR                         ; clear the addressing mode
    C28MAP                          ; set block M0 and M1 mode

    .if .TMS320C2800_FPU32
    SETFLG            RNDF32=1      ; Enable rounding in FPU32 mode.
    .endif
****************************************************************************
* SETTING THESE STATUS BITS/REGISTER TO RESET VALUES.  IF YOU RUN          *
* _c_int00 FROM RESET, YOU CAN REMOVE THIS CODE                            *
****************************************************************************
	CLRC    PAGE0			; use stack addressing mode
    MOVW    DP,#0			; initialize DP to point at low 64K
	CLRC    OVM         		; turn off overflow mode

	ASP				; ensure SP is aligned
****************************************************************************
*  CALL USER'S PROGRAM                                                     *
****************************************************************************
	LCR  _boot_main			; execute boot_main()
	LB _bootstrap_main
		.endasmfunc
***********************************************************************

WD_DISABLE  .set  1    ;set to 1 to disable WD, else set to 0
    .global code_start

***********************************************************************
* Function: codestart section
*
* Description: Branch to code starting point
***********************************************************************

    .sect "codestart"


code_start:
    .if WD_DISABLE == 1
        LB wd_disable       ;Branch to watchdog disable code
    .else
        LB _entry         ;Branch to start of boot._asm in RTS library
    .endif

;end codestart section

***********************************************************************
* Function: wd_disable
*
* Description: Disables the watchdog timer
***********************************************************************
    .if WD_DISABLE == 1

    .text
wd_disable:
    SETC OBJMODE        ;Set OBJMODE for 28x object code
    EALLOW              ;Enable EALLOW protected register access
    MOVZ DP, #7029h>>6  ;Set data page for WDCR register
    MOV @7029h, #0068h  ;Set WDDIS bit in WDCR to disable WD
    EDIS                ;Disable EALLOW protected register access
    LB _entry         ;Branch to start of boot._asm in RTS library

    .endif

;end wd_disable

    .end

;//
;// End of file.
;//
