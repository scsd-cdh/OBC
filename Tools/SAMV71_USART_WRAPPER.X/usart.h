/* 
 * File:   usart.h
 * Author: Sriram
 *
 * Created on October 11, 2025, 12:30 PM
 */

#ifndef USART_H
#define	USART_H

#ifdef	__cplusplus
extern "C"
{
#endif
  
/* Includes */
#include "driver_init.h"

/* Prototypes */
extern void usart_init(uint32_t baudrate); // Configure USART
extern void usart_write(char* const message); // Send data to the USART target
extern char *usart_read(); // Read data from the USART target


#ifdef	__cplusplus
}
#endif

#endif	/* USART_H */

