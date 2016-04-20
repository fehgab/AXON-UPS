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
	MEASUREMENT_TYPE measured;
	MEASUREMENT_TYPE measured_old;
	MEASUREMENT_TYPE P;
	MEASUREMENT_TYPE I;
	MEASUREMENT_TYPE max_integrator;
	MEASUREMENT_TYPE min_integrator;
	MEASUREMENT_TYPE output;
} PI_s;

void currentController(MEASUREMENT_TYPE current);
Uint16 PWMTest(Uint16 pwm_counter);

#endif /* INCLUDE_PICONTROLLER_H_ */
