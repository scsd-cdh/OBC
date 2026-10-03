/*
 * MSP430FR5989 I2C protection-circuit test
 *
 * The test drives one slave-side I2C line low, then releases it by changing
 * the pin to a high-impedance input. External pull-ups should restore the
 * released line high.
 *
 * Add this file as the application's main source in a CCS MSP430FR5989
 * project. The existing MIDDLEWARE/MSP_UTILS/msp_utils.c source is also
 * required for clock_init_16mhz().
 */

#include <msp430.h>
#include <stdint.h>

#include "msp_utils.h"

#ifndef I2C_PROTECTION_HOLD_MS
#define I2C_PROTECTION_HOLD_MS       1000u
#endif

#ifndef I2C_PROTECTION_RELEASE_MS
#define I2C_PROTECTION_RELEASE_MS    1000u
#endif

typedef struct {
    volatile uint8_t *sel0;
    volatile uint8_t *sel1;
    volatile uint8_t *out;
    volatile uint8_t *dir;
    uint8_t mask;
} test_pin_t;

static const test_pin_t sda_main = {
    &P1SEL0, &P1SEL1, &P1OUT, &P1DIR, BIT6
};
static const test_pin_t scl_main = {
    &P1SEL0, &P1SEL1, &P1OUT, &P1DIR, BIT7
};
static const test_pin_t sda_redundant = {
    &P3SEL0, &P3SEL1, &P3OUT, &P3DIR, BIT1
};
static const test_pin_t scl_redundant = {
    &P3SEL0, &P3SEL1, &P3OUT, &P3DIR, BIT2
};

static void delay_ms(uint16_t milliseconds)
{
    while (milliseconds-- != 0u) {
        __delay_cycles(16000);
    }
}

static void release_line(const test_pin_t *pin)
{
    /* GPIO mode plus input direction gives the pull-up control of the line. */
    *pin->sel0 &= (uint8_t)~pin->mask;
    *pin->sel1 &= (uint8_t)~pin->mask;
    *pin->out &= (uint8_t)~pin->mask;
    *pin->dir &= (uint8_t)~pin->mask;
}

static void hold_line_low(const test_pin_t *pin)
{
    /* Set OUT low before changing DIR to avoid any high-going transition. */
    *pin->sel0 &= (uint8_t)~pin->mask;
    *pin->sel1 &= (uint8_t)~pin->mask;
    *pin->out &= (uint8_t)~pin->mask;
    *pin->dir |= pin->mask;
}

static void test_line(const test_pin_t *pin)
{
    hold_line_low(pin);
    delay_ms(I2C_PROTECTION_HOLD_MS);
    release_line(pin);
    delay_ms(I2C_PROTECTION_RELEASE_MS);
}

int main(void)
{
    WDTCTL = WDTPW | WDTHOLD;

    /* Keep both eUSCI modules in reset; this test uses their pins as GPIO. */
    UCB0CTLW0 |= UCSWRST;
    UCB1CTLW0 |= UCSWRST;

    clock_init_16mhz();
    PM5CTL0 &= (uint16_t)~LOCKLPM5;

    release_line(&sda_main);
    release_line(&scl_main);
    release_line(&sda_redundant);
    release_line(&scl_redundant);
    delay_ms(I2C_PROTECTION_RELEASE_MS);

    for (;;) {
        test_line(&sda_main);
        test_line(&scl_main);
        test_line(&sda_redundant);
        test_line(&scl_redundant);
    }
}
