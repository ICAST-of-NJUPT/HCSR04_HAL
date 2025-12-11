#ifndef HCSR04_H
#define HCSR04_H

#include "main.h"

void hcsr04_init(TIM_HandleTypeDef *htim, GPIO_TypeDef *gpio_port, uint16_t gpio_pin);

/* Triger a measure */
void hcsr04_triger();

uint32_t hcsr04_read();

/* Return a variable that will be set to 1 when a measure is finished and set to 0 when hcsr04_read() or hcsr04_data_updated() is called */
int hcsr04_updated();

/*	Use HAL_MAX_DELAY for a forever wait. If timeout, return 0 */
uint32_t hcsr04_wait_for_read(uint32_t max_wait_ms);

/* Function below should be placed into "HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim)" like this:

void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim) {

	hcsr04_ic_handler(htim);

	// Any codes here ---
	// if (...) { ... }
	// switch (...) { ... }
	// ...
	// Any codes here ---

	// hcsr04_ic_handler(htim); // can also be here

	// Any codes here ...

	// hcsr04_ic_handler(htim); // can also be here
}

*/
void hcsr04_ic_handler(TIM_HandleTypeDef *htim);

void hcsr04_delay_us(uint32_t us);

float hcsr04_get_cm(uint32_t timFreq);


#endif