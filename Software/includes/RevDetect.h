#ifndef _REVISIONDETECT_H_
	#define _REVISIONDETECT_H_

	/* Invalid revisions */
	#define REV_INVALID0			0x00
	#define REV_INVALID1			0x0F
	/* HPC250HL revisions */
	#define REV_HPC250HL_rev1	0x01
	#define REV_HPC250HL_rev3	0x02
	#define REV_HPC250HL_rev4	0x03
	#define REV_HPC250HL_rev5	0x04
	#define REV_HPC250HL_rev6	0x05
	#define REV_HPC250HL_rev7	0x06
	#define REV_HPC250HL_rev8	0x07
	/* HPC500HL revisions */
	#define REV_HPC500HL_rev1	0x08
	#define REV_HPC500HL_rev3	0x09
	#define REV_HPC500HL_rev4	0x0A
	#define REV_HPC500HL_rev5	0x0B
	#define REV_HPC500HL_rev6	0x0C
	#define REV_HPC500HL_rev7	0x0D
	#define REV_HPC500HL_rev8	0x0E

	/* extern variables */
	extern uint8_t Revision;

	/* function definitions */
	void InitRevisionDetect(void);	//Initialize DSP IO pins
	uint8_t RevisionDetect(void);		//if invalid revisions detected returns TRUE else return FALSE
#endif
