#include "gen_types.h"
#include "err_hyst.h"
#include "spi_mng.h"

uint32_t err_hyst[ERRHYST_LEN][2];

/*!
 *  \brief Error history debug intialization.
 *  \author Weitzl Zoltan
 *  \version 1.0
 *	\date 2010. 07. 01.
 */

void InitErrHyst_dbg()
{
  err_hyst[0][0] = (9L << 24) + (12L << 16) + (1L << 8) + 11; err_hyst[0][1] = (30L << 24) + (21L << 16) + (25L << 8) + 1;
  err_hyst[1][0] = (9L << 24) + (12L << 16) + (2L << 8) + 12; err_hyst[1][1] = (30L << 24) + (21L << 16) + (3L << 8) + 2;
  err_hyst[2][0] = (9L << 24) + (12L << 16) + (3L << 8) + 13; err_hyst[2][1] = (30L << 24) + (21L << 16) + (5L << 8) + 3;
  err_hyst[3][0] = (9L << 24) + (12L << 16) + (4L << 8) + 14; err_hyst[3][1] = (30L << 24) + (21L << 16) + (7L << 8) + 4;
  err_hyst[4][0] = (9L << 24) + (12L << 16) + (5L << 8) + 15; err_hyst[4][1] = (30L << 24) + (21L << 16) + (8L << 8) + 4;
  err_hyst[5][0] = (9L << 24) + (12L << 16) + (6L << 8) + 16; err_hyst[5][1] = (30L << 24) + (21L << 16) + (9L << 8) + 4;
  err_hyst[6][0] = (10L << 24) + (1L << 16) + (1L << 8) + 5;  err_hyst[6][1] = (30L << 24) + (21L << 16) + (10L << 8) + 4;
  err_hyst[7][0] = (10L << 24) + (1L << 16) + (2L << 8) + 6;  err_hyst[7][1] = (33L << 24) + (45L << 16) + (22L << 8) + 5;
  err_hyst[8][0] = (10L << 24) + (1L << 16) + (3L << 8) + 7;  err_hyst[8][1] = (33L << 24) + (45L << 16) + (44L << 8) + 6;
  err_hyst[9][0] = (10L << 24) + (1L << 16) + (4L << 8) + 8;  err_hyst[9][1] = (33L << 24) + (45L << 16) + (55L << 8) + 7;
  err_hyst[10][0] = (10L << 24) + (1L << 16) + (5L << 8) + 9; err_hyst[10][1] = (33L << 24) + (45L << 16) + (99L << 8) + 8;
  err_hyst[11][0] = (10L << 24) + (1L << 16) + (6L << 8) + 10;  err_hyst[11][1] = (33L << 24) + (45L << 16) + (33L << 8) + 99;
  err_hyst[12][0] = (10L << 24) + (1L << 16) + (7L << 8) + 11;  err_hyst[12][1] = (33L << 24) + (45L << 16) + (33L << 8) + 3;
  err_hyst[13][0] = (10L << 24) + (1L << 16) + (8L << 8) + 12;  err_hyst[13][1] = (33L << 24) + (45L << 16) + (3L << 8) + 1;
  err_hyst[14][0] = (10L << 24) + (1L << 16) + (9L << 8) + 13;  err_hyst[14][1] = (33L << 24) + (45L << 16) + (25L << 8) + 2;
#if 0
  err_hyst[15][0] = (10L << 24) + (1L << 16) + (14L << 8) + 14; err_hyst[15][1] = (33L << 24) + (45L << 16) + (47L << 8) + 3;
  err_hyst[16][0] = (10L << 24) + (1L << 16) + (15L << 8) + 15; err_hyst[16][1] = (33L << 24) + (45L << 16) + (69L << 8) + 4;
  err_hyst[17][0] = (10L << 24) + (1L << 16) + (16L << 8) + 16; err_hyst[17][1] = (33L << 24) + (45L << 16) + (91L << 8) + 5;
  err_hyst[18][0] = (10L << 24) + (1L << 16) + (17L << 8) + 17; err_hyst[18][1] = (33L << 24) + (45L << 16) + (113L << 8) + 6;
  err_hyst[19][0] = (10L << 24) + (1L << 16) + (18L << 8) + 18; err_hyst[19][1] = (33L << 24) + (45L << 16) + (135L << 8) + 7;
  err_hyst[20][0] = (10L << 24) + (1L << 16) + (19L << 8) + 19; err_hyst[20][1] = (33L << 24) + (45L << 16) + (157L << 8) + 8;
  err_hyst[21][0] = (10L << 24) + (1L << 16) + (20L << 8) + 20; err_hyst[21][1] = (33L << 24) + (45L << 16) + (179L << 8) + 9;
#endif
}

uint8_t eh_push = 0;
uint8_t eh_push_data[4];
extern uint32_t c_date, c_time;

 /*!
   *  \brief Add error history data to the internal buffer (1 deep FIFO)
   *  later store in the external SPI access ram.
   *  \author Weitzl Zoltan
   *  \version 1.0
   *	\date 2010. 07. 01.
   *  \param ecode Error code
   *  \param mainsm Main statemachine value
 */
void SaveErrorHistoryData(uint8_t ecode, uint8_t mainsm)
{
  if (0 == eh_push)
  {
    eh_push_data[0] = c_date >> 8; // year, month
    eh_push_data[1] = ((c_date & 0xFF) << 8) + ((c_time >> 16) & 0xFF); // day, hour
    eh_push_data[2] = c_time & 0xFFFF;  // min, sec
    eh_push_data[3] = ((ecode & 0xFF) << 8) + (mainsm & 0xFF); // errorcode, mainsm
    eh_push = 1;
  }
}

uint32_t eh_idx = 0; /*!< error history id to be written */
uint16_t eh_push_SM = EHP_DNTG; /*!< error history write statemachine */

 /*!
   *  \brief Error history push background
   *  Error history data is pushed into external SPI nonvolatile RAM
   *  \author Weitzl Zoltan
   *  \version 1.0
   *	\date 2010. 07. 01.
 */
void ErrorHistoryPushBckGrnd(void)
{
  switch (eh_push_SM)
  {
    case EHP_DNTG:
      if (eh_push)
      {
        if (SPI_DNTG == spi_sm)
        {
          spi_sm = SPI_ERR_PUSH;
          eh_idx = (eh_idx + 1) & (EHIST_LEN - 1);
          eh_push_SM = EHP_START;
        }
      }
      break;
    case EHP_START:
      spi_cs_disable();
      spi_cs_assert(1);
      spi_wr_reg(FM_WREN);

      eh_push_SM = EHP_WT_WREN_IDX;
      break;
    case EHP_WT_WREN_IDX:
      if (spi_rd_count() == 1)
      {
        spi_cs_disable();
        spi_cs_assert(1);
        spi_wr_start(EHIST_ENTRY_LEN_IN16 * EHIST_LEN);
        spi_wr_reg(eh_idx >> 16);
        spi_wr_reg(eh_idx & 0xFFFF);
        eh_push_SM = EHP_WT_WRIDX;
      }
      break;
    case EHP_WT_WRIDX:
      if (spi_rd_count() == 4)
      {
        spi_cs_disable();
        spi_cs_assert(1);
        spi_wr_reg(FM_WREN);
        eh_push_SM = EHP_WT_WREN_EH;
      }
      break;
    case EHP_WT_WREN_EH:
      if (spi_rd_count() == 1)
      {
        spi_cs_disable();
        spi_cs_assert(1);
        spi_wr_start(eh_idx * EHIST_ENTRY_LEN_IN16);
        spi_wr_reg(eh_push_data[0]);
        spi_wr_reg(eh_push_data[1]);
        spi_wr_reg(eh_push_data[2]);
        spi_wr_reg(eh_push_data[3]);
        eh_push_SM = EHP_WT_FINISHED;
      }
      break;
    case EHP_WT_FINISHED:
      if (spi_rd_count() == 6)
      {
        spi_cs_disable();
        spi_sm = SPI_DNTG;
        eh_push_SM = EHP_DNTG;
        eh_push = 0;
      }
      break;
    case EHP_START_CLEAR:
      spi_cs_disable();
      spi_cs_assert(1);
      spi_wr_reg(FM_WREN);
      eh_push_SM = EHP_WT_WREN_CLEAR;
      break;
    case EHP_WT_WREN_CLEAR:
      if (spi_rd_count() == 1)
      {
        spi_cs_disable();
        spi_cs_assert(1);
        spi_wr_start(0);
        eh_push_SM = EHP_WT_CLR_ADDR;
      }
      break;
    case EHP_WT_CLR_ADDR:
      if (spi_rd_count() == 2)
      {
        spi_rd_unused_rd_start();
        spi_wr_reg(0x00);
        spi_wr_reg(0x00);
        spi_wr_reg(0x00);
        spi_wr_reg(0x00);
        eh_idx = EHIST_LEN;
        eh_push_SM = EHP_WT_CLRING;
      }
      break;
    case EHP_WT_CLRING:
      if (spi_rd_count() == 4)
      {
        if (--eh_idx == 0)
        {
          spi_cs_disable();
          spi_sm = SPI_DNTG;
          eh_push_SM = EHP_DNTG;
          eh_push = 0;
        }
        else
        {
          spi_rd_unused_rd_start();
          spi_rd_unused_rd_start();
          spi_wr_reg(0x00);
          spi_wr_reg(0x00);
          spi_wr_reg(0x00);
          spi_wr_reg(0x00);
        }
      }
      break;

  }
}

/*!
 *  \brief Read out id of error history.
 *  \author Weitzl Zoltan
 *  \version 1.0
 *	\date 2010. 07. 01.
 */

void ReadOutErrorHistoryIdx(void)
{
  uint16_t tmp[2];
  spi_cs_disable();
  spi_cs_assert(1);
  spi_rd_start(EHIST_ENTRY_LEN_IN16 * EHIST_LEN);
  while (!spi_rd_start_rdy());
  spi_rd_unused_rd_start();
	spi_set16bit();
  spi_rd_force();
  spi_rd_force();
  while (spi_rd_count() != 2);
  tmp[0] = spi_rddata();
  tmp[1] = spi_rddata();
  eh_idx = (((uint32_t)tmp[0]) << 16) + tmp[1];
  eh_idx &= (EHIST_LEN - 1);
}


