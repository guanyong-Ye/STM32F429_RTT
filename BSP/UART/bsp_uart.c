#include "bsp.h"


/*
*********************************************************************************************************
*
*	ģ������ : �����ж�+FIFO����ģ��
*	�ļ����� : bsp_uart_fifo.c
*	��    �� : V1.8
*	˵    �� : ���ô����ж�+FIFOģʽʵ�ֶ�����ڵ�ͬʱ����
*
*********************************************************************************************************
*/

/* ����ÿ�����ڽṹ����� */
#if UART1_FIFO_EN
	static UART_T g_tUart1;
	static uint8_t g_TxBuf1[UART1_TX_BUF_SIZE];		/* ���ͻ����� */
	static uint8_t g_RxBuf1[UART1_RX_BUF_SIZE];		/* ���ջ����� */
#endif

#if UART2_FIFO_EN
	static UART_T g_tUart2;
	static uint8_t g_TxBuf2[UART2_TX_BUF_SIZE];		/* ���ͻ����� */
	static uint8_t g_RxBuf2[UART2_RX_BUF_SIZE];		/* ���ջ����� */
#endif
/*
*********************************************************************************************************
*	�� �� ��: UartVarInit
*	����˵��: ��ʼ��������صı���
*	��    ��: ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
static void uartvarinit(void)
{
#if UART1_FIFO_EN
	g_tUart1.uart 	= USART1;						/* STM32 �����豸 */
	g_tUart1.ptxbuf = g_TxBuf1;					/* ���ͻ�����ָ�� */
	g_tUart1.prxbuf = g_RxBuf1;					/* ���ջ�����ָ�� */
	g_tUart1.ustxbufsize = UART1_TX_BUF_SIZE;	/* ���ͻ�������С */
	g_tUart1.usrxbufsize = UART1_RX_BUF_SIZE;	/* ���ջ�������С */
	g_tUart1.ustxwrite 	= 0;						/* ����FIFOд���� */
	g_tUart1.ustxread 	= 0;						/* ����FIFO������ */
	g_tUart1.usrxwrite 	= 0;						/* ����FIFOд���� */
	g_tUart1.usrxread 	= 0;						/* ����FIFO������ */
	g_tUart1.usrxcount 	= 0;						/* ���յ��������ݸ��� */
	g_tUart1.ustxcount 	= 0;						/* �����͵����ݸ��� */
	g_tUart1.sendbefor 	= 0;						/* ��������ǰ�Ļص����� */
	g_tUart1.sendover 	= 0;						/* ������Ϻ�Ļص����� */
	g_tUart1.recivenew 	= 0;						/* ���յ������ݺ�Ļص����� */
	g_tUart1.sending 	= 0;						/* ���ڷ����б�־ */
	g_tUart1.read_len = 0;
#endif
#if UART1_FIFO_EN
	g_tUart2.uart 	= USART2;						/* STM32 �����豸 */
	g_tUart2.ptxbuf = g_TxBuf2;					/* ���ͻ�����ָ�� */
	g_tUart2.prxbuf = g_RxBuf2;					/* ���ջ�����ָ�� */
	g_tUart2.ustxbufsize = UART2_TX_BUF_SIZE;	/* ���ͻ�������С */
	g_tUart2.usrxbufsize = UART2_RX_BUF_SIZE;	/* ���ջ�������С */
	g_tUart2.ustxwrite 	= 0;						/* ����FIFOд���� */
	g_tUart2.ustxread 	= 0;						/* ����FIFO������ */
	g_tUart2.usrxwrite 	= 0;						/* ����FIFOд���� */
	g_tUart2.usrxread 	= 0;						/* ����FIFO������ */
	g_tUart2.usrxcount 	= 0;						/* ���յ��������ݸ��� */
	g_tUart2.ustxcount 	= 0;						/* �����͵����ݸ��� */
	g_tUart2.sendbefor 	= 0;						/* ��������ǰ�Ļص����� */
	g_tUart2.sendover 	= 0;						/* ������Ϻ�Ļص����� */
	g_tUart2.recivenew 	= 0;						/* ���յ������ݺ�Ļص����� */
	g_tUart2.sending 	= 0;						/* ���ڷ����б�־ */
#endif
}
/*
*********************************************************************************************************
*	�� �� ��: bsp_setuartparam
*	����˵��: ���ô��ڵ�Ӳ�������������ʣ�����λ��ֹͣλ����ʼλ��У��λ���ж�ʹ�ܣ��ʺ���STM32- H7������
*	��    ��: Instance   USART_TypeDef���ͽṹ��
*             BaudRate   ������
*             Parity     У�����ͣ���У�����żУ��
*             Mode       ���ͺͽ���ģʽʹ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void bsp_setuartparam(USART_TypeDef *Instance,  uint32_t BaudRate, uint32_t Parity, uint32_t Mode)
{
	UART_HandleTypeDef UartHandle;
	
	/*##-1- ���ô���Ӳ������ ######################################*/
	/* �첽����ģʽ (UART Mode) */
	/* ��������:
	  - �ֳ�    = 8 λ
	  - ֹͣλ  = 1 ��ֹͣλ
	  - У��    = ����Parity
	  - ������  = ����BaudRate
	  - Ӳ�������ƹر� (RTS and CTS signals) */

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
*	�� �� ��: InitHardUart
*	����˵��: ���ô��ڵ�Ӳ�������������ʣ�����λ��ֹͣλ����ʼλ��У��λ���ж�ʹ�ܣ��ʺ���STM32-H7������
*	��    ��: ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
static void initharduart(void)
{
	GPIO_InitTypeDef  GPIO_InitStruct;


#if UART1_FIFO_EN		/* ����1 */
	/* ʹ�� GPIO TX/RX ʱ�� */
	USART1_TX_GPIO_CLK_ENABLE();
	USART1_RX_GPIO_CLK_ENABLE();
	
	/* ʹ�� USARTx ʱ�� */
	USART1_CLK_ENABLE();	

	/* ����TX���� */
	GPIO_InitStruct.Pin       = USART1_TX_PIN;
	GPIO_InitStruct.Mode      = GPIO_MODE_AF_PP;
	GPIO_InitStruct.Pull      = GPIO_PULLUP;
	GPIO_InitStruct.Speed     = GPIO_SPEED_FREQ_VERY_HIGH;
	GPIO_InitStruct.Alternate = USART1_TX_AF;
	HAL_GPIO_Init(USART1_TX_GPIO_PORT, &GPIO_InitStruct);	
	
	/* ����RX���� */
	GPIO_InitStruct.Pin = USART1_RX_PIN;
	GPIO_InitStruct.Alternate = USART1_RX_AF;
	HAL_GPIO_Init(USART1_RX_GPIO_PORT, &GPIO_InitStruct);

	/* ����NVIC the NVIC for UART */   
	HAL_NVIC_SetPriority(USART1_IRQn, 0, 1);
	HAL_NVIC_EnableIRQ(USART1_IRQn);
  
	/* ���ò����ʡ���żУ�� */
	bsp_setuartparam(USART1,  UART1_BAUD, UART_PARITY_NONE, UART_MODE_TX_RX);

	CLEAR_BIT(USART1->SR, USART_SR_TC);   /* ���TC������ɱ�־ */
    CLEAR_BIT(USART1->SR, USART_SR_RXNE); /* ���RXNE���ձ�־ */
	// USART_CR1_PEIE | USART_CR1_RXNEIE
	SET_BIT(USART1->CR1, USART_CR1_RXNEIE);	/* ʹ��PE. RX�����ж� */
#endif
#if UART1_FIFO_EN		/* ����2 */
	/* ʹ�� GPIO TX/RX ʱ�� */
	USART2_TX_GPIO_CLK_ENABLE();
	USART2_RX_GPIO_CLK_ENABLE();
	
	/* ʹ�� USARTx ʱ�� */
	USART2_CLK_ENABLE();	

	/* ����TX���� */
	GPIO_InitStruct.Pin       = USART2_TX_PIN;
	GPIO_InitStruct.Mode      = GPIO_MODE_AF_PP;
	GPIO_InitStruct.Pull      = GPIO_PULLUP;
	GPIO_InitStruct.Speed     = GPIO_SPEED_FREQ_VERY_HIGH;
	GPIO_InitStruct.Alternate = USART2_TX_AF;
	HAL_GPIO_Init(USART2_TX_GPIO_PORT, &GPIO_InitStruct);	
	
	/* ����RX���� */
	GPIO_InitStruct.Pin = USART2_RX_PIN;
	GPIO_InitStruct.Alternate = USART2_RX_AF;
	HAL_GPIO_Init(USART2_RX_GPIO_PORT, &GPIO_InitStruct);

	/* ����NVIC the NVIC for UART */   
	HAL_NVIC_SetPriority(USART2_IRQn, 0, 1);
	HAL_NVIC_EnableIRQ(USART2_IRQn);
  
	/* ���ò����ʡ���żУ�� */
	bsp_setuartparam(USART2,  UART2_BAUD, UART_PARITY_NONE, UART_MODE_TX_RX);

	CLEAR_BIT(USART2->SR, USART_SR_TC);   /* ���TC������ɱ�־ */
    CLEAR_BIT(USART2->SR, USART_SR_RXNE); /* ���RXNE���ձ�־ */
	// USART_CR1_PEIE | USART_CR1_RXNEIE
	SET_BIT(USART2->CR1, USART_CR1_RXNEIE);	/* ʹ��PE. RX�����ж� */
#endif
}

/*
*
*���� FIFO ��ʼ��
*
*/
 void bsp_inituart(void)
{
	uartvarinit();		/* �����ȳ�ʼ��ȫ�ֱ���,������Ӳ�� */

	initharduart();		/* ���ô��ڵ�Ӳ������(�����ʵ�) */

}
/*
*********************************************************************************************************
*	�� �� ��: ComToUart
*	����˵��: ��COM�˿ں�ת��ΪUARTָ��
*	��    ��: _ucPort: �˿ں�(COM1 - COM8)
*	�� �� ֵ: uartָ��
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
*	�� �� ��: ComToUart
*	����˵��: ��COM�˿ں�ת��Ϊ USART_TypeDef* USARTx
*	��    ��: _ucPort: �˿ں�(COM1 - COM8)
*	�� �� ֵ: USART_TypeDef*,  USART1, USART2, USART3, UART4, UART5��USART6��UART7��UART8��
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
		/* �����κδ��� */
		return 0;
	}
}
/*
*********************************************************************************************************
*	�� �� ��: comClearTxFifo
*	����˵��: ���㴮�ڷ��ͻ�����
*	��    ��: _ucPort: �˿ں�(COM1 - COM8)
*	�� �� ֵ: ��
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
*	�� �� ��: comClearRxFifo
*	����˵��: ���㴮�ڽ��ջ�����
*	��    ��: _ucPort: �˿ں�(COM1 - COM8)
*	�� �� ֵ: ��
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
*	�� �� ��: comSetBaud
*	����˵��: ���ô��ڵĲ�����. �������̶�����Ϊ��У�飬�շ���ʹ��ģʽ
*	��    ��: _ucPort: �˿ں�(COM1 - COM8)
*			  _BaudRate: �����ʣ�8��������  ������.0-12.5Mbps
*                                16�������� ������.0-6.25Mbps
*	�� �� ֵ: ��
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
*�������ݽ���
*
*/
/*
*********************************************************************************************************
*	�� �� ��: UartSend
*	����˵��: ��д���ݵ�UART���ͻ�����,�����������жϡ��жϴ�����������Ϻ��Զ��رշ����ж�
*	��    ��: ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
static void uartsend(UART_T *_pUart, uint8_t *_ucaBuf, uint16_t _usLen)
{
	uint16_t i;

	for (i = 0; i < _usLen; i++)
	{
		/* ������ͻ������Ѿ����ˣ���ȴ��������� */
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
			else if(usCount == _pUart->ustxbufsize)/* ���������������� */
			{
				if((_pUart->uart->CR1 & USART_CR1_TXEIE) == 0)
				{
					SET_BIT(_pUart->uart->CR1, USART_CR1_TXEIE);
				}  
			}
		}

		/* �����������뷢�ͻ����� */
		_pUart->ptxbuf[_pUart->ustxwrite] = _ucaBuf[i];

		DISABLE_INT();
		if (++_pUart->ustxwrite >= _pUart->ustxbufsize)
		{
			_pUart->ustxwrite = 0;
		}
		_pUart->ustxcount++;
		ENABLE_INT();
	}

	SET_BIT(_pUart->uart->CR1, USART_CR1_TXEIE);	/* ʹ�ܷ����жϣ��������գ� */
}
/*
*********************************************************************************************************
*	�� �� ��: comSendBuf
*	����˵��: �򴮿ڷ���һ�����ݡ����ݷŵ����ͻ��������������أ����жϷ�������ں�̨��ɷ���
*	��    ��: _ucPort: �˿ں�(COM1 - COM8)
*			  _ucaBuf: �����͵����ݻ�����
*			  _usLen : ���ݳ���
*	�� �� ֵ: ��
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
		pUart->sendbefor();		/* �����RS485ͨ�ţ���������������н�RS485����Ϊ����ģʽ */
	}

	uartsend(pUart, _ucaBuf, _usLen);
}
/*
*********************************************************************************************************
*	�� �� ��: comSendChar
*	����˵��: �򴮿ڷ���1���ֽڡ����ݷŵ����ͻ��������������أ����жϷ�������ں�̨��ɷ���
*	��    ��: _ucPort: �˿ں�(COM1 - COM8)
*			  _ucByte: �����͵�����
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void comsendchar(COM_PORT_E _ucPort, uint8_t _ucByte)
{
	comsendbuf(_ucPort, &_ucByte, 1);
}

/*
*
*�������ݽ���
*
*/
/*
*********************************************************************************************************
*   �� �� ��: UartTxEmpty
*   ����˵��: �жϷ��ͻ������Ƿ�Ϊ�ա�
*   ��    ��:  _pUart : �����豸
*   �� �� ֵ: 1Ϊ�ա�0Ϊ���ա�
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
*	�� �� ��: UartGetChar
*	����˵��: �Ӵ��ڽ��ջ�������ȡ1�ֽ����� ��������������ã�
*	��    ��: _pUart : �����豸
*			  _pByte : ��Ŷ�ȡ���ݵ�ָ��
*	�� �� ֵ: 0 ��ʾ������  1��ʾ��ȡ������
*********************************************************************************************************
*/
static uint8_t uartgetchar(UART_T *_pUart, uint8_t *_pByte)
{
	uint16_t usCount;

	/* usRxWrite �������жϺ����б���д���������ȡ�ñ���ʱ����������ٽ������� */
	DISABLE_INT();
	usCount = _pUart->usrxcount;
	ENABLE_INT();

	/* �������д������ͬ���򷵻�0 */
	//if (_pUart->usRxRead == usRxWrite)
	if (usCount == 0)	/* �Ѿ�û������ */
	{
		return 0;
	}
	else
	{
		*_pByte = _pUart->prxbuf[_pUart->usrxread];		/* �Ӵ��ڽ���FIFOȡ1������ */

		/* ��дFIFO������ */
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
*	�� �� ��: comgetchar
*	����˵��: �ӽ��ջ�������ȡ1�ֽڣ��������������������ݾ��������ء�
*	��    ��: _ucPort: �˿ں�(COM1 - COM8)
*			  _pByte: ���յ������ݴ���������ַ
*	�� �� ֵ: 0 ��ʾ������, 1 ��ʾ��ȡ����Ч�ֽ�
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
*	�� �� ��: UartIRQ
*	����˵��: ���жϷ��������ã�ͨ�ô����жϴ�����
*	��    ��: _pUart : �����豸
*	�� �� ֵ: ��
*********************************************************************************************************
*/
static void UartIRQ(UART_T *_pUart)
{
	uint32_t isrflags   = READ_REG(_pUart->uart->SR);
	uint32_t cr1its     = READ_REG(_pUart->uart->CR1);
	uint32_t cr3its     = READ_REG(_pUart->uart->CR3);
	
	/* ��������ж�  */
	if ((isrflags & USART_SR_RXNE) != RESET)
	{
		/* �Ӵ��ڽ������ݼĴ�����ȡ���ݴ�ŵ�����FIFO */
		uint8_t ch;

		ch = READ_REG(_pUart->uart->DR);			/* ��ȡ���ڽ��ռĴ��� */
		_pUart->prxbuf[_pUart->usrxwrite] = ch;		/* ���봮�ڽ���FIFO */
		if (++_pUart->usrxwrite >= _pUart->usrxbufsize)/* ���� FIFO ��дָ��+1 */
		{
			_pUart->usrxwrite = 0;
		}
		if (_pUart->usrxcount < _pUart->usrxbufsize)/* ͳ��δ������ֽڸ��� */
		{
			_pUart->usrxcount++;
		}

		/* �ص�����,֪ͨӦ�ó����յ�������,һ���Ƿ���1����Ϣ��������һ����� */
		if (_pUart->recivenew)
		{
			_pUart->recivenew(ch); /* ���磬����MODBUS����������ֽ��� */
		}
	}

	/* �����ͻ��������ж� */
	if ( ((isrflags & USART_SR_TXE) != RESET) && (cr1its & USART_CR1_TXEIE) != RESET)
	{
		if (_pUart->ustxcount == 0)
		{
			/* ���ͻ�������������ȡ��ʱ�� ��ֹ���ͻ��������ж� ��ע�⣺��ʱ���1�����ݻ�δ����������ϣ�*/
			//USART_ITConfig(_pUart->uart, USART_IT_TXE, DISABLE);
			CLEAR_BIT(_pUart->uart->CR1, USART_CR1_TXEIE);

			/* ʹ�����ݷ�������ж� */
			//USART_ITConfig(_pUart->uart, USART_IT_TC, ENABLE);
			SET_BIT(_pUart->uart->CR1, USART_CR1_TCIE);
		}
		else
		{
			_pUart->sending = 1;
			
			/* �ӷ���FIFOȡ1���ֽ�д�봮�ڷ������ݼĴ��� */
			//USART_SendData(_pUart->uart, _pUart->pTxBuf[_pUart->usTxRead]);
			_pUart->uart->DR = _pUart->ptxbuf[_pUart->ustxread];
			if (++_pUart->ustxread >= _pUart->ustxbufsize)
			{
				_pUart->ustxread = 0;
			}
			_pUart->ustxcount--;
		}

	}
	/* ����bitλȫ��������ϵ��ж� */
	if (((isrflags & USART_SR_TC) != RESET) && ((cr1its & USART_CR1_TCIE) != RESET))
	{
		//if (_pUart->usTxRead == _pUart->usTxWrite)
		if (_pUart->ustxcount == 0)
		{
			/* �������FIFO������ȫ��������ϣ���ֹ���ݷ�������ж� */
			//USART_ITConfig(_pUart->uart, USART_IT_TC, DISABLE);
			CLEAR_BIT(_pUart->uart->CR1, USART_CR1_TCIE);

			/* �ص�����, һ����������RS485ͨ�ţ���RS485оƬ����Ϊ����ģʽ��������ռ���� */
			if (_pUart->sendover)
			{
				_pUart->sendover();
			}
			
			_pUart->sending = 0;
		}
		else
		{
			/* ��������£��������˷�֧ */

			/* �������FIFO�����ݻ�δ��ϣ���ӷ���FIFOȡ1������д�뷢�����ݼĴ��� */
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
*	�� �� ��: USART1_IRQHandler  USART2_IRQHandler USART3_IRQHandler UART4_IRQHandler UART5_IRQHandler��
*	����˵��: USART�жϷ������
*	��    ��: ��
*	�� �� ֵ: ��
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
*	�� �� ��: fputc
*	����˵��: �ض���putc��������������ʹ��printf�����Ӵ���1��ӡ���
*	��    ��: ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
int fputc(int ch, FILE *f)
{
#if 1	/* ����Ҫprintf���ַ�ͨ�������ж�FIFO���ͳ�ȥ��printf�������������� */
	comsendchar(COM1, ch);
	
	return ch;
#else	/* ����������ʽ����ÿ���ַ�,�ȴ����ݷ������ */
	/* дһ���ֽڵ�USART1 */
	USART1->DR = ch;
	
	/* �ȴ����ͽ��� */
	while((USART1->SR & USART_SR_TC) == 0)
	{}
	
	return ch;
#endif
}

/*
*********************************************************************************************************
*	�� �� ��: fgetc
*	����˵��: �ض���getc��������������ʹ��getchar�����Ӵ���1��������
*	��    ��: ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
int fgetc(FILE *f)
{

#if 1	/* �Ӵ��ڽ���FIFO��ȡ1������, ֻ��ȡ�����ݲŷ��� */
	uint8_t ucData;

	while(comgetchar(COM1, &ucData) == 0);

	return ucData;
#else
	/* �ȴ����յ����� */
	while((USART1->SR & USART_SR_RXNE) == 0)
	{}

	return (int)USART1->DR;
#endif
}




