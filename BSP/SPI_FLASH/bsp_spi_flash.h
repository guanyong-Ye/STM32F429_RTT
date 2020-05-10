/*
*********************************************************************************************************
*
*	ģ������ : SPI�ӿڴ���FLASH ��дģ��
*	�ļ����� : bsp_spi_flash.h
*
*********************************************************************************************************
*/

#ifndef _BSP_SPI_FLASH_H
#define _BSP_SPI_FLASH_H

#include "stm32f4xx_hal.h"

#define SF_MAX_PAGE_SIZE	(16 * 1024)

/* ����Flsh��ƬѡGPIO�˿�  */
#define SPI_CS_PORT_ENABLE() 	__HAL_RCC_GPIOF_CLK_ENABLE()
#define SF_PORT_CS				GPIOF
#define SF_PIN_CS					GPIO_PIN_6

#define SF_CS_0()			(SF_PORT_CS->BSRR |= SF_PIN_CS << 16)
#define SF_CS_1()			(SF_PORT_CS->BSRR |= SF_PIN_CS)

#define CMD_AAI       0xAD  	/* AAI �������ָ��(FOR SST25VF016B) */
#define CMD_DISWR	  0x04		/* ��ֹд, �˳�AAI״̬ */
#define CMD_EWRSR	  0x50		/* ����д״̬�Ĵ��������� */
#define CMD_WRSR      0x01  	/* д״̬�Ĵ������� */
#define CMD_WREN      0x06		/* дʹ������ */
#define CMD_READ      0x03  	/* ������������ */
#define CMD_RDSR      0x05		/* ��״̬�Ĵ������� */
#define CMD_RDID      0x9F		/* ������ID���� */
#define CMD_SE        0x20		/* ������������ */
#define CMD_BE        0xC7		/* ������������ */
#define DUMMY_BYTE    0xA5		/* ���������Ϊ����ֵ�����ڶ����� */

#define WIP_FLAG      0x01		/* ״̬�Ĵ����е����ڱ�̱�־��WIP) */


/* ���崮��Flash ID */
enum
{
	SST25VF016B_ID 	= 0xBF2541,
	MX25L1606E_ID  	= 0xC22015,
	W25Q64BV_ID    	= 0xEF4017,
	W25Q128FV_ID	= 0XEF4018,
};

typedef struct
{
	uint32_t ChipID;		/* оƬID */
	char ChipName[16];		/* оƬ�ͺ��ַ�������Ҫ������ʾ */
	uint32_t TotalSize;		/* ������ */
	uint16_t PageSize;		/* ҳ���С */
}flash_t;

void bsp_initsflash(void);
void sf_erasesector(uint32_t _uiSectorAddr);
void sf_erasechip(void);
void sf_pagewrite(uint8_t * _pBuf, uint32_t _uiWriteAddr, uint16_t _usSize);
void sf_readbuffer(uint8_t * _pBuf, uint32_t _uiReadAddr, uint32_t _uiSize);


uint32_t sf_readid(void);
uint8_t sf_writebuffer(uint8_t* _pBuf, uint32_t _uiWriteAddr, uint16_t _usWriteSize);
void sf_readinfo(void);

extern flash_t g_tsf;

#endif


