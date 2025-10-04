#ifdef __ROWAN__ 

#ifndef APP_APP_H_
#define APP_APP_H_

#include <stdint.h>
#include <msp430.h>
#include <driverlib.h>  // Included for test LEDs, REMOVE AFTER USE

// The state of the board
typedef enum appStatusEnum
{
    idle,
    system_status,
    health_check,
    reboot,
    telecom_acknowledge,
    converter_monitor,
} appStatus;


// Commands and their IDs
#define SYSTEM_STATUS           0x01
#define HEALTH_CHECK            0x02
#define REBOOT                  0x03
#define CONVERTER_MONITOR       0x04
#define TELECOMMAND_ACK         0x05

#define BINGO_BONGO 0x09  // this can stay for now

// The lengths of a given command's arguments
#define SYSTEM_STATUS_LEN       2  // All set to 2 for now
#define HEALTH_CHECK_LEN        2
#define REBOOT_LEN              2
#define CONVERTER_MONITOR_LEN   2
#define TELECOMMAND_ACK_LEN     2

// The lengths of data to be returned for the given command
#define SYSTEM_STATUS_RESP_LEN      5
#define HEALTH_CHECK_RESP_LEN       16
#define REBOOT_RESP_LEN             1
#define CONVERTER_MONITOR_RESP_LEN  3
#define TELECOMMAND_ACK_RESP_LEN    2

#define MAX_BUFFER_SIZE     20  // Can be changed later


void initialize();
void initClockTo16MHz();
void initGPIO();
void initI2C();


/* Initialized the software state machine according to the received cmd
 *
 * cmd: The command/register address received
 * */
void I2C_Slave_ProcessCMD(uint8_t cmd);

/* The transaction between the slave and master is completed. Uses cmd
 * to do post transaction operations. (Place data from ReceiveBuffer
 * to the corresponding buffer based in the last received cmd)
 *
 * cmd: The command/register address corresponding to the completed
 * transaction
 */
void I2C_Slave_TransactionDone(uint8_t cmd);
void CopyArray(uint8_t *source, uint8_t *dest, uint8_t count);
void updateTransmissionBuffer(uint8_t *inputBuffer, uint8_t respLength);
void ToggleGPIO(uint8_t port, uint8_t pin, uint8_t mode);


void run();


#endif /* APP_APP_H_ */
