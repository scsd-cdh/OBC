/* Headers */
// Libraries
#include <atmel_start.h>
#include "driver_init.h"
#include "i2c.h"
#include "usart.h"

/* FreeRTOS */
#include "FreeRTOS.h" 
#include "task.h"
#include "semphr.h"

/* Definitions */
// Stack Size
#define TASK_STACK_SIZE (512/sizeof(portSTACK_TYPE))

// Task Priority
#define TASK_READ_CH0_PRIORITY (tskIDLE_PRIORITY + 1)
#define TASK_READ_CH1_PRIORITY (tskIDLE_PRIORITY + 1)

// Commands
#define READ_CHANNEL_0  (uint8_t)0x02
#define READ_CHANNEL_1  (uint8_t)0x03

// Structures
typedef struct cdh_packet_t{
  uint8_t slave_address;
  uint8_t send_slave_command; // Command Received from Master
  uint16_t received_slave_data[2]; // Data Received from slave
}cdh_packet_t;


/* Globals */
// RTOS
volatile char* failedTask;
static SemaphoreHandle_t read_mutex, print_mutex; // read_mutex
static TaskHandle_t read_ch0_task_handle,read_ch1_task_handle;

// Main Variable
cdh_packet_t cdh;
char message[100] = "\r\n";
volatile char* failedTask;

/* Tasks */
void read_LDR_channel(void *pvParameter); // Thread to read channels

/* Main*/
int main(void)
{
    // Initialize all Peripheral 
	atmel_start_init();
    
    // Initialize both mutexes
    read_mutex = xSemaphoreCreateMutex();
    print_mutex = xSemaphoreCreateMutex();
    if(read_mutex == NULL || print_mutex == NULL){
        gpio_set_pin_level(LED0,false);
        return -1;
    }
    
    // Configure Slave Board
    cdh.slave_address = COMMAND_ADDRESS;
    cdh.received_slave_data[0] = 0x0000;
    cdh.received_slave_data[1] = 0x0000;
    
    // Setup Slave Boards
    
    // Setup RTOS Tasks
    if(xTaskCreate(read_LDR_channel,
            "Read Channel 0",
            TASK_STACK_SIZE,
            (void*)0,
            TASK_READ_CH0_PRIORITY,
            read_ch0_task_handle) != pdPASS)
    {
       snprintf(message,100,"Task Read Channel 0 failed to be created\r\n");
       usart_write(serial_io,message);   
    }
    
    if(xTaskCreate(read_LDR_channel,
            "Read Channel 1",
            TASK_STACK_SIZE,
            (void*)1,
            TASK_READ_CH0_PRIORITY,
            read_ch0_task_handle) != pdPASS)
    {
       snprintf(message,100,"Task Read Channel 1 failed to be created\r\n");
       usart_write(serial_io,message);   
    }
    
    // STart Scheduler
    vTaskStartScheduler();
    return 0;      
}

void read_LDR_channel(void *pvParameter){
    uint8_t channel = (uint8_t)(uintptr_t)pvParameter;
    while(1){
        // Send Command
        if(xSemaphoreTake(read_mutex, ~0)){ 
			cdh.send_slave_command = (channel == 0)?
                READ_CHANNEL_0:READ_CHANNEL_1;
            i2c_write(
                    command_io,
                    &command,
                    cdh.slave_address,
                    0x0000|cdh.send_slave_command
                    );
			xSemaphoreGive(read_mutex);
		}	
        vTaskDelay(pdMS_TO_TICKS(10));
        
        // Read Result
        uint8_t data[4] = {0xFF,0xFF,0xFF,0xFF};
        if(xSemaphoreTake(read_mutex, ~0)){ 
			i2c_read(&command,cdh.slave_address, data);
            cdh.received_slave_data[0] = (data[0] << 8) | data[1];
            cdh.received_slave_data[1] = (data[2] << 8) | data[3];
			xSemaphoreGive(read_mutex);
        }
        vTaskDelay(pdMS_TO_TICKS(100)); 
        
        // Print Result
        if(xSemaphoreTake(print_mutex, ~0)){ 
			// Print All Data
            snprintf(
                message,
                100,
                "CH0: %d, CH1: %d\r\n",
                cdh.received_slave_data[0],
                cdh.received_slave_data[1]
            );
            usart_write(serial_io,message);
			xSemaphoreGive(print_mutex);
        }
        
        if(xSemaphoreTake(print_mutex, ~0)){ 
			// Print All Data
            snprintf(message,100, "--------------------------------------\r\n");
            usart_write(serial_io,message);
			xSemaphoreGive(print_mutex);
        }
        vTaskDelay(pdMS_TO_TICKS(100)); 
    }   
}