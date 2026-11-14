/**
 * @file
 * @brief Battery Management System (BMS) application interface for MSP430.
 *
 * @author Brendan Kelly
 */

#ifndef BMS_H
#define BMS_H

#include <stdbool.h>


/**
 * @brief Initializes the Battery Management System hardware and communication.
 *
 * Sets up ADCs, GPIOs, PWM, crystal/RTC scheduling, and telemetry/telecommand channels.

 */
void bms_init(void);

/**
 * @brief Handle a pending one-second scan or a scheduled failed-read retry.
 *
 * @pre bms_init() has been called. Run in main, with interrupts enabled.
 * @post Only complete successful scans are published. Failed groups remain pending.
 */
void bms_collectdata(void);

/**
 * @brief Check whether the scheduler has requested a scan or retry.
 *
 * @return `true` if acquisition work is pending;
 *         `false` otherwise.
 */
bool bms_isr_triggered(void);

#endif
