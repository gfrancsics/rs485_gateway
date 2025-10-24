#include "mcc_generated_files/mcc.h"
#include "uart1_statemachine.h"
#include "uart2_statemachine.h"

// Tick sz�ml�l�. Volatile �s static for INTERNAL use
static volatile uint32_t tick = 0;
void timer1_ISR(void);
extern uint8_t SIM_RX_INDEX;
uint32_t power_consumption;

/*
                         Main application
 */
void main()
{
    // initialize the device
    SYSTEM_Initialize();
    
    TMR1_SetInterruptHandler(timer1_ISR);
    
    //UART1_initialise_logic();
    //UART2_initialise_logic();

    // If using interrupts in PIC18 High/Low Priority Mode you need to enable the Global High and Low Interrupts
    // If using interrupts in PIC Mid-Range Compatibility Mode you need to enable the Global Interrupts
    // Use the following macros to:

    // Enable the Global Interrupts
    INTERRUPT_GlobalInterruptEnable();

    // Disable the Global Interrupts
    //INTERRUPT_GlobalInterruptDisable();
    
    // 1. HARDVER BE�LL�T�SOK (a profilerhez)
    // Tegy�k fel, hogy a RB7-t haszn�ljuk m�r�sre
    LED_SetLow();

    // 2. TESZT CIKLUSOK
    bool is_response_ready = false;
    
    while(1) 
    {
        LED_Toggle();
        // Futtassunk 1000 iter�ci�t a megb�zhat� m�r�shez
        for(int i = 0; i < 10; i++) 
        {
            // 1. K�RELEM FELDOLGOZ�SA: Bet�ltj�k a keretet
            // A szimul�lt UART_Read_NonBlocking automatikusan kiadja a 8 b�jtot 
            // a 'UART1_request_state()' bels? while ciklus�ban.
            power_consumption=(uint32_t)i;

            // Futtatjuk az �llapotg�pet, am�g a FUNC_RESP-be nem ker�l
            // (vagyis az �sszes bemeneti b�jt el nem fogy)
            if (!is_response_ready) {
                dynamic_funtion_t state = UART1_request_state();

                if (state == FUNC_RESP) {
                    is_response_ready = true;
                }
            }

            // 2. V�LASZ GENER�L�S �S M�R�S
            if (is_response_ready) {
                // A GPIO Toggle m�r�s a UART1_response_state() BELS?BEN van elhelyezve!
                dynamic_funtion_t state = UART1_response_state();

                if (state == FUNC_REQ) {
                    // SIKERES K�LD�S: Visszakapcsolt RX-re �s k�sz.

                    // �J TESZT IND�T�SA
                    is_response_ready = false; 
                    SIM_RX_INDEX = 0; // �jra bet�lthetj�k a 8 b�jtot a mock-b�l
                }
            }

            // K�sleltet�s n�lk�l, a lehet? leggyorsabban futtatjuk a ciklust a m�r�shez
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