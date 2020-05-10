
#include "bsp.h"

/*
*********************************************************************************************************
*	函 数 名: bsp_rcc_gpio_enable
*	功能说明: 使能GPIO时钟
*	形    参: GPIOx GPIOA - GPIOK
*	返 回 值: 无
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
*	函 数 名: bsp_RCC_TIM_Enable
*	功能说明: 使能TIM RCC 时钟
*	形    参: TIMx TIM1 - TIM14
*	返 回 值: 无
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
*	函 数 名: bsp_rcc_tim_disable
*	功能说明: 关闭TIM RCC 时钟
*	形    参: TIMx TIM1 - TIM14
*	返 回 值: TIM外设时钟名
*********************************************************************************************************
*/
void bsp_rcc_tim_disable(TIM_TypeDef* timx)
{
	/*
		APB1 定时器有 TIM2, TIM3 ,TIM4, TIM5, TIM6, TIM7, TIM12, TIM13, TIM14 
		APB2 定时器有 TIM1, TIM8 ,TIM9, TIM10, TIM11
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
*	函 数 名: bsp_set_af_oftim
*	功能说明: 根据TIM 得到AF寄存器配置
*	形    参: TIMx TIM1 - TIM14
*	返 回 值: AF寄存器配置
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
*	函 数 名: bsp_configtimgpio
*	功能说明: 配置GPIO和TIM时钟， GPIO连接到TIM输出通道
*	形    参: GPIOx : GPIOA - GPIOK
*			  GPIO_PinX : GPIO_PIN_0 - GPIO__PIN_15
*			  TIMx : TIM1 - TIM14
*	返 回 值: 无
*********************************************************************************************************
*/
void bsp_configtimgpio(GPIO_TypeDef* GPIOx, uint16_t gpio_pin, TIM_TypeDef* timx)
{
	GPIO_InitTypeDef   GPIO_InitStruct;

	/* 使能GPIO时钟 */
	bsp_rcc_gpio_enable(GPIOx);

  	/* 使能TIM时钟 */
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
*	函 数 名: bsp_ConfigGpioOut
*	功能说明: 配置GPIO为推挽输出。主要用于PWM输出，占空比为0和100的情况。
*	形    参: GPIOx : GPIOA - GPIOK
*			  GPIO_PinX : GPIO_PIN_0 - GPIO__PIN_15
*	返 回 值: 无
*********************************************************************************************************
*/
void bsp_configgpioout(GPIO_TypeDef* GPIOx, uint16_t gpio_pinX)
{
	GPIO_InitTypeDef   GPIO_InitStruct;

	bsp_rcc_gpio_enable(GPIOx);		/* 使能GPIO时钟 */

	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
	GPIO_InitStruct.Pin = gpio_pinX;
	HAL_GPIO_Init(GPIOx, &GPIO_InitStruct);
}
/*
*********************************************************************************************************
*	函 数 名: bsp_settimoutpwm
*	功能说明: 设置引脚输出的PWM信号的频率和占空比.  当频率为0，并且占空为0时，关闭定时器，GPIO输出0；
*			  当频率为0，占空比为100%时，GPIO输出1.
*	形    参: GPIOx : GPIOA - GPIOK
*			  GPIO_Pin : GPIO_PIN_0 - GPIO__PIN_15
*			  TIMx : TIM1 - TIM17
*             _ucChannel：使用的定时器通道，范围1 - 4
*			  _ulFreq : PWM信号频率，单位Hz 0 表示禁止输出
*			  _ulDutyCycle : PWM信号占空比，单位: 万分之一。如5000，表示50.00%的占空比
*	返 回 值: 无
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
		bsp_configgpioout(GPIOx,GPIO_Pin);/* 关闭TIM时钟，可能影响其他通道 */
		HAL_GPIO_WritePin(GPIOx,GPIO_Pin,GPIO_PIN_RESET);/* PWM=0 */
		return;
	}
	else if(_ulDutyCycle == 10000)
	{
		bsp_configgpioout(GPIOx,GPIO_Pin);/* 关闭TIM时钟，可能影响其他通道 */
		HAL_GPIO_WritePin(GPIOx,GPIO_Pin,GPIO_PIN_SET);/* PWM=0 */
		return;
	}
	
	/* 以下是PWM */
	bsp_configtimgpio(GPIOx,GPIO_Pin,TIMx);/* 使能 GPIO 和 TIM 时钟，并连接 TIM 通道到 GPIO */
	
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
		usprescaler = 10000 - 1;					/* 分频比 = 10000 */
		usperiod =  (uiTIMxCLK / 10000) / _ulFreq  - 1;		/* 自动重装的值 */
	}
	else if (_ulFreq < 3000)
	{
		usprescaler = 100 - 1;					/* 分频比 = 100 */
		usperiod =  (uiTIMxCLK / 100) / _ulFreq  - 1;		/* 自动重装的值 */
	}
	else	/* 大于4K的频率，无需分频 */
	{
		usprescaler = 0;					/* 分频比 = 1 */
		usperiod = uiTIMxCLK / _ulFreq - 1;	/* 自动重装的值 */
	}
	pulse = (_ulDutyCycle * usperiod) / 10000;

	HAL_TIM_PWM_DeInit(&timhandle);
    
	/*  PWM频率 = TIMxCLK / usPrescaler + 1）/usPeriod + 1）*/
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
	
	/* 配置定时器PWM输出通道 */
	sconfig.OCMode       = TIM_OCMODE_PWM1;
	sconfig.OCPolarity   = TIM_OCPOLARITY_HIGH;
	sconfig.OCFastMode   = TIM_OCFAST_DISABLE;
	sconfig.OCNPolarity  = TIM_OCNPOLARITY_HIGH;
	sconfig.OCNIdleState = TIM_OCNIDLESTATE_RESET;
	sconfig.OCIdleState  = TIM_OCIDLESTATE_RESET;

	/* 占空比 */
	sconfig.Pulse = pulse;
	if (HAL_TIM_PWM_ConfigChannel(&timhandle, &sconfig, timchannel[_ucChannel]) != HAL_OK)
	{
		_Error_Handler(__FILE__, __LINE__);
	}
	
	/* 启动PWM输出 */
	if (HAL_TIM_PWM_Start(&timhandle, timchannel[_ucChannel]) != HAL_OK)
	{
		_Error_Handler(__FILE__, __LINE__);
	}
}
/*
*********************************************************************************************************
*	函 数 名: bsp_settimforint
*	功能说明: 配置TIM和NVIC，用于简单的定时中断，开启定时中断。另外注意中断服务程序需要由用户应用程序实现。
*	形    参: TIMx : 定时器
*			  _ulFreq : 定时频率 （Hz）。 0 表示关闭。
*			  _PreemptionPriority : 抢占优先级
*			  _SubPriority : 子优先级
*	返 回 值: 无
*********************************************************************************************************
*/

void bsp_settimforint(TIM_TypeDef* TIMx, float _ulFreq, uint8_t _PreemptionPriority, uint8_t _SubPriority)
{
	TIM_HandleTypeDef   TimHandle = {0};
	uint16_t usPeriod;
	uint16_t usPrescaler;
	uint32_t uiTIMxCLK;
	
	/* 使能TIM时钟 */
	bsp_rcc_tim_enable(TIMx);
	
	/*-----------------------------------------------------------------------
		system_stm32f4xx.c 文件中 void SetSysClock(void) 函数对时钟的配置如下：

		HCLK = SYSCLK / 1     (AHB1Periph)
		PCLK2 = HCLK / 2      (APB2Periph)
		PCLK1 = HCLK / 4      (APB1Periph)

		因为APB1 prescaler != 1, 所以 APB1上的TIMxCLK = PCLK1 x 2 = SystemCoreClock / 2;
		因为APB2 prescaler != 1, 所以 APB2上的TIMxCLK = PCLK2 x 2 = SystemCoreClock;

		APB1 定时器有 TIM2, TIM3 ,TIM4, TIM5, TIM6, TIM7, TIM12, TIM13,TIM14
		APB2 定时器有 TIM1, TIM8 ,TIM9, TIM10, TIM11

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
		usPrescaler = 10000 - 1;					/* 分频比 = 10000 */
		usPeriod =  (uiTIMxCLK / 10000) / _ulFreq  - 1;		/* 自动重装的值 */
	}
	else if (_ulFreq < 3000)
	{
		usPrescaler = 100 - 1;					/* 分频比 = 100 */
		usPeriod =  (uiTIMxCLK / 100) / _ulFreq  - 1;		/* 自动重装的值 */
	}
	else	/* 大于4K的频率，无需分频 */
	{
		usPrescaler = 0;					/* 分频比 = 1 */
		usPeriod = uiTIMxCLK / _ulFreq - 1;	/* 自动重装的值 */
	}

	/* 
       定时器中断更新周期 = TIMxCLK / usPrescaler + 1）/usPeriod + 1）
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

	/* 使能定时器中断  */
	__HAL_TIM_ENABLE_IT(&TimHandle, TIM_IT_UPDATE);
	

	/* 配置TIM定时更新中断 (Update) */
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
*	函 数 名: TIM6_DAC_IRQHandler
*	功能说明: TIM6定时中断服务程序
*	返 回 值: 无
*********************************************************************************************************
*/
void TIM6_DAC_IRQHandler(void)
{
	if((TIM6->SR & TIM_FLAG_UPDATE) != RESET)
	{
		/* 清除更新标志 */
		TIM6->SR = ~ TIM_FLAG_UPDATE;
		
		/* 翻转LED4 */
		toggle_pin(LED1_GPIO,LED1_PIN);
	}
}


