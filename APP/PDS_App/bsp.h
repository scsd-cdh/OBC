
//*******************************************************************************
// Device Configuration, Pinout and defines *************************************
//*******************************************************************************

#ifndef _BSP_
#define _BSP_

#define SLAVE_ADDR                  0x08  /**< I2C Slave Address for the MSP430 device */

void initBsp();

/**
 * @brief Initializes the system clock to 16MHz to support I2C and other peripherals.
 */
void initClockTo16MHz();

/**
 * @brief Configures GPIO pins for I2C communication and debugging (LED indicators).
 */
void initGPIO();

#endif // _BSP_
