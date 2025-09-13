/*
 * swi2c.c
 *
 *  Created on: Feb. 21, 2024
 *      Author: Ruben
 */

#include "swi2c.h"
#include <msp430.h>

void SWI2C_Init(SWI2C_Descriptor *descriptor) {
    *descriptor->scl_port_out &= ~descriptor->scl_pin;
    *descriptor->sda_port_out &= ~descriptor->sda_pin;

    *descriptor->scl_port_dir &= ~descriptor->scl_pin;
    *descriptor->sda_port_dir &= ~descriptor->sda_pin;

    TB0CCR0 = SWI2C_TIMER_PERIOD;
}

int32_t SWI2C_Read(SWI2C_Descriptor *descriptor, uint8_t *buffer, uint16_t len) {
    uint_fast8_t bits, temp;
    uint16_t ii = 0;
    uint16_t i = 0;

    /* Starting the timer */
    TB0CTL = TBSSEL_2 + MC_1 + TBCLR;

    /* Sending the START */
    *descriptor->sda_port_dir |= descriptor->sda_pin;
    __no_operation();
    *descriptor->scl_port_dir |= descriptor->scl_pin;
    TIMER_ITERATION();

    /* Next doing the control byte */
    temp = (descriptor->address << 1) | BIT0;
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
        TIMER_ITERATION();

        /* Incrementing to the next bit and waiting for the next clock cycle */
        temp = (temp << 1);
        bits = (bits - 1);
        *descriptor->scl_port_dir |= descriptor->scl_pin;
        TIMER_ITERATION();

    }
    while (bits > 0);

    /* Detecting if we have a NAK on the bus. If the slave device NAKed the
     control byte, it probably isn't there on the bus so we should send
     an I2C stop and return false */
    *descriptor->sda_port_dir &= ~descriptor->sda_pin;
    *descriptor->scl_port_dir &= ~descriptor->scl_pin;
    TIMER_ITERATION();

    if (*descriptor->sda_port_in & descriptor->sda_pin)
    {
        goto I2CReadTransactionCleanUp;
    }

    /* Next, we want to read out all of the data requested */
    for (ii = 0; ii < len; ii++)
    {
        /*
         * Waiting for our clock line to go high if the slave is stretching
         */
        for(i = 0; i < 5000; i ++) {
            if(*descriptor->scl_port_in & descriptor->scl_pin) {
                break;
            }

            if(i == 4999) {
                goto I2CReadTransactionCleanUp;
            }
        }

        /* Setup the read variables */
        temp = 0;
        bits = 0x08;

        /* Sending out another clock cycle */
        *descriptor->scl_port_dir |= descriptor->scl_pin;
        TIMER_ITERATION();
        *descriptor->sda_port_dir &= ~descriptor->sda_pin;

        /* Loop to read until all bits have been read */
        do
        {
            /* Priming our temporary variable and sending a clock pulse */
            temp = (temp << 1);
            *descriptor->scl_port_dir &= ~descriptor->scl_pin;
            TIMER_ITERATION();

            /* If the data line is high, recording that */
            if (*descriptor->sda_port_in & descriptor->sda_pin)
            {
                temp += 1;
            }

            /* Send out another clock cycle and decrement our counter */
            bits = (bits - 1);
            *descriptor->scl_port_dir |= descriptor->scl_pin;
            TIMER_ITERATION();
        }
        while (bits > 0);

        /* Storing the data off */
        buffer[ii] = temp;

        /* Now the master needs to send out the ACK */
        if (ii == len - 1)
            *descriptor->sda_port_dir &= ~descriptor->sda_pin;
        else
            *descriptor->sda_port_dir |= descriptor->sda_pin;
        *descriptor->scl_port_dir &= ~descriptor->scl_pin;

        /*
         * Waiting for our clock line to go high if the slave is stretching
         */
        for(i = 0; i < 5000; i ++) {
            if(*descriptor->scl_port_in & descriptor->scl_pin) {
                break;
            }

            if(i == 4999) {
                goto I2CReadTransactionCleanUp;
            }
        }

        TIMER_ITERATION();
    }

    I2CReadTransactionCleanUp:

    /* Sending out the stop bit */
    *descriptor->scl_port_dir |= descriptor->scl_pin;
    *descriptor->sda_port_dir |= descriptor->sda_pin;
    TIMER_ITERATION();
    *descriptor->scl_port_dir &= ~descriptor->scl_pin;
    __no_operation();
    *descriptor->sda_port_dir &= ~descriptor->sda_pin;
    TIMER_ITERATION();

    /* Stop the timer */
    TB0CTL = MC_0;

    /* If all bytes were read, return true- otherwise false. */
    return ii;
}

int32_t SWI2C_Write(SWI2C_Descriptor *descriptor, uint8_t *buffer, uint16_t len) {
    uint_fast8_t bits, temp;
    uint16_t ii = 0;
    uint16_t i = 0;

    // Start the timer
    TB0CTL = TBSSEL_2 + MC_1 + TBCLR;

    /* Sending the START */
    *descriptor->sda_port_dir |= descriptor->sda_pin;
    __no_operation();
    *descriptor->scl_port_dir |= descriptor->scl_pin;
    TIMER_ITERATION();

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
        TIMER_ITERATION();

        /* Incrementing to the next bit and waiting for the next clock cycle */
        temp = (temp << 1);
        bits = (bits - 1);

        *descriptor->scl_port_dir |= descriptor->scl_pin;
        TIMER_ITERATION();

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
            goto I2CWriteTransactionCleanUp;
        }
    }

    TIMER_ITERATION();

    if (*descriptor->sda_port_in & descriptor->sda_pin)
    {
        goto I2CWriteTransactionCleanUp;
    }

    /* Sending out another clock cycle */
    *descriptor->scl_port_dir |= descriptor->scl_pin;
    TIMER_ITERATION();

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
            TIMER_ITERATION();

            /* Incrementing to the next bit and waiting for next clock cycle */
            temp = (temp << 1);
            bits = (bits - 1);
            *descriptor->scl_port_dir |= descriptor->scl_pin;
            TIMER_ITERATION();

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
                goto I2CWriteTransactionCleanUp;
            }
        }

        TIMER_ITERATION();

        if (*descriptor->sda_port_in & descriptor->sda_pin)
        {
            goto I2CWriteTransactionCleanUp;
        }

        /* Sending out another clock cycle */
        *descriptor->scl_port_dir |= descriptor->scl_pin;
        TIMER_ITERATION();
    }

    I2CWriteTransactionCleanUp:
    /* If the user did not request to skip, we send out the stop bit */

    *descriptor->scl_port_dir |= descriptor->scl_pin;
    TIMER_ITERATION();
    *descriptor->sda_port_dir |= descriptor->sda_pin;
    TIMER_ITERATION();
    *descriptor->scl_port_dir &= ~descriptor->scl_pin;
    __no_operation();
    *descriptor->sda_port_dir &= ~descriptor->sda_pin;

    /* Stop the timer */
    TB0CTL = MC_0;

    return ii;
}
