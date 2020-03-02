/*
 * File:   main.c
 * Author: plivai
 *
 * Created on 9 April 2018, 10:28 PM
 */


#include <xc.h>

// Config
#pragma config FOSC = INTOSCIO  // Oscillator Selection bits (INTOSCIO oscillator: I/O function on RA4/OSC2/CLKOUT pin, I/O function on RA5/OSC1/CLKIN)
#pragma config WDTE = OFF       // Watchdog Timer Enable bit (WDT disabled)
#pragma config PWRTE = OFF      // Power-up Timer Enable bit (PWRT disabled)
#pragma config MCLRE = ON       // MCLR Pin Function Select bit (MCLR pin function is MCLR)
#pragma config CP = OFF         // Code Protection bit (Program memory code protection is disabled)
#pragma config CPD = OFF        // Data Code Protection bit (Data memory code protection is disabled)
#pragma config BOREN = OFF      // Brown Out Detect (BOR disabled)
#pragma config IESO = OFF       // Internal External Switchover bit (Internal External Switchover mode is disabled)
#pragma config FCMEN = OFF      // Fail-Safe Clock Monitor Enabled bit (Fail-Safe Clock Monitor is disabled)

//declarations of input aliases XXX actually map these to something useful
#define LED1 RC2
#define LED2 RC1
#define LED3 RC0
#define LED4 RA2


#define pressure_input 0
#define set_point 10 // this needs to be adjusted to be a useful value that actually reflects the water column


// global PI constants XXX need actual values
char k_p = 0; // Proportional control
char k_i = 0; // Integral control

int readPressure()
{
    return pressure_input;
}

// The tuning program for using POTs to set the values of k_p and k_i to sensible values. Seems like the easiest way to develop the system.
void initializePI()
{
    //XXX Do it!
}


void main() 
{
    //Set up ports
    TRISA = 0x00;
    PORTA = 0x00;
    //Set up PWM
    //Set up sensors
    //set up timer for events

    //PID variables XXX discover values for these, either via trial and error or by a tuning circuit
    int error = 0; // difference between the wanted height and the current height
    // char k_p = 0; //XXX Probably won't implement Derivative control
    int accumulator = 0;

    int motor_speed = 0; // speed to drive the motor at to give a certain amount of flow that counteracts the outflow in the water column

    
    while (1) // mainloop where PI Values are evaluated XXX Proportional only right now
    {
        PORTC ^= LED1; // toggle LED1, to show we are at least 
        
        error = set_point - readPressure(); // I think I need to ensure this isn't 0, as I don't think the motor can go slowly

        motor_speed = k_p * error; // Proportional
        /*
        accumulator += error; // save the error
        motor_speed += k_i * accumulator;
        */


        __delay_ms(100); // We don't to adjust too often

    }
    return;
}
