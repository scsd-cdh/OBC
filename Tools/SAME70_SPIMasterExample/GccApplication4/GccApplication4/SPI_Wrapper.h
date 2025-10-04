/*
 * SPI_Wrapper.h
 *
 * Created: 10/1/2025 6:59:42 PM
 *  Author: Brendan
 */ 


#ifndef SPI_WRAPPER_H
#define SPI_WRAPPER_H

#define MISOpin   20u   // PD20 = SPI0_MISO
#define MOSIpin   21u   // PD21 = SPI0_MOSI
#define SPCKpin   22u   // PD22 = SPI0_SPCK
#define NPCS1pin  25u   // PD25 = SPI0_NPCS1

#define PD_MASK   ((1u<<MISOpin) | (1u<<MOSIpin) | (1u<<SPCKpin) | (1u<<NPCS1pin))

// Target SPI bit rate (adjust as needed)
#define SPI_BAUD_HZ   100000u

extern inline void SPI_masterTransfer(const uint8_t cmdId);
extern inline uint8_t SPI_masterReceive();
extern void SPI_init();


#endif 