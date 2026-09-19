#ifndef MSP_UTILS
#define MSP_UTILS

/**
 * Shared utility functions for MSP peripherals
 */

/**
 * Initialize i2c module. If target is MSP430FR6989 or MSP4305989, use RTC_C, else RTC_B
 */
void rtc_init();

/**
 * Initialize on chip clock to 16mhz. (Apparently) Necessary for i2c slave
 */ 
void clock_init_16mhz();

#endif // MSP_UTILS
