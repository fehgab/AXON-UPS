#ifndef _ADCM_H_
#define _ADCM_H_
#include "DSP2802x_Device.h"
#include "IQmathLib.h"

#define MEASUREMENT_TYPE					_iq6
#define MEASUREMENT_MULTIPLE				_IQ6mpy
#define MEASUREMENT_DEVIDE					_IQ6div
#define MEASUREMENT_CONVERSION				_IQ6

#define DEVIDER								_IQ6(4095)
#define MAXIMUM_VOLTAGE						_IQ6(3.3)

#define CURRENT_SAMPLE						100
#define MAXIMUM_CURRENT						_IQ6(5)

#define BATTERY_VOLTAGE_MULTIPLIER			_IQ6(1.367721063)
#define BATTERY_CURRENT_MULTIPLIER			_IQ6(0.410256410)
#define INPUT_VOLTAGE_MULTIPLIER			_IQ6(1.726219726)
#define OUTPUT_VOLTAGE_MULTIPLIER			_IQ6(1.513230769)


MEASUREMENT_TYPE calculateBatteryVoltage(Uint16 measurement);
MEASUREMENT_TYPE calculateBatteryCurrent(Uint16 measurement);
void calculateBatteryCurrentOffset(Uint16 measurement);
MEASUREMENT_TYPE calculateInputVoltage(Uint16 measurement);
MEASUREMENT_TYPE calculateOutputVoltage(Uint16 measurement);

#endif
