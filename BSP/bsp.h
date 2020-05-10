/*
*********************************************************************************************************
*
*	模块名称 : BSP模块
*	文件名称 : bsp.h
*	版    本 : V1.0
*	说    明 : 这是硬件底层驱动程序的主文件。每个c文件可以 #include "bsp.h" 来包含所有的外设驱动模块。
*			   bsp = Borad surport packet 板级支持包
*
*********************************************************************************************************
*/

#ifndef _BSP_H_
#define _BSP_H_

/* 开关全局中断的宏 */
#define ENABLE_INT()	__set_PRIMASK(0)	/* 使能全局中断 */
#define DISABLE_INT()	__set_PRIMASK(1)	/* 禁止全局中断 */

/* 这个宏仅用于调试阶段排错 */
#define BSP_Printf		printf
//#define BSP_Printf(...)

#define EXTI9_5_ISR_MOVE_OUT		/* bsp.h 中定义此行，表示本函数移到 stam32f4xx_it.c。 避免重复定义 */

/* 默认是关闭状态 */
#define  Enable_EventRecorder  0

#if Enable_EventRecorder == 1
	#include "EventRecorder.h"
#endif

#include "stm32f4xx_hal.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <rthw.h>
#include <rtthread.h>
#ifndef TRUE
	#define TRUE  1
#endif

#ifndef FALSE
	#define FALSE 0
#endif

/* 定义优先级分组 */
#define NVIC_PREEMPT_PRIORITY	4

/* 通过取消注释或者添加注释的方式控制是否包含底层驱动模块 */

#include "bsp_gpio.h"

#include "bsp_key.h"

#include "bsp_timer.h"
#include "bsp_tim.h"
#include "bsp_uart.h"
//#include "bsp_i2c_gpio.h"
//#include "bsp_spi_bus.h"

//#include "bsp_eeprom_24xx.h"
//#include "bsp_spi_flash.h"
//#include "esp8266.h"

#include "math_calc.h"

/* 提供给其他C文件调用的函数 */
void bsp_init(void);
void bsp_idle(void);


#endif
