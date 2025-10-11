#include "usart.h"

#include <hpl_usart_base.h>
#include <string.h>

// Definitions
#define MESSAGE_SIZE 1024

// Data
static struct io_descriptor *usart_io;

void usart_init(uint32_t baudrate){ 
    // Configure USART for Logging only
    usart_sync_set_baud_rate(&USART,baudrate); // Set Baudrate 
    usart_sync_set_mode(&USART, USART_MODE_ASYNCHRONOUS); // ASynchronous Mode
    usart_sync_set_parity(&USART, USART_PARITY_NONE); // No Parity Bits
    usart_sync_set_stopbits(&USART, USART_STOP_BITS_ONE); // 1 Stop Bit
       
    // Setup descriptor
    usart_sync_get_io_descriptor(&USART, &usart_io);
	usart_sync_enable(&USART);
}

void usart_write(char* const message){
    io_write(usart_io, (uint8_t*)message, strlen(message));
}

char *usart_read(){
    // Variable
    static uint32_t rx_index = 0;
    static char rx_buffer[MESSAGE_SIZE];
    uint8_t data;
    
    // Read incoming data bit by bit
    while(io_read(usart_io, &data, 1) == 1){
        if(rx_index < MESSAGE_SIZE-1){
            rx_buffer[rx_index++] = data;
            if (data == '\n' || data == '\r') {
                rx_buffer[rx_index] = '\0'; // Null terminate
                return rx_buffer;
            }
        }
        else{
            rx_buffer[rx_index] = '\0'; // Null terminate
            return rx_buffer; 
        }
    }
    return NULL;
      
} // Read data from the USART target
