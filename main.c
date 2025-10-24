#include "mcc_generated_files/mcc.h"
#include "uart1_statemachine.h"
#include "uart2_statemachine.h"

// Tick számláló. Volatile és static for INTERNAL use
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
    
    // 1. HARDVER BEÁLLÍTÁSOK (a profilerhez)
    // Tegyük fel, hogy a RB7-t használjuk mérésre
    LED_SetLow();

    // 2. TESZT CIKLUSOK
    bool is_response_ready = false;
    
    while(1) 
    {
        LED_Toggle();
        // Futtassunk 1000 iterációt a megbízható méréshez
        for(int i = 0; i < 10; i++) 
        {
            // 1. KÉRELEM FELDOLGOZÁSA: Betöltjük a keretet
            // A szimulált UART_Read_NonBlocking automatikusan kiadja a 8 bájtot 
            // a 'UART1_request_state()' bels? while ciklusában.
            power_consumption=(uint32_t)i;

            // Futtatjuk az állapotgépet, amíg a FUNC_RESP-be nem kerül
            // (vagyis az összes bemeneti bájt el nem fogy)
            if (!is_response_ready) {
                dynamic_funtion_t state = UART1_request_state();

                if (state == FUNC_RESP) {
                    is_response_ready = true;
                }
            }

            // 2. VÁLASZ GENERÁLÁS ÉS MÉRÉS
            if (is_response_ready) {
                // A GPIO Toggle mérés a UART1_response_state() BELS?BEN van elhelyezve!
                dynamic_funtion_t state = UART1_response_state();

                if (state == FUNC_REQ) {
                    // SIKERES KÜLDÉS: Visszakapcsolt RX-re és kész.

                    // ÚJ TESZT INDÍTÁSA
                    is_response_ready = false; 
                    SIM_RX_INDEX = 0; // Újra betölthetjük a 8 bájtot a mock-ból
                }
            }

            // Késleltetés nélkül, a lehet? leggyorsabban futtatjuk a ciklust a méréshez
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