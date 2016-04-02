#ifndef _BBX_H_
#define _BBX_H_

#include "gen_types.h"
#include "currentdt.h"
#include "err_hyst.h"

#define BBX_MEM_FRAM		 0
#define BBX_MEM_SRAM		 1
#define BBX_MEM_DSPRAM   2

#define USE_SPI_RAM      0   /*!< use SPI ram for bbox storage */
#define ENABLE_FLOAT_BBX 1   /*!< enable to use floating point variables in bbox */
#define BBOX_USE_32BIT   1   /*!< use only 32 bit variables in the blackbox */

#define BBOX_LEN  0x0100     /*!< length of the black box in x (time) direction) */
#define NO_CH       4        /*!< number of channels used for data collecting (blackbox function) only implemented for 4*/

#define BBX_BASE         0   /*!< blackbox do nothing */
#define BBX_WAIT4TRIGGER 2   /*!< blackbox is waiting for trigger */
#define BBX_TRIGGERED    4   /*!< blackbox triggered */
#define BBX_FINISHED     5   /*!< blackbox finished data collection */
#define BBX_FORCE_START  1   /*!< blackbox starting - written to this state by HiTerm */
#define BBX_WT_WREN      6   /*!< blackbox wait write enable spi IC */
#define BBX_WT_ADDR      7   /*!< blackbox wait address set up */
#define BBX_ALL_FINISHED 3   /*!< blackbox finished data collection, spi closed */

#define BBX_TV_SIGNED    0x01   /*!< blackbox triggered value signed */
#define BBX_TV_UNSIGNED  0x00   /*!< blackbox triggered value unsigned */
#define BBX_TV_16BIT     0x02   /*!< blackbox triggered value 16bit */
#define BBX_TV_32BIT     0x00   /*!< blackbox triggered value 32bit */
#define BBX_TV_16B_UPPER 0x04   /*!< blackbox triggered value at upper16 bit (only valid for 16 bit variable) */
#define BBX_TV_FLOAT     0x08   /*!< blackbox triggered value float */

typedef struct {
  uint32_t  val_addr[NO_CH]; /*!< address of the variables to be sampled */
  int32_t   min[NO_CH];      /*!< minimum value for the variable without before shifting*/
  int32_t   max[NO_CH];      /*!< minimum value for the variable without before shifting TODO: this could be eliminated by saturating after shifting*/
  float     fp_mult[NO_CH];  /*!< Floating point multiplication, if 0 the variable is non-float type*/
  uint32_t  sampled_date;    /*!< date when the data was sampled */
  uint32_t  sampled_time;    /*!< time when the data was sampled - time of the trigger point (as much as possible)*/
  uint32_t  sample_time;     /*!< undersampling of the variables (not sampling in every call of \a bbx_trigger */
  int32_t   trig_level;      /*!< trigger level in raw (non-iq non floating point format */
  uint16_t  sh[NO_CH];       /*!< shift of the current variable (only 16 bit is going to be stored) */

  uint16_t  state;           /*!< state of the FSM of the bbx*/
  uint16_t  start_idx;       /*!< first sample of the bbx*/
  uint16_t  trigger_ch;      /*!< trigger channel 0-NO_CH-1: sampled channel; NO_CH: trigger for Errorcode */
  uint16_t  pretrigger;      /*!< number of samples collected before the trigger occurs*/
  uint16_t  trig_slope;      /*!< rising (0); falling (1) edge */
  uint16_t  tnow;            /*!< force triggering immediatelly */
  uint16_t  tlevel_type;     /*!< trigger level type, for definition see TYP_FLOAT, TYP_UNSIGNED, TYP_32BIT etc */

  uint16_t  idx;             /*!< current id of data collecting */
  uint32_t  div;             /*!< variable used for undersampling */
  int32_t  trig_ch_val_old;  /*!< used to detect rising edge/falling edge */
  int32_t  trig_ch_val;      /*!< value of the triggered channel */
  uint16_t  trig_delay;      /*!< to ensure that enough data are already in the pretrigger part of the buffer */
  uint16_t  end_idx;         /*!< last data id */
} bbx_struct_type;

extern bbx_struct_type bbx;
#if BBOX_USE_32BIT == 1
	#if USE_SPI_RAM == 0
		extern uint32_t bbox[BBOX_LEN][NO_CH]; /*!< 32 bit buffer for storing the data */
	#else
		extern uint32_t bbox[1][1]; /*!< 32 bit buffer for storing the data */
	#endif
#else
	extern uint16_t bbox[BBOX_LEN][NO_CH]; /*!< 16 bit buffer for storing the data */
#endif

extern uint8_t bbx_mem_type;
extern uint32_t bbx_mem_length;
extern void bbx_trigger();
extern void bbx_inic();

#endif
