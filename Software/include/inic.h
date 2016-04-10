#ifndef __INIC_H_
	#define __INIC_H_
	#include "DSP2802x_Device.h"
	#include "DSP2802x_usDelay.h"
	#include "DSP2802x_EPwm_defines.h"

	#define Device_cal (void   (*)(void))0x3D7C80
	extern volatile struct SCI_REGS *sci_ptr_terminal;

	#define ADC_usDELAY  1000L
	/* Function declarations */
	void DeviceInit(void);
	void SetSCITMSmon(volatile struct SCI_REGS *ptr);
	void WDogDisable(void);
	void PLLset(Uint16 val);
	void InitFlash(void);
	void MemCopy(Uint16 *SourceAddr, Uint16* SourceEndAddr, Uint16* DestAddr);
#endif
