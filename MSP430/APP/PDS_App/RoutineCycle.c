
#include "rtc_b.h"
#include "gpio.h"
#include "bsp.h"
#include "ADC_Read.h"

#include "RoutineCycle.h"

#include <stdint.h>
#include <stddef.h>

bool heartbeat_msgs_recieved = false;

static sVoltageConvData_t vConvData = {
    .Int_5v_vs_data = 0,
    .Reg_5v_vs_data = 0,
    .A_5v_vs_data = 0,
    .B_5v_vs_data = 0,
    .Temp_Sense_data = 0
};

static uint8_t ConvFlagsPorts[] = {    
    CONV_FLAG1_X_PLUS_PORT,
    CONV_FLAG2_X_PLUS_PORT,
    CONV_FLAG1_X_MINUS_PORT,
    CONV_FLAG2_X_MINUS_PORT,
    CONV_FLAG1_Y_PLUS_PORT,
    CONV_FLAG2_Y_PLUS_PORT,
    CONV_FLAG1_Y_MINUS_PORT,
    CONV_FLAG2_Y_MINUS_PORT,
};

static uint8_t ConvFlagsPin[] = {
    CONV_FLAG1_X_PLUS_PIN,
    CONV_FLAG2_X_PLUS_PIN,
    CONV_FLAG1_X_MINUS_PIN,
    CONV_FLAG2_X_MINUS_PIN,
    CONV_FLAG1_Y_PLUS_PIN,
    CONV_FLAG2_Y_PLUS_PIN,
    CONV_FLAG1_Y_MINUS_PIN,
    CONV_FLAG2_Y_MINUS_PIN
};

// Macros
inline uint16_t Read_INT_5V_VS(void) { return Read_ADC(ADC12_B_MEMORY_0); }
inline uint16_t Read_REG_5V_VS(void) { return Read_ADC(ADC12_B_MEMORY_1); }
inline uint16_t Read_A_5V_VS(void) { return Read_ADC(ADC12_B_MEMORY_2); }
inline uint16_t Read_B_5V_VS(void) { return Read_ADC(ADC12_B_MEMORY_3); }
inline uint16_t Read_TEMP_SENSE(void) { return Read_ADC(ADC12_B_MEMORY_4); }

inline void shutdownConvA(void) {
    GPIO_setOutputLowOnPin(CONV_RUN_A_PORT, CONV_RUN_A_PIN);
}
inline void shutdownConvB(void) {
    GPIO_setOutputLowOnPin(CONV_RUN_B_PORT, CONV_RUN_B_PIN);
}

void RoutineCycle_Process(void) {
    vConvData.Int_5v_vs_data = Read_INT_5V_VS();
    vConvData.Reg_5v_vs_data = Read_REG_5V_VS();
    vConvData.A_5v_vs_data = Read_A_5V_VS();
    vConvData.B_5v_vs_data = Read_B_5V_VS();
    vConvData.Temp_Sense_data = Read_TEMP_SENSE();

    size_t i;
    for (i = 0;i < sizeof(ConvFlagsPorts); i++) {
        if (GPIO_getInputPinValue(ConvFlagsPorts[i], ConvFlagsPin[i]) == GPIO_INPUT_PIN_HIGH) {
            RoutineCycle_Fault();
        }
    }
}

void RoutineCycle_Reboot(void) {
    shutdownConvA();
    shutdownConvB();
}

void RoutineCycle_Fault(void) {
    // TODO add more fault handling
    RoutineCycle_Reboot();
    return;
}

uint8_t* RoutineCycle_GetVoltageConvData(void) {
    return vConvData.bytes;
}

/*ISR that maintains LPM until 30 minutes has passed*/
#if defined(__TI_COMPILER_VERSION__) || defined(__IAR_SYSTEMS_ICC__)
#pragma vector=RTC_VECTOR
__interrupt
#elif defined(__GNUC__)
__attribute__((interrupt(RTC_VECTOR)))
#endif
void RTC_B_ISR (void)
{
    switch (__even_in_range(RTCIV,16))
    {
        case 2:     //RTCRDYIFG, triggered every second
            // TODO - move to another timer if it needs to be called more than once a second
            RoutineCycle_Process();
            break;
        case 4:     //RTCEVIFG, triggered every minute
            if (heartbeat_msgs_recieved == true) {
                heartbeat_msgs_recieved = false;
            } else {
                RoutineCycle_Reboot();
            }
            break;
        case 6:     //RTCAIFG, triggers at set alarm
            RoutineCycle_Reboot();
            break;
        default: 
            break;
    }
}
