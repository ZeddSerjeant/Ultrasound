/*
 * File:   main.c
 * Author: zjtp1
 *
 * Created on 2 March 2020, 12:46 PM
 */

// CONFIG
#pragma config FOSC = INTRCIO   // Oscillator Selection bits (INTOSC oscillator: I/O function on GP4/OSC2/CLKOUT pin, I/O function on GP5/OSC1/CLKIN)
#pragma config WDTE = OFF       // Watchdog Timer Enable bit (WDT disabled)
#pragma config PWRTE = OFF      // Power-Up Timer Enable bit (PWRT disabled)
#pragma config MCLRE = OFF      // GP3/MCLR pin function select (GP3/MCLR pin function is MCLR)
#pragma config BOREN = OFF       // Brown-out Detect Enable bit (BOD enabled)
#pragma config CP = OFF         // Code Protection bit (Program Memory code protection is disabled)
#pragma config CPD = OFF        // Data Code Protection bit (Data memory code protection is disabled)

#include "head.h"

unsigned char led_state = OFF; // for ease of toggling in an interrupt

void interrupt ISR()
{
	if (TIMER0_INTERRUPT_FLAG) // if the timer0 interrupt flag was set (timer0 triggered)
	{
		led_state = ~led_state; // toggle LED
		TIMER0_INTERRUPT_FLAG = 0; // clear interrupt flag since we dealt with it
	}
}

void main() 
{
    // TRISIO = 0xDF; // make GP5 an output using full byte (faster to set multiple things)
	LED_PIN = 0; // Set GP5 to output directly. Slower with more outputs, but more readable
    LED = OFF; // Initalize LED as off
    
   	TIMER0_COUNTER = CLEAR; // clear counter
   	TIMER0_CLOCK_SCOURCE = TIMER_ENABLE; // internal clock
   	PRESCALER = 0; // enable prescaler for Timer0
   	PS2=1; PS1=1; PS0=1; // Set prescaler to 1:256
   	TIMER0_INTERRUPT_ENABLE = ON; // enable timer0 interrupts
   	GLOBAL_INTERRUPTS = ON;

    while (1)
    {
   		LED = led_state;
    }
    return;
}
