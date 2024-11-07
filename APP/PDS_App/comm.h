
//*******************************************************************************
// Device Communication Defines *************************************************
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
