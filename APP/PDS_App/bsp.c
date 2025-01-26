
#include "bsp.h"
#include "gpio.h"
#include "i2c.h"
#include "rtc_b.h"
#include "ADC_Read.h"

#include <msp430.h> 

/**
 * @brief Initializes the system clock to 16MHz to support I2C and other peripherals.
 */
static void initClockTo16MHz();

/**
 * @brief Configures GPIO pins for I2C communication and debugging (LED indicators).
 */
static void initGPIO();

/**
 * @brief Configures RTC pins.
 */
static void initRTCB();

/**
 * @brief Configures ADC pins.
 */
static void initADCs();

//******************************************************************************
// Device Initialization *******************************************************
//******************************************************************************
void initBSP()
{
    initClockTo16MHz();
    initGPIO();
    initRTCB();
    initADCs();
}

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

static void initGPIO()
{
    // Configure GPIO
    P1OUT &= ~BIT0;                           // Clear P1.0 output latch
    P1DIR |= BIT0;                            // For LED
    P1SEL1 |= BIT6 | BIT7;                    // I2C pins
    // Disable the GPIO power-on default high-impedance mode to activate
    // previously configured port settings
    PM5CTL0 &= ~LOCKLPM5;

    // Converter Run Pins
    GPIO_setAsOutputPin(CONV_RUN_A_PORT, CONV_RUN_A_PIN);
    GPIO_setAsOutputPin(CONV_RUN_B_PORT, CONV_RUN_B_PIN);

    // Flags pins
    GPIO_setAsInputPin(CONV_FLAG1_X_PLUS_PORT, CONV_FLAG1_X_PLUS_PIN);
    GPIO_setAsInputPin(CONV_FLAG2_X_PLUS_PORT, CONV_FLAG2_X_PLUS_PIN);
    GPIO_setAsInputPin(CONV_FLAG1_X_MINUS_PORT, CONV_FLAG1_X_MINUS_PIN);
    GPIO_setAsInputPin(CONV_FLAG2_X_MINUS_PORT, CONV_FLAG2_X_MINUS_PIN);
    GPIO_setAsInputPin(CONV_FLAG1_Y_PLUS_PORT, CONV_FLAG1_Y_PLUS_PIN);
    GPIO_setAsInputPin(CONV_FLAG2_Y_PLUS_PORT, CONV_FLAG2_Y_PLUS_PIN);
    GPIO_setAsInputPin(CONV_FLAG1_Y_MINUS_PORT, CONV_FLAG1_Y_MINUS_PIN);
    GPIO_setAsInputPin(CONV_FLAG2_Y_MINUS_PORT, CONV_FLAG2_Y_MINUS_PIN);

    // Init to High for power up
    GPIO_setOutputHighOnPin(CONV_RUN_A_PORT, CONV_RUN_A_PIN);
    GPIO_setOutputHighOnPin(CONV_RUN_B_PORT, CONV_RUN_B_PIN);
}

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
    param.minutesAlarm      = 0x1;  // Currently set to 1 minute for testing - TODO change to 24 hours
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
//        RTC_B_CLOCK_READ_READY_INTERRUPT +
//        RTC_B_TIME_EVENT_INTERRUPT +
        RTC_B_CLOCK_ALARM_INTERRUPT
        );

    //Start RTC Clock
    RTC_B_startClock(RTC_B_BASE);
}

static void initADCs() 
{
    ADC_init_Standard();

    ADC_PinSelect(INT_5V_VS, ADC12_B_MEMORY_0);
    ADC_PinSelect(REG_5V_VS, ADC12_B_MEMORY_1);
    ADC_PinSelect(A_5V_VS, ADC12_B_MEMORY_2);
    ADC_PinSelect(B_5V_VS, ADC12_B_MEMORY_3);
    ADC_PinSelect(TEMP_SENSE, ADC12_B_MEMORY_4);
}
