;******************************************************************************
;* BOOT  v6.4.11
;*
;* Copyright (c) 2000-2016 Texas Instruments Incorporated
;* http://www.ti.com/
;*
;*  Redistribution and  use in source  and binary forms, with  or without
;*  modification,  are permitted provided  that the  following conditions
;*  are met:
;*
;*     Redistributions  of source  code must  retain the  above copyright
;*     notice, this list of conditions and the following disclaimer.
;*
;*     Redistributions in binary form  must reproduce the above copyright
;*     notice, this  list of conditions  and the following  disclaimer in
;*     the  documentation  and/or   other  materials  provided  with  the
;*     distribution.
;*
;*     Neither the  name of Texas Instruments Incorporated  nor the names
;*     of its  contributors may  be used to  endorse or  promote products
;*     derived  from   this  software  without   specific  prior  written
;*     permission.
;*
;*  THIS SOFTWARE  IS PROVIDED BY THE COPYRIGHT  HOLDERS AND CONTRIBUTORS
;*  "AS IS"  AND ANY  EXPRESS OR IMPLIED  WARRANTIES, INCLUDING,  BUT NOT
;*  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
;*  A PARTICULAR PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT
;*  OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
;*  SPECIAL,  EXEMPLARY,  OR CONSEQUENTIAL  DAMAGES  (INCLUDING, BUT  NOT
;*  LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
;*  DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
;*  THEORY OF  LIABILITY, WHETHER IN CONTRACT, STRICT  LIABILITY, OR TORT
;*  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
;*  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
;*
;******************************************************************************

;****************************************************************************

;*
;****************************************************************************

	.global  _c_int00
	.global  _exit
	.global  _bootstrap_main

	.global  _start_ebss
	.global  _end_ebss
.text
****************************************************************************
* FUNCTION DEF : _bootstrap_main                                               *
*                                                                          *
****************************************************************************

_bootstrap_main:	.asmfunc
	MOVL	XAR0, #_start_ebss
	MOV		AL, #0
	MOV		AH, #(_end_ebss - _start_ebss - 1)
	RPT		AH || MOV *XAR0++, AL
****************************************************************************
*  CALL USER'S PROGRAM                                                     *
****************************************************************************
	LB      _c_int00			; execute TI init routines
	LCR		_exit
	.endasmfunc

