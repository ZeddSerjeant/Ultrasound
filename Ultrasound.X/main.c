
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
bit led_stay_on = OFF; // for indicating a solid light rather than something with a duty cycle
unsigned int led_duty_cycle = 0; // Duty cycle of LED as on_time[ms]
const unsigned int led_period = 1000; // period of flashing LED [ms]
unsigned int led_duty_cycle_counter = 0;

const unsigned int ping_delay = 20; //[ms]

unsigned char timer0_initial= 0; // for timing smaller than a single time loop.
unsigned char timer0_delay = 0; // for times longer than the timer itself
unsigned char delay_count = 0; // for use with the above

bit device_state = 0; // pressing the button alters state
const unsigned char button_bounce = 200; // a number of interrupts to allow ignoring button bounce [ms]
unsigned char button_bounce_count = 0; // to increment to this value

bit do_calcs = 0; // for the ping cycle to indicate it has captured something so we can make calculations

union time // a union is used for ease of adjusting the range whiles assigning the values to the timer
{
	unsigned short int range;
	struct
	{
		unsigned char low_byte;
		unsigned char high_byte;
	};
} range_to_target; // this represents the value to initialise timer1 to, so that it counts down approximately the value after the subtraction. This is the range as this is increased to search further away, and will represent the time/distance to the object when it is found
const unsigned char range_band = 20; // [us] to begin with, I'm doing a linear search which will proceed in steps of this
const unsigned short int initial_range = 0xFFFF - 198; // [us] ~33mm from beginning the range from the transducer to begin searching for objects

// const unsigned short int min_range = 0xFFFF - 343; // 10cm round trip, so 5cm from beginning
// const unsigned short int max_range = 0xFFFF - 488; // 16cm, 8cm from beginning

const unsigned short int min_range = 0xFFFF - 874; // [us] the min range, about 150mm 
const unsigned short int max_range = 0xFFFF - 3499; // [us] the max range, about 600mm 
unsigned short int read_threshold = 0; // XXX the threshold for the previous variable
unsigned short int receiver_dc_offset = 0; // set on calibration

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
	GPIO = CLEAR; // clear all outputs

	//reset all main variables
	led_duty_cycle = 0;
	led_stay_on = 0;
	led_state = 0;
	range_to_target.range = initial_range;

	ADC_CHANNEL1 = 1; ADC_CHANNEL0 = 1; // Set the channel to AN3 (where the POT is)
	PAUSE1; // give the adc time to point at the new channel
	ADC_GODONE = ON; // begin a conversion
	
	while (ADC_GODONE); // wait till its done

	read_threshold = (unsigned short int)ADC_RESULT_HIGH; // store a new threshold based on this value
	read_threshold = read_threshold*read_threshold; // square the value for comparison with magnitude squared later, with 20**2 for conversion to mv

	ADC_CHANNEL1 = 1; ADC_CHANNEL0 = 0; // Set the channel back to AN2 (where the receiver is)
	PAUSE1; // give the adc time to point at the new channel
	ADC_GODONE = ON; // begin a reading of the ADC, to set the midpoint of the receiver
	while (ADC_GODONE); // wait till its done
	
	receiver_dc_offset = (unsigned short int)ADC_RESULT_HIGH; // store the offset
}

unsigned short int rangeToDuty(unsigned short int range) // converts a time delay in to a duty cycle
{		
	range = 0xFFFF - range; // remove the offset needed for the internal clock
	range = (range * 343)/1000; // distance in mm
	range = range/2; // this distance was for a round trip of the signal, its actually only half as far away
	
	// return ((range-5)*500)/495; // TTT for the distances involved with my test set up
	return ((range-150)*500)/450; // convert to duty cycle as a proportion of the range
}

void main() 
{	
	CORE_CLOCK = 0x6B; // set the clock difference manually XXX change this if the chip changes
	//set up ping
	unsigned int ping_delay_count = ping_delay;
	T1_PIN1 = OUTPUT; // first transmit pin is output
	T1_PIN2 = OUTPUT; // second transmit pin is output
	RECEIVER_PIN = INPUT;
	RECEIVER_ADC = ON; // enable this to be used with the ADC

    // Set up timer0
    // calculate intial for accurate timing $ inital = TimerMax-((Delay*Fosc)/(Prescaler*4))
    // This shrinks timing smaller than directly prescaling for when thats necessary
    timer0_initial = 118; // the interrupt triggers every 256-this
    timer0_delay = 1; // for 1ms and prescaler of 1:8 (adjusted empirically)
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
    unsigned long int magnitude1, magnitude2, magnitude3;
    range_to_target.range = initial_range; // begin scanning at 61us which is approximately a 10mm difference
    
    //calibration
    TIMER1 = ON;
    while (!TIMER1_INTERRUPT_FLAG); // wait a couple hundred us so the device is ready
    TIMER1_INTERRUPT_FLAG = 0;

    runCalibration(); // pull a threshold from the POT and set the DC bias
					    
   	GLOBAL_INTERRUPTS = ON;






   	//runtime
    while (1)
    {
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

    	if (!ping_delay_count) // is it time to transmit a ping?
    	{ 
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
	    	readings[0] = ADC_RESULT_HIGH; //2us

	    	// second sample
	    	SAMPLE_PAUSE; // XXX apparently mine is going very fast????
	    	ADC_GODONE = ON; // begin a reading 1us
	    	// LED = OFF; //TTT see when the samples are
	    	while(ADC_GODONE); // wait for the remaining time till we get the ADC reading 22us+3us 
	    	readings[1] = ADC_RESULT_HIGH; //2us
	    	
	    	// // third sample
	    	// SAMPLE_PAUSE;
	    	// ADC_GODONE = ON; // begin a reading 1us
	    	// // LED = ON; //TTT see when the samples are
	    	// while(ADC_GODONE); // wait for the remaining time till we get the ADC reading 22us+3us 
	    	// readings[2] = ADC_RESULT_HIGH; //2us

	    	// // fourth sample
	    	// SAMPLE_PAUSE; // XXX apparently mine is going very fast????
	    	// ADC_GODONE = ON; // begin a reading 1us
	    	// // LED = OFF; //TTT see when the samples are
	    	// while(ADC_GODONE); // wait for the remaining time till we get the ADC reading 22us+3us 
	    	// readings[3] = ADC_RESULT_HIGH; //2us


	    	// clean up
	    	ping_delay_count = ping_delay;
	    	GLOBAL_INTERRUPTS = ON; // turn these back on

	    	//calculations can now happen
	    	// do_calcs = ON;
	    	// calculate the magnitude as the square sum of the samples, removing the dc offset. It is done like this to save memory
	    	magnitude1 = (unsigned long int)(((readings[0]-receiver_dc_offset)*(readings[0]-receiver_dc_offset))+((readings[1]-receiver_dc_offset)*(readings[1]-receiver_dc_offset)));
	    	//ideally these are 90deg apart because of the previous code, and so RSS should give us amplitude
	    	// this is between the sample 2 and 3, since there are 4 readings all 1.25 cycles apart, I can convert that two 3 magnitudes
	    	// magnitude2 = (unsigned long int)(((readings[1]-receiver_dc_offset)*(readings[1]-receiver_dc_offset))+((readings[2]-receiver_dc_offset)*(readings[2]-receiver_dc_offset)));
	    	// magnitude3 = (unsigned long int)(((readings[2]-receiver_dc_offset)*(readings[2]-receiver_dc_offset))+((readings[3]-receiver_dc_offset)*(readings[3]-receiver_dc_offset)));;
    		
			if (magnitude1 >= read_threshold) //  this is the rough beginning of the envelope of the wave, so we have found a distance
			{
				// led_duty_cycle = (range_to_target.range / 2)*10; // a rough output for verification purposes
				if (range_to_target.range >= min_range)
				{
					led_stay_on = 1;
					led_duty_cycle = 0;
				}
				else
				{
					led_stay_on = 0;
					led_duty_cycle = rangeToDuty(range_to_target.range);
				}
				range_to_target.range = initial_range; // reset search
			}
			else //  still need to keep searching
			{
				range_to_target.range -= range_band; // search the next range band
				
				if (range_to_target.range <= max_range)
				{
					range_to_target.range = initial_range; //reset the search to within 5cm
					led_stay_on = 0;
					led_duty_cycle = 0;
				}
			}

	    	
  		
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
   		// TTT rewrite to ignore period, we need absolutes to obtain test values
   		// if (led_duty_cycle_counter >= led_duty_cycle)
   		// {
   		// 	led_state = OFF;
   		// }
   		// else
   		// {
   		// 	led_state = ON;
   		// }
   		if (led_duty_cycle_counter >= led_duty_cycle)
   		{
   			if (led_duty_cycle_counter >= led_period)
   			{
   				led_duty_cycle_counter -= led_period; //reset led counter safely
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

   		if (led_stay_on)
   		{
   			LED = ON;
   		}
   		else
   		{
   			LED = led_state;
   		}

   // 		//calculations
   // 		if (do_calcs)
  	// 	{
  			

			// do_calcs = OFF;
  	// 	}
    }
    
    return;
}
