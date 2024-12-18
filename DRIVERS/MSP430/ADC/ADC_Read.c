/*
 * This file contains a standard ADC initialising function (with standard parameters)
 * and a ADC_PinSelect() function which sets whatever pin you want to be an ADC pin, this function
 * requires you to also assign a memory register to hold the value of the ADC in that pin,
 * this value is in the form "ADC12_B_MEMORY_x" where 0 <= x <= 31 such that you can create multiple ADC pins.
 * For example, you can assign pin P1_3 to ADC12_B_MEMORY_0 and P2_4 to ADC12_B_MEMORY_1.
 * IT IS HIGHLY RECOMMENDED TO START AT 0 AND ASSIGN INCREMENTALLY.
 *
 * Finally, a Read_ADC() function, where the memory register you have set before is passed as a parameter,
 * is used to return the results. For example, Read_ADC(ADC12_B_MEMORY_0) will return the ADC at P1_3 (as arbitrarily set above)
 *
 */

#include "ADC_Read.h"

void ADC_init_Standard()
{
    ADC12_B_initParam adcParams = {
        .sampleHoldSignalSourceSelect = ADC12_B_SAMPLEHOLDSOURCE_SC,
        .clockSourceSelect = ADC12_B_CLOCKSOURCE_SMCLK,
        .clockSourceDivider = ADC12_B_CLOCKDIVIDER_1,
        .clockSourcePredivider = ADC12_B_CLOCKPREDIVIDER__1,
        .internalChannelMap = ADC12_B_NOINTCH
    };
    ADC12_B_init(ADC12_B_BASE, &adcParams);

    ADC12_B_enable(ADC12_B_BASE);

    ADC12_B_setupSamplingTimer(ADC12_B_BASE,
        ADC12_B_CYCLEHOLD_16_CYCLES,
        ADC12_B_CYCLEHOLD_16_CYCLES,
        ADC12_B_MULTIPLESAMPLESDISABLE);
}

void ADC_PinSelect(ADC_Pin pin, uint8_t memoryBufferIndex){
    ADC12_B_configureMemoryParam memoryParams = {
        .memoryBufferControlIndex = memoryBufferIndex,
        .inputSourceSelect = pin,
        .refVoltageSourceSelect = ADC12_B_VREFPOS_AVCC_VREFNEG_VSS,
        .endOfSequence = ADC12_B_NOTENDOFSEQUENCE,
        .windowComparatorSelect = ADC12_B_WINDOW_COMPARATOR_DISABLE,
        .differentialModeSelect = ADC12_B_DIFFERENTIAL_MODE_DISABLE
    };

    ADC12_B_configureMemory(ADC12_B_BASE, &memoryParams);
}

uint16_t Read_ADC(uint8_t memoryBufferIndex){
    ADC12_B_startConversion(ADC12_B_BASE,
        ADC12_B_START_AT_ADC12MEM0,
        ADC12_B_SINGLECHANNEL);

    while (ADC12_B_isBusy(ADC12_B_BASE));

    volatile uint16_t result = ADC12_B_getResults(ADC12_B_BASE, memoryBufferIndex);

    return result;
}

uint16_t Read_ADC_Voltage(uint8_t memoryBufferIndex){
    volatile uint16_t result = Read_ADC(memoryBufferIndex) * (ADC12_B_VREFPOS_AVCC_VREFNEG_VSS * 1000) / 4096;
    return result;
}
