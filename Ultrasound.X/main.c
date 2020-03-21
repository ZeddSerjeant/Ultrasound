
// CONFIG
#pragma config FOSC = INTRCIO   // Oscillator Selection bits (INTOSC oscillator: I/O function on GP4/OSC2/CLKOUT pin, I/O function on GP5/OSC1/CLKIN)
#pragma config WDTE = OFF       // Watchdog Timer Enable bit (WDT disabled)
#pragma config PWRTE = OFF      // Power-Up Timer Enable bit (PWRT disabled)
#pragma config MCLRE = OFF      // GP3/MCLR pin function select (GP3/MCLR pin function is MCLR)
#pragma config BOREN = OFF       // Brown-out Detect Enable bit (BOD enabled)
#pragma config CP = OFF         // Code Protection bit (Program Memory code protection is disabled)
#pragma config CPD = OFF        // Data Code Protection bit (Data memory code protection is disabled)

#include "head.h"

bit led_test_state = OFF; // TTT
bit led_state = OFF; // for ease of toggling
bit reset_led = OFF; // for when we find no object
unsigned short int led_duty_cycle = 0; // Duty cycle of LED as on_time[ms]
// period of flashing LED [ms]
#define LED_PERIOD 500
unsigned short int led_duty_cycle_counter = 0;

//[ms]
#define PING_DELAY 12
unsigned short int ping_delay_count = PING_DELAY;

#define SEARCH_RESET 250
// this number defines the number of pings that occur before the search stops tracking and begins a search from scratch
unsigned char search_count = SEARCH_RESET;

// the number of searches without a peak I'll accept
#define FAILED_SEARCH_LIMIT 5
unsigned char failed_search_count = FAILED_SEARCH_LIMIT; // used for tracking the number of search where I don't find an envelope. If that happens, something has gone drastically wrong

// for timing smaller than a single time loop.
#define TIMER0_INITIAL 118

bit device_state = 0; // pressing the button alters state
// a number of interrupts to allow ignoring button bounce [ms]
#define BUTTON_BOUNCE 200
unsigned char button_bounce_count = 0; // to increment to this value

bit found_a_peak = 0; // set if we find the peak at least once, so that a binary search doesn't fail trivially when there is no object
bit less_than_resolution = 0; // is set if we accept a value less that RESOLUTION, most likely set when the range is less than minimum
union time // a union is used for ease of adjusting the range whiles assigning the values to the timer
{
	unsigned short int range;
	struct
	{
		unsigned char low_byte;
		unsigned char high_byte;
	};
} range_to_target; // this represents the value to initialise timer1 to, so that it counts down approximately the value after the subtraction. This is the range as this is increased to search further away, and will represent the time/distance to the object when it is found


// the smallest step I'm willing to commit to
#define RESOLUTION 12

#define INITIAL_RANGE 300
// [us] ~500mm from beginning the range from the transducer to begin searching for objects
// #define DELTA_RANGE 20 XXX implement this sometime
// [us] offset the initial time by this mis-triggered hard reset, which represents we've found ourselves in a whole

#define MIN_MEASURE_RANGE 910
// [us] the min range, about 150mm 
#define MAX_MEASURE_RANGE 3510
// [us] the max range, about 600mm 

#define MAX_SEARCH_RANGE 3800
// the maximum we will search

unsigned int range_step; // [us] to begin with, I'm doing a linear search which will proceed in steps of this
#define INITIAL_RANGE_STEP 96
///(MAX_MEASURE_RANGE - INITIAL_RANGE)/165

unsigned short int read_threshold = 0; // XXX the threshold for the previous variable
unsigned short int receiver_dc_offset = 0; // set on calibration

union reading // a union combining reading space with the magnitude, since once we have a magnitude, we no longer need the readings. Memory!
{
	// arranged this way for memory locations to match
	unsigned long int magnitude;
	struct
	{
		unsigned char reading2_array[2]; // second adc result
		unsigned char reading1_array[2]; // first adc result.
	};
	struct
	{
		unsigned short int reading2; //reading2_array[1] # reading2_array[0]
		unsigned short int reading1; //reading1_array[1] # reading1_array[0]
	};
} sample;


void interrupt ISR()
{
	LED = 1;
	if (TIMER0_INTERRUPT_FLAG) // if the timer0 interrupt flag was set (timer0 triggered)
	{
		TIMER0_INTERRUPT_FLAG = CLEAR; // clear interrupt flag since we are dealing with it
		TIMER0_COUNTER = TIMER0_INITIAL + 2; // reset counter, but also add 2 since it takes 2 clock cycles to get going
		// move counters, which is the job of this timer interrupt
		led_duty_cycle_counter++; // increment the led counter
		
		if (led_duty_cycle_counter >= led_duty_cycle)
   		{
   			if (led_duty_cycle_counter >= LED_PERIOD)
   			{
   				led_duty_cycle_counter -= LED_PERIOD; //reset led counter safely
   				// led_state = ON; // we are in the ON part of the duty cycle
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
   		
   		// LED = led_state;
   		// LED = led_test_state; //TTT

		// check other timing events
		if (button_bounce_count)
		{
			button_bounce_count--; // get closer to point in time that another button press can occur
		}
		if (ping_delay_count)
		{
			ping_delay_count--;
		}
	}
	if (BUTTON_INTERRUPT_FLAG) // if the button has been pressed (Only IO Interrupt set)
	{
		BUTTON_INTERRUPT_FLAG = CLEAR; // we are dealing with it
		if (!BUTTON && !button_bounce_count) // button was pressed and therefore this will read low (and we avoided bounce)
		{
			device_state = ~device_state; // toggle the stored button state so we can have an internal state based on it
			button_bounce_count = BUTTON_BOUNCE; // prevent this code from being triggered by the button bounce.
		}
	}

	LED = 0;
}

void runCalibration() //pull a threshold from the POT and set the DC bias of the receiver
{
	GPIO = CLEAR; // clear all outputs

	//reset all main variables
	led_duty_cycle = 0;
	led_state = 0;
	range_to_target.range = INITIAL_RANGE;

	ADC_CHANNEL1 = 1; ADC_CHANNEL0 = 1; // Set the channel to AN3 (where the POT is)
	PAUSE1; // give the adc time to point at the new channel
	ADC_GODONE = ON; // begin a conversion
	
	while (ADC_GODONE); // wait till its done

	read_threshold = ADC_RESULT_HIGH<<8 | ADC_RESULT_LOW ; // store a new threshold based on this value
	read_threshold = read_threshold*read_threshold; // square the value for comparison with magnitude squared later, with 20**2 for conversion to mv

	ADC_CHANNEL1 = 1; ADC_CHANNEL0 = 0; // Set the channel back to AN2 (where the receiver is)
	PAUSE1; // give the adc time to point at the new channel
	ADC_GODONE = ON; // begin a reading of the ADC, to set the midpoint of the receiver
	while (ADC_GODONE); // wait till its done
	
	receiver_dc_offset = ADC_RESULT_HIGH<<8 | ADC_RESULT_LOW; // store the offset
}

unsigned short int rangeToDuty(unsigned short int range) // converts a time delay in to a duty cycle
{
	range = 0xFFFF - range; // remove the offset needed for the internal clock
	
	//resusing this variable because space
	// sample.magnitude = range>>1; //divide it by 2
	// sample.magnitude = 666 - (167*(range))/450; // this is guarenteed to be in range

	// return (unsigned short int)sample.magnitude;

	// sample.magnitude = sample.magnitude*333;
	// sample.magnitude = sample.magnitude/1000;
	// if (range < 2700) // less than the half way point
	// {
	// 	range = range>>1; // divide by 2 since this number represents round trip
	// 	range = (range*33)/100; // distance in mm
		
	// 	// sample.magnitude = sample.magnitude*333;
	// 	// sample.magnitude = sample.magnitude/1000;
	// }
	// else // above the half way point
	// {
	// 	range = range>>2; // divide by 2 since this number represents round trip, and then divide by 2 for multiplication, since +-3 doesn't matter
	// 	range = (range*34)/100; // distance in mm
	// 	range<<1; // multiply by 2 to get back the original number
	// 	// sample.magnitude = sample.magnitude*341;
	// 	// sample.magnitude = sample.magnitude/1000;
	// }
	
	// range = (unsigned short int)sample.magnitude;
	// // return range;
	// // return ((range-5)*500)/495; // TTT for the distances involved with my test set up

	range = range/2;
	range = (range*33)/100;
	return ((600-range)*50)/45; // convert to duty cycle as a proportion of the range
}

void main() 
{	
	CORE_CLOCK = 0x6B; // set the clock difference manually XXX change this if the chip changes
	//set up ping
	ping_delay_count = PING_DELAY;
	T1_PIN1 = OUTPUT; // first transmit pin is output
	T1_PIN2 = OUTPUT; // second transmit pin is output
	RECEIVER_PIN = INPUT;
	RECEIVER_ADC = ON; // enable this to be used with the ADC

    // Set up timer0
    // calculate intial for accurate timing $ inital = TimerMax-((Delay*Fosc)/(Prescaler*4))
   	TIMER0_COUNTER = TIMER0_INITIAL; // set counter
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
    ADC_OUTPUT_FORMAT = RIGHT; // right Shifted ADC_RESULT_HIGH contains the first 2 bits
    ADC_INTERRUPT = OFF; // by default these aren't necessary
    ADC_ON = ON; // turn it on

    
    //calibration
    TIMER1 = ON;
    while (!TIMER1_INTERRUPT_FLAG); // wait a couple hundred us so the device is ready
    TIMER1_INTERRUPT_FLAG = 0;

    runCalibration(); // pull a threshold from the POT and set the DC bias

    //set up calc variables
    range_to_target.range = 0xFFFF - INITIAL_RANGE; // begin scan at an initial range, offset from 16bit max for use in the timer1
	range_step = INITIAL_RANGE_STEP; // search the 5 bins, linearly (see later code) initially

   	GLOBAL_INTERRUPTS = ON;






   	//runtime
    while (1)
    {
    	// LED = led_state; // reset this thing
    	// State based on button
   		if (device_state) // enter this state when button is pressed, recalibrating the device
  		{
  			GLOBAL_INTERRUPTS = OFF;
  			TIMER1_COUNTER_HIGH = 0; TIMER1_COUNTER_LOW = 0; // set the timer to 0
  			TIMER1 = ON; // wait for about 65ms
  			while (!TIMER1_INTERRUPT_FLAG);
  			TIMER1_INTERRUPT_FLAG = 0;

  			runCalibration();
  			device_state = 0; // got back to first state 

  			GLOBAL_INTERRUPTS = ON;
  		}

  		// sample.reading1_array[1] = 0; // high
  		// sample.reading1_array[0] = 50; // low
  		// sample.reading2_array[1] = 0; // high
  		// sample.reading2_array[0] = 50; // low
  		// sample.magnitude = sample.reading1 + sample.reading2;

  		// led_duty_cycle = sample.magnitude;

    	if (!ping_delay_count) // is it time to transmit a ping?
    	{ 
    		ping_delay_count = PING_DELAY; // reset delay till next ping

    		if ((!search_count) || (!failed_search_count)) // if our total number of pings before reset is up, or we have failed to find any samples for too long
    		{
    			//reset search parameters
    			search_count = SEARCH_RESET;
    			failed_search_count = FAILED_SEARCH_LIMIT;
    			found_a_peak = 0;
    			range_to_target.range = (0xFFFF - INITIAL_RANGE); //reset the search
				range_step = INITIAL_RANGE_STEP; // reset range steps, just in case

				if (reset_led) // we need to reset things cause we didn't find anything
				{
					led_duty_cycle = 0;
					reset_led = 0;
				}
    		}

    		search_count--; // count another ping

	    	// set needed variables so timing can be accurate in the middle of the ping
	    	TIMER1_COUNTER_HIGH = range_to_target.high_byte; TIMER1_COUNTER_LOW = range_to_target.low_byte; // set the current wait time to check for a value
	 
	    	// Ping code. This is done outside interrupts and loops when it occurs, as timing is crucial
	    	GLOBAL_INTERRUPTS = OFF; // disable interrupts because that would break things in here
	    	
	    	// A single pulse is enough energy for this simple system
	    	TIMER1 = ON; // begin a count down. this happens here, because the wave starts here
	    	GPIO = TRANSMIT_01; // one pin up, the other one down
	    	PAUSE1;
	    	GPIO = TRANSMIT_10; // one pin up, the other one down
	    	PAUSE2;
	    	GPIO = 0; // disable the transmit
	    	// Now wait long enough to read a value.
	    	
   			while (!TIMER1_INTERRUPT_FLAG); //wait

   			// first sample 
	    	ADC_GODONE = ON; // begin a reading 1us
	    	// LED = ON; //TTT see when the samples are
	    	//reset
	    	TIMER1_INTERRUPT_FLAG = 0;
	    	TIMER1 = OFF;
	    	

	    	// take 4 samples so an average can be aquired, and see if there is an increase in the trend so decisions are smoothed
	    	// first sample
	    	while(ADC_GODONE); // wait for the remaining time till we get the ADC reading 22us+3us 
	    	
	    	// second sample
	    	SAMPLE_PAUSE; // XXX apparently mine is going very fast????
	    	ADC_GODONE = ON; // begin a reading 1us
	    	// LED = OFF; //TTT see when the samples are
	    	sample.reading1_array[1] = ADC_RESULT_HIGH;
	    	sample.reading1_array[0] = ADC_RESULT_LOW;
	    	
	    	
	    	
	    	while(ADC_GODONE); // wait for the remaining time till we get the ADC reading 22us+3us 
	    	sample.reading2_array[1] = ADC_RESULT_HIGH;
	    	sample.reading2_array[0] = ADC_RESULT_LOW;

			
			// LED = led_state; // reset led immeditely

	    	GLOBAL_INTERRUPTS = ON; // turn these back on

	    	//calculations can now happen
	    	// calculate the magnitude as the square sum of the samples, removing the dc offset. It is done like this to save memory
	    	sample.magnitude = (unsigned long int)(((sample.reading1-receiver_dc_offset)*(sample.reading1-receiver_dc_offset))+((sample.reading2-receiver_dc_offset)*(sample.reading2-receiver_dc_offset)));
    		
			if (sample.magnitude >= read_threshold) // passing threshold means there is some wave form, search backwards to it find the beginning of the envelope
			{
				// led_test_state = ON; // TTT
				failed_search_count = FAILED_SEARCH_LIMIT; // we found something, so we can stress less about this
				// led_duty_cycle = (range_to_target.range / 2)*10; // a rough output for verification purposes
				if ((0xFFFF - range_to_target.range) < MIN_MEASURE_RANGE) // check if the range (offset from the clock) is less than min range
				{
					found_a_peak = 0; // we don't want to go non-linear here, since we do not resolve to a small amount
					led_duty_cycle = 500; // full led on, as per spec
					less_than_resolution = 1; // we accepted a value that wasn't at full resolution. If we loset the value, we want a full rescan
					// LED = ON; // TTT see where we are finding the object
					// do nothing else as we have found to a close enough resolution for the spec
				}
				else
				{
					found_a_peak = 1; // indicate that this threshold being exceeded happened at least once, outside of the min since we stop going to the smallest resolution once we realize we are there
					// led_test_state = ON;
					if (range_step <= RESOLUTION) // if we are already at the smallest point, so this is the value we want
					{
						if ((0xFFFF - range_to_target.range) >= MAX_MEASURE_RANGE) // if we are beyond max range, this happens here because we could land in the middle of the tail end, and the start, after reaching resolution, may be in range
						{
							led_duty_cycle = 0; // the led needs to be off
						}
						else
						{
							led_duty_cycle = rangeToDuty(range_to_target.range);
						}
						range_to_target.range += range_step; // move backwards in time, just in case the wave form moves
						// LED = ON; // TTT diagnostic, I can see what value this finds to be the peak
					}
					else
					{
						range_step = range_step>>1; // divide range step by 2, for a narrower search
						range_to_target.range += range_step; // move back in time by a range step (which is halved)
					}
				}
			}
			else // no waveform here, so search forwards
			{
				// led_test_state = ON;
				

				if ((0xFFFF - range_to_target.range) >= MAX_SEARCH_RANGE) //if we have gone beyond the limit we care about
				{
					search_count = 0; // force this search cycle to be the reset one
					reset_led = 1; // we found nothing, so set everything to 0
				}

				if (range_step <=  RESOLUTION) // we are at the smallest search size so we missed it slightly, the object is probably at the previous range_step
				{
					range_to_target.range -= range_step; // move forwards in time
					failed_search_count--; // we didn't find anything, keep that in mind as we don't want to do it too much
				}
				else if (found_a_peak) // if a peak was found earlier
				{
					range_step = range_step>>1; // divide range step by 2, for a narrower search
					range_to_target.range -= range_step; // move forward in time by a range step (which is halved)
					failed_search_count--; // we didn't find anything, keep that in mind as we don't want to do it too much
				}
				else // if we are here, we have yet to find a peak at all, so we are linearly searching just in case there is nothing to find
				{
					range_to_target.range -= range_step; // move forward in time by a range step, linearly
				}
				
				if (less_than_resolution) // if we failed to find something while not at full resolution, reset immediately since its easy to vanish
				{
					less_than_resolution = 0;
					search_count = 0;
					// reset_led = 1;
				} 
			}

			// LED = led_state;
    	}

    }
    
    return;
}
