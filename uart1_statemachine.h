#ifndef UART1_STATEMACHINE_H
#define	UART1_STATEMACHINE_H

#ifdef	__cplusplus
extern "C" {
#endif

#include "mcc_generated_files/mcc.h"
#include "checksum.h"

#define UART1_RX_EN() USART_EN1_SetLow()
#define UART1_TX_EN() USART_EN1_SetHigh()
    
typedef enum {
    FUNC_REQ,
    FUNC_RESP
} dynamic_funtion_t;

/**
 * HOWTO USE
 * dynamic_funtion_t current_function=FUNC_REQ;
 * current_function == FUNC_REQ ? current_function=UART1_request_state():current_function=UART1_response_state()
 * 
 */

void UART1_initialise_logic(void);
dynamic_funtion_t UART1_response_state(void);
dynamic_funtion_t UART1_request_state(void);

#ifdef	__cplusplus
}
#endif

#endif	/* UART1_STATEMACHINE_H */