
#include "AppComm.h"
#include "i2c.h"

//***************************Private functions definitions***************************************************
/**
 * @brief Sets up the system status response buffer.
 *
 * @param SystemStatusRespBuf Buffer to hold the system status response data
 * @param SystemStatusBuf Buffer holding system status command arguments
 */
static void setup_system_status_test(uint8_t *SystemStatusRespBuf, uint8_t *SystemStatusBuf);

/**
 * @brief Sets up the health check response buffer.
 *
 * @param HealthCheckRespBuf Buffer to hold the health check response data
 * @param HealthCheckBuf Buffer holding health check command arguments
 */
static void setup_health_check_test(uint8_t *HealthCheckRespBuf, uint8_t *HealthCheckBuf);

/**
 * @brief Sets up the converter monitor response buffer.
 *
 * @param ConverterMonitorRespBuf Buffer to hold the converter monitor response data
 */
static void setup_converter_monitor_test(uint8_t *ConverterMonitorRespBuf);

/**
 * @brief Sets up the telecommand acknowledgement response buffer.
 *
 * @param TelecommandAckRespBuf Buffer to hold the telecommand acknowledgement response data
 */
static void setup_telecom_acknowledge_test(uint8_t *TelecommandAckRespBuf);

/**
 * @brief Processes the received I2C command, updating the application state based on
 *        the received command.
 *
 * @param cmd Command or register address received from the master.
 */
static void I2C_Slave_ProcessCMD(uint8_t cmd);

/**
 * @brief Completes an I2C transaction. Uses the command to handle any post-transaction
 *        actions, such as copying received data to command-specific buffers.
 *
 * @param cmd The command/register address corresponding to the completed transaction.
 */
static void I2C_Slave_TransactionDone(uint8_t cmd);

//***************************Public functions***************************************************
void InitAppComm(void) {
    sI2cConfigCb_t i2c_cb = {
        .Rx_Proc_Cmd = &I2C_Slave_ProcessCMD;
        .Rx_Proc_Data = &I2C_Slave_TransactionDone
    };
    initI2C(&i2c_cb);
}

// State Machine
void RunAppComm(void) {
    switch(app){

        /* Idle state */
        case idle:
            // Resume LPM after any command is executed
            __bis_SR_register(LPM0_bits + GIE);  // Enter LPM with interrupts
            break;

        /* Modes used for each command */
        case system_status:
            setup_system_status_test(SystemStatusRespBuf,SystemStatusBuf);
            updateTransmissionBuffer(SystemStatusRespBuf, SYSTEM_STATUS_RESP_LEN);
            app = idle;

            // GPIO_setAsOutputPin(GPIO_PORT_P1, GPIO_PIN0);  // Green LED on
            // GPIO_setOutputHighOnPin(GPIO_PORT_P1, GPIO_PIN0);

            break;
        case health_check:
            setup_health_check_test(HealthCheckRespBuf, HealthCheckBuf);
            updateTransmissionBuffer(HealthCheckRespBuf, HEALTH_CHECK_RESP_LEN);
            app = idle;
            
            // TODO : enable for testing
            //ToggleGPIO(1, 0, 0); // Port 0, Pin 1, set to low  -> Green LED off
            //ToggleGPIO(4, 6, 1); // Port 4, Pin 6, set to high -> Red LED on

            //
            // GPIO_setAsOutputPin(GPIO_PORT_P1, GPIO_PIN0);  // Green LED off
            // GPIO_setOutputLowOnPin(GPIO_PORT_P1, GPIO_PIN0);
            // GPIO_setAsOutputPin(GPIO_PORT_P4, GPIO_PIN6);  // Red LED on
            // GPIO_setOutputHighOnPin(GPIO_PORT_P4, GPIO_PIN6);
            //

            break;
        case reboot:
            // FUNCTION CALL TO REBOOT GOES HERE -> Response?

            updateTransmissionBuffer(RebootRespBuf, REBOOT_RESP_LEN);
            app = idle;
            break;
        case converter_monitor:
            // FUNCTION CALL GOES HERE fills ConverterMonitorRespBuf (len = 3)
            setup_converter_monitor_test(ConverterMonitorRespBuf);
            updateTransmissionBuffer(ConverterMonitorRespBuf, CONVERTER_MONITOR_RESP_LEN);
            app = idle;

            // GPIO_setAsOutputPin(GPIO_PORT_P4, GPIO_PIN6);  // Red and Green LEDs on
            // GPIO_setOutputHighOnPin(GPIO_PORT_P4, GPIO_PIN6);
            // GPIO_setAsOutputPin(GPIO_PORT_P1, GPIO_PIN0);
            // GPIO_setOutputHighOnPin(GPIO_PORT_P1, GPIO_PIN0);

            break;
        case telecom_acknowledge:
            setup_telecom_acknowledge_test(TelecommandAckRespBuf);
            updateTransmissionBuffer(TelecommandAckRespBuf, TELECOMMAND_ACK_RESP_LEN);
            app = idle;
            // TODO : enable for testing
            // GPIO_setAsOutputPin(GPIO_PORT_P4, GPIO_PIN6);  // Flash both LEDs
            // GPIO_toggleOutputOnPin(GPIO_PORT_P4, GPIO_PIN6);
            // GPIO_setAsOutputPin(GPIO_PORT_P1, GPIO_PIN0);
            // GPIO_toggleOutputOnPin(GPIO_PORT_P1, GPIO_PIN0);

            break;
    }
}

void updateTransmissionBuffer(uint8_t *inputBuffer, uint8_t respLength) 
{
    CopyArray(inputBuffer, TransmitBuffer, respLength);  // Copy response to TransmitBuffer
    TXByteCtr = respLength;
    SlaveMode = TX_DATA_MODE;  // Switch to transfer mode
    UCB0IE |= UCRXIE;          // Enable TX interrupt
}


//***************************Private Functions Implementations***************************************************
void I2C_Slave_ProcessCMD(uint8_t cmd)
{
    // Reset these values in order to receive a new command
    ReceiveIndex = 0;
    TransmitIndex = 0;
    RXByteCtr = 0;
    TXByteCtr = 0;

    switch (cmd)   // For each command, set the board state, set it to data recv mode, set the expected bytes counter, switch interrupts
    {
        case (SYSTEM_STATUS_ID):
            app = system_status;      // Change the board state
            RXByteCtr = SYSTEM_STATUS_CMD_LEN;
            break;
        case (HEALTH_CHECK_ID):
            app = health_check;
            SlaveMode = RX_DATA_MODE;
            RXByteCtr = HEALTH_CHECK_CMD_LEN;
            break;
        case (REBOOT_ID):
            app = reboot;
            SlaveMode = RX_DATA_MODE;
            RXByteCtr = REBOOT_CMD_LEN;
            break;
        case (CONVERTER_MONITOR_ID):
            app = converter_monitor;
            SlaveMode = RX_DATA_MODE;
            RXByteCtr = CONVERTER_MONITOR_CMD_LEN;
            break;
        case (TELECOMMAND_ACK_ID):
            app = telecom_acknowledge;
            SlaveMode = RX_DATA_MODE;
            RXByteCtr = TELECOMMAND_ACK_CMD_LEN;
            break;
        case (BINGO_BONGO):
            SlaveMode = RX_DATA_MODE;
            TXByteCtr = 10;
            RXByteCtr = 10;
            break;
        default:  // Unrecognized command
            __no_operation();
            break;
    }
}


// When all bytes are received, copy the args array
void I2C_Slave_TransactionDone(uint8_t cmd)
{
    switch (cmd)
    {
        case (SYSTEM_STATUS_ID):
            CopyArray(ReceiveBuffer, SystemStatusBuf, SYSTEM_STATUS_CMD_LEN);
            break;
        case (HEALTH_CHECK_ID):
            CopyArray(ReceiveBuffer, HealthCheckBuf, HEALTH_CHECK_CMD_LEN);
            break;
        case (REBOOT_ID):
            CopyArray(ReceiveBuffer, RebootBuf, REBOOT_CMD_LEN);
            break;
        case (CONVERTER_MONITOR_ID):
            CopyArray(ReceiveBuffer, ConverterMonitorBuf, CONVERTER_MONITOR_CMD_LEN);
            break;
        case (TELECOMMAND_ACK_ID):
            CopyArray(ReceiveBuffer, TelecommandAckBuf, TELECOMMAND_ACK_CMD_LEN);
            break;
        case (BINGO_BONGO):
            SlaveMode = TX_DATA_MODE;
            CopyArray(ReceiveBuffer, TransmitBuffer, TYPE_3_LENGTH);
        default:
            __no_operation();
            break;
    }
}

//******************************************************************************
// Test Functions **************************************************************
//******************************************************************************
void setup_system_status_test(uint8_t *SystemStatusRespBuf, uint8_t *SystemStatusBuf)
{
  // FUNCTION CALL TO RETRIEVE SYSTEM STATUS GOES HERE -> fills SystemStatusRespBuf (len=5)
  SystemStatusRespBuf[0] = 'S';
  SystemStatusRespBuf[1] = 'y';
  SystemStatusRespBuf[2] = 's';
  SystemStatusRespBuf[3] = SystemStatusBuf[0];  // Whichever byte was sent as an arg
  SystemStatusRespBuf[4] = SystemStatusBuf[1];
}

void setup_health_check_test(uint8_t *HealthCheckRespBuf, uint8_t *HealthCheckBuf) {
    // Fills HealthCheckRespBuf (len = 16)
    const char testMessage[] = "TestingHealth";
    int i;
    for (i = 0; i < 12; i++) {
        HealthCheckRespBuf[i] = testMessage[i];
    }
    HealthCheckRespBuf[13] = HealthCheckBuf[0];
    HealthCheckRespBuf[14] = HealthCheckBuf[1];
    HealthCheckRespBuf[15] = '2';
}

void setup_converter_monitor_test(uint8_t *ConverterMonitorRespBuf) {
    // Fills ConverterMonitorRespBuf (len = 3)
    ConverterMonitorRespBuf[0] = 'H';
    ConverterMonitorRespBuf[1] = 'i';
    ConverterMonitorRespBuf[2] = '4';
}

void setup_telecom_acknowledge_test(uint8_t *TelecommandAckRespBuf) {
    // Fills TelecommandAckRespBuf (len = 2)
    TelecommandAckRespBuf[0] = 'O';
    TelecommandAckRespBuf[1] = '5';
}
