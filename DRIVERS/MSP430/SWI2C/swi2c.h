#ifndef SRC_DRIVERS_SWI2C_H_
#define SRC_DRIVERS_SWI2C_H_

/**
 * @file
 * @brief Software (bit-banged) I²C driver interface for MCUs with GPIO and a timer.
 *
 * @details
 * This module provides a minimal software I²C master using GPIO pins for SDA/SCL
 * and a hardware timer to pace edges. Timing is controlled via
 * #SWI2C_TIMER_PERIOD and internal Timer_A driver calls. The application supplies
 * GPIO register pointers and pin masks through #SWI2C_Descriptor, then calls
 * SWI2C_Init() followed by SWI2C_Read()/SWI2C_Write() for transfers.
 *
 * @note This driver exclusively owns Timer_A1 and CCR0. It polls the compare
 *       flag without interrupts and does not configure any timer output pins.
 *       Other code must not reconfigure TA1 while software I2C is in use.
 */

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @def SWI2C_TIMER_PERIOD
 * @brief TA1 CCR0 period in SMCLK ticks, used to pace I²C bit transitions.
 *       Software execution also contributes to the actual SCL period.
 */
#define SWI2C_TIMER_PERIOD  5

/**
 * @struct SWI2C_Descriptor
 * @brief Hardware binding and address for a software I²C master instance.
 *
 * @details
 * All GPIO pointers must reference byte-addressable OUT/IN/DIR registers for
 * the specified pins. Pins are active-high masks (single-bit set).
 */
typedef struct {
    /** Pointer to SDA port OUT register (e.g., &P4OUT). */
    volatile uint8_t* sda_port_out;

    /** Pointer to SDA port IN register (e.g., &P4IN). */
    volatile uint8_t* sda_port_in;

    /** Pointer to SDA port DIR register (e.g., &P4DIR). */
    volatile uint8_t* sda_port_dir;

    /** Bit mask for SDA pin (e.g., GPIO_PIN1 for P4.1). */
    uint16_t          sda_pin;

    /** Pointer to SCL port OUT register (e.g., &P4OUT). */
    volatile uint8_t* scl_port_out;

    /** Pointer to SCL port IN register (e.g., &P4IN). */
    volatile uint8_t* scl_port_in;

    /** Pointer to SCL port DIR register (e.g., &P4DIR). */
    volatile uint8_t* scl_port_dir;

    /** Bit mask for SCL pin (e.g., GPIO_PIN0 for P4.0). */
    uint16_t          scl_pin;

    /** 7-bit slave address (without R/W bit). Example: 0x1A for address 0b0011010. */
    uint8_t           address;
} SWI2C_Descriptor;

/**
 * @brief Initializes timer module and sets registers to correct bits. Needs to be called before doing any SWI2C
 *
 * @param[in,out] descriptor  Descriptor with GPIO register pointers.
 *
 */
void SWI2C_Init(SWI2C_Descriptor *descriptor);

/**
 * @brief Read bytes from the slave at @p descriptor->address.
 *
 * @param[in]  descriptor  Initialized SWI2C descriptor.
 * @param[out] buffer      Destination buffer to receive bytes.
 * @param[in]  len         Number of bytes to read.
 *
 * @return
 *  The iteration the read made it to - int32_t -- treat as boolean 
 *
 */
int32_t SWI2C_Read(SWI2C_Descriptor *descriptor, uint8_t *buffer, uint16_t len);

/**
 * @brief Write bytes to the slave at @p descriptor->address.
 *
 * @param[in] descriptor  Initialized SWI2C descriptor.
 * @param[in] buffer      Source buffer containing bytes to transmit.
 * @param[in] len         Number of bytes to write.
 *
 * @return
 *   The iteration the write made it to - int32_t -- treat as boolean 
 */
int32_t SWI2C_Write(SWI2C_Descriptor *descriptor, uint8_t *buffer, uint16_t len);

#ifdef __cplusplus
}
#endif

#endif /* SRC_DRIVERS_SWI2C_H_ */
