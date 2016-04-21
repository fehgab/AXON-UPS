#include <PIController.h>
#include "DSP2802x_Device.h"
#include "inic.h"
#include "i2c.h"
#include "HiMon.h"
#include "IQmathLib.h"
#include "adcMeasurements.h"
#include "bbx.h"

extern MEASUREMENT_TYPE current_sample_cnt;

/* Timer variables for 10ms 100ms and 1s tasks */
Uint16 sw_timer_1ms;
Uint16 adc_int_cnt;

Uint16 pwm_duty = 0;
Uint16 TZclear = 0;
Uint16 TZforce = 0;

MEASUREMENT_TYPE batteryVoltage;
MEASUREMENT_TYPE batteryCurrentOffset;
MEASUREMENT_TYPE batteryCurrent;
MEASUREMENT_TYPE inputVoltage;
MEASUREMENT_TYPE outputVoltage;

Uint16 outputVoltageResult = 0;
Uint16 inputVoltageResult = 0;
Uint16 batteryVoltageResult = 0;
Uint16 batteryCurrentResult = 0;

void ISR_ILLEGAL(void);

__interrupt void cpu_timer0_isr(void)
{
	sw_timer_1ms++;
	if(TZclear){
		EALLOW;
		EPwm1Regs.TZCLR.bit.OST = 1;
		EDIS;
		TZclear = 0;
	}
	if(TZforce){
			EALLOW;
			EPwm1Regs.TZFRC.bit.OST = 1;
			EDIS;
			TZforce = 0;
	}

  // Acknowledge this interrupt to receive more interrupts from group 1
  PieCtrlRegs.PIEACK.all = PIEACK_GROUP1;
}

__interrupt void  adc_isr(void)
{
	outputVoltageResult = AdcResult.ADCRESULT0;
	outputVoltage = calculateOutputVoltage(AdcResult.ADCRESULT0);

		inputVoltageResult = AdcResult.ADCRESULT1;
	inputVoltage = calculateInputVoltage(AdcResult.ADCRESULT1);

		batteryVoltageResult = AdcResult.ADCRESULT2;
	batteryVoltage = calculateBatteryVoltage(AdcResult.ADCRESULT2);

	if(current_sample_cnt > 99){
	   batteryCurrentResult = AdcResult.ADCRESULT3;
	   batteryCurrent = calculateBatteryCurrent(AdcResult.ADCRESULT3);
	   currentController(batteryCurrent);
	}
	else{
		calculateBatteryCurrentOffset(AdcResult.ADCRESULT3);
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

	DeviceInit();	// Device Life support & GPIO mux settings
	initControlValueStructures();

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
