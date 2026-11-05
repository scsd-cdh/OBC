/*
 * BMS.c
 *
 *  Battery Management System (BMS) application logic for MSP4305969/89 microcontroller.
 *  This file handles initialization and periodic operation of battery monitoring hardware,
 *  including ADC setup for current and voltage sensing, GPIO configuration for protection flags,
 *  PWM control for battery heaters, and I2C communications for telemetry and telecommand.
 *
 *  Author: Brendan Kelly
 */

#include <stdbool.h>

#include "BMS.h"
#include "ADC_Read.h"
#include "include/pmm.h"
#include "msp430.h"
#include "gpio.h"
#include "PWM.h"
#include "swi2c.h"
#include "ads7138irter.h"
#include "i2c.h"
#include "lfp.h"
#include "lfp/stream.h"
#include "asn1/bms.h"
#include "ASN1SCC/asn1_lfp.h"
#include "asn1/_systems.h"
#include "asn1/bms.h"
#include "i2c.h"
#include "lfp_i2c.h"
#include "msp_utils.h"
#include "utils.h"
#if defined(__MSP430FR5969__)
#include "rtc_b.h"
#define BMS_RTC(name) RTC_B_ ## name
#else
#include "rtc_c.h"
#define BMS_RTC(name) RTC_C_ ## name
#endif

typedef struct {
    const lfp_header_t* p_header;
    const uint8_t* p_body;
    uint16_t body_length;
    lfp_code_t reason;
} user_ctx_t;

// ADC pin assignments for current and voltage sensing
// Each *_PIN macro maps a physical pin to an ADC input channel
// Each *_MEM macro maps an ADC channel to a memory buffer
// 1 and 2 does in fact refer to battery number

// Current sensing VUR = Discharge current.
// Pin numbers are FR5989-EP 64-pin RGC; FR6989 numbers are 100-pin PZ / 80-pin PN.
#define I_SENSE_VUR_1_CP_PIN      ADC12_B_INPUT_A10 // P9.2; FR5989-EP pin 42; FR6989 pin 69 / 54
#define I_SENSE_VUR_2_CP_PIN      ADC12_B_INPUT_A0  // FR5989 P1.0 pin 39; FR6989 P1.0 pin 66 / 51

// Current sensing CHR = Charge current.
#define I_SENSE_CHR_1_CP_PIN      ADC12_B_INPUT_A9  // P9.1; FR5989-EP pin 41; FR6989 pin 68 / 53
#define I_SENSE_CHR_2_CP_PIN      ADC12_B_INPUT_A8  // P9.0; FR5989-EP pin 40; FR6989 pin 67 / 52

// Voltage sensing.
#define V_CELL_1A_CP_PIN          ADC12_B_INPUT_A13 // P9.5; FR5989-EP pin 45; FR6989 pin 72 / 57
#define V_CELL_1B_CP_PIN          ADC12_B_INPUT_A12 // P9.4; FR5989-EP pin 44; FR6989 pin 71 / 56
#define V_CELL_2A_CP_PIN          ADC12_B_INPUT_A3  // FR5989 P1.3 pin 36; FR6989 P1.3 pin 63 / 48
#define V_CELL_2B_CP_PIN          ADC12_B_INPUT_A2  // FR5989 P1.2 pin 37; FR6989 P1.2 pin 64 / 49
#define V_BATTPACK_1_CP_PIN       ADC12_B_INPUT_A11 // P9.3; FR5989-EP pin 43; FR6989 pin 70 / 55
#define V_BATTPACK_2_CP_PIN       ADC12_B_INPUT_A1  // FR5989 P1.1 pin 38; FR6989 P1.1 pin 65 / 50

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

// GPIO pin assignments for over-voltage, under-voltage, and over-current protection flags
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
#if defined (__MSP430FR5989__) || defined (__MSP430FR6989__)
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

#define BMS_SLAVE_ADDR 0x09

// number of external ADS7138IRTER channels. Corresponds to thermistors -> heaters
#define ADC_NUM_CHANNELS 8

// RTC read-ready events schedule one-second scans; RT1PS supplies retry slots.
static volatile bool s_isr_triggered = false;
static volatile bool s_power_pending = false;
static volatile bool s_temperature_pending = false;

// Static serialized structs to be populated periodically from sensor data etc...
typedef struct {
    BMSPowerStatusResponse power;
    BMSTemperatureStatusResponse temperature;
} bms_telemetry_t;

// Acquisition owns reading_buffer; CDH serializes only comm_buffer.
static volatile bms_telemetry_t s_reading_buffer;
static volatile bms_telemetry_t s_comm_buffer;

// Static asn1 encoded data buffers to be sent back to CDH on request
static volatile uint8_t s_powerstatus_tx_buf[ASN1_LFP_SEND_BUF_SIZE(BMSPowerStatusResponse)];
static volatile uint8_t s_temperature_status_tx_buf[ASN1_LFP_SEND_BUF_SIZE(BMSTemperatureStatusResponse)];
static uint8_t s_heaterduty_response_tx_buf[ASN1_LFP_SEND_BUF_SIZE(BMSSetHeaterDutyResponse)];


static lfp_stream_ctx_t s_lfp_ctx;

// Global SWI2C config "descriptor"
static SWI2C_Descriptor s_swi2c_descriptor;

// LFP
#define RX_BODY_BUFFER_SIZE \
    MAX(ASN1_LFP_RECV_BUF_SIZE(BMSSetHeaterDutyRequest), \
    MAX(ASN1_LFP_RECV_BUF_SIZE(BMSPowerStatusRequest), \
        ASN1_LFP_RECV_BUF_SIZE(BMSTemperatureStatusRequest)))

static uint8_t s_rx_body_buffer[RX_BODY_BUFFER_SIZE];
static user_ctx_t s_user_ctx;

static bool on_header(const lfp_header_t* p_header, void* p_ctx) {
    lfp_i2c_transition(I2C_SLAVE_STATE_REQUEST);
    return true;
}

// TODO: implement me?
static void on_asn1_error(int error_code, const struct asn1_lfp_decode_data_t * p_data) {
    (void)error_code;
    (void)p_data;
}

static void on_lfp_error(lfp_code_t error_code, void *p_ctx) {
    (void)error_code;
    (void)p_ctx;
}

static inline void bms_power_status_req_cb(const BMSPowerStatusRequest * p_payload, const asn1_lfp_decode_data_t * p_data) {
    (void)p_payload;
    lfp_i2c_transition(I2C_SLAVE_STATE_PROCESSING);
    s_comm_buffer.power = s_reading_buffer.power;
    uint16_t size = ASN1_LFP_SERIALIZE(
        p_data->p_header->initiator, bmsSystemId,
        BMSPowerStatusResponse,
        (unsigned char*)s_powerstatus_tx_buf, sizeof(s_powerstatus_tx_buf),
        s_comm_buffer.power
    );
    lfp_i2c_set_txbuf(s_powerstatus_tx_buf, size);
}

static inline void bms_temperature_status_req_cb(const BMSTemperatureStatusRequest * p_payload, const asn1_lfp_decode_data_t * p_data) {
    (void)p_payload;
    lfp_i2c_transition(I2C_SLAVE_STATE_PROCESSING);
    s_comm_buffer.temperature = s_reading_buffer.temperature;
    uint16_t size = ASN1_LFP_SERIALIZE(
        p_data->p_header->initiator, bmsSystemId,
        BMSTemperatureStatusResponse,
        (unsigned char*)s_temperature_status_tx_buf,
        sizeof(s_temperature_status_tx_buf),
        s_comm_buffer.temperature
    );
    lfp_i2c_set_txbuf(s_temperature_status_tx_buf, size);
}

static inline void bms_set_heater_duty_cb(const BMSSetHeaterDutyRequest * p_payload, const asn1_lfp_decode_data_t * p_data) {
    // It's 100-1 and not 99 because that's what they do in the TI examples for PWM, IDK why the do that but best to stick to a standard if it exists
    lfp_i2c_transition(I2C_SLAVE_STATE_PROCESSING);
    if (p_payload->exist.heater1) {
        PWM_Generate(100-1, p_payload->heater1, HEATER1_CCR);
    }
    if (p_payload->exist.heater2) {
        PWM_Generate(100-1, p_payload->heater2, HEATER2_CCR);
    }
    if (p_payload->exist.heater3) {
        PWM_Generate(100-1, p_payload->heater3, HEATER3_CCR);
    }
    if (p_payload->exist.heater4) {
        PWM_Generate(100-1, p_payload->heater4, HEATER4_CCR);
    }

    // Success acknowledges applying the requested PWM duties, not heater health.
    const BMSSetHeaterDutyResponse response = { .responseCode = 0 };
    uint16_t size = ASN1_LFP_SERIALIZE(
        p_data->p_header->initiator, bmsSystemId,
        BMSSetHeaterDutyResponse,
        s_heaterduty_response_tx_buf, sizeof(s_heaterduty_response_tx_buf),
        response
    );
    lfp_i2c_set_txbuf(s_heaterduty_response_tx_buf, size);
}

// bit silly to try to inline this. asm output at i2c.asm confirms a call with max optimizations (line 657): CALLA &s_ctx+0
static void on_msg(const lfp_header_t * p_header, const uint8_t * p_body, uint16_t body_length, void * p_ctx) {
    // if we get a valid message while responding to something else, forget what we were doing before respond to the new request
    if (lfp_i2c_state() == I2C_SLAVE_STATE_RESPONSE) {
        lfp_i2c_transition(I2C_SLAVE_STATE_REQUEST);
    }
    const asn1_lfp_decode_data_t data = {
        .p_header = p_header,
        .p_body = p_body,
        .p_ctx = p_ctx,
        .body_length = body_length,
        .p_on_error_cb = on_asn1_error,
    };

    if (ASN1_LFP_HANDLE_MSG(data, bmsSystemId, BMSPowerStatusRequest, bms_power_status_req_cb)) {
        return;
    }

    if (ASN1_LFP_HANDLE_MSG(data, bmsSystemId, BMSTemperatureStatusRequest, bms_temperature_status_req_cb)) {
        return;
    }

    if (ASN1_LFP_HANDLE_MSG(data, bmsSystemId, BMSSetHeaterDutyRequest, bms_set_heater_duty_cb)) {
        return;
    }
}

/* INITIALIZATOIN LOGIC */
// Init ADCs
// Sets up all ADC channels and memory buffers for multi-channel sweeps
// Uses ADC12_B driverlib
// NOTE: You need to set the end of sequence to the last ADC pin and set sequence. I repeat this a lot because it's easy to miss!
static void adc_init() {
    ADC_initMultiple();

    ADC12_B_disableConversions(ADC12_B_BASE, 1);
    // Selects what pin to get mapped to what ADC memory thing
    // Current sensors
    ADC_PinSelect(I_SENSE_VUR_1_CP_PIN,     I_SENSE_VUR_1_CP_MEM); 
    ADC_PinSelect(I_SENSE_VUR_2_CP_PIN,     I_SENSE_VUR_2_CP_MEM);
    ADC_PinSelect(I_SENSE_CHR_1_CP_PIN,     I_SENSE_CHR_1_CP_MEM);
    ADC_PinSelect(I_SENSE_CHR_2_CP_PIN,     I_SENSE_CHR_2_CP_MEM);
    // Voltage sensors
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
    // This allows us to use more than one MEM, necessary for multiple sensors
    ADC12CTL1 |= ADC12CONSEQ_1;   // driverlib name: ADC12_B_SEQUENCEOFCHANNELS
}

// Initialize GPIO pins for flags, I2C, and PWM outputs
// Uses GPIO driverlib
static void gpio_init() {

    // I2C pins
    P1SEL0 |= BIT6 | BIT7;
    P1SEL1 &= ~(BIT6 | BIT7);

    // Redundant I2C pins
#if defined (__MSP430FR5989__) || (__MSP430FR6989__)
    P3SEL0 |= BIT1 | BIT2;
    P3SEL1 &= ~(BIT1 | BIT2);
#endif

    // Set flag pins
    // MSP430FR5989 Pins 10 through 13 use GPIO_PORT_P5
    GPIO_setAsInputPin(GPIO_PORT_P5, OVP_FLAG_1A_PIN); // MSP430FR5989 10 P5.0
    GPIO_setAsInputPin(GPIO_PORT_P5, UVP_FLAG_1B_PIN); // MSP430FR5989 11
    GPIO_setAsInputPin(GPIO_PORT_P5, UVP_FLAG_1A_PIN); // MSP430FR5989 12 P5.2
    GPIO_setAsInputPin(GPIO_PORT_P5, OVP_FLAG_1B_PIN); // MSP430FR5989 13 P5.3
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

#if defined(__MSP430FR5989__) || defined(__MSP430FR6989__)
    // Internal voltage/current ADC inputs: A0-A3 on P1.0-P1.3 and
    // A8-A13 on P9.0-P9.5. ADC_PinSelect() only configures the ADC mux.
    const uint8_t p1_analog_pins = BIT0 | BIT1 | BIT2 | BIT3;
    const uint8_t p9_analog_pins = BIT0 | BIT1 | BIT2 | BIT3 | BIT4 | BIT5;
    P1DIR &= ~p1_analog_pins;
    P1REN &= ~p1_analog_pins;
    P1SEL0 |= p1_analog_pins;
    P1SEL1 |= p1_analog_pins;
    P9DIR &= ~p9_analog_pins;
    P9REN &= ~p9_analog_pins;
    P9SEL0 |= p9_analog_pins;
    P9SEL1 |= p9_analog_pins;
#endif

    // Y2 (32.768 kHz): PJ.4/LFXIN and PJ.5/LFXOUT on FR5989/FR6989.
    // FR5989-EP RGC physical pins 57/58; FR6989 PZ pins 84/85.
    PJSEL1 &= ~(BIT4 | BIT5);
    PJSEL0 |= BIT4 | BIT5;
    PJREN &= ~(BIT4 | BIT5);

    // Activate port settings after analog, digital and PWM pins are configured.
    PM5CTL0 &= ~LOCKLPM5;
}


// Initialize GPIO, the 16 MHz clock, and the internal ADC.
static void hardware_init() {
    WDTCTL = WDTPW | WDTHOLD;                                // Stop WDT
    gpio_init();
    clock_init_16mhz();
    adc_init();
}

// App communication initialization. Initializes I2C and lfp, registers telemetry and telecommand channels
static void comm_init() {
    // Initialize main and redundant hardware i2c
    lfp_i2c_init(&s_lfp_ctx, I2C_MODULE_UCB0|I2C_MODULE_UCB1, BMS_SLAVE_ADDR);

    // SW I2C
    s_swi2c_descriptor.sda_port_out =   &P4OUT;
    s_swi2c_descriptor.sda_port_in =    &P4IN;
    s_swi2c_descriptor.sda_port_dir =   &P4DIR;
    s_swi2c_descriptor.scl_port_out =   &P4OUT;
    s_swi2c_descriptor.scl_port_in =    &P4IN;
    s_swi2c_descriptor.scl_port_dir =   &P4DIR;
#if defined (__MSP430FR5989__) || defined (__MSP430FR6989__)
    // As-built local ADC bus: SDA=P4.1, SCL=P4.0 (schematic labels are reversed).
    s_swi2c_descriptor.sda_pin =        GPIO_PIN1;
    s_swi2c_descriptor.scl_pin =        GPIO_PIN0;
#elif defined (__MSP430FR5969__)
    s_swi2c_descriptor.sda_pin =        GPIO_PIN2;
    s_swi2c_descriptor.scl_pin =        GPIO_PIN3;
#endif
    // Select general purpose IO for port 4
    P4SEL0 &= ~(BIT0 | BIT1);
    P4SEL1 &= ~(BIT0 | BIT1);

    ADS7138IRTER_Initialize(&s_swi2c_descriptor);

    lfp_stream_init(&s_lfp_ctx, s_rx_body_buffer, 
        sizeof(s_rx_body_buffer), on_header, on_msg, 
        on_lfp_error, &s_user_ctx);
}

// Each collection builds a complete local result and publishes only on success.
static bool collect_power_status(void) {
    BMSPowerStatusResponse power;
#ifndef MOCK_POWER
    // Collect voltage and current ADC values
    // Must do this this way (all at once) if using SEQOFCHANNELS mode
    // Do not reconfigure a previous conversion if it is still stuck busy.
    if (ADC_waitForConversion() != ADC_STATUS_OK) {
        return false;
    }
    // trigger the ten‑channel sweep
    ADC12_B_startConversion(ADC12_B_BASE,
        ADC12_B_START_AT_ADC12MEM0,
        ADC12_B_SEQOFCHANNELS);        // ENC+SC, walk MEM0->MEM9

    if (ADC_waitForConversion() != ADC_STATUS_OK) {
        return false;
    }

    // Current
    power.batteryPack1.currentDraw        = ADC12_B_getResults(ADC12_B_BASE, I_SENSE_VUR_1_CP_MEM);
    power.batteryPack2.currentDraw        = ADC12_B_getResults(ADC12_B_BASE, I_SENSE_VUR_2_CP_MEM);
    power.batteryPack1.currentCharge      = ADC12_B_getResults(ADC12_B_BASE, I_SENSE_CHR_1_CP_MEM);
    power.batteryPack2.currentCharge      = ADC12_B_getResults(ADC12_B_BASE, I_SENSE_CHR_2_CP_MEM);

    // Voltage per cell
    power.batteryPack1.cellA.voltage      = ADC12_B_getResults(ADC12_B_BASE, V_CELL_1A_CP_MEM);
    power.batteryPack1.cellB.voltage      = ADC12_B_getResults(ADC12_B_BASE, V_CELL_1B_CP_MEM);
    power.batteryPack2.cellA.voltage      = ADC12_B_getResults(ADC12_B_BASE, V_CELL_2A_CP_MEM);
    power.batteryPack2.cellB.voltage      = ADC12_B_getResults(ADC12_B_BASE, V_CELL_2B_CP_MEM);

    // Combined voltage per battery
    power.batteryPack1.voltage            = ADC12_B_getResults(ADC12_B_BASE, V_BATTPACK_1_CP_MEM);
    power.batteryPack2.voltage            = ADC12_B_getResults(ADC12_B_BASE, V_BATTPACK_2_CP_MEM);

    // Collect voltage and current flags
    power.batteryPack1.cellA.overvoltage  = GPIO_getInputPinValue(GPIO_PORT_P5, OVP_FLAG_1A_PIN);
    power.batteryPack1.cellA.undervoltage = GPIO_getInputPinValue(GPIO_PORT_P5, UVP_FLAG_1A_PIN);
    power.batteryPack1.cellB.overvoltage  = GPIO_getInputPinValue(GPIO_PORT_P5, OVP_FLAG_1B_PIN);
    power.batteryPack1.cellB.undervoltage = GPIO_getInputPinValue(GPIO_PORT_P5, UVP_FLAG_1B_PIN);
    power.batteryPack1.overcurrent        = GPIO_getInputPinValue(GPIO_PORT_P3, OCP_FLAG_1_PIN);

    power.batteryPack2.cellA.overvoltage  = GPIO_getInputPinValue(GPIO_PORT_P3, OVP_FLAG_2A_PIN);
    power.batteryPack2.cellA.undervoltage = GPIO_getInputPinValue(GPIO_PORT_P3, UVP_FLAG_2A_PIN);
    power.batteryPack2.cellB.overvoltage  = GPIO_getInputPinValue(GPIO_PORT_P3, OVP_FLAG_2B_PIN);
    power.batteryPack2.cellB.undervoltage = GPIO_getInputPinValue(GPIO_PORT_P3, UVP_FLAG_2B_PIN);
    power.batteryPack2.overcurrent        = GPIO_getInputPinValue(GPIO_PORT_P3, OCP_FLAG_2_PIN);
#else
    power.batteryPack1.currentDraw        = 0x11ee;
    power.batteryPack1.currentCharge      = 0x12ee;

    power.batteryPack1.cellA.voltage      = 0x1Aee;
    power.batteryPack1.cellB.voltage      = 0x1Bee;

    power.batteryPack1.voltage            = 0x1111;

    power.batteryPack1.cellA.overvoltage  = 0;
    power.batteryPack1.cellA.undervoltage = 1;
    power.batteryPack1.cellB.overvoltage  = 0;
    power.batteryPack1.cellB.undervoltage = 0;
    power.batteryPack1.overcurrent        = 1;

    power.batteryPack2.currentDraw        = 0x21ee;
    power.batteryPack2.currentCharge      = 0x22ee;

    power.batteryPack2.cellA.voltage      = 0x2Aee;
    power.batteryPack2.cellB.voltage      = 0x2Bee;
    power.batteryPack2.voltage            = 0x2222;

    power.batteryPack2.cellA.overvoltage  = 1;
    power.batteryPack2.cellA.undervoltage = 0;
    power.batteryPack2.cellB.overvoltage  = 1;
    power.batteryPack2.cellB.undervoltage = 0;
    power.batteryPack2.overcurrent        = 0;
#endif
    uint16_t gie = __get_SR_register() & GIE;
    __disable_interrupt();
    s_reading_buffer.power = power;
    if (gie) __enable_interrupt();
    return true;
}

static bool collect_temperature_status(void) {
    BMSTemperatureStatusResponse temperature;
    for (uint8_t i = 0; i < ADC_NUM_CHANNELS; i++) {
        if (ADS7138IRTER_SingleRegisterWrite(&s_swi2c_descriptor,
                ADS7138_CHANNEL_SEL_REGISTER, i) != ADS7138_STATUS_OK ||
            ADS7138IRTER_Read(&s_swi2c_descriptor,
                &temperature.thermistors.arr[i]) != ADS7138_STATUS_OK) {
            return false;
        }
    }

    uint16_t gie = __get_SR_register() & GIE;
    __disable_interrupt();
    s_reading_buffer.temperature = temperature;
    if (gie) __enable_interrupt();
    return true;
}

// Calendar mode uses the 32.768 kHz crystal directly. RT1PS sees 128 Hz;
// its /16 interrupt provides 125 ms retry slots without consuming Timer_A0.
// Apply after the shared rtc_init(), before main enables global interrupts.
static bool bms_rtc_config(void) {
    const uint8_t interrupts = BMS_RTC(CLOCK_READ_READY_INTERRUPT) |
        BMS_RTC(TIME_EVENT_INTERRUPT) | BMS_RTC(CLOCK_ALARM_INTERRUPT) |
        BMS_RTC(PRESCALE_TIMER0_INTERRUPT) | BMS_RTC(PRESCALE_TIMER1_INTERRUPT) |
        BMS_RTC(OSCILLATOR_FAULT_INTERRUPT);
    BMS_RTC(holdClock)(BMS_RTC(BASE));
    BMS_RTC(disableInterrupt)(BMS_RTC(BASE), interrupts);

    CSCTL0_H = CSKEY_H;
    CSCTL4 &= ~(LFXTOFF | LFXTBYPASS);
    CSCTL4 |= LFXTDRIVE_3;
    bool ready = false;
    // At 16 MHz, each delay is 100 us: finite startup budget of about 1 s.
    for (uint16_t attempt = 0; attempt < 10000u; attempt++) {
        CSCTL5 &= ~LFXTOFFG;
        SFRIFG1 &= ~OFIFG;
        __delay_cycles(1600);
        if (!(CSCTL5 & LFXTOFFG)) {
            ready = true;
            break;
        }
    }
    CSCTL0_H = 0;
    if (!ready) return false;

#if !defined(__MSP430FR5969__)
    RTCCTL0_H = RTCKEY_H;
    RTCCTL13 |= RTCMODE;
    RTCCTL0_H = 0;
#endif
    // Valid calendar seed; acquisition needs intervals, not wall-clock time.
    Calendar calendar = { .Seconds = 0, .Minutes = 0, .Hours = 0,
        .DayOfWeek = 6, .DayOfMonth = 1, .Month = 1, .Year = 2000 };
    BMS_RTC(initCalendar)(BMS_RTC(BASE), &calendar, BMS_RTC(FORMAT_BINARY));
    BMS_RTC(setPrescaleValue)(BMS_RTC(BASE), BMS_RTC(PRESCALE_0), 0);
    BMS_RTC(setPrescaleValue)(BMS_RTC(BASE), BMS_RTC(PRESCALE_1), 0);
    BMS_RTC(definePrescaleEvent)(BMS_RTC(BASE), BMS_RTC(PRESCALE_1),
                               BMS_RTC(PSEVENTDIVIDER_16));
    BMS_RTC(clearInterrupt)(BMS_RTC(BASE), interrupts);
    BMS_RTC(enableInterrupt)(BMS_RTC(BASE), BMS_RTC(CLOCK_READ_READY_INTERRUPT));
    BMS_RTC(startClock)(BMS_RTC(BASE));
    return true;
}

// Called with interrupts disabled after publishing a pass. Keep the retry
// interrupt off while healthy so idle wakeups occur only once per second.
static void retry_interrupt_update(void) {
    if (s_power_pending || s_temperature_pending) {
        BMS_RTC(clearInterrupt)(BMS_RTC(BASE), BMS_RTC(PRESCALE_TIMER1_INTERRUPT));
        BMS_RTC(enableInterrupt)(BMS_RTC(BASE), BMS_RTC(PRESCALE_TIMER1_INTERRUPT));
    } else {
        BMS_RTC(disableInterrupt)(BMS_RTC(BASE), BMS_RTC(PRESCALE_TIMER1_INTERRUPT));
    }
}

void bms_init(void) {
    hardware_init();
    comm_init();
    rtc_init();
    (void)bms_rtc_config();
}

// Claim scheduled work atomically, then attempt each pending group once.
// Pending flags are cleared before reading so a new RTC event during a scan
// remains pending. A failed group is retried in full on the next 125 ms slot.
void bms_collectdata(void) {
    uint16_t gie = __get_SR_register() & GIE;

    __disable_interrupt();
    if (!s_isr_triggered) {
        if (gie) {
            __enable_interrupt();
        }
        return;
    }

    bool power_due = s_power_pending;
    bool temperature_due = s_temperature_pending;
    s_isr_triggered = false;
    s_power_pending = false;
    s_temperature_pending = false;
    if (gie) {
        __enable_interrupt();
    }

    if (power_due && !collect_power_status()) {
        s_power_pending = true;
    }

    if (temperature_due && !collect_temperature_status()) {
        s_temperature_pending = true;
    }

    __disable_interrupt();
    retry_interrupt_update();
    if (gie) {
        __enable_interrupt();
    }
}

bool bms_isr_triggered(void) {
    return s_isr_triggered;
}

#if defined(__TI_COMPILER_VERSION__) || defined(__IAR_SYSTEMS_ICC__)
#pragma vector=RTC_VECTOR
__interrupt
#elif defined(__GNUC__)
__attribute__((interrupt(RTC_VECTOR)))
#endif
void RTC_ISR (void)
{
    switch (__even_in_range(RTCIV, 16)) {
        case RTCIV_NONE:         break;
        case RTCIV_RTCRDYIFG:
            s_power_pending = true;
            s_temperature_pending = true;
            s_isr_triggered = true;
            break;
        case RTCIV_RT1PSIFG:
            if (s_power_pending || s_temperature_pending) {
                s_isr_triggered = true;
            }
            break;
        default:
            break;
    }

    if (bms_isr_triggered()) {
        __bic_SR_register_on_exit(LPM0_bits);
    }
}
