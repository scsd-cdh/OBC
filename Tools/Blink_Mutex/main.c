/* Libraries */
#include <atmel_start.h>

// HAL 
#include "hal_gpio.h" // GPIO
#include "hal_delay.h" // Delay

// RTOS
#include "FreeRTOS.h" 
#include "task.h"
#include "semphr.h"

/* Definitions */
// Stack Size
#define TASK_STACK_SIZE (128/sizeof(portSTACK_TYPE))

// Task Priority
#define TASK_CONTROLLER_PRIORITY (tskIDLE_PRIORITY + 2)
#define TASK_BLINK_PRIORITY (tskIDLE_PRIORITY + 1)

/* GLobals Definitions */
volatile char* failedTask;
volatile uint8_t blinkCount = 1;
static SemaphoreHandle_t Mutex; // Mutex
static TaskHandle_t ControlTaskHandle,BlinkTaskHandle;

/* Task Prototypes */
void control_task(void *pvParameter);
void blink_task(void *pvParameter);

// Helper Functions
void createTask(TaskFunction_t task,const char *const name,UBaseType_t priority,TaskHandle_t *const taskHandle);
void blink();

/* Implementation */
int main(void)
{
	// Initializes MCU, drivers and middleware
	atmel_start_init();

	// Create Mutex
	Mutex = xSemaphoreCreateMutex();
	
	// Confirm Mutexes are ready
	if(Mutex == NULL){
		gpio_set_pin_level(LED0,false);
		return -1;
	}
	
	// Create Tasks
	createTask(control_task,"Control",TASK_CONTROLLER_PRIORITY,&ControlTaskHandle);
	createTask(blink_task,"Blink",TASK_BLINK_PRIORITY,&BlinkTaskHandle);
	
	// Start Scheduler
	vTaskStartScheduler();
	
	return 0;
}

// Tasks
void control_task(void *pvParameter){
	while(1){
		// 1. Read Global Variable
		uint8_t count = blinkCount;
		
		// 2. Update Global Variable
		if(xSemaphoreTake(Mutex, ~0)){ 
			blinkCount = count+1; // Update Global Variable
			xSemaphoreGive(Mutex);
		}	
		// 4. Wait 1 second
		vTaskDelay(pdMS_TO_TICKS(1000)); 
	}
	
}

void blink_task(void *pvParameter){
	while(1){
		// 1. Safely Read Global Variable
		uint8_t count = blinkCount;
		
		// 2. Wait for Blink Mutex
		if(xSemaphoreTake(Mutex, ~0)){	
			// 3. Perform Blink Operation
			int i;
			for(i=0;i<count;i++){
				blink();
			}
			xSemaphoreGive(Mutex);
		}
		
		// 4. Wait 2s
		vTaskDelay(pdMS_TO_TICKS(1000)); 
	}
}

// Helper
void createTask(TaskFunction_t task,const char *const name,UBaseType_t priority,TaskHandle_t *const taskHandle){
	if(xTaskCreate(task,name,TASK_STACK_SIZE,NULL, priority,taskHandle
	) != pdPASS){
		while(1){
			failedTask = name;
			gpio_set_pin_level(LED0,true);
		}
	}
}

void blink(){
    gpio_set_pin_level(LED0,false);
    delay_ms(250); // Delay for 250ms
    gpio_set_pin_level(LED0,true);
    delay_ms(250); // Delay for 250ms
}