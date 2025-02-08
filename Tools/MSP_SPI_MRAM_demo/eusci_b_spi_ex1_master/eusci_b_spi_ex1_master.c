/* --COPYRIGHT--,BSD
 * Copyright (c) 2017, Texas Instruments Incorporated
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * *  Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * *  Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * *  Neither the name of Texas Instruments Incorporated nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 * --/COPYRIGHT--*/
//*****************************************************************************
//! USCI_B0, SPI 3-Wire Master Incremented Data
//! This example shows how SPI master talks to SPI slave using 3-wire mode.
//! Incrementing data is sent by the master starting at 0x01. Received data is
//! expected to be same as the previous transmission.  eUSCI RX ISR is used to
//! handle communication with the CPU, normally in LPM0. If high, P1.0 indicates
//! valid data reception.  Because all execution after LPM0 is in ISRs,
//! initialization waits for DCO to stabilize against SMCLK.
//! ACLK = ~32.768kHz, MCLK = SMCLK = DCO ~ 1048kHz.  BRCLK = SMCLK/2
//!
//! Use with SPI Slave Data Echo code example.  If slave is in debug mode, P1.1
//! slave reset signal conflicts with slave's JTAG; to work around, use IAR's
//! "Release JTAG on Go" on slave device.  If breakpoints are set in
//! slave RX ISR, master must stopped also to avoid overrunning slave
//! RXBUF.
//!
//!             Tested on MSP430FR5969
//!                 -----------------
//!            /|\ |                 |
//!             |  |                 |
//!    Master---+->|RST              |
//!                |                 |
//!                |             P1.6|-> Data Out (UCB0SIMO)
//!                |                 |
//!                |             P1.7|<- Data In (UCB0SOMI)
//!                |                 |
//!                |             P2.2|-> Serial Clock Out (UCB0CLK)
//!
//!
//! This example uses the following peripherals and I/O signals.  You must
//! review these and change as needed for your own board:
//! - SPI peripheral
//! - GPIO Port peripheral (for SPI pins)
//! - UCB0SIMO
//! - UCB0SOMI
//! - UCB0CLK
//!
//! This example uses the following interrupt handlers.  To use this example
//! in your own application you must add these interrupt handlers to your
//! vector table.
//! - USCI_B0_VECTOR
//!
//*****************************************************************************

#include "crc.h"
#include "driverlib.h"
#include "MRAM_SPI.h"
#include "msp430fr5969.h"
#include "string.h"
#include <stdint.h>
#include <stddef.h>
#include <stdlib.h>

#define LENGTH 9

void main(void)
{
    uint32_t clockSpeed = 10000;
    SPI_init(clockSpeed, SPI_MODE_ZERO, DEFAULT_CS_PIN, DEFAULT_SCLK_PIN, DEFAULT_MOSI_PIN, DEFAULT_MISO_PIN);

    //Wait for slave to initialize
    __delay_cycles(100000);
   
    uint8_t buffer[4];
    MRAM_readDeviceId(buffer);

   /* P28 Datasheet: 
    * In normal operational mode, Write instructions must be preceded by the WREN command.  
    * WREN command sets the WREN bit in the Status register. WREN bit is reset at the end of every Write instruction.  
    * WREN bit can also be reset by executing the WRDI command. 
    */  
    MRAM_writeMemoryEn();
    MRAM_writeStatusRegister(0b11000110);

    uint8_t statusRegister = 0;
    volatile MRAM_ErrorCode statusRegisterErr = MRAM_readStatusRegister(&statusRegister);

    uint32_t addr = 0xEF;

    const uint8_t* inBuffer = "123456789";

    MRAM_writeMemoryEn();
    // MSP430 does not support VLAs so we have to do this
    volatile MRAM_ErrorCode writeRet = MRAM_writeMemoryArray(addr, inBuffer, LENGTH);
    uint8_t outBuffer[LENGTH];
    volatile MRAM_ErrorCode readRet = MRAM_readMemoryArray(addr, outBuffer, LENGTH);

    __bis_SR_register(LPM0_bits + GIE);      // CPU off, enable interrupts
    __no_operation();                       // Remain in LPM0
}
