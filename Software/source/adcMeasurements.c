#include "adcMeasurements.h"

Uint16 current_sample_cnt = MEASUREMENT_CONVERSION(0);
_iq3 iq3_current_offset = MEASUREMENT_CONVERSION(0);
_iq3 iq3_current_meas;

MEASUREMENT_TYPE calculateBatteryVoltage(Uint16 measurement){
	return MEASUREMENT_MULTIPLE(MEASUREMENT_MULTIPLE(MAXIMUM_VOLTAGE, MEASUREMENT_DEVIDE(measurement, (Uint16)DEVIDER)), BATTERY_VOLTAGE_MULTIPLIER);
}

MEASUREMENT_TYPE calculateInputVoltage(Uint16 measurement){
	return MEASUREMENT_MULTIPLE(MEASUREMENT_MULTIPLE(MAXIMUM_VOLTAGE, MEASUREMENT_DEVIDE(measurement, (Uint16)DEVIDER)), INPUT_VOLTAGE_MULTIPLIER);
}

MEASUREMENT_TYPE calculateOutputVoltage(Uint16 measurement){
	return MEASUREMENT_MULTIPLE(MEASUREMENT_MULTIPLE(MAXIMUM_VOLTAGE, MEASUREMENT_DEVIDE(measurement, (Uint16)DEVIDER)), OUTPUT_VOLTAGE_MULTIPLIER);
}

void calculateBatteryCurrentOffset(Uint16 measurement){
	//LED1 clear before current offset measurement
	GpioDataRegs.GPBCLEAR.bit.GPIO33 = 1;
	while(current_sample_cnt < CURRENT_SAMPLE){
		iq3_current_offset += _IQ3div(_IQ3(measurement), CURRENT_SAMPLE_DEVIDER);
		current_sample_cnt++;
		break;
	}
	//LED1 set after current offset measurement
	GpioDataRegs.GPBSET.bit.GPIO33 = 1;
}

MEASUREMENT_TYPE calculateBatteryCurrent(Uint16 measurement){
	iq3_current_meas = _IQ3(measurement) - iq3_current_offset;
	return MEASUREMENT_MULTIPLE(MEASUREMENT_MULTIPLE(MAXIMUM_CURRENT, MEASUREMENT_DEVIDE(iq3_current_meas, IQ3_DEVIDER)), BATTERY_CURRENT_MULTIPLIER);
}
