#ifndef ADC_H_
#define ADC_H_

// Library
#include <msp430.h>

extern void intializeADC();
extern void enableADC();
extern void startADC();
extern void stopADC();
extern int readADC();

#endif /* ADC_H_ */
