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

## Script checks without hardware

```bash
python3 -m unittest discover -s tests -v
```

These checks mock the debugger and Docker; they do not program a device.

## Acquisition timeouts

Internal ADC waits have a 10,000-poll budget (`ADC_CONVERSION_POLL_LIMIT`).
Software I2C timer waits have a 5,000-poll budget (`SWI2C_TIMER_POLL_LIMIT`);
existing clock-stretch waits are also bounded. These are iteration limits,
not fixed millisecond deadlines, and remain finite if the peripheral clock stops.

A timeout aborts the current acquisition without changing heater PWM or blocking
heater commands. Failed readings are not published; telemetry does not yet include
validity or age fields. Later acquisition attempts are still allowed.

The shared `Read_ADC()` helper returns `ADC_READ_INVALID` (`UINT16_MAX`) on timeout.
Checked ADS7138 operations return `ADS7138_STATUS_TIMEOUT`; software I2C returns
`SWI2C_ERROR_TIMEOUT`. A NACK remains a transfer failure.
