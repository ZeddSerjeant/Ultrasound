
// CONFIG
#pragma config FOSC = INTRCIO   // Oscillator Selection bits (INTOSC oscillator: I/O function on GP4/OSC2/CLKOUT pin, I/O function on GP5/OSC1/CLKIN)
#pragma config WDTE = OFF       // Watchdog Timer Enable bit (WDT disabled)
#pragma config PWRTE = OFF      // Power-Up Timer Enable bit (PWRT disabled)
#pragma config MCLRE = OFF      // GP3/MCLR pin function select (GP3/MCLR pin function is MCLR)
#pragma config BOREN = OFF       // Brown-out Detect Enable bit (BOD enabled)
#pragma config CP = OFF         // Code Protection bit (Program Memory code protection is disabled)
#pragma config CPD = OFF        // Data Code Protection bit (Data memory code protection is disabled)

#include "head.h"

bit led_state = OFF; // for ease of toggling
unsigned int led_duty_cycle = 0; // Duty cycle of LED as on_time[ms]
const unsigned int led_period = 1000; // period of flashing LED [ms]
unsigned int led_duty_cycle_counter = 0;

unsigned char timer0_initial= 0; // for timing smaller than a single time loop.
unsigned char timer0_delay = 0; // for times longer than the timer itself
unsigned char delay_count = 0; // for use with the above

bit device_state = 0; // pressing the button alters state
const unsigned char button_bounce = 200; // a number of interrupts to allow ignoring button bounce [ms]
unsigned char button_bounce_count = 0; // to increment to this value

unsigned char timer_high = 0xFE;
unsigned char timer_low = 0x3D; // so that time delays can be adjusted

unsigned int read_threshold = 0; // XXX the threshold for the previous variable
signed int receiver_dc_offset = 0; // set on calibration

unsigned char readings[2]; // an array for storing ADC results

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
	// if (ADC_INTERRUPT_FLAG)
	// {
	// 	ADC_INTERRUPT_FLAG = CLEAR; // we are dealing with the interrupt
	// 	led_duty_cycle = ADC_RESULT_HIGH; // set duty
	// }
}

void runCalibration() //pull a threshold from the POT and set the DC bias of the receiver
{
	ADC_CHANNEL1 = 1; ADC_CHANNEL0 = 1; // Set the channel to AN3 (where the POT is)
	PAUSE; // give the adc time to point at the new channel
	ADC_GODONE = ON; // begin a conversion
	
	while (ADC_GODONE); // wait till its done

	read_threshold = (unsigned int)ADC_RESULT_HIGH; // store a new threshold based on this value
	read_threshold *= read_threshold;

	ADC_CHANNEL1 = 1; ADC_CHANNEL0 = 0; // Set the channel back to AN2 (where the receiver is)
	PAUSE; // give the adc time to point at the new channel
	ADC_GODONE = ON; // begin a reading of the ADC, to set the midpoint of the receiver
	while (ADC_GODONE); // wait till its done
	
	receiver_dc_offset = (signed int)ADC_RESULT_HIGH; // store the offset
}

void main() 
{	
	//set up ping
	unsigned int ping_delay = 2000; //[ms] ping ever 1000ms
	unsigned int ping_delay_count = ping_delay;
	T1_PIN1 = OUTPUT; // first transmit pin is output
	T1_PIN2 = OUTPUT; // second transmit pin is output
	RECEIVER_PIN = INPUT;
	RECEIVER_ADC = ON; // enable this to be used with the ADC

    // Set up timer0
    // calculate intial for accurate timing $ inital = TimerMax-((Delay*Fosc)/(Prescaler*4))
    // This shrinks timing smaller than directly prescaling for when thats necessary
    timer0_initial = 220; // the interrupt triggers every 
    timer0_delay = 4; // for 1ms and prescaler of 1:8 (adjusted empirically)
   	TIMER0_COUNTER = timer0_initial; // set counter
   	TIMER0_CLOCK_SCOURCE = INTERNAL; // internal clock
   	PRESCALER = 0; // enable prescaler for Timer0
    PS2=0; PS1=1; PS0=0; // Set prescaler to 1:8
   	TIMER0_INTERRUPT = ON; // enable timer0 interrupts

   	// set up timer1
   	TIMER1_COUNTER_HIGH = 0; TIMER1_COUNTER_LOW = 0; // initialise at 0 because we use this for a callibration delay first (65ms)
   	// TIMER1 = ON; // begin a count down

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
    ADC_CHANNEL1 = 1; ADC_CHANNEL0 = 0; // Set the channel to AN3 (where the POT is)
    ADC_CLOCK_SOURCE2 = 0; ADC_CLOCK_SOURCE1 = 0; ADC_CLOCK_SOURCE0 = 1; // Set the clock rate of the ADC
    ADC_OUTPUT_FORMAT = 0; // Left Shifted ADC_RESULT_HIGH contains the first 8 bits
    ADC_INTERRUPT = OFF; // by default these aren't necessary
    ADC_ON = ON; // turn it on

    //set up calc variables
    signed int sample1, sample2; // the 2 sample from which to calculate the magnitude of the wave
    signed long int magnitude;
    
    //calibration
    TIMER1 = ON;
    while (!TIMER1_INTERRUPT_FLAG); // wait a couple hundred us so the device is ready
    TIMER1_INTERRUPT_FLAG = 0;

	TIMER1_COUNTER_HIGH = timer_high; TIMER1_COUNTER_LOW = timer_low;// assign a value, this causes approximately a 200us count
    runCalibration(); // pull a threshold from the POT and set the DC bias
					    
   	GLOBAL_INTERRUPTS = ON;

    while (1)
    {
    	// State based on button
   		if (device_state) // enter this state when button is pressed, recalibrating the device
  		{
  			GLOBAL_INTERRUPTS = OFF;

  			runCalibration();
  			device_state = 0; // got back to first state 

  			GLOBAL_INTERRUPTS = ON;
  		}

    	if (!ping_delay_count) // is it time to transmit a ping?
    	{ 
	    	// Ping code. This is done outside interrupts and loops when it occurs, as timing is crucial
	    	GLOBAL_INTERRUPTS = OFF; // disable interrupts because that would break things in here
	    	
	    	// A single pulse is enough energy for this simple system
	    	GPIO = TRANSMIT_01; // one pin up, the other one down
	    	PAUSE;
	    	GPIO = TRANSMIT_10; // one pin up, the other one down
	    	PAUSE;

	    	// Now wait long enough to read a value. XXX this is a simple test of the ADC
	    	TIMER1_COUNTER_HIGH = timer_high; TIMER1_COUNTER_LOW = timer_low;
	    	TIMER1 = ON; // begin a count down
   			while (!TIMER1_INTERRUPT_FLAG); //wait

   			// first sample 
	    	ADC_GODONE = ON; // begin a reading 1us
	    	LED = ON; //TTT see when the samples are
	    	//reset
	    	TIMER1_INTERRUPT_FLAG = 0;
	    	TIMER1 = OFF;

	    	while(ADC_GODONE); // wait for the remaining time till we get the ADC reading 22us+3us 

	    	readings[0] = ADC_RESULT_HIGH; //2us

	    	// second sample
	    	ADC_PAUSE; // XXX apparently mine is going very fast????
	    	ADC_GODONE = ON; // begin a reading 1us
	    	LED = OFF; //TTT see when the samples are

	    	while(ADC_GODONE); // wait for the remaining time till we get the ADC reading 22us+3us 

	    	readings[1] = ADC_RESULT_HIGH; //2us
	    	


	    	// clean up
	    	ping_delay_count = ping_delay;
	    	GPIO = 0x00; // turn off transmitter
	    	GLOBAL_INTERRUPTS = ON; // turn these back on

	    	//calculations can happen here because now timers can increment
	    	sample1 = (signed int)readings[0] * 19; // convert reading into sample in mV (19 ~ 5000/256)
	    	sample1 -= receiver_dc_offset * 19; // remove dc bias
	    	
	    	sample2 = (signed int)readings[1] * 19; // convert reading into sample in mV
	    	sample2 -= receiver_dc_offset * 19; // remove dc bias

	    	//ideally these are 90deg apart because of the previous code, and so RSS should give us amplitude
	    	magnitude = (signed long int)sample1*sample1 + (signed long int)sample2*sample2;

	    	// if (sample1 > 0)
	    	// {
		    // 	led_duty_cycle = sample1; // show a divided magnitude to see if it is properly proportional
		    // }
		    // else
		    // {
		    // 	led_duty_cycle = -1 * sample1;
		    // }
		    led_duty_cycle = magnitude>>7;
  		
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

   		LED = led_state;
    }
    
    return;
}
