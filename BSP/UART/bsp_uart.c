#include "bsp.h"


/*
*********************************************************************************************************
*
*	模块名称 : 串口中断+FIFO驱动模块
*	文件名称 : bsp_uart_fifo.c
*	版    本 : V1.8
*	说    明 : 采用串口中断+FIFO模式实现多个串口的同时访问
*
*********************************************************************************************************
*/

/* 定义每个串口结构体变量 */
#if UART1_FIFO_EN
	static UART_T g_tUart1;
	static uint8_t g_TxBuf1[UART1_TX_BUF_SIZE];		/* 发送缓冲区 */
	static uint8_t g_RxBuf1[UART1_RX_BUF_SIZE];		/* 接收缓冲区 */
#endif

#if UART2_FIFO_EN
	static UART_T g_tUart2;
	static uint8_t g_TxBuf2[UART2_TX_BUF_SIZE];		/* 发送缓冲区 */
	static uint8_t g_RxBuf2[UART2_RX_BUF_SIZE];		/* 接收缓冲区 */
#endif
/*
*********************************************************************************************************
*	函 数 名: UartVarInit
*	功能说明: 初始化串口相关的变量
*	形    参: 无
*	返 回 值: 无
*********************************************************************************************************
*/
static void uartvarinit(void)
{
#if UART1_FIFO_EN
	g_tUart1.uart 	= USART1;						/* STM32 串口设备 */
	g_tUart1.ptxbuf = g_TxBuf1;					/* 发送缓冲区指针 */
	g_tUart1.prxbuf = g_RxBuf1;					/* 接收缓冲区指针 */
	g_tUart1.ustxbufsize = UART1_TX_BUF_SIZE;	/* 发送缓冲区大小 */
	g_tUart1.usrxbufsize = UART1_RX_BUF_SIZE;	/* 接收缓冲区大小 */
	g_tUart1.ustxwrite 	= 0;						/* 发送FIFO写索引 */
	g_tUart1.ustxread 	= 0;						/* 发送FIFO读索引 */
	g_tUart1.usrxwrite 	= 0;						/* 接收FIFO写索引 */
	g_tUart1.usrxread 	= 0;						/* 接收FIFO读索引 */
	g_tUart1.usrxcount 	= 0;						/* 接收到的新数据个数 */
	g_tUart1.ustxcount 	= 0;						/* 待发送的数据个数 */
	g_tUart1.sendbefor 	= 0;						/* 发送数据前的回调函数 */
	g_tUart1.sendover 	= 0;						/* 发送完毕后的回调函数 */
	g_tUart1.recivenew 	= 0;						/* 接收到新数据后的回调函数 */
	g_tUart1.sending 	= 0;						/* 正在发送中标志 */
	g_tUart1.read_len = 0;
#endif
#if UART1_FIFO_EN
	g_tUart2.uart 	= USART2;						/* STM32 串口设备 */
	g_tUart2.ptxbuf = g_TxBuf2;					/* 发送缓冲区指针 */
	g_tUart2.prxbuf = g_RxBuf2;					/* 接收缓冲区指针 */
	g_tUart2.ustxbufsize = UART2_TX_BUF_SIZE;	/* 发送缓冲区大小 */
	g_tUart2.usrxbufsize = UART2_RX_BUF_SIZE;	/* 接收缓冲区大小 */
	g_tUart2.ustxwrite 	= 0;						/* 发送FIFO写索引 */
	g_tUart2.ustxread 	= 0;						/* 发送FIFO读索引 */
	g_tUart2.usrxwrite 	= 0;						/* 接收FIFO写索引 */
	g_tUart2.usrxread 	= 0;						/* 接收FIFO读索引 */
	g_tUart2.usrxcount 	= 0;						/* 接收到的新数据个数 */
	g_tUart2.ustxcount 	= 0;						/* 待发送的数据个数 */
	g_tUart2.sendbefor 	= 0;						/* 发送数据前的回调函数 */
	g_tUart2.sendover 	= 0;						/* 发送完毕后的回调函数 */
	g_tUart2.recivenew 	= 0;						/* 接收到新数据后的回调函数 */
	g_tUart2.sending 	= 0;						/* 正在发送中标志 */
#endif
}
/*
*********************************************************************************************************
*	函 数 名: bsp_setuartparam
*	功能说明: 配置串口的硬件参数（波特率，数据位，停止位，起始位，校验位，中断使能）适合于STM32- H7开发板
*	形    参: Instance   USART_TypeDef类型结构体
*             BaudRate   波特率
*             Parity     校验类型，奇校验或者偶校验
*             Mode       发送和接收模式使能
*	返 回 值: 无
*********************************************************************************************************
*/
void bsp_setuartparam(USART_TypeDef *Instance,  uint32_t BaudRate, uint32_t Parity, uint32_t Mode)
{
	UART_HandleTypeDef UartHandle;
	
	/*##-1- 配置串口硬件参数 ######################################*/
	/* 异步串口模式 (UART Mode) */
	/* 配置如下:
	  - 字长    = 8 位
	  - 停止位  = 1 个停止位
	  - 校验    = 参数Parity
	  - 波特率  = 参数BaudRate
	  - 硬件流控制关闭 (RTS and CTS signals) */

	UartHandle.Instance        = Instance;

	UartHandle.Init.BaudRate   = BaudRate;
	UartHandle.Init.WordLength = UART_WORDLENGTH_8B;
	UartHandle.Init.StopBits   = UART_STOPBITS_1;
	UartHandle.Init.Parity     = Parity;
	UartHandle.Init.HwFlowCtl  = UART_HWCONTROL_NONE;
	UartHandle.Init.Mode       = Mode;
	UartHandle.Init.OverSampling = UART_OVERSAMPLING_16;
	if (HAL_UART_Init(&UartHandle) != HAL_OK)
	{
		_Error_Handler(__FILE__, __LINE__);
	}
}
/*
*********************************************************************************************************
*	函 数 名: InitHardUart
*	功能说明: 配置串口的硬件参数（波特率，数据位，停止位，起始位，校验位，中断使能）适合于STM32-H7开发板
*	形    参: 无
*	返 回 值: 无
*********************************************************************************************************
*/
static void initharduart(void)
{
	GPIO_InitTypeDef  GPIO_InitStruct;


#if UART1_FIFO_EN		/* 串口1 */
	/* 使能 GPIO TX/RX 时钟 */
	USART1_TX_GPIO_CLK_ENABLE();
	USART1_RX_GPIO_CLK_ENABLE();
	
	/* 使能 USARTx 时钟 */
	USART1_CLK_ENABLE();	

	/* 配置TX引脚 */
	GPIO_InitStruct.Pin       = USART1_TX_PIN;
	GPIO_InitStruct.Mode      = GPIO_MODE_AF_PP;
	GPIO_InitStruct.Pull      = GPIO_PULLUP;
	GPIO_InitStruct.Speed     = GPIO_SPEED_FREQ_VERY_HIGH;
	GPIO_InitStruct.Alternate = USART1_TX_AF;
	HAL_GPIO_Init(USART1_TX_GPIO_PORT, &GPIO_InitStruct);	
	
	/* 配置RX引脚 */
	GPIO_InitStruct.Pin = USART1_RX_PIN;
	GPIO_InitStruct.Alternate = USART1_RX_AF;
	HAL_GPIO_Init(USART1_RX_GPIO_PORT, &GPIO_InitStruct);

	/* 配置NVIC the NVIC for UART */   
	HAL_NVIC_SetPriority(USART1_IRQn, 0, 1);
	HAL_NVIC_EnableIRQ(USART1_IRQn);
  
	/* 配置波特率、奇偶校验 */
	bsp_setuartparam(USART1,  UART1_BAUD, UART_PARITY_NONE, UART_MODE_TX_RX);

	CLEAR_BIT(USART1->SR, USART_SR_TC);   /* 清除TC发送完成标志 */
    CLEAR_BIT(USART1->SR, USART_SR_RXNE); /* 清除RXNE接收标志 */
	// USART_CR1_PEIE | USART_CR1_RXNEIE
	SET_BIT(USART1->CR1, USART_CR1_RXNEIE);	/* 使能PE. RX接受中断 */
#endif
#if UART1_FIFO_EN		/* 串口2 */
	/* 使能 GPIO TX/RX 时钟 */
	USART2_TX_GPIO_CLK_ENABLE();
	USART2_RX_GPIO_CLK_ENABLE();
	
	/* 使能 USARTx 时钟 */
	USART2_CLK_ENABLE();	

	/* 配置TX引脚 */
	GPIO_InitStruct.Pin       = USART2_TX_PIN;
	GPIO_InitStruct.Mode      = GPIO_MODE_AF_PP;
	GPIO_InitStruct.Pull      = GPIO_PULLUP;
	GPIO_InitStruct.Speed     = GPIO_SPEED_FREQ_VERY_HIGH;
	GPIO_InitStruct.Alternate = USART2_TX_AF;
	HAL_GPIO_Init(USART2_TX_GPIO_PORT, &GPIO_InitStruct);	
	
	/* 配置RX引脚 */
	GPIO_InitStruct.Pin = USART2_RX_PIN;
	GPIO_InitStruct.Alternate = USART2_RX_AF;
	HAL_GPIO_Init(USART2_RX_GPIO_PORT, &GPIO_InitStruct);

	/* 配置NVIC the NVIC for UART */   
	HAL_NVIC_SetPriority(USART2_IRQn, 0, 1);
	HAL_NVIC_EnableIRQ(USART2_IRQn);
  
	/* 配置波特率、奇偶校验 */
	bsp_setuartparam(USART2,  UART2_BAUD, UART_PARITY_NONE, UART_MODE_TX_RX);

	CLEAR_BIT(USART2->SR, USART_SR_TC);   /* 清除TC发送完成标志 */
    CLEAR_BIT(USART2->SR, USART_SR_RXNE); /* 清除RXNE接收标志 */
	// USART_CR1_PEIE | USART_CR1_RXNEIE
	SET_BIT(USART2->CR1, USART_CR1_RXNEIE);	/* 使能PE. RX接受中断 */
#endif
}

/*
*
*串口 FIFO 初始化
*
*/
 void bsp_inituart(void)
{
	uartvarinit();		/* 必须先初始化全局变量,再配置硬件 */

	initharduart();		/* 配置串口的硬件参数(波特率等) */

}
/*
*********************************************************************************************************
*	函 数 名: ComToUart
*	功能说明: 将COM端口号转换为UART指针
*	形    参: _ucPort: 端口号(COM1 - COM8)
*	返 回 值: uart指针
*********************************************************************************************************
*/
UART_T *comtouart(COM_PORT_E _ucPort)
{
	switch (_ucPort)
	{
		case COM1:
			#if UART1_FIFO_EN
				return &g_tUart1;
			#else
				return 0;
			#endif
		case COM2:
			#if UART2_FIFO_EN
				return &g_tUart2;
			#else
				return 0;
			#endif
		case COM3:
			#if UART3_FIFO_EN
				return &g_tUart3;
			#else
				return 0;
			#endif
		case COM4:
			#if UART4_FIFO_EN
				return &g_tUart4;
			#else
				return 0;
			#endif
		case COM5:
			#if UART5_FIFO_EN
				return &g_tUart5;
			#else
				return 0;
			#endif
		case COM6:
			#if UART6_FIFO_EN
				return &g_tUart6;
			#else
				return 0;
			#endif
		case COM7:
			#if UART7_FIFO_EN
				return &g_tUart7;
			#else
				return 0;
			#endif
		case COM8:
			#if UART8_FIFO_EN
				return &g_tUart8;
			#else
				return 0;
			#endif
		default:
			_Error_Handler(__FILE__, __LINE__);
			return 0;
	}
}
/*
*********************************************************************************************************
*	函 数 名: ComToUart
*	功能说明: 将COM端口号转换为 USART_TypeDef* USARTx
*	形    参: _ucPort: 端口号(COM1 - COM8)
*	返 回 值: USART_TypeDef*,  USART1, USART2, USART3, UART4, UART5，USART6，UART7，UART8。
*********************************************************************************************************
*/
USART_TypeDef *comtoUSARTx(COM_PORT_E _ucPort)
{
	if (_ucPort == COM1)
	{
		#if UART1_FIFO_EN
			return USART1;
		#else
			return 0;
		#endif
	}
	else if (_ucPort == COM2)
	{
		#if UART2_FIFO_EN
			return USART2;
		#else
			return 0;
		#endif
	}
	else if (_ucPort == COM3)
	{
		#if UART3_FIFO_EN == 1
			return USART3;
		#else
			return 0;
		#endif
	}
	else if (_ucPort == COM4)
	{
		#if UART4_FIFO_EN
			return UART4;
		#else
			return 0;
		#endif
	}
	else if (_ucPort == COM5)
	{
		#if UART5_FIFO_EN
			return UART5;
		#else
			return 0;
		#endif
	}
	else if (_ucPort == COM6)
	{
		#if UART6_FIFO_EN
			return USART6;
		#else
			return 0;
		#endif
	}
	else if (_ucPort == COM7)
	{
		#if UART7_FIFO_EN
			return UART7;
		#else
			return 0;
		#endif
	}
	else if (_ucPort == COM8)
	{
		#if UART8_FIFO_EN
			return UART8;
		#else
			return 0;
		#endif
	}	
	
	else
	{
		/* 不做任何处理 */
		return 0;
	}
}
/*
*********************************************************************************************************
*	函 数 名: comClearTxFifo
*	功能说明: 清零串口发送缓冲区
*	形    参: _ucPort: 端口号(COM1 - COM8)
*	返 回 值: 无
*********************************************************************************************************
*/
void comcleartxfifo(COM_PORT_E _ucPort)
{
	UART_T *pUart;

	pUart = comtouart(_ucPort);
	if (pUart == 0)
	{
		return;
	}

	pUart->ustxwrite = 0;
	pUart->ustxread = 0;
	pUart->ustxcount = 0;
}
/*
*********************************************************************************************************
*	函 数 名: comClearRxFifo
*	功能说明: 清零串口接收缓冲区
*	形    参: _ucPort: 端口号(COM1 - COM8)
*	返 回 值: 无
*********************************************************************************************************
*/
void comclearrxfifo(COM_PORT_E _ucPort)
{
	UART_T *pUart;

	pUart = comtouart(_ucPort);
	if (pUart == 0)
	{
		return;
	}

	pUart->usrxwrite = 0;
	pUart->usrxread = 0;
	pUart->usrxcount = 0;
}
/*
*********************************************************************************************************
*	函 数 名: comSetBaud
*	功能说明: 设置串口的波特率. 本函数固定设置为无校验，收发都使能模式
*	形    参: _ucPort: 端口号(COM1 - COM8)
*			  _BaudRate: 波特率，8倍过采样  波特率.0-12.5Mbps
*                                16倍过采样 波特率.0-6.25Mbps
*	返 回 值: 无
*********************************************************************************************************
*/
void comsetbaud(COM_PORT_E _ucPort, uint32_t _BaudRate)
{
	USART_TypeDef* USARTx;
	
	USARTx = comtoUSARTx(_ucPort);
	if (USARTx == 0)
	{
		return;
	}
	
	bsp_setuartparam(USARTx,_BaudRate,UART_PARITY_NONE,UART_MODE_TX_RX);
}

/*
*
*串口数据接收
*
*/
/*
*********************************************************************************************************
*	函 数 名: UartSend
*	功能说明: 填写数据到UART发送缓冲区,并启动发送中断。中断处理函数发送完毕后，自动关闭发送中断
*	形    参: 无
*	返 回 值: 无
*********************************************************************************************************
*/
static void uartsend(UART_T *_pUart, uint8_t *_ucaBuf, uint16_t _usLen)
{
	uint16_t i;

	for (i = 0; i < _usLen; i++)
	{
		/* 如果发送缓冲区已经满了，则等待缓冲区空 */
		while (1)
		{
			__IO uint16_t usCount;

			DISABLE_INT();
			usCount = _pUart->ustxcount;
			ENABLE_INT();

			if (usCount < _pUart->ustxbufsize)
			{
				break;
			}
			else if(usCount == _pUart->ustxbufsize)/* 数据已填满缓冲区 */
			{
				if((_pUart->uart->CR1 & USART_CR1_TXEIE) == 0)
				{
					SET_BIT(_pUart->uart->CR1, USART_CR1_TXEIE);
				}  
			}
		}

		/* 将新数据填入发送缓冲区 */
		_pUart->ptxbuf[_pUart->ustxwrite] = _ucaBuf[i];

		DISABLE_INT();
		if (++_pUart->ustxwrite >= _pUart->ustxbufsize)
		{
			_pUart->ustxwrite = 0;
		}
		_pUart->ustxcount++;
		ENABLE_INT();
	}

	SET_BIT(_pUart->uart->CR1, USART_CR1_TXEIE);	/* 使能发送中断（缓冲区空） */
}
/*
*********************************************************************************************************
*	函 数 名: comSendBuf
*	功能说明: 向串口发送一组数据。数据放到发送缓冲区后立即返回，由中断服务程序在后台完成发送
*	形    参: _ucPort: 端口号(COM1 - COM8)
*			  _ucaBuf: 待发送的数据缓冲区
*			  _usLen : 数据长度
*	返 回 值: 无
*********************************************************************************************************
*/
void comsendbuf(COM_PORT_E _ucPort, uint8_t *_ucaBuf, uint16_t _usLen)
{
	UART_T *pUart;

	pUart = comtouart(_ucPort);
	if (pUart == 0)
	{
		return;
	}

	if (pUart->sendbefor != 0)
	{
		pUart->sendbefor();		/* 如果是RS485通信，可以在这个函数中将RS485设置为发送模式 */
	}

	uartsend(pUart, _ucaBuf, _usLen);
}
/*
*********************************************************************************************************
*	函 数 名: comSendChar
*	功能说明: 向串口发送1个字节。数据放到发送缓冲区后立即返回，由中断服务程序在后台完成发送
*	形    参: _ucPort: 端口号(COM1 - COM8)
*			  _ucByte: 待发送的数据
*	返 回 值: 无
*********************************************************************************************************
*/
void comsendchar(COM_PORT_E _ucPort, uint8_t _ucByte)
{
	comsendbuf(_ucPort, &_ucByte, 1);
}

/*
*
*串口数据接收
*
*/
/*
*********************************************************************************************************
*   函 数 名: UartTxEmpty
*   功能说明: 判断发送缓冲区是否为空。
*   形    参:  _pUart : 串口设备
*   返 回 值: 1为空。0为不空。
*********************************************************************************************************
*/
uint8_t uarttxempty(COM_PORT_E _ucPort)
{
   UART_T *pUart;
   uint8_t Sending;
   
   pUart = comtouart(_ucPort);
   if (pUart == 0)
   {
      return 0;
   }

   Sending = pUart->sending;

   if (Sending != 0)
   {
      return 0;
   }
   return 1;
}
/*
*********************************************************************************************************
*	函 数 名: UartGetChar
*	功能说明: 从串口接收缓冲区读取1字节数据 （用于主程序调用）
*	形    参: _pUart : 串口设备
*			  _pByte : 存放读取数据的指针
*	返 回 值: 0 表示无数据  1表示读取到数据
*********************************************************************************************************
*/
static uint8_t uartgetchar(UART_T *_pUart, uint8_t *_pByte)
{
	uint16_t usCount;

	/* usRxWrite 变量在中断函数中被改写，主程序读取该变量时，必须进行临界区保护 */
	DISABLE_INT();
	usCount = _pUart->usrxcount;
	ENABLE_INT();

	/* 如果读和写索引相同，则返回0 */
	//if (_pUart->usRxRead == usRxWrite)
	if (usCount == 0)	/* 已经没有数据 */
	{
		return 0;
	}
	else
	{
		*_pByte = _pUart->prxbuf[_pUart->usrxread];		/* 从串口接收FIFO取1个数据 */

		/* 改写FIFO读索引 */
		DISABLE_INT();
		if (++_pUart->usrxread >= _pUart->usrxbufsize)
		{
			_pUart->usrxread = 0;
		}
		_pUart->usrxcount--;
		ENABLE_INT();
		return 1;
	}
}
/*
*********************************************************************************************************
*	函 数 名: comgetchar
*	功能说明: 从接收缓冲区读取1字节，非阻塞。无论有无数据均立即返回。
*	形    参: _ucPort: 端口号(COM1 - COM8)
*			  _pByte: 接收到的数据存放在这个地址
*	返 回 值: 0 表示无数据, 1 表示读取到有效字节
*********************************************************************************************************
*/
uint8_t comgetchar(COM_PORT_E _ucPort, uint8_t *_pByte)
{
	UART_T *pUart;

	pUart = comtouart(_ucPort);
	if (pUart == 0)
	{
		return 0;
	}

	return uartgetchar(pUart, _pByte);
}
/*
*********************************************************************************************************
*	函 数 名: UartIRQ
*	功能说明: 供中断服务程序调用，通用串口中断处理函数
*	形    参: _pUart : 串口设备
*	返 回 值: 无
*********************************************************************************************************
*/
static void UartIRQ(UART_T *_pUart)
{
	uint32_t isrflags   = READ_REG(_pUart->uart->SR);
	uint32_t cr1its     = READ_REG(_pUart->uart->CR1);
	uint32_t cr3its     = READ_REG(_pUart->uart->CR3);
	
	/* 处理接收中断  */
	if ((isrflags & USART_SR_RXNE) != RESET)
	{
		/* 从串口接收数据寄存器读取数据存放到接收FIFO */
		uint8_t ch;

		ch = READ_REG(_pUart->uart->DR);			/* 读取串口接收寄存器 */
		_pUart->prxbuf[_pUart->usrxwrite] = ch;		/* 填入串口接收FIFO */
		if (++_pUart->usrxwrite >= _pUart->usrxbufsize)/* 接收 FIFO 的写指针+1 */
		{
			_pUart->usrxwrite = 0;
		}
		if (_pUart->usrxcount < _pUart->usrxbufsize)/* 统计未处理的字节个数 */
		{
			_pUart->usrxcount++;
		}

		/* 回调函数,通知应用程序收到新数据,一般是发送1个消息或者设置一个标记 */
		if (_pUart->recivenew)
		{
			_pUart->recivenew(ch); /* 比如，交给MODBUS解码程序处理字节流 */
		}
	}

	/* 处理发送缓冲区空中断 */
	if ( ((isrflags & USART_SR_TXE) != RESET) && (cr1its & USART_CR1_TXEIE) != RESET)
	{
		if (_pUart->ustxcount == 0)
		{
			/* 发送缓冲区的数据已取完时， 禁止发送缓冲区空中断 （注意：此时最后1个数据还未真正发送完毕）*/
			//USART_ITConfig(_pUart->uart, USART_IT_TXE, DISABLE);
			CLEAR_BIT(_pUart->uart->CR1, USART_CR1_TXEIE);

			/* 使能数据发送完毕中断 */
			//USART_ITConfig(_pUart->uart, USART_IT_TC, ENABLE);
			SET_BIT(_pUart->uart->CR1, USART_CR1_TCIE);
		}
		else
		{
			_pUart->sending = 1;
			
			/* 从发送FIFO取1个字节写入串口发送数据寄存器 */
			//USART_SendData(_pUart->uart, _pUart->pTxBuf[_pUart->usTxRead]);
			_pUart->uart->DR = _pUart->ptxbuf[_pUart->ustxread];
			if (++_pUart->ustxread >= _pUart->ustxbufsize)
			{
				_pUart->ustxread = 0;
			}
			_pUart->ustxcount--;
		}

	}
	/* 数据bit位全部发送完毕的中断 */
	if (((isrflags & USART_SR_TC) != RESET) && ((cr1its & USART_CR1_TCIE) != RESET))
	{
		//if (_pUart->usTxRead == _pUart->usTxWrite)
		if (_pUart->ustxcount == 0)
		{
			/* 如果发送FIFO的数据全部发送完毕，禁止数据发送完毕中断 */
			//USART_ITConfig(_pUart->uart, USART_IT_TC, DISABLE);
			CLEAR_BIT(_pUart->uart->CR1, USART_CR1_TCIE);

			/* 回调函数, 一般用来处理RS485通信，将RS485芯片设置为接收模式，避免抢占总线 */
			if (_pUart->sendover)
			{
				_pUart->sendover();
			}
			
			_pUart->sending = 0;
		}
		else
		{
			/* 正常情况下，不会进入此分支 */

			/* 如果发送FIFO的数据还未完毕，则从发送FIFO取1个数据写入发送数据寄存器 */
			//USART_SendData(_pUart->uart, _pUart->pTxBuf[_pUart->usTxRead]);
			_pUart->uart->DR = _pUart->ptxbuf[_pUart->ustxread];
			if (++_pUart->ustxread >= _pUart->ustxbufsize)
			{
				_pUart->ustxread = 0;
			}
			_pUart->ustxcount--;
		}
	}
}
/*
*********************************************************************************************************
*	函 数 名: USART1_IRQHandler  USART2_IRQHandler USART3_IRQHandler UART4_IRQHandler UART5_IRQHandler等
*	功能说明: USART中断服务程序
*	形    参: 无
*	返 回 值: 无
*********************************************************************************************************
*/
#if UART1_FIFO_EN 
void USART1_IRQHandler(void)
{
	UartIRQ(&g_tUart1);
}
#endif
#if UART2_FIFO_EN 
void USART2_IRQHandler(void)
{
	UartIRQ(&g_tUart2);
}
#endif

/*
*********************************************************************************************************
*	函 数 名: fputc
*	功能说明: 重定义putc函数，这样可以使用printf函数从串口1打印输出
*	形    参: 无
*	返 回 值: 无
*********************************************************************************************************
*/
int fputc(int ch, FILE *f)
{
#if 1	/* 将需要printf的字符通过串口中断FIFO发送出去，printf函数会立即返回 */
	comsendchar(COM1, ch);
	
	return ch;
#else	/* 采用阻塞方式发送每个字符,等待数据发送完毕 */
	/* 写一个字节到USART1 */
	USART1->DR = ch;
	
	/* 等待发送结束 */
	while((USART1->SR & USART_SR_TC) == 0)
	{}
	
	return ch;
#endif
}

/*
*********************************************************************************************************
*	函 数 名: fgetc
*	功能说明: 重定义getc函数，这样可以使用getchar函数从串口1输入数据
*	形    参: 无
*	返 回 值: 无
*********************************************************************************************************
*/
int fgetc(FILE *f)
{

#if 1	/* 从串口接收FIFO中取1个数据, 只有取到数据才返回 */
	uint8_t ucData;

	while(comgetchar(COM1, &ucData) == 0);

	return ucData;
#else
	/* 等待接收到数据 */
	while((USART1->SR & USART_SR_RXNE) == 0)
	{}

	return (int)USART1->DR;
#endif
}




