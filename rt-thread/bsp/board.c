#include "board.h"


#if 0
/* 内核外设 NVIC 相关的寄存器定义 */
#define _SCB_BASE       (0xE000E010UL)
#define _SYSTICK_CTRL   (*(rt_uint32_t *)(_SCB_BASE + 0x0))
#define _SYSTICK_LOAD   (*(rt_uint32_t *)(_SCB_BASE + 0x4))
#define _SYSTICK_VAL    (*(rt_uint32_t *)(_SCB_BASE + 0x8))
#define _SYSTICK_CALIB  (*(rt_uint32_t *)(_SCB_BASE + 0xC))
#define _SYSTICK_PRI    (*(rt_uint8_t  *)(0xE000ED23UL))

// Updates the variable SystemCoreClock and must be called 
// whenever the core clock is changed during program execution.
extern void SystemCoreClockUpdate(void);

// Holds the system core clock, which is the system clock 
// frequency supplied to the SysTick timer and the processor 
// core clock.
extern uint32_t SystemCoreClock;

static uint32_t _SysTick_Config(rt_uint32_t ticks)
{
    if ((ticks - 1) > 0xFFFFFF)
    {
        return 1;
    }
    
    _SYSTICK_LOAD = ticks - 1; 
    _SYSTICK_PRI = 0xFF;
    _SYSTICK_VAL  = 0;
    _SYSTICK_CTRL = 0x07;  
    
    return 0;
}
#endif

#if defined(RT_USING_USER_MAIN) && defined(RT_USING_HEAP)
#define RT_HEAP_SIZE 4*1024
static uint32_t rt_heap[RT_HEAP_SIZE];	// heap default size: 4K(1024 * 4)

/* 用于获取堆的起始地址 */
RT_WEAK void *rt_heap_begin_get(void)
{
    return rt_heap;
}
/* 用于获取堆的结束地址 */
RT_WEAK void *rt_heap_end_get(void)
{
    return rt_heap + RT_HEAP_SIZE;
}
#endif
/**
* @brief 重映射串口 DEBUG_USARTx 到 rt_kprintf()函数
* Note： DEBUG_USARTx 是在 bsp_usart.h 中定义的宏，默认使用串口 1
* @param str：要输出到串口的字符串
* @retval 无
*
* @attention
*
*/
void rt_hw_console_output(const char *str)
{
    rt_size_t i = 0, size = 0;
    char a = '\r';

    size = rt_strlen(str);
    for (i = 0; i < size; i++)
    {
        if (*(str + i) == '\n')
        {
			printf("%c",a);
        }
		printf("%c",*(str + i));
    }

}
char rt_hw_console_getchar(void)
{
    int8_t ch = -1;

	ch = getchar();
    return ch;
}
/**
 * This function will initial your board.
 */
void rt_hw_board_init()
{	
	bsp_init();
	/* 调用组件初始化函数 (use INIT_BOARD_EXPORT()) */
#ifdef RT_USING_COMPONENTS_INIT
    rt_components_board_init();
#endif
    
#if defined(RT_USING_CONSOLE) && defined(RT_USING_DEVICE)
	rt_console_set_device(RT_CONSOLE_DEVICE_NAME);
#endif
    
#if defined(RT_USING_USER_MAIN) && defined(RT_USING_HEAP)
    rt_system_heap_init(rt_heap_begin_get(), rt_heap_end_get());
#endif
}

void SysTick_Handler(void)
{
	/* enter interrupt */
	rt_interrupt_enter();
	
	SysTick_ISR();
	rt_tick_increase();

	/* leave interrupt */
	rt_interrupt_leave();
	
}
