#include "BMS.h"
#include "ADC_Read.h"
#include "msp430.h"
#include "gpio.h"
#include "PWM.h"
#include <stdint.h>
#include "swi2c.h"
#include "ads7138irter.h"
#if defined (__MSP430FR5989__)
#include "rtc_c.h"
#elif defined (__MSP430FR5969__)
#include "rtc_b.h"
#endif

// ADC pins
// Current sensing 
// 1 and 2 does in fact refer to battery number
#define I_SENSE_VUR_1_CP_PIN      ADC12_B_INPUT_A10 // PIN 42 5989
#define I_SENSE_VUR_2_CP_PIN      ADC12_B_INPUT_A0  // PIN 39 5989

#define I_SENSE_CHR_1_CP_PIN      ADC12_B_INPUT_A9  // PIN 41 5989
#define I_SENSE_CHR_2_CP_PIN      ADC12_B_INPUT_A8  // PIN 40 5989

// Voltage sensing
#define V_CELL_1A_CP_PIN          ADC12_B_INPUT_A13 // PIN 45 5989
#define V_CELL_1B_CP_PIN          ADC12_B_INPUT_A12 // PIN 44 5989
#define V_CELL_2A_CP_PIN          ADC12_B_INPUT_A3  // PIN 36 5989
#define V_CELL_2B_CP_PIN          ADC12_B_INPUT_A2  // PIN 37 5989
#define V_BATTPACK_1_CP_PIN       ADC12_B_INPUT_A11 // PIN 43 5989
#define V_BATTPACK_2_CP_PIN       ADC12_B_INPUT_A1  // PIN 38 5989

// ADC memory buffers
#define I_SENSE_VUR_1_CP_MEM      ADC12_B_MEMORY_0
#define I_SENSE_VUR_2_CP_MEM      ADC12_B_MEMORY_1
#define I_SENSE_CHR_1_CP_MEM      ADC12_B_MEMORY_2
#define I_SENSE_CHR_2_CP_MEM      ADC12_B_MEMORY_3
#define V_CELL_1A_CP_MEM          ADC12_B_MEMORY_4
#define V_CELL_1B_CP_MEM          ADC12_B_MEMORY_5
#define V_CELL_2A_CP_MEM          ADC12_B_MEMORY_6
#define V_CELL_2B_CP_MEM          ADC12_B_MEMORY_7
#define V_BATTPACK_1_CP_MEM       ADC12_B_MEMORY_8  
#define V_BATTPACK_2_CP_MEM       ADC12_B_MEMORY_9

// GPIO pins
#define OVP_FLAG_1A_PIN GPIO_PIN0 
#define UVP_FLAG_1B_PIN GPIO_PIN1 
#define UVP_FLAG_1A_PIN GPIO_PIN2 
#define OVP_FLAG_1B_PIN GPIO_PIN3 
#define OCP_FLAG_1_PIN  GPIO_PIN0 
#define OVP_FLAG_2A_PIN GPIO_PIN3 
#define OVP_FLAG_2B_PIN GPIO_PIN4 
#define UVP_FLAG_2A_PIN GPIO_PIN5 
#define UVP_FLAG_2B_PIN GPIO_PIN6 
#define OCP_FLAG_2_PIN  GPIO_PIN7 


// PWM designated output pins
#define HEATER_PWM_PORT GPIO_PORT_P2 // Port
// Pins
#if defined (__MSP430FR5989__)
#define HEATER1_PWM_PIN GPIO_PIN4 
#define HEATER2_PWM_PIN GPIO_PIN5
#define HEATER3_PWM_PIN GPIO_PIN6
#define HEATER4_PWM_PIN GPIO_PIN7 

#define HEATER1_CCR TIMER_B_CAPTURECOMPARE_REGISTER_3
#define HEATER2_CCR TIMER_B_CAPTURECOMPARE_REGISTER_4
#define HEATER3_CCR TIMER_B_CAPTURECOMPARE_REGISTER_5
#define HEATER4_CCR TIMER_B_CAPTURECOMPARE_REGISTER_6

#elif defined (__MSP430FR5969__)
#define HEATER1_PWM_PIN GPIO_PIN1 
#define HEATER2_PWM_PIN GPIO_PIN5
#define HEATER3_PWM_PIN GPIO_PIN6
#define HEATER4_PWM_PIN GPIO_PIN0

#define HEATER1_CCR TIMER_B_CAPTURECOMPARE_REGISTER_0
#define HEATER2_CCR TIMER_B_CAPTURECOMPARE_REGISTER_0
#define HEATER3_CCR TIMER_B_CAPTURECOMPARE_REGISTER_1
#define HEATER4_CCR TIMER_B_CAPTURECOMPARE_REGISTER_6
#endif  

// External ADC 
#define ADS7138_ADDR 0x10  // Default I2C address

// App communications
#define SLAVE_ADDR 0x08

// Static data buffers to be sent back to CDH
static const SystemStatusResp_t sSystemStatus = {
    .runtime = 0x12,
    .fw_version = 0xA, 
};

static CurrentResp_t sCurrentDraw = {
    .isense1 = 0,
    .isense2 = 0,
};

static CurrentResp_t sCurrentCharge = {
    .isense1 = 0,
    .isense2 = 0,
};

static VoltageResp_t sVoltageBattery1 = {
    .vcell_a = 0,
    .vcell_b = 0,
};

static VoltageResp_t sVoltageBattery2 = {
    .vcell_a = 0,
    .vcell_b = 0,
};

static CombinedVoltageResp_t sCombinedBatteryVoltage = {
    .vbatt1 = 0,
    .vbatt2 = 0,
};

static Flag_t sFlags = {
    .val = 0x00,
};

// Need 2 buffers since max buffer size for tinyprotocol is 11 and we have 8 16 bit 
static uint8_t sExtADCBuffer03[8] = {};
static uint8_t sExtADCBuffer47[8] = {};


// Global SWI2C config "descriptor"
static SWI2C_Descriptor sADS7138_SWI2C_Descriptor;

static void initADCs() 
{
    ADC_initMultiple();

    ADC12_B_disableConversions(ADC12_B_BASE, 1);
    // Selects what pin to get mapped to what ADC memory thing
    ADC_PinSelect(I_SENSE_VUR_1_CP_PIN,     I_SENSE_VUR_1_CP_MEM);
    ADC_PinSelect(I_SENSE_VUR_2_CP_PIN,     I_SENSE_VUR_2_CP_MEM);
    ADC_PinSelect(I_SENSE_CHR_1_CP_PIN,     I_SENSE_CHR_1_CP_MEM);
    ADC_PinSelect(I_SENSE_CHR_2_CP_PIN,     I_SENSE_CHR_2_CP_MEM);
    ADC_PinSelect(V_CELL_1A_CP_PIN,         V_CELL_1A_CP_MEM);
    ADC_PinSelect(V_CELL_1B_CP_PIN,         V_CELL_1B_CP_MEM);
    ADC_PinSelect(V_CELL_2A_CP_PIN,         V_CELL_2A_CP_MEM);
    ADC_PinSelect(V_CELL_2B_CP_PIN,         V_CELL_2B_CP_MEM);
    ADC_PinSelect(V_BATTPACK_1_CP_PIN,      V_BATTPACK_1_CP_MEM);

    // NOTE: You need to set the end of sequence to the last ADC pin and set sequence of channels mode!!! Otherwise 
    // it will only read the first pin you set (i.e. MEM0!!!)
    // Manually select last pin and set ENDOFSEQUENCE
    ADC12_B_configureMemoryParam eos = {
        .memoryBufferControlIndex = V_BATTPACK_2_CP_MEM,         // MEM9
        .inputSourceSelect        = V_BATTPACK_2_CP_PIN,
        .refVoltageSourceSelect   = ADC12_B_VREFPOS_AVCC_VREFNEG_VSS,
        .endOfSequence            = ADC12_B_ENDOFSEQUENCE,
        .windowComparatorSelect   = ADC12_B_WINDOW_COMPARATOR_DISABLE,
        .differentialModeSelect   = ADC12_B_DIFFERENTIAL_MODE_DISABLE
    };
    ADC12_B_configureMemory(ADC12_B_BASE, &eos);

    /* 4. Sequence‑of‑channels mode, one pass per trigger */
    // This allows us to use more than one MEM
    ADC12CTL1 |= ADC12CONSEQ_1;   // driverlib name: ADC12_B_SEQUENCEOFCHANNELS
}

static void initGPIO()
{
    WDTCTL = WDTPW | WDTHOLD;   // Stop watchdog timer
    // Configure GPIO
    
    P1DIR |= BIT0 | BIT1;
    P1OUT &= ~(BIT0 | BIT1);         // P1 setup for LED & reset output

    P1SEL0 |= BIT6 | BIT7;                    // I2C pins
    P1SEL1 &= ~(BIT6 | BIT7);

    // Disable the GPIO power-on default high-impedance mode to activate
    // previously configured port settings
    PM5CTL0 &= ~LOCKLPM5;

    // Flag pins
    // MSP430FR5989 Pins 10 through 13 use GPIO_PORT_P5

    GPIO_setAsInputPin(GPIO_PORT_P5, OVP_FLAG_1A_PIN); // MSP430FR5989 10 P5.0
    GPIO_setAsInputPin(GPIO_PORT_P5, UVP_FLAG_1B_PIN); // MSP430FR5989 11
    GPIO_setAsInputPin(GPIO_PORT_P5, OVP_FLAG_1A_PIN); // MSP430FR5989 12
    GPIO_setAsInputPin(GPIO_PORT_P5, OVP_FLAG_1A_PIN); // MSP430FR5989 13
    // MSP430FR5989 Pins 14, 25 through 29 use GPIO_PORT_P3 
    GPIO_setAsInputPin(GPIO_PORT_P3, OCP_FLAG_1_PIN);  // MSP430FR5989 14 P3.0
    GPIO_setAsInputPin(GPIO_PORT_P3, OVP_FLAG_2A_PIN); // MSP430FR5989 25
    GPIO_setAsInputPin(GPIO_PORT_P3, OVP_FLAG_2B_PIN); // MSP430FR5989 26
    GPIO_setAsInputPin(GPIO_PORT_P3, UVP_FLAG_2A_PIN); // MSP430FR5989 27
    GPIO_setAsInputPin(GPIO_PORT_P3, UVP_FLAG_2B_PIN); // MSP430FR5989 28
    GPIO_setAsInputPin(GPIO_PORT_P3, OCP_FLAG_2_PIN);  // MSP430FR5989 29

    // Init PWM 
    PWM_PinSelect(HEATER_PWM_PORT, HEATER1_PWM_PIN);
    PWM_PinSelect(HEATER_PWM_PORT, HEATER2_PWM_PIN);
    PWM_PinSelect(HEATER_PWM_PORT, HEATER3_PWM_PIN);
    PWM_PinSelect(HEATER_PWM_PORT, HEATER4_PWM_PIN);
}

void initClockTo16MHz()
{
    // Configure one FRAM waitstate as required by the device datasheet for MCLK
    // operation beyond 8MHz _before_ configuring the clock system.
    FRCTL0 = FRCTLPW | NWAITS_1;

    // Clock System Setup
    CSCTL0_H = CSKEY_H;                     // Unlock CS registers
    CSCTL1 = DCOFSEL_0;                     // Set DCO to 1MHz
    // Set SMCLK = MCLK = DCO, ACLK = LFXTCLK (VLOCLK if unavailable)
    CSCTL2 = SELA__LFXTCLK | SELS__DCOCLK | SELM__DCOCLK;
    // Per Device Errata set divider to 4 before changing frequency to
    // prevent out of spec operation from overshoot transient
    CSCTL3 = DIVA__4 | DIVS__4 | DIVM__4;   // Set all corresponding clk sources to divide by 4 for errata
    CSCTL1 = DCOFSEL_4 | DCORSEL;           // Set DCO to 16MHz
    // Delay by ~10us to let DCO settle. 60 cycles = 20 cycles buffer + (10us / (1/4MHz))
    __delay_cycles(60);
    CSCTL3 = DIVA__1 | DIVS__1 | DIVM__1;   // Set all dividers to 1 for 16MHz operation
    CSCTL0_H = 0;                           // Lock CS registers
}

#if defined (__MSP430FR5989__)
static void initRTC()
{
    Calendar currentTime;

    //Setup for Calendar
    currentTime.Seconds    = 0x00;
    currentTime.Minutes    = 0x00;
    currentTime.Hours      = 0x00;
    currentTime.DayOfWeek  = 0x00;
    currentTime.DayOfMonth = 0x00;
    currentTime.Month      = 0x00;
    currentTime.Year       = 0x7E9;  // 2025

    //Initialize Calendar Mode of RTC
    RTC_C_initCalendar(RTC_C_BASE, &currentTime, RTC_C_FORMAT_BCD);

    //Setup Calendar Alarm for 30 minutes after start.
    RTC_C_configureCalendarAlarmParam param = {0};
    param.minutesAlarm      = 0x2;  // Currently set to 2 minute for testing - TODO change to 24 hours
    param.hoursAlarm        = 0x0;
    param.dayOfWeekAlarm    = 0x0;
    param.dayOfMonthAlarm   = 0x0;
    RTC_C_configureCalendarAlarm(RTC_C_BASE, &param);

    RTC_C_clearInterrupt(RTC_C_BASE,
        RTC_C_CLOCK_READ_READY_INTERRUPT +
        RTC_C_TIME_EVENT_INTERRUPT +
        RTC_C_CLOCK_ALARM_INTERRUPT
        );
    //Enable interrupt for RTC Ready Status, which asserts when the RTC
    //Calendar registers are ready to read.
    //Also, enable interrupts for the Calendar alarm and Calendar event.
    RTC_C_enableInterrupt(RTC_C_BASE,
        RTC_C_CLOCK_READ_READY_INTERRUPT +
        RTC_C_TIME_EVENT_INTERRUPT +
        RTC_C_CLOCK_ALARM_INTERRUPT
    );

    //Start RTC Clock
    RTC_C_startClock(RTC_C_BASE);
}

#elif defined (__MSP430FR5969__)
static void initRTC()
{
    Calendar currentTime;

    //Setup for Calendar
    currentTime.Seconds    = 0x00;
    currentTime.Minutes    = 0x00;
    currentTime.Hours      = 0x00;
    currentTime.DayOfWeek  = 0x00;
    currentTime.DayOfMonth = 0x00;
    currentTime.Month      = 0x00;
    currentTime.Year       = 0x7E9;  // 2025

    //Initialize Calendar Mode of RTC
    RTC_B_initCalendar(RTC_B_BASE, &currentTime, RTC_B_FORMAT_BCD);

    //Setup Calendar Alarm for 30 minutes after start.
    RTC_B_configureCalendarAlarmParam param = {0};
    param.minutesAlarm      = 0x2;  // Currently set to 2 minute for testing - TODO change to 24 hours
    param.hoursAlarm        = 0x0;
    param.dayOfWeekAlarm    = 0x0;
    param.dayOfMonthAlarm   = 0x0;
    RTC_B_configureCalendarAlarm(RTC_B_BASE, &param);

    RTC_B_clearInterrupt(RTC_B_BASE,
        RTC_B_CLOCK_READ_READY_INTERRUPT +
        RTC_B_TIME_EVENT_INTERRUPT +
        RTC_B_CLOCK_ALARM_INTERRUPT
        );
    //Enable interrupt for RTC Ready Status, which asserts when the RTC
    //Calendar registers are ready to read.
    //Also, enable interrupts for the Calendar alarm and Calendar event.
    RTC_B_enableInterrupt(RTC_B_BASE,
        RTC_B_CLOCK_READ_READY_INTERRUPT +
        RTC_B_TIME_EVENT_INTERRUPT +
        RTC_B_CLOCK_ALARM_INTERRUPT
    );

    //Start RTC Clock
    RTC_B_startClock(RTC_B_BASE);
}
#endif

void initBSP()
{
    initClockTo16MHz();
    initGPIO();
    initRTC();
    initADCs();
}

static void I2C_Proc_RX_Data(uint8_t data);

static uint16_t SendTelemetryResponse(uint8_t request)
{
    uint8_t bytes[TINYPROTOCOL_MAX_PACKET_SIZE];
    uint8_t count = 0;
    uint8_t* pbyte = bytes;

    while(TINYPROTOCOL_TelemetryBytesLeft() > 0) {
        int16_t result = TINYPROTOCOL_ReadNextTelemetryByte(pbyte);
        volatile uint8_t byte = *pbyte;
        if (result == ETINYPROTOCOL_SUCCESS) {
            pbyte++;
            count++;
        } else {
            return result;
        }
    }
    
    transmitI2C(bytes, count);

    return ETINYPROTOCOL_SUCCESS;
} 

static int16_t ProcessTelemetryRequest(uint8_t request)
{
    return SendTelemetryResponse(request);
}

// FIXME: Need to do research on what values the heaters actually expect... 
// These are basically random
// For now CDH is responsible for deciding what PWM values to send
static void SendPWM(const uint8_t* buffer, uint8_t size)
{
    PWM_Generate(1000, buffer[0], HEATER1_CCR);
    PWM_Generate(1000, buffer[1], HEATER2_CCR);
    PWM_Generate(1000, buffer[2], HEATER3_CCR);
    PWM_Generate(1000, buffer[3], HEATER4_CCR);
}

static int16_t ProcessTelecommand(uint8_t command, const uint8_t* buffer, uint8_t size)
{
    switch (command) {
        case BMS_HEATERS_CONTROLLER_ID:
            SendPWM(buffer, size);
            break;
        
        default: 
            return ETINYPROTOCOL_INVALID_CMD_ID;
    }
    return ETINYPROTOCOL_SUCCESS;
}

const struct TINYPROTOCOL_Config protocolConfig =
{
    .TINYPROTOCOL_ProcessTelecommand = ProcessTelecommand,
    .TINYPROTOCOL_ProcessTelemetryRequest = ProcessTelemetryRequest,
    .TINYPROTOCOL_WriteBuffer = transmitI2C
};

static void I2C_Proc_RX_Data(uint8_t data)
{
    TINYPROTOCOL_ParseByte(&protocolConfig, data);
}

void InitAppComm()
{
    sI2cConfigCb_t i2cConfig = {
      .Rx_Proc_Data = I2C_Proc_RX_Data,
        .slave_addr = SLAVE_ADDR,
    };

    initI2C(&i2cConfig);  

    // SW I2C
    sADS7138_SWI2C_Descriptor.sda_port_out =   &P4OUT;
    sADS7138_SWI2C_Descriptor.sda_port_in =    &P4IN;
    sADS7138_SWI2C_Descriptor.sda_port_dir =   &P4DIR;
    sADS7138_SWI2C_Descriptor.sda_pin =        GPIO_PIN1;
    sADS7138_SWI2C_Descriptor.scl_port_out =   &P4OUT;
    sADS7138_SWI2C_Descriptor.scl_port_in =    &P4IN;
    sADS7138_SWI2C_Descriptor.scl_port_dir =   &P4DIR;
    sADS7138_SWI2C_Descriptor.scl_pin =        GPIO_PIN0;
    P4SEL0 &= ~(BIT0 | BIT1);
    P4SEL1 &= ~(BIT0 | BIT1); 
    P1DIR |= BIT0 | BIT1;

    ADS7138IRTER_Initialize(&sADS7138_SWI2C_Descriptor);

    TINYPROTOCOL_Initialize();
    TINYPROTOCOL_RegisterTelemetryChannel(BMS_SYSTEM_STATUS_ID, sSystemStatus.buffer , sizeof(sSystemStatus.buffer));
    TINYPROTOCOL_RegisterTelemetryChannel(BMS_FLAG_ID, sFlags.buffer, sizeof(sFlags.buffer));
    TINYPROTOCOL_RegisterTelemetryChannel(BMS_CURRENT_DRAW_ID, sCurrentDraw.buffer, sizeof(sCurrentDraw.buffer));
    TINYPROTOCOL_RegisterTelemetryChannel(BMS_CURRENT_CHARGE_ID, sCurrentCharge.buffer, sizeof(sCurrentCharge.buffer));
    TINYPROTOCOL_RegisterTelemetryChannel(BMS_VOLTAGE_BATTERY1_ID, sVoltageBattery1.buffer, sizeof(sVoltageBattery1.buffer));
    TINYPROTOCOL_RegisterTelemetryChannel(BMS_VOLTAGE_BATTERY2_ID, sVoltageBattery2.buffer, sizeof(sVoltageBattery2.buffer));
    TINYPROTOCOL_RegisterTelemetryChannel(BMS_VOLTAGE_COMBINED_ID, sCombinedBatteryVoltage.buffer, sizeof(sCombinedBatteryVoltage.buffer));
    TINYPROTOCOL_RegisterTelemetryChannel(BMS_THERMISTOR03_DATA_ID, sExtADCBuffer03, sizeof(sExtADCBuffer03));
    TINYPROTOCOL_RegisterTelemetryChannel(BMS_THERMISTOR47_DATA_ID, sExtADCBuffer47, sizeof(sExtADCBuffer47));

    TINYPROTOCOL_RegisterTelecommand(BMS_HEATERS_CONTROLLER_ID, 4);
}

// ISR 
static inline void RoutineCycle_Process()
{
    // Collect ADC data and put it into buffers
    // Must do this this way (all at once) if using SEQOFCHANNELS mode
    /* trigger the ten‑channel sweep */
    ADC12_B_startConversion(ADC12_B_BASE,
        ADC12_B_START_AT_ADC12MEM0,
        ADC12_B_SEQOFCHANNELS);        // ENC+SC, walk MEM0→MEM9

    while (ADC12_B_isBusy(ADC12_B_BASE));

    /* pull results out */
    sCurrentDraw.isense1             = ADC12_B_getResults(ADC12_B_BASE, I_SENSE_VUR_1_CP_MEM);
    sCurrentDraw.isense2             = ADC12_B_getResults(ADC12_B_BASE, I_SENSE_VUR_2_CP_MEM);
    sCurrentCharge.isense1           = ADC12_B_getResults(ADC12_B_BASE, I_SENSE_CHR_1_CP_MEM);
    sCurrentCharge.isense2           = ADC12_B_getResults(ADC12_B_BASE, I_SENSE_CHR_2_CP_MEM);
    sVoltageBattery1.vcell_a         = ADC12_B_getResults(ADC12_B_BASE, V_CELL_1A_CP_MEM);
    sVoltageBattery1.vcell_b         = ADC12_B_getResults(ADC12_B_BASE, V_CELL_1B_CP_MEM);
    sVoltageBattery2.vcell_a         = ADC12_B_getResults(ADC12_B_BASE, V_CELL_2A_CP_MEM);
    sVoltageBattery2.vcell_b         = ADC12_B_getResults(ADC12_B_BASE, V_CELL_2B_CP_MEM);
    sCombinedBatteryVoltage.vbatt1   = ADC12_B_getResults(ADC12_B_BASE, V_BATTPACK_1_CP_MEM);
    sCombinedBatteryVoltage.vbatt2   = ADC12_B_getResults(ADC12_B_BASE, V_BATTPACK_2_CP_MEM);

    // Collect GPIO flag data and put it into buffer
    volatile uint8_t ovp1Aval = GPIO_getInputPinValue(GPIO_PORT_P5, OVP_FLAG_1A_PIN);
    sFlags.val |= ovp1Aval << 9;

    volatile uint8_t uvp1Bval = GPIO_getInputPinValue(GPIO_PORT_P5, UVP_FLAG_1B_PIN);
    sFlags.val |= uvp1Bval << 8;

    volatile uint8_t uvp1Aval = GPIO_getInputPinValue(GPIO_PORT_P5, UVP_FLAG_1A_PIN);
    sFlags.val |= uvp1Aval << 7;

    volatile uint8_t ovp1Bval = GPIO_getInputPinValue(GPIO_PORT_P5, OVP_FLAG_1B_PIN);
    sFlags.val |= ovp1Bval << 6;

    volatile uint8_t ocp1val = GPIO_getInputPinValue(GPIO_PORT_P3, OCP_FLAG_1_PIN);
    sFlags.val |= ocp1val << 5;

    volatile uint8_t ovp2Aval = GPIO_getInputPinValue(GPIO_PORT_P3, OVP_FLAG_2A_PIN);
    sFlags.val |= ovp2Aval << 4;

    volatile uint8_t ovp2Bval = GPIO_getInputPinValue(GPIO_PORT_P3, OVP_FLAG_2B_PIN);
    sFlags.val |= ovp2Bval << 3;

    volatile uint8_t uvp2Aval = GPIO_getInputPinValue(GPIO_PORT_P3, UVP_FLAG_2A_PIN);
    sFlags.val |= uvp2Aval << 2;

    volatile uint8_t uvp2Bval = GPIO_getInputPinValue(GPIO_PORT_P3, UVP_FLAG_2B_PIN);
    sFlags.val |= uvp2Bval << 1;

    volatile uint8_t ocp2val = GPIO_getInputPinValue(GPIO_PORT_P3, OCP_FLAG_2_PIN);
    sFlags.val |= ocp2val;


    // Get External ADC data
    uint8_t i;
    uint16_t val = 0;
    uint8_t num_half_channels = 4;
    uint8_t buffer_idx = 0;
    for (i = 0 ; i < num_half_channels; i++) {
        ADS7138IRTER_SingleRegisterWrite(&sADS7138_SWI2C_Descriptor, ADS7138_CHANNEL_SEL_REGISTER, i);
        val = ADS7138IRTER_Read(&sADS7138_SWI2C_Descriptor); 
        sExtADCBuffer03[buffer_idx] = val >> 8;
        sExtADCBuffer03[buffer_idx + 1u] = val & 0xff; 
        buffer_idx += 2;
    }

    buffer_idx = 0;
    for (i = 0 ; i < num_half_channels; i++) {
        ADS7138IRTER_SingleRegisterWrite(&sADS7138_SWI2C_Descriptor, ADS7138_CHANNEL_SEL_REGISTER, i + num_half_channels);
        val = ADS7138IRTER_Read(&sADS7138_SWI2C_Descriptor); 
        sExtADCBuffer47[buffer_idx] = val >> 8;
        sExtADCBuffer47[buffer_idx + 1u] = val & 0xff; 
        buffer_idx += 2;
    }
}

/*ISR that maintains LPM until 30 minutes has passed*/
// -- I have no idea what this means
#if defined(__TI_COMPILER_VERSION__) || defined(__IAR_SYSTEMS_ICC__)
#pragma vector=RTC_VECTOR
__interrupt
#elif defined(__GNUC__)
__attribute__((interrupt(RTC_VECTOR)))
#endif
void RTC_ISR (void)
{
    switch (__even_in_range(RTCIV, 16))
    {
        case RTCIV_NONE:         break;
        case RTCIV_RTCRDYIFG:    RoutineCycle_Process(); break;
        case RTCIV_RTCTEVIFG:    P1OUT |= BIT0; break; // I think this is blinking an LED or some shit
        case RTCIV_RTCAIFG:      /* alarm */ break;
        case RTCIV_RT0PSIFG:     /* prescale 0 */ break;
        case RTCIV_RT1PSIFG:     /* prescale 1 */ break;
        case RTCIV_RTCOFIFG:     /* oscillator fault: clear fault, maybe restart LFXT */ break;
        default:                 break;   // (should not happen)
    }
}
