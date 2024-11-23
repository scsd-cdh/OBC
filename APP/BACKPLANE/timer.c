/*
 * App.c
 *
 *  Created on: Aug 10, 2024
 *      Author: Rowan
 */


// TODO: The commented code is mostly I2C communications, this file should be Timer only

#include <timer.h>
#include <rtc_b.h>
#include <gpio.h>

#define SLAVE_ADDRESS 0x08
//uint8_t stopCondition = 0x13;


// 255 is used instead of a defined literal because the use of one led to a definition error

static volatile appStatus app;  // Current state of the Backplane

//static uint8_t commandBuffer[255] = {0};    // Buffer for receiving commands from OBC
//static volatile uint8_t bytesReceived = 0;      // Number of bytes received from OBC for one command
//static volatile uint8_t receivingData = 0;      // Acts like a boolean
//
//static uint8_t responseBuffer[255] = {0};   // Buffer for response to be sent to OBC
//static volatile uint8_t *responsePointer = 0;

void init_App(){
    app = timer_mode;       // Start in timer mode
}


/* Timer functionality */

void startCountdownAlarm() {

    Calendar currentTime;

    /*
    * Select Port J
    * Set Pin 4, 5 to input Primary Module Function, LFXT.
    */
    GPIO_setAsPeripheralModuleFunctionInputPin(
        GPIO_PORT_PJ,
        GPIO_PIN4 + GPIO_PIN5,
        GPIO_PRIMARY_MODULE_FUNCTION
    );

    //Initialize LFXT1
    CS_turnOnLFXT(
        CS_LFXT_DRIVE_3
        );

    //Setup for Calendar
    currentTime.Seconds    = 0x00;
    currentTime.Minutes    = 0x26;
    currentTime.Hours      = 0x13;
    currentTime.DayOfWeek  = 0x03;
    currentTime.DayOfMonth = 0x20;
    currentTime.Month      = 0x07;
    currentTime.Year       = 0x2011;

    //Initialize Calendar Mode of RTC
    RTC_B_initCalendar(RTC_B_BASE, &currentTime, RTC_B_FORMAT_BCD);

    //Setup Calendar Alarm for 30 minutes after start.
    RTC_B_configureCalendarAlarmParam param = {0};
    param.minutesAlarm      = 0x27;  // Currently set to 1 minute for testing
    param.hoursAlarm        = 0x13;
    param.dayOfWeekAlarm    = 0x03;
    param.dayOfMonthAlarm   = 0x20;
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

    //Enter low power mode with interrupts enabled
    __bis_SR_register(LPM0_bits + GIE);

    __no_operation();

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
    switch (__even_in_range(RTCIV,16)){
        case 2:     //RTCRDYIFG, triggered every second
            break;
        case 4:     //RTCEVIFG, triggered every minute
            break;
        case 6:     //RTCAIFG, triggers at set alarm
            // Disable and clear interrupts
            RTC_B_disableInterrupt(RTC_B_BASE,
                RTC_B_CLOCK_READ_READY_INTERRUPT +
                RTC_B_TIME_EVENT_INTERRUPT +
                RTC_B_CLOCK_ALARM_INTERRUPT
                );
            RTC_B_clearInterrupt(RTC_B_BASE,
                RTC_B_CLOCK_READ_READY_INTERRUPT +
                RTC_B_TIME_EVENT_INTERRUPT +
                RTC_B_CLOCK_ALARM_INTERRUPT
                );
            // Exit low power mode to resume run()
            __bic_SR_register_on_exit(LPM0_bits);
            // Switch to idle mode
            app = idle;
            // Initialize I2C for idle mode
            void initializeI2C();
            break;
        default: break;
    }
}



/* Data reading functionality */

void initializeI2C()
{

//    // Configure Pins for I2C
//    //Set P1.6 and P1.7 as Secondary Module Function Input.
//    /*
//
//    * Select Port 1
//    * Set Pin 6, 7 to input Secondary Module Function, (UCB0SIMO/UCB0SDA, UCB0SOMI/UCB0SCL).
//    */
//    GPIO_setAsPeripheralModuleFunctionInputPin(
//        GPIO_PORT_P1,
//        GPIO_PIN6 + GPIO_PIN7,
//        GPIO_SECONDARY_MODULE_FUNCTION
//    );
//
//    // eUSCI configuration
//    EUSCI_B_I2C_initSlaveParam param = {0};
//    param.slaveAddress = SLAVE_ADDRESS;
//    param.slaveAddressOffset = EUSCI_B_I2C_OWN_ADDRESS_OFFSET0;
//    param.slaveOwnAddressEnable = EUSCI_B_I2C_OWN_ADDRESS_ENABLE;
//    EUSCI_B_I2C_initSlave(EUSCI_B0_BASE, &param);
//
//    EUSCI_B_I2C_enable(EUSCI_B0_BASE);
//
//    EUSCI_B_I2C_clearInterrupt(EUSCI_B0_BASE,
//                EUSCI_B_I2C_RECEIVE_INTERRUPT0
//                );
//
//    EUSCI_B_I2C_enableInterrupt(EUSCI_B0_BASE,
//                EUSCI_B_I2C_RECEIVE_INTERRUPT0
//                );
//
//    //Enter low power mode with interrupts enabled
//    __bis_SR_register(CPUOFF + GIE);
//    __no_operation();
//}
//
//void suspendI2CInterrupts() {
//    EUSCI_B_I2C_disableInterrupt(EUSCI_B0_BASE,
//         EUSCI_B_I2C_RECEIVE_INTERRUPT0
//         );
//}
//
//void resumeI2CInterrupts() {
//    EUSCI_B_I2C_enableInterrupt(EUSCI_B0_BASE,
//        EUSCI_B_I2C_RECEIVE_INTERRUPT0
//        );
//    __bis_SR_register(CPUOFF + GIE); // Enter LPM with interrupts
//}
//
//
//
//void commandHandler() {
//
//    switch(commandBuffer[0]){  // Command ID will always be the first byte transmitted
//
//        case SystemStatus:
//            if (bytesReceived != SystemStatusSize) {
//                // return error
//            } else if (commandBuffer[bytesReceived] != stopCondition) {
//                // return error
//            } else {
//                app = system_status;
//            }
//            break;
//        case HealthCheck:
//            if (bytesReceived != HealthCheckSize) {
//                // return error
//            } else if (commandBuffer[bytesReceived] != stopCondition) {
//                // return error
//            } else {
//                app = health_check;
//            }
//            break;
//        case Temperature:
//            if (bytesReceived != TemperatureSize) {
//                // return error
//            } else if (commandBuffer[bytesReceived] != stopCondition) {
//                // return error
//            } else {
//                app = temp_reading;
//            }
//            break;
//        case TelecomAcknowledge:
//            if (bytesReceived != TelecomAcknowledgeSize) {
//                // return error
//            } else if (commandBuffer[bytesReceived] != stopCondition) {
//                // return error
//            } else {
//                app = telecom_acknowledge;
//            }
//            break;
//        default: // Unrecognized command
//            // return error
//            break;
//    }
}


//#if defined(__TI_COMPILER_VERSION__) || defined(__IAR_SYSTEMS_ICC__)
//#pragma vector=USCI_B0_VECTOR
//__interrupt
//#elif defined(__GNUC__)
//__attribute__((interrupt(USCI_B0_VECTOR)))
//#endif
//void USCIB0_ISR(void)
//{
//    switch(__even_in_range(UCB0IV, USCI_I2C_UCBIT9IFG))
//    {
//        case USCI_NONE:             // No interrupts break;
//            break;
//        case USCI_I2C_UCALIFG:      // Arbitration lost
//            break;
//        case USCI_I2C_UCNACKIFG:    // NAK received (master only)
//            break;
//        case USCI_I2C_UCSTTIFG:     // START condition detected with own address (slave mode only)
//            break;
//        case USCI_I2C_UCSTPIFG:     // STOP condition detected (master & slave mode)
//
//            // Receive the final byte and process the command
//            commandBuffer[bytesReceived++] = EUSCI_B_I2C_slaveGetData(EUSCI_B0_BASE);
//            receivingData = 0;      // End transmission of data
//            suspendI2CInterrupts(); // Suspend interrupts and exit LPM during command handling
//            commandHandler();
//            __bic_SR_register_on_exit(CPUOFF); // Exit LPM to resume in correct mode
//
//            break;
//        case USCI_I2C_UCRXIFG3:     // RXIFG3
//            break;
//        case USCI_I2C_UCTXIFG3:     // TXIFG3
//            break;
//        case USCI_I2C_UCRXIFG2:     // RXIFG2
//            break;
//        case USCI_I2C_UCTXIFG2:     // TXIFG2
//            break;
//        case USCI_I2C_UCRXIFG1:     // RXIFG1
//            break;
//        case USCI_I2C_UCTXIFG1:     // TXIFG1
//            break;
//        case USCI_I2C_UCRXIFG0:     // RXIFG0 -> slave mode receive data
//            break;
//        case USCI_I2C_UCTXIFG0:     // TXIFG0 -> slave mode transmit data
//            break;
//        case USCI_I2C_UCBCNTIFG:    // Byte count limit reached (UCBxTBCNT)
//            break;
//        case USCI_I2C_UCCLTOIFG:    // Clock low timeout - clock held low too long
//            break;
//        case USCI_I2C_UCBIT9IFG:    // Generated on 9th bit of a transmit (for debugging)
//            break;
//        default:
//            break;
//    }
//}



// LEDs used below to verify state of board


void run(){

    switch(app){
        /* Count-down mode after power-on before switching to idle state */
        case timer_mode:
            startCountdownAlarm(); // Starts countdown using 30 minute alarm
            break;
        /* Idle state */
        case idle:
            responsePointer = responseBuffer;  // Reset the response buffer pointer
            resumeI2CInterrupts(); // Resume interrupts and enter LPM after executing command
            break;
        /* Single-use function to power on main board before changing to idle mode */
        case power_cdh:
            break;
        /* Modes used for each command */
        case system_status:
            GPIO_setAsOutputPin(GPIO_PORT_P1, GPIO_PIN0);  // Green LED on
            GPIO_setOutputHighOnPin(GPIO_PORT_P1, GPIO_PIN0);
            // Do action
            // Fill responseBuffer
            app = idle;
            break;
        case health_check:
            GPIO_setAsOutputPin(GPIO_PORT_P4, GPIO_PIN6);  // Red LED on
            GPIO_setOutputHighOnPin(GPIO_PORT_P4, GPIO_PIN6);
            // Do action
            // Fill responseBuffer
            app = idle;
            break;
        case temp_reading:
            // Should enter MASTER mode to get data from Temp sensors
            // EUSCI_B_I2C_setMode()
            GPIO_setAsOutputPin(GPIO_PORT_P4, GPIO_PIN6);  // Red and Green LEDs on
            GPIO_setOutputHighOnPin(GPIO_PORT_P4, GPIO_PIN6);
            GPIO_setAsOutputPin(GPIO_PORT_P1, GPIO_PIN0);
            GPIO_setOutputHighOnPin(GPIO_PORT_P1, GPIO_PIN0);
            // Do action
            // Fill responseBuffer
            app = idle;
            break;
        case telecom_acknowledge:
            GPIO_setAsOutputPin(GPIO_PORT_P4, GPIO_PIN6);  // Flash both LEDs
            GPIO_toggleOutputOnPin(GPIO_PORT_P4, GPIO_PIN6);
            GPIO_toggleOutputOnPin(GPIO_PORT_P4, GPIO_PIN6);
            GPIO_setAsOutputPin(GPIO_PORT_P1, GPIO_PIN0);
            GPIO_toggleOutputOnPin(GPIO_PORT_P1, GPIO_PIN0);
            GPIO_toggleOutputOnPin(GPIO_PORT_P1, GPIO_PIN0);
            // Do action
            // Fill responseBuffer
            app = idle;
            break;
    }

}
