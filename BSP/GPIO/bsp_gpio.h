

#ifndef __GPIO_H
#define __GPIO_H

#include "stm32f4xx_hal.h"

#define GPIO_OUT_NUM 3

#define LED1_GPIO	GPIOH
#define LED1_PIN	GPIO_PIN_10
#define LED2_GPIO	GPIOH
#define LED2_PIN	GPIO_PIN_11
#define LED3_GPIO	GPIOH
#define LED3_PIN	GPIO_PIN_12
/* ʹ��GPIOʱ�� */
#define allgpio_clkopen() {	\
		__HAL_RCC_GPIOH_CLK_ENABLE();	\
	};

/* ���ζ���GPIO */
typedef struct
{
	GPIO_TypeDef* gpio;
	uint16_t pin;
}gpio_out;


/* ���ⲿ���õĺ������� */
void out_gpio_init(void);
void set_pin_high(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin);
void set_pin_low(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin);
void toggle_pin(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin);
#endif

