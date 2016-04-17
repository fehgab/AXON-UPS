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

Uint16 pwm_counter;
Uint16 pwm_counter_max = 100;
Uint16 down = 1;
Uint16 pwm_duty = 500;

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

  // Acknowledge this interrupt to receive more interrupts from group 1
  PieCtrlRegs.PIEACK.all = PIEACK_GROUP1;
}

__interrupt void  adc_isr(void)
{
//	if(pwm_counter >= pwm_counter_max){
//		pwm_counter = 0;
//		if (pwm_duty < 1500)
//		{
//			if(down == 1){
//				pwm_duty -= 10;
//			}
//			else{
//				pwm_duty += 10;
//			}
//		}
//		if(pwm_duty <= 10){
//			pwm_duty = 1;
//			down = 0;
//		}
//		if(pwm_duty >= 1500 && pwm_duty < 2000){
//			pwm_duty = 1499;
//			down = 1;
//		}
	if(pwm_duty <= 750){
		EPwm1Regs.CMPA.half.CMPA = pwm_duty;
	}
	else{
		pwm_duty = 750;
	}

	if(adc_int_cnt >= 4){
		adc_int_cnt = 0;
	}

	switch(adc_int_cnt) {
	   case 0:
		   outputVoltageResult = AdcResult.ADCRESULT0;
		   outputVoltage = calculateOutputVoltage(AdcResult.ADCRESULT0);
		   break;
	   case 1:
		   inputVoltageResult = AdcResult.ADCRESULT1;
		   inputVoltage = calculateInputVoltage(AdcResult.ADCRESULT1);
		   break;
	   case 2:
		   batteryVoltageResult = AdcResult.ADCRESULT2;
		   batteryVoltage = calculateBatteryVoltage(AdcResult.ADCRESULT2);
		   break;
	   case 3:
		   calculateBatteryCurrentOffset(AdcResult.ADCRESULT3);
		   if(current_sample_cnt > 99){
			   batteryCurrentResult = AdcResult.ADCRESULT3;
			   batteryCurrent = calculateBatteryCurrent(AdcResult.ADCRESULT3);
		   }
		   break;
	   default:
		   break;
	}
	adc_int_cnt++;
//	pwm_counter++;

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
