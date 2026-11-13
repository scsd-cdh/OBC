# ADS7138 heater test emulator

Upload `ads7138_emulator.ino` to a Raspberry Pi Pico using an Arduino core
that supports Pico `Wire` slave mode. It responds at 7-bit I2C address `0x10`,
accepts channel-select writes, and returns left-aligned 12-bit ADC samples.

Connect Pico GP4 (SDA), GP5 (SCL), and GND to the BMS local ADC bus. For the
FR5989/FR6989 configuration, BMS SDA is P4.1 and SCL is P4.0. Use 3.3 V bus
pull-ups. Disconnect/isolate the real ADS7138 so it does not also answer at
`0x10`. Monitor heater control pins at logic level, not the heater power nodes.

## Select temperatures

In the sketch's configuration block, leave `FIXED_TEMPERATURE_MODE = true`
and edit `TEST_TEMPERATURES_C`. Values are **degrees Celsius**, not milliC:

```cpp
const float TEST_TEMPERATURES_C[8] = {
  0.0f, 0.0f,    // Heater 1, ADC channels 0 and 1
  20.0f, 20.0f,  // Heater 2, ADC channels 2 and 3
  10.0f, 10.0f,  // Heater 3, ADC channels 4 and 5
  0.0f, 0.0f     // Heater 4, ADC channels 6 and 7
};
```

`TEST_INVALID_READINGS` forces selected channels to ADC code zero in fixed mode.
By default only sensor 8 (channel 7) is invalid; set its flag to `false` to
return its configured temperature normally. This tests invalid data, not an
I2C failure. A real 12-bit ADC sample cannot encode a count above 4095.

Both sensor temperatures are converted and averaged by BMS. Use equal values
in each pair to test that heater at a specific temperature.

The emulator uses the same nominal Honeywell 112-103FAJ-B01 constants as BMS:
10k ohms at 25 C, Beta 3669 K, and a 10k pull-up. Conversion uses 4096 counts
per full scale and clamps to codes 0..4095. Serial output at 115200 shows
the eight ADC codes actually being supplied.

| Set temperature | ADC code (approximately) | Expected at BMS 5 C / 15 C thresholds |
| --- | --- | --- |
| 0 C | 3093 | ON (HIGH) |
| 10 C | 2692 | Retain previous state |
| 20 C | 2262 | OFF (LOW) |
| 25 C | 2048 | OFF (LOW) |

| Heater | Sensors | Default condition | After BMS reset | If previously ON |
| --- | --- | --- | --- | --- |
| 1 | 1, 2 | Both 0 C: below 5 C | ON | ON |
| 2 | 3, 4 | Both 20 C: above 15 C | OFF | OFF |
| 3 | 5, 6 | Both 10 C: inside band | OFF | ON |
| 4 | 7, 8 | Sensor 7 at 0 C; sensor 8 invalid | OFF | ON |

Invalid data preserves that heater's previous state; it does not force it OFF.
The other three heaters continue to update when all I2C channel reads succeed.
The BMS/CDH temperature response reports signed millidegrees Celsius, not the
raw codes printed by the emulator. With these defaults, expect approximately
`[+3, +3, +20003, +20003, +10021, +10021, +3, -2147483648]`;
the last value is the invalid-temperature sentinel, not a physical temperature.
Small rounding differences are normal. At 25 C the nominal response is +25000.
FR5989/FR6989 outputs are P2.4, P2.5, P2.6, P2.7 respectively.
Allow at least one complete successful one-second BMS scan after changing data.

To verify the previous-ON column, first set all temperatures to 0 C and all
invalid flags to false. Let all four heaters turn ON, then restore the default
arrays and re-upload without resetting BMS. Outputs should be ON, OFF, ON, ON.

For a mixed-pair averaging check, clear the invalid flags and set a heater's
two temperatures to 0 C and 20 C. Their average is approximately 10 C, so the
heater retains its previous state even though each sensor is outside the band.

## Check hysteresis

Keep BMS powered and running between steps. Change both entries for a heater
and re-upload the emulator sketch; wait for communication and sampling to resume:

Clear the invalid flags before this sequence:

1. Set 0 C: heater turns ON.
2. Set 10 C: heater stays ON.
3. Set 20 C: heater turns OFF.
4. Set 10 C: heater stays OFF.
5. Set 0 C: heater turns ON again.

Repeat for all four pairs. Resetting BMS resets its remembered state to OFF.
Avoid exact threshold temperatures for this test because ADC quantization can
put the converted result just above or below the boundary.

For the original heating/cooling simulation, set `FIXED_TEMPERATURE_MODE = false`
and connect one heater's logic output to Pico GP15. This mode assigns the same
simulated temperature to all eight channels and reacts only to that one output.

If readings do not update, check the bus wiring/address and that BMS RTC sampling
is running. Failed scans retain the last heater state, including an ON state.
