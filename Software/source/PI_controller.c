#include "PI_controller.h"

PI_s batteryCurrentControlValues;

void initControlValueStructures(){
	batteryCurrentControlValues.measured = 0;
	batteryCurrentControlValues.measured_old = 0;
	batteryCurrentControlValues.min_integrator = 0;
	batteryCurrentControlValues.max_integrator = 1;
	batteryCurrentControlValues.P = MEASUREMENT_CONVERSION(0);
	batteryCurrentControlValues.I = MEASUREMENT_CONVERSION(0);
}

void currentController(MEASUREMENT_TYPE current){
	MEASUREMENT_TYPE duty = 0;
	MEASUREMENT_TYPE Ierror = 0;
	MEASUREMENT_TYPE Ireference = MEASUREMENT_CONVERSION(1.0);

	//Calculate error
	Ierror = Ireference - current;
	batteryCurrentControlValues.measured = Ierror;

	/*ÁRAMSZABÁLYZÓ KEZDETE*/
	//Calculate sum(Ierror*I + dIerror * Ap)
	batteryCurrentControlValues.output += batteryCurrentControlValues.measured *
										  batteryCurrentControlValues.I +
										  (batteryCurrentControlValues.measured - batteryCurrentControlValues.measured_old) *
										  batteryCurrentControlValues.P;
	//perform z^-1
	batteryCurrentControlValues.measured_old = batteryCurrentControlValues.measured;

	//perform saturation on integrator output
	if (batteryCurrentControlValues.output > batteryCurrentControlValues.max_integrator){
		batteryCurrentControlValues.output = batteryCurrentControlValues.max_integrator;
	}
	if (batteryCurrentControlValues.output < batteryCurrentControlValues.min_integrator){
		batteryCurrentControlValues.output = batteryCurrentControlValues.min_integrator;
	}
	/*ÁRAMSZABÁLYZÓ VÉGE*/

	duty = batteryCurrentControlValues.output;
	EPwm1Regs.CMPA.half.CMPA = (Uint16)(EPwm1Regs.TBPRD * duty);	//set PWM compare register
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
