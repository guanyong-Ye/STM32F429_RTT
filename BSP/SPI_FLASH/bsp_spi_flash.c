/*
*********************************************************************************************************
*
*	ģ������ : SPI�ӿڴ���FLASH ��дģ��
*	�ļ����� : bsp_spi_flash.c
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
static uint8_t s_spibuf[4*1024];	/* ����д�������ȶ�������page���޸Ļ�������������page��д */

//static void sf_CfgSpiHard(void);
static void sf_configgpio(void);
static void sf_setcs(uint8_t _level);

/*
*********************************************************************************************************
*	�� �� ��: sf_configgpio
*	����˵��: ���ô���Flash��ƬѡGPIO�� ����Ϊ�������
*	��    ��: ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
static void sf_configgpio(void)
{
	/*
		������STM32-V5 ��������߷��䣺  ����Flash�ͺ�Ϊ W25Q64BVSSIG (80MHz)
		PF8/SF_CS
	*/
	GPIO_InitTypeDef GPIO_InitStructure;

	/* ʹ��GPIO ʱ�� */
	SPI_CS_PORT_ENABLE() ;

	/* ����Ƭѡ����Ϊ�������ģʽ */
	sf_setcs(1);		/* Ƭѡ�øߣ���ѡ�� */
	
	GPIO_InitStructure.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStructure.Speed = GPIO_SPEED_FREQ_HIGH;
	GPIO_InitStructure.Pull = GPIO_PULLUP;
	GPIO_InitStructure.Pin 	= SF_PIN_CS;
	
	sf_setcs(1);		/* Ƭѡ�øߣ���ѡ�� */
	
	HAL_GPIO_Init(SF_PORT_CS, &GPIO_InitStructure);
}
#ifdef HARD_SPI		/* Ӳ��SPI */
/*
*********************************************************************************************************
*	�� �� ��: sf_CfgSpiHard
*	����˵��: ����STM32�ڲ�SPIӲ���Ĺ���ģʽ���ٶȵȲ��������ڷ���SPI�ӿڵĴ���Flash��
*	��    ��: ��
*	�� �� ֵ: ��
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
*	�� �� ��: sf_setcs(0)
*	����˵��: ����CS�� ����������SPI����
*	��    ��: ��
	�� �� ֵ: ��
*********************************************************************************************************
*/
static void sf_setcs(uint8_t _level)
{
	if (_level == 0)
	{
		bsp_spibusenter();	/* ռ��SPI���ߣ� �������߹��� */

		#ifdef SOFT_SPI		/* ���SPI */
			bsp_setspisck(1);
			SF_CS_0();
		#endif

		#ifdef HARD_SPI		/* Ӳ��SPI */

			bsp_SPI_Init(SPI_Direction_2Lines_FullDuplex | SPI_Mode_Master | SPI_DataSize_8b
				| SPI_CPOL_High | SPI_CPHA_2Edge | SPI_NSS_Soft | SPI_BaudRatePrescaler_8 | SPI_FirstBit_MSB);

			SF_CS_0();
		#endif
	}
	else
	{
		SF_CS_1();

		bsp_spibusexit();	/* �ͷ�SPI���ߣ� �������߹��� */
	}
}

/*
*********************************************************************************************************
*	�� �� ��: bsp_InitSpiFlash
*	����˵��: ��ʼ������FlashӲ���ӿڣ�����STM32��SPIʱ�ӡ�GPIO)
*	��    ��:  ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void bsp_initsflash(void)
{
	sf_configgpio();			/* ����GPIO */
	//sf_CfgSpiHard();

	sf_readinfo();				/* �Զ�ʶ��оƬ�ͺ� */

	sf_setcs(0);				/* �����ʽ��ʹ�ܴ���FlashƬѡ */
	bsp_spiwrite1(CMD_DISWR);		/* ���ͽ�ֹд�������,��ʹ�����д���� */
	sf_setcs(1);				/* �����ʽ�����ܴ���FlashƬѡ */

	sf_waitforwriteend();		/* �ȴ�����Flash�ڲ�������� */

	sf_writestatus(0);			/* �������BLOCK��д���� */
}

/*
*********************************************************************************************************
*	�� �� ��: sf_EraseSector
*	����˵��: ����ָ��������
*	��    ��:  _uiSectorAddr : ������ַ
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void sf_erasesector(uint32_t _uiSectorAddr)
{
	sf_writeenable();								/* ����дʹ������ */

	/* ������������ */
	sf_setcs(0);									/* ʹ��Ƭѡ */
	bsp_spiwrite1(CMD_SE);								/* ���Ͳ������� */
	bsp_spiwrite1((_uiSectorAddr & 0xFF0000) >> 16);	/* ����������ַ�ĸ�8bit */
	bsp_spiwrite1((_uiSectorAddr & 0xFF00) >> 8);		/* ����������ַ�м�8bit */
	bsp_spiwrite1(_uiSectorAddr & 0xFF);				/* ����������ַ��8bit */
	sf_setcs(1);									/* ����Ƭѡ */

	sf_waitforwriteend();							/* �ȴ�����Flash�ڲ�д������� */
}

/*
*********************************************************************************************************
*	�� �� ��: sf_EraseChip
*	����˵��: ��������оƬ
*	��    ��:  ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void sf_erasechip(void)
{
	sf_writeenable();								/* ����дʹ������ */

	/* ������������ */
	sf_setcs(0);									/* ʹ��Ƭѡ */
	bsp_spiwrite1(CMD_BE);							/* ������Ƭ�������� */
	sf_setcs(1);									/* ����Ƭѡ */

	sf_waitforwriteend();							/* �ȴ�����Flash�ڲ�д������� */
}

/*
*********************************************************************************************************
*	�� �� ��: sf_PageWrite
*	����˵��: ��һ��page��д�������ֽڡ��ֽڸ������ܳ���ҳ���С��4K)
*	��    ��:  	_pBuf : ����Դ��������
*				_uiWriteAddr ��Ŀ�������׵�ַ
*				_usSize �����ݸ��������ܳ���ҳ���С
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void sf_pagewrite(uint8_t * _pBuf, uint32_t _uiWriteAddr, uint16_t _usSize)
{
	uint32_t i, j;

	if (g_tsf.ChipID == SST25VF016B_ID)
	{
		/* AAIָ��Ҫ��������ݸ�����ż�� */
		if ((_usSize < 2) && (_usSize % 2))
		{
			return ;
		}

		sf_writeenable();								/* ����дʹ������ */

		sf_setcs(0);									/* ʹ��Ƭѡ */
		bsp_spiwrite1(CMD_AAI);							/* ����AAI����(��ַ�Զ����ӱ��) */
		bsp_spiwrite1((_uiWriteAddr & 0xFF0000) >> 16);	/* ����������ַ�ĸ�8bit */
		bsp_spiwrite1((_uiWriteAddr & 0xFF00) >> 8);		/* ����������ַ�м�8bit */
		bsp_spiwrite1(_uiWriteAddr & 0xFF);				/* ����������ַ��8bit */
		bsp_spiwrite1(*_pBuf++);							/* ���͵�1������ */
		bsp_spiwrite1(*_pBuf++);							/* ���͵�2������ */
		sf_setcs(1);									/* ����Ƭѡ */

		sf_waitforwriteend();							/* �ȴ�����Flash�ڲ�д������� */

		_usSize -= 2;									/* ����ʣ���ֽ��� */

		for (i = 0; i < _usSize / 2; i++)
		{
			sf_setcs(0);								/* ʹ��Ƭѡ */
			bsp_spiwrite1(CMD_AAI);						/* ����AAI����(��ַ�Զ����ӱ��) */
			bsp_spiwrite1(*_pBuf++);						/* �������� */
			bsp_spiwrite1(*_pBuf++);						/* �������� */
			sf_setcs(1);								/* ����Ƭѡ */
			sf_waitforwriteend();						/* �ȴ�����Flash�ڲ�д������� */
		}

		/* ����д����״̬ */
		sf_setcs(0);
		bsp_spiwrite1(CMD_DISWR);
		sf_setcs(1);

		sf_waitforwriteend();							/* �ȴ�����Flash�ڲ�д������� */
	}
	else	/* for MX25L1606E �� W25Q64BV */
	{
		for (j = 0; j < _usSize / 256; j++)
		{
			sf_writeenable();								/* ����дʹ������ */

			sf_setcs(0);									/* ʹ��Ƭѡ */
			bsp_spiwrite1(0x02);								/* ����AAI����(��ַ�Զ����ӱ��) */
			bsp_spiwrite1((_uiWriteAddr & 0xFF0000) >> 16);	/* ����������ַ�ĸ�8bit */
			bsp_spiwrite1((_uiWriteAddr & 0xFF00) >> 8);		/* ����������ַ�м�8bit */
			bsp_spiwrite1(_uiWriteAddr & 0xFF);				/* ����������ַ��8bit */

			for (i = 0; i < 256; i++)
			{
				bsp_spiwrite1(*_pBuf++);					/* �������� */
			}

			sf_setcs(1);								/* ��ֹƬѡ */

			sf_waitforwriteend();						/* �ȴ�����Flash�ڲ�д������� */

			_uiWriteAddr += 256;
		}

		/* ����д����״̬ */
		sf_setcs(0);
		bsp_spiwrite1(CMD_DISWR);
		sf_setcs(1);

		sf_waitforwriteend();							/* �ȴ�����Flash�ڲ�д������� */
	}
}

/*
*********************************************************************************************************
*	�� �� ��: sf_ReadBuffer
*	����˵��: ������ȡ�����ֽڡ��ֽڸ������ܳ���оƬ������
*	��    ��:  	_pBuf : ����Դ��������
*				_uiReadAddr ���׵�ַ
*				_usSize �����ݸ���, ���Դ���PAGE_SIZE,���ǲ��ܳ���оƬ������
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void sf_readbuffer(uint8_t * _pBuf, uint32_t _uiReadAddr, uint32_t _uiSize)
{
	/* �����ȡ�����ݳ���Ϊ0���߳�������Flash��ַ�ռ䣬��ֱ�ӷ��� */
	if ((_uiSize == 0) ||(_uiReadAddr + _uiSize) > g_tsf.TotalSize)
	{
		return;
	}

	/* ������������ */
	sf_setcs(0);									/* ʹ��Ƭѡ */
	bsp_spiwrite1(CMD_READ);							/* ���Ͷ����� */
	bsp_spiwrite1((_uiReadAddr & 0xFF0000) >> 16);	/* ����������ַ�ĸ�8bit */
	bsp_spiwrite1((_uiReadAddr & 0xFF00) >> 8);		/* ����������ַ�м�8bit */
	bsp_spiwrite1(_uiReadAddr & 0xFF);				/* ����������ַ��8bit */
	while (_uiSize--)
	{
		*_pBuf++ = bsp_spiread1();			/* ��һ���ֽڲ��洢��pBuf�������ָ���Լ�1 */
	}
	sf_setcs(1);									/* ����Ƭѡ */
}

/*
*********************************************************************************************************
*	�� �� ��: sf_CmpData
*	����˵��: �Ƚ�Flash������.
*	��    ��:  	_ucpTar : ���ݻ�����
*				_uiSrcAddr ��Flash��ַ
*				_uiSize �����ݸ���, ���Դ���PAGE_SIZE,���ǲ��ܳ���оƬ������
*	�� �� ֵ: 0 = ���, 1 = ����
*********************************************************************************************************
*/
static uint8_t sf_cmpdata(uint32_t _uiSrcAddr, uint8_t *_ucpTar, uint32_t _uiSize)
{
	uint8_t ucValue;

	/* �����ȡ�����ݳ���Ϊ0���߳�������Flash��ַ�ռ䣬��ֱ�ӷ��� */
	if ((_uiSrcAddr + _uiSize) > g_tsf.TotalSize)
	{
		return 1;
	}

	if (_uiSize == 0)
	{
		return 0;
	}

	sf_setcs(0);									/* ʹ��Ƭѡ */
	bsp_spiwrite1(CMD_READ);							/* ���Ͷ����� */
	bsp_spiwrite1((_uiSrcAddr & 0xFF0000) >> 16);		/* ����������ַ�ĸ�8bit */
	bsp_spiwrite1((_uiSrcAddr & 0xFF00) >> 8);		/* ����������ַ�м�8bit */
	bsp_spiwrite1(_uiSrcAddr & 0xFF);					/* ����������ַ��8bit */
	while (_uiSize--)
	{
		/* ��һ���ֽ� */
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
*	�� �� ��: sf_NeedErase
*	����˵��: �ж�дPAGEǰ�Ƿ���Ҫ�Ȳ�����
*	��    ��:   _ucpOldBuf �� ������
*			   _ucpNewBuf �� ������
*			   _uiLen �����ݸ��������ܳ���ҳ���С
*	�� �� ֵ: 0 : ����Ҫ������ 1 ����Ҫ����
*********************************************************************************************************
*/
static uint8_t sf_neederase(uint8_t * _ucpOldBuf, uint8_t *_ucpNewBuf, uint16_t _usLen)
{
	uint16_t i;
	uint8_t ucOld;

	/*
	�㷨��1����old ��, new ����
	      old    new
		  1101   0101
	~     1111
		= 0010   0101

	�㷨��2��: old �󷴵Ľ���� new λ��
		  0010   old
	&	  0101   new
		 =0000

	�㷨��3��: ���Ϊ0,���ʾ�������. �����ʾ��Ҫ����
	*/

	for (i = 0; i < _usLen; i++)
	{
		ucOld = *_ucpOldBuf++;
		ucOld = ~ucOld;

		/* ע������д��: if (ucOld & (*_ucpNewBuf++) != 0) */
		if ((ucOld & (*_ucpNewBuf++)) != 0)
		{
			return 1;
		}
	}
	return 0;
}

/*
*********************************************************************************************************
*	�� �� ��: sf_autowritepage
*	����˵��: д1��PAGE��У��,�������ȷ������д���Ρ��������Զ���ɲ���������
*	��    ��:  	_pBuf : ����Դ��������
*				_uiWriteAddr ��Ŀ�������׵�ַ
*				_usSize �����ݸ��������ܳ���ҳ���С
*	�� �� ֵ: 0 : ���� 1 �� �ɹ�
*********************************************************************************************************
*/
static uint8_t sf_autowritepage(uint8_t *_ucpSrc, uint32_t _uiWrAddr, uint16_t _usWrLen)
{
	uint16_t i;
	uint16_t j;					/* ������ʱ */
	uint32_t uiFirstAddr;		/* ������ַ */
	uint8_t ucNeedErase;		/* 1��ʾ��Ҫ���� */
	uint8_t cRet;

	/* ����Ϊ0ʱ����������,ֱ����Ϊ�ɹ� */
	if (_usWrLen == 0)
	{
		return 1;
	}

	/* ���ƫ�Ƶ�ַ����оƬ�������˳� */
	if (_uiWrAddr >= g_tsf.TotalSize)
	{
		return 0;
	}

	/* ������ݳ��ȴ����������������˳� */
	if (_usWrLen > g_tsf.PageSize)
	{
		return 0;
	}

	/* ���FLASH�е�����û�б仯,��дFLASH */
	sf_readbuffer(s_spibuf, _uiWrAddr, _usWrLen);
	if (memcmp(s_spibuf, _ucpSrc, _usWrLen) == 0)
	{
		return 1;
	}

	/* �ж��Ƿ���Ҫ�Ȳ������� */
	/* ����������޸�Ϊ�����ݣ�����λ���� 1->0 ���� 0->0, ���������,���Flash���� */
	ucNeedErase = 0;
	if (sf_neederase(s_spibuf, _ucpSrc, _usWrLen))
	{
		ucNeedErase = 1;
	}

	uiFirstAddr = _uiWrAddr & (~(g_tsf.PageSize - 1));

	if (_usWrLen == g_tsf.PageSize)		/* ������������д */
	{
		for	(i = 0; i < g_tsf.PageSize; i++)
		{
			s_spibuf[i] = _ucpSrc[i];
		}
	}
	else						/* ��д�������� */
	{
		/* �Ƚ��������������ݶ��� */
		sf_readbuffer(s_spibuf, uiFirstAddr, g_tsf.PageSize);

		/* ���������ݸ��� */
		i = _uiWrAddr & (g_tsf.PageSize - 1);
		memcpy(&s_spibuf[i], _ucpSrc, _usWrLen);
	}

	/* д��֮�����У�飬�������ȷ����д�����3�� */
	cRet = 0;
	for (i = 0; i < 3; i++)
	{

		/* ����������޸�Ϊ�����ݣ�����λ���� 1->0 ���� 0->0, ���������,���Flash���� */
		if (ucNeedErase == 1)
		{
			sf_erasesector(uiFirstAddr);		/* ����1������ */
		}

		/* ���һ��PAGE */
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

			/* ʧ�ܺ��ӳ�һ��ʱ�������� */
			for (j = 0; j < 10000; j++);
		}
	}

	return cRet;
}

/*
*********************************************************************************************************
*	�� �� ��: sf_writebuffer
*	����˵��: д1��������У��,�������ȷ������д���Ρ��������Զ���ɲ���������
*	��    ��:  	_pBuf : ����Դ��������
*				_uiWrAddr ��Ŀ�������׵�ַ
*				_usSize �����ݸ��������ܳ���ҳ���С
*	�� �� ֵ: 1 : �ɹ��� 0 �� ʧ��
*********************************************************************************************************
*/
uint8_t sf_writebuffer(uint8_t* _pBuf, uint32_t _uiWriteAddr, uint16_t _usWriteSize)
{
	uint16_t NumOfPage = 0, NumOfSingle = 0, Addr = 0, count = 0, temp = 0;

	Addr = _uiWriteAddr % g_tsf.PageSize;
	count = g_tsf.PageSize - Addr;
	NumOfPage =  _usWriteSize / g_tsf.PageSize;
	NumOfSingle = _usWriteSize % g_tsf.PageSize;

	if (Addr == 0) /* ��ʼ��ַ��ҳ���׵�ַ  */
	{
		if (NumOfPage == 0) /* ���ݳ���С��ҳ���С */
		{
			if (sf_autowritepage(_pBuf, _uiWriteAddr, _usWriteSize) == 0)
			{
				return 0;
			}
		}
		else 	/* ���ݳ��ȴ��ڵ���ҳ���С */
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
	else  /* ��ʼ��ַ����ҳ���׵�ַ  */
	{
		if (NumOfPage == 0) /* ���ݳ���С��ҳ���С */
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
		else	/* ���ݳ��ȴ��ڵ���ҳ���С */
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
	return 1;	/* �ɹ� */
}

/*
*********************************************************************************************************
*	�� �� ��: sf_ReadID
*	����˵��: ��ȡ����ID
*	��    ��:  ��
*	�� �� ֵ: 32bit������ID (���8bit��0����ЧIDλ��Ϊ24bit��
*********************************************************************************************************
*/
uint32_t sf_ReadID(void)
{
	uint32_t uiID;
	uint8_t id1, id2, id3;

	sf_setcs(0);									/* ʹ��Ƭѡ */
	bsp_spiwrite1(CMD_RDID);								/* ���Ͷ�ID���� */
	id1 = bsp_spiread1();					/* ��ID�ĵ�1���ֽ� */
	id2 = bsp_spiread1();					/* ��ID�ĵ�2���ֽ� */
	id3 = bsp_spiread1();					/* ��ID�ĵ�3���ֽ� */
	sf_setcs(1);									/* ����Ƭѡ */

	uiID = ((uint32_t)id1 << 16) | ((uint32_t)id2 << 8) | id3;

	return uiID;
}

/*
*********************************************************************************************************
*	�� �� ��: sf_readinfo
*	����˵��: ��ȡ����ID,�������������
*	��    ��:  ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void sf_readinfo(void)
{
	/* �Զ�ʶ����Flash�ͺ� */
	{
		g_tsf.ChipID = sf_ReadID();	/* оƬID */

		switch (g_tsf.ChipID)
		{
			case SST25VF016B_ID:
				strcpy(g_tsf.ChipName, "SST25VF016B");
				g_tsf.TotalSize = 2 * 1024 * 1024;	/* ������ = 2M */
				g_tsf.PageSize = 4 * 1024;			/* ҳ���С = 4K */
				break;

			case MX25L1606E_ID:
				strcpy(g_tsf.ChipName, "MX25L1606E");
				g_tsf.TotalSize = 2 * 1024 * 1024;	/* ������ = 2M */
				g_tsf.PageSize = 4 * 1024;			/* ҳ���С = 4K */
				break;

			case W25Q64BV_ID:
				strcpy(g_tsf.ChipName, "W25Q64BV");
				g_tsf.TotalSize = 8 * 1024 * 1024;	/* ������ = 8M */
				g_tsf.PageSize = 4 * 1024;			/* ҳ���С = 4K */
				break;
			case W25Q128FV_ID:
				strcpy(g_tsf.ChipName, "W25Q128FV");
				g_tsf.TotalSize = 16 * 1024 * 1024;	/* ������ = 8M */
				g_tsf.PageSize = 4 * 1024;			/* ҳ���С = 4K */
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
*	�� �� ��: sf_WriteEnable
*	����˵��: ����������дʹ������
*	��    ��:  ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
static void sf_writeenable(void)
{
	sf_setcs(0);									/* ʹ��Ƭѡ */
	bsp_spiwrite1(CMD_WREN);								/* �������� */
	sf_setcs(1);									/* ����Ƭѡ */
}

/*
*********************************************************************************************************
*	�� �� ��: sf_writestatus
*	����˵��: д״̬�Ĵ���
*	��    ��:  _ucValue : ״̬�Ĵ�����ֵ
*	�� �� ֵ: ��
*********************************************************************************************************
*/
static void sf_writestatus(uint8_t _ucValue)
{

	if (g_tsf.ChipID == SST25VF016B_ID)
	{
		/* ��1������ʹ��д״̬�Ĵ��� */
		sf_setcs(0);									/* ʹ��Ƭѡ */
		bsp_spiwrite1(CMD_EWRSR);							/* ������� ����д״̬�Ĵ��� */
		sf_setcs(1);									/* ����Ƭѡ */

		/* ��2������д״̬�Ĵ��� */
		sf_setcs(0);									/* ʹ��Ƭѡ */
		bsp_spiwrite1(CMD_WRSR);							/* ������� д״̬�Ĵ��� */
		bsp_spiwrite1(_ucValue);							/* �������ݣ�״̬�Ĵ�����ֵ */
		sf_setcs(1);									/* ����Ƭѡ */
	}
	else
	{
		sf_setcs(0);									/* ʹ��Ƭѡ */
		bsp_spiwrite1(CMD_WRSR);							/* ������� д״̬�Ĵ��� */
		bsp_spiwrite1(_ucValue);							/* �������ݣ�״̬�Ĵ�����ֵ */
		sf_setcs(1);									/* ����Ƭѡ */
	}
}

/*
*********************************************************************************************************
*	�� �� ��: sf_waitforwriteend
*	����˵��: ����ѭ����ѯ�ķ�ʽ�ȴ������ڲ�д�������
*	��    ��:  ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
static void sf_waitforwriteend(void)
{
	sf_setcs(0);									/* ʹ��Ƭѡ */
	bsp_spiwrite1(CMD_RDSR);							/* ������� ��״̬�Ĵ��� */
	while((bsp_spiread1() & WIP_FLAG) == SET);	/* �ж�״̬�Ĵ�����æ��־λ */
	sf_setcs(1);									/* ����Ƭѡ */
}
