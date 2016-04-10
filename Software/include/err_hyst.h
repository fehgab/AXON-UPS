#ifndef _ERR_HYST_H_
#define _ERR_HYST_H_


#define EHIST_LEN  0x100 /*!< error history length in entries - should be power of 2*/
#define EHIST_ENTRY_LEN_IN16 4 /*!< nr of bytes in one entry */

//@{
/*! Error history write statemachine*/
#define EHP_DNTG             0 /*!< wait for data to be written */
#define EHP_START            1 /*!< start data writing */
#define EHP_WT_WREN_IDX      2 /*!< wait for write enable for idx is in progress */
#define EHP_WT_WRIDX         3 /*!< wait for idx writing in progress */
#define EHP_WT_WREN_EH       4 /*!< wait for write enable for error history entry in progress */
#define EHP_WT_FINISHED      5 /*!< wait for all finished */

#define EHP_START_CLEAR    100 /*!< start clearing of error history */
#define EHP_WT_WREN_CLEAR  101 /*!< wait for wren sent */
#define EHP_WT_CLR_ADDR    102 /*!< wait address to be sent */
#define EHP_WT_CLRING      103 /*!< clearing 32bit-by-32bit */
//@}

#define ERRHYST_LEN   15

extern uint32_t err_hyst[ERRHYST_LEN][2];

#endif

