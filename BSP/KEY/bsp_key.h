

#ifndef __BSP_KEY_H
#define __BSP_KEY_H

#include "stm32f4xx_hal.h"

#define KEY_ON 	0
#define KEY_OFF	1

#define HARD_KEY_NUM	    2	   						/* ʵ�尴������ */
#define KEY_COUNT   	 	(HARD_KEY_NUM + 2)			/* 8�������� + 2����ϰ��� */

/* ʹ��GPIOʱ�� */
#define allkey_gpioclkopen() {	\
		__HAL_RCC_GPIOC_CLK_ENABLE();	\
		__HAL_RCC_GPIOA_CLK_ENABLE();	\
	};
/*
	�����˲�ʱ��20ms, ��λ10ms��
	ֻ��������⵽50ms״̬�������Ϊ��Ч����������Ͱ��������¼�
	��ʹ������·����Ӳ���˲������˲�����Ҳ���Ա�֤�ɿ��ؼ�⵽�����¼�
*/
#define KEY_FILTER_TIME   2
#define KEY_LONG_TIME     100			/* ��λ10ms�� ����1�룬��Ϊ�����¼� */

	
	
typedef struct
{
	/* ������һ������ָ�룬ָ���жϰ����ַ��µĺ��� */
	uint8_t (*iskeydownfunc)(void); /* �������µ��жϺ���,1 ��ʾ���� */
	
	uint8_t count; 			/* �˲��������� */
	uint16_t longcount; 	/* ���������� */
	uint16_t longtime; 		/* �������³���ʱ��, 0 ��ʾ����ⳤ�� */
	uint8_t state; 			/* ������ǰ״̬�����»��ǵ��� */
	uint8_t repeatspeed; 	/* ������������ */
	uint8_t repeatcount; 	/* �������������� */
}key_t;


typedef enum
{
	KEY_NONE = 0,			/* 0 ��ʾ�����¼� */

	KEY_1_DOWN,				/* 1������ */
	KEY_1_UP,				/* 1������ */
	KEY_1_LONG,				/* 1������ */

	KEY_2_DOWN,				/* 2������ */
	KEY_2_UP,				/* 2������ */
	KEY_2_LONG,				/* 2������ */

	KEY_3_DOWN,				/* 3������ */
	KEY_3_UP,				/* 3������ */
	KEY_3_LONG,				/* 3������ */

	KEY_4_DOWN,				/* 4������ */
	KEY_4_UP,				/* 4������ */
	KEY_4_LONG,				/* 4������ */

	KEY_5_DOWN,				/* 5������ */
	KEY_5_UP,				/* 5������ */
	KEY_5_LONG,				/* 5������ */

	KEY_6_DOWN,				/* 6������ */
	KEY_6_UP,				/* 6������ */
	KEY_6_LONG,				/* 6������ */

	KEY_7_DOWN,				/* 7������ */
	KEY_7_UP,				/* 7������ */
	KEY_7_LONG,				/* 7������ */

	KEY_8_DOWN,				/* 8������ */
	KEY_8_UP,				/* 8������ */
	KEY_8_LONG,				/* 8������ */

	/* ��ϼ� */
	KEY_9_DOWN,				/* 9������ */
	KEY_9_UP,				/* 9������ */
	KEY_9_LONG,				/* 9������ */

	KEY_10_DOWN,			/* 10������ */
	KEY_10_UP,				/* 10������ */
	KEY_10_LONG,			/* 10������ */
}KEY_ENUM;

/* ����FIFO�õ����� */
#define KEY_FIFO_SIZE	10
typedef struct
{
	uint8_t buf[KEY_FIFO_SIZE];		/* ��ֵ������ */
	uint8_t read;					/* ��������ָ��1 */
	uint8_t write;					/* ������дָ�� */
	uint8_t read2;					/* ��������ָ��2 */
	/* ��ĳЩ����£�����������������Ҫ���ʰ�����������
	Ϊ�˱����ֵ������һ������ȡ������˵�2����ָ��Read2 */
}key_fifo_t;


/* ����ID, ��Ҫ����bsp_KeyState()��������ڲ��� */
typedef enum
{
	KID_K1 = 0,
	KID_K2,
}KEY_ID_E;

/* ���ζ���GPIO */
typedef struct
{
	GPIO_TypeDef* gpio;
	uint16_t pin;
	uint8_t activelevel;	/* �����ƽ */
}x_gpio_t;

/* ���ⲿ���õĺ������� */
void bsp_initkey(void);
void bsp_keyscan10ms(void);
void bsp_putkey_fifo(uint8_t keycode);
uint8_t bsp_getkey_state(KEY_ID_E _ucKeyID);
uint8_t bsp_getkey(void);
uint8_t bsp_getkey2(void);
void bsp_setkeyparam(uint8_t _ucKeyID, uint16_t _LongTime, uint8_t  _RepeatSpeed);
void bsp_clearkey(void);

#endif

