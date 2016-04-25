#include "adcMeasurements.h"

Uint16 current_sample_cnt = 0;
_iq3 iq3_current_offset = MEASUREMENT_CONVERSION(0);
_iq3 iq3_current_meas;
MEASUREMENT_TYPE iq14_devided;
MEASUREMENT_TYPE iq14_multiplied;

MEASUREMENT_TYPE calculateBatteryVoltage(Uint16 measurement){
	return MEASUREMENT_MULTIPLE(MEASUREMENT_CONVERSION(measurement), BATTERY_VOLTAGE_MULTIPLIER);
}

MEASUREMENT_TYPE calculateInputVoltage(Uint16 measurement){
	return MEASUREMENT_MULTIPLE(MEASUREMENT_CONVERSION(measurement), INPUT_VOLTAGE_MULTIPLIER);
}

MEASUREMENT_TYPE calculateOutputVoltage(Uint16 measurement){
	return MEASUREMENT_MULTIPLE(MEASUREMENT_CONVERSION(measurement), OUTPUT_VOLTAGE_MULTIPLIER);
}

void calculateBatteryCurrentOffset(Uint16 measurement){
	if(current_sample_cnt < CURRENT_SAMPLE){
		iq3_current_offset += _IQ3div(_IQ3(measurement), CURRENT_SAMPLE_DEVIDER);
		current_sample_cnt++;
	}
}

MEASUREMENT_TYPE calculateBatteryCurrent(Uint16 measurement){
	iq3_current_meas = _IQ3((long)measurement) - iq3_current_offset;
	return _IQ14mpyIQX(iq3_current_meas, 3, BATTERY_CURRENT_MULTIPLIER, 14);
}
