#include <xc.h>

enum FLAGS {OFF=0, CLEAR=0, TIMER_ENABLE=0, INTERNAL=0, OUTPUT=0, ON=1}; // Constants (timers are set to 0 for internal clocks)

// more readable
#define PRESCALER PSA
#define GLOBAL_INTERRUPTS GIE
//TIMER0
#define TIMER0_INTERRUPT_ENABLE T0IE
#define TIMER0_INTERRUPT_FLAG TMR0IF
#define TIMER0_COUNTER TMR0
#define TIMER0_CLOCK_SCOURCE T0CS


// LED based on board
#define LED GPIO5
#define LED_PIN TRISIO5

