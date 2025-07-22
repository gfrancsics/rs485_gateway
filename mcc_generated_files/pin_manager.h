/**
  @Generated Pin Manager Header File

  @Company:
    Microchip Technology Inc.

  @File Name:
    pin_manager.h

  @Summary:
    This is the Pin Manager file generated using PIC10 / PIC12 / PIC16 / PIC18 MCUs

  @Description
    This header file provides APIs for driver for .
    Generation Information :
        Product Revision  :  PIC10 / PIC12 / PIC16 / PIC18 MCUs - 1.81.8
        Device            :  PIC18F16Q40
        Driver Version    :  2.11
    The generated drivers are tested against the following:
        Compiler          :  XC8 2.36 and above
        MPLAB 	          :  MPLAB X 6.00	
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

#ifndef PIN_MANAGER_H
#define PIN_MANAGER_H

/**
  Section: Included Files
*/

#include <xc.h>

#define INPUT   1
#define OUTPUT  0

#define HIGH    1
#define LOW     0

#define ANALOG      1
#define DIGITAL     0

#define PULL_UP_ENABLED      1
#define PULL_UP_DISABLED     0

// get/set RA2 procedures
#define RA2_SetHigh()            do { LATAbits.LATA2 = 1; } while(0)
#define RA2_SetLow()             do { LATAbits.LATA2 = 0; } while(0)
#define RA2_Toggle()             do { LATAbits.LATA2 = ~LATAbits.LATA2; } while(0)
#define RA2_GetValue()              PORTAbits.RA2
#define RA2_SetDigitalInput()    do { TRISAbits.TRISA2 = 1; } while(0)
#define RA2_SetDigitalOutput()   do { TRISAbits.TRISA2 = 0; } while(0)
#define RA2_SetPullup()             do { WPUAbits.WPUA2 = 1; } while(0)
#define RA2_ResetPullup()           do { WPUAbits.WPUA2 = 0; } while(0)
#define RA2_SetAnalogMode()         do { ANSELAbits.ANSELA2 = 1; } while(0)
#define RA2_SetDigitalMode()        do { ANSELAbits.ANSELA2 = 0; } while(0)

// get/set RB4 procedures
#define RB4_SetHigh()            do { LATBbits.LATB4 = 1; } while(0)
#define RB4_SetLow()             do { LATBbits.LATB4 = 0; } while(0)
#define RB4_Toggle()             do { LATBbits.LATB4 = ~LATBbits.LATB4; } while(0)
#define RB4_GetValue()              PORTBbits.RB4
#define RB4_SetDigitalInput()    do { TRISBbits.TRISB4 = 1; } while(0)
#define RB4_SetDigitalOutput()   do { TRISBbits.TRISB4 = 0; } while(0)
#define RB4_SetPullup()             do { WPUBbits.WPUB4 = 1; } while(0)
#define RB4_ResetPullup()           do { WPUBbits.WPUB4 = 0; } while(0)
#define RB4_SetAnalogMode()         do { ANSELBbits.ANSELB4 = 1; } while(0)
#define RB4_SetDigitalMode()        do { ANSELBbits.ANSELB4 = 0; } while(0)

// get/set USART_EN2 aliases
#define USART_EN2_TRIS                 TRISBbits.TRISB5
#define USART_EN2_LAT                  LATBbits.LATB5
#define USART_EN2_PORT                 PORTBbits.RB5
#define USART_EN2_WPU                  WPUBbits.WPUB5
#define USART_EN2_OD                   ODCONBbits.ODCB5
#define USART_EN2_ANS                  ANSELBbits.ANSELB5
#define USART_EN2_SetHigh()            do { LATBbits.LATB5 = 1; } while(0)
#define USART_EN2_SetLow()             do { LATBbits.LATB5 = 0; } while(0)
#define USART_EN2_Toggle()             do { LATBbits.LATB5 = ~LATBbits.LATB5; } while(0)
#define USART_EN2_GetValue()           PORTBbits.RB5
#define USART_EN2_SetDigitalInput()    do { TRISBbits.TRISB5 = 1; } while(0)
#define USART_EN2_SetDigitalOutput()   do { TRISBbits.TRISB5 = 0; } while(0)
#define USART_EN2_SetPullup()          do { WPUBbits.WPUB5 = 1; } while(0)
#define USART_EN2_ResetPullup()        do { WPUBbits.WPUB5 = 0; } while(0)
#define USART_EN2_SetPushPull()        do { ODCONBbits.ODCB5 = 0; } while(0)
#define USART_EN2_SetOpenDrain()       do { ODCONBbits.ODCB5 = 1; } while(0)
#define USART_EN2_SetAnalogMode()      do { ANSELBbits.ANSELB5 = 1; } while(0)
#define USART_EN2_SetDigitalMode()     do { ANSELBbits.ANSELB5 = 0; } while(0)

// get/set RB6 procedures
#define RB6_SetHigh()            do { LATBbits.LATB6 = 1; } while(0)
#define RB6_SetLow()             do { LATBbits.LATB6 = 0; } while(0)
#define RB6_Toggle()             do { LATBbits.LATB6 = ~LATBbits.LATB6; } while(0)
#define RB6_GetValue()              PORTBbits.RB6
#define RB6_SetDigitalInput()    do { TRISBbits.TRISB6 = 1; } while(0)
#define RB6_SetDigitalOutput()   do { TRISBbits.TRISB6 = 0; } while(0)
#define RB6_SetPullup()             do { WPUBbits.WPUB6 = 1; } while(0)
#define RB6_ResetPullup()           do { WPUBbits.WPUB6 = 0; } while(0)
#define RB6_SetAnalogMode()         do { ANSELBbits.ANSELB6 = 1; } while(0)
#define RB6_SetDigitalMode()        do { ANSELBbits.ANSELB6 = 0; } while(0)

// get/set LED aliases
#define LED_TRIS                 TRISBbits.TRISB7
#define LED_LAT                  LATBbits.LATB7
#define LED_PORT                 PORTBbits.RB7
#define LED_WPU                  WPUBbits.WPUB7
#define LED_OD                   ODCONBbits.ODCB7
#define LED_ANS                  ANSELBbits.ANSELB7
#define LED_SetHigh()            do { LATBbits.LATB7 = 1; } while(0)
#define LED_SetLow()             do { LATBbits.LATB7 = 0; } while(0)
#define LED_Toggle()             do { LATBbits.LATB7 = ~LATBbits.LATB7; } while(0)
#define LED_GetValue()           PORTBbits.RB7
#define LED_SetDigitalInput()    do { TRISBbits.TRISB7 = 1; } while(0)
#define LED_SetDigitalOutput()   do { TRISBbits.TRISB7 = 0; } while(0)
#define LED_SetPullup()          do { WPUBbits.WPUB7 = 1; } while(0)
#define LED_ResetPullup()        do { WPUBbits.WPUB7 = 0; } while(0)
#define LED_SetPushPull()        do { ODCONBbits.ODCB7 = 0; } while(0)
#define LED_SetOpenDrain()       do { ODCONBbits.ODCB7 = 1; } while(0)
#define LED_SetAnalogMode()      do { ANSELBbits.ANSELB7 = 1; } while(0)
#define LED_SetDigitalMode()     do { ANSELBbits.ANSELB7 = 0; } while(0)

// get/set USART_EN1 aliases
#define USART_EN1_TRIS                 TRISCbits.TRISC0
#define USART_EN1_LAT                  LATCbits.LATC0
#define USART_EN1_PORT                 PORTCbits.RC0
#define USART_EN1_WPU                  WPUCbits.WPUC0
#define USART_EN1_OD                   ODCONCbits.ODCC0
#define USART_EN1_ANS                  ANSELCbits.ANSELC0
#define USART_EN1_SetHigh()            do { LATCbits.LATC0 = 1; } while(0)
#define USART_EN1_SetLow()             do { LATCbits.LATC0 = 0; } while(0)
#define USART_EN1_Toggle()             do { LATCbits.LATC0 = ~LATCbits.LATC0; } while(0)
#define USART_EN1_GetValue()           PORTCbits.RC0
#define USART_EN1_SetDigitalInput()    do { TRISCbits.TRISC0 = 1; } while(0)
#define USART_EN1_SetDigitalOutput()   do { TRISCbits.TRISC0 = 0; } while(0)
#define USART_EN1_SetPullup()          do { WPUCbits.WPUC0 = 1; } while(0)
#define USART_EN1_ResetPullup()        do { WPUCbits.WPUC0 = 0; } while(0)
#define USART_EN1_SetPushPull()        do { ODCONCbits.ODCC0 = 0; } while(0)
#define USART_EN1_SetOpenDrain()       do { ODCONCbits.ODCC0 = 1; } while(0)
#define USART_EN1_SetAnalogMode()      do { ANSELCbits.ANSELC0 = 1; } while(0)
#define USART_EN1_SetDigitalMode()     do { ANSELCbits.ANSELC0 = 0; } while(0)

// get/set RC2 procedures
#define RC2_SetHigh()            do { LATCbits.LATC2 = 1; } while(0)
#define RC2_SetLow()             do { LATCbits.LATC2 = 0; } while(0)
#define RC2_Toggle()             do { LATCbits.LATC2 = ~LATCbits.LATC2; } while(0)
#define RC2_GetValue()              PORTCbits.RC2
#define RC2_SetDigitalInput()    do { TRISCbits.TRISC2 = 1; } while(0)
#define RC2_SetDigitalOutput()   do { TRISCbits.TRISC2 = 0; } while(0)
#define RC2_SetPullup()             do { WPUCbits.WPUC2 = 1; } while(0)
#define RC2_ResetPullup()           do { WPUCbits.WPUC2 = 0; } while(0)
#define RC2_SetAnalogMode()         do { ANSELCbits.ANSELC2 = 1; } while(0)
#define RC2_SetDigitalMode()        do { ANSELCbits.ANSELC2 = 0; } while(0)

/**
   @Param
    none
   @Returns
    none
   @Description
    GPIO and peripheral I/O initialization
   @Example
    PIN_MANAGER_Initialize();
 */
void PIN_MANAGER_Initialize (void);

/**
 * @Param
    none
 * @Returns
    none
 * @Description
    Interrupt on Change Handling routine
 * @Example
    PIN_MANAGER_IOC();
 */
void PIN_MANAGER_IOC(void);



#endif // PIN_MANAGER_H
/**
 End of File
*/