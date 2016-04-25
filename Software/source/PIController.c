#include "PIController.h"

PI_s batteryCurVls;
PI_s outputVoltVls;

Uint16 duty = 0;

_iq20 multiple_1;
_iq20 multiple_2;
_iq20 sum;

void initControlValueStructures(){
	EPwm1Regs.CMPA.half.CMPA = 750;
	batteryCurVls.output = 0;
	batteryCurVls.Error = 0;
	batteryCurVls.oldError = 0;
	batteryCurVls.Diff = 0;
	batteryCurVls.min_integrator = _IQ20(0.1);
	batteryCurVls.max_integrator = _IQ20(0.9);
	batteryCurVls.P = _IQ20(0.048);
	batteryCurVls.I = _IQ20(0.01231);

	outputVoltVls.output = 0;
	outputVoltVls.Error = 0;
	outputVoltVls.oldError = 0;
	outputVoltVls.Diff = 0;
	outputVoltVls.min_integrator = _IQ20(0.1);
	outputVoltVls.max_integrator = _IQ20(0.9);
	outputVoltVls.P = _IQ20(0.48031);
	outputVoltVls.I = _IQ20(0.1231);
}

void currentController(MEASUREMENT_TYPE current, MEASUREMENT_TYPE Ireference){
	//Calculate error
	if (Ireference > 0){
		batteryCurVls.Error = Ireference - current;
	}
	else{
		batteryCurVls.Error = current - Ireference;
	}
	batteryCurVls.Diff = batteryCurVls.Error - batteryCurVls.oldError;

	multiple_1 = _IQ20mpyIQX(batteryCurVls.Error, 14, batteryCurVls.I, 20);
	multiple_2 = _IQ20mpyIQX(batteryCurVls.Diff, 14, batteryCurVls.P, 20);
	sum = multiple_1 + multiple_2;

	/*ÁRAMSZABÁLYZÓ KEZDETE*/
	//Calculate sum(Ierror*I + dIerror * Ap)
	batteryCurVls.output += sum;
	//perform z^-1
	batteryCurVls.oldError = batteryCurVls.Error;
	//perform saturation on integrator output
	if (batteryCurVls.output > batteryCurVls.max_integrator){
		batteryCurVls.output = batteryCurVls.max_integrator;
	}
	if (batteryCurVls.output < batteryCurVls.min_integrator){
		batteryCurVls.output = batteryCurVls.min_integrator;
	}
	/*ÁRAMSZABÁLYZÓ VÉGE*/
	duty = _IQ1mpyIQX(batteryCurVls.output, 20, (long)EPwm1Regs.TBPRD, 0) >> 1;
	EPwm1Regs.CMPA.half.CMPA = EPwm1Regs.TBPRD - duty;	//set PWM compare register
}


void voltageController(MEASUREMENT_TYPE voltage, MEASUREMENT_TYPE Ureference){
	//Calculate error
	outputVoltVls.Error = voltage - Ureference;
	outputVoltVls.Diff = outputVoltVls.Error - outputVoltVls.oldError;

	multiple_1 = _IQ20mpyIQX(outputVoltVls.Error, 14, outputVoltVls.I, 20);
	multiple_2 = _IQ20mpyIQX(outputVoltVls.Diff, 14, outputVoltVls.P, 20);
	sum = multiple_1 + multiple_2;


	/*FESZÜLTSÉG KEZDETE*/
	//Calculate sum(Ierror*I + dIerror * Ap)
	outputVoltVls.output += sum;
	//perform z^-1
	outputVoltVls.oldError = outputVoltVls.Error;
	//perform saturation on integrator output
	if (outputVoltVls.output > outputVoltVls.max_integrator){
		outputVoltVls.output = outputVoltVls.max_integrator;
	}
	if (outputVoltVls.output < outputVoltVls.min_integrator){
		outputVoltVls.output = outputVoltVls.min_integrator;
	}
	/*FESZÜLTSÉG VÉGE*/
	duty = _IQ1mpyIQX(outputVoltVls.output, 20, (long)EPwm1Regs.TBPRD, 0) >> 1;
	EPwm1Regs.CMPA.half.CMPA = EPwm1Regs.TBPRD - duty;	//set PWM compare register
}


Uint16 PWMTest(Uint16 pwm_counter){

	Uint16 pwm_duty = 0;
	Uint16 pwm_counter_max = 100;
	Uint16 down = 1;

	if(pwm_counter >= pwm_counter_max){
		pwm_counter = 0;
		if (pwm_duty < 1500)
		{
			if(down == 1){
				pwm_duty -= 10;
			}
			else{
				pwm_duty += 10;
			}
		}
		if(pwm_duty <= 10){
			pwm_duty = 1;
			down = 0;
		}
		if(pwm_duty >= 1500 && pwm_duty < 2000){
			pwm_duty = 1499;
			down = 1;
		}
	}
	return pwm_duty;
}
