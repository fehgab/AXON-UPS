 /*! \file set_save.c
  *  \brief
  *  \author
  *  \version 2.0
  *  \date    2010-06-25
 */
#include "gen_types.h"
#include "Term_defs.h"

uint32_t c_date = 0x091120;         /*!< current date; read out from RTC by an other module */
uint32_t c_time = 0x175200;        /*!< current time; read out from RTC by an other module */
uint32_t s_date;                   /*!< variable used to set date by PAC/PC */
uint32_t s_time;                   /*!< variable used to set time by PAC/PC */
uint16_t set_datetm;               /*!< write it to 1 to set time, will became 100 if succesfull, 101 if unsuccesful */
uint16_t save_var;                 /*!< save variables to the EEPROM; will became 100 if succesfull, 101 if unsuccesful */
uint16_t load_defv;                /*!< load default values; will became 100 if succesfull, 101 if unsuccesful */

#define STDT_DNTG                  0  /*!< Do not go state of the set statemachine */
#define STDT_SET                   1  /*!< Needs to be set written to this value by the monitor */
#define STDT_SETSUCCESFUL          5  /*!< Succesfully set */
#define STDT_SETERROR              6  /*!< Set with an error */

extern uint8_t year, day, month;
extern uint8_t hour, min, sec;

extern uint16_t PVC_System_Year;
extern uint16_t PVC_System_Month;
extern uint16_t PVC_System_Day;
extern uint16_t Thet_napjai;
extern uint16_t PVC_System_Hour;
extern uint16_t PVC_System_Minute;
extern uint16_t PVC_System_Second;
extern void WrClock(void);

/*! Converts from uint32_t's pos part to uint8_t
 */
uint16_t convert_partBCD8_to_u8(uint32_t ui,uint8_t pos)
{
  uint8_t current_part = (ui >> (pos*8)) & 0xFF;
  return ((current_part & 0xF0) >> 4) * 10 + (current_part & 0xF);
}

  /*!
   *  \brief Set up the clock or write EEPROM by the Terminal software
   *  The terminal writes the variables to STDT_SET, the DSP tries to do the selected task (setting clock etc), if
   *  successfully set it goes to STDT_SETSUCCESFUL state, otherwise STDT_SETERROR state.
   *  \author Weitzl Zoltan
   *  \version 1.0
   *	\date 2010. 07. 01.
 */
void SetUpByTerminal()
{
  switch (set_datetm)
  {
    case STDT_SET:
      PVC_System_Year = 2000+ convert_partBCD8_to_u8(s_date,2);
      PVC_System_Month = convert_partBCD8_to_u8(s_date,1);
      PVC_System_Day = convert_partBCD8_to_u8(s_date,0);
      PVC_System_Hour = convert_partBCD8_to_u8(s_time,2);
      PVC_System_Minute = convert_partBCD8_to_u8(s_time,1);
      PVC_System_Second = convert_partBCD8_to_u8(s_time,0);
			WrClock();
      set_datetm = STDT_SETSUCCESFUL;
      break;
    case STDT_SETSUCCESFUL:
      break;
  }

  switch (save_var)
  {
    case STDT_SET:
      c_date = s_date;
      c_time = s_time;
      save_var = STDT_SETSUCCESFUL;
      break;
    case STDT_SETSUCCESFUL:
      break;
  }
}
