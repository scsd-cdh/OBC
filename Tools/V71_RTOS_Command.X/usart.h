#ifndef USART_WRAPPER    /* Guard against multiple inclusion */
#define USART_WRAPPER

/* Includes */
// Header
#include "atmel_start_pins.h"
#include "atmel_start.h"

/* Prototypes */
void usart_write(struct io_descriptor *const io_descr, char* const message);


#endif 
