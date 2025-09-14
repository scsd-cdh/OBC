#include "BMS.h"
#include "ADC_Read.h"
#include "rtc_c.h"


// ADC pin mappings
// I_SENSE_CHR_CP: Current sensing for battery charging current
// This is supposed to be for pin 40. We need to map pin to memory where ADC does it's thing 
// ADC pin numbers
#define I_SENSE_CHR_CP_ADC_IN_PIN ADC12_B_INPUT_A8 // PIN 40

// ADC MEM buffer
#define I_SENSE_CHR_CP_ADC_MEM ADC12_B_MEMORY_0

PowerStatusResp_t sPowerStatusBattery1Out = {
    .current = 0,
    .voltage = 0,
    .battery_number = 1,
};

static void initADCs() 
{
    ADC_init_Standard();

    // Selects what pin to get mapped to what ADC memory thing
    ADC_PinSelect(I_SENSE_CHR_CP_ADC_IN_PIN, I_SENSE_CHR_CP_ADC_MEM);
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

void initBSP()
{
    initClockTo16MHz();
    initGPIO();
    initRTCC();
    initADCs();
}

static PowerStatusResp_t sPowerStatusBattery2Out = {
    .current = 0,
    .voltage = 0,
    .battery_number = 2,
};

void RoutineCycle_Process()
{
    sPowerStatusBattery1Out.current = Read_ADC(I_SENSE_CHR_CP_ADC_MEM);
}


/*ISR that maintains LPM until 30 minutes has passed*/
#if defined(__TI_COMPILER_VERSION__) || defined(__IAR_SYSTEMS_ICC__)
#pragma vector=RTC_VECTOR
__interrupt
#elif defined(__GNUC__)
__attribute__((interrupt(RTC_VECTOR)))
#endif
void RTC_C_ISR (void)
{
    switch (__even_in_range(RTCIV,16))
    {
        case 2:     //RTCRDYIFG, triggered every second
            // TODO - move to another timer if it needs to be called more than once a second
            RoutineCycle_Process();
            break;
    }
}
