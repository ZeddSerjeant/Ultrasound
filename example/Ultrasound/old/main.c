/* PIC functionality Attempts*/

//#pragma config WDTE = OFF       // Watchdog Timer Enable bit (WDT enabled)




#include <pic.h>
#include <xc.h>
#include <stdlib.h>

// Config
#pragma config FOSC = INTRCIO   // Oscillator Selection bits (INTOSC oscillator: I/O function on GP4/OSC2/CLKOUT pin, I/O function on GP5/OSC1/CLKIN)
#pragma config WDTE = OFF       // Watchdog Timer Enable bit (WDT disabled)
#pragma config PWRTE = OFF      // Power-Up Timer Enable bit (PWRT disabled)
#pragma config MCLRE = OFF      // GP3/MCLR pin function select (GP3/MCLR pin function is digital I/O, MCLR internally tied to VDD)
#pragma config BOREN = OFF      // Brown-out Detect Enable bit (BOD disabled)
#pragma config CP = OFF         // Code Protection bit (Program Memory code protection is disabled)
#pragma config CPD = OFF        // Data Code Protection bit (Data memory code protection is disabled)

#define LED GPIO5
int led_on = 0;
int start_counter = 1000000;
int led_counter = 0;

void main()
{   
    // Initialize XXX convert to C eventually
    asm("call 0x3FF");
    RP0=1; // use Bank 1 for registers // asm("bsf STATUS,5");
    asm("movwf OSCCAL");

    VRCON = 0X00; // turn off voltage reference
    CMCON = 0X07; // disable comparator

    INTCON = 0x00; // disable all interrupts
    T0IE = 1; // enable timer0 interrupt

    TRISIO = 0x0; // Set all io as output
    GPIO = 0x00; // set all GPIO to low

   

    //timer0
    T0CS = 0; // Use internal clock for the timer
    PSA = 0; // prescaler is used by Timer0
    // set the prescaler to be 1:128
    PS2 = 1;
    PS1 = 1;
    PS0 = 1;



    GIE = 1; // enable all interrupts

    while (1) // flash LED
    {
        if (led_on) // && (led_counter < start_counter)
        {
            LED = 1; // turn on the LED
            //led_counter++;
        }
        else
        {
            LED = 0;
            //led_counter = 0;
            //LED = 0;
        }
    }
}

void interrupt ISR()
{
    T0IF = 0;//clear the overflow bit
    TMR0 = 0;

    led_on ^= 1;
    // if (T0IF)
    // {
    //     led_on = 1;
    //     led_counter = 0;
    // }
    return;
}

