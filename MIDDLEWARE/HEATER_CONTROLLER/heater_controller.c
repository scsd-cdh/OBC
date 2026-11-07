#include "heater_controller.h"
#include <assert.h>
#include <math.h>

#define UINT12BIT_MAX 4095u
#define ADC_COUNTS_PER_FULL_SCALE 4096.0f // 2^12; ADS7138 uses AVCC as reference

static bool adc_to_millicelsius(uint16_t adc, int32_t *temperature) {
    if (adc == 0u || adc > UINT12BIT_MAX) {
        return false;
    }

    // AVCC cancels because the divider supply and ADC reference are the same.
    // Derived from the divider on BMS schematic PDF p. 9 (sheet 7/19) and
    // ADS7138 datasheet p. 14, sections 8.3.2-8.3.3, Eq. (1): LSB = AVDD/4096.
    // https://www.ti.com/lit/ds/symlink/ads7138.pdf#page=14
    float resistance = THERMISTOR_PULLUP_OHMS * (float)adc /
                       (ADC_COUNTS_PER_FULL_SCALE - (float)adc);

    // 1/T = 1/T25 + ln(R/R25)/Beta; temperatures in this equation are kelvin.
    // Beta-model reference: Analog Devices, Thermistor-Based Temperature
    // Sensing System, Part 1, p. 3, "Linearization: Beta vs. Steinhart-Hart".
    // https://www.analog.com/media/en/analog-dialogue/volume-56/number-3/thermistor-temperature-sensing-system-part-1.pdf#page=3
    // Honeywell's one-page datasheet supplies R25/Beta, not this equation.
    float inverse_kelvin = 1.0f / THERMISTOR_REFERENCE_K +
        logf(resistance / THERMISTOR_R25_OHMS) / THERMISTOR_BETA_K;

    if (!(inverse_kelvin > 0.0f) || !isfinite(inverse_kelvin)) {
        return false;
    }

    float calibrated = (1.0f / inverse_kelvin - CELSIUS_TO_KELVIN) *
                       MILLICELSIUS_PER_CELSIUS;
    // Ordered comparisons also reject NaN and infinity. Upper bound is exclusive
    // because INT32_MAX rounds up to 2^31 in single precision.
    if (!(calibrated >= (float)INT32_MIN && calibrated < (float)INT32_MAX)) {
        return false;
    }

    *temperature = (int32_t)calibrated;
    return true;
}

void heater_controller_init(HeaterController *controller, uint16_t heater,
                            HeaterOutput output) {
    controller->heater = heater;
    controller->output = output;
    controller->is_on = false;
    if (output) {
        output(heater, false);
    }
}

bool heater_controller_update(HeaterController *controller,
                              uint16_t adc_a, uint16_t adc_b) 
{
    // Check fixed thermistor configuration and thresholds in debug builds only.
    assert(THERMISTOR_R25_OHMS > 0.0f && isfinite(THERMISTOR_R25_OHMS));
    assert(THERMISTOR_BETA_K > 0.0f && isfinite(THERMISTOR_BETA_K));
    assert(THERMISTOR_PULLUP_OHMS > 0.0f && isfinite(THERMISTOR_PULLUP_OHMS));
    assert(LOW_THRES_TEMP < HIGH_THRES_TEMP);

    if (!controller->output) {
        return false;
    }

    int32_t first, second;
    if (!adc_to_millicelsius(adc_a, &first) ||
        !adc_to_millicelsius(adc_b, &second)) 
    {
        return false;
    }
    
    int32_t average = (int32_t)(((int64_t)first + second) / 2);
    bool on = controller->is_on;
    if (average < LOW_THRES_TEMP) {
        on = true;
    } else if (average > HIGH_THRES_TEMP) {
        on = false;
    }

    if (on != controller->is_on) 
    {
        controller->is_on = on;
        controller->output(controller->heater, on);
    }

    return true;
}
