
#include "bsp.h"
/* 定于软件定时器结构体变量 */
static soft_tmr s_tmr[TMR_CNT];
/*
	全局运行时间，单位1ms
	最长可以表示 24.85天，如果你的产品连续运行时间超过这个数，则必须考虑溢出问题
*/
__IO int32_t getruntime = 0;
static __IO uint8_t enablesystickisr = 0;	/* 等待变量初始化 */
/* 这2个全局变量转用于 bsp_DelayMS() 函数 */
static volatile uint32_t delaycnt = 0;
static volatile uint8_t timeoutflag = 0;


/*
*********************************************************************************************************
*	函 数 名: bsp_init_timer
*	功能说明: 配置systick中断，并初始化软件定时器变量
*	形    参:  无
*	返 回 值: 无
*********************************************************************************************************
*/
void bsp_init_timer(void)
{
	uint8_t i;

	/* 清零所有的软件定时器 */
	for (i = 0; i < TMR_CNT; i++)
	{
		s_tmr[i].count = 0;
		s_tmr[i].preload = 0;
		s_tmr[i].flag = 0;
		s_tmr[i].mode = TMR_ONCE_MODE;	/* 缺省是1次性工作模式 */
	}

	/*
		配置systic中断周期为1ms，并启动systick中断。

    	SystemCoreClock 是固件中定义的系统内核时钟

    	SysTick_Config() 函数的形参表示内核时钟多少个周期后触发一次Systick定时中断.
	    	-- SystemCoreClock / 1000  表示定时频率为 1000Hz， 也就是定时周期为  1ms
	    	-- SystemCoreClock / 500   表示定时频率为 500Hz，  也就是定时周期为  2ms
	    	-- SystemCoreClock / 2000  表示定时频率为 2000Hz， 也就是定时周期为  500us

    	对于常规的应用，我们一般取定时周期1ms。对于低速CPU或者低功耗应用，可以设置定时周期为 10ms
    */
	SysTick_Config(SystemCoreClock / 1000);
	
	enablesystickisr = 1;		/* 1表示执行systick中断 */
	
}
/*
*********************************************************************************************************
*	函 数 名: bsp_start_timer
*	功能说明: 启动一个定时器，并设置定时周期。
*	形    参:  	_id     : 定时器ID，值域【0,TMR_COUNT-1】。用户必须自行维护定时器ID，以避免定时器ID冲突。
*				_period : 定时周期，单位1ms
*	返 回 值: 无
*********************************************************************************************************
*/
void bsp_start_timer(uint8_t _id, uint32_t _period)
{
	if (_id >= TMR_CNT)
	{
		/* 打印出错的源代码文件名、函数名称 */
		BSP_Printf("Error: file %s, function %s()\r\n", __FILE__, __FUNCTION__);
		while(1); /* 参数异常，死机等待看门狗复位 */
	}

	DISABLE_INT();  			/* 关中断 */

	s_tmr[_id].count = _period;		/* 实时计数器初值 */
	s_tmr[_id].preload = _period;		/* 计数器自动重装值，仅自动模式起作用 */
	s_tmr[_id].flag = 0;				/* 定时时间到标志 */
	s_tmr[_id].mode = TMR_ONCE_MODE;	/* 1次性工作模式 */

	ENABLE_INT();  				/* 开中断 */
}
/*
*********************************************************************************************************
*	函 数 名: bsp_start_autotimer
*	功能说明: 启动一个自动定时器，并设置定时周期。
*	形    参:  	_id     : 定时器ID，值域【0,TMR_COUNT-1】。用户必须自行维护定时器ID，以避免定时器ID冲突。
*				_period : 定时周期，单位10ms
*	返 回 值: 无
*********************************************************************************************************
*/
void bsp_start_autotimer(uint8_t _id, uint32_t _period)
{
	if (_id >= TMR_CNT)
	{
		/* 打印出错的源代码文件名、函数名称 */
		BSP_Printf("Error: file %s, function %s()\r\n", __FILE__, __FUNCTION__);
		while(1); /* 参数异常，死机等待看门狗复位 */
	}

	DISABLE_INT();  		/* 关中断 */

	s_tmr[_id].count = _period;		/* 实时计数器初值 */
	s_tmr[_id].preload = _period;		/* 计数器自动重装值，仅自动模式起作用 */
	s_tmr[_id].flag = 0;				/* 定时时间到标志 */
	s_tmr[_id].mode = TMR_AUTO_MODE;	/* 1次性工作模式 */

	ENABLE_INT();  			/* 开中断 */
}
/*
*********************************************************************************************************
*	函 数 名: bsp_stop_timer
*	功能说明: 停止一个定时器
*	形    参:  	_id     : 定时器ID，值域【0,TMR_COUNT-1】。用户必须自行维护定时器ID，以避免定时器ID冲突。
*	返 回 值: 无
*********************************************************************************************************
*/
void bsp_stop_timer(uint8_t _id)
{
	if (_id >= TMR_CNT)
	{
		/* 打印出错的源代码文件名、函数名称 */
		BSP_Printf("Error: file %s, function %s()\r\n", __FILE__, __FUNCTION__);
		while(1); /* 参数异常，死机等待看门狗复位 */
	}

	DISABLE_INT();  	/* 关中断 */

	s_tmr[_id].count = 0;		/* 实时计数器初值 */
	s_tmr[_id].flag = 0;				/* 定时时间到标志 */
	s_tmr[_id].mode = TMR_ONCE_MODE;	/* 1次性工作模式 */

	ENABLE_INT();  		/* 开中断 */
}
/*
*********************************************************************************************************
*	函 数 名: bsp_checktimer
*	功能说明: 检测定时器是否超时
*	形    参:  	_id     : 定时器ID，值域【0,TMR_COUNT-1】。用户必须自行维护定时器ID，以避免定时器ID冲突。
*				_period : 定时周期，单位1ms
*	返 回 值: 返回 0 表示定时未到， 1表示定时到
*********************************************************************************************************
*/
uint8_t bsp_checktimer(uint8_t _id)
{
	if (_id >= TMR_CNT)
	{
		return 0;
	}

	if (s_tmr[_id].flag == 1)
	{
		s_tmr[_id].flag = 0;
		return 1;
	}
	else
	{
		return 0;
	}
}
/*
*********************************************************************************************************
*	函 数 名: bsp_getruntime
*	功能说明: 获取CPU运行时间，单位1ms。最长可以表示 24.85天，如果你的产品连续运行时间超过这个数，则必须考虑溢出问题
*	形    参:  无
*	返 回 值: CPU运行时间，单位1ms
*********************************************************************************************************
*/
int32_t bsp_getruntime(void)
{
	int32_t runtime;

	DISABLE_INT();  	/* 关中断 */

	runtime = getruntime;	/* 这个变量在Systick中断中被改写，因此需要关中断进行保护 */

	ENABLE_INT();  		/* 开中断 */

	return runtime;
}
/*
*********************************************************************************************************
*	函 数 名: bsp_checkruntime
*	功能说明: 计算当前运行时间和给定时刻之间的差值。处理了计数器循环。
*	形    参:  _LastTime 上个时刻
*	返 回 值: 当前时间和过去时间的差值，单位1ms
*********************************************************************************************************
*/
int32_t bsp_checkruntime(int32_t _lastTime)
{
	int32_t now_time;
	int32_t time_diff;

	DISABLE_INT();  	/* 关中断 */

	now_time = getruntime;	/* 这个变量在Systick中断中被改写，因此需要关中断进行保护 */

	ENABLE_INT();  		/* 开中断 */
	
	if (now_time >= _lastTime)
	{
		time_diff = now_time - _lastTime;
	}
	else
	{
		time_diff = 0x7FFFFFFF - _lastTime + now_time;
	}

	return time_diff;
}
/*
*********************************************************************************************************
*	函 数 名: bsp_SoftTimerDec
*	功能说明: 每隔1ms对所有定时器变量减1。必须被SysTick_ISR周期性调用。
*	形    参:  _tmr : 定时器变量指针
*	返 回 值: 无
*********************************************************************************************************
*/
static void bsp_softtimerdec(soft_tmr *_tmr)
{
	if (_tmr->count > 0)
	{
		/* 如果定时器变量减到1则设置定时器到达标志 */
		if (--_tmr->count == 0)
		{
			_tmr->flag = 1;

			/* 如果是自动模式，则自动重装计数器 */
			if(_tmr->mode == TMR_AUTO_MODE)
			{
				_tmr->count = _tmr->preload;
			}
		}
	}
}
/*
*********************************************************************************************************
*	函 数 名: bsp_delaymS
*	功能说明: ms级延迟，延迟精度为正负1ms
*	形    参:  n : 延迟长度，单位1 ms。 n 应大于2
*	返 回 值: 无
*********************************************************************************************************
*/
void bsp_delayms(uint32_t n)
{
	if (n == 0)
	{
		return;
	}
	else if (n == 1)
	{
		n = 2;
	}

	DISABLE_INT();  			/* 关中断 */

	delaycnt = n;
	timeoutflag = 0;

	ENABLE_INT();  				/* 开中断 */

	while (1)
	{
		bsp_idle();				/* CPU空闲执行的操作， 见 bsp.c 和 bsp.h 文件 */

		/*
			等待延迟时间到
			注意：编译器认为 s_ucTimeOutFlag = 0，所以可能优化错误，因此 s_ucTimeOutFlag 变量必须申明为 volatile
		*/
		if (timeoutflag == 1)
		{
			break;
		}
	}
}

/*
*********************************************************************************************************
*    函 数 名: bsp_delayus
*    功能说明: us级延迟。 必须在systick定时器启动后才能调用此函数。
*    形    参:  n : 延迟长度，单位1 us
*    返 回 值: 无
*********************************************************************************************************
*/
void bsp_delayus(uint32_t n)
{
    uint32_t ticks;
    uint32_t told;
    uint32_t tnow;
    uint32_t tcnt = 0;
    uint32_t reload;
       
	reload = SysTick->LOAD;                
    ticks = n * (SystemCoreClock / 1000000);	 /* 需要的节拍数 */  
    
    tcnt = 0;
    told = SysTick->VAL;             /* 刚进入时的计数器值 */

    while (1)
    {
        tnow = SysTick->VAL;    
        if (tnow != told)
        {    
            /* SYSTICK是一个递减的计数器 */    
            if (tnow < told)
            {
                tcnt += told - tnow;    
            }
            /* 重新装载递减 */
            else
            {
                tcnt += reload - tnow + told;    
            }        
            told = tnow;

            /* 时间超过/等于要延迟的时间,则退出 */
            if (tcnt >= ticks)
            {
            	break;
            }
        }  
    }
} 
/*
*********************************************************************************************************
*	函 数 名: SysTick_ISR
*	功能说明: SysTick中断服务程序，每隔1ms进入1次
*	形    参:  无
*	返 回 值: 无
*********************************************************************************************************
*/
extern void bsp_RunPer1ms(void);
extern void bsp_RunPer10ms(void);
void SysTick_ISR(void)
{
	static uint8_t s_count = 0;
	uint8_t i;
	
	/* 每隔1ms进来1次 （仅用于 bsp_DelayMS） */
	if (delaycnt > 0)
	{
		if (--delaycnt == 0)
		{
			timeoutflag = 1;
		}
	}

	/* 每隔1ms，对软件定时器的计数器进行减一操作 */
	for (i = 0; i < TMR_CNT; i++)
	{
		bsp_softtimerdec(&s_tmr[i]);
	}

	/* 全局运行时间每1ms增1 */
	getruntime++;
	if (getruntime == 0x7FFFFFFF)	/* 这个变量是 int32_t 类型，最大数为 0x7FFFFFFF */
	{
		getruntime = 0;
	}

	bsp_RunPer1ms();		/* 每隔1ms调用一次此函数，此函数在 bsp.c */

	if (++s_count >= 10)
	{
		s_count = 0;

		bsp_RunPer10ms();	/* 每隔10ms调用一次此函数，此函数在 bsp.c */
	}
}
/*
*********************************************************************************************************
*	函 数 名: SysTick_Handler
*	功能说明: 系统嘀嗒定时器中断服务程序。启动文件中引用了该函数。
*	形    参:  无
*	返 回 值: 无
*********************************************************************************************************
*/
//void SysTick_Handler(void)
//{
//	/* enter interrupt */
//	rt_interrupt_enter();
//	HAL_IncTick();
//	if (enablesystickisr == 0)
//	{
//		return;
//	}
//	HAL_SYSTICK_IRQHandler();
//	SysTick_ISR();	/* 安富莱bsp库的滴答定时中断服务程序 */
//		/* leave interrupt */
//	rt_interrupt_leave();
//}

