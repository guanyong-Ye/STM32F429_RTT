
#ifndef __BSP_TIMER_H
#define __BSP_TIMER_H

#include "stm32f4xx_hal.h"

#define TMR_CNT 4 		/* 软件定时器的个数 */

typedef enum
{
	TMR_ONCE_MODE = 0,		/* 一次工作模式 */
	TMR_AUTO_MODE = 1		/* 自动定时工作模式 */
}TMR_MODE_E;

/* 定时器结构体，成员变量必须是 volatile, 
   否则C编译器优化时可能有问题 */
typedef struct
{
	__IO uint8_t mode;		/* 计数器模式，1次性 */
	__IO uint8_t flag;		/* 定时到达标志  */
	__IO uint32_t count;	/* 计数器 */
	__IO uint32_t preload;	/* 计数器预装值 */
}soft_tmr;

/* 提供给其他C文件调用的函数 */
void bsp_init_timer(void);
void bsp_start_timer(uint8_t _id, uint32_t _period);
void bsp_start_autotimer(uint8_t _id, uint32_t _period);
void bsp_stop_timer(uint8_t _id);
uint8_t bsp_checktimer(uint8_t _id);
int32_t bsp_getruntime(void);
int32_t bsp_checkruntime(int32_t _LastTime);
void SysTick_ISR(void);
void bsp_delayms(uint32_t n);
void bsp_delayus(uint32_t n);

#endif

