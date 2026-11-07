#ifndef HEATER_CONTROLLER_H
#define HEATER_CONTROLLER_H

#include <stdbool.h>
#include <stdint.h>

// Configure these before use. Equal thresholds and zero gain are placeholders;
// with these defaults, controllers stay OFF and update() returns false.
#ifndef LOW_THRES_TEMP
#define LOW_THRES_TEMP 0           // milli degrees Celsius
#endif
#ifndef HIGH_THRES_TEMP
#define HIGH_THRES_TEMP 0          // milli degrees Celsius
#endif
#ifndef CALIB_GAIN
#define CALIB_GAIN 0.0f            // milli degrees Celsius per ADC count
#endif
#ifndef CALIB_OFFSET
#define CALIB_OFFSET 0.0f          // milli degrees Celsius
#endif

typedef void (*HeaterOutput)(uint16_t heater, bool on);

typedef struct {
    uint16_t heater;
    HeaterOutput output;
    bool is_on;
} HeaterController;

void heater_controller_init(HeaterController *controller, uint16_t heater,
                            HeaterOutput output);

// Inputs are two successful 12-bit ADC readings from the same completed scan.
// Convert each to milliC using gain * adc + offset, then average in milliC.
// Below LOW turns ON; above HIGH turns OFF; equality/in-band retains state.
// Returns false on invalid configuration or conversion, preserving the output.
bool heater_controller_update(HeaterController *controller,
                              uint16_t adc_a, uint16_t adc_b);

#endif
