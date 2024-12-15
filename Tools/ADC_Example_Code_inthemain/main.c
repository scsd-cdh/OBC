#include "ADC_Read.c"
#include <msp430.h>


/**
 * main.c
 */
int main(void)
{
	WDTCTL = WDTPW | WDTHOLD;	// stop watchdog timer

	ADC_init_Standard();
	ADC_PinSelect(P1_3,ADC12_B_MEMORY_0);
	uint16_t Result = Read_ADC(ADC12_B_MEMORY_0);

	return 0;
}
