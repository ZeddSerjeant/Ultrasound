/* PIC functionality Attempts*/

//#pragma config WDTE = OFF       // Watchdog Timer Enable bit (WDT enabled)




// #include <pic.h>
#include <xc.h>
// #include <stdlib.h>

// Config
#pragma config FOSC = INTRCIO   // Oscillator Selection bits (INTOSC oscillator: I/O function on GP4/OSC2/CLKOUT pin, I/O function on GP5/OSC1/CLKIN)
#pragma config WDTE = OFF       // Watchdog Timer Enable bit (WDT disabled)
#pragma config PWRTE = OFF      // Power-Up Timer Enable bit (PWRT disabled)
#pragma config MCLRE = OFF      // GP3/MCLR pin function select (GP3/MCLR pin function is digital I/O, MCLR internally tied to VDD)
#pragma config BOREN = OFF      // Brown-out Detect Enable bit (BOD disabled)
#pragma config CP = OFF         // Code Protection bit (Program Memory code protection is disabled)
#pragma config CPD = OFF        // Data Code Protection bit (Data memory code protection is disabled)

#define LED GPIO5
#define POT GPIO4 // the potentiometer. This is purely to get a value into the system. //AN3?
#define SWITCH GPIO3 // on-off switch

// time defines
#define n_time_on 40 // time for led to spend on
#define n_time_heartbeat 960 // time for led rest
#define n_time_detected 160 // time for led detected
#define ping_time_top 100 // time to take for each ping

// unsigned volatile short int rtmr1 @0x0E;

bit flag_button = 0; // represents a flag set by the button

bit led_on = 0;
char led_counter = 0;

// time counting variables 1tick = 1/(1MHz/4/256)~=1ms

int n_time_off = n_time_heartbeat; // time for led to spend off. This changes depending on signal detection to one of the above 2 variables
volatile int time_count = 0; // the counter that is compared to the above two variables

signed short int dc_offset = 0;

volatile char ping_time_count = 0; // counting to the top

char n_pings = 6; // the number of cycles of a ping to broadcast at once

char delay_time = 0; // allows a delay to occur, by setting this higher than 0

char tmp_GPIO = 0; // Used to save the GPIO State between Ultrasound pulses, otherwise wrong pins are changed

unsigned short long int temp = 0; // used for temporary calculations
unsigned short long int old_mag = 0;
unsigned short long int mag = 0;
short int pulse_sensitivity = 0; // difference in phase before something is considered movement. we don't need degrees, so absolute divisions work

const char n_bins = 4;
// char measurement_delay[n_bins] = {2, 2, 3, 3}; // XXX to modify
// char measurement_index = 0;
char new_measured = 0;
bit measured = 0;



// union ADC_STORE // allows chars to overlap in memory into a 16bit int
// {
//     unsigned short int value; // final 16bit value that combines the two bytes
//     char bytes[2]; // left and right bytes
// } q1, q2; // two points minimum for data capture

char h1, h2;
signed short int a1, a2;

void main()
{
    char i;
    // Initialize XXX convert to C eventually
    asm("call 0x3FF");
    asm("bsf STATUS,5");
    //RP0=1; // use Bank 1 for registers // asm("bsf STATUS,5");
    asm("movwf OSCCAL");

    VRCON = 0X00; // turn off voltage reference
    CMCON = 0X07; // disable comparator

    //interrupts
    INTCON = 0x00; // disable all interrupts
    T0IE = 1; // enable timer0 interrupt
    //PEIE = 1; // enable peripheral interrupts (ADC is a peripheral)
    //ADIE = 1; // enables ADC interruptrs

    TRISIO = 0b00011100; // Set all Set all except GP4(AN3), GP3(Switch), GP2(Transducer) to be outputs
    GPIO = 0x00; // set all GPIO to low
    IOC3 = 1; // enable interrupts on change for GPIO3

    //ADC
    ADFM = 1; // RIGHT justified registers (2MSB in ADRESH, 8LSB in ADRESL)
    VCFG = 0; // Use Vdd as reference
    CHS1 = 1; CHS0 = 0;// (AN3,POT)CHS=0b11; (AN2,Receive)CHS=0b10;
    //ADCS = 0b001; // Set ADC to Fosc/16 1us (XXX Check this)
    ADCS2 = 0;
    ADCS1 = 0;
    ADCS0 = 1;
    ANS3 = 1; // Set AN3 as an analogue input
    ANS2 = 1; // Set AN2 as an analogue input
    ADON = 1; // enable the ADC, GO=1 starts a conversion
   

    //timer0
    T0CS = 0; // Use internal clock for the timer
    PSA = 0; // prescaler is used by Timer0
    //set the prescaler to be 1:4  
    PS2 = 0;
    PS1 = 0;
    PS0 = 1;

    GIE = 1; // enable all interrupts

    //sample to find DC offset
    GO = 1; // fire ADC
    while (GO); // wait until the ADC has a value
    dc_offset = (ADRESH<<8) | ADRESL;

    while (1) // flash LED
    {
        if (led_on) // && (led_counter < start_counter)
        {
            LED = 1; // turn on the LED
            if (time_count > n_time_on) // check to see if we are finished being on
            {
                time_count = 0;
                led_on = 0;
            }
        }
        else
        {
            LED = 0;
            if (time_count > n_time_off)
            {
                time_count = 0;
                led_on = 1;
            }
        }

               

        if (ping_time_count > ping_time_top)
        {
            ping_time_count = 0;

            // if (measurement_index == n_bins)
            // {
                // measurement_index = 0;
            CHS1 = 1; CHS0 = 1;// (AN3,POT)CHS=0b11; (AN2,Receive)CHS=0b10;
            asm("NOP");
            asm("NOP");
            asm("NOP");
            asm("NOP");
            GO = 1;
            while (GO);
            // if ((temp>>3) > ((ADRESH<<8)+ADRESL))
            // {
            //     LED = 1;
            // }
            // else
            // {
            //     LED = 0;
            // }
            //Change ADC back
            pulse_sensitivity = (ADRESH<<8)+ADRESL;
            CHS1 = 1; CHS0 = 0;// (AN3,POT)CHS=0b11; (AN2,Receive)CHS=0b10;
            // }
            GIE = 0; // disable interrupts for this
            tmp_GPIO = GPIO; // save GPIO

            
            // Loop to get x cycles off the transducer
            for (i=0; i<n_pings; i++)
            {
                // asm("NOP");
                // asm("NOP");
                // asm("NOP");
                // asm("NOP");
                //asm("NOP");
                
                GPIO = 0x01;

                asm("NOP");
                asm("NOP");
                asm("NOP");
                asm("NOP");
                asm("NOP");
                asm("NOP");
                asm("NOP");
                asm("NOP");
                asm("NOP");

                GPIO = 0x02;
            }
            GIE = 1;

            delay_time = 2;
            // measurement_index++;

            while (delay_time); // delay for 3 ticks to give time for the ping to reflect back, ignoring transmit echo
            GIE = 0;

            GO = 1; // fire ADC
            while (GO); // wait until the ADC has a value
            // LED = 1;
            asm("NOP");
            asm("NOP");
            // LED = 0;
            GO = 1; // Once it has a value, tell it to fetch another value. This doesn't overwrite current value, and this is done for speed
            h1 = ADRESH;
            h2 = ADRESL;
            while (GO); // wait for current conversion to finish now.

            GPIO = tmp_GPIO; // restore GPIO
            GIE = 1; // reenable interrupts

            a1 = ((signed short int)(((h1<<8)+h2) - dc_offset));
            a2 = ((signed short int)(((ADRESH<<8)+ADRESL) - dc_offset));

            old_mag = mag;
            mag = ((a1*a1) + (a2*a2));
            // if (measurement_index == 0)
            // {
            //     continue;
            // }

            if (old_mag > mag)
            {
                temp = old_mag - mag;
            }
            else if (old_mag < mag)
            {
                temp = mag - old_mag;
            }
            else
            {
                temp = 0;
            }

            if ((temp>>4) > (pulse_sensitivity)) // if the magnitude with sqrt is larger than 70mV, this is a point worth considering, and for now this will just active 
            {
                n_time_off = n_time_detected;
            }
            else
            {
                n_time_off = n_time_heartbeat;
            }
        }
    }
}

void interrupt ISR()
{
    if (T0IF) // timer0 triggered
    {
        T0IF = 0; //clear the overflow bit
        TMR0 = 0; // set the counter back to 0 (XXX maybe not strictly necessary?)
        time_count++;
        ping_time_count++;
        if (delay_time > 0)
        {
            delay_time--; 
        }
    }
    // if (ADIF) // adc triggered
    // {
    //     ADIF = 0; // clear the interrupt flag
    //     if (ADRESH > 125)
    //     {
    //         LED = 1;
    //     }
    //     else
    //     {
    //         LED = 0;
    //     }
    // }
    if (GPIF) // pin change
    {
        if (GPIO3)
        {
            flag_button ^= 1;
        }
        GPIF = 0;
    }
    // return;
}

