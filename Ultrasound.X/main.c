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

#include <xc.h>


enum FLAGS {OFF=0, CLEAR=0, ON=1}; // Constants (timers are set to 0 for internal clocks)

#define LED_PIN TRISIO5
#define LED GPIO5


void main() 
{
    // TRISIO = 0xDF; // make GP5 an output using full byte (faster to set multiple things)
	LED_PIN = 0; // Set GP5 to output directly. Slower with more outputs, but more readable
    LED = OFF; // Initalize LED as off
    
   	TMR0 = 0; // clear counter
   	T0CS = 0; // internal clock
   	PSA = 0; // enable prescaler for Timer0
   	PS2=1; PS1=1; PS0=1; // Set prescaler to 1:256
   	


   	const unsigned char COUNT = 8; // value to achieve 500ms timing
   	unsigned char counter=0; // counter for accurate timing. Once the timer triggers, counter counts up to COUNT for the left over time.
    while (1)
    {
   		while (!T0IF); //waits till overflow triggers, when the timer0 counts down
   		T0IF = CLEAR; // clear the overflow bit so the loop can repeat
    	// LED = ~LED; // takes approximately 3 clockcycles of 1us each
   		counter++; // incrememnt the counter as per its definition
   		if (counter==COUNT)
   		{
   			counter = 0; // reset for next loop
   			LED = ~LED;
   		}
    }
    return;
}
