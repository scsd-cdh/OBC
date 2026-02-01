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
 * Sets up ADCs, GPIOs, PWM, RTC, and registers telemetry/telecommand channels.

 */
void BMS_init();

/**
 * @brief Synchronous data-acquisition pass that refreshes telemetry buffers.
 *
 * @pre ::BMS_init() has been called.
 * @post Telemetry buffers registered with tinyprotocol contain fresh data.
 */
void BMS_collectData();

/**
 * @brief Check whether an RTC event occurred since the last collection.
 *
 * @return `true` if an RTC event has occurred and data collection is due;
 *         `false` otherwise.
 */
bool BMS_ISRTriggered();

#endif