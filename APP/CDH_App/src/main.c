#include "main.h"

#include <assert.h>
#include <stdio.h>
#include <zephyr/devicetree.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/drivers/i2c.h>
#include <zephyr/drivers/uart.h>

static const struct i2c_dt_spec dev_test = I2C_DT_SPEC_GET(DT_NODELABEL(demo_i2c));
void demo_i2c(void)
{
    printk("I2C Demo: Start\n");

    char * msg = "Hello!";

    // Write out message via i2c
    i2c_write_dt(&dev_test, (uint8_t *)msg, strlen(msg) + 1);

    // Allocate buffer for response
    uint8_t buf[128];

    // Read the response into the buffer
    i2c_read_dt(&dev_test, buf, strlen(msg));

    // Check if we got back what we sent
    printk("I2C Demo: Sent \"%s\", got back \"%s\"\n", msg, (char *)buf);
    __ASSERT(strcmp(buf, msg) == 0, "I2C demo mismatch");

    printk("I2C Demo: Done\n");
}



static const struct device * dev_demo_uart = DEVICE_DT_GET(DT_NODELABEL(demo_serial));
void demo_uart(void)
{
    printk("UART Demo: Start\n");

    char * msg = "Testing.";

    // Send the message character by character
    for (char * chr = msg; *chr != 0; ++chr)
    {
        uart_poll_out(dev_demo_uart, *chr);
    }

    // Allocate buffer to receive response
    uint8_t rx_buf[64];
    size_t rx_buf_size = 0;

    while (rx_buf_size < sizeof(rx_buf))
    {
        // Put a chr in the buffer, its going to return 0 if successful
        int ret = uart_poll_in(dev_demo_uart, &rx_buf[rx_buf_size]);

        // No character ready yet!
        if (ret != 0)
            continue;

        // Increment size
        rx_buf_size++;

        // End the loop when we reach a dot
        if (rx_buf[rx_buf_size - 1] == '.')
            break;
    }

    // Add null terminator at the end of the string
    rx_buf[min(rx_buf_size, sizeof(rx_buf) - 1)] = 0;

    // Check if the string was echo'd back to us
    printk("UART Demo: Sent \"%s\", got back \"%s\"\n", msg, (char *)rx_buf);
    __ASSERT(strcmp(rx_buf, msg) == 0, "UART demo mismatch");

    printk("UART Demo: Done\n");
}


int main(void)
{
    // logging_init();

    demo_uart();
    demo_i2c();

}