/*
*********************************************************************************************************
*
*	ģ������ : ����EEPROM 24xx02����ģ��
*	�ļ����� : bsp_eeprom_24xx.h
*
*********************************************************************************************************
*/

#ifndef _BSP_EEPROM_24XX_H
#define _BSP_EEPROM_24XX_H

#include "stm32f4xx_hal.h"

#define AT24C01		127
#define AT24C02		255
#define AT24C04		511
#define AT24C08		1023
#define AT24C16		2047
#define AT24C32		4095
#define AT24C64	  	8191
#define AT24C128	16383
#define AT24C256	32767  
#define XAT24C128

#define EE_TYPE AT24C128
#ifdef AT24C02
	#define EE_MODEL_NAME		"AT24C02"
	#define EE_DEV_ADDR			0xA0		/* �豸��ַ */
	#define EE_PAGE_SIZE		8			/* ҳ���С(�ֽ�) */
	#define EE_SIZE				256			/* ������(�ֽ�) */
	#define EE_ADDR_BYTES		1			/* ��ַ�ֽڸ��� */
#endif

//#ifdef AT24C128
//	#define EE_MODEL_NAME		"AT24C128"
//	#define EE_DEV_ADDR			0xA0		/* �豸��ַ */
//	#define EE_PAGE_SIZE		64			/* ҳ���С(�ֽ�) */
//	#define EE_SIZE				(16*1024)	/* ������(�ֽ�) */
//	#define EE_ADDR_BYTES		2			/* ��ַ�ֽڸ��� */
//#endif

uint8_t ee_checkok(void);
uint8_t ee_readbytes(uint8_t *_pReadBuf, uint16_t _usAddress, uint16_t _usSize);
uint8_t  ee_writebytes(uint8_t *_pWriteBuf, uint16_t _usAddress, uint16_t _usSize);

#endif




