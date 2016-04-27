#include "StateMachine.h"

extern MEASUREMENT_TYPE current_sample_cnt;

Uint16 TZclear = 0;
Uint16 TZforce = 0;

MEASUREMENT_TYPE batteryVoltage;
MEASUREMENT_TYPE batteryCurrentOffset;
MEASUREMENT_TYPE batteryCurrent;
MEASUREMENT_TYPE inputVoltage;
MEASUREMENT_TYPE outputVoltage;

MEASUREMENT_TYPE Iref = MEASUREMENT_CONVERSION(-0.5);

void stateMachine(){
	outputVoltage = calculateOutputVoltage(AdcResult.ADCRESULT0);
	inputVoltage = calculateInputVoltage(AdcResult.ADCRESULT1);
	batteryVoltage = calculateBatteryVoltage(AdcResult.ADCRESULT2);
	batteryCurrent = calculateBatteryCurrent(AdcResult.ADCRESULT3);

	if(current_sample_cnt == CURRENT_SAMPLE){
		currentController(batteryCurrent, Iref);
//		if(inputVoltage >= inputVoltageLimit && batteryVoltage >= highBatteryVoltageLimit){
//			EALLOW;
//			GpioDataRegs.GPASET.bit.GPIO2 = 1;		//Set High initially
//			EDIS;
//			currentController(batteryCurrent, 0);
//		}
//		else if(inputVoltage >= inputVoltageLimit && batteryVoltage < highBatteryVoltageLimit){
//			EALLOW;
//			GpioDataRegs.GPASET.bit.GPIO2 = 1;		//Set High initially
//			EDIS;
//			currentController(batteryCurrent, batteryCurrentLimit);
//		}
//		else if(inputVoltage <= inputVoltageLimit && batteryVoltage >= highBatteryVoltageLimit){
//			EALLOW;
//			GpioDataRegs.GPACLEAR.bit.GPIO2 = 1; //Set Low initially
//			EDIS;
//			currentController(batteryCurrent, outputCurrentLimit);
//		}
//		else if(inputVoltage <= inputVoltageLimit && batteryVoltage >= lowBatteryVoltageLimit){
//			EALLOW;
//			GpioDataRegs.GPACLEAR.bit.GPIO2 = 1;	//Set Low initially
//			EDIS;
//			forcePWMLock(1);
//		}
	}
	else{
		calculateBatteryCurrentOffset(AdcResult.ADCRESULT3);
		if(current_sample_cnt == CURRENT_SAMPLE){
			//LED1 set after current offset measurement
			GpioDataRegs.GPBSET.bit.GPIO33 = 1;
		}
	}
}

void forcePWMLock(Uint16 forceBit){
	if(forceBit){
		EALLOW;
		EPwm1Regs.TZFRC.bit.OST = 1;
		EDIS;
		TZforce = 0;
	}
}

void forcePWMRelease(Uint16 clearBit){
	if(clearBit){
		EALLOW;
		EPwm1Regs.TZCLR.bit.OST = 1;
		EDIS;
		TZclear = 0;
	}
}
