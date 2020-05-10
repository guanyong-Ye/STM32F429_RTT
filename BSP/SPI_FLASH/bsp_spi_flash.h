/*
*********************************************************************************************************
*
*	模块名称 : SPI接口串行FLASH 读写模块
*	文件名称 : bsp_spi_flash.h
*
*********************************************************************************************************
*/

#ifndef _BSP_SPI_FLASH_H
#define _BSP_SPI_FLASH_H

#include "stm32f4xx_hal.h"

#define SF_MAX_PAGE_SIZE	(16 * 1024)

/* 串行Flsh的片选GPIO端口  */
#define SPI_CS_PORT_ENABLE() 	__HAL_RCC_GPIOF_CLK_ENABLE()
#define SF_PORT_CS				GPIOF
#define SF_PIN_CS					GPIO_PIN_6

#define SF_CS_0()			(SF_PORT_CS->BSRR |= SF_PIN_CS << 16)
#define SF_CS_1()			(SF_PORT_CS->BSRR |= SF_PIN_CS)

#define CMD_AAI       0xAD  	/* AAI 连续编程指令(FOR SST25VF016B) */
#define CMD_DISWR	  0x04		/* 禁止写, 退出AAI状态 */
#define CMD_EWRSR	  0x50		/* 允许写状态寄存器的命令 */
#define CMD_WRSR      0x01  	/* 写状态寄存器命令 */
#define CMD_WREN      0x06		/* 写使能命令 */
#define CMD_READ      0x03  	/* 读数据区命令 */
#define CMD_RDSR      0x05		/* 读状态寄存器命令 */
#define CMD_RDID      0x9F		/* 读器件ID命令 */
#define CMD_SE        0x20		/* 擦除扇区命令 */
#define CMD_BE        0xC7		/* 批量擦除命令 */
#define DUMMY_BYTE    0xA5		/* 哑命令，可以为任意值，用于读操作 */

#define WIP_FLAG      0x01		/* 状态寄存器中的正在编程标志（WIP) */


/* 定义串行Flash ID */
enum
{
	SST25VF016B_ID 	= 0xBF2541,
	MX25L1606E_ID  	= 0xC22015,
	W25Q64BV_ID    	= 0xEF4017,
	W25Q128FV_ID	= 0XEF4018,
};

typedef struct
{
	uint32_t ChipID;		/* 芯片ID */
	char ChipName[16];		/* 芯片型号字符串，主要用于显示 */
	uint32_t TotalSize;		/* 总容量 */
	uint16_t PageSize;		/* 页面大小 */
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


