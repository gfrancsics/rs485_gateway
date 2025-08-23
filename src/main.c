/**
  Generated Main Source File

  Company:
    Microchip Technology Inc.

  File Name:
    main.c

  Summary:
    This is the main file generated using PIC10 / PIC12 / PIC16 / PIC18 MCUs

  Description:
    This header file provides implementations for driver APIs for all modules selected in the GUI.
    Generation Information :
        Product Revision  :  PIC10 / PIC12 / PIC16 / PIC18 MCUs - 1.81.8
        Device            :  PIC18F16Q40
        Driver Version    :  2.00
*/

/*
    (c) 2018 Microchip Technology Inc. and its subsidiaries. 
    
    Subject to your compliance with these terms, you may use Microchip software and any 
    derivatives exclusively with Microchip products. It is your responsibility to comply with third party 
    license terms applicable to your use of third party software (including open source software) that 
    may accompany Microchip software.
    
    THIS SOFTWARE IS SUPPLIED BY MICROCHIP "AS IS". NO WARRANTIES, WHETHER 
    EXPRESS, IMPLIED OR STATUTORY, APPLY TO THIS SOFTWARE, INCLUDING ANY 
    IMPLIED WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY, AND FITNESS 
    FOR A PARTICULAR PURPOSE.
    
    IN NO EVENT WILL MICROCHIP BE LIABLE FOR ANY INDIRECT, SPECIAL, PUNITIVE, 
    INCIDENTAL OR CONSEQUENTIAL LOSS, DAMAGE, COST OR EXPENSE OF ANY KIND 
    WHATSOEVER RELATED TO THE SOFTWARE, HOWEVER CAUSED, EVEN IF MICROCHIP 
    HAS BEEN ADVISED OF THE POSSIBILITY OR THE DAMAGES ARE FORESEEABLE. TO 
    THE FULLEST EXTENT ALLOWED BY LAW, MICROCHIP'S TOTAL LIABILITY ON ALL 
    CLAIMS IN ANY WAY RELATED TO THIS SOFTWARE WILL NOT EXCEED THE AMOUNT 
    OF FEES, IF ANY, THAT YOU HAVE PAID DIRECTLY TO MICROCHIP FOR THIS 
    SOFTWARE.
*/
#include "../mcc_generated_files/mcc.h"
#include "task.h"

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

//task configuration table
static task_t tasks[] =
{
    {CONTINUOUS,    0,  task},
    {INTERVAL_10MS, 0,  task_10ms},
    {INTERVAL_100MS,0,  task_100ms},
    {INTERVAL_1S,   0,  task_1000ms},
    {INTERVAL_1M,   0,  task_60000ms},
};

static volatile uint32_t tick = 0;
/*
                         Main application
 */
void main(void)
{    
    // initialize the device
    SYSTEM_Initialize();
    
    TMR1_SetInterruptHandler(timer1_ISR);
    
    // If using interrupts in PIC18 High/Low Priority Mode you need to enable the Global High and Low Interrupts
    // If using interrupts in PIC Mid-Range Compatibility Mode you need to enable the Global Interrupts
    // Use the following macros to:
    
    // Enable the Global Interrupts
    INTERRUPT_GlobalInterruptEnable();
    
    // Disable the Global Interrupts
    //INTERRUPT_GlobalInterruptDisable();
    
    while (1)
    {
        // Add your application code
        run_scheduler(tasks, sizeof(tasks)/sizeof(task_t), tick);
    }
}

void task(void)
{
}

void task_10ms(void)
{
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