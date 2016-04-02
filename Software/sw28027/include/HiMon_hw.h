#ifndef _SER_HW_H_
	#define _SER_HW_H_

	extern volatile struct SCI_REGS *sci_ptr_terminal;

	//#define USE_HWFIFO
	#define RxRDY ( sci_ptr_terminal->SCIFFRX.bit.RXFFST )
	#define TxRDY ( sci_ptr_terminal->SCIFFTX.bit.TXFFST!= 4 )
	#define RxDAT ( sci_ptr_terminal->SCIRXBUF.bit.RXDT & 0xFF )
	#define TxDAT sci_ptr_terminal->SCITXBUF
	#define TxEMPTY ( sci_ptr_terminal->SCICTL2.bit.TXEMPTY )
	#define RxERR   ( sci_ptr_terminal->SCIRXST.bit.RXERROR )
	#define RxSWRESET ( sci_ptr_terminal->SCICTL1.bit.SWRESET )

	#define UART_REC_DATA     ( sci_ptr_terminal->SCIRXBUF.bit.RXDT & 0xFF )


	#ifndef USE_HWFIFO
		#define SW_FIFO_LEN 16

		extern uint8_t swt_fifo[SW_FIFO_LEN];
		extern uint8_t swt_fifo_wr;
		extern uint8_t swt_fifo_rd;

		inline uart_tmt_data(uint8_t dat)
		{
			swt_fifo[swt_fifo_wr] = dat;
			swt_fifo_wr = (swt_fifo_wr + 1) % SW_FIFO_LEN;
		}
		inline uint8_t UART_TMT_NOEMPTY()
		{
			if (swt_fifo_rd == swt_fifo_wr)
			 return SW_FIFO_LEN;
			else
			 return (swt_fifo_rd - swt_fifo_wr + SW_FIFO_LEN) % SW_FIFO_LEN;
		}
		#define UART_DATA_SEMPTY sci_ptr_terminal->SCICTL2.bit.TXEMPTY
		#define UART_TMT_DATA sci_ptr_terminal->SCITXBUF
		#define UART_DATA_ARRIVED ( sci_ptr_terminal->SCIFFRX.bit.RXFFST )
	#else
		inline void uart_tmt_data(uint8_t data)
		{
			SciaRegs.SCITXBUF = data;
		}
		#define UART_TMT_NOEMPTY()  ( 4 - sci_ptr_terminal->SCIFFTX.bit.TXFFST )
		#define UART_DATA_ARRIVED ( sci_ptr_terminal->SCIFFRX.bit.RXFFST )
	#endif

#endif

