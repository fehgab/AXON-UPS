/*
//###########################################################################
//
// FILE:    DSP2803x_Headers_nonBIOS.cmd
//
// TITLE:   DSP2803x Peripheral registers linker command file
//
// DESCRIPTION:
//
//          This file is for use in Non-BIOS applications.
//
//          Linker command file to place the peripheral structures
//          used within the DSP2803x headerfiles into the correct memory
//          mapped locations.
//
//          This version of the file includes the PieVectorTable structure.
//          For BIOS applications, please use the DSP2803x_Headers_BIOS.cmd file
//          which does not include the PieVectorTable structure.
//
//###########################################################################
// $TI Release: F2803x C/C++ Header Files and Peripheral Examples V127 $
// $Release Date: March 30, 2013 $
//###########################################################################
*/

-q
-c
-x
-stack0x3b0
-i "lib"
-l rts2800_ml.lib
-l IQmath.lib
-o debug\sw28027.out
-m debug\sw28027.map

MEMORY
{
PAGE 0 :   /* Program Memory */
           /* Memory (RAM/FLASH/OTP) blocks can be moved to PAGE1 for data allocation */
	BEGIN       : origin = 0x3F0000, length = 0x000002     /* Part of FLASHH.  Used for "boot to Flash" bootloader mode. */
	FLASH       : origin = 0x3F0002, length = 0x005FFD     /* on-chip FLASH */   
	RAMM1       : origin = 0x000400, length = 0x000400     /* on-chip RAM block M1 */
	OTP         : origin = 0x3D7800, length = 0x000400     /* on-chip OTP */
	CSM_RSVD    : origin = 0x3F7F80, length = 0x000076     /* Part of FLASHA.  Program with all 0x0000 when CSM is in use. */
	CSM_PWL_P0  : origin = 0x3F7FF8, length = 0x000008     /* Part of FLASHA.  CSM password locations in FLASHA */
	IQTABLES    : origin = 0x3FE000, length = 0x000B50     /* IQ Math Tables in Boot ROM */
	IQTABLES2   : origin = 0x3FEB50, length = 0x00008C     /* IQ Math Tables in Boot ROM */
	IQTABLES3   : origin = 0x3FEBDC, length = 0x0000AA	  /* IQ Math Tables in Boot ROM */
	ROM         : origin = 0x3FF27C, length = 0x000D44     /* Boot ROM */
	RESET       : origin = 0x3FFFC0, length = 0x000002     /* part of boot ROM  */
	VECTORS     : origin = 0x3FFFC2, length = 0x00003E     /* part of boot ROM  */

PAGE 1 :   /* Data Memory */
           /* Memory (RAM/FLASH/OTP) blocks can be moved to PAGE0 for program allocation */
           /* Registers remain on PAGE1                                                  */

	BOOT_RSVD   : origin = 0x000000, length = 0x000050     /* Part of M0, BOOT rom will use this for stack */
	RAMM0       : origin = 0x000050, length = 0x0003B0     /* on-chip RAM block M0 */

	RAML0       : origin = 0x008000, length = 0x001000     /* on-chip RAM block L0 */
	DEV_EMU     : origin = 0x000880, length = 0x000105     /* device emulation registers */
	SYS_PWR_CTL : origin = 0x000985, length = 0x000003     /* System power control registers */
	FLASH_REGS  : origin = 0x000A80, length = 0x000060     /* FLASH registers */
	CSM         : origin = 0x000AE0, length = 0x000010     /* code security module registers */

	ADC_RESULT  : origin = 0x000B00, length = 0x000020     /* ADC Results register mirror */

	CPU_TIMER0  : origin = 0x000C00, length = 0x000008     /* CPU Timer0 registers */
	CPU_TIMER1  : origin = 0x000C08, length = 0x000008     /* CPU Timer0 registers (CPU Timer1 & Timer2 reserved TI use)*/
	CPU_TIMER2  : origin = 0x000C10, length = 0x000008     /* CPU Timer0 registers (CPU Timer1 & Timer2 reserved TI use)*/

	PIE_CTRL    : origin = 0x000CE0, length = 0x000020     /* PIE control registers */
	PIE_VECT    : origin = 0x000D00, length = 0x000100     /* PIE Vector Table */

   COMP1       : origin = 0x006400, length = 0x000020     /* Comparator 1 registers */
   COMP2       : origin = 0x006420, length = 0x000020     /* Comparator 2 registers */
   
   EPWM1       : origin = 0x006800, length = 0x000040     /* Enhanced PWM 1 registers */
   EPWM2       : origin = 0x006840, length = 0x000040     /* Enhanced PWM 2 registers */
   EPWM3       : origin = 0x006880, length = 0x000040     /* Enhanced PWM 3 registers */
   EPWM4       : origin = 0x0068C0, length = 0x000040     /* Enhanced PWM 4 registers */

   ECAP1       : origin = 0x006A00, length = 0x000020     /* Enhanced Capture 1 registers */
 
   GPIOCTRL    : origin = 0x006F80, length = 0x000040     /* GPIO control registers */
   GPIODAT     : origin = 0x006FC0, length = 0x000020     /* GPIO data registers */
   GPIOINT     : origin = 0x006FE0, length = 0x000020     /* GPIO interrupt/LPM registers */
                 
   SYSTEM      : origin = 0x007010, length = 0x000020     /* System control registers */
   
   SPIA        : origin = 0x007040, length = 0x000010     /* SPI-A registers */
   
   SCIA        : origin = 0x007050, length = 0x000010     /* SCI-A registers */
   
   NMIINTRUPT  : origin = 0x007060, length = 0x000010     /* NMI Watchdog Interrupt Registers */
   XINTRUPT    : origin = 0x007070, length = 0x000010     /* external interrupt registers */

   ADC         : origin = 0x007100, length = 0x000080     /* ADC registers */

   I2CA        : origin = 0x007900, length = 0x000040     /* I2C-A registers */
   
   CSM_PWL     : origin = 0x3F7FF8, length = 0x000008     /* Part of FLASHA.  CSM password locations. */

   PARTID      : origin = 0x3D7FFF, length = 0x000001     /* Part ID register location */

}

/* Allocate sections to memory blocks.
   Note:
         codestart user defined section in DSP28_CodeStartBranch.asm used to redirect code
                   execution when booting to flash
         ramfuncs  user defined section to store functions that will be copied from Flash into RAM
*/


SECTIONS
{

   /* Allocate program areas: */
   .cinit              : > FLASH,     PAGE = 0
   .pinit              : > FLASH,     PAGE = 0
   .text               : > FLASH,     PAGE = 0
   codestart           : > BEGIN,      PAGE = 0
   ramfuncs            : LOAD = FLASH,
                         RUN = RAMM1,
                         LOAD_START(_RamfuncsLoadStart),
                         LOAD_END(_RamfuncsLoadEnd),
                         RUN_START(_RamfuncsRunStart),
						 LOAD_SIZE(_RamfuncsLoadSize),
                         PAGE = 0

   csmpasswds          : > CSM_PWL_P0, PAGE = 0
   csm_rsvd            : > CSM_RSVD,   PAGE = 0

   /* Allocate uninitalized data sections: */
   .stack              : > RAMM0,      PAGE = 1
   .ebss               : > RAML0,      PAGE = 1
   .esysmem            : > RAML0,      PAGE = 1

   /* Initalized sections to go in Flash */
   /* For SDFlash to program these, they must be allocated to page 0 */
   .econst             : > FLASH,     PAGE = 0
   .switch             : > FLASH,     PAGE = 0

   /* Allocate IQ math areas: */
   IQmath              : > FLASH,     PAGE = 0            /* Math Code */
   IQmathTables        : > IQTABLES,   PAGE = 0, TYPE = NOLOAD
   
  /* Uncomment the section below if calling the IQNexp() or IQexp()
      functions from the IQMath.lib library in order to utilize the
      relevant IQ Math table in Boot ROM (This saves space and Boot ROM
      is 1 wait-state). If this section is not uncommented, IQmathTables2
      will be loaded into other memory (SARAM, Flash, etc.) and will take
      up space, but 0 wait-state is possible.
   */
   /*
   IQmathTables2    : > IQTABLES2, PAGE = 0, TYPE = NOLOAD
   {

              IQmath.lib<IQNexpTable.obj> (IQmathTablesRam)

   }
   */
    /* Uncomment the section below if calling the IQNasin() or IQasin()
       functions from the IQMath.lib library in order to utilize the
       relevant IQ Math table in Boot ROM (This saves space and Boot ROM
       is 1 wait-state). If this section is not uncommented, IQmathTables2
       will be loaded into other memory (SARAM, Flash, etc.) and will take
       up space, but 0 wait-state is possible.
    */
    /*
    IQmathTables3    : > IQTABLES3, PAGE = 0, TYPE = NOLOAD
    {

               IQmath.lib<IQNasinTable.obj> (IQmathTablesRam)

    }
    */

   /* .reset is a standard section used by the compiler.  It contains the */
   /* the address of the start of _c_int00 for C Code.   /*
   /* When using the boot ROM this section and the CPU vector */
   /* table is not needed.  Thus the default type is set here to  */
   /* DSECT  */
   .reset              : > RESET,      PAGE = 0, TYPE = DSECT
   vectors             : > VECTORS,    PAGE = 0, TYPE = DSECT

/*** PIE Vect Table and Boot ROM Variables Structures ***/
  UNION run = PIE_VECT, PAGE = 1
   {
      PieVectTableFile
      GROUP
      {
         EmuKeyVar
         EmuBModeVar
         FlashCallbackVar
         FlashScalingVar
      }
   }

/*** Peripheral Frame 0 Register Structures ***/
   DevEmuRegsFile    : > DEV_EMU,     PAGE = 1
	 SysPwrCtrlRegsFile: > SYS_PWR_CTL, PAGE = 1
   FlashRegsFile     : > FLASH_REGS,  PAGE = 1
   CsmRegsFile       : > CSM,         PAGE = 1
   AdcResultFile     : > ADC_RESULT,  PAGE = 1   
   CpuTimer0RegsFile : > CPU_TIMER0,  PAGE = 1
   CpuTimer1RegsFile : > CPU_TIMER1,  PAGE = 1
   CpuTimer2RegsFile : > CPU_TIMER2,  PAGE = 1  
   PieCtrlRegsFile   : > PIE_CTRL,    PAGE = 1      

/*** Peripheral Frame 1 Register Structures ***/      
   ECap1RegsFile     : > ECAP1        PAGE = 1   
   GpioCtrlRegsFile  : > GPIOCTRL     PAGE = 1
   GpioDataRegsFile  : > GPIODAT      PAGE = 1
   GpioIntRegsFile   : > GPIOINT      PAGE = 1
   
/*** Peripheral Frame 2 Register Structures ***/
   SysCtrlRegsFile   : > SYSTEM,      PAGE = 1
   SpiaRegsFile      : > SPIA,        PAGE = 1
   SciaRegsFile      : > SCIA,        PAGE = 1
   NmiIntruptRegsFile: > NMIINTRUPT,  PAGE = 1
   XIntruptRegsFile  : > XINTRUPT,    PAGE = 1
   AdcRegsFile       : > ADC,         PAGE = 1
   I2caRegsFile      : > I2CA,        PAGE = 1 
               
/*** Peripheral Frame 3 Register Structures ***/
   Comp1RegsFile     : > COMP1,       PAGE = 1
   Comp2RegsFile     : > COMP2,       PAGE = 1
   EPwm1RegsFile     : > EPWM1        PAGE = 1   
   EPwm2RegsFile     : > EPWM2        PAGE = 1   
   EPwm3RegsFile     : > EPWM3        PAGE = 1   
   EPwm4RegsFile     : > EPWM4        PAGE = 1   


/*** Code Security Module Register Structures ***/
   CsmPwlFile        : > CSM_PWL,     PAGE = 1

/*** Device Part ID Register Structures ***/
   PartIdRegsFile    : > PARTID,      PAGE = 1

}

/*
//===========================================================================
// End of file.
//===========================================================================
*/
