
#ifndef __BSP_TIM_H
#define __BSP_TIM_H

#include "stm32f4xx_hal.h"

void bsp_setTIMoutPWM(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin, TIM_TypeDef* TIMx, uint8_t _ucChannel,
	 uint32_t _ulFreq, uint32_t _ulDutyCycle);	 

void bsp_settimforint(TIM_TypeDef* TIMx, float _ulFreq, uint8_t _PreemptionPriority, uint8_t _SubPriority);


#endif


