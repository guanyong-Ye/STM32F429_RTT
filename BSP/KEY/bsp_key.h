

#ifndef __BSP_KEY_H
#define __BSP_KEY_H

#include "stm32f4xx_hal.h"

#define KEY_ON 	0
#define KEY_OFF	1

#define HARD_KEY_NUM	    2	   						/* 实体按键个数 */
#define KEY_COUNT   	 	(HARD_KEY_NUM + 2)			/* 8个独立建 + 2个组合按键 */

/* 使能GPIO时钟 */
#define allkey_gpioclkopen() {	\
		__HAL_RCC_GPIOC_CLK_ENABLE();	\
		__HAL_RCC_GPIOA_CLK_ENABLE();	\
	};
/*
	按键滤波时间20ms, 单位10ms。
	只有连续检测到50ms状态不变才认为有效，包括弹起和按下两种事件
	即使按键电路不做硬件滤波，该滤波机制也可以保证可靠地检测到按键事件
*/
#define KEY_FILTER_TIME   2
#define KEY_LONG_TIME     100			/* 单位10ms， 持续1秒，认为长按事件 */

	
	
typedef struct
{
	/* 下面是一个函数指针，指向判断按键手否按下的函数 */
	uint8_t (*iskeydownfunc)(void); /* 按键按下的判断函数,1 表示按下 */
	
	uint8_t count; 			/* 滤波器计数器 */
	uint16_t longcount; 	/* 长按计数器 */
	uint16_t longtime; 		/* 按键按下持续时间, 0 表示不检测长按 */
	uint8_t state; 			/* 按键当前状态（按下还是弹起） */
	uint8_t repeatspeed; 	/* 连续按键周期 */
	uint8_t repeatcount; 	/* 连续按键计数器 */
}key_t;


typedef enum
{
	KEY_NONE = 0,			/* 0 表示按键事件 */

	KEY_1_DOWN,				/* 1键按下 */
	KEY_1_UP,				/* 1键弹起 */
	KEY_1_LONG,				/* 1键长按 */

	KEY_2_DOWN,				/* 2键按下 */
	KEY_2_UP,				/* 2键弹起 */
	KEY_2_LONG,				/* 2键长按 */

	KEY_3_DOWN,				/* 3键按下 */
	KEY_3_UP,				/* 3键弹起 */
	KEY_3_LONG,				/* 3键长按 */

	KEY_4_DOWN,				/* 4键按下 */
	KEY_4_UP,				/* 4键弹起 */
	KEY_4_LONG,				/* 4键长按 */

	KEY_5_DOWN,				/* 5键按下 */
	KEY_5_UP,				/* 5键弹起 */
	KEY_5_LONG,				/* 5键长按 */

	KEY_6_DOWN,				/* 6键按下 */
	KEY_6_UP,				/* 6键弹起 */
	KEY_6_LONG,				/* 6键长按 */

	KEY_7_DOWN,				/* 7键按下 */
	KEY_7_UP,				/* 7键弹起 */
	KEY_7_LONG,				/* 7键长按 */

	KEY_8_DOWN,				/* 8键按下 */
	KEY_8_UP,				/* 8键弹起 */
	KEY_8_LONG,				/* 8键长按 */

	/* 组合键 */
	KEY_9_DOWN,				/* 9键按下 */
	KEY_9_UP,				/* 9键弹起 */
	KEY_9_LONG,				/* 9键长按 */

	KEY_10_DOWN,			/* 10键按下 */
	KEY_10_UP,				/* 10键弹起 */
	KEY_10_LONG,			/* 10键长按 */
}KEY_ENUM;

/* 按键FIFO用到变量 */
#define KEY_FIFO_SIZE	10
typedef struct
{
	uint8_t buf[KEY_FIFO_SIZE];		/* 键值缓冲区 */
	uint8_t read;					/* 缓冲区读指针1 */
	uint8_t write;					/* 缓冲区写指针 */
	uint8_t read2;					/* 缓冲区读指针2 */
	/* 在某些情况下，可能有两个任务都需要访问按键缓冲区，
	为了避免键值被其中一个任务取空添加了第2个读指针Read2 */
}key_fifo_t;


/* 按键ID, 主要用于bsp_KeyState()函数的入口参数 */
typedef enum
{
	KID_K1 = 0,
	KID_K2,
}KEY_ID_E;

/* 依次定义GPIO */
typedef struct
{
	GPIO_TypeDef* gpio;
	uint16_t pin;
	uint8_t activelevel;	/* 激活电平 */
}x_gpio_t;

/* 供外部调用的函数声明 */
void bsp_initkey(void);
void bsp_keyscan10ms(void);
void bsp_putkey_fifo(uint8_t keycode);
uint8_t bsp_getkey_state(KEY_ID_E _ucKeyID);
uint8_t bsp_getkey(void);
uint8_t bsp_getkey2(void);
void bsp_setkeyparam(uint8_t _ucKeyID, uint16_t _LongTime, uint8_t  _RepeatSpeed);
void bsp_clearkey(void);

#endif

