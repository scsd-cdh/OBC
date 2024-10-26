/**
 *
 *  I2C driver for MSP430
 *
 *  Requirements:
 *
 *
 */

//******************************************************************************
// Example Commands ************************************************************
//******************************************************************************

#define SLAVE_ADDR  0x08

/* CMD_TYPE_X_SLAVE are example commands the master sends to the slave.
 * The slave will send example SlaveTypeX buffers in response.
 *
 * CMD_TYPE_X_MASTER are example commands the master sends to the slave.
 * The slave will initialize itself to receive MasterTypeX example buffers.
 * */

#define CMD_TYPE_0_SLAVE      0
#define CMD_TYPE_1_SLAVE      1
#define CMD_TYPE_2_SLAVE      2

#define CMD_TYPE_0_MASTER      3
#define CMD_TYPE_1_MASTER      4
#define CMD_TYPE_2_MASTER      5

#define BINGO_BONGO 0x09

#define TYPE_0_LENGTH   1
#define TYPE_1_LENGTH   2
#define TYPE_2_LENGTH   6
#define TYPE_3_LENGTH   10

#define MAX_BUFFER_SIZE     20



/**
 * @brief MasterTypeX are example buffers initialized in the master, they will be
 * sent by the master to the slave.
 * SlaveTypeX are example buffers initialized in the slave, they will be
 * sent by the slave to the master.
 * */

uint8_t MasterType2 [TYPE_2_LENGTH] = {0};
uint8_t MasterType1 [TYPE_1_LENGTH] = { 0, 0};
uint8_t MasterType0 [TYPE_0_LENGTH] = { 0};

uint8_t SlaveType2 [TYPE_2_LENGTH] = {'A', 'B', 'C', 'D', '1', '2'};
uint8_t SlaveType1 [TYPE_1_LENGTH] = {15, 16};
uint8_t SlaveType0 [TYPE_0_LENGTH] = {12};


/**
 *  @brief Initialized the software state machine according to the received cmd
 *
 *  @param cmd: The command/register address received
 * */
void I2C_Slave_ProcessCMD(uint8_t cmd);


/**
*   @brief The following section will implement the functions
*
*/
void ProcessCMD(uint8_t cmd);

void Set_CMD_Params(I2C_Mode mode, );

/**
 * @brief The transaction between the slave and master is completed. Uses cmd
 * to do post transaction operations. (Place data from ReceiveBuffer
 * to the corresponding buffer based in the last received cmd)
 *
 * @param cmd: The command/register address corresponding to the completed
 * transaction
 */
void I2C_Slave_TransactionDone(uint8_t cmd);
void CopyArray(uint8_t *source, uint8_t *dest, uint8_t count);

