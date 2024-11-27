#include "adc.h"

void intializeADC(){
    P1SEL |= 0x01; // Enable A0 on pin P1.0
    REFCTL0 &= ~REFMSTR; // Reset REFMSTR to hand over control to ADC12_A ref control registers

    /*
     Turn On ADC 12-bit Sample Time
     Reference generator set to 2.5V
    */
    ADC12CTL0 = ADC12ON|ADC12SHT02|ADC12REFON|ADC12REF2_5V;

    ADC12CTL1 = ADC12SHP;// Use sampling timer
    ADC12MCTL0 = ADC12SREF_1;// Vr+=Vref+ and Vr-=AVss

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


