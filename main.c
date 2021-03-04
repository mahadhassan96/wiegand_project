#include <stdio.h>
#include <stdint.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "freertos/semphr.h"

uint32_t wgnd;
int cntr;
SemaphoreHandle_t xSemaphore = NULL;
SemaphoreHandle_t xSemaphore0 = NULL;

static void IRAM_ATTR gpio_isr_handler1(void* arg){
	xSemaphoreGiveFromISR(xSemaphore, NULL);
}

static void IRAM_ATTR gpio_isr_handler0(void* arg){
	xSemaphoreGiveFromISR(xSemaphore0, NULL);
}

// task that will react to button clicks
void button_task(void* arg) {
	while(1){
		if(xSemaphoreTake(xSemaphore,portMAX_DELAY) == pdTRUE) {
			if(cntr == 0){	wgnd |= 1; }
			else if(wgnd == 0){ wgnd = wgnd | (1 << cntr); }
			else{ wgnd = wgnd | (wgnd << cntr); }
			cntr++;
		}
		if(cntr > 25){
			printf("%x\n", wgnd);
			cntr = 0;
		}
	}
}

void button_task0(void* arg) {
	while(1){
		if(xSemaphoreTake(xSemaphore0,portMAX_DELAY) == pdTRUE) {
			wgnd = wgnd << cntr;
			cntr++;
		}
		if(cntr > 25){
			printf("%x\n", wgnd);
			cntr = 0;
		}
	}
}

void app_main(void){
	
	//GPIO_init();
	wgnd = 0;
	cntr = 0;
	xSemaphore = xSemaphoreCreateBinary();
	xSemaphore0 = xSemaphoreCreateBinary();
	
	gpio_set_direction(34,GPIO_MODE_DEF_INPUT);
	gpio_set_direction(36,GPIO_MODE_DEF_INPUT);
	
	//change gpio intrrupt type for one pin
 	gpio_set_intr_type(34, GPIO_PIN_INTR_NEGEDGE);
	gpio_set_intr_type(36, GPIO_PIN_INTR_NEGEDGE);
	
	xTaskCreate(button_task, "button_task", 2048, NULL, 10, NULL);
	xTaskCreate(button_task0, "button_task0", 2048, NULL, 10, NULL);
	
    //install gpio isr service
    gpio_install_isr_service(ESP_INTR_FLAG_IRAM);
    //hook isr handler for specific gpio pin
    gpio_isr_handler_add(34, gpio_isr_handler1, NULL);
	gpio_isr_handler_add(36, gpio_isr_handler0, (void*) 36);

}
