#pragma once

#include <stddef.h>
#include <stdint.h>
#include <zephyr/device.h>
#include <zephyr/kernel.h>


/**
* @brief initializes zephyr UART interrupt driven layer and ring buffer.
*          must be called before uart_transport_read. uart_transport_write is not dependent on this function.
* @param rx_dev pointer to the zephyr device struct for the UART device to read from
* @return 0 on success, negative error code on failure
*/
int uart_transport_open(const struct device *rx_dev);



/**
* @brief reads data from the UART device into a buffer on interrupt. non blocking, puts thread to sleep until data is available or timeout occurs
* @param rxdata pointer to the buffer to read data into
* @param rxlen length of the buffer to read into
* @param timeout maximum time to wait for data to be available
* @return number of bytes read on success, negative error code on failure
*/
int uart_transport_read(
    uint8_t *rxdata,
    size_t rxlen,
    k_timeout_t timeout
);

/**
* @brief writes data to the UART device. Polling, not interrupt driven
* @param tx_dev pointer to the zephyr device struct for the UART device to write to
* @param txdata pointer to the buffer to write data from
* @param txlen length of the buffer to write from
* @return number of bytes written on success, negative error code on failure
* */
int uart_transport_write(
    const struct device *tx_dev,
    const uint8_t *txdata,
    size_t txlen
);
