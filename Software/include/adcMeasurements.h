#ifndef _ADCM_H_
#define _ADCM_H_
#include "DSP2802x_Device.h"
#include "IQmathLib.h"

#define MEASUREMENT_TYPE					_iq14
#define MEASUREMENT_MULTIPLE				_IQ14mpy
#define MEASUREMENT_DEVIDE					_IQ14div
#define MEASUREMENT_CONVERSION				_IQ14

#define DEVIDER								4095
#define IQ3_DEVIDER							_IQ3(4095)
#define IQ6_DEVIDER							_IQ6(4095)
#define IQ14_DEVIDER						_IQ14(4095)
#define MAXIMUM_VOLTAGE						_IQ14(3.3)

#define CURRENT_SAMPLE						100
#define CURRENT_SAMPLE_DEVIDER				_IQ3(CURRENT_SAMPLE)
#define MAXIMUM_CURRENT						_IQ14(5)

#define BATTERY_VOLTAGE_MULTIPLIER			_IQ14(1.367721063)
#define BATTERY_CURRENT_MULTIPLIER			_IQ14(0.7)
#define INPUT_VOLTAGE_MULTIPLIER			_IQ14(1.726219726)
#define OUTPUT_VOLTAGE_MULTIPLIER			_IQ14(1.734561356)


MEASUREMENT_TYPE calculateBatteryVoltage(Uint16 measurement);
MEASUREMENT_TYPE calculateBatteryCurrent(Uint16 measurement);
void calculateBatteryCurrentOffset(Uint16 measurement);
MEASUREMENT_TYPE calculateInputVoltage(Uint16 measurement);
MEASUREMENT_TYPE calculateOutputVoltage(Uint16 measurement);

#endif
