#include "PDS.h"

#include "ADC_Read.h"
#include "ASN1SCC/asn1_lfp.h"
#include "asn1/_systems.h"
#include "asn1/pds.h"
#include "i2c.h"
#include "lfp_i2c.h"
#include "msp_utils.h"

#include <msp430.h>
#include <stddef.h>

#if defined(__MSP430FR5989__) || defined(__MSP430FR6989__)
#include "rtc_c.h"
#elif defined(__MSP430FR5969__)
#include "rtc_b.h"
#endif

#define PDS_FW_VERSION 1

static const PDSHealthCheckResponse s_health_check = {
    .hash = {
        .arr[0] = 0xD,
        .arr[1] = 0xE,
        .arr[2] = 0xA,
        .arr[3] = 0xD,
        .arr[4] = 0xB,
        .arr[5] = 0xE,
        .arr[6] = 0xE,
        .arr[7] = 0xF,
        .arr[8] = 0x0,
        .arr[9] = 0x0,
        .arr[10] = 0x0,
        .arr[11] = 0x0,
        .arr[12] = 0x0,
        .arr[13] = 0x0,
        .arr[14] = 0x0,
        .arr[15] = 0x0
    }
};

static volatile bool s_isr_triggered;
static bool s_heartbeat_received;
static PDSConverterMonitorResponse s_converter_data;
static lfp_stream_ctx_t s_lfp_ctx;
static uint8_t s_rx_body_buffer[ASN1_LFP_RECV_BUF_SIZE(PDSHealthCheckResponse)];
static uint8_t s_tx_buffer[ASN1_LFP_SEND_BUF_SIZE(PDSHealthCheckResponse)];
static uint32_t s_uptime;

static uint8_t s_flag_ports[] = {
    CONV_FLAG1_X_PLUS_PORT, CONV_FLAG2_X_PLUS_PORT,
    CONV_FLAG1_X_MINUS_PORT, CONV_FLAG2_X_MINUS_PORT,
    CONV_FLAG1_Y_PLUS_PORT, CONV_FLAG2_Y_PLUS_PORT,
    CONV_FLAG1_Y_MINUS_PORT, CONV_FLAG2_Y_MINUS_PORT
};
static uint8_t s_flag_pins[] = {
    CONV_FLAG1_X_PLUS_PIN, CONV_FLAG2_X_PLUS_PIN,
    CONV_FLAG1_X_MINUS_PIN, CONV_FLAG2_X_MINUS_PIN,
    CONV_FLAG1_Y_PLUS_PIN, CONV_FLAG2_Y_PLUS_PIN,
    CONV_FLAG1_Y_MINUS_PIN, CONV_FLAG2_Y_MINUS_PIN
};

static void pds_reboot(void)
{
    GPIO_setOutputLowOnPin(CONV_RUN_A_PORT, CONV_RUN_A_PIN);
    GPIO_setOutputLowOnPin(CONV_RUN_B_PORT, CONV_RUN_B_PIN);
}

static void pds_fault(void)
{
    pds_reboot();
}

static void pds_process(void)
{
    ADC12_B_startConversion(ADC12_B_BASE,
                            ADC12_B_START_AT_ADC12MEM0,
                            ADC12_B_SEQOFCHANNELS);
    while (ADC12_B_isBusy(ADC12_B_BASE)) {
    }

    s_converter_data.int5vVs =
        ADC12_B_getResults(ADC12_B_BASE, ADC12_B_MEMORY_0);
    s_converter_data.reg5vVs =
        ADC12_B_getResults(ADC12_B_BASE, ADC12_B_MEMORY_1);
    s_converter_data.converterA5vVs =
        ADC12_B_getResults(ADC12_B_BASE, ADC12_B_MEMORY_2);
    s_converter_data.converterB5vVs =
        ADC12_B_getResults(ADC12_B_BASE, ADC12_B_MEMORY_3);
    s_converter_data.temperatureSense =
        ADC12_B_getResults(ADC12_B_BASE, ADC12_B_MEMORY_4);

    for (size_t i = 0; i < sizeof(s_flag_ports); ++i) {
        if (GPIO_getInputPinValue(s_flag_ports[i], s_flag_pins[i]) == GPIO_INPUT_PIN_HIGH) {
            pds_fault();
        }
    }
}

static void init_hardware(void)
{
    WDTCTL = WDTPW | WDTHOLD;

    P1SEL0 |= BIT6 | BIT7;
    P1SEL1 &= ~(BIT6 | BIT7);

    PM5CTL0 &= ~LOCKLPM5;

    GPIO_setAsOutputPin(CONV_RUN_A_PORT, CONV_RUN_A_PIN);
    GPIO_setAsOutputPin(CONV_RUN_B_PORT, CONV_RUN_B_PIN);
    GPIO_setAsInputPin(CONV_FLAG1_X_PLUS_PORT, CONV_FLAG1_X_PLUS_PIN);
    GPIO_setAsInputPin(CONV_FLAG2_X_PLUS_PORT, CONV_FLAG2_X_PLUS_PIN);
    GPIO_setAsInputPin(CONV_FLAG1_X_MINUS_PORT, CONV_FLAG1_X_MINUS_PIN);
    GPIO_setAsInputPin(CONV_FLAG2_X_MINUS_PORT, CONV_FLAG2_X_MINUS_PIN);
    GPIO_setAsInputPin(CONV_FLAG1_Y_PLUS_PORT, CONV_FLAG1_Y_PLUS_PIN);
    GPIO_setAsInputPin(CONV_FLAG2_Y_PLUS_PORT, CONV_FLAG2_Y_PLUS_PIN);
    GPIO_setAsInputPin(CONV_FLAG1_Y_MINUS_PORT, CONV_FLAG1_Y_MINUS_PIN);
    GPIO_setAsInputPin(CONV_FLAG2_Y_MINUS_PORT, CONV_FLAG2_Y_MINUS_PIN);
    GPIO_setOutputHighOnPin(CONV_RUN_A_PORT, CONV_RUN_A_PIN);
    GPIO_setOutputHighOnPin(CONV_RUN_B_PORT, CONV_RUN_B_PIN);


    clock_init_16mhz();

#if defined(__MSP430FR5989__) || defined(__MSP430FR6989__)
    Calendar current_time = {
        .Seconds = 0, .Minutes = 0, .Hours = 0, .DayOfWeek = 0,
        .DayOfMonth = 0, .Month = 0, .Year = 0x7E9
    };
    RTC_C_initCalendar(RTC_C_BASE, &current_time, RTC_C_FORMAT_BCD);
    RTC_C_configureCalendarAlarmParam alarm = {0};
    alarm.minutesAlarm = 0x2;
    RTC_C_configureCalendarAlarm(RTC_C_BASE, &alarm);
    RTC_C_clearInterrupt(RTC_C_BASE, RTC_C_CLOCK_READ_READY_INTERRUPT |
                         RTC_C_TIME_EVENT_INTERRUPT | RTC_C_CLOCK_ALARM_INTERRUPT);
#elif defined(__MSP430FR5969__)
    Calendar current_time = {
        .Seconds = 0, .Minutes = 0, .Hours = 0, .DayOfWeek = 0,
        .DayOfMonth = 0, .Month = 0, .Year = 0x7E9
    };
    RTC_B_initCalendar(RTC_B_BASE, &current_time, RTC_B_FORMAT_BCD);
    RTC_B_configureCalendarAlarmParam alarm = {0};
    alarm.minutesAlarm = 0x2;
    RTC_B_configureCalendarAlarm(RTC_B_BASE, &alarm);
    RTC_B_clearInterrupt(RTC_B_BASE, RTC_B_CLOCK_READ_READY_INTERRUPT |
                         RTC_B_TIME_EVENT_INTERRUPT | RTC_B_CLOCK_ALARM_INTERRUPT);
#endif
    rtc_init();

    ADC_initMultiple();
    ADC12_B_disableConversions(ADC12_B_BASE, 1);
    ADC_PinSelect(INT_5V_VS, ADC12_B_MEMORY_0);
    ADC_PinSelect(REG_5V_VS, ADC12_B_MEMORY_1);
    ADC_PinSelect(A_5V_VS, ADC12_B_MEMORY_2);
    ADC_PinSelect(B_5V_VS, ADC12_B_MEMORY_3);

    ADC12_B_configureMemoryParam end_of_sequence = {
        .memoryBufferControlIndex = ADC12_B_MEMORY_4,
        .inputSourceSelect = TEMP_SENSE,
        .refVoltageSourceSelect = ADC12_B_VREFPOS_AVCC_VREFNEG_VSS,
        .endOfSequence = ADC12_B_ENDOFSEQUENCE,
        .windowComparatorSelect = ADC12_B_WINDOW_COMPARATOR_DISABLE,
        .differentialModeSelect = ADC12_B_DIFFERENTIAL_MODE_DISABLE
    };
    ADC12_B_configureMemory(ADC12_B_BASE, &end_of_sequence);
    ADC12CTL1 |= ADC12CONSEQ_1;
}

static bool on_header(const lfp_header_t *header, void *ctx)
{
    (void)header;
    (void)ctx;
    if (lfp_i2c_state() == I2C_SLAVE_STATE_RESPONSE) {
        lfp_i2c_transition(I2C_SLAVE_STATE_REQUEST);
    }
    return true;
}

static void on_decode_error(int error, const asn1_lfp_decode_data_t *data)
{
    (void)error;
    (void)data;
}

static void on_stream_error(lfp_code_t reason, void *ctx)
{
    (void)reason;
    (void)ctx;
}

static void send_system_status(const PDSSystemStatusRequest *request,
                               const asn1_lfp_decode_data_t *data)
{
    (void)request;
    lfp_i2c_transition(I2C_SLAVE_STATE_PROCESSING);
    // GIE -- this is technically in the i2c isr, we don't want getting stuck here to stall BMS
    __enable_interrupt();
    PDSSystemStatusResponse response = {.version = PDS_FW_VERSION, .uptime = s_uptime++};
    lfp_i2c_transition(I2C_SLAVE_STATE_PROCESSING);
    uint16_t size = ASN1_LFP_SERIALIZE(data->p_header->initiator, pdsSystemId,
                                        PDSSystemStatusResponse, s_tx_buffer,
                                        sizeof(s_tx_buffer), response);
    __disable_interrupt();

    lfp_i2c_set_txbuf(s_tx_buffer, size);
}

static void send_health_check(const PDSHealthCheckRequest *request,
                              const asn1_lfp_decode_data_t *data)
{
    (void)request;
    lfp_i2c_transition(I2C_SLAVE_STATE_PROCESSING);
    // GIE -- this is technically in the i2c isr, we don't want getting stuck here to stall BMS
    __enable_interrupt();
    s_heartbeat_received = true;
    uint16_t size = ASN1_LFP_SERIALIZE(data->p_header->initiator, pdsSystemId,
                                        PDSHealthCheckResponse, (unsigned char*)s_tx_buffer,
                                        sizeof(s_tx_buffer), s_health_check);
    __disable_interrupt();
    (void)lfp_i2c_set_txbuf(s_tx_buffer, size);
}

static void send_converter_status(const PDSConverterMonitorRequest *request,
                                  const asn1_lfp_decode_data_t *data)
{
    (void)request;
    lfp_i2c_transition(I2C_SLAVE_STATE_PROCESSING);
    __enable_interrupt();
    uint16_t size = ASN1_LFP_SERIALIZE(data->p_header->initiator, pdsSystemId,
                                        PDSConverterMonitorResponse, s_tx_buffer,
                                        sizeof(s_tx_buffer), s_converter_data);
    __disable_interrupt();
    lfp_i2c_set_txbuf(s_tx_buffer, size);
}

static void send_reboot(const PDSRebootRequest *request,
                        const asn1_lfp_decode_data_t *data)
{
    (void)request;
    PDSRebootResponse response = {.placeholder = 0};
    __enable_interrupt();
    pds_reboot();
    lfp_i2c_transition(I2C_SLAVE_STATE_PROCESSING);
    uint16_t size = ASN1_LFP_SERIALIZE(data->p_header->initiator, pdsSystemId,
                                        PDSRebootResponse, s_tx_buffer,
                                        sizeof(s_tx_buffer), response);
    __disable_interrupt();
    lfp_i2c_set_txbuf(s_tx_buffer, size);
}

static void on_message(const lfp_header_t *header, const uint8_t *body,
                       uint16_t body_length, void *ctx)
{
    const asn1_lfp_decode_data_t data = {
        .p_header = header, .p_body = body, .body_length = body_length,
        .p_ctx = ctx, .p_on_error_cb = on_decode_error
    };
    if (ASN1_LFP_HANDLE_MSG(data, pdsSystemId, PDSSystemStatusRequest, send_system_status)) return;
    if (ASN1_LFP_HANDLE_MSG(data, pdsSystemId, PDSHealthCheckRequest, send_health_check)) return;
    if (ASN1_LFP_HANDLE_MSG(data, pdsSystemId, PDSConverterMonitorRequest, send_converter_status)) return;
    (void)ASN1_LFP_HANDLE_MSG(data, pdsSystemId, PDSRebootRequest, send_reboot);
}

static void init_communication(void)
{
    lfp_i2c_init(&s_lfp_ctx, I2C_MODULE_UCB0, SLAVE_ADDR);
    lfp_stream_init(&s_lfp_ctx, s_rx_body_buffer, sizeof(s_rx_body_buffer),
                    on_header, on_message, on_stream_error, NULL);
}

void pds_init(void)
{
    init_hardware();
    init_communication();
}

bool pds_isr_triggered(void)
{
    return s_isr_triggered;
}

void pds_collectdata(void)
{
    pds_process();
    s_isr_triggered = false;
}

#if defined(__TI_COMPILER_VERSION__) || defined(__IAR_SYSTEMS_ICC__)
#pragma vector=RTC_VECTOR
__interrupt
#elif defined(__GNUC__)
__attribute__((interrupt(RTC_VECTOR)))
#endif
void RTC_ISR(void)
{
    switch (__even_in_range(RTCIV, 16)) {
    case 2:
        s_isr_triggered = true;
        break;
    case 4:
        if (s_heartbeat_received) {
            s_heartbeat_received = false;
        } else {
            pds_reboot();
        }
        break;
    case 6:
        pds_reboot();
        break;
    default:
        break;
    }
}
