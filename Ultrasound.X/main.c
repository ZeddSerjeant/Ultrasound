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

unsigned char led_state = ON; // for ease of toggling
unsigned int led_duty_cycle = 222; // Duty cycle of LED as on_time[ms]
unsigned int led_period = 500; // period of flashing LED [ms]
unsigned int led_duty_cycle_counter = 0; 

unsigned char timer0_initial= 0; // for timing smaller than a single time loop.
unsigned char timer0_delay = 0; // for times longer than the timer itself
unsigned char delay_count = 0; // for use with the above

void interrupt ISR()
{
	if (TIMER0_INTERRUPT_FLAG) // if the timer0 interrupt flag was set (timer0 triggered)
	{
		TIMER0_INTERRUPT_FLAG = 0; // clear interrupt flag since we are dealing with it
		delay_count++; // increment time delay
		TIMER0_COUNTER = timer0_initial + 2; // reset counter, but also add 2 since it takes 2 clock cycles to get going
		
	}
}

void main() 
{
    // TRISIO = 0xDF; // make GP5 an output using full byte (faster to set multiple things)
	LED_PIN = 0; // Set GP5 to output directly. Slower with more outputs, but more readable
    LED = led_state; // Initalize LED

    //calculate intial for accurate timing $ inital = TimerMax-((Delay*Fosc)/(Prescaler*4))
    // This shrinks timing smaller than directly prescaling for when thats necessary
    timer0_initial = 220;
    timer0_delay = 4; // for 1ms and prescaler of 1:8 (adjusted empirically)
    
   	TIMER0_COUNTER = timer0_initial; // set counter
   	TIMER0_CLOCK_SCOURCE = INTERNAL; // internal clock
   	PRESCALER = 0; // enable prescaler for Timer0
   	PS2=0; PS1=1; PS0=0; // Set prescaler to 1:8
   	TIMER0_INTERRUPT_ENABLE = ON; // enable timer0 interrupts
   	GLOBAL_INTERRUPTS = ON;

    while (1)
    {
    	if (delay_count >= timer0_delay) // runs approximately 1/1ms
   		{
   			delay_count -= timer0_delay; // reset counter safely
   			led_duty_cycle_counter++; // increment the led counter

   		}
   		if (led_duty_cycle_counter >= led_duty_cycle)
   		{
   			if (led_duty_cycle_counter >= led_period)
   			{
   				led_duty_cycle_counter -= led_period; //reset led counter safely
   				led_state = ON; // we are in the ON part of the duty cycle
   			}
   			else
   			{
   				led_state = OFF;
   			}
   		}
   		else
   		{
   			led_state = ON; // within On part of duty cycle
   		}

   		LED = led_state; // output required voltage
    }
    
    return;
}
