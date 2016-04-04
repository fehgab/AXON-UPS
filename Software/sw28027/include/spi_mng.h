#ifndef SPI_MNG_H_
#define SPI_MNG_H_

#include "DSP2802x_Device.h"
#include "bbx.h"

/* OP codes same for FRAM and SRAM */
#define FM_SRAM_READ 0x0300
#define FM_SRAM_WR   0x0200
#define FM_SRAM_WRSR 0x0100
#define FM_SRAM_RDSR 0x0500
/* OP codes only for FRAM */
#define FM_RDID 0x9F00
#define FM_WREN 0x0600

extern uint16_t spi_sm;

#define SPI_DNTG        0 /*!< SPI is not busy */
#define SPI_OCC_HIMON   1 /*!< SPI is busy with reading out for monitoring */
#define SPI_OCC_BBX     2 /*!< SPI is busy with blackbox */
#define SPI_ERR_PUSH    3 /*!< SPI is busy with pushing error history data */

#define SPI_ADDRESS_MASK (~0xC0000000) /*!< spi address mask - CS elimination */
#define SPI_ADD_CS_SHIFT 30 /*!< spi address shift - to get CS */

/*!
 *  \brief Assert the a-th CS of SPI
 */
inline spi_cs_assert(uint16_t a)
{
	if (BBX_MEM_FRAM == bbx_mem_type || BBX_MEM_SRAM == bbx_mem_type)
	{
//		if (a)
//			GpioDataRegs.GPBCLEAR.bit.GPIO57 = 1;
//		else
//			GpioDataRegs.GPBCLEAR.bit.GPIO58 = 1;
	}
	else if (BBX_MEM_DSPRAM == bbx_mem_type)
	{

	}
}

/*!
 *  \brief Disable all CS
 */
inline spi_cs_disable(void)
{
	if (BBX_MEM_FRAM == bbx_mem_type || BBX_MEM_SRAM == bbx_mem_type)
	{
//		GpioDataRegs.GPBSET.bit.GPIO57 = 1;
//		GpioDataRegs.GPBSET.bit.GPIO58 = 1;
		SpiaRegs.SPIFFTX.bit.TXFIFO = 0;
		SpiaRegs.SPIFFRX.bit.RXFIFORESET = 0;
		SpiaRegs.SPIFFTX.bit.TXFIFO = 1;
		SpiaRegs.SPIFFRX.bit.RXFIFORESET = 1;
	}
	else if (BBX_MEM_DSPRAM == bbx_mem_type)
	{

	}
}

inline uint8_t spi_rd_count()
{
	return SpiaRegs.SPIFFRX.bit.RXFFST;
}

inline spi_set8bit()
{
	SpiaRegs.SPICCR.bit.SPICHAR = 7;
}

inline spi_set16bit()
{
	SpiaRegs.SPICCR.bit.SPICHAR = 15;
}

inline spi_rd_start(uint32_t address)
{
	if (BBX_MEM_FRAM == bbx_mem_type)
	{
		spi_set16bit();
		SpiaRegs.SPITXBUF = FM_SRAM_READ + (((address * 2) >> 16) & 0xFF);
		SpiaRegs.SPITXBUF = (address * 2 ) & 0xFFFF;
	}
	else if (BBX_MEM_SRAM == bbx_mem_type)
	{
		spi_set8bit();
		SpiaRegs.SPITXBUF = FM_SRAM_READ;
		SpiaRegs.SPITXBUF = (address * 2) & 0xFF00;
		SpiaRegs.SPITXBUF = ((address * 2) << 8) & 0xFF00;
	}
	else if (BBX_MEM_DSPRAM == bbx_mem_type)
	{
	}
}

inline uint8_t spi_rd_start_rdy()
{
	if (BBX_MEM_FRAM == bbx_mem_type)
	{
		if (2 == spi_rd_count())
			return 1;
	}
	else if (BBX_MEM_SRAM == bbx_mem_type)
	{
		if (3 == spi_rd_count())
			return 1;
	}
	else if (BBX_MEM_DSPRAM == bbx_mem_type)
	{
	}
	return 0;
}

inline spi_rd_unused_rd_start()
{
  volatile uint16_t tmp;
	if (BBX_MEM_FRAM == bbx_mem_type)
	{
		tmp = SpiaRegs.SPIRXBUF;
		tmp = SpiaRegs.SPIRXBUF;
	}
	else if (BBX_MEM_SRAM == bbx_mem_type)
	{
		tmp = SpiaRegs.SPIRXBUF;
		tmp = SpiaRegs.SPIRXBUF;
		tmp = SpiaRegs.SPIRXBUF;
	}
	else if (BBX_MEM_DSPRAM == bbx_mem_type)
	{
	}
}

inline spi_wr_start(uint32_t address)
{
	if (BBX_MEM_FRAM == bbx_mem_type)
	{
		spi_set16bit();
		SpiaRegs.SPITXBUF = FM_SRAM_WR + (((address * 2) >> 16) & 0xFF);
		SpiaRegs.SPITXBUF = (address * 2 ) & 0xFFFF;
	}
	else if (BBX_MEM_SRAM == bbx_mem_type)
	{
		spi_set8bit();
		SpiaRegs.SPITXBUF = FM_SRAM_WR;
		SpiaRegs.SPITXBUF = (address * 2) & 0xFF00;
		SpiaRegs.SPITXBUF = ((address * 2) << 8) & 0xFF00;
	}
	else if (BBX_MEM_DSPRAM == bbx_mem_type)
	{
	}
}

inline uint8_t spi_wr_start_rdy()
{
	if (BBX_MEM_FRAM == bbx_mem_type)
	{
		if (2 == spi_rd_count())
			return 1;
	}
	else if (BBX_MEM_SRAM == bbx_mem_type)
	{
		if (3 == spi_rd_count())
			return 1;
	}
	else if (BBX_MEM_DSPRAM == bbx_mem_type)
	{
			return 1;
	}
	return 0;
}

inline spi_rd_force()
{
	SpiaRegs.SPITXBUF = 0xFFFF;
}

inline spi_wr_reg(uint16_t data)
{
	SpiaRegs.SPITXBUF = data;
}

inline uint16_t spi_rddata()
{
	return SpiaRegs.SPIRXBUF;
}

inline uint8_t spi_rd_rdy()
{
	if (SpiaRegs.SPIFFRX.bit.RXFFST != 0)
		return 1;
	else
		return 0;
}

inline uint8_t spi_tx_empty()
{
	if (SpiaRegs.SPIFFTX.bit.TXFFST == 0)
		return 1;
	else
		return 0;
}

inline spi_init_mem()
{

	if (BBX_MEM_FRAM == bbx_mem_type)
	{
		spi_set8bit();
		SpiaRegs.SPITXBUF = FM_WREN;
	}
	else if (BBX_MEM_SRAM == bbx_mem_type)
	{
		spi_set8bit();
		SpiaRegs.SPITXBUF = FM_SRAM_WRSR;
		SpiaRegs.SPITXBUF = 0x4100;
	}
	else if (BBX_MEM_DSPRAM == bbx_mem_type)
	{
	}
}

inline uint8_t spi_init_mem_rdy()
{
	if (BBX_MEM_FRAM == bbx_mem_type)
	{
		if (1 == spi_rd_count())
			return 1;
	}
	else if (BBX_MEM_SRAM == bbx_mem_type)
	{
		if (2 == spi_rd_count())
			return 1;
	}
	else if (BBX_MEM_DSPRAM == bbx_mem_type)
	{
		return 1;
	}
	return 0;
}
#endif


