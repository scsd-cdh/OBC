#include "BMS.h"
#include "ADC_Read.h"
#if defined (__MSP430FR5989__)
#include "rtc_c.h"
#elif defined (__MSP430FR5969__)
#include "rtc_b.h"
#endif

// ADC pins
// Current sensing 
// NOTE: 1 and 2 does not refer to battery number!! We don't know what it means right now! Waiting  for elec update 
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


static void initADCs() 
{
    ADC_init_Standard();

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
    ADC_PinSelect(V_BATTPACK_2_CP_PIN,      V_BATTPACK_2_CP_MEM);
}

static void initGPIO()
{
    WDTCTL = WDTPW | WDTHOLD;   // Stop watchdog timer

    // Configure GPIO
    P1OUT &= ~BIT0;                           // Clear P1.0 output latch
    P1DIR |= BIT0;                            // For LED
    P1SEL1 |= BIT6 | BIT7;                    // I2C pins
    // Disable the GPIO power-on default high-impedance mode to activate
    // previously configured port settings
    PM5CTL0 &= ~LOCKLPM5;
}

// FIXME: Copied directly from PDS
static void initClockTo16MHz()
{
    // Configure one FRAM waitstate as required by the device datasheet for MCLK
    // operation beyond 8MHz _before_ configuring the clock system.
    FRCTL0 = FRCTLPW | NWAITS_1;

    // Clock System Setup
    CSCTL0_H = CSKEY >> 8;                    // Unlock CS registers
    CSCTL1 = DCORSEL | DCOFSEL_4;             // Set DCO to 16MHz
    CSCTL2 = SELA__VLOCLK | SELS__DCOCLK | SELM__DCOCLK;
    CSCTL3 = DIVA__1 | DIVS__1 | DIVM__1;     // Set all dividers

    CSCTL0_H = 0;                             // Lock CS registers
}


#if defined (__MSP430FR5989__)
static void initRTCC()
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
static void initRTCB()
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
    #if defined (__MSP430FR5989__)
    initRTCC();
    #elif defined (__MSP430FR6989__)
    initRTCB();
    #endif
    initADCs();
}

// App communications
#define SLAVE_ADDR 0x08

volatile uint8_t buffer[8];
const uint8_t outBuffer[9] = { 1, 2, 3, 4, 5, 6, 7, 8, 9 };

static void I2C_Proc_RX_Data(uint8_t data);

static uint16_t SendTelemetryResponse()
{
    uint8_t bytes[TINYPROTOCOL_MAX_PACKET_SIZE];
    uint8_t count = 0;
    uint8_t* pbyte = bytes;

    while(TINYPROTOCOL_TelemetryBytesLeft() > 0) {
        int16_t result = TINYPROTOCOL_ReadNextTelemetryByte(pbyte);
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
    return SendTelemetryResponse();
}

static int16_t ProcessTelecommand(uint8_t command, const uint8_t* buffer, uint8_t size)
{
    return 0;
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

void InitAppComm()
{
    sI2cConfigCb_t i2cConfig = {
      .Rx_Proc_Data = I2C_Proc_RX_Data,
        .slave_addr = SLAVE_ADDR,
    };
    initI2C(&i2cConfig);

    TINYPROTOCOL_Initialize();
    TINYPROTOCOL_RegisterTelemetryChannel(BMS_SYSTEM_STATUS_ID, sSystemStatus.buffer , sizeof(sSystemStatus.buffer));
    TINYPROTOCOL_RegisterTelemetryChannel(BMS_CURRENT_DRAW_ID, sCurrentDraw.buffer, sizeof(sCurrentDraw.buffer));
    TINYPROTOCOL_RegisterTelemetryChannel(BMS_CURRENT_CHARGE_ID, sCurrentCharge.buffer, sizeof(sCurrentCharge.buffer));
    TINYPROTOCOL_RegisterTelemetryChannel(BMS_VOLTAGE_BATTERY1_ID, sVoltageBattery1.buffer, sizeof(sVoltageBattery1.buffer));
    TINYPROTOCOL_RegisterTelemetryChannel(BMS_VOLTAGE_BATTERY1_ID, sCurrentDraw.buffer, sizeof(sCurrentDraw.buffer));
    TINYPROTOCOL_RegisterTelemetryChannel(BMS_VOLTAGE_COMBINED_ID, sCombinedBatteryVoltage.buffer, sizeof(sCombinedBatteryVoltage));
}

// ISR
void RoutineCycle_Process()
{
    sCurrentDraw.isense1 = Read_ADC(I_SENSE_VUR_1_CP_MEM);
    sCurrentDraw.isense2 = Read_ADC(I_SENSE_VUR_2_CP_MEM);
    sCurrentCharge.isense1 = Read_ADC(I_SENSE_CHR_1_CP_MEM);
    sCurrentCharge.isense2 = Read_ADC(I_SENSE_CHR_2_CP_MEM);

    sVoltageBattery1.vcell_a = Read_ADC(V_CELL_1A_CP_MEM);
    sVoltageBattery1.vcell_b = Read_ADC(V_CELL_1B_CP_MEM);
    sVoltageBattery2.vcell_a = Read_ADC(V_CELL_2A_CP_MEM);
    sVoltageBattery2.vcell_b = Read_ADC(V_CELL_2B_CP_MEM);

    sCombinedBatteryVoltage.vbatt1 = Read_ADC(V_BATTPACK_1_CP_MEM);
    sCombinedBatteryVoltage.vbatt2 = Read_ADC(V_BATTPACK_2_CP_MEM);
}

/*ISR that maintains LPM until 30 minutes has passed*/
#if defined(__TI_COMPILER_VERSION__) || defined(__IAR_SYSTEMS_ICC__)
#pragma vector=RTC_VECTOR
__interrupt
#elif defined(__GNUC__)
__attribute__((interrupt(RTC_VECTOR)))
#endif
void RTC_ISR (void)
{
    switch (__even_in_range(RTCIV,16))
    {
        case 2:     //RTCRDYIFG, triggered every second
            // TODO - move to another timer if it needs to be called more than once a second
            RoutineCycle_Process();
            break;
    }
}
