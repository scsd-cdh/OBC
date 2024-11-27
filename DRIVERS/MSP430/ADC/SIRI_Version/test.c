// Library
#include <stdbool.h>
#include <stdio.h>

// Headers
#include "adc.h"


// Task
int readVoltage(){
    return readADC()/4096 * 3300; // Convert ADC to mV
}

int main(void)
{
	WDTCTL = WDTPW | WDTHOLD;	// stop watchdog timer
	
	// Initialize ADC
	intializeADC();

	// Delay
	int i;
	for(i=0;i<30;i++); // Small Delay

	// Read Voltages
	while(true){
	    startADC();
	    printf("A0 Value = %d\n", readADC());
	    printf("A0 Voltage = %d\n mV", readVoltage());
	}
	stopADC();
	return 0;
}
