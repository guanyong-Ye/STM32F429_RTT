
#include "bsp.h"

/*
*********************************************************************************************************
*	�� �� ��: bsp_rcc_gpio_enable
*	����˵��: ʹ��GPIOʱ��
*	��    ��: GPIOx GPIOA - GPIOK
*	�� �� ֵ: ��
*********************************************************************************************************
*/
static void bsp_rcc_gpio_enable(GPIO_TypeDef* gpiox)
{
	if (gpiox == GPIOA) 
		__HAL_RCC_GPIOA_CLK_ENABLE();
	else if (gpiox == GPIOB)  
		__HAL_RCC_GPIOB_CLK_ENABLE();
	else if (gpiox == GPIOC) 
		__HAL_RCC_GPIOC_CLK_ENABLE();
	else if (gpiox == GPIOD) 
		__HAL_RCC_GPIOD_CLK_ENABLE();
	else if (gpiox == GPIOE) 
		__HAL_RCC_GPIOE_CLK_ENABLE();
	else if (gpiox == GPIOF) 
		__HAL_RCC_GPIOF_CLK_ENABLE();
	else if (gpiox == GPIOG)
		__HAL_RCC_GPIOG_CLK_ENABLE();
	else if (gpiox == GPIOH) 
		__HAL_RCC_GPIOH_CLK_ENABLE();
	else if (gpiox == GPIOI) 
		__HAL_RCC_GPIOI_CLK_ENABLE();
	else if (gpiox == GPIOJ) 
		__HAL_RCC_GPIOJ_CLK_ENABLE();
	else if (gpiox == GPIOK) 
		__HAL_RCC_GPIOK_CLK_ENABLE();
	else
		_Error_Handler(__FILE__, __LINE__);
}
/*
*********************************************************************************************************
*	�� �� ��: bsp_RCC_TIM_Enable
*	����˵��: ʹ��TIM RCC ʱ��
*	��    ��: TIMx TIM1 - TIM14
*	�� �� ֵ: ��
*********************************************************************************************************
*/
static void bsp_rcc_tim_enable(TIM_TypeDef* timx)
{
	if (timx == TIM1) 
		__HAL_RCC_TIM1_CLK_ENABLE();
	else if (timx == TIM2) 
		__HAL_RCC_TIM2_CLK_ENABLE();
	else if (timx == TIM3) 
		__HAL_RCC_TIM3_CLK_ENABLE();
	else if (timx == TIM4) 
		__HAL_RCC_TIM4_CLK_ENABLE();
	else if (timx == TIM5) 
		__HAL_RCC_TIM5_CLK_ENABLE();
	else if (timx == TIM6) 
		__HAL_RCC_TIM6_CLK_ENABLE();
	else if (timx == TIM7) 
		__HAL_RCC_TIM7_CLK_ENABLE();
	else if (timx == TIM8) 
		__HAL_RCC_TIM8_CLK_ENABLE();
	else if (timx == TIM9) 
		__HAL_RCC_TIM9_CLK_ENABLE();
	else if (timx == TIM10) 
		__HAL_RCC_TIM10_CLK_ENABLE();
	else if (timx == TIM11) 
		__HAL_RCC_TIM11_CLK_ENABLE();
	else if (timx == TIM12) 
		__HAL_RCC_TIM12_CLK_ENABLE();
	else if (timx == TIM13) 
		__HAL_RCC_TIM13_CLK_ENABLE();
	else if (timx == TIM14) 
		__HAL_RCC_TIM14_CLK_ENABLE();
	else
	{
		_Error_Handler(__FILE__, __LINE__);
	}	
}
/*
*********************************************************************************************************
*	�� �� ��: bsp_rcc_tim_disable
*	����˵��: �ر�TIM RCC ʱ��
*	��    ��: TIMx TIM1 - TIM14
*	�� �� ֵ: TIM����ʱ����
*********************************************************************************************************
*/
void bsp_rcc_tim_disable(TIM_TypeDef* timx)
{
	/*
		APB1 ��ʱ���� TIM2, TIM3 ,TIM4, TIM5, TIM6, TIM7, TIM12, TIM13, TIM14 
		APB2 ��ʱ���� TIM1, TIM8 ,TIM9, TIM10, TIM11
	*/
	if (timx == TIM1) 
		__HAL_RCC_TIM3_CLK_DISABLE();
	else if (timx == TIM2) 
		__HAL_RCC_TIM2_CLK_DISABLE();
	else if (timx == TIM3) 
		__HAL_RCC_TIM3_CLK_DISABLE();
	else if (timx == TIM4) 
		__HAL_RCC_TIM4_CLK_DISABLE();
	else if (timx == TIM5) 
		__HAL_RCC_TIM5_CLK_DISABLE();
	else if (timx == TIM6) 
		__HAL_RCC_TIM6_CLK_DISABLE();
	else if (timx == TIM7) 
		__HAL_RCC_TIM7_CLK_DISABLE();
	else if (timx == TIM8) 
		__HAL_RCC_TIM8_CLK_DISABLE();
	else if (timx == TIM9) 
		__HAL_RCC_TIM9_CLK_DISABLE();
	else if (timx == TIM10) 
		__HAL_RCC_TIM10_CLK_DISABLE();
	else if (timx == TIM11) 
		__HAL_RCC_TIM11_CLK_DISABLE();
	else if (timx == TIM12) 
		__HAL_RCC_TIM12_CLK_DISABLE();
	else if (timx == TIM13) 
		__HAL_RCC_TIM13_CLK_DISABLE();
	else if (timx == TIM14) 
		__HAL_RCC_TIM14_CLK_DISABLE();
	else
	{
		_Error_Handler(__FILE__, __LINE__);
	}
}
/*
*********************************************************************************************************
*	�� �� ��: bsp_set_af_oftim
*	����˵��: ����TIM �õ�AF�Ĵ�������
*	��    ��: TIMx TIM1 - TIM14
*	�� �� ֵ: AF�Ĵ�������
*********************************************************************************************************
*/
static uint8_t bsp_set_af_oftim(TIM_TypeDef* timx)
{
	uint8_t ret = 0;

	if (timx == TIM1) 
		ret = GPIO_AF1_TIM1;
	else if (timx == TIM2) 
		ret = GPIO_AF1_TIM2;
	else if (timx == TIM3) 
		ret = GPIO_AF2_TIM3;
	else if (timx == TIM4) 
		ret = GPIO_AF2_TIM4;
	else if (timx == TIM5) 
		ret = GPIO_AF2_TIM5;
	else if (timx == TIM8) 
		ret = GPIO_AF3_TIM8;
	else if (timx == TIM9) 
		ret = GPIO_AF3_TIM9;
	else if (timx == TIM10) 
		ret = GPIO_AF3_TIM10;
	else if (timx == TIM11) 
		ret = GPIO_AF3_TIM11;
	else if (timx == TIM12) 
		ret = GPIO_AF9_TIM12;
	else if (timx == TIM13) 
		ret = GPIO_AF9_TIM13;
	else if (timx == TIM14) 
		ret = GPIO_AF9_TIM14;
	else
	{
		_Error_Handler(__FILE__, __LINE__);
	}
	

	return ret;
}
/*
*********************************************************************************************************
*	�� �� ��: bsp_configtimgpio
*	����˵��: ����GPIO��TIMʱ�ӣ� GPIO���ӵ�TIM���ͨ��
*	��    ��: GPIOx : GPIOA - GPIOK
*			  GPIO_PinX : GPIO_PIN_0 - GPIO__PIN_15
*			  TIMx : TIM1 - TIM14
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void bsp_configtimgpio(GPIO_TypeDef* GPIOx, uint16_t gpio_pin, TIM_TypeDef* timx)
{
	GPIO_InitTypeDef   GPIO_InitStruct;

	/* ʹ��GPIOʱ�� */
	bsp_rcc_gpio_enable(GPIOx);

  	/* ʹ��TIMʱ�� */
	bsp_rcc_tim_enable(timx);

	GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
	GPIO_InitStruct.Pull = GPIO_PULLUP;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
	GPIO_InitStruct.Alternate = bsp_set_af_oftim(timx);
	GPIO_InitStruct.Pin = gpio_pin;
	HAL_GPIO_Init(GPIOx, &GPIO_InitStruct);
}
/*
*********************************************************************************************************
*	�� �� ��: bsp_ConfigGpioOut
*	����˵��: ����GPIOΪ�����������Ҫ����PWM�����ռ�ձ�Ϊ0��100�������
*	��    ��: GPIOx : GPIOA - GPIOK
*			  GPIO_PinX : GPIO_PIN_0 - GPIO__PIN_15
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void bsp_configgpioout(GPIO_TypeDef* GPIOx, uint16_t gpio_pinX)
{
	GPIO_InitTypeDef   GPIO_InitStruct;

	bsp_rcc_gpio_enable(GPIOx);		/* ʹ��GPIOʱ�� */

	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
	GPIO_InitStruct.Pin = gpio_pinX;
	HAL_GPIO_Init(GPIOx, &GPIO_InitStruct);
}
/*
*********************************************************************************************************
*	�� �� ��: bsp_settimoutpwm
*	����˵��: �������������PWM�źŵ�Ƶ�ʺ�ռ�ձ�.  ��Ƶ��Ϊ0������ռ��Ϊ0ʱ���رն�ʱ����GPIO���0��
*			  ��Ƶ��Ϊ0��ռ�ձ�Ϊ100%ʱ��GPIO���1.
*	��    ��: GPIOx : GPIOA - GPIOK
*			  GPIO_Pin : GPIO_PIN_0 - GPIO__PIN_15
*			  TIMx : TIM1 - TIM17
*             _ucChannel��ʹ�õĶ�ʱ��ͨ������Χ1 - 4
*			  _ulFreq : PWM�ź�Ƶ�ʣ���λHz 0 ��ʾ��ֹ���
*			  _ulDutyCycle : PWM�ź�ռ�ձȣ���λ: ���֮һ����5000����ʾ50.00%��ռ�ձ�
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void bsp_settimoutpwm(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin, TIM_TypeDef* TIMx, uint8_t _ucChannel,
	 uint32_t _ulFreq, uint32_t _ulDutyCycle)
{
	TIM_HandleTypeDef   timhandle 	= {0};
	TIM_OC_InitTypeDef	sconfig		= {0};
	uint16_t usperiod 	 = 0;
	uint16_t usprescaler = 0;
	uint32_t pulse		 = 0;
	uint32_t uiTIMxCLK	 = 0;
	const uint16_t timchannel[6+1] = 
	{
		0,TIM_CHANNEL_1,TIM_CHANNEL_2,TIM_CHANNEL_3,TIM_CHANNEL_4
	};
	
	if(_ucChannel > 6)
		_Error_Handler(__FILE__,__LINE__);
	
	if(_ulDutyCycle == 0)
	{
		bsp_configgpioout(GPIOx,GPIO_Pin);/* �ر�TIMʱ�ӣ�����Ӱ������ͨ�� */
		HAL_GPIO_WritePin(GPIOx,GPIO_Pin,GPIO_PIN_RESET);/* PWM=0 */
		return;
	}
	else if(_ulDutyCycle == 10000)
	{
		bsp_configgpioout(GPIOx,GPIO_Pin);/* �ر�TIMʱ�ӣ�����Ӱ������ͨ�� */
		HAL_GPIO_WritePin(GPIOx,GPIO_Pin,GPIO_PIN_SET);/* PWM=0 */
		return;
	}
	
	/* ������PWM */
	bsp_configtimgpio(GPIOx,GPIO_Pin,TIMx);/* ʹ�� GPIO �� TIM ʱ�ӣ������� TIM ͨ���� GPIO */
	
	if ((TIMx == TIM1) || (TIMx == TIM8) || (TIMx == TIM9) || (TIMx == TIM10) || (TIMx == TIM11))
	{
		uiTIMxCLK = SystemCoreClock;
	}
	else	
	{
		uiTIMxCLK = SystemCoreClock / 2;
	}
	if(_ulDutyCycle < 100)
	{
		usprescaler = 10000 - 1;					/* ��Ƶ�� = 10000 */
		usperiod =  (uiTIMxCLK / 10000) / _ulFreq  - 1;		/* �Զ���װ��ֵ */
	}
	else if (_ulFreq < 3000)
	{
		usprescaler = 100 - 1;					/* ��Ƶ�� = 100 */
		usperiod =  (uiTIMxCLK / 100) / _ulFreq  - 1;		/* �Զ���װ��ֵ */
	}
	else	/* ����4K��Ƶ�ʣ������Ƶ */
	{
		usprescaler = 0;					/* ��Ƶ�� = 1 */
		usperiod = uiTIMxCLK / _ulFreq - 1;	/* �Զ���װ��ֵ */
	}
	pulse = (_ulDutyCycle * usperiod) / 10000;

	HAL_TIM_PWM_DeInit(&timhandle);
    
	/*  PWMƵ�� = TIMxCLK / usPrescaler + 1��/usPeriod + 1��*/
	timhandle.Instance = TIMx;
	timhandle.Init.Prescaler         = usprescaler;
	timhandle.Init.Period            = usperiod;
	timhandle.Init.ClockDivision     = 0;
	timhandle.Init.CounterMode       = TIM_COUNTERMODE_UP;
	timhandle.Init.RepetitionCounter = 0;
	if (HAL_TIM_PWM_Init(&timhandle) != HAL_OK)
	{
		_Error_Handler(__FILE__, __LINE__);
	}
	
	/* ���ö�ʱ��PWM���ͨ�� */
	sconfig.OCMode       = TIM_OCMODE_PWM1;
	sconfig.OCPolarity   = TIM_OCPOLARITY_HIGH;
	sconfig.OCFastMode   = TIM_OCFAST_DISABLE;
	sconfig.OCNPolarity  = TIM_OCNPOLARITY_HIGH;
	sconfig.OCNIdleState = TIM_OCNIDLESTATE_RESET;
	sconfig.OCIdleState  = TIM_OCIDLESTATE_RESET;

	/* ռ�ձ� */
	sconfig.Pulse = pulse;
	if (HAL_TIM_PWM_ConfigChannel(&timhandle, &sconfig, timchannel[_ucChannel]) != HAL_OK)
	{
		_Error_Handler(__FILE__, __LINE__);
	}
	
	/* ����PWM��� */
	if (HAL_TIM_PWM_Start(&timhandle, timchannel[_ucChannel]) != HAL_OK)
	{
		_Error_Handler(__FILE__, __LINE__);
	}
}
/*
*********************************************************************************************************
*	�� �� ��: bsp_settimforint
*	����˵��: ����TIM��NVIC�����ڼ򵥵Ķ�ʱ�жϣ�������ʱ�жϡ�����ע���жϷ��������Ҫ���û�Ӧ�ó���ʵ�֡�
*	��    ��: TIMx : ��ʱ��
*			  _ulFreq : ��ʱƵ�� ��Hz���� 0 ��ʾ�رա�
*			  _PreemptionPriority : ��ռ���ȼ�
*			  _SubPriority : �����ȼ�
*	�� �� ֵ: ��
*********************************************************************************************************
*/

void bsp_settimforint(TIM_TypeDef* TIMx, float _ulFreq, uint8_t _PreemptionPriority, uint8_t _SubPriority)
{
	TIM_HandleTypeDef   TimHandle = {0};
	uint16_t usPeriod;
	uint16_t usPrescaler;
	uint32_t uiTIMxCLK;
	
	/* ʹ��TIMʱ�� */
	bsp_rcc_tim_enable(TIMx);
	
	/*-----------------------------------------------------------------------
		system_stm32f4xx.c �ļ��� void SetSysClock(void) ������ʱ�ӵ��������£�

		HCLK = SYSCLK / 1     (AHB1Periph)
		PCLK2 = HCLK / 2      (APB2Periph)
		PCLK1 = HCLK / 4      (APB1Periph)

		��ΪAPB1 prescaler != 1, ���� APB1�ϵ�TIMxCLK = PCLK1 x 2 = SystemCoreClock / 2;
		��ΪAPB2 prescaler != 1, ���� APB2�ϵ�TIMxCLK = PCLK2 x 2 = SystemCoreClock;

		APB1 ��ʱ���� TIM2, TIM3 ,TIM4, TIM5, TIM6, TIM7, TIM12, TIM13,TIM14
		APB2 ��ʱ���� TIM1, TIM8 ,TIM9, TIM10, TIM11

	----------------------------------------------------------------------- */
	if ((TIMx == TIM1) || (TIMx == TIM8) || (TIMx == TIM9) || (TIMx == TIM10) || (TIMx == TIM11))
	{
		uiTIMxCLK = SystemCoreClock;
	}
	else	
	{
		uiTIMxCLK = SystemCoreClock / 2;
	}

	if (_ulFreq < 100)
	{
		usPrescaler = 10000 - 1;					/* ��Ƶ�� = 10000 */
		usPeriod =  (uiTIMxCLK / 10000) / _ulFreq  - 1;		/* �Զ���װ��ֵ */
	}
	else if (_ulFreq < 3000)
	{
		usPrescaler = 100 - 1;					/* ��Ƶ�� = 100 */
		usPeriod =  (uiTIMxCLK / 100) / _ulFreq  - 1;		/* �Զ���װ��ֵ */
	}
	else	/* ����4K��Ƶ�ʣ������Ƶ */
	{
		usPrescaler = 0;					/* ��Ƶ�� = 1 */
		usPeriod = uiTIMxCLK / _ulFreq - 1;	/* �Զ���װ��ֵ */
	}

	/* 
       ��ʱ���жϸ������� = TIMxCLK / usPrescaler + 1��/usPeriod + 1��
	*/
	TimHandle.Instance = TIMx;
	TimHandle.Init.Prescaler         = usPrescaler;
	TimHandle.Init.Period            = usPeriod;	
	TimHandle.Init.ClockDivision     = 0;
	TimHandle.Init.CounterMode       = TIM_COUNTERMODE_UP;
	TimHandle.Init.RepetitionCounter = 0;
	if (HAL_TIM_Base_Init(&TimHandle) != HAL_OK)
	{
		_Error_Handler(__FILE__, __LINE__);
	}

	/* ʹ�ܶ�ʱ���ж�  */
	__HAL_TIM_ENABLE_IT(&TimHandle, TIM_IT_UPDATE);
	

	/* ����TIM��ʱ�����ж� (Update) */
	{
        uint8_t irq = 0;	

        if (TIMx == TIM1) irq = TIM1_UP_TIM10_IRQn;
        else if (TIMx == TIM2) irq = TIM2_IRQn;
        else if (TIMx == TIM3) irq = TIM3_IRQn;
        else if (TIMx == TIM4) irq = TIM4_IRQn;
        else if (TIMx == TIM5) irq = TIM5_IRQn;
        else if (TIMx == TIM6) irq = TIM6_DAC_IRQn;
        else if (TIMx == TIM7) irq = TIM7_IRQn;
        else if (TIMx == TIM8) irq = TIM8_UP_TIM13_IRQn;
		else if (TIMx == TIM9) irq = TIM1_BRK_TIM9_IRQn;
        else if (TIMx == TIM10) irq = TIM1_UP_TIM10_IRQn;
        else if (TIMx == TIM11) irq =  TIM1_TRG_COM_TIM11_IRQn;
        else if (TIMx == TIM12) irq = TIM8_BRK_TIM12_IRQn;
        else if (TIMx == TIM13) irq = TIM8_UP_TIM13_IRQn;
        else if (TIMx == TIM14) irq = TIM8_TRG_COM_TIM14_IRQn;
        else
        {
            _Error_Handler(__FILE__, __LINE__);
        }	
        HAL_NVIC_SetPriority((IRQn_Type)irq, _PreemptionPriority, _SubPriority);
        HAL_NVIC_EnableIRQ((IRQn_Type)irq);		
	}
	
	HAL_TIM_Base_Start(&TimHandle);
}

/*
*********************************************************************************************************
*	�� �� ��: TIM6_DAC_IRQHandler
*	����˵��: TIM6��ʱ�жϷ������
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void TIM6_DAC_IRQHandler(void)
{
	if((TIM6->SR & TIM_FLAG_UPDATE) != RESET)
	{
		/* ������±�־ */
		TIM6->SR = ~ TIM_FLAG_UPDATE;
		
		/* ��תLED4 */
		toggle_pin(LED1_GPIO,LED1_PIN);
	}
}


