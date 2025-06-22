/* Libraries */
#include <atmel_start.h>

// HAL
#include "hal_gpio.h" // GPIO
#include "hal_delay.h" // Delay

// FreeRTOS
#include <FreeRTOS.h>
#include <task.h>
#include <semphr.h>
#include <hal_rtos.h>

/* Definitions */
#define CONTROL_STACK_SIZE ((configMINIMAL_STACK_SIZE * 4) / sizeof(portSTACK_TYPE))
#define CONTROL_STACK_PRIORITY (tskIDLE_PRIORITY + 2)

#define BLINK_STACK_SIZE ((configMINIMAL_STACK_SIZE * 3) / sizeof(portSTACK_TYPE))
#define BLINK_STACK_PRIORITY (tskIDLE_PRIORITY + 1)

/* Globals */
// Statics
static TaskHandle_t      xControlTask, xBlinkTask;
static SemaphoreHandle_t control_mutex,blink_mutex;

// Variables
uint8_t blinkCount = 0;

/* Tasks */
void task_control(void *pvParameter);
void task_blink(void *pvParameter);

/* Helper Function */
void blink();

/* Implementation */
int main(void)
{
	// 1. Initializes MCU, drivers and middleware
	atmel_start_init();
	
	// 2. Create and Check Mutex
	control_mutex = xSemaphoreCreateMutex();
	blink_mutex = xSemaphoreCreateMutex();
	if(blink_mutex == NULL){
		while(1){
			gpio_set_pin_level(LED0,false);
		}
	}
	
	// 3. Create Control & Blink Task
	if(xTaskCreate(task_control, "Control", CONTROL_STACK_SIZE,NULL,CONTROL_STACK_PRIORITY,xControlTask) != pdPASS){
		while (1) {
			gpio_set_pin_level(LED0,false);
		}
	}
	
	if(xTaskCreate(task_blink, "Blink", BLINK_STACK_SIZE,NULL,BLINK_STACK_PRIORITY,xBlinkTask) != pdPASS){
		while (1) {
			gpio_set_pin_level(LED0,false);
		}
	}

	vTaskStartScheduler();

	return 0;
}

void task_control(void *pvParameter){
	(void)pvParameter;
	
	while(1){
		if(xSemaphoreTake(blink_mutex, ~0)){
			blinkCount = (blinkCount<5)? blinkCount +1 : 1;
			xSemaphoreGive(blink_mutex);
		}
		os_sleep(1000);
	}
}

void task_blink(void *pvParameter){
	(void)pvParameter;
	
	while(1){
		if(xSemaphoreTake(blink_mutex, ~0)){
			int i;
			for(i=0;i<blinkCount;i++){
				blink();
			}
			xSemaphoreGive(blink_mutex);
		}
		os_sleep(1000);
	}
}


void blink(){
	gpio_set_pin_level(LED0,false);
	delay_ms(250); // Delay for 250ms
	gpio_set_pin_level(LED0,true);
	delay_ms(250); // Delay for 250ms
}
