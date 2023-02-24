--entry_point=code_start

/*Suppress the warning : #10063-D entry-point symbol other than "_c_int00" specified:  "code_start"	can_boot		 	 */
--diag_suppress=10063

/* Suppress the warning: Warning: no matching section */
--diag_suppress=10068


MEMORY
{
PAGE 0 :  /* Program Memory */
          /* Memory (RAM/FLASH) blocks can be moved to PAGE1 for data allocation */
          /* BEGIN is used for the "boot to Flash" bootloader mode   */

   BEGIN           	: origin = 0x080000, length = 0x000002
   RAMM0           	: origin = 0x000122, length = 0x0002DE
   RAMD0           	: origin = 0x00B000, length = 0x000800
   RAMLS0          	: origin = 0x008000, length = 0x000800
   RAMLS1          	: origin = 0x008800, length = 0x000800
   RAMLS2      		: origin = 0x009000, length = 0x000800
   RAMLS3      		: origin = 0x009800, length = 0x000800
   RAMLS4      		: origin = 0x00A000, length = 0x000800
   RAMGS14     		: origin = 0x01A000, length = 0x001000
   RAMGS15     		: origin = 0x01B000, length = 0x001000
   RESET           	: origin = 0x3FFFC0, length = 0x000002
   
   /* Flash sectors */
   /* BANK 0 */

   BOOT_FLASH           : origin = 0x080002, length = 0x007FFE	/* on-chip Flash */
   APP_FLASH            : origin = 0x088000, length = 0x038000	/* on-chip Flash */

   /* BANK 1 */

   FLASHO           : origin = 0x0C0000, length = 0x002000	/* on-chip Flash */
   FLASHP           : origin = 0x0C2000, length = 0x002000	/* on-chip Flash */
   FLASHQ           : origin = 0x0C4000, length = 0x002000	/* on-chip Flash */   
   FLASHR           : origin = 0x0C6000, length = 0x002000	/* on-chip Flash */
   FLASHS           : origin = 0x0C8000, length = 0x008000	/* on-chip Flash */
   FLASHT           : origin = 0x0D0000, length = 0x008000	/* on-chip Flash */   
   FLASHU           : origin = 0x0D8000, length = 0x008000	/* on-chip Flash */   
   FLASHV           : origin = 0x0E0000, length = 0x008000	/* on-chip Flash */   
   FLASHW           : origin = 0x0E8000, length = 0x008000	/* on-chip Flash */   
   FLASHX           : origin = 0x0F0000, length = 0x008000	/* on-chip Flash */
   FLASHY           : origin = 0x0F8000, length = 0x002000	/* on-chip Flash */
   FLASHZ           : origin = 0x0FA000, length = 0x002000	/* on-chip Flash */
   FLASHAA          : origin = 0x0FC000, length = 0x002000	/* on-chip Flash */   
   FLASHAB          : origin = 0x0FE000, length = 0x002000	/* on-chip Flash */   

PAGE 1 : /* Data Memory */
         /* Memory (RAM/FLASH) blocks can be moved to PAGE0 for program allocation */

   BOOT_RSVD       : origin = 0x000002, length = 0x000120     /* Part of M0, BOOT rom will use this for stack */
   RAMM1           : origin = 0x000400, length = 0x000400     /* on-chip RAM block M1 */
   RAMD1           : origin = 0x00B800, length = 0x000800

   RAMLS5      : origin = 0x00A800, length = 0x001000

   RAMGS0      : origin = 0x00C000, length = 0x001000
   RAMGS1      : origin = 0x00D000, length = 0x001000
   RAMGS2      : origin = 0x00E000, length = 0x001000
   RAMGS3      : origin = 0x00F000, length = 0x001000
   RAMGS4      : origin = 0x010000, length = 0x001000
   RAMGS5      : origin = 0x011000, length = 0x001000
   RAMGS6      : origin = 0x012000, length = 0x001000
   RAMGS7      : origin = 0x013000, length = 0x001000
   RAMGS8      : origin = 0x014000, length = 0x001000
   RAMGS9      : origin = 0x015000, length = 0x001000
   RAMGS10     : origin = 0x016000, length = 0x001000
   RAMGS11     : origin = 0x017000, length = 0x001000
   RAMGS12     : origin = 0x018000, length = 0x001000
   RAMGS13     : origin = 0x019000, length = 0x001000
   
}


SECTIONS
{
   codestart        : > BEGIN,       PAGE = 0
   .text            : > APP_FLASH,   PAGE = 0
   .cinit           : > APP_FLASH,   PAGE = 0
   .pinit           : > APP_FLASH,   PAGE = 0
   .switch          : > APP_FLASH,   PAGE = 0
   .reset           : > RESET,     PAGE = 0, TYPE = DSECT /* not used, */

   .stack           : > RAMM1,     PAGE = 1
   .ebss               : >> RAMLS5 | RAMGS0 | RAMGS1       PAGE = 1
    					START(_start_ebss)
   						END(_end_ebss)

   .esysmem         : > RAMLS5,    PAGE = 1
   .econst          : > APP_FLASH,    PAGE = 0

   ramgs0           : > RAMGS0,    PAGE = 1
   ramgs1           : > RAMGS1,    PAGE = 1

/*boot memory allocation */
   boot_cinit
   {
      -l F021_API_F2837xS_FPU32.lib (.cinit)
         boot_main.obj(.cinit)
         boot_hwinit.obj(.cinit)
         boot_flash.obj(.cinit)
    boot_flash.obj(.econst)
   } > BOOT_FLASH PAGE = 0

    boot_start
    {
    entry.obj (.text)
    boot_main.obj(.text)
    boot_hwinit.obj(.text)
    boot_flash.obj(.text)

    } > BOOT_FLASH

   boot_flash_lib :
   {
 	  -l F021_API_F2837xS_FPU32.lib (.econst)
 	   -l F021_API_F2837xS_FPU32.lib (.text)
   } LOAD = BOOT_FLASH,
     RUN = RAMLS0,
     LOAD_START(_boot_flash_code_load_start),
     LOAD_SIZE(_boot_flash_code_size),
     RUN_START(_boot_flash_code_run_start)
     PAGE = 0

   .data_boot
   {
      entry.obj (.data)
      boot_main.obj (.data)
      boot_hwinit.obj (.data)
        boot_flash.obj(.data)
      -l F021_API_F2837xS_FPU32.lib (.data)
   } > RAMM1       PAGE = 1

   .ebss_boot
   {
      entry.obj (.ebss)
      boot_main.obj (.ebss)
      boot_hwinit.obj (.ebss)
           boot_flash.obj(.ebss)
      -l F021_API_F2837xS_FPU32.lib (.ebss)
   } > RAMM1       PAGE = 1

 /* Allocate program areas: */
   bootstrap
   {
      bootstrap_app.obj (.text)
    //  -l F021_API_F2837xS_FPU32.lib (.cinit)

   } > 0x088000     PAGE = 0

	.TI.ramfunc : {} LOAD = APP_FLASH,
                         RUN = RAMLS0,
                         LOAD_START(_RamfuncsLoadStart),
                         LOAD_SIZE(_RamfuncsLoadSize),
                         LOAD_END(_RamfuncsLoadEnd),
                         RUN_START(_RamfuncsRunStart),
                         RUN_SIZE(_RamfuncsRunSize),
                         RUN_END(_RamfuncsRunEnd),
                         PAGE = 0, ALIGN(4)


/*
    boot_start
    {
	*(.TI.ramfunc)
    entry.obj
    boot_main.obj
    } LOAD = FLASH_BANK0_SEC1,
                         RUN = RAMLS0 | RAMLS1 | RAMLS2 |RAMLS3,
                         LOAD_START(_RamfuncsLoadStart),
                         LOAD_SIZE(_RamfuncsLoadSize),
                         LOAD_END(_RamfuncsLoadEnd),
                         RUN_START(_RamfuncsRunStart),
                         RUN_SIZE(_RamfuncsRunSize),
                         RUN_END(_RamfuncsRunEnd),
                         PAGE = 0, ALIGN(4)
*/

}

/*
//===========================================================================
// End of file.
//===========================================================================
*/
