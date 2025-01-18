
//*******************************************************************************
// Device Configuration, Pinout and defines *************************************
//*******************************************************************************

#ifndef _BSP_
#define _BSP_

#define SLAVE_ADDR                  0x08  /**< I2C Slave Address for the MSP430 device */

#define CONV_RUN_A_PORT             ( GPIO_PORT_P1 )
#define CONV_RUN_A_PIN              ( GPIO_PIN5 )
#define CONV_RUN_B_PORT             ( GPIO_PORT_P2 )
#define CONV_RUN_B_PIN              ( GPIO_PIN4 )

void initBsp();

/**
 * @brief Initializes the system clock to 16MHz to support I2C and other peripherals.
 */
void initClockTo16MHz();

/**
 * @brief Configures GPIO pins for I2C communication and debugging (LED indicators).
 */
void initGPIO();

/**
 * @brief Configures RTC pins.
 */
void initRTCB();
#endif // _BSP_
