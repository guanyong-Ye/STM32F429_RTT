/*
*********************************************************************************************************
*
*	模块名称 : SPI接口串行FLASH 读写模块
*	文件名称 : bsp_spi_flash.c
*
*********************************************************************************************************
*/

#include "bsp.h"


flash_t g_tsf;

static void sf_writeenable(void);
static void sf_writestatus(uint8_t _ucValue);
static void sf_waitforwriteend(void);
static uint8_t sf_neederase(uint8_t * _ucpOldBuf, uint8_t *_ucpNewBuf, uint16_t _uiLen);
static uint8_t sf_cmpdata(uint32_t _uiSrcAddr, uint8_t *_ucpTar, uint32_t _uiSize);
static uint8_t sf_autowritepage(uint8_t *_ucpSrc, uint32_t _uiWrAddr, uint16_t _usWrLen);
static uint8_t s_spibuf[4*1024];	/* 用于写函数，先读出整个page，修改缓冲区后，再整个page回写 */

//static void sf_CfgSpiHard(void);
static void sf_configgpio(void);
static void sf_setcs(uint8_t _level);

/*
*********************************************************************************************************
*	函 数 名: sf_configgpio
*	功能说明: 配置串行Flash的片选GPIO。 设置为推挽输出
*	形    参: 无
*	返 回 值: 无
*********************************************************************************************************
*/
static void sf_configgpio(void)
{
	/*
		安富莱STM32-V5 开发板口线分配：  串行Flash型号为 W25Q64BVSSIG (80MHz)
		PF8/SF_CS
	*/
	GPIO_InitTypeDef GPIO_InitStructure;

	/* 使能GPIO 时钟 */
	SPI_CS_PORT_ENABLE() ;

	/* 配置片选口线为推挽输出模式 */
	sf_setcs(1);		/* 片选置高，不选中 */
	
	GPIO_InitStructure.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStructure.Speed = GPIO_SPEED_FREQ_HIGH;
	GPIO_InitStructure.Pull = GPIO_PULLUP;
	GPIO_InitStructure.Pin 	= SF_PIN_CS;
	
	sf_setcs(1);		/* 片选置高，不选中 */
	
	HAL_GPIO_Init(SF_PORT_CS, &GPIO_InitStructure);
}
#ifdef HARD_SPI		/* 硬件SPI */
/*
*********************************************************************************************************
*	函 数 名: sf_CfgSpiHard
*	功能说明: 配置STM32内部SPI硬件的工作模式、速度等参数，用于访问SPI接口的串行Flash。
*	形    参: 无
*	返 回 值: 无
*********************************************************************************************************
*/
SPI_HandleTypeDef SpiHandle;
static void sf_CfgSpiHard(void)
{
	SpiHandle.Instance =  SPI1;
	
	SpiHandle.Init.Mode 			 = SPI_MODE_MASTER;
	SpiHandle.Init.Direction         = SPI_DIRECTION_2LINES;
	SpiHandle.Init.DataSize          = SPI_DATASIZE_8BIT;
	SpiHandle.Init.CLKPolarity       = SPI_POLARITY_HIGH;
	SpiHandle.Init.CLKPhase          = SPI_PHASE_2EDGE;
	SpiHandle.Init.NSS               = SPI_NSS_SOFT;
	SpiHandle.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_2;
	SpiHandle.Init.FirstBit          = SPI_FIRSTBIT_MSB;
	SpiHandle.Init.TIMode            = SPI_TIMODE_DISABLE;
	
	SpiHandle.Init.CRCCalculation    = SPI_CRCCALCULATION_DISABLE;
	SpiHandle.Init.CRCPolynomial     = 7;


	HAL_SPI_Init(&SpiHandle); 

	__HAL_SPI_ENABLE(&SpiHandle);
}
#endif
/*
*********************************************************************************************************
*	函 数 名: sf_setcs(0)
*	功能说明: 设置CS。 用于运行中SPI共享。
*	形    参: 无
	返 回 值: 无
*********************************************************************************************************
*/
static void sf_setcs(uint8_t _level)
{
	if (_level == 0)
	{
		bsp_spibusenter();	/* 占用SPI总线， 用于总线共享 */

		#ifdef SOFT_SPI		/* 软件SPI */
			bsp_setspisck(1);
			SF_CS_0();
		#endif

		#ifdef HARD_SPI		/* 硬件SPI */

			bsp_SPI_Init(SPI_Direction_2Lines_FullDuplex | SPI_Mode_Master | SPI_DataSize_8b
				| SPI_CPOL_High | SPI_CPHA_2Edge | SPI_NSS_Soft | SPI_BaudRatePrescaler_8 | SPI_FirstBit_MSB);

			SF_CS_0();
		#endif
	}
	else
	{
		SF_CS_1();

		bsp_spibusexit();	/* 释放SPI总线， 用于总线共享 */
	}
}

/*
*********************************************************************************************************
*	函 数 名: bsp_InitSpiFlash
*	功能说明: 初始化串行Flash硬件接口（配置STM32的SPI时钟、GPIO)
*	形    参:  无
*	返 回 值: 无
*********************************************************************************************************
*/
void bsp_initsflash(void)
{
	sf_configgpio();			/* 配置GPIO */
	//sf_CfgSpiHard();

	sf_readinfo();				/* 自动识别芯片型号 */

	sf_setcs(0);				/* 软件方式，使能串行Flash片选 */
	bsp_spiwrite1(CMD_DISWR);		/* 发送禁止写入的命令,即使能软件写保护 */
	sf_setcs(1);				/* 软件方式，禁能串行Flash片选 */

	sf_waitforwriteend();		/* 等待串行Flash内部操作完成 */

	sf_writestatus(0);			/* 解除所有BLOCK的写保护 */
}

/*
*********************************************************************************************************
*	函 数 名: sf_EraseSector
*	功能说明: 擦除指定的扇区
*	形    参:  _uiSectorAddr : 扇区地址
*	返 回 值: 无
*********************************************************************************************************
*/
void sf_erasesector(uint32_t _uiSectorAddr)
{
	sf_writeenable();								/* 发送写使能命令 */

	/* 擦除扇区操作 */
	sf_setcs(0);									/* 使能片选 */
	bsp_spiwrite1(CMD_SE);								/* 发送擦除命令 */
	bsp_spiwrite1((_uiSectorAddr & 0xFF0000) >> 16);	/* 发送扇区地址的高8bit */
	bsp_spiwrite1((_uiSectorAddr & 0xFF00) >> 8);		/* 发送扇区地址中间8bit */
	bsp_spiwrite1(_uiSectorAddr & 0xFF);				/* 发送扇区地址低8bit */
	sf_setcs(1);									/* 禁能片选 */

	sf_waitforwriteend();							/* 等待串行Flash内部写操作完成 */
}

/*
*********************************************************************************************************
*	函 数 名: sf_EraseChip
*	功能说明: 擦除整个芯片
*	形    参:  无
*	返 回 值: 无
*********************************************************************************************************
*/
void sf_erasechip(void)
{
	sf_writeenable();								/* 发送写使能命令 */

	/* 擦除扇区操作 */
	sf_setcs(0);									/* 使能片选 */
	bsp_spiwrite1(CMD_BE);							/* 发送整片擦除命令 */
	sf_setcs(1);									/* 禁能片选 */

	sf_waitforwriteend();							/* 等待串行Flash内部写操作完成 */
}

/*
*********************************************************************************************************
*	函 数 名: sf_PageWrite
*	功能说明: 向一个page内写入若干字节。字节个数不能超出页面大小（4K)
*	形    参:  	_pBuf : 数据源缓冲区；
*				_uiWriteAddr ：目标区域首地址
*				_usSize ：数据个数，不能超过页面大小
*	返 回 值: 无
*********************************************************************************************************
*/
void sf_pagewrite(uint8_t * _pBuf, uint32_t _uiWriteAddr, uint16_t _usSize)
{
	uint32_t i, j;

	if (g_tsf.ChipID == SST25VF016B_ID)
	{
		/* AAI指令要求传入的数据个数是偶数 */
		if ((_usSize < 2) && (_usSize % 2))
		{
			return ;
		}

		sf_writeenable();								/* 发送写使能命令 */

		sf_setcs(0);									/* 使能片选 */
		bsp_spiwrite1(CMD_AAI);							/* 发送AAI命令(地址自动增加编程) */
		bsp_spiwrite1((_uiWriteAddr & 0xFF0000) >> 16);	/* 发送扇区地址的高8bit */
		bsp_spiwrite1((_uiWriteAddr & 0xFF00) >> 8);		/* 发送扇区地址中间8bit */
		bsp_spiwrite1(_uiWriteAddr & 0xFF);				/* 发送扇区地址低8bit */
		bsp_spiwrite1(*_pBuf++);							/* 发送第1个数据 */
		bsp_spiwrite1(*_pBuf++);							/* 发送第2个数据 */
		sf_setcs(1);									/* 禁能片选 */

		sf_waitforwriteend();							/* 等待串行Flash内部写操作完成 */

		_usSize -= 2;									/* 计算剩余字节数 */

		for (i = 0; i < _usSize / 2; i++)
		{
			sf_setcs(0);								/* 使能片选 */
			bsp_spiwrite1(CMD_AAI);						/* 发送AAI命令(地址自动增加编程) */
			bsp_spiwrite1(*_pBuf++);						/* 发送数据 */
			bsp_spiwrite1(*_pBuf++);						/* 发送数据 */
			sf_setcs(1);								/* 禁能片选 */
			sf_waitforwriteend();						/* 等待串行Flash内部写操作完成 */
		}

		/* 进入写保护状态 */
		sf_setcs(0);
		bsp_spiwrite1(CMD_DISWR);
		sf_setcs(1);

		sf_waitforwriteend();							/* 等待串行Flash内部写操作完成 */
	}
	else	/* for MX25L1606E 、 W25Q64BV */
	{
		for (j = 0; j < _usSize / 256; j++)
		{
			sf_writeenable();								/* 发送写使能命令 */

			sf_setcs(0);									/* 使能片选 */
			bsp_spiwrite1(0x02);								/* 发送AAI命令(地址自动增加编程) */
			bsp_spiwrite1((_uiWriteAddr & 0xFF0000) >> 16);	/* 发送扇区地址的高8bit */
			bsp_spiwrite1((_uiWriteAddr & 0xFF00) >> 8);		/* 发送扇区地址中间8bit */
			bsp_spiwrite1(_uiWriteAddr & 0xFF);				/* 发送扇区地址低8bit */

			for (i = 0; i < 256; i++)
			{
				bsp_spiwrite1(*_pBuf++);					/* 发送数据 */
			}

			sf_setcs(1);								/* 禁止片选 */

			sf_waitforwriteend();						/* 等待串行Flash内部写操作完成 */

			_uiWriteAddr += 256;
		}

		/* 进入写保护状态 */
		sf_setcs(0);
		bsp_spiwrite1(CMD_DISWR);
		sf_setcs(1);

		sf_waitforwriteend();							/* 等待串行Flash内部写操作完成 */
	}
}

/*
*********************************************************************************************************
*	函 数 名: sf_ReadBuffer
*	功能说明: 连续读取若干字节。字节个数不能超出芯片容量。
*	形    参:  	_pBuf : 数据源缓冲区；
*				_uiReadAddr ：首地址
*				_usSize ：数据个数, 可以大于PAGE_SIZE,但是不能超出芯片总容量
*	返 回 值: 无
*********************************************************************************************************
*/
void sf_readbuffer(uint8_t * _pBuf, uint32_t _uiReadAddr, uint32_t _uiSize)
{
	/* 如果读取的数据长度为0或者超出串行Flash地址空间，则直接返回 */
	if ((_uiSize == 0) ||(_uiReadAddr + _uiSize) > g_tsf.TotalSize)
	{
		return;
	}

	/* 擦除扇区操作 */
	sf_setcs(0);									/* 使能片选 */
	bsp_spiwrite1(CMD_READ);							/* 发送读命令 */
	bsp_spiwrite1((_uiReadAddr & 0xFF0000) >> 16);	/* 发送扇区地址的高8bit */
	bsp_spiwrite1((_uiReadAddr & 0xFF00) >> 8);		/* 发送扇区地址中间8bit */
	bsp_spiwrite1(_uiReadAddr & 0xFF);				/* 发送扇区地址低8bit */
	while (_uiSize--)
	{
		*_pBuf++ = bsp_spiread1();			/* 读一个字节并存储到pBuf，读完后指针自加1 */
	}
	sf_setcs(1);									/* 禁能片选 */
}

/*
*********************************************************************************************************
*	函 数 名: sf_CmpData
*	功能说明: 比较Flash的数据.
*	形    参:  	_ucpTar : 数据缓冲区
*				_uiSrcAddr ：Flash地址
*				_uiSize ：数据个数, 可以大于PAGE_SIZE,但是不能超出芯片总容量
*	返 回 值: 0 = 相等, 1 = 不等
*********************************************************************************************************
*/
static uint8_t sf_cmpdata(uint32_t _uiSrcAddr, uint8_t *_ucpTar, uint32_t _uiSize)
{
	uint8_t ucValue;

	/* 如果读取的数据长度为0或者超出串行Flash地址空间，则直接返回 */
	if ((_uiSrcAddr + _uiSize) > g_tsf.TotalSize)
	{
		return 1;
	}

	if (_uiSize == 0)
	{
		return 0;
	}

	sf_setcs(0);									/* 使能片选 */
	bsp_spiwrite1(CMD_READ);							/* 发送读命令 */
	bsp_spiwrite1((_uiSrcAddr & 0xFF0000) >> 16);		/* 发送扇区地址的高8bit */
	bsp_spiwrite1((_uiSrcAddr & 0xFF00) >> 8);		/* 发送扇区地址中间8bit */
	bsp_spiwrite1(_uiSrcAddr & 0xFF);					/* 发送扇区地址低8bit */
	while (_uiSize--)
	{
		/* 读一个字节 */
		ucValue = bsp_spiread1();
		if (*_ucpTar++ != ucValue)
		{
			sf_setcs(1);
			return 1;
		}
	}
	sf_setcs(1);
	return 0;
}

/*
*********************************************************************************************************
*	函 数 名: sf_NeedErase
*	功能说明: 判断写PAGE前是否需要先擦除。
*	形    参:   _ucpOldBuf ： 旧数据
*			   _ucpNewBuf ： 新数据
*			   _uiLen ：数据个数，不能超过页面大小
*	返 回 值: 0 : 不需要擦除， 1 ：需要擦除
*********************************************************************************************************
*/
static uint8_t sf_neederase(uint8_t * _ucpOldBuf, uint8_t *_ucpNewBuf, uint16_t _usLen)
{
	uint16_t i;
	uint8_t ucOld;

	/*
	算法第1步：old 求反, new 不变
	      old    new
		  1101   0101
	~     1111
		= 0010   0101

	算法第2步: old 求反的结果与 new 位与
		  0010   old
	&	  0101   new
		 =0000

	算法第3步: 结果为0,则表示无需擦除. 否则表示需要擦除
	*/

	for (i = 0; i < _usLen; i++)
	{
		ucOld = *_ucpOldBuf++;
		ucOld = ~ucOld;

		/* 注意错误的写法: if (ucOld & (*_ucpNewBuf++) != 0) */
		if ((ucOld & (*_ucpNewBuf++)) != 0)
		{
			return 1;
		}
	}
	return 0;
}

/*
*********************************************************************************************************
*	函 数 名: sf_autowritepage
*	功能说明: 写1个PAGE并校验,如果不正确则再重写两次。本函数自动完成擦除操作。
*	形    参:  	_pBuf : 数据源缓冲区；
*				_uiWriteAddr ：目标区域首地址
*				_usSize ：数据个数，不能超过页面大小
*	返 回 值: 0 : 错误， 1 ： 成功
*********************************************************************************************************
*/
static uint8_t sf_autowritepage(uint8_t *_ucpSrc, uint32_t _uiWrAddr, uint16_t _usWrLen)
{
	uint16_t i;
	uint16_t j;					/* 用于延时 */
	uint32_t uiFirstAddr;		/* 扇区首址 */
	uint8_t ucNeedErase;		/* 1表示需要擦除 */
	uint8_t cRet;

	/* 长度为0时不继续操作,直接认为成功 */
	if (_usWrLen == 0)
	{
		return 1;
	}

	/* 如果偏移地址超过芯片容量则退出 */
	if (_uiWrAddr >= g_tsf.TotalSize)
	{
		return 0;
	}

	/* 如果数据长度大于扇区容量，则退出 */
	if (_usWrLen > g_tsf.PageSize)
	{
		return 0;
	}

	/* 如果FLASH中的数据没有变化,则不写FLASH */
	sf_readbuffer(s_spibuf, _uiWrAddr, _usWrLen);
	if (memcmp(s_spibuf, _ucpSrc, _usWrLen) == 0)
	{
		return 1;
	}

	/* 判断是否需要先擦除扇区 */
	/* 如果旧数据修改为新数据，所有位均是 1->0 或者 0->0, 则无需擦除,提高Flash寿命 */
	ucNeedErase = 0;
	if (sf_neederase(s_spibuf, _ucpSrc, _usWrLen))
	{
		ucNeedErase = 1;
	}

	uiFirstAddr = _uiWrAddr & (~(g_tsf.PageSize - 1));

	if (_usWrLen == g_tsf.PageSize)		/* 整个扇区都改写 */
	{
		for	(i = 0; i < g_tsf.PageSize; i++)
		{
			s_spibuf[i] = _ucpSrc[i];
		}
	}
	else						/* 改写部分数据 */
	{
		/* 先将整个扇区的数据读出 */
		sf_readbuffer(s_spibuf, uiFirstAddr, g_tsf.PageSize);

		/* 再用新数据覆盖 */
		i = _uiWrAddr & (g_tsf.PageSize - 1);
		memcpy(&s_spibuf[i], _ucpSrc, _usWrLen);
	}

	/* 写完之后进行校验，如果不正确则重写，最多3次 */
	cRet = 0;
	for (i = 0; i < 3; i++)
	{

		/* 如果旧数据修改为新数据，所有位均是 1->0 或者 0->0, 则无需擦除,提高Flash寿命 */
		if (ucNeedErase == 1)
		{
			sf_erasesector(uiFirstAddr);		/* 擦除1个扇区 */
		}

		/* 编程一个PAGE */
		sf_pagewrite(s_spibuf, uiFirstAddr, g_tsf.PageSize);

		if (sf_cmpdata(_uiWrAddr, _ucpSrc, _usWrLen) == 0)
		{
			cRet = 1;
			break;
		}
		else
		{
			if (sf_cmpdata(_uiWrAddr, _ucpSrc, _usWrLen) == 0)
			{
				cRet = 1;
				break;
			}

			/* 失败后延迟一段时间再重试 */
			for (j = 0; j < 10000; j++);
		}
	}

	return cRet;
}

/*
*********************************************************************************************************
*	函 数 名: sf_writebuffer
*	功能说明: 写1个扇区并校验,如果不正确则再重写两次。本函数自动完成擦除操作。
*	形    参:  	_pBuf : 数据源缓冲区；
*				_uiWrAddr ：目标区域首地址
*				_usSize ：数据个数，不能超过页面大小
*	返 回 值: 1 : 成功， 0 ： 失败
*********************************************************************************************************
*/
uint8_t sf_writebuffer(uint8_t* _pBuf, uint32_t _uiWriteAddr, uint16_t _usWriteSize)
{
	uint16_t NumOfPage = 0, NumOfSingle = 0, Addr = 0, count = 0, temp = 0;

	Addr = _uiWriteAddr % g_tsf.PageSize;
	count = g_tsf.PageSize - Addr;
	NumOfPage =  _usWriteSize / g_tsf.PageSize;
	NumOfSingle = _usWriteSize % g_tsf.PageSize;

	if (Addr == 0) /* 起始地址是页面首地址  */
	{
		if (NumOfPage == 0) /* 数据长度小于页面大小 */
		{
			if (sf_autowritepage(_pBuf, _uiWriteAddr, _usWriteSize) == 0)
			{
				return 0;
			}
		}
		else 	/* 数据长度大于等于页面大小 */
		{
			while (NumOfPage--)
			{
				if (sf_autowritepage(_pBuf, _uiWriteAddr, g_tsf.PageSize) == 0)
				{
					return 0;
				}
				_uiWriteAddr +=  g_tsf.PageSize;
				_pBuf += g_tsf.PageSize;
			}
			if (sf_autowritepage(_pBuf, _uiWriteAddr, NumOfSingle) == 0)
			{
				return 0;
			}
		}
	}
	else  /* 起始地址不是页面首地址  */
	{
		if (NumOfPage == 0) /* 数据长度小于页面大小 */
		{
			if (NumOfSingle > count) /* (_usWriteSize + _uiWriteAddr) > SPI_FLASH_PAGESIZE */
			{
				temp = NumOfSingle - count;

				if (sf_autowritepage(_pBuf, _uiWriteAddr, count) == 0)
				{
					return 0;
				}

				_uiWriteAddr +=  count;
				_pBuf += count;

				if (sf_autowritepage(_pBuf, _uiWriteAddr, temp) == 0)
				{
					return 0;
				}
			}
			else
			{
				if (sf_autowritepage(_pBuf, _uiWriteAddr, _usWriteSize) == 0)
				{
					return 0;
				}
			}
		}
		else	/* 数据长度大于等于页面大小 */
		{
			_usWriteSize -= count;
			NumOfPage =  _usWriteSize / g_tsf.PageSize;
			NumOfSingle = _usWriteSize % g_tsf.PageSize;

			if (sf_autowritepage(_pBuf, _uiWriteAddr, count) == 0)
			{
				return 0;
			}

			_uiWriteAddr +=  count;
			_pBuf += count;

			while (NumOfPage--)
			{
				if (sf_autowritepage(_pBuf, _uiWriteAddr, g_tsf.PageSize) == 0)
				{
					return 0;
				}
				_uiWriteAddr +=  g_tsf.PageSize;
				_pBuf += g_tsf.PageSize;
			}

			if (NumOfSingle != 0)
			{
				if (sf_autowritepage(_pBuf, _uiWriteAddr, NumOfSingle) == 0)
				{
					return 0;
				}
			}
		}
	}
	return 1;	/* 成功 */
}

/*
*********************************************************************************************************
*	函 数 名: sf_ReadID
*	功能说明: 读取器件ID
*	形    参:  无
*	返 回 值: 32bit的器件ID (最高8bit填0，有效ID位数为24bit）
*********************************************************************************************************
*/
uint32_t sf_ReadID(void)
{
	uint32_t uiID;
	uint8_t id1, id2, id3;

	sf_setcs(0);									/* 使能片选 */
	bsp_spiwrite1(CMD_RDID);								/* 发送读ID命令 */
	id1 = bsp_spiread1();					/* 读ID的第1个字节 */
	id2 = bsp_spiread1();					/* 读ID的第2个字节 */
	id3 = bsp_spiread1();					/* 读ID的第3个字节 */
	sf_setcs(1);									/* 禁能片选 */

	uiID = ((uint32_t)id1 << 16) | ((uint32_t)id2 << 8) | id3;

	return uiID;
}

/*
*********************************************************************************************************
*	函 数 名: sf_readinfo
*	功能说明: 读取器件ID,并填充器件参数
*	形    参:  无
*	返 回 值: 无
*********************************************************************************************************
*/
void sf_readinfo(void)
{
	/* 自动识别串行Flash型号 */
	{
		g_tsf.ChipID = sf_ReadID();	/* 芯片ID */

		switch (g_tsf.ChipID)
		{
			case SST25VF016B_ID:
				strcpy(g_tsf.ChipName, "SST25VF016B");
				g_tsf.TotalSize = 2 * 1024 * 1024;	/* 总容量 = 2M */
				g_tsf.PageSize = 4 * 1024;			/* 页面大小 = 4K */
				break;

			case MX25L1606E_ID:
				strcpy(g_tsf.ChipName, "MX25L1606E");
				g_tsf.TotalSize = 2 * 1024 * 1024;	/* 总容量 = 2M */
				g_tsf.PageSize = 4 * 1024;			/* 页面大小 = 4K */
				break;

			case W25Q64BV_ID:
				strcpy(g_tsf.ChipName, "W25Q64BV");
				g_tsf.TotalSize = 8 * 1024 * 1024;	/* 总容量 = 8M */
				g_tsf.PageSize = 4 * 1024;			/* 页面大小 = 4K */
				break;
			case W25Q128FV_ID:
				strcpy(g_tsf.ChipName, "W25Q128FV");
				g_tsf.TotalSize = 16 * 1024 * 1024;	/* 总容量 = 8M */
				g_tsf.PageSize = 4 * 1024;			/* 页面大小 = 4K */
				break;

			default:
				strcpy(g_tsf.ChipName, "Unknow Flash");
				g_tsf.TotalSize = 2 * 1024 * 1024;
				g_tsf.PageSize = 4 * 1024;
				break;
		}
	}
}

/*
*********************************************************************************************************
*	函 数 名: sf_WriteEnable
*	功能说明: 向器件发送写使能命令
*	形    参:  无
*	返 回 值: 无
*********************************************************************************************************
*/
static void sf_writeenable(void)
{
	sf_setcs(0);									/* 使能片选 */
	bsp_spiwrite1(CMD_WREN);								/* 发送命令 */
	sf_setcs(1);									/* 禁能片选 */
}

/*
*********************************************************************************************************
*	函 数 名: sf_writestatus
*	功能说明: 写状态寄存器
*	形    参:  _ucValue : 状态寄存器的值
*	返 回 值: 无
*********************************************************************************************************
*/
static void sf_writestatus(uint8_t _ucValue)
{

	if (g_tsf.ChipID == SST25VF016B_ID)
	{
		/* 第1步：先使能写状态寄存器 */
		sf_setcs(0);									/* 使能片选 */
		bsp_spiwrite1(CMD_EWRSR);							/* 发送命令， 允许写状态寄存器 */
		sf_setcs(1);									/* 禁能片选 */

		/* 第2步：再写状态寄存器 */
		sf_setcs(0);									/* 使能片选 */
		bsp_spiwrite1(CMD_WRSR);							/* 发送命令， 写状态寄存器 */
		bsp_spiwrite1(_ucValue);							/* 发送数据：状态寄存器的值 */
		sf_setcs(1);									/* 禁能片选 */
	}
	else
	{
		sf_setcs(0);									/* 使能片选 */
		bsp_spiwrite1(CMD_WRSR);							/* 发送命令， 写状态寄存器 */
		bsp_spiwrite1(_ucValue);							/* 发送数据：状态寄存器的值 */
		sf_setcs(1);									/* 禁能片选 */
	}
}

/*
*********************************************************************************************************
*	函 数 名: sf_waitforwriteend
*	功能说明: 采用循环查询的方式等待器件内部写操作完成
*	形    参:  无
*	返 回 值: 无
*********************************************************************************************************
*/
static void sf_waitforwriteend(void)
{
	sf_setcs(0);									/* 使能片选 */
	bsp_spiwrite1(CMD_RDSR);							/* 发送命令， 读状态寄存器 */
	while((bsp_spiread1() & WIP_FLAG) == SET);	/* 判断状态寄存器的忙标志位 */
	sf_setcs(1);									/* 禁能片选 */
}
