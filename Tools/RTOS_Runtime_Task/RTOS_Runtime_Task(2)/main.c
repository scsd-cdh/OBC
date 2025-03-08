#include <atmel_start.h>
#include "hal_gpio.h"
#include "hal_delay.h"

// defintion
#define LED0 GPIO(GPIO_PORTA, 23)

/* Globals */
// Variables
char message[128];
struct io_descriptor *io;

// FreeRTOS
xTaskHandle computation_tasks1_id,computation_tasks2_id,computation_tasks3_id; // Computations
SemaphoreHandle_t mutex; // Mutex


/* Protoypes */
// Tasks
void controlTask(void *pvParameters);
void initializeParts(void);
void computationTask1(void *pvParameters);
void computationTask2(void *pvParameters);
void computationTask3(void *pvParameters);

static void sendMessage(char*message){
	io_write(io,message,128);
}


int main(void)
{
	/* Initializes MCU, drivers and middleware */
	atmel_start_init();
	initializeParts();

	/* Replace with your application code */
	controlTask(NULL);
}

void initializeParts(void) {
	
	usart_sync_get_io_descriptor(&USART_3, &io);
	usart_sync_enable(&USART_3);
	
	gpio_set_pin_direction(LED0,GPIO_DIRECTION_OUT);
	gpio_set_pin_pull_mode(LED0,GPIO_PULL_UP);
}

void controlTask(void *pvParameters){
	// Initialize Mutex
	mutex = xSemaphoreCreateMutex();
	sendMessage("Enter Control Task");
	
	if(mutex != NULL){
		sendMessage("Creates Computation Task 1\n");
		xTaskCreate(computationTask1,"Computation Task 1",configMINIMAL_STACK_SIZE, NULL,1,&computation_tasks1_id);
		
		sendMessage("Creates Computation Task 2\n");
		xTaskCreate(computationTask2,"Computation Task 2",configMINIMAL_STACK_SIZE, NULL,0,&computation_tasks2_id);
		
		sendMessage("StartTask Scheduler");
		vTaskStartScheduler();
	}
}

void computationTask1(void *pvParameters){
	while(true){
		if(xSemaphoreTake(mutex, portMAX_DELAY) == pdTRUE){
			sendMessage("Execute Computation Task 1\n");
			sendMessage("Turn LED ON\n");
			gpio_set_pin_level(LED0, false);
			xSemaphoreGive(mutex);
		}
		vTaskDelay(pdMS_TO_TICKS(1000));
	}
}


void computationTask2(void *pvParameters){
	while(true){
		if(xSemaphoreTake(mutex, portMAX_DELAY) == pdTRUE){
			sendMessage("Execute Computation Task 2\n");
			if(computation_tasks3_id == NULL){
				sendMessage("Creates Computation Task 3\n");
				xTaskCreate(computationTask3,"Computation Task 3",configMINIMAL_STACK_SIZE, NULL,2,&computation_tasks3_id);
			}
			xSemaphoreGive(mutex);
		}
		vTaskDelay(pdMS_TO_TICKS(1000));
	}
}


void computationTask3(void *pvParameters){
	while(true){
		if(xSemaphoreTake(mutex, portMAX_DELAY) == pdTRUE){
			sendMessage("Execute Computation Task 3\n");
			sendMessage("Turn LED OFF\n");
			gpio_set_pin_level(LED0, true);
			xSemaphoreGive(mutex);
		}
		vTaskDelay(pdMS_TO_TICKS(1000));
	}
}

