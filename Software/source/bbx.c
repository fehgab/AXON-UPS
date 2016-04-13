/*! \file BBX.c
 *  \brief ("BlackBoX.c")Data collecting for the recorder function.
 *  \author Originally Gal Istvan; further development: Bilau Zoltan Tamas (IQ, float, shifting, time, now, SPI)
 *  \version 2.0
 *  \date    2010-06-25
 */

#include "gen_types.h"
//#include "IQmathLib.h"
#include "bbx.h"
#include "DSP2802x_Device.h"
#include "spi_mng.h"

bbx_struct_type bbx;
#if BBOX_USE_32BIT == 1
	#if USE_SPI_RAM == 0
		uint32_t bbox[BBOX_LEN][NO_CH];
	#else
		uint32_t bbox[1][1];
	#endif
#else
	uint16_t bbox[BBOX_LEN][NO_CH];
#endif

#define VAR5TH (PVC_Error_Code) /*!< 5th variable just for triggering */
uint16_t PVC_Error_Code;
extern uint32_t c_date ;
extern uint32_t c_time ;

uint8_t bbx_mem_type = BBX_MEM_DSPRAM;
uint32_t bbx_mem_length = 0x0100;

inline void push_bbx();
inline void push_single_bbx_32(uint16_t i);

/*!
 *  \brief Sets the id into a given value
 *  Handles overflow of idx. It handles it differently if it is a power of two (ANDing is possible), and if it is not.
 *  \param[in] ii to be saturated
 *  \return saturated value
 */
inline unsigned int sat_arr_idx(unsigned int ii)
{
  unsigned int jj = ii;
  #if ((bbx_mem_length & (bbx_mem_length - 1)) != 0)
  while (jj >= bbx_mem_length)
    jj -= bbx_mem_length;
  #else
  jj &= (bbx_mem_length - 1);
  #endif
  return jj;
}

 /*!
  *  \brief Initiates the black box variables.
  *  \author Weitzl Zoltan
  *  \version 1.0
  *	\date 2010. 07. 01.
 */
void bbx_init()
{
  int k;
  for (k = 0; k < NO_CH; k++)
  {
    bbx.val_addr[k] = 0;
    bbx.max[k] = 0;
    bbx.sh[k] = 0;
  }
  bbx.tnow = 0;
  bbx.state = BBX_BASE;
}

#define BBX_TV_SIGNED    0x01   /*!< blackbox triggered value signed */
#define BBX_TV_UNSIGNED  0x00   /*!< blackbox triggered value unsigned */
#define BBX_TV_16BIT     0x02   /*!< blackbox triggered value 16bit */
#define BBX_TV_32BIT     0x00   /*!< blackbox triggered value 32bit */
#define BBX_TV_16B_UPPER 0x04   /*!< blackbox triggered value at upper16 bit (only valid for 16 bit variable) */
#define BBX_TV_FLOAT     0x08   /*!< blackbox triggered value float */


#if BBOX_USE_32BIT == 1
/*!
 *  \brief Calculating trigger value.
 *  \author Weitzl Zoltan
 *  \version 1.0
 *	\date 2010. 07. 01.
 *  \param[in] tval
 */
int16_t CalcTrigValueGreater(int32_t tval)
{
  int16_t i16,i16_tlevel;
  int16_t ret_par;
  switch (bbx.tlevel_type)
  {
  case (BBX_TV_SIGNED | BBX_TV_32BIT):
    ret_par = tval > bbx.trig_level ? 1 : 0;
    break;
  case (BBX_TV_UNSIGNED | BBX_TV_32BIT):
    ret_par = *(uint32_t*)&tval > *(uint32_t*)&bbx.trig_level ? 1 : 0;
    break;
  case (BBX_TV_SIGNED | BBX_TV_16BIT):
    i16 = tval & 0xFFFF;
    i16_tlevel = bbx.trig_level & 0xFFFF;
    ret_par = i16 > i16_tlevel ? 1:0;
    break;
  case (BBX_TV_SIGNED | BBX_TV_16BIT | BBX_TV_16B_UPPER):
    i16 = tval >> 16;
    i16_tlevel = bbx.trig_level;
    ret_par = i16 > i16_tlevel ? 1:0;
    break;
  case (BBX_TV_UNSIGNED | BBX_TV_16BIT | BBX_TV_16B_UPPER):
    i16 = tval >> 16;
    i16_tlevel = bbx.trig_level;
    ret_par = *(uint16_t*)&i16 > *(uint16_t*)&i16_tlevel ? 1:0;
    break;
  case (BBX_TV_UNSIGNED | BBX_TV_16BIT):
    i16 = tval & 0xFFFF;
    i16_tlevel = bbx.trig_level & 0xFFFF;
    ret_par = *(uint16_t*)&i16 > *(uint16_t*)&i16_tlevel ? 1:0;
    break;
  case (BBX_TV_FLOAT):
    ret_par = *(float*)&tval > *(float*)&bbx.trig_level ? 1 : 0;
    break;
  default:
    ret_par = 1;
    break;
  }
  return ret_par;
}
#endif

/*!
 *  \brief Data Collection
 *  It collects the data in \a bbox[][] or in the SPI RAM.
 *  \author Weitzl Zoltan
 *  \version 1.0
 *	\date 2010. 07. 01.
 */
void bbx_trigger(void)
{
  /* division of the sampling frequency */
  if (++bbx.div >= bbx.sample_time)
    bbx.div = 0;
  else
    return ;

  if (bbx.state == BBX_WAIT4TRIGGER)
  // wait for the trigger
  {
    push_bbx();
    if (bbx.trig_delay)
      bbx.trig_delay--;
    else
    {
      if (!bbx.trig_slope)
      // rising edge
      {
        if (
#if BBOX_USE_32BIT == 1
            (!CalcTrigValueGreater(bbx.trig_ch_val_old) && CalcTrigValueGreater(bbx.trig_ch_val))
#else
            ((bbx.trig_ch_val_old < bbx.trig_level) && (bbx.trig_level <= bbx.trig_ch_val) && (bbx.trigger_ch != NO_CH))
#endif
           || (bbx.tnow) || (bbx.trigger_ch == 4 && bbx.trig_ch_val))
        {
          bbx.start_idx = bbx.idx;
          bbx.end_idx = sat_arr_idx(bbx.start_idx + (bbx_mem_length - 1 - bbx.pretrigger));
          bbx.state = BBX_TRIGGERED;
          bbx.sampled_date = c_date;
          bbx.sampled_time = c_time;
          bbx.tnow = 0;
        }
      }
      else
      // falling edge
      {
        if (
#if BBOX_USE_32BIT == 1
            (CalcTrigValueGreater(bbx.trig_ch_val_old) && !CalcTrigValueGreater(bbx.trig_ch_val))
#else
            ((bbx.trig_ch_val_old > bbx.trig_level) && (bbx.trig_level >= bbx.trig_ch_val) && (bbx.trigger_ch != NO_CH))
#endif
       || (bbx.tnow) || (bbx.trigger_ch == 4 && bbx.trig_ch_val))
        {
          bbx.start_idx = bbx.idx;
          bbx.end_idx = sat_arr_idx(bbx.start_idx + (bbx_mem_length - 1 - bbx.pretrigger));
          bbx.state = BBX_TRIGGERED;
          bbx.sampled_date = c_date;
          bbx.sampled_time = c_time;
          bbx.tnow = 0;
        }
      }
    }
  }
  else if (bbx.state == BBX_TRIGGERED)
  {
    if (bbx.idx == bbx.end_idx)
    // end of bbx
      bbx.state = BBX_FINISHED;
    push_bbx();
  }
  else
  // wait for starting
    bbx.trig_delay = sat_arr_idx(bbx.pretrigger + 1);
}

volatile uint16_t delme_var = 0;

/*!
 *  \brief Pushes the NO_CH data into the buffer \a bbox / \a SPI RAM.
 *  In case of SPI RAM is used (USE_SPI_RAM), bbox data is pushed into it.
 *  \author Weitzl Zoltan
 *  \version 1.0
 *	\date 2010. 07. 01.
 */

inline void push_bbx()
{
  bbx.trig_ch_val_old = bbx.trig_ch_val;
  push_single_bbx_32(0);
  push_single_bbx_32(1);
  push_single_bbx_32(2);
  push_single_bbx_32(3);

  bbx.trig_ch_val = (bbx.trigger_ch == NO_CH) ? VAR5TH : *(int32_t*)(bbx.val_addr[bbx.trigger_ch]);
  bbx.idx = sat_arr_idx(bbx.idx + 1);
}

/*!
 *  \brief Data pushed to the \a bbox buffer in 32 bit format.
 *  \author Weitzl Zoltan
 *  \version 1.0
 *	\date 2010. 07. 01.
 *  \param i the data which needs to be copying in floating format
 */
inline void push_single_bbx_32(uint16_t i)
{
  #if USE_SPI_RAM == 1
		tvar = *(uint32_t*)(bbx.val_addr[i]);
		spi_wr_reg(tvar & 0xFFFF);
		spi_wr_reg(tvar >> 16);
  #else
  	bbox[bbx.idx][i] = *(uint32_t*)(bbx.val_addr[i]);
  #endif
}

 /*!
  *  \brief Blackbox management in the background
  *  \author Weitzl Zoltan
  *  \version 1.0
  *	\date 2010. 07. 01.
 */
 uint16_t bbx_delay;

void bbx_background(void)
{
  switch (bbx.state)
  {
    case BBX_FORCE_START:
      if ((spi_sm == SPI_DNTG) || (spi_sm == SPI_OCC_BBX))
      {
        // init memory
        spi_cs_disable();
        spi_sm = SPI_OCC_BBX;
        spi_cs_assert(0);
        spi_init_mem();
        bbx.state = BBX_WT_WREN;
      }
      break;
    case BBX_WT_WREN:
      if (spi_init_mem_rdy())
      {
        // set up address
        spi_cs_disable();
        asm("nop;");
        spi_cs_assert(0);
        spi_wr_start(0);
        bbx.state = BBX_WT_ADDR;
      }
      break;
    case BBX_WT_ADDR:
      if (spi_wr_start_rdy())
      {
        bbx.idx = 0;
        spi_set16bit();
        bbx.state = BBX_WAIT4TRIGGER;
      }
      break;
    case BBX_FINISHED:
      spi_cs_disable();
      spi_set8bit();
      spi_sm = SPI_DNTG;
      bbx.state = BBX_ALL_FINISHED;
      break;
  }
}


