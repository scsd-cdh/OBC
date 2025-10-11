#include <atmel_start.h>
#include "usart.h"
#include <stdio.h>
#include <string.h>

int main(void)
{
	/* Initializes MCU, drivers and middleware */
	atmel_start_init();
    
    // Initialize USART
    usart_init(115200);

	/* Replace with your application code */
	while (1) {
        // Read Message
        char* read_message = "\r\n";
        
        // Write Message back to VCOM
        snprintf(read_message,1024,"Extracted Message : %s",usart_read());
        usart_write(read_message);
        delay_ms(100);
	}
}
