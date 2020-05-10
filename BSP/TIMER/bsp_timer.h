
#ifndef __BSP_TIMER_H
#define __BSP_TIMER_H

#include "stm32f4xx_hal.h"

#define TMR_CNT 4 		/* �����ʱ���ĸ��� */

typedef enum
{
	TMR_ONCE_MODE = 0,		/* һ�ι���ģʽ */
	TMR_AUTO_MODE = 1		/* �Զ���ʱ����ģʽ */
}TMR_MODE_E;

/* ��ʱ���ṹ�壬��Ա���������� volatile, 
   ����C�������Ż�ʱ���������� */
typedef struct
{
	__IO uint8_t mode;		/* ������ģʽ��1���� */
	__IO uint8_t flag;		/* ��ʱ�����־  */
	__IO uint32_t count;	/* ������ */
	__IO uint32_t preload;	/* ������Ԥװֵ */
}soft_tmr;

/* �ṩ������C�ļ����õĺ��� */
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

