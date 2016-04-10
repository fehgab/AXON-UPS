#include "gen_types.h"
#include "HiMon.h"
#include "DSP2802x_Device.h"
#include "HiMon_hw.h"
#include "Term_defs.h"
#include "modbus_crc.inc"
#include "err_hyst.h"
#include "spi_mng.h"
#include "bbx.h"
#include "currentdt.h"

#define F_SAMP (2*3150)

const uint16_t f_samp = F_SAMP;    /*!< sampling frequency - should be in a spearate file */
const uint16_t mon_ver = 100;      /*!< monitor version */
const uint16_t en_fl_bbx = ENABLE_FLOAT_BBX; /*!< enable monitoring floating point variables in the recorder */
const uint16_t nr_tabs=MON_MAXTABS;/*!< maximum number of tabs */
const uint32_t Consy = RND_MNUMB_0;/*!< random number for consistency check */
const uint16_t dev_mode = 2;       /*!< if the value is non-zero password is not needed for the monitor */
const uint32_t len_ehist = EHIST_LEN; /*!< length of error history */
uint32_t serial_nr = 12345;

/* just for dbg */
extern float tmp_float;
extern uint16_t led_state[3];


/* extern variables */
extern uint32_t c_date;
extern uint32_t c_time;
extern int32_t s_date;
extern int32_t s_time;
extern int16_t set_datetm;
extern int16_t save_var;
extern int16_t load_defv;

/* max, min values */
const int32_t max_s_date = 0x991231, min_s_date = 0x90101;
const int32_t max_s_time = 0x235959, min_s_time = 0x0;
const int16_t max_set_datetm = 1, min_set_datetm = 0;
const int16_t max_save_var = 1, min_save_var = 0;
const int16_t max_load_defv = 1, min_load_defv = 0;

uint16_t len_error_descriptor,len_mainsm_descriptor;

const var_type vars_misc[] = {
  { "en_fl_bbx", UNIT_NONE,  TYP_U16,                     CA_U32(en_fl_bbx),  0,              0                } ,  /*!< enable floating point monitoring in blackbox for all DSP */
  { "bbox_str",  UNIT_NONE,  TYP_U16,                     CA_U32(bbx),        0,              0                } ,  /*!< address of bbx struct for all DSP */
  { "f_samp",    UNIT_NONE,  TYP_U16,                     CA_U32(f_samp),     0,              0                } ,  /*!< sampling frequency by bbx_trigger() for all DSP */
  { "Consy",     UNIT_NONE,  TYP_U32,                     CA_U32(Consy),      0,              0                } ,  /*!< for the consistency check for all DSP */
  { "date",      UNIT_NONE,  TYP_HEX|TYP_U32,             CA_U32(c_date),     0,              0                } ,  /*!< current date for all DSP */
  { "time",      UNIT_NONE,  TYP_HEX|TYP_U32,             CA_U32(c_time),     0,              0                } ,  /*!< current time for all DSP */
  { "s_date",    UNIT_NONE,  TYP_ENWR|TYP_32BIT,          CA_U32(s_date),     CA_U32(max_s_date),CA_U32(min_s_date)} ,  /*!< for date setup for all DSP */
  { "s_time",    UNIT_NONE,  TYP_ENWR|TYP_32BIT,          CA_U32(s_time),     CA_U32(max_s_time),CA_U32(min_s_time)} ,  /*!< for time setup for all DSP */
  { "set_dt",    UNIT_NONE,  TYP_ENWR|TYP_16BIT,          CA_U32(set_datetm), CA_U32(max_set_datetm),CA_U32(min_set_datetm)} ,  /*!< for date,time setup for all DSP */
  { "save_var",  UNIT_NONE,  TYP_ENWR|TYP_16BIT,          CA_U32(save_var),   CA_U32(max_save_var),CA_U32(min_save_var)} ,  /*!< save variables for all DSP */
  { "load_defv", UNIT_NONE,  TYP_ENWR|TYP_16BIT,          CA_U32(load_defv),  CA_U32(max_load_defv),CA_U32(min_load_defv)} ,  /*!< load default variable values for all DSP */
  { "serial",    UNIT_NONE,  TYP_U32,                     CA_U32(serial_nr),  0,              0                } ,  /*!< serial number for the pwd generation */
  { "dev_mode",  UNIT_NONE,  TYP_U16,                     CA_U32(dev_mode),   0,              0                } ,  /*!< development mode, if the value is non-zero the Monitor goes immediatelly into dev mode */
  { "mon_ver",   UNIT_NONE,  TYP_U16,                     CA_U32(mon_ver),    0,              0                } ,  /*'< monitor version */
  { "l_ehist",   UNIT_NONE,  TYP_U32,                     CA_U32(len_ehist),  0,              0                } ,  /*!< length of error history in entries */
  { "l_errdesc", UNIT_NONE,  TYP_U16,                     CA_U32(len_error_descriptor),0,              0                } ,  /*'< length of the error texts in characters */
  { "l_msmdesc", UNIT_NONE,  TYP_U16,                     CA_U32(len_mainsm_descriptor),0,              0                } ,  /*'< length of the mainsm texts in characters */
  { "errdesc",   UNIT_NONE,  TYP_U16,                     CA_U32(error_descriptor),0,              0                } ,  /*'< used to get the pointer to the error history text */
  { "msmdesc",   UNIT_NONE,  TYP_U16,                     CA_U32(mainsm_descriptor),0,              0                } ,  /*'< used to get the pointer to the mainsm  text */
};


/*! Titles of the disserent tabs: the 0th one is the Tab name, since it is not a real tab, but only used for the HiTerm.
*/
const uint8_t tabtitle[MON_MAXTABS][MAXCH_TABNAME+1] = { {"DSPname"}};
/*! Pointers for the tab descriptors. Used for all DSPs.*/
const var_type* tabptr[MON_MAXTABS] = {vars_misc};
/*! Size of the tab descriptors, calculated in run time.*/
uint16_t tabsize[MON_MAXTABS];

const uint8_t units[][MAXCH_UNIT+1] = { {""},
                                        {"A"},
                                        {"V"},
                                        {"kVA"},
                                        {"kW"},
                                        {"s"},
                                        {"hour"},
                                        {"Nm"},
                                        {"kWh"},
                                        {"Hz"},
                                        {"Vs"},
                                        };
const desc_type error_descriptor[LEN_ERR_DESC] = {
  {0   , "No error" },
  {1   , "Overcurrent R" },
  {2   , "Overcurrent S" },
  {3   , "Overcurrent T" },
  {4   , "Overv DC-link" },
  {5   , "Overvoltage R" },
  {6   , "Overvoltage S" },
  {7   , "Overvoltage T" },
  {8   , "IGBT sat prot R" },
  {9   , "IGBT sat prot S" },
  {10  , "IGBT sat prot T" },
  {11  , "Overload R" },
  {12  , "Overload S" },
  {13  , "Overload T" },
  {44  , "Unknown error" },
};

const desc_type mainsm_descriptor[LEN_MAINSM_DESC] = {
  {0   , "Stopped" },
  {1   , "Starting" },
  {2   , "Operational" },
  {3   , "Waiting" },
  {99  , "Crazy state" },
};

void InitTermVars()
{
tabsize[0] = sizeof(vars_misc);

len_error_descriptor = sizeof(error_descriptor);
len_mainsm_descriptor = sizeof(mainsm_descriptor);

}


uint16_t spi_sm = SPI_DNTG;
uint8_t TransmitEnable = 1;
/*! \file HiMon.c
 *  \brief UART communication interpreter (PAC/PC <=> DSPs) and processing module.
 *  \author Bilau Zoltan Tamas
 *  \version 1.0
 *  \date    2010-06-11
 *
 *  <B>Physical layer:</B> RS485, baudrate selectable via BAUD_RATE, 1 start bit, 1 stop bit. \n\n
 *
 *  <B>Application layer:</B>
 *  Based on the modified TMSmon communication. STX,ETX,DLE. The communication is controlled (initiated by the PAC/PC), the DSPs
 *  only answers to the slaves. \n
 *  The message begins with STX character, ends with ETX character.
 *  In case the DSP receives STX character we are sure that the message begins (unless it is a noisy data),
 *  in case it receives ETX character it is the end of DSP.
 *  If the message contains ETX/STX/DLE character instead of that character the following characters are transmitted:
 *    - STX: [DLE] [STX^XDAT]
 *    - ETX: [DLE] [ETX^XDAT]
 *    - DLE: [DLE] [DLE^XDAT]
 *
 *  The higher level communication without the STX,ETX and DLE escaping is as following.
 *    - [DSP address] [mode] [key] [length16] [Message data] [Crc 16] where
 *      - [DSP address] : 1 byte long 0-127 if data is transmitted by the PAC/PC; highest bit is 1 if it is transmitted by the DSP
 *      - [mode]        : 1 byte long mode select
 *          -# MON_RD   : variables read by the PAC/PC
 *              - [Message data] is as following
 *                   - PAC => DSP : X* [Address32][len8]
 *                   - DSP => PAC : X* [Address32][len8][data: 2*len8 byte MSBF]
 *          -# MON_WR   : variables write by the PAC/PC
 *              - [Message data] is as following
 *                   - PAC => DSP : X* [Address32][len8][data: 2*len8 byte MSBF]
 *                   - DSP => PAC : empty
 *          -# MON_TABS(i) : read the tab contect
 *              - [Message data]
 *                   - PAC => DSP : empty
 *                   - DSP => PAC : N* var_type where N is the number of variables in the current tab
 *          -# MON_TABNAMES: read tab names
 *              - [Message data]
 *                   - PAC => DSP : empty
 *                   - DSP => PAC : N* tabname_type where N is the number of variables in the current tab
 *          -# MON_TABVALUES(i) : read the values of the tabs  <B> (not implemented) </B>
 *              - [Message data]
 *                   - PAC => DSP : empty
 *                   - DSP => PAC : N* Data32 where N is the number of variables in the current tab, 16 bit variable is also transmitted as 32 bit
 *          -# MON_RDUNITS:
 *              - [Message data]
 *                   - PAC => DSP : empty
 *                   - DSP => PAC : N* MAXCH_UNIT+1 where N is the number of definied units
 *          -# MON_RD_ERRH:
 *              - [Message data]
 *                   - PAC => DSP : empty
 *                   - DSP => PAC : ERRHYST_LEN * 8 byte length data consisting the error history in the SPI memory/flash/DSP memory
 *          -# MON_RD_BBX:
 *              - [Message data]
 *                   - PAC => DSP : [address32][length16] address (16 bit manner) of the requested bbx data (begins with 0x00); 2*length16 byte data length req.ed
 *                   - DSP => PAC : [address32][length16][data field] same as above, but with data field MSBF data
 *          -# MON_RD_SPI_RAM
 *              - [Message data]
 *                   - PAC => DSP : [address32][length16] where address32's MSBF2 is the SPI device's address
 *                   - DSP => PAC : [address32][length16][data field] same as above, but with data field MSBF data [length16]-in 16bit manner
 *          -# MON_PING
 *              - [Message data]
 *                   - PAC => DSP : empty
 *                   - DSP => PAC : 0 - application firmware running (this firmware)
 *                                : 1 - boot running checksum OK
 *                                : 2 - boot running checksum NOK
 *      - [key]        : 1 byte long key, the slave should send the answer with the same key
 *      - [length16]   : 2 byte long length of the Message data field MSBF (length in bytes)
 *      - [Message data]: definied above for the different modes
 *      - [Crc 16]     : 16 bit CRC as definied in modbus RTU (LSBF)  http://www.modbus.org/specs.php
 *
 *  What to add to a certain project in order to work with HiTerm
 *      - add .c files
 *         # HiMon.c (as source)
 *         # Term_defs.c (it will be included by HiMon.c, so it does not need to be built) (generated by Excel)
 *         # add SetSave.c (for Saveing the NV variables, setting up RTC)
 *         # for debuging purposes err_hyst.c
 *         # bbx.c
 *      - run from main loop
 *         # Serial_mng()
 *         # SetUpByTerminal()
 *         # bbx_background()
 *         # ErrorHistoryPushBckGrnd()
 *      - run as initializers:
 *         # InitTermVars()
 *         # InitErrHyst_dbg()
 *         # ReadOutErrorHistoryIdx()
 *      - us as .h file
 *         # HiMon_hw.h
 *         # HiMon.h
 *         # bbx.h
 *         # Term_defs.h
 *      - modify HiMon_hw.h for the definied hardware
 */

#ifndef USE_HWFIFO

  uint8_t swt_fifo[SW_FIFO_LEN];
  uint8_t swt_fifo_wr = 0;
  uint8_t swt_fifo_rd = 0;

#endif

uint16_t uart_data; /*!< current received data through UART */
uint16_t uart_state = SER_BASE; /*!< state of the parser and message transmitter's FSM */
uint32_t uart_cmd[UART_CMD_LEN];/*!< received data through uart without escaping, and ETX, STX characters in 32 bit \a uart_cmd[0] MSB is the first received data */
uint32_t uart_tmt_addr; /*!< cirrent address of the data which needs to be trasmitted as an answer through UART*/
uint16_t uart_pos; /*!< the next data of from PAC/PC should come to this index (8 bit), can be handles as length after parsing with CRC*/
uint16_t uart_dlewas; /*!< used by the parser to determine whether the last character was a DLE message or not */
uint16_t uart_crc; /*!< CRC for the receive data till uart_pos */
uint16_t uart_tcrc; /*!< CRC for the transmit data */
uint16_t uart_tmt_pos; /*!< transmit data position in sections*/
uint16_t uart_tmt_len; /*!< transmit data length in sections*/
uint16_t uart_tmt_apos; /*!< transmit data position in the current section*/
uint16_t uart_tmt_alen; /*!< transmit data length for the current section*/
uint16_t *pRaw; /*!< pointer to the raw sending data */
const var_type* pTab; /*< pointer to the next readable Tab descriptor */
uint32_t tmp32;
uint16_t tmp16;
uint32_t uart_bbx_1st_data; /*!< reading out data from bbx, offset address */

uint8_t uart_address = 0; /*!< DSP address */


/*!
 * This function calculates the checksum specified for the PAC/PC <=> DSP communication.
 * \param *chksum is a pointer for a 16 bit checksum, it needs to have the previously calculated checksums \n
 * \param data is the nth data which's chksum needs to be calculated together with the previous ones.
 */


void PushCheckSum(uint16_t *chksum, uint8_t data)
{
  uint8_t uchCRCHi = *chksum >> 8;   /* high byte of CRC initialized   */
  uint8_t uchCRCLo = *chksum & 0xFF; /* low byte of CRC initialized   */
  uint8_t uIndex; /* will index into CRC lookup table   */

  uIndex = uchCRCLo ^ data; /* calculate the CRC   */
  uchCRCLo = uchCRCHi ^ auchCRCHi[uIndex];
  uchCRCHi = auchCRCLo[uIndex];

  *chksum = (uchCRCHi << 8 | uchCRCLo);
}

/*!
 * This function adds the received data into a FIFO  uart_cmd. Since the processor does not support (only with memory wastage)
 * 8 bit long variables, 32 bit long array is used for FIFO. To simplify the code this short function is used to push the data into FIFO.
 * The function also calculates the CRC, it checks if the FIFO is full or not.
 * \param data data to be pushed into the FIFO
 * \return 0 succesful \n
 *         1 unsuccesful.
 */
inline uint16_t UartRec_Add_Char(uint8_t data)
{
  uart_cmd[uart_pos / 4] = (uart_cmd[uart_pos / 4] & (~(((uint32_t)(0x00FF)) << 8*(3 - (uart_pos % 4))))) | (((uint32_t)(data)) << 8*(3 - (uart_pos % 4)));
  PushCheckSum(&uart_crc,data);
  if (++uart_pos >= 4*UART_CMD_LEN)
   return 1;
  else
   return 0;
}

/*!
 * This function gets 8 bit data from the FIFO \a nth element is the return parameter.
 * \param uart_cmd pointer to the FIFO
 * \param nth nth data needs to be calculated
 * \return the nth 8 bit data
 */
inline uint8_t UartGetNthU8(uint32_t *uart_cmd,uint16_t nth)
{
  return (uart_cmd[nth / 4] >> (8*(3 - (nth % 4)))) & 0xFF;
}

/*!
 * This function gets 16 bit data from the FIFO \a nth element is the return parameter. (MSBF)
 * \param uart_cmd pointer to the FIFO
 * \param nth nth data needs to be calculated (\a nth specifies in 8 bit manner)
 * \return the nth 16 bit data
 */
inline uint16_t UartGetNthU16(uint32_t *uart_cmd,uint16_t nth)
{
  // MSB first
  return ((uint16_t)(UartGetNthU8(uart_cmd,nth)) << 8) + UartGetNthU8(uart_cmd,nth+1);
}

/*!
 * This function gets 32 bit data from the FIFO \a nth element is the return parameter. (MSBF)
 * \param uart_cmd pointer to the FIFO
 * \param nth nth data needs to be calculated (\a nth specifies in 8 bit manner)
 * \return nth 16 bit data
 */
inline uint32_t UartGetNthU32(uint32_t *uart_cmd,uint16_t nth)
{
  // MSB first
  return (uint32_t)(UartGetNthU16(uart_cmd,nth+2)) + (((uint32_t)UartGetNthU16(uart_cmd,nth)) << 16);
}

#if 0
/*!
 * This function gets 24 bit data from the FIFO \a nth element is the return parameter. (MSBF)
 * \param uart_cmd pointer to the FIFO
 * \param nth nth data needs to be calculated (\a nth specifies in 8 bit manner)
 * \return nth 24 bit data
 */
inline uint32_t UartGetNthU24(uint32_t *uart_cmd,uint16_t nth)
{
  // MSB first
  return (uint32_t)(UartGetNthU16(uart_cmd,nth+1)) + (((uint32_t)UartGetNthU8(uart_cmd,nth)) << 16);
}
#endif

/*!
 * This function transmits the character, and calculates the 16bit checksum, keeping in mind the escaping (DLE substitution).
 * \param[in] chksum pointer to the checksum
 * \param[in] dat 8 bit data to be sent through UART
 */
inline uart_tmt_datadle(uint16_t *chksum,uint8_t dat)
{
  switch (dat & 0xFF)
  {
    case STX:
    case ETX:
    case DLE:
     uart_tmt_data(DLE);
     uart_tmt_data(dat ^ XDT);
     break;
    default:
     uart_tmt_data(dat);
     break;

  }
  PushCheckSum(chksum,dat & 0xFF);
}

uint16_t msg_len,msg_pos; /*!< variable writing: the length of the message, and the current position of the processing*/
uint32_t caddr; /*!< variable writing: the base address for the current section*/
uint8_t clen; /*!< variable writing: the length (16 bits) of data for the current section*/
uint32_t ccaddr; /*!< variable writing: current address*/
uint16_t ccdat16; /*!< variable writing: address for 16 bit writing*/
uint32_t ccdat32; /*!< variable writing: address for 32 bit writing*/
const uint8_t* pU16rd;  /*!< pointer to the 16 bit transmit as 8 bit variables */

/*!
 * The function writes the data into the memory according to the specification from the input receive fifo \a uart_cmd.\n
 * The function tries to write data in 32bit format (by taking care of the adresses). It also disables the interrupt
 * when writing straight to the memory.
 * \param write_it (not yet implemented) according to the plan if it is 0 it only checks if it can be written (good framing etc)
 * \return 0 succesful \n
 *         1 unsuccesful (not yet implemented)
 */
uint16_t TryToWriteValues(uint16_t write_it)
{
  int ii;
  msg_len = UartGetNthU16(uart_cmd,3);
  msg_pos = 0;


    while (msg_len > msg_pos)
    {
      caddr = UartGetNthU32(uart_cmd,msg_pos+5);
      clen = UartGetNthU8(uart_cmd,msg_pos + 5 + 4);
      for (ii=0; ii < clen; ii++)
      {
        if ((caddr + ii) & 0x01)
        {
          // odd address (cannot be 32 bit long variable)
          ccaddr = caddr + ii;
          ccdat16 = UartGetNthU16(uart_cmd,msg_pos+5 + 5 + ii*2);
          *(uint16_t*)(ccaddr) = ccdat16;
          ii++;
        }
        else
        {
          // even address
          ccaddr = caddr + ii;
          ccdat16 = UartGetNthU16(uart_cmd,msg_pos+5 + 5 + ii*2);
          ccdat32 = UartGetNthU32(uart_cmd,msg_pos+5 + 5 + ii*2);
          if (ii != clen - 1)
          {
            DINT;
            // not the last so can be 32 bit long variable
            *(uint32_t*)(ccaddr) = ccdat32;
            EINT;
            ii++;
          }
          else
          {
            *(uint16_t*)(ccaddr) = ccdat16;
          }
        }
      }
      msg_pos += 5 + clen*2;

    }
    return 0;

}


uint8_t tmt_dat = 'a';
/*!
 * This function is called when a good framed message arrived. The input FIFO \a uart_cmd contains
 * the raw message (without escaping, CRC, ETX,STX framing characters.
 * \return the state where \a uart_state needs to be forced.
 */
uint8_t Serial_cmd_mng(void)
{
  uint8_t  mode = UartGetNthU8(uart_cmd,1);
  uint16_t len = UartGetNthU16(uart_cmd,3);
  uint16_t answ_len;
  uint16_t i;

  if (MON_RD == mode) /*! Read command arrived */
  {
    if (((len % 5) != 0))// || (len == 0))
     return SER_BASE;

    answ_len = len;
    for (i=0; i < len / 5; i++)  /* Calculates the nr of bytes to be transmitted as an answer by the DSP */
      answ_len += 2*(uint16_t)UartGetNthU8(uart_cmd,5 + 4 + i*5);

    uart_cmd[0] |= 0x80000000;
    uart_tcrc = 0xFFFF;
    uart_tmt_data(STX);  /* transmit the necessary characters for the framing: STX, Address, Mode, Key, Length16 */
    uart_tmt_datadle(&uart_tcrc,UartGetNthU8(uart_cmd,0));
    uart_tmt_datadle(&uart_tcrc,UartGetNthU8(uart_cmd,1));
    uart_tmt_datadle(&uart_tcrc,UartGetNthU8(uart_cmd,2));
    uart_tmt_datadle(&uart_tcrc,(answ_len >> 8) & 0xFF);
    uart_tmt_datadle(&uart_tcrc,answ_len & 0xFF);
    uart_tmt_pos = 0;
    uart_tmt_len = len / 5;

    if (0 != len)
      return SER_ANS_RDB;
    tmp16 = uart_tcrc;
    uart_tmt_datadle(&uart_tcrc,(tmp16 >> 0) & 0xFF);
    uart_tmt_datadle(&uart_tcrc,(tmp16 >> 8) & 0xFF);
    uart_tmt_data(ETX);
    return SER_BASE;
  }
  else if (MON_PING == mode)
  {
    uart_cmd[0] |= 0x80000000;
    uart_tcrc = 0xFFFF;
    uart_tmt_data(STX);  /* transmit the necessary characters for the framing: STX, Address, Mode, Key, Length16 */
    uart_tmt_datadle(&uart_tcrc,UartGetNthU8(uart_cmd,0));
    uart_tmt_datadle(&uart_tcrc,UartGetNthU8(uart_cmd,1));
    uart_tmt_datadle(&uart_tcrc,UartGetNthU8(uart_cmd,2));
    uart_tmt_datadle(&uart_tcrc,0); // length of message high byte
    uart_tmt_datadle(&uart_tcrc,1); // length of message low byte
    uart_tmt_datadle(&uart_tcrc,0); // 0 - application fw. is running
    tmp16 = uart_tcrc;
    uart_tmt_datadle(&uart_tcrc,(tmp16 >> 0) & 0xFF);
    uart_tmt_datadle(&uart_tcrc,(tmp16 >> 8) & 0xFF);
    uart_tmt_data(ETX);
  }
  else if (MON_WR == mode)
  {
    TryToWriteValues(1);  // TODO: implement try function
    uart_cmd[0] |= 0x80000000;
    uart_tcrc = 0xFFFF;
    uart_tmt_data(STX);
    uart_tmt_datadle(&uart_tcrc,UartGetNthU8(uart_cmd,0));
    uart_tmt_datadle(&uart_tcrc,UartGetNthU8(uart_cmd,1));
    uart_tmt_datadle(&uart_tcrc,UartGetNthU8(uart_cmd,2));
    uart_tmt_datadle(&uart_tcrc,0);
    uart_tmt_datadle(&uart_tcrc,0);
    tmp32 = uart_tcrc;
    uart_tmt_datadle(&uart_tcrc,tmp32 & 0xFF);
    uart_tmt_datadle(&uart_tcrc,(tmp32 >> 8) & 0xFF);
    uart_tmt_data(ETX);

    return SER_BASE;
  }
  else if ((MON_TABS(0) <= mode) && (MON_TABS(MON_MAXTABS) > mode))
  {
    uart_cmd[0] |= 0x80000000;
    uart_tcrc = 0xFFFF;
    uart_tmt_data(STX);
    uart_tmt_datadle(&uart_tcrc,UartGetNthU8(uart_cmd,0));
    uart_tmt_datadle(&uart_tcrc,UartGetNthU8(uart_cmd,1));
    uart_tmt_datadle(&uart_tcrc,UartGetNthU8(uart_cmd,2));
    len = tabsize[mode - MON_TABS(0)] * 2;
    uart_tmt_datadle(&uart_tcrc,len >> 8);
    uart_tmt_datadle(&uart_tcrc,len & 0xFF);

    uart_tmt_pos = 0;
    uart_tmt_len = tabsize[mode - MON_TABS(0)];
    pTab = tabptr[mode - MON_TABS(0)];

    return SER_RDTABS;
  }
  else if ((MON_TABVALUES(0) <= mode) && (MON_TABVALUES(MON_MAXTABS) > mode))
  {
    uart_cmd[0] |= 0x80000000;
    uart_tcrc = 0xFFFF;
    uart_tmt_data(STX);
    uart_tmt_datadle(&uart_tcrc,UartGetNthU8(uart_cmd,0));
    uart_tmt_datadle(&uart_tcrc,UartGetNthU8(uart_cmd,1));
    uart_tmt_datadle(&uart_tcrc,UartGetNthU8(uart_cmd,2));
    len = 4 * tabsize[mode - MON_TABVALUES(0)] / sizeof(var_type);
    uart_tmt_datadle(&uart_tcrc,len >> 8);
    uart_tmt_datadle(&uart_tcrc,len & 0xFF);

    uart_tmt_pos = 0;
    uart_tmt_len = len / 4;
    pTab = tabptr[mode - MON_TABVALUES(0)];

    return SER_RDTABVALUES;
  }
  else if (MON_TABNAMES == mode)
  {
    uart_cmd[0] |= 0x80000000;
    uart_tcrc = 0xFFFF;
    uart_tmt_data(STX);
    uart_tmt_datadle(&uart_tcrc,UartGetNthU8(uart_cmd,0));
    uart_tmt_datadle(&uart_tcrc,UartGetNthU8(uart_cmd,1));
    uart_tmt_datadle(&uart_tcrc,UartGetNthU8(uart_cmd,2));
    len = sizeof(tabtitle); // will be transmitted char by char
    uart_tmt_datadle(&uart_tcrc,len >> 8);
    uart_tmt_datadle(&uart_tcrc,len & 0xFF);

    uart_tmt_pos = 0;
    uart_tmt_len = sizeof(tabtitle);
    pU16rd = tabtitle[0];

    return SER_RDU16_ASTEXT;
  }
  else if (MON_RDUNITS == mode)
  {
    uart_cmd[0] |= 0x80000000;
    uart_tcrc = 0xFFFF;
    uart_tmt_data(STX);
    uart_tmt_datadle(&uart_tcrc,UartGetNthU8(uart_cmd,0));
    uart_tmt_datadle(&uart_tcrc,UartGetNthU8(uart_cmd,1));
    uart_tmt_datadle(&uart_tcrc,UartGetNthU8(uart_cmd,2));
    len = sizeof(units); // will be transmitted char by char
    uart_tmt_datadle(&uart_tcrc,len >> 8);
    uart_tmt_datadle(&uart_tcrc,len & 0xFF);

    uart_tmt_pos = 0;
    uart_tmt_len = sizeof(units);
    pU16rd = units[0];

    return SER_RDU16_ASTEXT;
  }
  else if (MON_RD_ERRH == mode)
  {
    uart_cmd[0] |= 0x80000000;
    uart_tcrc = 0xFFFF;
    uart_tmt_data(STX);
    uart_tmt_datadle(&uart_tcrc,UartGetNthU8(uart_cmd,0));
    uart_tmt_datadle(&uart_tcrc,UartGetNthU8(uart_cmd,1));
    uart_tmt_datadle(&uart_tcrc,UartGetNthU8(uart_cmd,2));
    len = ERRHYST_LEN*8; // will be transmitted byte/byte
    uart_tmt_datadle(&uart_tcrc,len >> 8);
    uart_tmt_datadle(&uart_tcrc,len & 0xFF);

    uart_tmt_pos = 0;
    uart_tmt_len = ERRHYST_LEN*4;  // time 16 bit
    pRaw = (uint16_t*)err_hyst;

    return SER_RDRAW;
  }
  else if (MON_RD_BBX == mode)
  {
    uart_cmd[0] |= 0x80000000;
    uart_tcrc = 0xFFFF;
    uart_tmt_data(STX);
    uart_tmt_datadle(&uart_tcrc,UartGetNthU8(uart_cmd,0)); // DSP nr
    uart_tmt_datadle(&uart_tcrc,UartGetNthU8(uart_cmd,1)); // Mode
    uart_tmt_datadle(&uart_tcrc,UartGetNthU8(uart_cmd,2)); // Key
    uart_tmt_len = (UartGetNthU8(uart_cmd,9) << 8) + UartGetNthU8(uart_cmd,10);
    len = uart_tmt_len*2 + 4 + 2;
    uart_tmt_datadle(&uart_tcrc,len >> 8);
    uart_tmt_datadle(&uart_tcrc,len & 0xFF);
    uart_bbx_1st_data = (((uint32_t)UartGetNthU16(uart_cmd,5)) << 16) + (uint32_t)UartGetNthU16(uart_cmd,7);
    uart_tmt_datadle(&uart_tcrc,uart_bbx_1st_data >> 24);
    uart_tmt_datadle(&uart_tcrc,uart_bbx_1st_data >> 16);
    uart_tmt_datadle(&uart_tcrc,uart_bbx_1st_data >> 8);
    uart_tmt_datadle(&uart_tcrc,uart_bbx_1st_data);
    uart_bbx_1st_data &= SPI_ADDRESS_MASK;
    uart_tmt_datadle(&uart_tcrc,uart_tmt_len >> 8);
    uart_tmt_datadle(&uart_tcrc,uart_tmt_len);

    uart_tmt_pos = 0;

    return SER_RD_BBX;
  }
  else if (MON_RD_SPI_RAM == mode)
  {
    uart_cmd[0] |= 0x80000000;
    uart_tcrc = 0xFFFF;
    uart_tmt_data(STX);
    uart_tmt_datadle(&uart_tcrc,UartGetNthU8(uart_cmd,0)); // DSP nr
    uart_tmt_datadle(&uart_tcrc,UartGetNthU8(uart_cmd,1)); // Mode
    uart_tmt_datadle(&uart_tcrc,UartGetNthU8(uart_cmd,2)); // Key
    if (spi_sm == SPI_DNTG)
    {
      spi_sm = SPI_OCC_HIMON; // spi is used by himon
      spi_cs_disable();
      uart_tmt_len = (UartGetNthU8(uart_cmd,9) << 8) + UartGetNthU8(uart_cmd,10);
      len = uart_tmt_len*2 + 4 + 2;
      uart_tmt_datadle(&uart_tcrc,len >> 8);
      uart_tmt_datadle(&uart_tcrc,len & 0xFF);
      uart_bbx_1st_data = (((uint32_t)UartGetNthU16(uart_cmd,5)) << 16) + (uint32_t)UartGetNthU16(uart_cmd,7);
      uart_tmt_datadle(&uart_tcrc,uart_bbx_1st_data >> 24); // address
      uart_tmt_datadle(&uart_tcrc,uart_bbx_1st_data >> 16);
      uart_tmt_datadle(&uart_tcrc,uart_bbx_1st_data >> 8);
      uart_tmt_datadle(&uart_tcrc,uart_bbx_1st_data);
      spi_cs_assert(uart_bbx_1st_data >> SPI_ADD_CS_SHIFT);
      uart_bbx_1st_data &= SPI_ADDRESS_MASK;
      spi_rd_start(uart_bbx_1st_data);

      uart_tmt_datadle(&uart_tcrc,uart_tmt_len >> 8); // length
      uart_tmt_datadle(&uart_tcrc,uart_tmt_len);
      uart_tmt_pos = 0;
      return SER_RD_SPI_START;
    }
    else
    {
      uart_tmt_datadle(&uart_tcrc,0); // length16
      uart_tmt_datadle(&uart_tcrc,0);
      tmp32 = uart_tcrc;
      uart_tmt_datadle(&uart_tcrc,tmp32 & 0xFF);
      uart_tmt_datadle(&uart_tcrc,(tmp32 >> 8) & 0xFF);
      uart_tmt_data(ETX);
      return SER_BASE;
    }


  }
  else if (MON_REBOOT == mode)
  {
		EALLOW;
    SysCtrlRegs.WDCR=0;
    EDIS;
    return SER_BASE;
  }

  return SER_BASE;
}

/*!
 *  \brief  * This function is responsible for parsing the data arrived from PAC/PC.
 * It places the parsed and DLE freed message into \a uart_cmd.
 * If the message is OK and the address is this DSP's address it calles the function
 * \a Serial_cmd_mng which forces the \a uart_state into a mode, which may transmit
 * the necessary data to uart. This is used to minimize the the needed memory resources
 * for sending data. So no software FIFO is used for transmitting only the built in
 * FIFO for the SCI periphery.
 *  \author Weitzl Zoltan
 *  \version 1.0
 *	\date 2010. 07. 01.
 */
void Serial_mng(void)
{
  if (RxERR)
  {
    RxSWRESET = 0;
    asm("nop;");
    RxSWRESET = 1;
  }

  #if SIMPLE_ECHO == 0
      switch (uart_state)
      {
        case SER_BASE:
          if (UART_DATA_ARRIVED)
          {
            uart_data = UART_REC_DATA;
            if (uart_data == STX)
            {
              uart_crc = 0xFFFF;
              uart_dlewas = uart_pos = 0;
              uart_state = SER_RECING;
            }
          }
          break;
        case SER_RECING:
          if (UART_DATA_ARRIVED)
          {
            uart_data = UART_REC_DATA;
            if (uart_dlewas)
            {
              if (UartRec_Add_Char(uart_data ^ XDT))
               uart_state = SER_BASE;
              uart_dlewas = 0;
            }
            else
            {
              if (uart_data == DLE)
              {
                uart_dlewas = 1;
              }
              else
              {
                if (uart_data == ETX)
                {

                  if ((uart_crc == 0) && (UartGetNthU16(uart_cmd,3) + 7 == uart_pos)) // good CRC, length
                  {
                    #if SIMULATE_MORE_DSPS == 0
                      if (UartGetNthU8(uart_cmd,0) == uart_address)
                    #else
                      //if ((UartGetNthU8(uart_cmd,0) == 1) || (UartGetNthU8(uart_cmd,0) == 2))
                    #endif
                        uart_state = Serial_cmd_mng();
                  }
                  else
                  {
                    uart_state = SER_BASE;
                  }
                }
                else
                {
                  if (UartRec_Add_Char(uart_data))
                    uart_state = SER_BASE;
                }
              }
            }
          }
          break;

        case SER_ANS_RDB:
          if (UART_TMT_NOEMPTY() > 10)
          {
            uart_tmt_addr = UartGetNthU32(uart_cmd,5 + 5*uart_tmt_pos);
            uart_tmt_alen = UartGetNthU8(uart_cmd,5 + 4 + 5*uart_tmt_pos);
            uart_tmt_apos = 0;
            uart_tmt_datadle(&uart_tcrc,(uart_tmt_addr >> 24) & 0xFF);  // addr
            uart_tmt_datadle(&uart_tcrc,(uart_tmt_addr >> 16) & 0xFF);
            uart_tmt_datadle(&uart_tcrc,(uart_tmt_addr >> 8) & 0xFF);
            uart_tmt_datadle(&uart_tcrc,uart_tmt_addr & 0xFF);
            uart_tmt_datadle(&uart_tcrc,uart_tmt_alen);  // len

            uart_state = SER_ANS_RDDAT;
          }
          break;
        case SER_ANS_RDDAT:
          if (UART_TMT_NOEMPTY() > 10)
          {
            if (uart_tmt_apos >= uart_tmt_alen)
            {
              if (++uart_tmt_pos >= uart_tmt_len)
              {
                // send CRC, ETX
                tmp16 = uart_tcrc;
                uart_tmt_datadle(&uart_tcrc,(tmp16 >> 0) & 0xFF);
                uart_tmt_datadle(&uart_tcrc,(tmp16 >> 8) & 0xFF);
                uart_tmt_data(ETX);
                uart_state = SER_BASE;
              }
              else
              {
                uart_state = SER_ANS_RDB;
              }
            }
            else if (uart_tmt_alen - uart_tmt_apos >= 2)
            {
              DINT;
              tmp32 = *(uint32_t*)(uart_tmt_addr + uart_tmt_apos);
              EINT;
              uart_tmt_datadle(&uart_tcrc,(tmp32 >> 24) & 0xFF);  // data
              uart_tmt_datadle(&uart_tcrc,(tmp32 >> 16) & 0xFF);
              uart_tmt_datadle(&uart_tcrc,(tmp32 >>  8) & 0xFF);
              uart_tmt_datadle(&uart_tcrc,(tmp32 >>  0) & 0xFF);
              uart_tmt_apos += 2;
            }
            else
            {
              tmp32 = *(uint16_t*)(uart_tmt_addr + uart_tmt_apos);
              uart_tmt_datadle(&uart_tcrc,(tmp32 >>  8) & 0xFF);
              uart_tmt_datadle(&uart_tcrc,(tmp32 >>  0) & 0xFF);
              uart_tmt_apos++;
            }
          }
          break;
        case SER_RDTABS:
          if (UART_TMT_NOEMPTY() > 10)
          {
            if (uart_tmt_pos < uart_tmt_len)
            {
              uart_tmt_datadle(&uart_tcrc,((uint16_t*)(pTab))[uart_tmt_pos] >> 8);
              uart_tmt_datadle(&uart_tcrc,((uint16_t*)(pTab))[uart_tmt_pos] &  0xFF);
              uart_tmt_pos++;
            }
            else
            {
              tmp16 = uart_tcrc;
              uart_tmt_datadle(&uart_tcrc,(tmp16 >> 0) & 0xFF);
              uart_tmt_datadle(&uart_tcrc,(tmp16 >> 8) & 0xFF);
              uart_tmt_data(ETX);
              uart_state = SER_BASE;
            }

          }
          break;
        case SER_RDRAW:
          if (UART_TMT_NOEMPTY() > 10)
          {
            if (uart_tmt_pos < uart_tmt_len)
            {
              uart_tmt_datadle(&uart_tcrc,((uint16_t*)(pRaw))[uart_tmt_pos] >> 8);
              uart_tmt_datadle(&uart_tcrc,((uint16_t*)(pRaw))[uart_tmt_pos] &  0xFF);
              uart_tmt_pos++;
            }
            else
            {
              tmp16 = uart_tcrc;
              uart_tmt_datadle(&uart_tcrc,(tmp16 >> 0) & 0xFF);
              uart_tmt_datadle(&uart_tcrc,(tmp16 >> 8) & 0xFF);
              uart_tmt_data(ETX);
              uart_state = SER_BASE;
            }

          }
          break;
        case SER_RD_BBX:
          if (UART_TMT_NOEMPTY() > 10)
          {
            if (uart_tmt_pos < uart_tmt_len)
            {
              uart_tmt_datadle(&uart_tcrc,((uint16_t*)(bbox))[(uart_bbx_1st_data + uart_tmt_pos) % (BBOX_LEN * 8)] >> 8);
              uart_tmt_datadle(&uart_tcrc,((uint16_t*)(bbox))[(uart_bbx_1st_data + uart_tmt_pos) % (BBOX_LEN * 8)] &  0xFF);
              uart_tmt_pos++;
            }
            else
            {
              tmp16 = uart_tcrc;
              uart_tmt_datadle(&uart_tcrc,(tmp16 >> 0) & 0xFF);
              uart_tmt_datadle(&uart_tcrc,(tmp16 >> 8) & 0xFF);
              uart_tmt_data(ETX);
              uart_state = SER_BASE;
            }

          }
          break;
        case SER_RDTABVALUES:
          if (UART_TMT_NOEMPTY() > 10)
          {
            if (uart_tmt_pos < uart_tmt_len)
            {
              if ((pTab[uart_tmt_pos].type & TYP_BIT_MASK) == TYP_32BIT)
               tmp32 = *(uint32_t*)(pTab[uart_tmt_pos].ptr);
              else
               tmp32 = *(uint16_t*)(pTab[uart_tmt_pos].ptr);
              uart_tmt_datadle(&uart_tcrc,(tmp32 >> 24) & 0xFF);
              uart_tmt_datadle(&uart_tcrc,(tmp32 >> 16) & 0xFF);
              uart_tmt_datadle(&uart_tcrc,(tmp32 >>  8) & 0xFF);
              uart_tmt_datadle(&uart_tcrc,(tmp32 >>  0) & 0xFF);
              uart_tmt_pos++;
            }
            else
            {
              tmp16 = uart_tcrc;
              uart_tmt_datadle(&uart_tcrc,(tmp16 >> 0) & 0xFF);
              uart_tmt_datadle(&uart_tcrc,(tmp16 >> 8) & 0xFF);
              uart_tmt_data(ETX);
              uart_state = SER_BASE;
            }

          }
          break;

        case SER_RDU16_ASTEXT:
          while ((UART_TMT_NOEMPTY() > 10) & (uart_tmt_pos < uart_tmt_len))
          {
            uart_tmt_datadle(&uart_tcrc,((uint16_t*)pU16rd)[uart_tmt_pos++]);
          }
          if (!(uart_tmt_pos < uart_tmt_len))
          {
            tmp16 = uart_tcrc;
            uart_tmt_datadle(&uart_tcrc,(tmp16 >> 0) & 0xFF);
            uart_tmt_datadle(&uart_tcrc,(tmp16 >> 8) & 0xFF);
            uart_tmt_data(ETX);
            uart_state = SER_BASE;
          }
          break;

        case SER_RD_SPI_START:
          if (spi_rd_start_rdy())
          {
            spi_rd_unused_rd_start();
						spi_set16bit();
            spi_rd_force();
            uart_state = SER_RD_SPI_VALID;
          }
          break;

        case SER_RD_SPI_VALID:
          if (spi_rd_rdy())
          {
            if (UART_TMT_NOEMPTY() > 10)
            {
              if (uart_tmt_pos < uart_tmt_len)
              {
                tmp16 = spi_rddata();
                uart_tmt_datadle(&uart_tcrc,(tmp16 >> 8) & 0xFF);
                uart_tmt_datadle(&uart_tcrc,(tmp16 >> 0) & 0xFF);
                spi_rd_force();
                uart_tmt_pos++;
              }
              else
              {
                tmp16 = uart_tcrc;
                uart_tmt_datadle(&uart_tcrc,(tmp16 >> 0) & 0xFF);
                uart_tmt_datadle(&uart_tcrc,(tmp16 >> 8) & 0xFF);
                uart_tmt_data(ETX);
                spi_sm = SPI_DNTG;
                spi_cs_disable();
                uart_state = SER_BASE;
              }
            }
          }
          break;

      }


  #else
   #if 1
    if (UART_DATA_ARRIVED) {
      //UART_TMT_DATA = UART_REC_DATA;
      uart_tmt_data(UART_REC_DATA);
      //  DELAY_US(1000);
    }
   #else
    if (UART_TMT_NOEMPTY() > 10)
    {
      uart_tmt_data(tmt_dat++);
        DELAY_US(1000);
    }
   #endif
  #endif


#if 1
  #ifndef USE_HWFIFO
  if (TransmitEnable)
	{
		if (UART_DATA_SEMPTY)
		{
			if (swt_fifo_wr != swt_fifo_rd)
			{
				UART_TMT_DATA = swt_fifo[swt_fifo_rd];
				swt_fifo_rd = (swt_fifo_rd + 1) % SW_FIFO_LEN;
			}
		}
	}
  #endif
#endif
}




