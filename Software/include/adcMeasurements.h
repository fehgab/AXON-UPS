#ifndef _ADCM_H_
#define _ADCM_H_
#include "DSP2802x_Device.h"
#include "IQmathLib.h"

#define MEASUREMENT_TYPE					_iq14
#define MEASUREMENT_MULTIPLE				_IQ14mpy
#define MEASUREMENT_DEVIDE					_IQ14div
#define MEASUREMENT_CONVERSION				_IQ14

#define CURRENT_SAMPLE						100
#define CURRENT_SAMPLE_DEVIDER				_IQ3(CURRENT_SAMPLE)

#define BATTERY_VOLTAGE_MULTIPLIER			_IQ14(3.3 / 4095 * 1.367721063)
#define BATTERY_CURRENT_MULTIPLIER			_IQ14(0.7 / 4095 * 15.87 * 2)
#define INPUT_VOLTAGE_MULTIPLIER			_IQ14(3.3 / 4095 * 1.78)
#define OUTPUT_VOLTAGE_MULTIPLIER			_IQ14(3.3 / 4095 * 1.77)


MEASUREMENT_TYPE calculateBatteryVoltage(Uint16 measurement);
MEASUREMENT_TYPE calculateBatteryCurrent(Uint16 measurement);
void calculateBatteryCurrentOffset(Uint16 measurement);
MEASUREMENT_TYPE calculateInputVoltage(Uint16 measurement);
MEASUREMENT_TYPE calculateOutputVoltage(Uint16 measurement);

#endif
