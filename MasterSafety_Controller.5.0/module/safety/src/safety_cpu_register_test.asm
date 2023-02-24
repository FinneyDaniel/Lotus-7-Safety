;/*=============================================================================
;Copyright Enarka India Pvt Ltd (EIPL) All Rights Reserved.
;All trademarks are owned by Enarka India Private Limited
;============================================================================ */
;
;/*==============================================================================
; @file  safety_crc.c
; @author JOTHI RAMESH
; @date 26-Jan-2022
;
; @brief Description: testing the CPU , FPU & VCU regeister
;==============================================================================*/

		.cdecls C,LIST,"safety_lib.h","safety_lib_prv.h"
	.global	_sl_fnCPU_RegistersTest
	.global _sl_fnFPU_RegistersTest
	.global _sl_fnVCU_RegistersTest
	.if	.TMS320C2800
;	.if	.TMS320F28377
; Constants
TEST_mITERATIONS        .set    0x1
SET_mTO_ZERO            .set    0x0
STF_mTEST_MASK_MSB       .set    0x0000
STF_mTEST_MASK_LSB       .set    0x027C
VSTATUS_mTEST_MASK_MSB   .set    0x0000
VSTATUS_mTEST_MASK_LSB	.set    0x3FFF
	.text

; void sl_fnCPU_RegistersTest(void)
_sl_fnCPU_RegistersTest:

start_cpu_stuck_at_test:
		; Save registers that compiler assumes are preserved upon return
		PUSH    XAR1
		PUSH    XAR2
		PUSH    XAR3

		; Test ACC register
		MOV     AH, #REG_mTEST_PATTERN_STUCK_AT_FAULT
		CMP     @AH, #REG_mTEST_PATTERN_STUCK_AT_FAULT
		B       failed_cpu_test, NEQ
		MOV     AL, #REG_mTEST_PATTERN_STUCK_AT_FAULT
		CMP     @AL, #REG_mTEST_PATTERN_STUCK_AT_FAULT
		B       failed_cpu_test, NEQ

		MOV     AH, #N_REG_mTEST_PATTERN_STUCK_AT_FAULT
		CMP     @AH, #N_REG_mTEST_PATTERN_STUCK_AT_FAULT
		B       failed_cpu_test, NEQ
		MOV     AL, #N_REG_mTEST_PATTERN_STUCK_AT_FAULT
		CMP     @AL, #N_REG_mTEST_PATTERN_STUCK_AT_FAULT
		B       failed_cpu_test, NEQ

		; Test P register using ACC
		; P register is used as loop counter
		MOVL    P, @ACC
		CMPL    ACC, @P
		B       failed_cpu_test, NEQ
		NOT     ACC
		MOVL    P, @ACC
		CMPL    ACC, @P
		B       failed_cpu_test, NEQ

		; Use PL as loop
		MOV     PL, #TEST_mITERATIONS
loop_aux_reg_stuck_at_test:
		NOT     ACC
		MOVL    XAR0, @ACC
		MOVL    XAR1, @ACC
		MOVL    XAR2, @ACC
		MOVL    XAR3, @ACC
		MOVL    XAR4, @ACC
		MOVL    XAR5, @ACC
		MOVL    XAR6, @ACC
		MOVL    XAR7, @ACC

		CMPL    ACC, @XAR0
		B       failed_cpu_test, NEQ
		CMPL    ACC, @XAR1
		B       failed_cpu_test, NEQ
		CMPL    ACC, @XAR2
		B       failed_cpu_test, NEQ
		CMPL    ACC, @XAR3
		B       failed_cpu_test, NEQ
		CMPL    ACC, @XAR4
		B       failed_cpu_test, NEQ
		CMPL    ACC, @XAR5
		B       failed_cpu_test, NEQ
		CMPL    ACC, @XAR6
		B       failed_cpu_test, NEQ
		CMPL    ACC, @XAR7
		B       failed_cpu_test, NEQ

		MOVL    XT, @ACC
		CMPL    ACC, @XT
		B       failed_cpu_test, NEQ

		DEC     PL     ; Firt loop tests with 0x55555555, next loop 0xAAAAAAAA
		B       loop_aux_reg_stuck_at_test, GEQ

		; Disable interrupts
		SETC    INTM
		; Test stack pointer
		MOVZ    AR4, @SP    ; save stack pointer
		MOV     @SP, AL
		CMP     AL, @SP
		B       failed_stack_stuck_at_test, NEQ
		NOT     ACC
		MOV     @SP, AL
		CMP     AL, @SP
		B       failed_stack_stuck_at_test, NEQ
		MOV     @SP, AR4	; restore stack pointer
		B       status_reg_stuck_at_test, UNC
failed_stack_stuck_at_test:
		; Restore stack pointer, if possible
		MOV     @SP, AR4
		B       failed_cpu_test, UNC
status_reg_stuck_at_test:
		; Save registers
		PUSH    ST0
		POP     @PL
		PUSH    DP
		POP     @PH

		; Use AR6 as loop counter
		MOV     AR6, #TEST_mITERATIONS
loop_status_reg_stuck_at_test:
		PUSH    @AL
		POP     DP
		PUSH    DP
		POP     AR4
		CMP     AL, @AR4
		B       failed_cpu_test, NEQ

		PUSH    @AL
		POP     ST0
		PUSH    ST0
		POP     AR4
		CMP     AL, @AR4
		B       failed_cpu_test, NEQ

		NOT     ACC
		DEC     AR6
		B       loop_status_reg_stuck_at_test, GEQ

		; Restore registers
		PUSH    @PL
		POP     ST0
		PUSH    @PH
		POP     DP
		; Enable interrutps
		CLRC    INTM

		; DC Fault Test
start_cpu_test:
		; Test ACC register
		MOV     AH, #REG_mTEST_PATTERN_DC_FAULT
		CMP     @AH, #REG_mTEST_PATTERN_DC_FAULT
		B       failed_cpu_test, NEQ
		MOV     AL, #REG_mTEST_PATTERN_DC_FAULT
		CMP     @AL, #REG_mTEST_PATTERN_DC_FAULT
		B       failed_cpu_test, NEQ

		MOV     AH, #N_REG_mTEST_PATTERN_DC_FAULT
		CMP     @AH, #N_REG_mTEST_PATTERN_DC_FAULT
		B       failed_cpu_test, NEQ
		MOV     AL, #N_REG_mTEST_PATTERN_DC_FAULT
		CMP     @AL, #N_REG_mTEST_PATTERN_DC_FAULT
		B       failed_cpu_test, NEQ

		; Test P register using ACC
		; P register is used as loop counter
		MOVL    P, @ACC
		CMPL    ACC, @P
		B       failed_cpu_test, NEQ
		NOT     ACC
		MOVL    P, @ACC
		CMPL    ACC, @P
		B       failed_cpu_test, NEQ

		; Use PL as loop
		MOV     PL, #TEST_mITERATIONS
loop_aux_reg_test:
		NOT     ACC
		MOVL    XAR0, @ACC
		MOVL    XAR1, @ACC
		MOVL    XAR2, @ACC
		MOVL    XAR3, @ACC
		MOVL    XAR4, @ACC
		MOVL    XAR5, @ACC
		MOVL    XAR6, @ACC
		MOVL    XAR7, @ACC

	.if $isdefed("MICROELECTRONIC_TESTING_ENABLE")
		PUSH    ACC
		PUSH    XAR0
		PUSH    XAR4

		MOV     AL, #microelectronic_fault_t.MEF_CPU
		LCR      _sl_is_fault_test_enabled
		CMP     @AL, #1
		B       continue_test, EQ
		POP     XAR4
		POP     XAR0
		POP     ACC
continue_test:
	.endif
		CMPL    ACC, @XAR0
		B       failed_cpu_test, NEQ
		CMPL    ACC, @XAR1
		B       failed_cpu_test, NEQ
		CMPL    ACC, @XAR2
		B       failed_cpu_test, NEQ
		CMPL    ACC, @XAR3
		B       failed_cpu_test, NEQ
		CMPL    ACC, @XAR4
		B       failed_cpu_test, NEQ
		CMPL    ACC, @XAR5
		B       failed_cpu_test, NEQ
		CMPL    ACC, @XAR6
		B       failed_cpu_test, NEQ
		CMPL    ACC, @XAR7
		B       failed_cpu_test, NEQ

		MOVL    XT, @ACC
		CMPL    ACC, @XT
		B       failed_cpu_test, NEQ

		DEC     PL     ; Firt loop tests with 0x00000000, next loop 0xFFFFFFFF
		B       loop_aux_reg_test, GEQ

		; Disable interrupts
		SETC    INTM
		; Test stack pointer
		MOVZ    AR4, @SP    ; save stack pointer
		MOV     @SP, AL
		CMP     AL, @SP
		B       failed_stack_test, NEQ
		NOT     ACC
		MOV     @SP, AL
		CMP     AL, @SP
		B       failed_stack_test, NEQ
		MOV     @SP, AR4	; restore stack pointer
		B       status_reg_test, UNC
failed_stack_test:
		; Restore stack pointer, if possible
		MOV     @SP, AR4
		B       failed_cpu_test, UNC
status_reg_test:
		; Save registers
		PUSH    ST0
		POP     @PL
		PUSH    DP
		POP     @PH

		; Use AR6 as loop counter
		MOV     AR6, #TEST_mITERATIONS
loop_status_reg_test:
		PUSH    @AL
		POP     DP
		PUSH    DP
		POP     AR4
		CMP     AL, @AR4
		B       failed_cpu_test, NEQ

		PUSH    @AL
		POP     ST0
		PUSH    ST0
		POP     AR4
		CMP     AL, @AR4
		B       failed_cpu_test, NEQ

		NOT     ACC
		DEC     AR6
		B       loop_status_reg_test, GEQ

		; Restore registers
		PUSH    @PL
		POP     ST0
		PUSH    @PH
		POP     DP
		; Enable interrutps
		CLRC    INTM
		; All tests complete and good
		B       end_cpu_test, UNC
failed_cpu_test:
		POP     XAR3
		POP     XAR2
		POP     XAR1
		MOV     AL, #microelectronic_fault_t.MEF_CPU
		LCR     _safety_fnPower_off

end_cpu_test:
		POP     XAR3
		POP     XAR2
		POP     XAR1

 		LRETR
     	.endif


;    .if BUILD_LIB_F2806X | BUILD_LIB_F2833X
;********************************************************************************************
;
; Uint16 sl_fnFPU_RegistersTest(void)
;
;    - Test FPU registers for stuck at bits.
;      Registers R0H – R7H and RND32, TF, ZI, NI, ZF, NF, LUF and LVF bits of STF
;      register are tested.
;	   contents of STF, R7H,R6H,R5H R4H are saved and restored
;    - on success
;		 return a success code
;    - on failure
;	     -returns 0
;		 -jumps to fail safe routine if JUMP_mTO_FAILSAFE is set to 1
;********************************************************************************************

_sl_fnFPU_RegistersTest:

start_fpu_reg_test:
	; Push save on entry registers onto the stack
	MOV32   *SP++,R4H
	MOV32   *SP++,R5H
	MOV32   *SP++,R6H
	MOV32   *SP++,R7H
	; Push status register onto the stack
	MOV32   *SP++,STF

	; ACC = 0xAAAA AAAA
	MOV     AH,#REG_mTEST_PATTERN_STUCK_AT_FAULT
	MOV     AL,#REG_mTEST_PATTERN_STUCK_AT_FAULT

	; Use PL as a loop counter
	; Test 0xAAAAAAAA first
	; then 0x55555555
	MOV     PL,#TEST_mITERATIONS	 ; loop twice
loop_FPU_test:
    NOT     ACC                  ; write pattern ro FPU registers
	MOV32   R0H,@ACC
	MOV32   R1H,@ACC
	MOV32   R2H,@ACC
	MOV32   R3H,@ACC
	MOV32   R4H,@ACC
	MOV32   R5H,@ACC
	MOV32   R6H,@ACC
	MOV32   R7H,@ACC

;	.if LIB_TEST_VCU_TEST_ERROR			 ; %%%%%% Begin Add error
;    MOV     DP,#_gErrorTestFlag
;    TBIT    @_gErrorTestFlag,#0
;	B       skip_vcu_error_test,NTC;

;	MOVXI   R0H,#FPU_ERROR_CODE
;	MOVXI   R1H,#FPU_ERROR_CODE
;	MOVXI   R2H,#FPU_ERROR_CODE
;	MOVXI   R3H,#FPU_ERROR_CODE
;	MOVXI   R4H,#FPU_ERROR_CODE
;	MOVXI   R5H,#FPU_ERROR_CODE
;	MOVXI   R6H,#FPU_ERROR_CODE
;	MOVXI   R7H,#FPU_ERROR_CODE
;skip_vcu_error_test:
;	.endif								 ; %%%%%% End Add error
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
	PUSH   ACC

    .if $defined(MICROELECTRONIC_TESTING_ENABLE)
        MOV     AL, #microelectronic_fault_t.MEF_FPU
		LCR     _sl_is_fault_test_enabled
		CMP     AL, #0x1
		B       continueFPU_test, NEQ
  		MOVXI   R5H, #microelectronic_fault_t.MEF_FPU

continueFPU_test:
    .endif

		POP     ACC
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

	MOV32   @XAR0,R0H
	CMPL    ACC,@XAR0
	B       fail_FPU_test,NEQ
	MOV32   @XAR0,R1H
	CMPL    ACC,@XAR0
	B       fail_FPU_test,NEQ
	MOV32   @XAR0,R2H
	CMPL    ACC,@XAR0
	B       fail_FPU_test,NEQ
	MOV32   @XAR0,R3H
	CMPL    ACC,@XAR0
	B       fail_FPU_test,NEQ
	MOV32   @XAR0,R4H
	CMPL    ACC,@XAR0
	B       fail_FPU_test,NEQ
	MOV32   @XAR0,R5H
	CMPL    ACC,@XAR0
	B       fail_FPU_test,NEQ
	MOV32   @XAR0,R6H
	CMPL    ACC,@XAR0
	B       fail_FPU_test,NEQ
	MOV32   @XAR0,R7H
	CMPL    ACC,@XAR0
	B       fail_FPU_test,NEQ

	MOVL    XAR6,@ACC			  ; save ACC  contents


	AND     AH,#STF_mTEST_MASK_MSB
	AND     AL,#STF_mTEST_MASK_LSB ; Use mask to test STF register
	PUSH    ACC
	MOV32   STF,*--SP             ; STF = ACC
	MOV32   *SP++,STF
	POP     XAR0
	CMPL    ACC,@XAR0
	B       fail_FPU_test,NEQ
	MOVL    ACC,@XAR6            ; restore ACC contents

	DEC     PL                   ; decrement loop counter
	B       loop_FPU_test,GEQ

pass_FPU_test:
	MOV     AL,#TEST_mPASSED     ; return a success code
	B	    end_Test_FPU_Registers,UNC
fail_FPU_test:
		MOV     AL, #microelectronic_fault_t.MEF_FPU
		LCR     _safety_fnPower_off
end_Test_FPU_Registers:
	MOV32   STF,*--SP            ; restore FPU registers
	MOV32   R7H,*--SP
	MOV32   R6H,*--SP
	MOV32   R5H,*--SP
	MOV32   R4H,*--SP
end_sl_fnFPU_RegistersTest:
 	LRETR
;	.endif
;********************************************************************************************
;
; Uint16 _sl_fnVCU_RegistersTest( Uint32 *scratch_RAM_address)
;										                      XAR4
;
;        - tests VCU (Viterbi, Complex Math and CRC Unit) registers for stuck at bits.
;          VR0 – VR8, VT0, VT1, VCRC and VSTATUS registers are tested
;          contents of VSTATUS are saved and restored.
;          XAR4 register has the adress of the scratch RAM
;        - on success
;	  	    return a success code
;        - on failure
;	         -returns 0
;	     	 -jumps to fail safe routine if JUMP_mTO_FAILSAFE is set to 1
;********************************************************************************************
_sl_fnVCU_RegistersTest:


start_vcu_reg_test:
	MOV     AH,#REG_mTEST_PATTERN_STUCK_AT_FAULT
	MOV     AL,#REG_mTEST_PATTERN_STUCK_AT_FAULT ; ACC = 0xAAAAAAAA

	VMOV32   *XAR4,VSTATUS
	MOVL     XAR5,*XAR4               ; Save contents of VSTATUS to XAR5

	; Use PL as a loop counter
	; Test 0xAAAAAAAA first
	; then 0x55555555
	MOV     PL,#TEST_mITERATIONS
loop_VCU_test:
	NOT      ACC
	MOVL     XAR4,ACC    			  ; write pattern to VCU registers
	VMOV32   VR0, XAR4
	VMOV32   VR1, XAR4
	VMOV32   VR2, XAR4
	VMOV32   VR3, XAR4
	VMOV32   VR4, XAR4
	VMOV32   VR5, XAR4
	VMOV32   VR6, XAR4
	VMOV32   VR7, XAR4
	VMOV32   VR8, XAR4
	VMOV32   VT0, XAR4
	VMOV32   VT1, XAR4
	VMOV32   VCRC, XAR4

	PUSH    ACC
;___________________________________________________________________________________;
    .if $defined(MICROELECTRONIC_TESTING_ENABLE)
        MOV     AL, #microelectronic_fault_t.MEF_VCU
		LCR     _sl_is_fault_test_enabled
		CMP     AL, #0x1
		B       skip_vcu_error_test, NEQ
  		VMOVIX  VR4, #microelectronic_fault_t.MEF_VCU
;____________________________________________________________________________________;
skip_vcu_error_test:
    .endif								 ; %%%%%% End Add error

	POP     ACC
	VMOV32    XAR4,VR0
	CMPL     ACC, XAR4
	B        fail_VCU_test,NEQ
	VMOV32    XAR4,VR1
	CMPL     ACC, XAR4
	B        fail_VCU_test,NEQ
	VMOV32    XAR4,VR2
	CMPL     ACC, XAR4
	B        fail_VCU_test,NEQ
	VMOV32    XAR4,VR3
	CMPL     ACC, XAR4
	B        fail_VCU_test,NEQ
	VMOV32    XAR4,VR4
	CMPL     ACC, XAR4
	B        fail_VCU_test,NEQ
	VMOV32    XAR4,VR5
	CMPL     ACC, XAR4
	B        fail_VCU_test,NEQ
	VMOV32    XAR4,VR6
	CMPL     ACC, XAR4
	B        fail_VCU_test,NEQ
	VMOV32    XAR4,VR7
	CMPL     ACC, XAR4
	B        fail_VCU_test,NEQ
	VMOV32    XAR4,VR8
	CMPL     ACC, XAR4
	B        fail_VCU_test,NEQ
	VMOV32    XAR4,VT0
	CMPL     ACC, XAR4
	B        fail_VCU_test,NEQ
	VMOV32    XAR4,VT1
	CMPL     ACC, XAR4
	B        fail_VCU_test,NEQ
	VMOV32    XAR4,VCRC
	CMPL     ACC, XAR4
	B        fail_VCU_test,NEQ

	MOVL     XAR6,@ACC			       ; save ACC  contents
	AND      AH,#VSTATUS_mTEST_MASK_MSB ; use a mask to test VSTATUS register
	AND      AL,#VSTATUS_mTEST_MASK_LSB
	MOVL      XAR4,ACC
    VMOV32   VSTATUS, XAR4
	VMOV32    XAR4,VSTATUS
	CMPL     ACC, XAR4
	B        fail_VCU_test,NEQ
	MOVL     ACC,@XAR6            		; restore ACC contents

	DEC      PL                         ; decrement loop counter
	B        loop_VCU_test,GEQ

pass_VCU_test:
	MOV     AL,#TEST_mPASSED
	 B       endVCURegTest, UNC      ; return success code if pass
fail_VCU_test:
        MOV     AL, #microelectronic_fault_t.MEF_VCU       ; return a fail code
        LCR     _safety_fnPower_off

endVCURegTest:
    	MOVL      XAR4,XAR5
	VMOV32   VSTATUS, XAR4
        LRETR



