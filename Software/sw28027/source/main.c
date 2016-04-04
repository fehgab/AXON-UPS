#include "DSP2802x_Device.h"
#include "inic.h"
#include "i2c.h"
#include "HiMon.h"
#include "IQmathLib.h"

/* Timer variables for 10ms 100ms and 1s tasks */
Uint16 sw_timer_1ms;
Uint16 adc_int_cnt;

Uint16 pwm_duty;

void ISR_ILLEGAL(void);

__interrupt void cpu_timer0_isr(void)
{
	sw_timer_1ms++;

  // Acknowledge this interrupt to receive more interrupts from group 1
  PieCtrlRegs.PIEACK.all = PIEACK_GROUP1;
}

__interrupt void  adc_isr(void)
{
	//Set 0-1500 0->100% duty cycle, 1500 -> 0% duty cycle
	if (pwm_duty <= 1500 && pwm_duty > 0)
	{
		EPwm1Regs.CMPA.half.CMPA 	= pwm_duty;	// Set compare A value
	}

	adc_int_cnt++;

	bbx_trigger();
  AdcRegs.ADCINTFLGCLR.bit.ADCINT1 = 1;		//Clear ADCINT1 flag reinitialize for next SOC
  PieCtrlRegs.PIEACK.all = PIEACK_GROUP1;   // Acknowledge interrupt to PIE
  return;
}

interrupt void ISR_ILLEGAL(void)   // Illegal operation TRAP
{
  // Insert ISR Code here

  // Next two lines for debug only to halt the processor here
  // Remove after inserting ISR Code
  asm("          ESTOP0");
  for(;;);

}

void main(void) {
	EALLOW;

	pwm_duty = 1500;

	DeviceInit();	// Device Life support & GPIO mux settings

	// Enable ADCINT1 in PIE
	IER |= M_INT1; 						// Enable CPU Interrupt 1

	PieCtrlRegs.PIEIER1.bit.INTx1 = 1;	// Enable INT 1.1 in the PIE
	PieCtrlRegs.PIEIER1.bit.INTx7 = 1;	// Enable TINT0 in the PIE: Group 1 interrupt 7

	EINT;          						// Enable Global interrupt INTM
	ERTM;          						// Enable Global realtime interrupt DBGM

	i2c_force_stop();
	InitTermVars();

	for(;;)
	{
		Serial_mng();
    bbx_background();
	}
}
