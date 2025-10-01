/*
 * MRAM_peripheral.h
 *
 * Created: 10/1/2025 7:11:44 PM
 *  Author: space
 */ 


#ifndef MRAM_PERIPHERAL_H_
#define MRAM_PERIPHERAL_H_

#define MRAM_RDSR 0x05
#define MRAM_RDID 0x9F

#define MRAM_STATUS_REGISTER_ID_DATA_BYTES 1
#define MRAM_DEVICE_ID_DATA_BYTES 4

void MRAM_readStatusRegister(volatile uint8_t* buffer, uint8_t len);
void MRAM_readDeviceID(volatile uint8_t* buffer, size_t len);


#endif /* MRAM_PERIPHERAL_H_ */
