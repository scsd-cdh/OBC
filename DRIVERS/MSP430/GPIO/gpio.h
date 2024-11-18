

#ifndef _GPIO_
#define _GPIO_

#include <stdint.h>
#include <msp430.h> 

#define CONCATENATE(prefix, num) prefix##num
#define direction(x) CONCATENATE(P, x)DIR
#define port_out(x) CONCATENATE(P, x)OUT
#define set_pin(x) CONCATENATE(BIT, x)


/**
 * @brief Configures GPIO pins for I2C communication and debugging (LED indicators).
 */
void initGPIO();

/**
 * @brief Toggles a GPIO pin for debugging or status indication.
 *
 * @param port GPIO port number
 * @param pin GPIO pin number
 * @param mode 1 to set high, 0 to set low
 */
void ToggleGPIO(uint8_t port, uint8_t pin, uint8_t mode);

#endif // _GPIO_
