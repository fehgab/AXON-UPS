#include "StateMachine.h"

extern MEASUREMENT_TYPE current_sample_cnt;
extern Uint16 sw_timer_1ms;

Uint16 TZclear = 0;
Uint16 TZforce = 0;
Uint16 state = 1;

MEASUREMENT_TYPE batteryVoltage;
MEASUREMENT_TYPE batteryCurrentOffset;
MEASUREMENT_TYPE batteryCurrent;
MEASUREMENT_TYPE inputVoltage;
MEASUREMENT_TYPE outputVoltage;

MEASUREMENT_TYPE Iref = MEASUREMENT_CONVERSION(-0.5);
MEASUREMENT_TYPE Uref = MEASUREMENT_CONVERSION(5.0);

void stateMachine(){
	outputVoltage = calculateOutputVoltage(AdcResult.ADCRESULT0);
	inputVoltage = calculateInputVoltage(AdcResult.ADCRESULT1);
	batteryVoltage = calculateBatteryVoltage(AdcResult.ADCRESULT2);
	batteryCurrent = calculateBatteryCurrent(AdcResult.ADCRESULT3);

	if(current_sample_cnt == CURRENT_SAMPLE){
		switch(state){
			//BUCK starting state, calculate starting duty
			case 1:
				if(sw_timer_1ms >= 1000){
					//Go to Buck
					if(inputVoltage >= inputVoltageLimit){
						EPwm1Regs.CMPA.half.CMPA = _IQ1mpyIQX(MEASUREMENT_DEVIDE(batteryVoltage, outputVoltage), 14, (long)EPwm1Regs.TBPRD, 0) >> 1;
						state = 3;
					}
					//Go to Boost
					if(inputVoltage < lowBatteryVoltageLimit && batteryVoltage >= lowBatteryVoltageLimit){
						GpioDataRegs.GPACLEAR.bit.GPIO2 = 1;
						state = 2;
					}
				}
				break;
			//Boost state
			case 2:
				voltageController(outputVoltage, Uref);
				//Go to Buck
				if(inputVoltage >= inputVoltageLimit){
					GpioDataRegs.GPASET.bit.GPIO2 = 1;
					state = 1;
					state = 3; // Itt tudunk egybol BUCK uzembe menni?
				}
				//Go to depleted
				if(batteryVoltage < lowBatteryVoltageLimit){
					state = 4;
				}
				break;
			//BUCK state
			case 3:
				//currentController(batteryCurrent, Iref);
				//Go to Boost
				if(inputVoltage < lowBatteryVoltageLimit && batteryVoltage >= lowBatteryVoltageLimit){
					state = 2;
				}
				break;
			//Akkumulator depleted
			case 4:
				//Turn of PWM, go to empty state
				forcePWMLock(1);
				state = 5;
				break;
			default:
				break;
		}
	}
	//Collect current samples and calculate offset
	else{
		calculateBatteryCurrentOffset(AdcResult.ADCRESULT3);
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
