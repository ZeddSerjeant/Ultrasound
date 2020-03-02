/*
 * File:   ec_main.c
 * Author: chris
 *
 * Created on 31 August 2017, 10:44 AM
 */

//#define ADC_BIT_GO 1
//#define VREF 3.3


unsigned char ADCON_STORE = 0x0;
unsigned char ADCON_0_STORE = 0x0;
/*  ADCON0 SETUP
 * 
 *  bit 7 ADFM: A/D Conversion Result Format Select bit
 *      1 = Right justified     -upper 8bits in ADRESH, lower 2 in ADRESL
 *      0 = Left justified      -upper 2bits in ADRESH, upper 2 in ADRESL
 *  bit 6 VCFG: Voltage Reference bit
 *      1 = VREF pin
 *      0 = VDD
 *  bit 5 Unimplemented: Read as ?0?
 *  bit 4-2 CHS<2:0>: Analog Channel Select bits
 *      000 = AN0
 *      001 = AN1
 *      010 = AN2
 *      011 = AN3
 *      100 = AN4
 *      101 = AN5
 *      110 = AN6
 *      111 = AN7
 *  bit 1 GO/DONE: A/D Conversion Status bit
 *      1 = A/D conversion cycle in progress. Setting this bit starts an A/D conversion cycle.
 *          This bit is automatically cleared by hardware when the A/D conversion has completed.
 *      0 = A/D conversion completed/not in progress
 *  bit 0 ADON: ADC Enable bit
 *      1 = ADC is enabled
 *      0 = ADC is disabled and consumes no operating current
 */

int updateEC(){
    // ADCON_STORE |= (1 << ADC_BIT_GO);
    //ADCON0 = ADCON_STORE;   //request a new reading

    //wait for the conversion to be completed
    // while (ADCON_0_STORE)
    // {
    //     ADCON_0_STORE = ADCON0;
    //     ADCON_0_STORE = ADCON0 &= ~(1 << ADC_BIT_GO);
    //     ADCON0 = ADCON_0_STORE;

    // }

    //float ECvoltage;
    
    int ADCaverage = 0;
    int ADCresult = 0;

    ADCON_0_STORE = ADCON0 | 0x02;
    ADCON0 = ADCON_0_STORE;
    __delay_us(100);

    // Conversion is done
    //get 10bit result of A/D conversion
    ADCresult |= (ADRESH << 8);    //copy upper 2 bits in
    ADCresult |= ADRESL;     

    // for(int i=0; i < 10; i++){

    //     ADCON_0_STORE = ADCON0 | 0x02;
    //     ADCON0 = ADCON_0_STORE;

    //     __delay_us(100);


    //     // Conversion is done

    //     //get 10bit result of A/D conversion
    //     int ADCresult = 0;
    //     ADCresult |= (ADRESH << 8);    //copy upper 2 bits in
    //     ADCresult |= ADRESL;         //shift upper 2 bits in
        
    //     ADCaverage += ADCresult;
    // }

    // ADCaverage /= 10;
    
    //ECvoltage = ((ADCres / 1023) * VREF);
    //return ((float)(ADCres) / 1023) * VREF;
    return (ADCresult);
}

int calculateSalinity(int ADCvalue)
{
    // 500: 000%
    // 730: 100%
    // Terrible lack of data, but 500 is pure water, 730 is 100% saturate
    long temp = (435*(long)ADCvalue)/1000;
    int salinity = (int)temp - 217;
    if (salinity < 0)
    {
        salinity = 0;
    }
    else if (salinity > 100)
    {
        salinity = 100;
    }
    return salinity;

    //return (536*ADCvalue)/10000 - 40;
    // return (int)((0.0536*ADCvalue) + 40.406);
}
