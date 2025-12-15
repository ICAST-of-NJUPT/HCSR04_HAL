#include "hcsr04.h"

static TIM_HandleTypeDef *hcsr04_tim = NULL; // htim1 /2 ...
static GPIO_TypeDef *hcsr04_gpio_port = NULL; // GPIOA /B /C ...
static uint16_t hcsr04_gpio_pin = 0; // GPIO_PIN_0 /1 /2 ...

static int isch1;

static uint32_t hcsr04_result = 0;
static int hcsr04_data_updated = 0;

void hcsr04_init(TIM_HandleTypeDef *htim, GPIO_TypeDef *gpio_port, uint16_t gpio_pin, int is_ch1)
{
	hcsr04_tim = htim;
	hcsr04_gpio_port = gpio_port;
	hcsr04_gpio_pin = gpio_pin;
	isch1 = is_ch1;
	HAL_TIM_IC_Start_IT(htim, isch1 ? TIM_CHANNEL_2 : TIM_CHANNEL_1);
}

void hcsr04_triger()
{
	HAL_GPIO_WritePin(hcsr04_gpio_port, hcsr04_gpio_pin, GPIO_PIN_SET);
	hcsr04_delay_us(10);
	HAL_GPIO_WritePin(hcsr04_gpio_port, hcsr04_gpio_pin, GPIO_PIN_RESET);
}

void hcsr04_ic_handler(TIM_HandleTypeDef *htim)
{
	if (htim->Instance == hcsr04_tim->Instance) {
		if (htim->Channel == (isch1 ? HAL_TIM_ACTIVE_CHANNEL_2 : HAL_TIM_ACTIVE_CHANNEL_1)) {
			hcsr04_result = __HAL_TIM_GetCompare(htim, isch1 ? TIM_CHANNEL_2 : TIM_CHANNEL_1);
			hcsr04_data_updated = 1;
		}
	}
}

uint32_t hcsr04_read()
{
	hcsr04_data_updated = 0;
	return hcsr04_result;
}

int hcsr04_updated()
{
	return hcsr04_data_updated;
}

/*
	Use HAL_MAX_DELAY for a forever wait
	If timeout, return 0
*/
uint32_t hcsr04_wait_for_read(uint32_t max_wait_ms)
{
	extern volatile uint32_t uwTick;
	uint32_t start = uwTick;
	uint32_t current;
	while (hcsr04_data_updated == 0) {
		current = uwTick;
		if (current - start > max_wait_ms) {
			return 0;
		}
	}
	return hcsr04_read();
}

// us should be littler than 2000
void hcsr04_delay_us(uint32_t us)
{
	if (us > 1000) {
		hcsr04_delay_us(1000);
		hcsr04_delay_us(us - 1000);
	}
	else {
		uint32_t start = SysTick->VAL;
		uint32_t wait = us * (SystemCoreClock / 1000000);
		uint32_t target = (start - wait);
		if (target > start) {
				while (SysTick->VAL < start) ;
		}
		while (SysTick->VAL > target) ;
	}
}

float hcsr04_get_cm(uint32_t timFreq)
{
	return hcsr04_result * ( 34262.0 / 2 ) / timFreq; // Speed of voice at 20 *C
}
