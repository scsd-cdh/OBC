#include "heater_controller.h"

#define UINT12BIT_MAX 4095u

static bool adc_to_millicelsius(uint16_t adc, int32_t *temperature) {
    if (adc > UINT12BIT_MAX) {
        return false;
    }

    float calibrated = CALIB_GAIN * (float)adc + CALIB_OFFSET;
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
    if (!controller->output || CALIB_GAIN == 0.0f ||
            LOW_THRES_TEMP >= HIGH_THRES_TEMP) {
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
