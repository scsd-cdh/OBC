# BMS build and flashing

Build for the MCU fitted to your board (the scripts default to FR6989):

```bash
./build.sh msp430fr5989
```

Docker must be running for compilation. Outputs are `build/BMS.hex`,
`build/BMS.elf`, and `build/BMS.bin`.

## Flashing on macOS

`flash.sh` runs a native MSPDebug process on macOS. It does not use Docker
for flashing. Install [MSPDebug](https://formulae.brew.sh/formula/mspdebug):

```bash
brew install mspdebug
```

The `tilib` driver also needs TI's **macOS MSPDebugStack library**
(`libmsp430.dylib`, or `libmsp430.so` for older MSPDebug builds). This is a
separate dependency; installing the Homebrew formula alone does not install
it. TI's [MSPFlasher macOS installer](https://www.ti.com/tool/download/MSP430-FLASHER)
includes this library. Install it with its installation directory set to
`toolchain/mspflasher-macos` under the repository root; `flash.sh` detects this
location automatically, and `/toolchain/` is ignored by Git. You can also use
an existing TI MSP430 tool installation or build MSPDebugStack for macOS.
See [TI MSPDS](https://www.ti.com/tool/MSPDS)
and the [TI MSPDebugStack developer guide](https://www.ti.com/lit/ug/slau656d/slau656d.pdf).
Keep the accompanying libraries/resources together. The library and MSPDebug
must have matching CPU architectures; the Linux library in the build container
cannot be used on macOS.

If the library is outside the repository's `toolchain/mspflasher-macos`,
`/usr/local/lib`, or `/opt/homebrew/lib`, specify its directory:

```bash
export MSPDEBUG_TILIB_PATH="/absolute/path/to/TI/library/directory"
```

Connect the probe and identify its **debug** serial interface:

```bash
ls /dev/cu.usbmodem*
```

An MSP-FET or eZ-FET can expose both a debug interface and an application UART.
Use the debug interface; the script does not guess between them. Close other
debuggers that may own the probe.

```bash
./flash.sh --mcu msp430fr5989 --probe /dev/cu.usbmodemYOUR_DEBUG_INTERFACE
```

Replace the example interface with your actual device path. This programs
`build/BMS.hex`; `--mcu` does not rebuild or retarget an existing file.
The script translates `/dev/cu.usbmodem...` and `/dev/tty.usbmodem...` into
the `usbmodem...` port name expected by TI's macOS library.

An explicit firmware file can be outside the repository on macOS:

```bash
./flash.sh /absolute/path/to/BMS.hex --mcu msp430fr5989 --probe /dev/cu.usbmodemYOUR_DEBUG_INTERFACE
```

Alternatively, set `PROBE_DEV` instead of passing `--probe`. Set `MSPDEBUG`
to select a native executable outside your PATH.

For an interactive MSPDebug session, add `--debug`. For a native GDB server,
add `--gdb-server`; its default port is 2000 (`GDB_PORT` overrides it).
Connect a separate MSP430 GDB client to `localhost:2000`. The Docker-based
`debug.sh` wrapper has not been adapted for macOS; use `flash.sh` directly.

## Flashing on Linux and Windows

These platforms retain the Docker-based flashing path:

```bash
./flash.sh --mcu msp430fr5989
```

Windows requires Git Bash, Docker Desktop with WSL integration, usbipd-win,
a running WSL distribution, and a shared probe. Firmware files for Docker
flashing must be inside the repository.

## Checks without hardware

```bash
python3 -m unittest discover -s tests -v
```

These host checks use mocked hardware and do not program a device.

## Sampling, retries, and low-power operation

BMS uses the RTC read-ready interrupt to schedule a fresh voltage/current and
eight-channel thermistor scan every second. The timebase is the board's Y2
32.768 kHz crystal. Timer_A0 is not used for acquisition scheduling.

Initialization calls the shared `rtc_init()` unchanged, then `bms_rtc_config()`
for the BMS-specific setup. The latter holds the RTC while configuring it and
restarts it after selecting the one-second and retry events. Global interrupts
remain disabled until initialization finishes.

The schematic connects Y2 correctly to PJ.4/LFXIN and PJ.5/LFXOUT: FR5989-EP
64-pin RGC pins 57/58, or FR6989 LaunchPad 100-pin PZ pins 84/85. Firmware selects
their crystal function before releasing LOCKLPM5, enables LFXT in crystal mode,
and checks its fault flag with a bounded startup wait (about one second maximum).
It initializes a valid calendar and enables only the one-second read-ready event
while healthy. The calendar starts at an arbitrary valid epoch, not actual UTC.

If startup fails, RTC acquisition is not started, and CDH communication can still
run. There is no
automatic fallback to Timer_A0. Crystal oscillation and loading must be verified
on the assembled board; the schematic shows 15 pF capacitors C46/C47 on Y2.

The interrupt schedules work; main calls separate power and temperature collection
functions with interrupts enabled. Each function builds a local scan and publishes
it only on full success. If any temperature channel fails, the incomplete scan is
discarded and the next attempt starts again at channel 0. There is no per-channel
retry mask or saved partial scan.

Power and temperature have independent pending flags. A failed group is retried
in full on 125 ms slots until it succeeds; a successful group waits for the next
one-second scan. Power failure does not prevent a temperature attempt, and vice
versa. CDH receives the preceding complete snapshot while a group is failing
(initially zeros). Retry slots come from RTC prescaler 1 (128 Hz divided by 16);
its interrupt is enabled only while a group is pending and disabled after recovery.

Two telemetry buffers have fixed roles:

- The reading buffer holds the latest successfully completed power and temperature
  scans, published atomically. Incomplete scans are discarded.
- At each CDH request, the communication buffer copies the latest completed scan
  from the reading buffer before encoding the response. It remains fixed while
  that response is read, including reads split into header/body chunks. A new
  acquisition cannot change an in-flight response. Refresh no longer depends on
  CDH consuming a preceding response or sending STOP.

Each response is encoded into its own wire buffer, so acquisition cannot change
an in-flight response. The existing main/redundant CDH buses still share a single
protocol transaction context; masters should serialize their requests.

Main sleeps in LPM0 while idle, with an atomic check-and-sleep sequence. Hardware
I2C interrupts handle CDH traffic and return to sleep; scheduled acquisition wakes
main. LPM0 preserves SMCLK for heater PWM. Timer_A1 remains reserved for software
I2C and TB0 for heaters. Acquisition does not wait on UCB0's direction
bit and errors do not shut down or inhibit heater PWM.

## Acquisition timeouts

Internal ADC waits have a 10,000-poll budget (`ADC_CONVERSION_POLL_LIMIT`).
Software I2C timer waits have a 5,000-poll budget (`SWI2C_TIMER_POLL_LIMIT`);
existing clock-stretch waits are also bounded. These are iteration limits,
not fixed millisecond deadlines, and remain finite if the peripheral clock stops.

A timeout ends the failed operation without changing heater PWM or blocking
heater commands. The group stays pending for the next retry slot. Failed
readings are not published; telemetry does not yet include validity or age fields.

The shared `Read_ADC()` helper returns `ADC_READ_INVALID` (`UINT16_MAX`) on timeout.
Checked ADS7138 operations return `ADS7138_STATUS_TIMEOUT`; software I2C returns
`SWI2C_ERROR_TIMEOUT`. A NACK remains a transfer failure.

## Automatic heater control

`MIDDLEWARE/HEATER_CONTROLLER` provides a reusable controller for two ADC readings
and one heater output. BMS initializes four instances, commanding all outputs LOW
at startup, and updates them only after a complete successful eight-channel
temperature scan:

| Heater | Sensor numbers | ADS channels | FR5989/FR6989 output |
| --- | --- | --- | --- |
| 1 | 1, 2 | 0, 1 | P2.4 / TB0.3 |
| 2 | 3, 4 | 2, 3 | P2.5 / TB0.4 |
| 3 | 5, 6 | 4, 5 | P2.6 / TB0.5 |
| 4 | 7, 8 | 6, 7 | P2.7 / TB0.6 |

Configure these shared defines in
[`heater_controller.h`](../../MIDDLEWARE/HEATER_CONTROLLER/heater_controller.h):

- `LOW_THRES_TEMP` and `HIGH_THRES_TEMP`: signed integer milli degrees Celsius.
- `CALIB_GAIN`: linear slope in milli degrees Celsius per ADC count; it may be
  positive or negative and fractional.
- `CALIB_OFFSET`: linear offset in milli degrees Celsius.

Each ADC reading is converted using `CALIB_GAIN * adc + CALIB_OFFSET`, truncated
to integer milliC, then the two temperatures are averaged. Below LOW turns the
heater ON; above HIGH turns it OFF. Between thresholds, including equality, it
retains its previous state. The output uses Timer_B static output mode for true
0% (continuously LOW) or 100% (continuously HIGH).

The defaults are deliberately unconfigured: thresholds, gain, and offset are
zero. Controllers remain OFF until a nonzero gain and LOW < HIGH are configured.
No physical calibration or operating temperatures are supplied by these defaults.
Invalid ADC counts or out-of-range conversions are ignored. A failed scan leaves
all controller states unchanged, including a heater which was already ON.

The manual CDH heater-duty buffer, callback, and dispatch are temporarily disabled
with `#if 0` in `BMS.c`. Manual duty requests have no response while disabled.
The ASN.1 message definitions are retained for future re-enabling.

Run host tests from the repository root:

```bash
python3 -m unittest discover -s MIDDLEWARE/HEATER_CONTROLLER/tests -v
```
