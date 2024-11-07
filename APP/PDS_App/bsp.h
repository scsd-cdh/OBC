
//*******************************************************************************
// Device Configuration, Pinout and defines *************************************
//*******************************************************************************

#define SLAVE_ADDR                  0x08  /**< I2C Slave Address for the MSP430 device */

/**
 * @brief Initializes the system clock to 16MHz to support I2C and other peripherals.
 */
void initClockTo16MHz();

/**
 * @brief Configures GPIO pins for I2C communication and debugging (LED indicators).
 */
void initGPIO();

/**
 * @brief Initializes the I2C module in slave mode with the specified slave address.
 */
void initI2C();

