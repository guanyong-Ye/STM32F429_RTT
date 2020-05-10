

#ifndef _BSP_USART_FIFO_H_
#define _BSP_USART_FIFO_H_

#include "stm32f4xx_hal.h"
/* 串口1的GPIO  PA9, PA10 */
#define USART1_CLK_ENABLE()              __HAL_RCC_USART1_CLK_ENABLE()

#define USART1_TX_GPIO_CLK_ENABLE()      __HAL_RCC_GPIOA_CLK_ENABLE()
#define USART1_TX_GPIO_PORT              GPIOA
#define USART1_TX_PIN                    GPIO_PIN_9
#define USART1_TX_AF                     GPIO_AF7_USART1

#define USART1_RX_GPIO_CLK_ENABLE()      __HAL_RCC_GPIOA_CLK_ENABLE()
#define USART1_RX_GPIO_PORT              GPIOA
#define USART1_RX_PIN                    GPIO_PIN_10
#define USART1_RX_AF                     GPIO_AF7_USART1

/* 串口2的GPIO --- PD5 PD6  */
#define USART2_CLK_ENABLE()              __HAL_RCC_USART2_CLK_ENABLE()

#define USART2_TX_GPIO_CLK_ENABLE()      __HAL_RCC_GPIOD_CLK_ENABLE()
#define USART2_TX_GPIO_PORT              GPIOD
#define USART2_TX_PIN                    GPIO_PIN_5
#define USART2_TX_AF                     GPIO_AF7_USART2

#define USART2_RX_GPIO_CLK_ENABLE()      __HAL_RCC_GPIOD_CLK_ENABLE()
#define USART2_RX_GPIO_PORT              GPIOD
#define USART2_RX_PIN                    GPIO_PIN_6
#define USART2_RX_AF                     GPIO_AF7_USART2

#define	UART1_FIFO_EN	1
#define	UART2_FIFO_EN	1
#define	UART3_FIFO_EN	0
#define	UART4_FIFO_EN	0
#define	UART5_FIFO_EN	0
#define	UART6_FIFO_EN	0
#define	UART7_FIFO_EN	0
#define	UART8_FIFO_EN	0

/* 定义串口波特率和FIFO缓冲区大小，分为发送缓冲区和接收缓冲区, 支持全双工 */
#if UART1_FIFO_EN
	#define UART1_BAUD			115200
	#define UART1_TX_BUF_SIZE	1*1024
	#define UART1_RX_BUF_SIZE	1*1024
#endif
#if UART1_FIFO_EN
	#define UART2_BAUD			115200
	#define UART2_TX_BUF_SIZE	1*1024
	#define UART2_RX_BUF_SIZE	1*1024
#endif
/* 定义端口号 */
typedef enum
{
	COM1 = 0,	/* USART1 */
	COM2 = 1,	/* USART2 */
	COM3 = 2,	/* USART3 */
	COM4 = 3,	/* USART4 */
	COM5 = 4,	/* USART5 */
	COM6 = 5,	/* USART6 */
	COM7 = 6,	/* USART7 */	
	COM8 = 7	/* USART8 */	
}COM_PORT_E;
/* 串口设备结构体 */
typedef struct
{
	USART_TypeDef *uart;		/* STM32内部串口设备指针 */
	uint8_t *ptxbuf;			/* 发送缓冲区 */
	uint8_t *prxbuf;			/* 接收缓冲区 */
	
	uint16_t ustxbufsize;		/* 发送缓冲区大小 */
	uint16_t usrxbufsize;		/* 接收缓冲区大小 */
	
	__IO uint16_t ustxwrite;	/* 发送缓冲区写指针 */
	__IO uint16_t ustxread;		/* 发送缓冲区读指针 */
	__IO uint16_t ustxcount;	/* 等待发送的数据个数 */

	__IO uint16_t usrxwrite;	/* 接收缓冲区写指针 */
	__IO uint16_t usrxread;		/* 接收缓冲区读指针 */
	__IO uint16_t usrxcount;	/* 还未读取的新数据个数 */
	__IO uint16_t read_len;
	void (*sendbefor)(void); 	/* 开始发送之前的回调函数指针（主要用于RS485切换到发送模式） */
	void (*sendover)(void); 	/* 发送完毕的回调函数指针（主要用于RS485将发送模式切换为接收模式） */
	void (*recivenew)(uint8_t _byte);	/* 串口收到数据的回调函数指针 */
	uint8_t sending;			/* 正在发送中 */
}UART_T;
void bsp_inituart(void);

void comsendbuf(COM_PORT_E _ucPort, uint8_t *_ucaBuf, uint16_t _usLen);
void comsendchar(COM_PORT_E _ucPort, uint8_t _ucByte);

uint8_t comgetchar(COM_PORT_E _ucPort, uint8_t *_pByte);

void comcleartxfifo(COM_PORT_E _ucPort);
void comclearrxfifo(COM_PORT_E _ucPort);

void comsetbaud(COM_PORT_E _ucPort, uint32_t _BaudRate);
void bsp_setuartparam(USART_TypeDef *Instance,  uint32_t BaudRate, uint32_t Parity, uint32_t Mode);

uint8_t uarttxempty(COM_PORT_E _ucPort);

extern UART_T g_tUart1;

#endif
