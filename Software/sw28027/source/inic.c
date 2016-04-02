#include "inic.h"

#pragma CODE_SECTION(InitFlash, "ramfuncs");

extern Uint16 RamfuncsLoadStart;
extern Uint16 RamfuncsLoadEnd;
extern Uint16 RamfuncsRunStart;

volatile struct SCI_REGS *sci_ptr_terminal;
extern __interrupt void  adc_isr(void);
extern __interrupt void cpu_timer0_isr(void);
//--------------------------------------------------------------------
//  Configure Device for target Application Here
//--------------------------------------------------------------------

void DeviceInit(void)
{
	WDogDisable(); 	// Disable the watchdog initially
	DINT;			// Global Disable all Interrupts
	IER = 0x0000;	// Disable CPU interrupts
	IFR = 0x0000;	// Clear all CPU interrupt flags


// The Device_cal function, which copies the ADC & oscillator calibration values
// from TI reserved OTP into the appropriate trim registers, occurs automatically
// in the Boot ROM. If the boot ROM code is bypassed during the debug process, the
// following function MUST be called for the ADC and oscillators to function according
// to specification.
	EALLOW;
	SysCtrlRegs.PCLKCR0.bit.ADCENCLK = 1; // Enable ADC peripheral clock
	(*Device_cal)();					  					// Auto-calibrate from TI OTP
	SysCtrlRegs.PCLKCR0.bit.ADCENCLK = 0; // Return ADC clock to original state
	EDIS;

// Switch to Internal Oscillator 1 and turn off all other clock
// sources to minimize power consumption
	EALLOW;
	SysCtrlRegs.CLKCTL.bit.INTOSC1OFF = 0;
  SysCtrlRegs.CLKCTL.bit.OSCCLKSRCSEL=0;  // Clk Src = INTOSC1
	SysCtrlRegs.CLKCTL.bit.XCLKINOFF=1;     // Turn off XCLKIN
	SysCtrlRegs.CLKCTL.bit.XTALOSCOFF=1;    // Turn off XTALOSC
	SysCtrlRegs.CLKCTL.bit.INTOSC2OFF=1;    // Turn off INTOSC2
  EDIS;


// SYSTEM CLOCK speed based on internal oscillator = 10 MHz
// 0x10=  80    MHz		(16)
// 0xF =  75    MHz		(15)
// 0xE =  70    MHz		(14)
// 0xD =  65    MHz		(13)
// 0xC =  60	MHz		(12)
// 0xB =  55	MHz		(11)
// 0xA =  50	MHz		(10)
// 0x9 =  45	MHz		(9)
// 0x8 =  40	MHz		(8)
// 0x7 =  35	MHz		(7)
// 0x6 =  30	MHz		(6)
// 0x5 =  25	MHz		(5)
// 0x4 =  20	MHz		(4)
// 0x3 =  15	MHz		(3)
// 0x2 =  10	MHz		(2)

	PLLset( 0x0C );	// choose from options above

// Initialise interrupt controller and Vector Table
// to defaults for now. Application ISR mapping done later.
	EALLOW;  // This is needed to write to EALLOW protected register
  InitPieCtrl();
	InitPieVectTable();
  EDIS;    // This is needed to disable write to EALLOW protected registers

	EALLOW;  // This is needed to write to EALLOW protected register
	PieVectTable.TINT0 = &cpu_timer0_isr;
	PieVectTable.ADCINT1 = &adc_isr;
  EDIS;    // This is needed to disable write to EALLOW protected registers

  MemCopy(&RamfuncsLoadStart, &RamfuncsLoadEnd, &RamfuncsRunStart);

  InitFlash();

  EALLOW; // below registers are "protected", allow access.

// LOW SPEED CLOCKS prescale register settings

   SysCtrlRegs.LOSPCP.all = 0x0001;		// Sysclk / 2 (30 MHz)
   SysCtrlRegs.XCLK.bit.XCLKOUTDIV=0; // Sysclk / 4 (15 MHz)

// PERIPHERAL CLOCK ENABLES
//---------------------------------------------------
// If you are not using a peripheral you may want to switch
// the clock off to save power, i.e. set to =0
//
// Note: not all peripherals are available on all 280x derivates.
// Refer to the datasheet for your particular device.

   SysCtrlRegs.PCLKCR0.bit.ADCENCLK = 1;    // ADC
   //------------------------------------------------
   SysCtrlRegs.PCLKCR3.bit.COMP1ENCLK = 0;	// COMP1
   SysCtrlRegs.PCLKCR3.bit.COMP2ENCLK = 0;	// COMP2
   //------------------------------------------------
   SysCtrlRegs.PCLKCR0.bit.I2CAENCLK = 0;   // I2C
   //------------------------------------------------
   SysCtrlRegs.PCLKCR0.bit.SPIAENCLK = 0;	// SPI-A
   //------------------------------------------------
   SysCtrlRegs.PCLKCR0.bit.SCIAENCLK = 1; // SCI-A
   //------------------------------------------------
   SysCtrlRegs.PCLKCR1.bit.ECAP1ENCLK = 0;	//eCAP1
   //------------------------------------------------
   SysCtrlRegs.PCLKCR1.bit.EPWM1ENCLK = 1;  // ePWM1
   SysCtrlRegs.PCLKCR1.bit.EPWM2ENCLK = 0;  // ePWM2
   SysCtrlRegs.PCLKCR1.bit.EPWM3ENCLK = 0;  // ePWM3
   SysCtrlRegs.PCLKCR1.bit.EPWM4ENCLK = 0;  // ePWM4
   //------------------------------------------------
   SysCtrlRegs.PCLKCR0.bit.TBCLKSYNC = 1;   // Enable TBCLK
   //------------------------------------------------

// Step 4. Initialize the Device Peripheral. This function can be
//         found in F2806x_CpuTimers.c
   InitCpuTimers();   // For this example, only initialize the Cpu Timers

// Configure CPU-Timer 0 to interrupt every 1 milisecond:
// 60MHz CPU Freq, 1 milisecond Period (in uSeconds)

   ConfigCpuTimer(&CpuTimer0, 60, 1000);
   CpuTimer0Regs.TCR.all = 0x4000; // Use write-only instruction to set TSS bit = 0

// Configure ADC power up sequence
   	EALLOW;
    AdcRegs.ADCCTL1.bit.ADCBGPWD  = 1;      // Power ADC BG
    AdcRegs.ADCCTL1.bit.ADCREFPWD = 1;      // Power reference
    AdcRegs.ADCCTL1.bit.ADCPWDN   = 1;      // Power ADC
    AdcRegs.ADCCTL1.bit.ADCENABLE = 1;      // Enable ADC
    AdcRegs.ADCCTL1.bit.ADCREFSEL = 0;      // Select interal BG
    EDIS;

    DELAY_US(ADC_usDELAY);         // Delay before converting ADC channels

    EALLOW;
    AdcRegs.ADCCTL2.bit.CLKDIV2EN = 1;
    EDIS;

    DELAY_US(ADC_usDELAY);         // Delay before converting ADC channels

   	EALLOW;

//configure adc
	AdcRegs.ADCCTL2.bit.ADCNONOVERLAP = 1;	// Enable non-overlap mode
	AdcRegs.ADCCTL1.bit.INTPULSEPOS	= 1;		// ADCINT1 trips after AdcResults latch
	AdcRegs.INTSEL1N2.bit.INT1E     = 1;		// Enabled ADCINT1
	AdcRegs.INTSEL1N2.bit.INT1CONT  = 0;		// Disable ADCINT1 Continuous mode
  AdcRegs.INTSEL1N2.bit.INT1SEL 	= 1;  	// setup EOC1 to trigger ADCINT1 to fire
  AdcRegs.ADCSOC0CTL.bit.CHSEL 	= 0;			// set SOC0 channel select to ADCINA0
  AdcRegs.ADCSOC1CTL.bit.CHSEL 	= 1;			// set SOC1 channel select to ADCINA1
  AdcRegs.ADCSOC2CTL.bit.CHSEL 	= 2;			// set SOC2 channel select to ADCINA2
  AdcRegs.ADCSOC3CTL.bit.CHSEL 	= 3;			// set SOC3 channel select to ADCINA3

// Set trigger  ePWM1, ADCSOCA
  AdcRegs.ADCSOC0CTL.bit.TRIGSEL 	= 5;    // set SOC0 start trigger on EPWM1A, due to round-robin SOC0 converts first then SOC1
  AdcRegs.ADCSOC1CTL.bit.TRIGSEL 	= 5;    // set SOC1 start trigger on EPWM1A, due to round-robin SOC0 converts first then SOC1
  AdcRegs.ADCSOC2CTL.bit.TRIGSEL 	= 5;    // set SOC2 start trigger on EPWM1A, due to round-robin SOC0 converts first then SOC1
  AdcRegs.ADCSOC3CTL.bit.TRIGSEL 	= 5;    // set SOC2 start trigger on EPWM1A, due to round-robin SOC0 converts first then SOC1

	AdcRegs.ADCSOC0CTL.bit.ACQPS 	= 6;	// set SOC0 S/H Window to 7 ADC Clock Cycles, (6 ACQPS plus 1)
	AdcRegs.ADCSOC1CTL.bit.ACQPS 	= 6;	// set SOC1 S/H Window to 7 ADC Clock Cycles, (6 ACQPS plus 1)
	AdcRegs.ADCSOC2CTL.bit.ACQPS 	= 6;	// set SOC2 S/H Window to 7 ADC Clock Cycles, (6 ACQPS plus 1)
	AdcRegs.ADCSOC3CTL.bit.ACQPS 	= 6;	// set SOC3 S/H Window to 7 ADC Clock Cycles, (6 ACQPS plus 1)

  //pwm control
  EPwm1Regs.TBPRD 				= 1500;	// Set period for ePWM1 40kHz
  EPwm1Regs.TBCTL.bit.CTRMODE 	= TB_COUNT_UPDOWN;		// count updown
  EPwm1Regs.TBCTL.bit.PHSEN = TB_DISABLE; 						// Master module
  EPwm1Regs.TBCTL.bit.PRDLD = TB_SHADOW;
  EPwm1Regs.TBCTL.bit.SYNCOSEL = TB_CTR_ZERO; // Sync down-stream module

// Counter Compare
  EPwm1Regs.CMPCTL.bit.SHDWAMODE = CC_SHADOW;
  EPwm1Regs.CMPCTL.bit.SHDWBMODE = CC_SHADOW;
	EPwm1Regs.CMPCTL.bit.LOADAMODE = CC_CTR_ZERO_PRD;

  // Action Qualifier
  EPwm1Regs.AQCTLA.bit.CAU = AQ_SET; // set actions for EPWM1A
  EPwm1Regs.AQCTLA.bit.CAD = AQ_CLEAR;
  EPwm1Regs.AQCTLB.bit.CAU = AQ_CLEAR; // set actions for EPWM1B
  EPwm1Regs.AQCTLB.bit.CAD = AQ_SET;
  EPwm1Regs.AQSFRC.bit.RLDCSF = 2 ; // Load AQCSFRC on event CTR = Zero or PRD

  // Dead-Band generator bypass
  EPwm1Regs.DBCTL.bit.IN_MODE = 0;
  EPwm1Regs.DBCTL.bit.OUT_MODE = 0;

	EPwm1Regs.TZSEL.bit.OSHT1 = TZ_ENABLE;
	EPwm1Regs.TZCTL.bit.TZA = TZ_FORCE_LO; // TZ1 to TZ6 Trip Action On EPWMxA
	EPwm1Regs.TZCTL.bit.TZB = TZ_FORCE_LO; // TZ1 to TZ6 Trip Action On EPWMxB
  EPwm1Regs.TZFRC.bit.OST = 1; 					 // Trip Zones One Shot Int gyujtas tiltas

  EPwm1Regs.ETSEL.bit.SOCAEN	= 1;		// Enable SOC on A group
  EPwm1Regs.ETSEL.bit.SOCASEL	= 2;		// Select SOC from TBCTR=TBPRD
  EPwm1Regs.ETPS.bit.SOCAPRD 	= 1;		// Generate pulse on 1st event

//--------------------------------------------------------------------------------------
// GPIO (GENERAL PURPOSE I/O) CONFIG
//--------------------------------------------------------------------------------------
//-----------------------
// QUICK NOTES on USAGE:
//-----------------------
// If GpioCtrlRegs.GP?MUX?bit.GPIO?= 1, 2 or 3 (i.e. Non GPIO func), then leave
//	rest of lines commented
// If GpioCtrlRegs.GP?MUX?bit.GPIO?= 0 (i.e. GPIO func), then:
//	1) uncomment GpioCtrlRegs.GP?DIR.bit.GPIO? = ? and choose pin to be IN or OUT
//	2) If IN, can leave next to lines commented
//	3) If OUT, uncomment line with ..GPACLEAR.. to force pin LOW or
//			   uncomment line with ..GPASET.. to force pin HIGH or
//--------------------------------------------------------------------------------------
//  GPIO-28 - PIN FUNCTION = --Spare--
	GpioCtrlRegs.GPAMUX2.bit.GPIO28 = 1;	// 0=GPIO, 1=SCIRXA, 2=SDAA, 3=TZ2
	GpioCtrlRegs.GPADIR.bit.GPIO28 = 0;		// 1=OUTput,  0=INput
//	GpioDataRegs.GPACLEAR.bit.GPIO28 = 1;	// uncomment if --> Set Low initially
//	GpioDataRegs.GPASET.bit.GPIO28 = 1;		// uncomment if --> Set High initially
//--------------------------------------------------------------------------------------
//  GPIO-29 - PIN FUNCTION = --Spare--
	GpioCtrlRegs.GPAMUX2.bit.GPIO29 = 1;	// 0=GPIO, 1=SCITXDA, 2=SCLA, 3=TZ3
	GpioCtrlRegs.GPADIR.bit.GPIO29 = 1;		// 1=OUTput,  0=INput
//	GpioDataRegs.GPACLEAR.bit.GPIO29 = 1;	// uncomment if --> Set Low initially
//	GpioDataRegs.GPASET.bit.GPIO29 = 1;		// uncomment if --> Set High initially
//--------------------------------------------------------------------------------------

//--------------------------------------------------------------------------------------
//  GPIO-12 - PIN FUNCTION = --Spare--
//	GpioCtrlRegs.GPAMUX2.bit.GPIO12 = 3;	// 0=GPIO, 1=SCIRXA, 2=SDAA, 3=TZ2
//	GpioCtrlRegs.GPADIR.bit.GPIO12 = 0;		// 1=OUTput,  0=INput
//	GpioDataRegs.GPACLEAR.bit.GPIO12 = 1;	// uncomment if --> Set Low initially
//	GpioDataRegs.GPASET.bit.GPIO12 = 1;		// uncomment if --> Set High initially

//--------------------------------------------------------------------------------------
//  GPIO-16 - PIN FUNCTION = --Spare--
//	GpioCtrlRegs.GPAMUX2.bit.GPIO16 = 3;	// 0=GPIO, 1=SCIRXA, 2=SDAA, 3=TZ2
//	GpioCtrlRegs.GPADIR.bit.GPIO16 = 0;		// 1=OUTput,  0=INput
//	GpioDataRegs.GPACLEAR.bit.GPIO16 = 1;	// uncomment if --> Set Low initially
//	GpioDataRegs.GPASET.bit.GPIO16 = 1;		// uncomment if --> Set High initially

//--------------------------------------------------------------------------------------
//  GPIO-17 - PIN FUNCTION = --Spare--
//	GpioCtrlRegs.GPAMUX2.bit.GPIO17 = 3;	// 0=GPIO, 1=SCIRXA, 2=SDAA, 3=TZ2
//	GpioCtrlRegs.GPADIR.bit.GPIO17 = 0;		// 1=OUTput,  0=INput
//	GpioDataRegs.GPACLEAR.bit.GPIO17 = 1;	// uncomment if --> Set Low initially
//	GpioDataRegs.GPASET.bit.GPIO17 = 1;		// uncomment if --> Set High initially

  SetSCITMSmon(&SciaRegs);	//Setup HiTerm communications port

	EDIS;	// Disable register access
}

/******************************************************************************/
// A TMSmon protokolhoz megfeleoen allítja be a soros portot.
void SetSCITMSmon(volatile struct SCI_REGS *ptr)
{
	sci_ptr_terminal = ptr;

  sci_ptr_terminal->SCICCR.all = 7;  //1 stop bit,no parity, no test, idle–line protocol, 8bits

	sci_ptr_terminal->SCICTL2.bit.RXBKINTENA=0;
	sci_ptr_terminal->SCICTL2.bit.TXINTENA=0;

	sci_ptr_terminal->SCIPRI.bit.FREE = 0;
	sci_ptr_terminal->SCIPRI.bit.SOFT = 0;

	sci_ptr_terminal->SCICTL1.bit.RXERRINTENA=0;
	sci_ptr_terminal->SCICTL1.bit.SWRESET=0;

	sci_ptr_terminal->SCICTL1.bit.TXWAKE=0;
	sci_ptr_terminal->SCICTL1.bit.SLEEP=0;
	sci_ptr_terminal->SCICTL1.bit.TXENA=1;
	sci_ptr_terminal->SCICTL1.bit.RXENA=1;

  sci_ptr_terminal->SCILBAUD = (((30*1000000L)/57600-8)/8) & 0xff;
  sci_ptr_terminal->SCIHBAUD = (((30*1000000L)/57600-8)/8) >> 8;

	sci_ptr_terminal->SCICTL1.bit.RXERRINTENA=0;
	sci_ptr_terminal->SCICTL1.bit.SWRESET=1;

	sci_ptr_terminal->SCICTL1.bit.TXWAKE=0;
	sci_ptr_terminal->SCICTL1.bit.SLEEP=0;
	sci_ptr_terminal->SCICTL1.bit.TXENA=1;
	sci_ptr_terminal->SCICTL1.bit.RXENA=1;

	sci_ptr_terminal->SCIFFTX.bit.SCIFFENA = 1;
	sci_ptr_terminal->SCIFFTX.all=0xE000;
	sci_ptr_terminal->SCICTL1.all = 0x0023;     // Relinquish SCI from Reset
	sci_ptr_terminal->SCIFFTX.bit.TXFIFOXRESET = 1;

// FIFO
	sci_ptr_terminal->SCIFFRX.bit.RXFFOVRCLR = 1;
	sci_ptr_terminal->SCIFFRX.bit.RXFIFORESET = 0; // Clear and disable receiver FIFO
	sci_ptr_terminal->SCIFFRX.bit.RXFFINTCLR = 1;

	sci_ptr_terminal->SCIFFRX.bit.RXFIFORESET = 1; // Reenable FIFO
	sci_ptr_terminal->SCIFFRX.bit.RXFFIENA = 1;	// Receive FIFO interrupt enable
	sci_ptr_terminal->SCIFFRX.bit.RXFFIL = 1;   // Receive FIFO interrupt level: 1 bájt jött

}
/******************************************************************************/



//============================================================================
// NOTE:
// IN MOST APPLICATIONS THE FUNCTIONS AFTER THIS POINT CAN BE LEFT UNCHANGED
// THE USER NEED NOT REALLY UNDERSTAND THE BELOW CODE TO SUCCESSFULLY RUN THIS
// APPLICATION.
//============================================================================

void WDogDisable(void)
{
    EALLOW;
    SysCtrlRegs.WDCR= 0x0068;
    EDIS;
}

// This function initializes the PLLCR register.
//void InitPll(Uint16 val, Uint16 clkindiv)
void PLLset(Uint16 val)
{
   volatile Uint16 iVol;

   // Make sure the PLL is not running in limp mode
   if (SysCtrlRegs.PLLSTS.bit.MCLKSTS != 0)
   {
	  EALLOW;
      // OSCCLKSRC1 failure detected. PLL running in limp mode.
      // Re-enable missing clock logic.
      SysCtrlRegs.PLLSTS.bit.MCLKCLR = 1;
      EDIS;
      // Replace this line with a call to an appropriate
      // SystemShutdown(); function.
      asm("        ESTOP0");     // Uncomment for debugging purposes
   }

   // DIVSEL MUST be 0 before PLLCR can be changed from
   // 0x0000. It is set to 0 by an external reset XRSn
   // This puts us in 1/4
   if (SysCtrlRegs.PLLSTS.bit.DIVSEL != 0)
   {
       EALLOW;
       SysCtrlRegs.PLLSTS.bit.DIVSEL = 0;
       EDIS;
   }

   // Change the PLLCR
   if (SysCtrlRegs.PLLCR.bit.DIV != val)
   {

      EALLOW;
      // Before setting PLLCR turn off missing clock detect logic
      SysCtrlRegs.PLLSTS.bit.MCLKOFF = 1;
      SysCtrlRegs.PLLCR.bit.DIV = val;
      EDIS;

      // Optional: Wait for PLL to lock.
      // During this time the CPU will switch to OSCCLK/2 until
      // the PLL is stable.  Once the PLL is stable the CPU will
      // switch to the new PLL value.
      //
      // This time-to-lock is monitored by a PLL lock counter.
      //
      // Code is not required to sit and wait for the PLL to lock.
      // However, if the code does anything that is timing critical,
      // and requires the correct clock be locked, then it is best to
      // wait until this switching has completed.

      // Wait for the PLL lock bit to be set.
      // The watchdog should be disabled before this loop, or fed within
      // the loop via ServiceDog().

	  // Uncomment to disable the watchdog
      WDogDisable();

	  while(SysCtrlRegs.PLLSTS.bit.PLLLOCKS != 1) {}

      EALLOW;
      SysCtrlRegs.PLLSTS.bit.MCLKOFF = 0;
	  EDIS;
	}

	  //divide down SysClk by 2 to increase stability
	EALLOW;
	SysCtrlRegs.PLLSTS.bit.DIVSEL = 2;
	EDIS;
}

// This function initializes the Flash Control registers

//                   CAUTION
// This function MUST be executed out of RAM. Executing it
// out of OTP/Flash will yield unpredictable results

void InitFlash(void)
{
   EALLOW;
   //Enable Flash Pipeline mode to improve performance
   //of code executed from Flash.
   FlashRegs.FOPT.bit.ENPIPE = 1;

   //                CAUTION
   //Minimum waitstates required for the flash operating
   //at a given CPU rate must be characterized by TI.
   //Refer to the datasheet for the latest information.

   //Set the Paged Waitstate for the Flash
   FlashRegs.FBANKWAIT.bit.PAGEWAIT = 2;

   //Set the Random Waitstate for the Flash
   FlashRegs.FBANKWAIT.bit.RANDWAIT = 2;

   //Set the Waitstate for the OTP
   FlashRegs.FOTPWAIT.bit.OTPWAIT = 3;

   //                CAUTION
   //ONLY THE DEFAULT VALUE FOR THESE 2 REGISTERS SHOULD BE USED
   FlashRegs.FSTDBYWAIT.bit.STDBYWAIT = 0x01FF;
   FlashRegs.FACTIVEWAIT.bit.ACTIVEWAIT = 0x01FF;
   EDIS;

   //Force a pipeline flush to ensure that the write to
   //the last register configured occurs before returning.

   asm(" RPT #7 || NOP");
}


// This function will copy the specified memory contents from
// one location to another.
//
//	Uint16 *SourceAddr        Pointer to the first word to be moved
//                          SourceAddr < SourceEndAddr
//	Uint16* SourceEndAddr     Pointer to the last word to be moved
//	Uint16* DestAddr          Pointer to the first destination word
//
// No checks are made for invalid memory locations or that the
// end address is > then the first start address.

void MemCopy(Uint16 *SourceAddr, Uint16* SourceEndAddr, Uint16* DestAddr)
{
    while(SourceAddr < SourceEndAddr)
    {
       *DestAddr++ = *SourceAddr++;
    }
    return;
}
