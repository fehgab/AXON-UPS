#include "PIController.h"

PI_s batteryCurVls;

Uint16 duty = 0;

_iq20 multiple_1;
_iq20 multiple_2;
_iq20 sum;

void initControlValueStructures(){
	EPwm1Regs.CMPA.half.CMPA = 750;
	batteryCurVls.output = 0;
	batteryCurVls.IError = 0;
	batteryCurVls.oldIError = 0;
	batteryCurVls.IDiff = 0;
	batteryCurVls.min_integrator = _IQ20(0.1);
	batteryCurVls.max_integrator = _IQ20(0.9);
	batteryCurVls.P = _IQ20(0.48031);
	batteryCurVls.I = _IQ20(0.1231);
}

void currentController(MEASUREMENT_TYPE current, MEASUREMENT_TYPE Ireference){
	MEASUREMENT_TYPE Ierror = 0;

	//Calculate error
	Ierror = Ireference - current;
	batteryCurVls.IError = Ierror;
	batteryCurVls.IDiff = batteryCurVls.IError - batteryCurVls.oldIError;

	//Multiples
	multiple_1 = _IQ20mpyIQX(batteryCurVls.IError, 14,
			  batteryCurVls.I, 20);

	multiple_2 = _IQ20mpyIQX(batteryCurVls.IDiff, 14,
			  batteryCurVls.P, 20);

	sum = multiple_1 + multiple_2;

	/*ÁRAMSZABÁLYZÓ KEZDETE*/
	//Calculate sum(Ierror*I + dIerror * Ap)
	batteryCurVls.output += sum;
	//perform z^-1
	batteryCurVls.oldIError = batteryCurVls.IError;

	//perform saturation on integrator output
	if (batteryCurVls.output >
				batteryCurVls.max_integrator){
		batteryCurVls.output =
				batteryCurVls.max_integrator;
	}
	if (batteryCurVls.output <
				batteryCurVls.min_integrator){
		batteryCurVls.output =
				batteryCurVls.min_integrator;
	}
	/*ÁRAMSZABÁLYZÓ VÉGE*/

	duty = _IQ1mpyIQX(batteryCurVls.output, 20, (long)EPwm1Regs.TBPRD, 0) >> 1;
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
