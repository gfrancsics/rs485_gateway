#ifndef UART1_STATEMACHINE_H
#define	UART1_STATEMACHINE_H

#ifdef	__cplusplus
extern "C" {
#endif

#include "statemachine.h"   
#include "mcc_generated_files/mcc.h"
#include "checksum.h"

#define UART2_RX_EN() USART_EN2_SetLow()
#define UART2_TX_EN() USART_EN2_SetHigh()

void UART2_initialise_logic(void);

#ifdef	__cplusplus
}
#endif

#endif	/* UART1_STATEMACHINE_H */