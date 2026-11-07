#ifndef HEATER_CONTROLLER_H
#define HEATER_CONTROLLER_H

#include <stdbool.h>
#include <stdint.h>

// Heater hysteresis thresholds in millidegrees Celsius; LOW must be below HIGH.
#ifndef LOW_THRES_TEMP
#define LOW_THRES_TEMP 5000           // milli degrees Celsius
#endif
#ifndef HIGH_THRES_TEMP
#define HIGH_THRES_TEMP 15000          // milli degrees Celsius
#endif
// Honeywell 112-103FAJ-B01 NTC thermistor (nominal resistance tolerance: +/-20%).
// Update these values from the new sensor datasheet if the part changes.
// Datasheet p. 1: Product Specifications and 25/85 Beta in the description.
// https://www.farnell.com/datasheets/1880963.pdf#page=1
// Nominal resistance in ohms at 25 degrees Celsius (298.15 kelvin).
#ifndef THERMISTOR_R25_OHMS
#define THERMISTOR_R25_OHMS 10000.0f
#endif
// Beta constant in kelvin, specified between 25 and 85 degrees Celsius.
#ifndef THERMISTOR_BETA_K
#define THERMISTOR_BETA_K 3669.0f
#endif
// Board R23-R30: pull-up resistance in ohms, from ADC input to AVCC.
// BMS Central Board schematic: PDF p. 9, sheet 7/19 (Heaters).
// Thermistor connects from input to ground; AVCC is also the ADC reference.
#ifndef THERMISTOR_PULLUP_OHMS
#define THERMISTOR_PULLUP_OHMS 10000.0f
#endif

#define THERMISTOR_REFERENCE_K 298.15f  // 25 degrees Celsius in kelvin
#define CELSIUS_TO_KELVIN 273.15f       // Kelvin offset from Celsius
#define MILLICELSIUS_PER_CELSIUS 1000.0f

typedef void (*HeaterOutput)(uint16_t heater, bool on);

typedef struct {
    uint16_t heater;
    HeaterOutput output;
    bool is_on;
} HeaterController;

void heater_controller_init(HeaterController *controller, uint16_t heater,
                            HeaterOutput output);

// Inputs are two successful 12-bit ADC readings from the same completed scan.
// Convert each using the nominal NTC Beta equation, then average in milliC.
// This is an approximation; sensor tolerance requires individual calibration
// for accurate temperatures. Zero/out-of-range ADC counts are rejected.
// Below LOW turns ON; above HIGH turns OFF; equality/in-band retains state.
// Invalid thermistor constants or thresholds trigger assert() unless NDEBUG is defined.
// With NDEBUG, the caller must ensure constants and thresholds are valid.
// Returns false on missing output or invalid conversion, preserving output.
bool heater_controller_update(HeaterController *controller,
                              uint16_t adc_a, uint16_t adc_b);

#endif
