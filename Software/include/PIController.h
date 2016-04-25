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
	_iq20 max_integrator;
	_iq20 min_integrator;
	_iq20 output;
} PI_s;

void initControlValueStructures();
void currentController(MEASUREMENT_TYPE current, MEASUREMENT_TYPE Ireference);
Uint16 PWMTest(Uint16 pwm_counter);

#endif /* INCLUDE_PICONTROLLER_H_ */
