#include "adc.h"

void intializeADC(){
    P1SEL0 |= BIT0; // Enable A0 on pin P1.0
    PM5CTL0 &= ~LOCKLPM5; // Disable high Impedance Mode

    /*
     Turn On ADC 12-bit Sample Time
     Reference generator set to 2.5V
    */
    ADC12CTL0 = ADC12ON|ADC12SHT0_2;
    ADC12CTL1 = ADC12SHP;// Use sampling timer
    ADC12CTL2 |= ADC12RES_2;
    ADC12MCTL0 = ADC12VRSEL_4; // Vr+ = VeREF+ (ext) and Vr-=AVss

    // Enable ADC
    enableADC();

}

void enableADC(){
    ADC12CTL0 |= ADC12ENC; // Enable ADC
}

void startADC(){
    ADC12CTL0 |= ADC12SC; // Start Reading Voltages
}
void stopADC(){
    ADC12CTL0 &= ~(ADC12ENC | ADC12SC); // Disable ADC and Stop Reading Voltages
}

int readADC(){
    return ADC12MEM0; // Stored ADC value
}

int readVoltage(){
    return readADC()/4096 * 3300; // Convert ADC to mV
}
