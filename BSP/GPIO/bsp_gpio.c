

#include "bsp.h"

/* GPIO��PIN���� */
static const gpio_out gpio_list[GPIO_OUT_NUM] = {
	{LED1_GPIO, LED1_PIN},		/* GPIO1 */
	{LED2_GPIO, LED2_PIN},		/* GPIO2 */
	{LED3_GPIO, LED3_PIN},		/* GPIO3 */
};	
/*
*********************************************************************************************************
*	�� �� ��: gpio_init_hard
*	����˵��: ����GPIO
*	��    ��:  ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void out_gpio_init(void)
{
	GPIO_InitTypeDef gpio_init;
	uint8_t i = 0;
	/* ��GPIOʱ�� */
	allgpio_clkopen();
	/* �������еİ���GPIO���� */
	gpio_init.Mode = GPIO_MODE_OUTPUT_PP;   			/* ������� */
	gpio_init.Pull = GPIO_PULLUP;                 		/* ���� */
	gpio_init.Speed = GPIO_SPEED_FREQ_LOW;  			/* GPIO�ٶȵȼ� */
	
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
* �� �� ��: set_pin_high
* ����˵��: ��GPIO�ø�
* �� ��: ��
* �� �� ֵ: ��
*********************************************************************************************************
*/
void set_pin_high(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin)
{
	GPIOx->BSRR = (uint32_t)GPIO_Pin << 16U;
}
/*
*********************************************************************************************************
* �� �� ��: set_pin_low
* ����˵��: ��GPIO�õ�
* �� ��: ��
* �� �� ֵ: ��
*********************************************************************************************************
*/
void set_pin_low(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin)
{
	GPIOx->BSRR = GPIO_Pin;
}
/*
*********************************************************************************************************
* �� �� ��: toggle_pin
* ����˵��: ��ת��ƽ
* �� ��: ��
* �� �� ֵ: ��
*********************************************************************************************************
*/
void toggle_pin(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin)
{
	HAL_GPIO_TogglePin(GPIOx,GPIO_Pin);
}

