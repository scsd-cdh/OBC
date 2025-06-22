#include <atmel_start.h>
#include <stdbool.h>

#include "hal_delay.h"
#include "atmel_start_pins.h"

#include "FreeRTOS.h"
#include "timers.h"
#include "event_groups.h"

// Setup Event Bits
#define LED_ON_BIT (EventBits_t)(1<<0) // LED ON
#define LED_OFF_BIT (EventBits_t)(1<<1) // LED OFF

/* Setup Handles */
xTaskHandle control_task_id;
xTaskHandle turn_on_led_id, turn_off_led_id;
EventGroupHandle_t eventGroup; // Event Group Handle

// Debug variable
bool led_state = true;

/* Function Prototypes */
void controlTask(void *pvParameter);
void turnOnLED(void *pvParameter);
void turnOffLED(void *pvParameter);

int main(void)
{
	/* Initializes MCU, drivers and middleware */
	atmel_start_init();

	/* Replace with your application code */
#if 0 // TODO - enable if needed
	while (1) {
		gpio_toggle_pin_level(LED0);
		delay_ms(100);
	}
#else 
	/* Replace with your application code */
	controlTask(NULL);
#endif
}

void controlTask(void *pvParameter){
	// Initialize Event Group
	eventGroup = xEventGroupCreate();  // Create event group
	if (eventGroup == NULL) {
		// Handle error if event group creation fails
		return;
	}
	
	// Initialize the event bits
	xEventGroupSetBits(eventGroup, LED_OFF_BIT);
	
	// Initialize All tasks
	if (xTaskCreate(turnOnLED, "Turn On LED", configMINIMAL_STACK_SIZE, NULL, 2, &turn_on_led_id) != pdPASS) {
		// Handle task creation failure
	}
	if (xTaskCreate(turnOffLED, "Turn Off LED", configMINIMAL_STACK_SIZE, NULL, 1, &turn_off_led_id) != pdPASS) {
		// Handle task creation failure
	}
	
	// Start the scheduler
	vTaskStartScheduler();
}

void turnOnLED(void *pvParameter){
	while(true){
		xEventGroupWaitBits(eventGroup, LED_OFF_BIT, pdFALSE, pdTRUE, portMAX_DELAY);
		gpio_set_pin_level(LED0, false);
		vTaskDelay(pdMS_TO_TICKS(1000)); // Wait 1s
		xEventGroupClearBits(eventGroup, LED_OFF_BIT);
		xEventGroupSetBits(eventGroup, LED_ON_BIT);
	}
}

void turnOffLED(void *pvParameter){
	while(true){
		xEventGroupWaitBits(eventGroup, LED_ON_BIT, pdFALSE, pdTRUE, portMAX_DELAY);
		gpio_set_pin_level(LED0, true);
		vTaskDelay(pdMS_TO_TICKS(1000)); // Wait 1s
		xEventGroupClearBits(eventGroup, LED_ON_BIT);
		xEventGroupSetBits(eventGroup, LED_OFF_BIT);
	}
}
