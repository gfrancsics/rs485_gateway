#ifndef UART2_STATEMACHINE_H
#define	UART2_STATEMACHINE_H

#ifdef	__cplusplus
extern "C" {
#endif

#include "mcc_generated_files/mcc.h"
#include "checksum.h"
#include "uart_common_types.h"

#define UART2_RX_EN() USART_EN2_SetLow()
#define UART2_TX_EN() USART_EN2_SetHigh()
    
void UART2_initialise_logic(void);
dynamic_funtion_t UART2_request_state(void);
dynamic_funtion_t UART2_response_state(void);

#ifdef	__cplusplus
}
#endif

#endif	/* UART2_STATEMACHINE_H */