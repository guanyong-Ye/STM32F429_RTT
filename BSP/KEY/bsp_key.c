
#include "bsp.h"

static key_t s_tbtn[KEY_COUNT];
static key_fifo_t s_tkey; /* 按键 FIFO 变量,结构体 */
/* GPIO和PIN定义 */
static const x_gpio_t s_gpio_list[HARD_KEY_NUM] = {
	{GPIOA, GPIO_PIN_0, 0},		/* K1 */
	{GPIOC, GPIO_PIN_13, 0},	/* K2 */
};	
/*
*********************************************************************************************************
						按键FIFO相关
*********************************************************************************************************
*/
/*
*********************************************************************************************************
* 函 数 名: bsp_putkey_fifo
* 功能说明: 将 1 个键值压入按键 FIFO 缓冲区。可用于模拟一个按键。
* 形 参: _KeyCode : 按键代码
* 返 回 值: 无
*********************************************************************************************************
*/
void bsp_putkey_fifo(uint8_t keycode)
{
	s_tkey.buf[s_tkey.write] = keycode;
	if(++s_tkey.write >= KEY_FIFO_SIZE)
	{
		s_tkey.write = 0;
	}
}
/*
*********************************************************************************************************
*	函 数 名: bsp_GetKeyState
*	功能说明: 读取按键的状态
*	形    参:  _ucKeyID : 按键ID，从0开始
*	返 回 值: 1 表示按下， 0 表示未按下
*********************************************************************************************************
*/
uint8_t bsp_getkey_state(KEY_ID_E _ucKeyID)
{
	return s_tbtn[_ucKeyID].state;
}
/*
*********************************************************************************************************
* 函 数 名: bsp_GetKey
* 功能说明: 从按键 FIFO 缓冲区读取一个键值。
* 形 参: 无
* 返 回 值: 按键代码
*********************************************************************************************************
*/
uint8_t bsp_getkey(void)
{
	uint8_t ret;
	if(s_tkey.read == s_tkey.write)
	{
		return KEY_NONE;
	}
	else
	{
		ret = s_tkey.buf[s_tkey.read];
		
		if(++s_tkey.read >= KEY_FIFO_SIZE)
		{
			s_tkey.read = 0;
		}
		return ret;
	}
}
/*
*********************************************************************************************************
* 函 数 名: bsp_GetKey2
* 功能说明: 从按键 FIFO 缓冲区读取一个键值。
* 形 参: 无
* 返 回 值: 按键代码
*********************************************************************************************************
*/
uint8_t bsp_getkey2(void)
{
	uint8_t ret;
	if(s_tkey.read2 == s_tkey.write)
	{
		return KEY_NONE;
	}
	else
	{
		ret = s_tkey.buf[s_tkey.read2];
		
		if(++s_tkey.read2 >= KEY_FIFO_SIZE)
		{
			s_tkey.read2 = 0;
		}
		return ret;
	}
}
/*
*********************************************************************************************************
*	函 数 名: bsp_ClearKey
*	功能说明: 清空按键FIFO缓冲区
*	形    参：无
*	返 回 值: 按键代码
*********************************************************************************************************
*/
void bsp_clearkey(void)
{
	s_tkey.read = s_tkey.write;
}

/********************************************************************************************************
						按键按键检测相关
*********************************************************************************************************/

/*
*********************************************************************************************************
*	函 数 名: bsp_SetKeyParam
*	功能说明: 设置按键参数
*	形    参：_ucKeyID : 按键ID，从0开始
*			_LongTime : 长按事件时间
*			 _RepeatSpeed : 连发速度
*	返 回 值: 无
*********************************************************************************************************
*/
void bsp_setkeyparam(uint8_t _ucKeyID, uint16_t _LongTime, uint8_t  _RepeatSpeed)
{
	s_tbtn[_ucKeyID].longtime = _LongTime;			/* 长按时间 0 表示不检测长按键事件 */
	s_tbtn[_ucKeyID].repeatspeed = _RepeatSpeed;			/* 按键连发的速度，0表示不支持连发 */
	s_tbtn[_ucKeyID].repeatcount = 0;						/* 连发计数器 */
}

/*
*********************************************************************************************************
*	函 数 名: bsp_initkey_hard
*	功能说明: 配置按键对应的GPIO
*	形    参:  无
*	返 回 值: 无
*********************************************************************************************************
*/
static void bsp_initkey_hard(void)
{
	GPIO_InitTypeDef GPIO_Init;
	uint8_t i = 0;
	/* 打开GPIO时钟 */
	allkey_gpioclkopen();
	/* 配置所有的按键GPIO输入 */
	GPIO_Init.Mode = GPIO_MODE_INPUT;   			/* 设置输入 */
	GPIO_Init.Pull = GPIO_PULLUP;                 	/* 上拉 */
	GPIO_Init.Speed = GPIO_SPEED_FREQ_LOW;  	/* GPIO速度等级 */
	
	for (i = 0; i < HARD_KEY_NUM; i++)
	{
		GPIO_Init.Pin = s_gpio_list[i].pin;
		HAL_GPIO_Init(s_gpio_list[i].gpio, &GPIO_Init);	
	}
}
/*
*********************************************************************************************************
*	函 数 名: bsp_initkeyvar
*	功能说明: 初始化按键变量
*	形    参:  无
*	返 回 值: 无
*********************************************************************************************************
*/
static void bsp_initkeyvar(void)
{
	uint8_t i;

	/* 对按键FIFO读写指针清零 */
	s_tkey.read = 0;
	s_tkey.write = 0;
	s_tkey.read2 = 0;

	/* 给每个按键结构体成员变量赋一组缺省值 */
	for (i = 0; i < KEY_COUNT; i++)
	{
		s_tbtn[i].longtime	= KEY_LONG_TIME;			/* 长按时间 0 表示不检测长按键事件 */
		s_tbtn[i].count    	= KEY_FILTER_TIME / 2;		/* 计数器设置为滤波时间的一半 */
		s_tbtn[i].state    	= 0;						/* 按键缺省状态，0为未按下 */
		s_tbtn[i].repeatspeed = 0;						/* 按键连发的速度，0表示不支持连发 */
		s_tbtn[i].repeatcount = 0;						/* 连发计数器 */
	}

	/* 如果需要单独更改某个按键的参数，可以在此单独重新赋值 */
	
	/* 摇杆上下左右，支持长按1秒后，自动连发 */
//	bsp_setkeyparam(KID_JOY_U, 100, 6);
//	bsp_setkeyparam(KID_JOY_D, 100, 6);
//	bsp_setkeyparam(KID_JOY_L, 100, 6);
//	bsp_setkeyparam(KID_JOY_R, 100, 6);
}

/*
*********************************************************************************************************
* 函 数 名: bsp_initkey
* 功能说明: 初始化按键. 该函数被 bsp_Init() 调用。
* 形 参: 无
* 返 回 值: 无
*********************************************************************************************************
*/
void bsp_initkey(void)
{
	bsp_initkeyvar(); /* 初始化按键变量 */
	bsp_initkey_hard(); /* 初始化按键硬件 */
}

/*
*********************************************************************************************************
*	函 数 名: keypin_active
*	功能说明: 判断按键是否按下
*	形    参: 无
*	返 回 值: 返回值1 表示按下(导通），0表示未按下（释放）
*********************************************************************************************************
*/
static uint8_t keypin_active(uint8_t _id)
{
	uint8_t level;
	
	if ((s_gpio_list[_id].gpio->IDR & s_gpio_list[_id].pin) == GPIO_PIN_RESET)
	{
		level = 0;
	}
	else
	{
		level = 1;
	}

	if (level == s_gpio_list[_id].activelevel)
	{
		return 1;
	}
	else
	{
		return 0;
	}
}
/*
*********************************************************************************************************
*	函 数 名: iskeydownfunc
*	功能说明: 判断按键是否按下。单键和组合键区分。单键事件不允许有其他键按下。
*	形    参: 无
*	返 回 值: 返回值1 表示按下(导通），0表示未按下（释放）
*********************************************************************************************************
*/
static uint8_t iskeydownfunc(uint8_t id)
{
	uint8_t i = 0;
	uint8_t count = 0;
	uint8_t save = 255;
	
	if(id < HARD_KEY_NUM)
	{
		for(i = 0;i < HARD_KEY_NUM;i++)
		{
			if(keypin_active(i))
			{
				count++;
				save = i;
			}
		}
		if(count == 1 && save == id)
		{
			return 1;	/* 只有1个键按下时才有效 */
		}
		return 0;
	}
	/* 组合键 K1K2 */
	if (id == HARD_KEY_NUM + 0)
	{
		if (keypin_active(KID_K1) && keypin_active(KID_K2))
		{
			return 1;
		}
		else
		{
			return 0;
		}
	}
	return 0;
}
/*
*********************************************************************************************************
*	函 数 名: bsp_DetectKey
*	功能说明: 检测一个按键。非阻塞状态，必须被周期性的调用。
*	形    参: IO的id， 从0开始编码
*	返 回 值: 无
*********************************************************************************************************
*/
static void bsp_detectkey(uint8_t i)
{
	key_t *pbtn;
	pbtn = &s_tbtn[i];
	if(iskeydownfunc(i))
	{
		if(pbtn->count < KEY_FILTER_TIME)
			pbtn->count = KEY_FILTER_TIME;
		else if(pbtn->count < 2 * KEY_FILTER_TIME)
			pbtn->count++;
		else
		{
			/* 短按 */
			if(pbtn->state == 0)
			{
				pbtn->state = 1;
				/* 发送按钮按下的消息 */
				bsp_putkey_fifo((uint8_t)(3 * i + 1));
			}
			/* 长按 */
			if(pbtn->longtime > 0)
			{
				if(pbtn->longcount < pbtn->longtime)
				{
					/* 发送按钮持续按下的消息 */
					if (++pbtn->longcount == pbtn->longtime)
					{
						/* 键值放入按键FIFO */
						bsp_putkey_fifo((uint8_t)(3 * i + 3));
					}
				}
				else
				{
					if (pbtn->repeatspeed > 0)
					{
						if (++pbtn->repeatcount >= pbtn->repeatspeed)
						{
							pbtn->repeatcount = 0;
							/* 常按键后，每隔10ms发送1个按键 */
							bsp_putkey_fifo((uint8_t)(3 * i + 1));
						}
					}
				}
			}
		}
	}
	else
	{
		if(pbtn->count > KEY_FILTER_TIME)
		{
			pbtn->count = KEY_FILTER_TIME;
		}
		else if(pbtn->count != 0)
		{
			pbtn->count--;
		}
		else
		{
			if (pbtn->state == 1)
			{
				pbtn->state = 0;

				/* 发送按钮弹起的消息 */
				bsp_putkey_fifo((uint8_t)(3 * i + 2));
			}
		}

		pbtn->longcount = 0;
		pbtn->repeatcount = 0;
	}
}
/*
*********************************************************************************************************
*	函 数 名: bsp_KeyScan10ms
*	功能说明: 扫描所有按键。非阻塞，被systick中断周期性的调用，10ms一次
*	形    参: 无
*	返 回 值: 无
*********************************************************************************************************
*/
void bsp_keyscan10ms(void)
{
	uint8_t i;
	for(i = 0;i < KEY_COUNT;i++)
		bsp_detectkey(i);
}
/*
*********************************************************************************************************
*	函 数 名: bsp_detect_fastIO
*	功能说明: 检测高速的输入IO. 1ms刷新一次
*	形    参: IO的id， 从0开始编码
*	返 回 值: 无
*********************************************************************************************************
*/
static void bsp_detect_fastIO(uint8_t i)
{
	key_t *pbtn;
	pbtn = &s_tbtn[i];
	if (iskeydownfunc(i))
	{
		if (pbtn->state == 0)
		{
			pbtn->state = 1;

			/* 发送按钮按下的消息 */
			bsp_putkey_fifo((uint8_t)(3 * i + 1));
		}

		if (pbtn->longtime > 0)
		{
			if (pbtn->longcount < pbtn->longtime)
			{
				/* 发送按钮持续按下的消息 */
				if (++pbtn->longcount == pbtn->longtime)
				{
					/* 键值放入按键FIFO */
					bsp_putkey_fifo((uint8_t)(3 * i + 3));
				}
			}
			else
			{
				if (pbtn->repeatspeed > 0)
				{
					if (++pbtn->repeatcount >= pbtn->repeatspeed)
					{
						pbtn->repeatcount = 0;
						/* 常按键后，每隔10ms发送1个按键 */
						bsp_putkey_fifo((uint8_t)(3 * i + 1));
					}
				}
			}
		}
	}
	else
	{
		if (pbtn->state == 1)
		{
			pbtn->state = 0;

			/* 发送按钮弹起的消息 */
			bsp_putkey_fifo((uint8_t)(3 * i + 2));
		}

		pbtn->longcount = 0;
		pbtn->repeatcount = 0;
	}
}
/*
*********************************************************************************************************
*	函 数 名: bsp_KeyScan1ms
*	功能说明: 扫描所有按键。非阻塞，被systick中断周期性的调用，1ms一次.
*	形    参: 无
*	返 回 值: 无
*********************************************************************************************************
*/
void bsp_KeyScan1ms(void)
{
	uint8_t i;

	for (i = 0; i < KEY_COUNT; i++)
	{
		bsp_detect_fastIO(i);
	}
}


