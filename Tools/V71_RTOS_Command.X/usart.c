#include "usart.h"

// C Library
#include <string.h>
#include <stdint.h>

void usart_write(struct io_descriptor *const io_descr, char* const message){
    io_write(io_descr,(uint8_t*)message,strlen(message));
}
