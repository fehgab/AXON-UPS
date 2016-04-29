/*
 * PI-Controller.h
 *
 *  Created on: 2016 ápr. 19
 *      Author: Fehér
 */

#ifndef INCLUDE_PICONTROLLER_H_
#define INCLUDE_PICONTROLLER_H_
#include "DSP2802x_Device.h"
#include "IQmathLib.h"
#include "adcMeasurements.h"

typedef struct {
	MEASUREMENT_TYPE Error;
	MEASUREMENT_TYPE oldError;
	_iq14 Diff;
	_iq20 P;
	_iq20 I;
	_iq14 max_integrator;
	_iq14 min_integrator;
	_iq14 output;
} PI_s;

void initControlValueStructures();
void currentController(MEASUREMENT_TYPE current, MEASUREMENT_TYPE Ireference);
_iq14 outputVoltageController(MEASUREMENT_TYPE voltage, MEASUREMENT_TYPE Ureference);
Uint16 PWMTest(Uint16 pwm_counter);

#endif /* INCLUDE_PICONTROLLER_H_ */
