#include <atmel_start.h>
#include "i2c.h"
#include <stdio.h>
#include <string.h>

// LTR I2C Address
#define LTR_ADDRESS 0X29

// LTR REGISTERS
#define LTR_ALS_CONTR_ADDRESS 0x80
#define LTR_ALS_MEAS_RATE_ADDRESS 0x85

/* Constants */
uint8_t ch1[2] = {0x88, 0x89};
uint8_t ch0[2] = {0x8A, 0x8B};
static uint16_t channel0,channel1;

/* Prototype */
void ltr_init();
uint16_t ltr_channel(uint8_t *ch);

int main(void)
{
	/* Initializes MCU, drivers and middleware */
	atmel_start_init();
    
    // Initialize I2C
    i2c_init();
    
    // Initialize USART
    struct io_descriptor *i2c_io;
	usart_sync_get_io_descriptor(&USART, &i2c_io);
	usart_sync_enable(&USART);
    
    // Initialize LTR
    ltr_init();
    
	/* Replace with your application code */
    char message[1024];
	while (1) {
        channel0 = ltr_channel(&ch0[0]); delay_ms(100);
        channel1 = ltr_channel(&ch1[0]); delay_ms(100);
        
        // Print Data
        snprintf(message,1024,"CH0: %d, CH1: %d\r\n",channel0,channel1);
        io_write(i2c_io,message,strlen(message));
	}
}

void ltr_init(){
    // Setup Control
    uint8_t data = 0x01;
    i2c_reg_write(LTR_ADDRESS,LTR_ALS_CONTR_ADDRESS,&data,1);
    delay_ms(10);
    
    // Setup Measurement Rate
    data = 0x12;
    i2c_reg_write(LTR_ADDRESS,LTR_ALS_CONTR_ADDRESS,&data,1);
    delay_ms(10);
}

uint16_t ltr_channel(uint8_t *ch){
    uint8_t bytes[2]; uint8_t *byte_ptr = bytes;
    byte_ptr = i2c_reg_read(LTR_ADDRESS,ch[0],1); // Read Registers
    delay_ms(10);
    return bytes[1] << 8 | bytes[0]; // Return Channel Data
}
