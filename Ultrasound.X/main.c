
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

unsigned char button_pressed = 0; // pressing the button toggles this, rather than this reflecting the actual current state of the button. Probably change this name in the future to actually represent the difference modes I'll program
unsigned char button_bounce = 200; // a number of interupts to allow ignoring button bounce [ms]
unsigned char button_bounce_count = 0; // to increment to this value

void interrupt ISR()
{
	if (TIMER0_INTERRUPT_FLAG) // if the timer0 interrupt flag was set (timer0 triggered)
	{
		TIMER0_INTERRUPT_FLAG = CLEAR; // clear interrupt flag since we are dealing with it
		TIMER0_COUNTER = timer0_initial + 2; // reset counter, but also add 2 since it takes 2 clock cycles to get going
		// move counters, which is the job of this timer interrupt
		delay_count++; // increment time delay
	}
	if (BUTTON_INTERRUPT_FLAG) // if the button has been pressed (Only IO Interrupt set)
	{
		BUTTON_INTERRUPT_FLAG = CLEAR; // we are dealing with it
		if (!BUTTON && !button_bounce_count) // button was pressed and therefore this will read low (and we avoided bounce)
		{
			button_pressed = ~button_pressed; // toggle the stored button state so we can have an internal state based on it
			button_bounce_count = button_bounce; // prevent this code from being triggered by the button bounce.
		}
	}
}

void main() 
{	
    // Set up the timer
    // calculate intial for accurate timing $ inital = TimerMax-((Delay*Fosc)/(Prescaler*4))
    // This shrinks timing smaller than directly prescaling for when thats necessary
    timer0_initial = 220;
    timer0_delay = 4; // for 1ms and prescaler of 1:8 (adjusted empirically)
   	TIMER0_COUNTER = timer0_initial; // set counter
   	TIMER0_CLOCK_SCOURCE = INTERNAL; // internal clock
   	PRESCALER = 0; // enable prescaler for Timer0
    PS2=0; PS1=1; PS0=0; // Set prescaler to 1:8
   	TIMER0_INTERRUPT = ON; // enable timer0 interrupts

   	//Set up IO
   	LED_PIN = OUTPUT; // Set LED (GPIO5) to output directly. Slower with more outputs, but more readable
    LED = led_state; // Initalize LED

    BUTTON_PIN = INPUT;
    BUTTON_INTERRUPT = ON; // enable the pin the button is attached to to interrupt
    GPIO_INTERRUPT = ON; // enable intterupts for all gpio pins


   	GLOBAL_INTERRUPTS = ON;

    while (1)
    {
    	// State code
    	if (delay_count >= timer0_delay) // runs approximately 1/1ms
   		{
   			delay_count -= timer0_delay; // reset counter safely
   			led_duty_cycle_counter++; // increment the led counter
   			if (button_bounce_count)
			{
				button_bounce_count--; // get closer to point in time that another button press can occur
			}

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
   		
   		// Updating hardware code
   		if (button_pressed)
   		{
        	LED = led_state; // Make the PIN reflect the updated state
   		}
   		else
   		{
   			LED = OFF; // if we aren't in the flashing state, just be off
   		}
    }
    
    return;
}
