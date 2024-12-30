/**
 * @file main.c
 * @author Seb Madgwick
 * @brief Main file.
 *
 * Device:
 * PIC10F322
 *
 * Compiler:
 * XC8 v2.46
 */

//------------------------------------------------------------------------------
// Configuration bits

#pragma config FOSC = INTOSC    // Oscillator Selection bits (INTOSC oscillator: CLKIN function disabled)
#pragma config BOREN = ON       // Brown-out Reset Enable (Brown-out Reset enabled)
#pragma config WDTE = SWDTEN    // Watchdog Timer Enable (WDT controlled by the SWDTEN bit in the WDTCON register)
#pragma config PWRTE = OFF      // Power-up Timer Enable bit (PWRT disabled)
#pragma config MCLRE = ON       // MCLR Pin Function Select bit (MCLR pin function is MCLR)
#pragma config CP = OFF         // Code Protection bit (Program memory code protection is disabled)
#pragma config LVP = ON         // Low-Voltage Programming Enable (Low-voltage programming enabled)
#pragma config LPBOR = ON       // Brown-out Reset Selection bits (BOR enabled)
#pragma config BORV = LO        // Brown-out Reset Voltage Selection (Brown-out Reset Voltage (Vbor), low trip point selected.)
#pragma config WRT = OFF        // Flash Memory Self-Write Protection (Write protection off)

//------------------------------------------------------------------------------
// Includes

#include "Delay.h"
#include <stdbool.h>
#include "UartTX.h"
#include <xc.h>

//------------------------------------------------------------------------------
// Functions

void main(void) {

    // Select 16 MHz oscillator
    OSCCONbits.IRCF = 0b111;

    // Configure TX output
    ANSELAbits.ANSA1 = 0;
    TRISAbits.TRISA1 = 0;
    LATAbits.LATA1 = 1;

    // Enable ADC
    ADCONbits.ADCS = 0b101; // TAD = 1 us
    ADCONbits.ADON = 1;
    ADCONbits.CHS = 0b010; // AN2

    // Print firmware version
    Delay(DelayPeriod512ms);
    UartTXString("x-IMU3-SA-Light v1.0.2\n");

    // Main program loop
    while (true) {

        // Read ADC
        uint16_t adc = 0;
        for (uint8_t sample = 0; sample < 64; sample++) {
            ADCONbits.GO_nDONE = 1;
            while (ADCONbits.GO_nDONE == 1);
            adc += ADRES;
        }
        adc /= 64;

        // Write message
        UartTXByte('0');
        UartTXByte('.');
        uint16_t value = adc * (10000 / 255);
        value %= 10000;
        UartTXByte('0' + (uint8_t) (value / 1000));
        value %= 1000;
        UartTXByte('0' + (uint8_t) (value / 100));
        value %= 100;
        UartTXByte('0' + (uint8_t) (value / 10));
        UartTXByte('\n');

        // Wait remaining sample period
        Delay(DelayPeriod8ms);
    }
}

//------------------------------------------------------------------------------
// End of file
