# I2C protection-circuit test

`main.c` is a standalone MSP430FR5989 test program for the two I2C buses used
by the BMS:

| Bus | SDA | SCL |
| --- | --- | --- |
| UCB0 (main) | P1.6 | P1.7 |
| UCB1 (redundant) | P3.1 | P3.2 |

The program repeats this sequence:

1. Release all four lines as high-impedance GPIO inputs.
2. Drive each line low for `I2C_PROTECTION_HOLD_MS`.
3. Release the line and wait `I2C_PROTECTION_RELEASE_MS`.

The defaults are 1000 ms for both values. Change the macros in `main.c`, or
define them in the CCS compiler settings, to test a different duration. The
hold time should be longer than the protection circuit's 10 nF discharge
interval.

Create or copy the existing BMS MSP430FR5989 CCS project, replace its
application `main.c` with this file, and include
`MIDDLEWARE/MSP_UTILS/msp_utils.c` and its include path. Do not run this test
while the normal BMS firmware is controlling the same I2C pins.

The released state is an input with the output latch low; it never actively
drives the I2C line high. This is required for open-drain I2C wiring.
