#include <xc.h>

enum FLAGS {OFF=0, CLEAR=0, INTERNAL=0, OUTPUT=0, INPUT=1, ON=1}; // Constants (timers are set to 0 for internal clocks)

// more readable
#define PRESCALER PSA
#define GLOBAL_INTERRUPTS GIE
// this will take approximately 11 cycles of time
#define PING_PAUSE asm("NOP;");asm("NOP;");asm("NOP;");asm("NOP;");asm("NOP;");asm("NOP;");asm("NOP;");asm("NOP;");asm("NOP;");asm("NOP;");asm("NOP;")
#define CORE_CLOCK OSCCAL 

//TIMER0
#define TIMER0_COUNTER TMR0
#define TIMER0_CLOCK_SCOURCE T0CS
#define TIMER0_INTERRUPT T0IE
#define TIMER0_INTERRUPT_FLAG TMR0IF

//TIMER1
#define TIMER1 TMR1ON
#define TIMER1_PRE0 T1CKPS0
#define TIMER1_PRE1 T1CKPS1
#define TIMER1_INTERRUPT_FLAG TMR1IF
#define TIMER1_COUNTER_LOW TMR1L
#define TIMER1_COUNTER_HIGH TMR1H

//Ultrasonic Transducer
#define T11 GPIO0
#define T1_PIN1 TRISIO0
#define T12 GPIO1
#define T1_PIN2 TRISIO1
#define TRANSMIT_01 0x01
#define TRANSMIT_10 0x02

#define RECEIVER GPIO2
#define RECEIVER_PIN TRISIO2
#define RECEIVER_ADC ANS2

//Button
#define BUTTON GPIO3
#define BUTTON_PIN TRISIO3 
#define BUTTON_INTERRUPT IOC3
#define GPIO_INTERRUPT GPIE
#define BUTTON_INTERRUPT_FLAG GPIF

// LED based on board
#define LED GPIO5
#define LED_PIN TRISIO5

//Potentiometer
#define POT GPIO4
#define POT_ADC ANS3
#define POT_PIN TRISIO4


//ADC
#define ADC_VOLTAGE_REFERENCE VCFG
#define ADC_CLOCK_SOURCE2 ADCS2
#define ADC_CLOCK_SOURCE1 ADCS1
#define ADC_CLOCK_SOURCE0 ADCS0
#define ADC_CHANNEL1 CHS1
#define ADC_CHANNEL0 CHS0
#define ADC_GODONE GO_DONE
#define ADC_OUTPUT_FORMAT ADFM 
#define ADC_RESULT_HIGH ADRESH
#define ADC_RESULT_LOW ADRESL
#define ADC_INTERRUPT ADIE
#define ADC_INTERRUPT_FLAG ADIF
#define ADC_ON ADON