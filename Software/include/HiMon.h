#ifndef _HIMON_H_
#define _HIMON_H_

#define SIMULATE_MORE_DSPS    0

#define BAUD_RATE 115200

#define SIMPLE_ECHO 0

#define UART_CMD_LEN          256

#define SER_BASE              0
#define SER_RECING            1
#define SER_SENDING           2
#define SER_ANS_RDB           3
#define SER_ANS_RDDAT         4
#define SER_RDTABS            5
#define SER_RDU16_ASTEXT      6
#define SER_RDTABVALUES       7
#define SER_RDRAW             8
#define SER_RD_BBX            9
#define SER_RD_SPI_START     10
	#define SER_RD_SPI_VALID     11

	#define STX 0x3C                         /*!< Start of message character*/
	#define DLE 0x3D                         /*!< Escape of message character*/
	#define ETX 0x3E                         /*!< End of message character*/
	#define XDT 0x30                         /*!< Start of message character*/

	#define MON_RD            0x10           /*!< Read single/multple variables */
	#define MON_WR            0x23           /*!< Write single/multple variables */
	#define MON_TABS(i)      (0x40 + i)      /*!< Read the i-th tab descriptor*/
	#define MON_TABNAMES      0x55           /*!< Read the name of the tabs in user mode (only for 0th address DSP)*/
	#define MON_TABVALUES(i) (0xA0+i)        /*!< not implemented */
	#define MON_RDUNITS       0x79           /*!< read unit table */
	#define MON_RD_ERRH       0x51           /*!< read error history */
	#define MON_RD_BBX        0x24           /*!< read black box content */
	#define MON_RD_SPI_RAM    0x75           /*!< read spi ram (bbx, error history */
	#define MON_REBOOT        0x80           /*!< Reboot DSP */
	#define MON_PING          0x99           /*!< ping DSP */

void Serial_mng(void);
void InitTermVars();

#endif

