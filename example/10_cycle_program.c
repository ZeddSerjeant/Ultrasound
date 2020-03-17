/* This exists to callibrate OSCCAL manually when other methods aren't working */

// CONFIG
#pragma config FOSC = INTRCIO   // Oscillator Selection bits (INTOSC oscillator: I/O function on GP4/OSC2/CLKOUT pin, I/O function on GP5/OSC1/CLKIN)
#pragma config WDTE = OFF       // Watchdog Timer Enable bit (WDT disabled)
#pragma config PWRTE = OFF      // Power-Up Timer Enable bit (PWRT disabled)
#pragma config MCLRE = OFF      // GP3/MCLR pin function select (GP3/MCLR pin function is MCLR)
#pragma config BOREN = OFF       // Brown-out Detect Enable bit (BOD enabled)
#pragma config CP = OFF         // Code Protection bit (Program Memory code protection is disabled)
#pragma config CPD = OFF        // Data Code Protection bit (Data memory code protection is disabled)

#include "head.h"

void main() 
{	
	CORE_CLOCK = 0x6B;
	LED_PIN = OUTPUT; // Set LED (GPIO5) to output directly. Slower with more outputs, but more readable


	while (1)
	{
		asm("NOP;");
		asm("NOP;");
		asm("NOP;");
		asm("NOP;");
		asm("NOP;");
		asm("NOP;");
		asm("NOP;");
		LED = 0;
		asm("NOP;");
		asm("NOP;");
		asm("NOP;");
		asm("NOP;");
		asm("NOP;");
		asm("NOP;");
		asm("NOP;");
		asm("NOP;");
        asm("NOP;");
		LED = 1;
	}

	return;
}