#include "BMS.h"
#include "ADC_Read.h"
#if defined (__MSP430FR5989__)
#include "rtc_c.h"
#elif defined (__MSP430FR5969__)
#include "rtc_b.h"
#endif

// ADC pins
#define I_SENSE_VUR_1_CP_PIN      ADC12_B_INPUT_A10 // PIN 42 5989
#define I_SENSE_VUR_2_CP_PIN      ADC12_B_INPUT_A0  // PIN 39 5989
#define I_SENSE_CHR_1_CP_PIN      ADC12_B_INPUT_A9  // PIN 41 5989
#define I_SENSE_CHR_2_CP_PIN      ADC12_B_INPUT_A8  // PIN 40 5989
#define V_CELL_1A_CP_PIN          ADC12_B_INPUT_A13 // PIN 45 5989
#define V_CELL_1B_CP_PIN          ADC12_B_INPUT_A12 // PIN 44 5989
#define V_CELL_2A_CP_PIN          ADC12_B_INPUT_A2  // PIN 37 5989
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
#define V_BATTPACK_1_CP_MEM       ADC12_B_MEMORY_7  
#define V_BATTPACK_2_CP_MEM       ADC12_B_MEMORY_8

PowerStatusResp_t PowerStatusBattery1Out = {
    .current = 0,
    .voltage = 0,
    .battery_number = 1,
};

PowerStatusResp_t PowerStatusBattery2Out = {
    .current = 0,
    .voltage = 0,
    .battery_number = 2,
};

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
    #elif defined (MSP430FR6989)
    initRTCB();
    #endif
    initADCs();
}

static PowerStatusResp_t sPowerStatusBattery2Out = {
    .current = 0,
    .voltage = 0,
    .battery_number = 2,
};

void RoutineCycle_Process()
{
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
