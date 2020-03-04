#include <xc.h>

enum FLAGS {OFF=0, CLEAR=0, INTERNAL=0, OUTPUT=0, INPUT=1, ON=1}; // Constants (timers are set to 0 for internal clocks)

// more readable
#define PRESCALER PSA
#define GLOBAL_INTERRUPTS GIE

//TIMER0
#define TIMER0_COUNTER TMR0
#define TIMER0_CLOCK_SCOURCE T0CS
#define TIMER0_INTERRUPT T0IE
#define TIMER0_INTERRUPT_FLAG TMR0IF

//Ultrasonic Transducer
#define T11 GPIO0
#define T1_PIN1 TRISIO0
#define T12 GPIO1
#define T1_PIN2 TRISIO1

#define RECEIVER GPIO2
#define RECEIVER_PIN TRISIO2

//Button
#define BUTTON GPIO3
#define BUTTON_PIN TRISIO3 
#define BUTTON_INTERRUPT IOC3
#define BUTTON_INTERRUPT_FLAG GPIF

// LED based on board
#define LED GPIO5
#define LED_PIN TRISIO5

//Potentiometer
#define POT GPIO4
#define POT_PIN TRISIO4

//

