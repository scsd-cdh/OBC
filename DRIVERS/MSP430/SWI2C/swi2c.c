/*
 * swi2c.c
 *
 *  Created on: Feb. 21, 2024
 *      Author: Ruben
 */

#include "swi2c.h"
#include <msp430.h>
#include "timer_a.h"

// Reserve TA1/CCR0 for software I2C. TB0 belongs to heater PWM.
#define SWI2C_TIMER_BASE TIMER_A1_BASE
#define SWI2C_TIMER_CCR TIMER_A_CAPTURECOMPARE_REGISTER_0

static void swi2c_timer_start(void) {
    Timer_A_clear(SWI2C_TIMER_BASE);
    Timer_A_startCounter(SWI2C_TIMER_BASE, TIMER_A_UP_MODE);
}

static inline bool swi2c_timer_wait(void) {
    Timer_A_clearCaptureCompareInterrupt(SWI2C_TIMER_BASE, SWI2C_TIMER_CCR);
    for (uint16_t polls = 0; polls < SWI2C_TIMER_POLL_LIMIT; polls++) {
        if (Timer_A_getCaptureCompareInterruptStatus(
                SWI2C_TIMER_BASE, SWI2C_TIMER_CCR,
                TIMER_A_CAPTURECOMPARE_INTERRUPT_FLAG)) {
            return true;
        }
    }
    return false;
}

static int32_t swi2c_timeout(SWI2C_Descriptor *descriptor) {
    // Release the open-drain bus without waiting on a possibly stopped timer.
    *descriptor->sda_port_dir &= ~descriptor->sda_pin;
    *descriptor->scl_port_dir &= ~descriptor->scl_pin;
    Timer_A_stop(SWI2C_TIMER_BASE);
    return SWI2C_ERROR_TIMEOUT;
}

void SWI2C_Init(SWI2C_Descriptor *descriptor) {
    *descriptor->scl_port_out &= ~descriptor->scl_pin;
    *descriptor->sda_port_out &= ~descriptor->sda_pin;

    *descriptor->scl_port_dir &= ~descriptor->scl_pin;
    *descriptor->sda_port_dir &= ~descriptor->sda_pin;

    Timer_A_initUpModeParam timer = {
        .clockSource = TIMER_A_CLOCKSOURCE_SMCLK,
        .clockSourceDivider = TIMER_A_CLOCKSOURCE_DIVIDER_1,
        .timerPeriod = SWI2C_TIMER_PERIOD,
        .timerInterruptEnable_TAIE = TIMER_A_TAIE_INTERRUPT_DISABLE,
        .captureCompareInterruptEnable_CCR0_CCIE = TIMER_A_CCIE_CCR0_INTERRUPT_DISABLE,
        .timerClear = TIMER_A_DO_CLEAR,
        .startTimer = false
    };
    Timer_A_stop(SWI2C_TIMER_BASE);
    Timer_A_initUpMode(SWI2C_TIMER_BASE, &timer);
}

int32_t SWI2C_Read(SWI2C_Descriptor *descriptor, uint8_t *buffer, uint16_t len) {
    uint_fast8_t numBits, currentByte;
    uint16_t rxIndex = 0;
    uint16_t sclStretchCnt = 0;

    /* Starting the timer */
    swi2c_timer_start();

    /* Sending the START */
    *descriptor->sda_port_dir |= descriptor->sda_pin;
    __no_operation();
    *descriptor->scl_port_dir |= descriptor->scl_pin;
    if (!swi2c_timer_wait()) {
        return swi2c_timeout(descriptor);
    }

    /* Next doing the control byte */
    currentByte = (descriptor->address << 1) | BIT0;
    numBits = 8;

    /* Loop until all bits of the address byte are sent out */
    do
    {
        /* Deciding if we want to send a high or low out of the line */
        if (currentByte & BIT7)
        {
            *descriptor->sda_port_dir &= ~descriptor->sda_pin;
        }
        else
        {
            *descriptor->sda_port_dir |= descriptor->sda_pin;
        }

        /* Now that we set the SDA line, we have to send out a clock pulse */
        *descriptor->scl_port_dir &= ~descriptor->scl_pin;
        if (!swi2c_timer_wait()) {
            return swi2c_timeout(descriptor);
        }

        /* Incrementing to the next bit and waiting for the next clock cycle */
        currentByte = (currentByte << 1);
        numBits = (numBits - 1);
        *descriptor->scl_port_dir |= descriptor->scl_pin;
        if (!swi2c_timer_wait()) {
            return swi2c_timeout(descriptor);
        }

    }
    while (numBits > 0);

    /* Detecting if we have a NAK on the bus. If the slave device NAKed the
     control byte, it probably isn't there on the bus so we should send
     an I2C stop and return false */
    *descriptor->sda_port_dir &= ~descriptor->sda_pin;
    *descriptor->scl_port_dir &= ~descriptor->scl_pin;
    if (!swi2c_timer_wait()) {
        return swi2c_timeout(descriptor);
    }

    if (*descriptor->sda_port_in & descriptor->sda_pin)
    {
        goto I2CReadTransactionCleanUp;
    }

    /* Next, we want to read out all of the data requested */
    for (rxIndex = 0; rxIndex < len; rxIndex++)
    {
        /*
         * Waiting for our clock line to go high if the slave is stretching
         */
        for(sclStretchCnt = 0; sclStretchCnt < 5000; sclStretchCnt ++) {
            if(*descriptor->scl_port_in & descriptor->scl_pin) {
                break;
            }

            if(sclStretchCnt == 4999) {
                return swi2c_timeout(descriptor);
            }
        }

        /* Setup the read variables */
        currentByte = 0;
        numBits = 0x08;

        /* Sending out another clock cycle */
        *descriptor->scl_port_dir |= descriptor->scl_pin;
        if (!swi2c_timer_wait()) {
            return swi2c_timeout(descriptor);
        }
        *descriptor->sda_port_dir &= ~descriptor->sda_pin;

        /* Loop to read until all bits have been read */
        do
        {
            /* Priming our temporary variable and sending a clock pulse */
            currentByte = (currentByte << 1);
            *descriptor->scl_port_dir &= ~descriptor->scl_pin;
            if (!swi2c_timer_wait()) {
                return swi2c_timeout(descriptor);
            }

            /* If the data line is high, recording that */
            if (*descriptor->sda_port_in & descriptor->sda_pin)
            {
                currentByte += 1;
            }

            /* Send out another clock cycle and decrement our counter */
            numBits = (numBits - 1);
            *descriptor->scl_port_dir |= descriptor->scl_pin;
            if (!swi2c_timer_wait()) {
                return swi2c_timeout(descriptor);
            }
        }
        while (numBits > 0);

        /* Storing the data off */
        buffer[rxIndex] = currentByte;

        /* Now the master needs to send out the ACK */
        if (rxIndex == len - 1)
            *descriptor->sda_port_dir &= ~descriptor->sda_pin;
        else
            *descriptor->sda_port_dir |= descriptor->sda_pin;
        *descriptor->scl_port_dir &= ~descriptor->scl_pin;

        /*
         * Waiting for our clock line to go high if the slave is stretching
         */
        for(sclStretchCnt = 0; sclStretchCnt < 5000; sclStretchCnt ++) {
            if(*descriptor->scl_port_in & descriptor->scl_pin) {
                break;
            }

            if(sclStretchCnt == 4999) {
                return swi2c_timeout(descriptor);
            }
        }

        if (!swi2c_timer_wait()) {
            return swi2c_timeout(descriptor);
        }
    }

    I2CReadTransactionCleanUp:

    /* Sending out the stop bit */
    *descriptor->scl_port_dir |= descriptor->scl_pin;
    *descriptor->sda_port_dir |= descriptor->sda_pin;
    if (!swi2c_timer_wait()) {
        return swi2c_timeout(descriptor);
    }
    *descriptor->scl_port_dir &= ~descriptor->scl_pin;
    __no_operation();
    *descriptor->sda_port_dir &= ~descriptor->sda_pin;
    if (!swi2c_timer_wait()) {
        return swi2c_timeout(descriptor);
    }

    /* Stop the timer */
    Timer_A_stop(SWI2C_TIMER_BASE);

    /* If all bytes were read, return true- otherwise false. */
    return rxIndex;
}

int32_t SWI2C_Write(SWI2C_Descriptor *descriptor, uint8_t *buffer, uint16_t len) {
    uint_fast8_t bits, temp;
    uint16_t ii = 0;
    uint16_t i = 0;

    // Start the timer
    swi2c_timer_start();

    /* Sending the START */
    *descriptor->sda_port_dir |= descriptor->sda_pin;
    __no_operation();
    *descriptor->scl_port_dir |= descriptor->scl_pin;
    if (!swi2c_timer_wait()) {
        return swi2c_timeout(descriptor);
    }

    /* Next doing the control byte */
    temp = (descriptor->address << 1);
    bits = 8;

    /* Loop until all bits of the address byte are sent out */
    do
    {
        /* Deciding if we want to send a high or low out of the line */
        if (temp & BIT7)
        {
            *descriptor->sda_port_dir &= ~descriptor->sda_pin;
        }
        else
        {
            *descriptor->sda_port_dir |= descriptor->sda_pin;
        }

        /* Now that we set the SDA line, we have to send out a clock pulse */
        *descriptor->scl_port_dir &= ~descriptor->scl_pin;
        if (!swi2c_timer_wait()) {
            return swi2c_timeout(descriptor);
        }

        /* Incrementing to the next bit and waiting for the next clock cycle */
        temp = (temp << 1);
        bits = (bits - 1);

        *descriptor->scl_port_dir |= descriptor->scl_pin;
        if (!swi2c_timer_wait()) {
            return swi2c_timeout(descriptor);
        }

    }
    while (bits > 0);

    /* Detecting if we have a NAK on the bus. If the slave device NAKed the
     control byte, it probably isn't there on the bus so we should send
     an I2C stop and return false */
    *descriptor->sda_port_dir &= ~descriptor->sda_pin;
    *descriptor->scl_port_dir &= ~descriptor->scl_pin;
    /*
     * Waiting for our clock line to go high if the slave is stretching
     */
    for(i = 0; i < 5000; i ++) {
        if(*descriptor->scl_port_in & descriptor->scl_pin) {
            break;
        }

        if(i == 4999) {
            return swi2c_timeout(descriptor);
        }
    }

    if (!swi2c_timer_wait()) {
        return swi2c_timeout(descriptor);
    }

    if (*descriptor->sda_port_in & descriptor->sda_pin)
    {
        goto I2CWriteTransactionCleanUp;
    }

    /* Sending out another clock cycle */
    *descriptor->scl_port_dir |= descriptor->scl_pin;
    if (!swi2c_timer_wait()) {
        return swi2c_timeout(descriptor);
    }

    /* Next, let us send out all bytes in the user buffer */
    for (ii = 0; ii < len; ii++)
    {
        temp = buffer[ii];
        bits = 8;

        /* Loop until all bits of the current byte are sent out */
        do
        {
            /* Deciding if we want to send a high or low out of the line */
            if (temp & BIT7)
            {
                *descriptor->sda_port_dir &= ~descriptor->sda_pin;
            }
            else
            {
                *descriptor->sda_port_dir |= descriptor->sda_pin;
            }

            /* Now that we set the SDA line, we send out a clock pulse */
            *descriptor->scl_port_dir &= ~descriptor->scl_pin;
            if (!swi2c_timer_wait()) {
                return swi2c_timeout(descriptor);
            }

            /* Incrementing to the next bit and waiting for next clock cycle */
            temp = (temp << 1);
            bits = (bits - 1);
            *descriptor->scl_port_dir |= descriptor->scl_pin;
            if (!swi2c_timer_wait()) {
                return swi2c_timeout(descriptor);
            }

        }
        while (bits > 0);

        /* Detecting the NAK. We should break out of the send loop */
        *descriptor->sda_port_dir &= ~descriptor->sda_pin;
        *descriptor->scl_port_dir &= ~descriptor->scl_pin;
        /*
         * Waiting for our clock line to go high if the slave is stretching
         */
        for(i = 0; i < 5000; i ++) {
            if(*descriptor->scl_port_in & descriptor->scl_pin) {
                break;
            }

            if(i == 4999) {
                return swi2c_timeout(descriptor);
            }
        }

        if (!swi2c_timer_wait()) {
            return swi2c_timeout(descriptor);
        }

        if (*descriptor->sda_port_in & descriptor->sda_pin)
        {
            goto I2CWriteTransactionCleanUp;
        }

        /* Sending out another clock cycle */
        *descriptor->scl_port_dir |= descriptor->scl_pin;
        if (!swi2c_timer_wait()) {
            return swi2c_timeout(descriptor);
        }
    }

    I2CWriteTransactionCleanUp:
    /* If the user did not request to skip, we send out the stop bit */

    *descriptor->scl_port_dir |= descriptor->scl_pin;
    if (!swi2c_timer_wait()) {
        return swi2c_timeout(descriptor);
    }
    *descriptor->sda_port_dir |= descriptor->sda_pin;
    if (!swi2c_timer_wait()) {
        return swi2c_timeout(descriptor);
    }
    *descriptor->scl_port_dir &= ~descriptor->scl_pin;
    __no_operation();
    *descriptor->sda_port_dir &= ~descriptor->sda_pin;

    /* Stop the timer */
    Timer_A_stop(SWI2C_TIMER_BASE);

    return ii;
}
