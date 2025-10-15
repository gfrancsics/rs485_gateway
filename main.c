#include "mcc_generated_files/mcc.h"
#include "uart1_statemachine.h"

void timer1_ISR(void);

volatile uint32_t tick;

/*
                         Main application
 */
int main()
{
    // initialize the device
    SYSTEM_Initialize();
    
    TMR1_SetInterruptHandler(timer1_ISR);
    
    UART1_initialise_logic();
    
    // If using interrupts in PIC18 High/Low Priority Mode you need to enable the Global High and Low Interrupts
    // If using interrupts in PIC Mid-Range Compatibility Mode you need to enable the Global Interrupts
    // Use the following macros to:

    // Enable the Global Interrupts
    INTERRUPT_GlobalInterruptEnable();

    // Disable the Global Interrupts
    //INTERRUPT_GlobalInterruptDisable();

    dynamic_funtion_t current_function = FUNC_REQ;

    while (1)
    {
        switch(current_function) {
            case FUNC_REQ:
                current_function = UART1_request_state();
                break;
            case FUNC_RESP:
                current_function = UART1_response_state();
                break;
        }
    }
}

void timer1_ISR(void)
{
    tick++;
}
/**
 End of File
*/