/*
 * StateMachine.h
 *
 *  Created on: 2016 ápr. 24
 *      Author: Fehér
 */

#ifndef INCLUDE_STATEMACHINE_H_
#define INCLUDE_STATEMACHINE_H_

#include "adcMeasurements.h"
#include "PIController.h"

#define highBatteryVoltageLimit			MEASUREMENT_CONVERSION(4.2)
#define lowBatteryVoltageLimit			MEASUREMENT_CONVERSION(3.4)
#define inputVoltageLimit			MEASUREMENT_CONVERSION(5.0)
#define batteryCurrentLimit			MEASUREMENT_CONVERSION(-1.0)
#define outputCurrentLimit			MEASUREMENT_CONVERSION(3.0)

void stateMachine();
void forcePWMLock(Uint16 TZforce);
void forcePWMRelease(Uint16 TZclear);

#endif /* INCLUDE_STATEMACHINE_H_ */
