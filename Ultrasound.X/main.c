
// CONFIG
#pragma config FOSC = INTRCIO   // Oscillator Selection bits (INTOSC oscillator: I/O function on GP4/OSC2/CLKOUT pin, I/O function on GP5/OSC1/CLKIN)
#pragma config WDTE = OFF       // Watchdog Timer Enable bit (WDT disabled)
#pragma config PWRTE = OFF      // Power-Up Timer Enable bit (PWRT disabled)
#pragma config MCLRE = OFF      // GP3/MCLR pin function select (GP3/MCLR pin function is MCLR)
#pragma config BOREN = OFF       // Brown-out Detect Enable bit (BOD enabled)
#pragma config CP = OFF         // Code Protection bit (Program Memory code protection is disabled)
#pragma config CPD = OFF        // Data Code Protection bit (Data memory code protection is disabled)

#include "head.h"

unsigned char led_state = OFF; // for ease of toggling
unsigned int led_duty_cycle = 250; // Duty cycle of LED as on_time[ms]
unsigned int led_period = 500; // period of flashing LED [ms]
unsigned int led_duty_cycle_counter = 0;

unsigned char timer0_initial= 0; // for timing smaller than a single time loop.
unsigned char timer0_delay = 0; // for times longer than the timer itself
unsigned char delay_count = 0; // for use with the above

unsigned char device_state = 0; // pressing the button toggles this
unsigned char button_bounce = 200; // a number of interrupts to allow ignoring button bounce [ms]
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
			device_state = ~device_state; // toggle the stored button state so we can have an internal state based on it
			button_bounce_count = button_bounce; // prevent this code from being triggered by the button bounce.
		}
	}  
	if (ADC_INTERRUPT_FLAG)
	{
		ADC_INTERRUPT_FLAG = CLEAR; // we are dealing with the interrupt
		led_duty_cycle = ADC_RESULT_HIGH; // set duty
	}
}

void main() 
{	
	//set up ping
	unsigned int ping_delay = 1000; //[ms] ping ever 1000ms
	unsigned int ping_delay_count = ping_delay;
	T1_PIN1 = OUTPUT; // first transmit pin is output
	T1_PIN2 = OUTPUT; // second transmit pin is output
	RECEIVER_PIN = INPUT;

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

    //set up POT
    POT = OFF;
    POT_PIN = INPUT;
    POT_ADC = ON; // enable the adc on the pin the POT is on

    //Set up ADC
    ADC_VOLTAGE_REFERENCE = INTERNAL;
    ADC_CHANNEL1 = 1; ADC_CHANNEL0 = 1; // Set the channel to AN3 (where the pot is connected)
    ADC_CLOCK_SOURCE2 = 0; ADC_CLOCK_SOURCE1 = 0; ADC_CLOCK_SOURCE0 = 1; // Set the clock rate of the ADC
    ADC_OUTPUT_FORMAT = 0; // Left Shifted ADC_RESULT_HIGH contains the first 8 bits
    ADC_INTERRUPT = ON; // enable interrupts for the ADC
    ADC_ON = ON; // turn it on
    
   	GLOBAL_INTERRUPTS = ON;

    while (1)
    {
    	if (!ping_delay_count) // is it time to transmit a ping?
    	{ 
	    	// Ping code. This is done outside interrupts and loops when it occurs, as timing is crucial
	    	GLOBAL_INTERRUPTS = OFF; // disable interrupts because that would break things in here
	    	
	    	// this happens 3 times hardcoded for speed
	    	GPIO = TRANSMIT_01; // one pin up, the other one down
	    	PING_PAUSE;
	    	GPIO = TRANSMIT_10; // one pin up, the other one down
	    	PING_PAUSE;

	    	// GPIO = TRANSMIT_01;
	    	// PING_PAUSE;
	    	// GPIO = TRANSMIT_10;
	    	// PING_PAUSE;

	    	// GPIO = TRANSMIT_01;
	    	// PING_PAUSE;
	    	// GPIO = TRANSMIT_10;
	    	// PING_PAUSE;

	    	// clean up
	    	ping_delay_count = ping_delay;
	    	GPIO = 0x00; // turn off transmitter
	    	GLOBAL_INTERRUPTS = ON; // turn these back on
    	}

    	// State code
    	// led with duty cycle
    	if (delay_count >= timer0_delay) // runs approximately 1/1ms
   		{
   			delay_count -= timer0_delay; // reset counter safely
   			led_duty_cycle_counter++; // increment the led counter
   			if (button_bounce_count)
			{
				button_bounce_count--; // get closer to point in time that another button press can occur
			}
			if (ping_delay_count)
			{
				ping_delay_count--;
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
   		
   		// State based on button
   		if (device_state == 0) // first state, default
   		{
       		// LED = led_state; // Make the PIN reflect the updated state
   		}
  		else // enter this state when button is pressed
  		{
  			ADC_GODONE = ON; // begin a conversion hopefully
  			device_state = 0; // got back to first state 

  			// led_duty_cycle = 400; //otherwise, when I press the button, we go to a 400ms state
  			// LED = OFF; // if we aren't in the flashing state, just be off
  		}

   		// update hardware
   		LED = led_state;
    }
    
    return;
}
