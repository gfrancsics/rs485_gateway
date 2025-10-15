#include "mcc_generated_files/mcc.h"
#include "uart1_statemachine.h"
#include "uart2_statemachine.h"

#define CONTINUOUS 0
#define INTERVAL_10MS 10
#define INTERVAL_100MS 100
#define INTERVAL_1S 1000
#define INTERVAL_1M 60000

void timer1_ISR(void);
void task(void);
void task_10ms(void);
void task_100ms(void);
void task_1000ms(void);
void task_60000ms(void);

//task descriptor
typedef struct
{
    uint16_t interval;
    uint32_t last_tick;
    void (*func)(void);
} task_t;

//task configuration table for INTERNAL use
static task_t tasks[] =
{
    {CONTINUOUS,    0,  task},
    {INTERVAL_10MS, 0,  task_10ms},
    {INTERVAL_100MS,0,  task_100ms},
    {INTERVAL_1S,   0,  task_1000ms},
    {INTERVAL_1M,   0,  task_60000ms},
};

// Tick számláló. Volatile és static for INTERNAL use
static volatile uint32_t tick = 0;

// --- Segédmakrók az Atomicitás Biztosítására ---
// Ezek a makrók letiltják a megszakításokat a kritikus 32-bites olvasások/írások idejére.
// Az XC8 környezetnek megfelel? INTERRUPT_GlobalInterruptEnable() és Disable() hívásokat használja.
#define ATOMIC_START    INTERRUPT_GlobalInterruptDisable()
#define ATOMIC_END      INTERRUPT_GlobalInterruptEnable()


/*
                         Main application
 */
int main()
{
    uint8_t task_index = 0;
    const uint8_t number_of_tasks = sizeof(tasks)/sizeof(task_t);
    uint32_t current_tick_copy; // Atomikus olvasáshoz

    // initialize the device
    SYSTEM_Initialize();
    
    TMR1_SetInterruptHandler(timer1_ISR);
    
    UART1_initialise_logic();
    UART2_initialise_logic();

    // If using interrupts in PIC18 High/Low Priority Mode you need to enable the Global High and Low Interrupts
    // If using interrupts in PIC Mid-Range Compatibility Mode you need to enable the Global Interrupts
    // Use the following macros to:

    // Enable the Global Interrupts
    INTERRUPT_GlobalInterruptEnable();

    // Disable the Global Interrupts
    //INTERRUPT_GlobalInterruptDisable();

    while (1)
    {
        
        // 1. lépés: Megszakításmentes (Atomi) olvasás
        ATOMIC_START;
        current_tick_copy = tick;
        ATOMIC_END;
        
        // Add your application code
        for(task_index=0; task_index<number_of_tasks; task_index++)
        {
            if(tasks[task_index].interval == CONTINUOUS)
            {
                (*tasks[task_index].func)();
            }
            else if((current_tick_copy - tasks[task_index].last_tick) >= tasks[task_index].interval)
            {
                (*tasks[task_index].func)();
                
                // 2. lépés: Anti-drift logikával történ? id?zít? frissítés
                // A next_tick kiszámítása történhet megszakítás nélkül is,
                // de a last_tick értékadásához kell az atomicitás.
                
                ATOMIC_START;
                tasks[task_index].last_tick += tasks[task_index].interval;
                // Megjegyzés: Ha a feladat túl lassan fut, az intervalumot többször is hozzáadhatjuk, 
                // amíg a last_tick a current_tick_copy elé nem kerül.
                // Pl.: while (tasks[task_index].last_tick <= current_tick_copy) { tasks[task_index].last_tick += tasks[task_index].interval; }
                ATOMIC_END;
            }
        }
    }
}

void task(void)
{
}

static dynamic_funtion_t current_state=FUNC_REQ;

void task_10ms(void)
{
    if(current_state==FUNC_REQ) {
        current_state = UART1_request_state();
    } else {
        current_state = UART1_response_state();
    }
}

void task_100ms(void)
{
    
}

void task_1000ms(void)
{
    LED_Toggle();
}

void task_60000ms(void)
{
}

void timer1_ISR(void)
{
    tick++;
}

/**
 End of File
*/