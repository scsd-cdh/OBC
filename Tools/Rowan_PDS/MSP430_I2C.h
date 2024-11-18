/**
 * @file MSP430_I2C.h
 * @brief I2C driver header for MSP430. This header defines configurations, command IDs,
 * state machine variables, and functions used for I2C communication between an MSP430
 * device (slave) and a master device.
 *
 * @requirements:
 * - Compatible with MSP430FR59xx series running at 16 MHz.
 * - Configures I2C in slave mode with specific commands and response structures.
 */

#ifndef __MSP430_I2C__
#define __MSP430_I2C__

#include <msp430.h> 
#include <stdint.h>
// #include <driverlib.h>  // Optional: included for debugging with LEDs, remove after testing

//*******************************************************************************
// Initial Configuration for I2C ***********************************************
//*******************************************************************************

#define SLAVE_ADDR                  0x08  /**< I2C Slave Address for the MSP430 device */

//*******************************************************************************
// State Machine Variables *****************************************************
//*******************************************************************************

// Command IDs for various I2C commands
#define SYSTEM_STATUS_ID            0x01  /**< Command ID for requesting system status */
#define HEALTH_CHECK_ID             0x02  /**< Command ID for health check */
#define REBOOT_ID                   0x03  /**< Command ID to initiate reboot */
#define CONVERTER_MONITOR_ID        0x04  /**< Command ID for converter monitoring */
#define TELECOMMAND_ACK_ID          0x05  /**< Command ID for telecommand acknowledgement */
#define BINGO_BONGO                 0x09  /**< Example command ID for testing */

// Command argument lengths (number of bytes expected per command)
#define SYSTEM_STATUS_CMD_LEN       2     /**< Length of arguments for system status command */
#define HEALTH_CHECK_CMD_LEN        2     /**< Length of arguments for health check command */
#define REBOOT_CMD_LEN              2     /**< Length of arguments for reboot command */
#define CONVERTER_MONITOR_CMD_LEN   2     /**< Length of arguments for converter monitor command */
#define TELECOMMAND_ACK_CMD_LEN     2     /**< Length of arguments for telecommand acknowledgement */
#define TYPE_3_LENGTH               10    /**< Example length for an additional command */

// Response lengths for each command (number of bytes to be sent back to the master)
#define SYSTEM_STATUS_RESP_LEN      5     /**< Response length for system status */
#define HEALTH_CHECK_RESP_LEN       16    /**< Response length for health check */
#define REBOOT_RESP_LEN             1     /**< Response length for reboot */
#define CONVERTER_MONITOR_RESP_LEN  3     /**< Response length for converter monitor */
#define TELECOMMAND_ACK_RESP_LEN    2     /**< Response length for telecommand acknowledgement */

#define MAX_BUFFER_SIZE             20    /**< Maximum buffer size for I2C data transmission */

//*******************************************************************************
// Buffers **********************************************************************
//*******************************************************************************

// Buffers for holding command arguments received from the master
uint8_t SystemStatusBuf[SYSTEM_STATUS_CMD_LEN] = {0};  /**< Buffer for system status command arguments */
uint8_t HealthCheckBuf[HEALTH_CHECK_CMD_LEN] = {0};    /**< Buffer for health check command arguments */
uint8_t RebootBuf[REBOOT_CMD_LEN] = {0};               /**< Buffer for reboot command arguments */
uint8_t ConverterMonitorBuf[CONVERTER_MONITOR_CMD_LEN] = {0}; /**< Buffer for converter monitor command arguments */
uint8_t TelecommandAckBuf[TELECOMMAND_ACK_CMD_LEN] = {0};     /**< Buffer for telecommand acknowledgement arguments */

// Buffers for holding response data to be sent back to the master
uint8_t SystemStatusRespBuf[SYSTEM_STATUS_RESP_LEN] = {0};    /**< Buffer for system status response data */
uint8_t HealthCheckRespBuf[HEALTH_CHECK_RESP_LEN] = {0};      /**< Buffer for health check response data */
uint8_t RebootRespBuf[REBOOT_RESP_LEN] = {0};                 /**< Buffer for reboot response data */
uint8_t ConverterMonitorRespBuf[CONVERTER_MONITOR_RESP_LEN] = {0}; /**< Buffer for converter monitor response data */
uint8_t TelecommandAckRespBuf[TELECOMMAND_ACK_RESP_LEN] = {0};     /**< Buffer for telecommand acknowledgement response */

//*******************************************************************************
// General I2C State Machine ***************************************************
//*******************************************************************************

/**
 * @enum I2C_Mode
 * @brief Defines the various modes for the I2C state machine. Used to track
 *        the current mode of the I2C communication.
 */
typedef enum I2C_ModeEnum {
    IDLE_MODE,               /**< I2C is idle */
    NACK_MODE,               /**< No acknowledgment received */
    TX_REG_ADDRESS_MODE,     /**< Transmitting register address */
    RX_REG_ADDRESS_MODE,     /**< Receiving register address */
    TX_DATA_MODE,            /**< Transmitting data to master */
    RX_DATA_MODE,            /**< Receiving data from master */
    SWITCH_TO_RX_MODE,       /**< Switching to receive mode */
    SWITCH_TO_TX_MODE,       /**< Switching to transmit mode */
    TIMEOUT_MODE             /**< Communication timeout */
} I2C_Mode;

/**
 * @enum appStatus
 * @brief Defines the states for the application, representing different commands or actions
 *        that the MSP430 device will perform in response to master commands.
 */
typedef enum appStatusEnum {
    idle,                    /**< Idle state */
    system_status,           /**< System status command state */
    health_check,            /**< Health check command state */
    reboot,                  /**< Reboot command state */
    telecom_acknowledge,     /**< Telecommand acknowledge state */
    converter_monitor        /**< Converter monitor command state */
} appStatus;

static volatile appStatus app; /**< Tracks the current state of the application */

/** I2C SlaveMode - Tracks the current mode of the I2C software state machine */
I2C_Mode SlaveMode = RX_REG_ADDRESS_MODE;

/** Register address or command to use (determined by master) */
uint8_t ReceiveRegAddr = 0;

/**
 * Buffers and counters for data transmission and reception
 * - ReceiveBuffer: Buffer to hold data received from the master
 * - RXByteCtr: Counter for bytes left to receive
 * - ReceiveIndex: Index of the next byte to be received in ReceiveBuffer
 * - TransmitBuffer: Buffer to hold data to send to the master
 * - TXByteCtr: Counter for bytes left to transmit
 * - TransmitIndex: Index of the next byte to be transmitted in TransmitBuffer
 */
uint8_t ReceiveBuffer[MAX_BUFFER_SIZE] = {0};
uint8_t RXByteCtr = 0;
uint8_t ReceiveIndex = 0;
uint8_t TransmitBuffer[MAX_BUFFER_SIZE] = {0};
uint8_t TXByteCtr = 0;
uint8_t TransmitIndex = 0;

//*******************************************************************************
// Function Declarations *******************************************************
//*******************************************************************************

/**
 * @brief Initializes the program, setting up initial states and necessary configurations.
 */
void initialize();

/**
 * @brief Processes the received I2C command, updating the application state based on
 *        the received command.
 *
 * @param cmd Command or register address received from the master.
 */
void I2C_Slave_ProcessCMD(uint8_t cmd);

/**
 * @brief Completes an I2C transaction. Uses the command to handle any post-transaction
 *        actions, such as copying received data to command-specific buffers.
 *
 * @param cmd The command/register address corresponding to the completed transaction.
 */
void I2C_Slave_TransactionDone(uint8_t cmd);

/**
 * @brief Initializes the system clock to 16MHz to support I2C and other peripherals.
 */
void initClockTo16MHz();

/**
 * @brief Configures GPIO pins for I2C communication and debugging (LED indicators).
 */
void initGPIO();

/**
 * @brief Initializes the I2C module in slave mode with the specified slave address.
 */
void initI2C();

/**
 * @brief Begins the application state machine, determining the next action based
 *        on the current application state.
 */
void begin_state_machine();

/**
 * @brief Copies a specified number of bytes from a source array to a destination array.
 *
 * @param source Pointer to the source array
 * @param dest Pointer to the destination array
 * @param count Number of bytes to copy
 */
void CopyArray(uint8_t *source, uint8_t *dest, uint8_t count);

/**
 * @brief Toggles a GPIO pin for debugging or status indication.
 *
 * @param port GPIO port number
 * @param pin GPIO pin number
 * @param mode 1 to set high, 0 to set low
 */
void ToggleGPIO(uint8_t port, uint8_t pin, uint8_t mode);

/**
 * @brief Sets up the system status response buffer.
 *
 * @param SystemStatusRespBuf Buffer to hold the system status response data
 * @param SystemStatusBuf Buffer holding system status command arguments
 */
void setup_system_status_test(uint8_t *SystemStatusRespBuf, uint8_t *SystemStatusBuf);

/**
 * @brief Sets up the health check response buffer.
 *
 * @param HealthCheckRespBuf Buffer to hold the health check response data
 * @param HealthCheckBuf Buffer holding health check command arguments
 */
void setup_health_check_test(uint8_t *HealthCheckRespBuf, uint8_t *HealthCheckBuf);

/**
 * @brief Sets up the converter monitor response buffer.
 *
 * @param ConverterMonitorRespBuf Buffer to hold the converter monitor response data
 */
void setup_converter_monitor_test(uint8_t *ConverterMonitorRespBuf);

/**
 * @brief Sets up the telecommand acknowledgement response buffer.
 *
 * @param TelecommandAckRespBuf Buffer to hold the telecommand acknowledgement response data
 */
void setup_telecom_acknowledge_test(uint8_t *TelecommandAckRespBuf);

#endif /* __MSP430_I2C__ */
