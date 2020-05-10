

#include "bsp.h"

/* GPIO和PIN定义 */
static const gpio_out gpio_list[GPIO_OUT_NUM] = {
	{LED1_GPIO, LED1_PIN},		/* GPIO1 */
	{LED2_GPIO, LED2_PIN},		/* GPIO2 */
	{LED3_GPIO, LED3_PIN},		/* GPIO3 */
};	
/*
*********************************************************************************************************
*	函 数 名: gpio_init_hard
*	功能说明: 配置GPIO
*	形    参:  无
*	返 回 值: 无
*********************************************************************************************************
*/
void out_gpio_init(void)
{
	GPIO_InitTypeDef gpio_init;
	uint8_t i = 0;
	/* 打开GPIO时钟 */
	allgpio_clkopen();
	/* 配置所有的按键GPIO输入 */
	gpio_init.Mode = GPIO_MODE_OUTPUT_PP;   			/* 推挽输出 */
	gpio_init.Pull = GPIO_PULLUP;                 		/* 上拉 */
	gpio_init.Speed = GPIO_SPEED_FREQ_LOW;  			/* GPIO速度等级 */
	
	for (i = 0; i < GPIO_OUT_NUM; i++)
	{
		gpio_init.Pin = gpio_list[i].pin;
		HAL_GPIO_Init(gpio_list[i].gpio, &gpio_init);	
	}

	LED1_GPIO->BSRR |= LED1_PIN; 
	LED2_GPIO->BSRR |= LED2_PIN << 16;  
	LED2_GPIO->BSRR |= LED3_PIN; 
	
}
/*
*********************************************************************************************************
* 函 数 名: set_pin_high
* 功能说明: 将GPIO置高
* 形 参: 无
* 返 回 值: 无
*********************************************************************************************************
*/
void set_pin_high(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin)
{
	GPIOx->BSRR = (uint32_t)GPIO_Pin << 16U;
}
/*
*********************************************************************************************************
* 函 数 名: set_pin_low
* 功能说明: 将GPIO置低
* 形 参: 无
* 返 回 值: 无
*********************************************************************************************************
*/
void set_pin_low(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin)
{
	GPIOx->BSRR = GPIO_Pin;
}
/*
*********************************************************************************************************
* 函 数 名: toggle_pin
* 功能说明: 翻转电平
* 形 参: 无
* 返 回 值: 无
*********************************************************************************************************
*/
void toggle_pin(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin)
{
	HAL_GPIO_TogglePin(GPIOx,GPIO_Pin);
}

