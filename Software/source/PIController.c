#include "PIController.h"

PI_s batteryCurrentControlValues;

Uint16 duty = 0;
MEASUREMENT_TYPE Ireference = MEASUREMENT_CONVERSION(0.0);

_iq20 multiple_1;
_iq20 multiple_2;
_iq20 sum;

void initControlValueStructures(){
	EPwm1Regs.CMPA.half.CMPA = 750;
	batteryCurrentControlValues.output = 0;
	batteryCurrentControlValues.IError = 0;
	batteryCurrentControlValues.oldIError = 0;
	batteryCurrentControlValues.IDiff = 0;
	batteryCurrentControlValues.min_integrator = _IQ20(0.1);
	batteryCurrentControlValues.max_integrator = _IQ20(0.9);
	batteryCurrentControlValues.P = _IQ20(0.48031);
	batteryCurrentControlValues.I = _IQ20(0.1231);
}

void currentController(MEASUREMENT_TYPE current){
	MEASUREMENT_TYPE Ierror = 0;

	//Calculate error
	Ierror = Ireference - current;
	batteryCurrentControlValues.IError = Ierror;
	batteryCurrentControlValues.IDiff = batteryCurrentControlValues.IError -
										batteryCurrentControlValues.oldIError;


	multiple_1 = _IQ20mpyIQX(batteryCurrentControlValues.IError, 14,
			  batteryCurrentControlValues.I, 20);

	multiple_2 = _IQ20mpyIQX(batteryCurrentControlValues.IDiff, 14,
			  batteryCurrentControlValues.P, 20);

	sum = multiple_1 + multiple_2;


	/*ÁRAMSZABÁLYZÓ KEZDETE*/
	//Calculate sum(Ierror*I + dIerror * Ap)
	batteryCurrentControlValues.output += sum;
	//perform z^-1
	batteryCurrentControlValues.oldIError = batteryCurrentControlValues.IError;

	//perform saturation on integrator output
	if (batteryCurrentControlValues.output > batteryCurrentControlValues.max_integrator){
		batteryCurrentControlValues.output = batteryCurrentControlValues.max_integrator;
	}
	if (batteryCurrentControlValues.output < batteryCurrentControlValues.min_integrator){
		batteryCurrentControlValues.output = batteryCurrentControlValues.min_integrator;
	}
	/*ÁRAMSZABÁLYZÓ VÉGE*/

	duty = _IQ1mpyIQX(batteryCurrentControlValues.output, 20, (long)EPwm1Regs.TBPRD, 0) >> 1;
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
