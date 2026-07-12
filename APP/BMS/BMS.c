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

#if defined (__MSP430FR5989__)
#include "rtc_c.h"
#elif defined (__MSP430FR5969__)
#include "rtc_b.h"
#endif

// ADC pin assignments for current and voltage sensing
// Each *_PIN macro maps a physical pin to an ADC input channel
// Each *_MEM macro maps an ADC channel to a memory buffer
// 1 and 2 does in fact refer to battery number

// Current sensing VUR = Discharge current
#define I_SENSE_VUR_1_CP_PIN      ADC12_B_INPUT_A10 // PIN 42 5989
#define I_SENSE_VUR_2_CP_PIN      ADC12_B_INPUT_A0  // PIN 39 5989

// Current sensing CHR = Charge current
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

#define BMS_SLAVE_ADDR 0x09

// number of external ADS7138IRTER channels. Corresponds to thermistors -> heaters
#define ADC_NUM_CHANNELS 8

// private isr triggered flag for access only
static volatile bool s_isr_triggered = false;

// Static serialized structs to be populated periodically from sensor data etc...
static const BMSSystemStatusResponse s_systemstatus = {
    .uptime = 0xDEADBEEF,
    .version = 12    
};
static volatile BMSPowerStatusResponse s_powerstatus;
static volatile BMSTemperatureStatusResponse s_temperature_status;
static BMSSetHeaterDutyResponse s_heaterduty_response; // this 

// Static asn1 encoded data buffers to be sent back to CDH on request
static volatile uint8_t s_systemstatus_tx_buf[ASN1_LFP_SEND_BUF_SIZE(BMSSystemStatusResponse)];
static volatile uint16_t s_powerstatus_size = 0;
static volatile uint8_t s_powerstatus_tx_buf[ASN1_LFP_SEND_BUF_SIZE(BMSPowerStatusResponse)];
static volatile uint16_t s_temperature_status_size = 0;
static volatile uint8_t s_temperature_status_tx_buf[ASN1_LFP_SEND_BUF_SIZE(BMSTemperatureStatusResponse)];
static uint16_t s_heaterduty_response_size = 0;
static uint8_t s_heaterduty_response_tx_buf[ASN1_LFP_SEND_BUF_SIZE(BMSSetHeaterDutyResponse)];

// Global SWI2C config "descriptor"
static SWI2C_Descriptor s_swi2c_descriptor;

// LFP 
typedef struct {
    const lfp_header_t* p_header;
    const uint8_t* p_body;
    uint16_t body_length;
    lfp_code_t reason;
} user_ctx_t;

static lfp_stream_ctx_t s_lfp_ctx;

// FIXME: arbitrary temporary size
#define RX_BODY_BUFFER_SIZE 16

static uint8_t s_rx_body_buffer[RX_BODY_BUFFER_SIZE];
static user_ctx_t s_user_ctx;


typedef enum {
    I2C_SLAVE_STATE_REQUEST,
    I2C_SLAVE_STATE_PROCESSING,
    I2C_SLAVE_STATE_RESPONSE
} i2c_slave_state_t;

static volatile i2c_slave_state_t s_current_state = I2C_SLAVE_STATE_REQUEST;
static volatile uint16_t s_tx_idx = 0;
static volatile uint8_t* p_txbuf = NULL;
static volatile uint16_t s_msg_len = 0;

static void i2c_transition(i2c_slave_state_t state) {
    // naive for now
    if (state == I2C_SLAVE_STATE_REQUEST) {
        // discard tx buffer and reset 
        p_txbuf = NULL;
        s_tx_idx = 0;
        s_msg_len = 0;
    }

    // NACK if we are in processing stage
    if (state == I2C_SLAVE_STATE_PROCESSING) {
        i2c_send_nack();
    } else {
        i2c_clear_nack();
    }
    
    s_current_state = state;
}

static bool on_header(const lfp_header_t* p_header, void* p_ctx) {
    if (s_current_state == I2C_SLAVE_STATE_RESPONSE) {
        i2c_transition(I2C_SLAVE_STATE_REQUEST);
    }
    return true;
}

int16_t i2c_reassign_txbuf(volatile uint8_t* data, uint16_t size)
{
    s_tx_idx = 0;
    s_msg_len = size;
    p_txbuf = data; 
    // We're ready to start transmitting data
    i2c_transition(I2C_SLAVE_STATE_RESPONSE);
}

// FIXME: We should only ever transition REQUEST -> PROCESSING -> RESPONSE -> REQUEST ... 
static void i2c_start_cond_cb() {
    if (s_current_state == I2C_SLAVE_STATE_PROCESSING) { 
        i2c_send_nack(); // NACK master writes while we're processing
    }

    // Roll back 1 byte on start to transmit the byte that was missed during the last transmission
    if (s_tx_idx > 0 && s_tx_idx <= s_msg_len) { 
        s_tx_idx--;
    }
}

static void i2c_tx_byte_cb(volatile uint8_t* byte) {
    if (s_current_state != I2C_SLAVE_STATE_RESPONSE) { // Unless we are in response state, just return 0xFF
        *byte = 0xFF;
    } else if (!p_txbuf || s_tx_idx >= s_msg_len) { // if we are asked for more bytes than we have, send 0xFF
        // If we are at the byte after the last byte, go one over to make sure we dont roll back on start 
        // (by now the controller has received atleast one 0xFF)
        if (s_tx_idx == s_msg_len) {
            s_tx_idx++;
        }
        *byte = 0xFF;
    } else {
        *byte = p_txbuf[s_tx_idx++]; 
    }
}

static void i2c_rx_cb(uint8_t data) {
    lfp_stream_update(&s_lfp_ctx, data);
}

// TODO: implement me?
static void on_error(int error_code, const struct asn1_lfp_decode_data_t * p_data) {
}

// NOTE!! If we don't copy buffers into a seperate i2c transfer buffer, idk if it's possible for buffers to be updated mid transfer or not
// I don't think so, but wouldn't hurt testing this just in casee
// NOTE2: these callbacks are technically doing work in the isr, but on_msg only ever gets called on the last byte of the write
// TODO: Make put them in helper function
static inline void bms_system_status_req_cb(const BMSSystemStatusRequest * p_payload, const asn1_lfp_decode_data_t * p_data) {
    (void)p_payload;
    i2c_transition(I2C_SLAVE_STATE_PROCESSING);
    // GIE -- this is technically in the i2c isr, we don't want getting stuck here to stall BMS
    __enable_interrupt(); 

    uint16_t size = ASN1_LFP_SERIALIZE(
        p_data->p_header->initiator, bmsSystemId, 
        BMSSystemStatusResponse, 
        s_systemstatus_tx_buf, sizeof(s_systemstatus_tx_buf), 
        s_systemstatus
    );
    __disable_interrupt();
    i2c_reassign_txbuf(s_systemstatus_tx_buf, size);
}

static inline void bms_power_status_req_cb(const BMSPowerStatusRequest * p_payload, const asn1_lfp_decode_data_t * p_data) {
    (void)p_payload;
    i2c_transition(I2C_SLAVE_STATE_PROCESSING);
    __enable_interrupt();
    uint16_t size = ASN1_LFP_SERIALIZE(
        p_data->p_header->initiator, bmsSystemId, 
        BMSPowerStatusResponse, 
        s_powerstatus_tx_buf, sizeof(s_powerstatus_tx_buf), 
        s_powerstatus
    );
    __disable_interrupt();
    i2c_reassign_txbuf(s_powerstatus_tx_buf, size);    
}

static inline void bms_temperature_status_req_cb(const BMSTemperatureStatusRequest * p_payload, const asn1_lfp_decode_data_t * p_data) {
    (void)p_payload;
    i2c_transition(I2C_SLAVE_STATE_PROCESSING);
    __enable_interrupt();
    uint16_t size = ASN1_LFP_SERIALIZE(
        p_data->p_header->initiator, bmsSystemId, 
        BMSTemperatureStatusResponse, 
        s_temperature_status_tx_buf, 
        sizeof(s_temperature_status_tx_buf), 
        s_temperature_status
    );
    __disable_interrupt();
    i2c_reassign_txbuf(s_temperature_status_tx_buf, size);   
}

static inline void bms_set_heater_duty_cb(const BMSSetHeaterDutyRequest * p_payload, const asn1_lfp_decode_data_t * p_data) {
    // It's 100-1 and not 99 because that's what they do in the TI examples for PWM, IDK why the do that but best to stick to a standard if it exists
    i2c_transition(I2C_SLAVE_STATE_PROCESSING);
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

    // Send response (Empty body basically an ACK)
    i2c_reassign_txbuf(s_temperature_status_tx_buf, s_temperature_status_size);   
}

// bit silly to try to inline this. asm output at i2c.asm confirms a call with max optimizations (line 657): CALLA &s_ctx+0
static void on_msg(const lfp_header_t * p_header, const uint8_t * p_body, uint16_t body_length, void * p_ctx) {
    // if we get a valid message while responding to something else, forget what we were doing before respond to the new request
    if (s_current_state == I2C_SLAVE_STATE_RESPONSE) {
        i2c_transition(I2C_SLAVE_STATE_REQUEST);
    }
    const asn1_lfp_decode_data_t data = {
        .p_header = p_header,
        .p_body = p_body,
        .p_ctx = p_ctx,
        .body_length = body_length,
        .p_on_error_cb = on_error,
    };

    if (ASN1_LFP_HANDLE_MSG(data, bmsSystemId, BMSSystemStatusRequest, bms_system_status_req_cb)) {
        return;
    }

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
// Note most of the code is initialization, since the main logic is just reading sensors and updating buffers
// Init ADCs
// Sets up all ADC channels and memory buffers for multi-channel sweeps
// Uses ADC12_B driverlib
// NOTE: You need to set the end of sequence to the last ADC pin and set sequence. I repeat this a lot because it's easy to miss!
static void adc_init() {
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
    // This allows us to use more than one MEM, necessary for multiple sensors
    ADC12CTL1 |= ADC12CONSEQ_1;   // driverlib name: ADC12_B_SEQUENCEOFCHANNELS
}

// Initialize clock to 16MHz
// Uses direct register manipulation as per device datasheet
static void clock_init_16mhz() {
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

// Initialize GPIO pins for flags, I2C, and PWM outputs
// Uses GPIO driverlib
static void gpio_init() {
    // Importing drivers is a pain and we only really need WDT_A_hold(), so just copy and paste it here for now.
    // IMO it would be better to just have driverlib
    uint8_t new_wdt_status = ((HWREG16(WDT_A_BASE + OFS_WDTCTL) & 0x00FF) | WDTHOLD);
    HWREG16(WDT_A_BASE + OFS_WDTCTL) = WDTPW + new_wdt_status;

    // Configure Pins for I2C
    //Set P1.6 and P1.7 as Secondary Module Function Input.
    /*
    * Select Port 1
    * Set Pin 6, 7 to input Secondary Module Function, (UCB0SIMO/UCB0SDA, UCB0SOMI/UCB0SCL).
    */
    GPIO_setAsPeripheralModuleFunctionInputPin(
        GPIO_PORT_P1,
        GPIO_PIN6 + GPIO_PIN7,
        GPIO_SECONDARY_MODULE_FUNCTION
    );

    /*
     * Disable the GPIO power-on default high-impedance mode to activate
     * previously configured port settings
     */
    PMM_unlockLPM5();

    // Set flag pins
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

// Initialize the Real-Time Clock (RTC) for periodic interrupts
// Uses RTC_C or RTC_B driverlib depending on device
static void rtc_init() {
#if defined (__MSP430FR5989__)
    RTC_C_clearInterrupt(RTC_C_BASE,
        RTC_C_CLOCK_READ_READY_INTERRUPT +
        RTC_C_TIME_EVENT_INTERRUPT +
        RTC_C_CLOCK_ALARM_INTERRUPT
        );
    RTC_C_enableInterrupt(RTC_C_BASE,
        RTC_C_CLOCK_READ_READY_INTERRUPT +
        RTC_C_TIME_EVENT_INTERRUPT +
        RTC_C_CLOCK_ALARM_INTERRUPT
    );

    //Start RTC
    RTC_C_startClock(RTC_C_BASE);
#elif defined (__MSP430FR5969__)
    RTC_B_clearInterrupt(RTC_B_BASE,
        RTC_B_CLOCK_READ_READY_INTERRUPT +
        RTC_B_TIME_EVENT_INTERRUPT +
        RTC_B_CLOCK_ALARM_INTERRUPT
        );
    RTC_B_enableInterrupt(RTC_B_BASE,
        RTC_B_CLOCK_READ_READY_INTERRUPT +
        RTC_B_TIME_EVENT_INTERRUPT +
        RTC_B_CLOCK_ALARM_INTERRUPT
    );

    //Start RTC Clock
    RTC_B_startClock(RTC_B_BASE);
#endif
}

// Hardware initialization. Initializes MSP430 specific device modules for I2C, ADC, GPIO, Clock, and RTC
static void hardware_init() {
    gpio_init();
    clock_init_16mhz();
    rtc_init();
    adc_init();
}

// App communication initialization. Initializes I2C and lfp, registers telemetry and telecommand channels
static void comm_init() {
    i2c_ctx_t i2c_ctx = {
        .i2c_rx_cb = i2c_rx_cb,
        .i2c_tx_byte_cb = i2c_tx_byte_cb,
        .i2c_start_cond_cb = i2c_start_cond_cb,
        .slave_addr = BMS_SLAVE_ADDR,
    };
    i2c_init(&i2c_ctx);  

    // SW I2C
    s_swi2c_descriptor.sda_port_out =   &P4OUT;
    s_swi2c_descriptor.sda_port_in =    &P4IN;
    s_swi2c_descriptor.sda_port_dir =   &P4DIR;
    s_swi2c_descriptor.scl_port_out =   &P4OUT;
    s_swi2c_descriptor.scl_port_in =    &P4IN;
    s_swi2c_descriptor.scl_port_dir =   &P4DIR;
#if defined (__MSP430FR5989__)
    s_swi2c_descriptor.sda_pin =        GPIO_PIN1;
    s_swi2c_descriptor.scl_pin =        GPIO_PIN0;
#elif defined (__MSP430FR5969__)
    s_swi2c_descriptor.sda_pin =        GPIO_PIN2;
    s_swi2c_descriptor.scl_pin =        GPIO_PIN3;
#endif
    P1DIR |= BIT0 | BIT1; // Set 1.0 and 1.1 direction to "output". I have no idea why this is here
    // Select general purpose IO for port 4
    P4SEL0 &= ~(BIT0 | BIT1);
    P4SEL1 &= ~(BIT0 | BIT1);
    
    ADS7138IRTER_Initialize(&s_swi2c_descriptor);

    // We only every have to do this once since it's just an empty body used as an ACK. 
    s_heaterduty_response_size = ASN1_LFP_SERIALIZE(bmsSystemId, cdhSystemId, BMSSetHeaterDutyResponse, s_heaterduty_response_tx_buf, sizeof(s_heaterduty_response_tx_buf), s_heaterduty_response);
    if (!s_heaterduty_response_size) {
        __no_operation();
    }

    lfp_stream_init(&s_lfp_ctx, s_rx_body_buffer, sizeof(s_rx_body_buffer), on_header, on_msg, on_error, &s_user_ctx);
}

#ifndef MOCK_POWER
static void collect_power_status() {
    // Collect voltage and current ADC values 
    // Must do this this way (all at once) if using SEQOFCHANNELS mode
    // trigger the ten‑channel sweep 
    ADC12_B_startConversion(ADC12_B_BASE,
        ADC12_B_START_AT_ADC12MEM0,
        ADC12_B_SEQOFCHANNELS);        // ENC+SC, walk MEM0->MEM9

    // Profile this irl... can probably get away with having this in callback so we can get most accurate data immediately on asking
    // I'm not sure how possible it is for this to spin forever if at all. 
    uint16_t counter = 0;
    while (ADC12_B_isBusy(ADC12_B_BASE)) {
        counter++;
    }
    
    // Current
    s_powerstatus.batteryPack1.currentDraw        = ADC12_B_getResults(ADC12_B_BASE, I_SENSE_VUR_1_CP_MEM);
    s_powerstatus.batteryPack2.currentDraw        = ADC12_B_getResults(ADC12_B_BASE, I_SENSE_VUR_2_CP_MEM);
    s_powerstatus.batteryPack1.currentCharge      = ADC12_B_getResults(ADC12_B_BASE, I_SENSE_CHR_1_CP_MEM);
    s_powerstatus.batteryPack2.currentCharge      = ADC12_B_getResults(ADC12_B_BASE, I_SENSE_CHR_2_CP_MEM);
    
    // Voltage per cell   
    s_powerstatus.batteryPack1.cellA.voltage      = ADC12_B_getResults(ADC12_B_BASE, V_CELL_1A_CP_MEM);
    s_powerstatus.batteryPack1.cellB.voltage      = ADC12_B_getResults(ADC12_B_BASE, V_CELL_1B_CP_MEM);
    s_powerstatus.batteryPack2.cellA.voltage      = ADC12_B_getResults(ADC12_B_BASE, V_CELL_2A_CP_MEM);
    s_powerstatus.batteryPack2.cellB.voltage      = ADC12_B_getResults(ADC12_B_BASE, V_CELL_2B_CP_MEM);
    
    // Combined voltage per battery   
    s_powerstatus.batteryPack1.voltage            = ADC12_B_getResults(ADC12_B_BASE, V_BATTPACK_1_CP_MEM);
    s_powerstatus.batteryPack2.voltage            = ADC12_B_getResults(ADC12_B_BASE, V_BATTPACK_2_CP_MEM);
        
    // Collect voltage and current flags
    s_powerstatus.batteryPack1.cellA.overvoltage  = GPIO_getInputPinValue(GPIO_PORT_P5, OVP_FLAG_1A_PIN);
    s_powerstatus.batteryPack1.cellA.undervoltage = GPIO_getInputPinValue(GPIO_PORT_P5, UVP_FLAG_1A_PIN);
    s_powerstatus.batteryPack1.cellB.overvoltage  = GPIO_getInputPinValue(GPIO_PORT_P5, OVP_FLAG_1B_PIN);
    s_powerstatus.batteryPack1.cellB.undervoltage = GPIO_getInputPinValue(GPIO_PORT_P5, UVP_FLAG_1B_PIN);
    s_powerstatus.batteryPack1.overcurrent        = GPIO_getInputPinValue(GPIO_PORT_P3, OCP_FLAG_1_PIN);

    s_powerstatus.batteryPack2.cellA.overvoltage  = GPIO_getInputPinValue(GPIO_PORT_P3, OVP_FLAG_2A_PIN);
    s_powerstatus.batteryPack2.cellA.undervoltage = GPIO_getInputPinValue(GPIO_PORT_P3, UVP_FLAG_2A_PIN);
    s_powerstatus.batteryPack2.cellB.overvoltage  = GPIO_getInputPinValue(GPIO_PORT_P3, OVP_FLAG_2B_PIN);
    s_powerstatus.batteryPack2.cellB.undervoltage = GPIO_getInputPinValue(GPIO_PORT_P3, UVP_FLAG_2B_PIN);
    s_powerstatus.batteryPack2.overcurrent        = GPIO_getInputPinValue(GPIO_PORT_P3, OCP_FLAG_2_PIN);
}

#else
// Mock version using easy-to-identify test values
static inline void collect_power_status_mock() {
    s_powerstatus.batteryPack1.currentDraw        = 0x11ee;
    s_powerstatus.batteryPack1.currentCharge      = 0x12ee;

    s_powerstatus.batteryPack1.cellA.voltage      = 0x1Aee;
    s_powerstatus.batteryPack1.cellB.voltage      = 0x1Bee;

    s_powerstatus.batteryPack1.voltage            = 0x1111;

    s_powerstatus.batteryPack1.cellA.overvoltage  = 0;
    s_powerstatus.batteryPack1.cellA.undervoltage = 1;
    s_powerstatus.batteryPack1.cellB.overvoltage  = 0;
    s_powerstatus.batteryPack1.cellB.undervoltage = 0;
    s_powerstatus.batteryPack1.overcurrent        = 1;

    s_powerstatus.batteryPack2.currentDraw        = 0x21ee;
    s_powerstatus.batteryPack2.currentCharge      = 0x22ee;

    s_powerstatus.batteryPack2.cellA.voltage      = 0x2Aee;
    s_powerstatus.batteryPack2.cellB.voltage      = 0x2Bee;
    s_powerstatus.batteryPack2.voltage            = 0x2222;

    s_powerstatus.batteryPack2.cellA.overvoltage  = 1;
    s_powerstatus.batteryPack2.cellA.undervoltage = 0;
    s_powerstatus.batteryPack2.cellB.overvoltage  = 1;
    s_powerstatus.batteryPack2.cellB.undervoltage = 0;
    s_powerstatus.batteryPack2.overcurrent        = 0;
}
#endif

// Public BMS initialization. 
void bms_init() {
    hardware_init();
    comm_init();
}
/* END INITIALIZATION LOGIC  */

/* 
 * RTC ISR and main program logic. 
 */  
// Public interval data collection
void bms_collectdata() {
#ifdef MOCK_POWER
    collect_power_status_mock();
#else
    collect_power_status();
#endif

    // TI ADS7138IRTER EXTERNAL ADC DATA  (THERMISTORS)
    // NOTE: Profile this.. might be able to ask for this directly in callback 
    for (uint8_t i = 0 ; i < ADC_NUM_CHANNELS; i++) {
        ADS7138IRTER_SingleRegisterWrite(&s_swi2c_descriptor, ADS7138_CHANNEL_SEL_REGISTER, i);
        s_temperature_status.thermistors.arr[i] = ADS7138IRTER_Read(&s_swi2c_descriptor); 
    }

    s_isr_triggered = false;
}

bool bms_isr_triggered() {
    return (bool)s_isr_triggered;
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
    switch (__even_in_range(RTCIV, 16)) {
        case RTCIV_NONE:         break;
        case RTCIV_RTCRDYIFG: 
            s_isr_triggered = true; 
            break;
        case RTCIV_RTCTEVIFG:    break; 
        case RTCIV_RTCAIFG:      /* alarm */ break;
        case RTCIV_RT0PSIFG:     /* prescale 0 */ break;
        case RTCIV_RT1PSIFG:     /* prescale 1 */ break;
        case RTCIV_RTCOFIFG:     /* oscillator fault: clear fault, maybe restart LFXT */ break;
        default:                 break;   // (should not happen)
    }
}
