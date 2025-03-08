#include <atmel_start.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Globals */
// FreeRTOS
static volatile UBaseType_t priorities[3];
static xTaskHandle computation_tasks_id[3]; // Computations
static SemaphoreHandle_t mutex; // Mutex

// UART
static struct io_descriptor *uartIO;

// Variables
static uint8_t instance = 0;

/* Prototypes */
// Tasks
void controlTask(void *pvParameters);
void computationTask1(void *pvParameters);
void computationTask2(void *pvParameters);
void computationTask3(void *pvParameters);

// Helpers
uint8_t getRandomNumber(uint8_t min, uint8_t max);
void USART_Init(void);
void USART_Print(const char *str);

int main(void)
{
	/* Initializes MCU, drivers and middleware */
	atmel_start_init();

	/* Call the Controller */
	controlTask(NULL);
}

void controlTask(void *pvParameters){
	// Initialize Mutex
	mutex = xSemaphoreCreateMutex();
	USART_Init();
	
	// Create Computation Tasks during Runtime
	if(mutex != NULL){
		// Setup Priority
		priorities[0] = 1;
		priorities[1] = 4;
		priorities[2] = 3;
		
		xTaskCreate(computationTask1,"Computation Task 1",configMINIMAL_STACK_SIZE, NULL,priorities[0],computation_tasks_id[0]);
		xTaskCreate(computationTask2,"Computation Task 2",configMINIMAL_STACK_SIZE, NULL,priorities[1],computation_tasks_id[1]);
		xTaskCreate(computationTask3,"Computation Task 3",configMINIMAL_STACK_SIZE, NULL,priorities[2],computation_tasks_id[2]);
		vTaskStartScheduler();
	}
	
}

void computationTask1(void *pvParameters){
	while(true){
		if(xSemaphoreTake(mutex, portMAX_DELAY) == pdTRUE){
			instance++;
			
			// Write Random number
			char buffer[200];
			sprintf(
			buffer,
			"Computation Task 1, Instance %u, Priority: %u , Number : %u\n",
			instance,
			(uint8_t)priorities[0],
			10
			);
			USART_Print(buffer);
			xSemaphoreGive(mutex);
		}
		vTaskDelay(pdMS_TO_TICKS(500));
	}
	vTaskDelete(computation_tasks_id[0]);
}

void computationTask2(void *pvParameters){
	while(true){
		if(xSemaphoreTake(mutex, portMAX_DELAY) == pdTRUE){
			instance++;
			
			// Write Random number
			char buffer[200];
			sprintf(
			buffer,
			"Computation Task 2, Instance %u, Priority: %u , Number : %u\n",
			instance,
			(uint8_t)priorities[1],
			75
			);
			USART_Print(buffer);
			xSemaphoreGive(mutex);
		}
		vTaskDelay(pdMS_TO_TICKS(500));
	}
	vTaskDelete(computation_tasks_id[1]);
}

void computationTask3(void *pvParameters){
	while(true){
		if(xSemaphoreTake(mutex, portMAX_DELAY) == pdTRUE){
			instance++;
			
			// Write Random number
			char buffer[200];
			sprintf(
			buffer,
			"Computation Task 3, Instance %u, Priority: %u , Number : %u\n",
			instance,
			(uint8_t)priorities[2],
			200
			);
			USART_Print(buffer);
			xSemaphoreGive(mutex);
		}
		vTaskDelay(pdMS_TO_TICKS(500));
	}
	vTaskDelete(computation_tasks_id[2]);
}

void USART_Init(void){
	usart_sync_get_io_descriptor(&USART_0, &uartIO);
	usart_sync_enable(&USART_0);
}

void USART_Print(const char *str){
	io_write(uartIO, (uint8_t *)str, strlen(str));
}
