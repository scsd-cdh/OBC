/**
 * @file ads7138irter.h
 * @brief Minimal ADS7138 helper API over software I²C (SWI2C).
 *
 * @details
 * This layer builds ADS7138 single-register access and simple conversions
 * on top of the bit-banged I²C driver declared in @ref swi2c.h.
 *
 * Command opcodes used (see .c):
 * - GEN_CMD address: 0x00 (used for global/general commands)
 * - Device I²C 7-bit address: 0x10 (ADS7138 default when ADDR = 0x10)
 * - Opcodes: SNG_REG_WRITE (0x08), SNG_REG_READ (0x10),
 *            SET_BIT (0x18), CLEAR_BIT (0x20), SW_RESET (0x06)
 *
 * @note All functions take a pointer to an initialized @ref SWI2C_Descriptor.
 *       Most helpers will set @c descriptor->address internally to the
 *       appropriate 7-bit address before performing the transfer.
 *
 * @code
 * // Example (MSP430): configure GPIO as in SWI2C_Descriptor docs
 * SWI2C_Descriptor i2c = {
 *   .sda_port_out = &P4OUT, .sda_port_in = &P4IN, .sda_port_dir = &P4DIR, .sda_pin = GPIO_PIN1,
 *   .scl_port_out = &P4OUT, .scl_port_in = &P4IN, .scl_port_dir = &P4DIR, .scl_pin = GPIO_PIN0,
 *   .address = 0x10 // will be (re)assigned by Initialize as needed
 * };
 * P4SEL0 &= ~(BIT0 | BIT1);   // ensure GPIO mode
 *
 * ADS7138IRTER_Initialize(&i2c);  // sets address, primes SWI2C
 *
 * // Read a single register
 * uint8_t status = ADS7138IRTER_SingleRegisterRead(&i2c, ADS7138_SYSTEM_STATUS_REGISTER);
 *
 * // Write a single register
 * ADS7138IRTER_SingleRegisterWrite(&i2c, ADS7138_PIN_CFG_REGISTER, 0xA5);
 *
 * // Clear bits in a register
 * ADS7138IRTER_ClearBit(&i2c, ADS7138_PIN_CFG_REGISTER, 0x0F);
 *
 * // Read a 12-bit conversion (packed: high 8 bits, then high nibble)
 * uint16_t code12;
 * if (ADS7138IRTER_Read(&i2c, &code12) == ADS7138_STATUS_OK) {
 *     // Use the successfully read conversion.
 * }
 * @endcode
 */

#ifndef ADS7138IRTER_H_
#define ADS7138IRTER_H_

#include "swi2c.h"
#include <stdint.h>

/** Status for checked ADS7138 transfers. */
typedef enum {
    ADS7138_STATUS_OK = 0,
    ADS7138_STATUS_TRANSFER_FAILED,
    ADS7138_STATUS_TIMEOUT
} ADS7138_Status;

/**
 * @enum ADS7138_Register
 * @brief Subset of ADS7138 register addresses used by this helper.
 *
 * @var ADS7138_SYSTEM_STATUS_REGISTER
 *      System status register (0x00).
 * @var ADS7138_PIN_CFG_REGISTER
 *      GPIO/analog pin configuration register (0x05).
 * @var ADS7138_CHANNEL_SEL_REGISTER
 *      Channel select / sequence register (0x11).
 */
typedef enum {
    ADS7138_SYSTEM_STATUS_REGISTER = 0x00,
    ADS7138_PIN_CFG_REGISTER       = 0x05,
    ADS7138_CHANNEL_SEL_REGISTER   = 0x11,
} ADS7138_Register;

/**
 * @brief Initialize the ADS7138 I²C context on top of SWI2C.
 *
 * @details
 * Sets @c descriptor->address to the ADS7138 7-bit device address used by this
 * project and calls @ref SWI2C_Init. Does not configure any ADS7138 registers.
 *
 * @param[in,out] descriptor  Valid @ref SWI2C_Descriptor with SDA/SCL GPIO set.
 * @return 0 after local initialization. This does not probe the ADC on the bus.
 */
int32_t ADS7138IRTER_Initialize(SWI2C_Descriptor *descriptor);

/**
 * @brief Reset the ADS7138 using I2C general call address 0x00 and command 0x06.
 *
 * @details
 * Sets @c descriptor->address to 0x00, writes the reset byte, then restores
 * the normal device address 0x10, including when the write fails.
 *
 * @param[in,out] descriptor  I²C descriptor (its @c address is changed temporarily to gen cmd).
 * @return 1 on a complete write, 0 on NACK, or SWI2C_ERROR_TIMEOUT on timeout.
 */
int32_t ADS7138IRTER_SW_Reset(SWI2C_Descriptor *descriptor);

/**
 * @brief Read a 12-bit conversion result from the ADS7138.
 *
 * @details
 * Performs a 2-byte read and packs the 12-bit result only on a complete read.
 *
 * @param[in,out] descriptor  I²C descriptor (address set to device internally if needed).
 * @param[out] value Non-null output pointer. Updated only on success with the
 *                   right-aligned 12-bit conversion; unchanged on failure.
 * @return ADS7138_STATUS_OK, ADS7138_STATUS_TIMEOUT, or ADS7138_STATUS_TRANSFER_FAILED.
 */
ADS7138_Status ADS7138IRTER_Read(SWI2C_Descriptor *descriptor, uint16_t *value);

/**
 * @brief Read one ADS7138 register via “single register read” opcode.
 *
 * @details
 * Issues a two-byte write of { @c SNG_REG_READ, @p reg_addr }, followed by a 1-byte read.
 * The descriptor address is set to the device address before the transaction.
 *
 * @param[in,out] descriptor  I²C descriptor (its @c address is set internally).
 * @param[in]     reg_addr    Register address to read.
 * @return The 8-bit value read from @p reg_addr.
 */
uint8_t ADS7138IRTER_SingleRegisterRead(SWI2C_Descriptor *descriptor, ADS7138_Register reg_addr);

/**
 * @brief Write one ADS7138 register via “single register write” opcode.
 *
 * @details
 * Issues a three-byte write { @c SNG_REG_WRITE, @p reg_addr, @p reg_data } to the device address.
 *
 * @param[in,out] descriptor  I²C descriptor (its @c address is set internally).
 * @param[in]     reg_addr    Register address to write.
 * @param[in]     reg_data    Byte to write into @p reg_addr.
 * @return ADS7138_STATUS_OK, ADS7138_STATUS_TIMEOUT, or ADS7138_STATUS_TRANSFER_FAILED.
 */
ADS7138_Status ADS7138IRTER_SingleRegisterWrite(SWI2C_Descriptor *descriptor,
                                         ADS7138_Register reg_addr,
                                         uint8_t reg_data);

/**
 * @brief Clear specific bits in an ADS7138 register via “clear bit” opcode.
 *
 * @details
 * Issues a three-byte write { @c CLEAR_BIT, @p reg_addr, @p clear_bits }.
 * Any bit set to 1 in @p clear_bits will be cleared in the target register.
 *
 * @param[in,out] descriptor  I²C descriptor (its @c address is set internally).
 * @param[in]     reg_addr    Target register.
 * @param[in]     clear_bits  Bit mask of bits to clear (1 = clear that bit).
 * @return Non-negative number of bytes written (3 on success), or negative on error.
 *
 * @note A complementary “set bit” helper could be implemented similarly with opcode 0x18.
 */
int32_t ADS7138IRTER_ClearBit(SWI2C_Descriptor *descriptor,
                              ADS7138_Register reg_addr,
                              uint8_t clear_bits);

#endif /* ADS7138IRTER_H_ */
