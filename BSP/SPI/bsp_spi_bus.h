/*
*********************************************************************************************************
*
*	ģ������ : SPI��������
*	�ļ����� : bsp_spi_bus.h
*
*********************************************************************************************************
*/

#ifndef __BSP_SPI_BUS_H
#define __BSP_SPI_BUS_H

#define SOFT_SPI		/* ������б�ʾʹ��GPIOģ��SPI�ӿ� */
#define xHARD_SPI		/* ������б�ʾʹ��CPU��Ӳ��SPI�ӿ� */

#include "stm32f4xx_hal.h"

/*
	STM32Ӳ��SPI�ӿ� = SPI3 ���� SPI1
	����SPI1��ʱ��Դ��90M, SPI3��ʱ��Դ��45M��Ϊ�˻�ø�����ٶȣ������ѡ��SPI1��
*/
#ifdef SOFT_SPI		/* ���SPI */
	/* ����GPIO�˿� */
	#define SPI_SCK_PORT_ENABLE() 	__HAL_RCC_GPIOF_CLK_ENABLE()
	#define PORT_SCK				GPIOF
	#define PIN_SCK					GPIO_PIN_7

	#define SPI_MOSI_PORT_ENABLE() 	__HAL_RCC_GPIOF_CLK_ENABLE()
	#define PORT_MOSI				GPIOF
	#define PIN_MOSI				GPIO_PIN_9

	#define SPI_MISO_PORT_ENABLE() 	__HAL_RCC_GPIOF_CLK_ENABLE()
	#define PORT_MISO				GPIOF
	#define PIN_MISO				GPIO_PIN_8

	#define SCK_0()					(PORT_SCK->BSRR |= PIN_SCK << 16)
	#define SCK_1()					(PORT_SCK->BSRR |= PIN_SCK)

	#define MOSI_0()				(PORT_MOSI->BSRR |= PIN_MOSI << 16)
	#define MOSI_1()				(PORT_MOSI->BSRR |= PIN_MOSI)

	#define MISO_IS_HIGH()			(HAL_GPIO_ReadPin(PORT_MISO,PIN_MISO) == GPIO_PIN_SET)
#endif

#ifdef HARD_SPI
	#define SPI_HARD	SPI1
	#define RCC_SPI		RCC_APB2Periph_SPI1
	
	/* SPI or I2S mode selection masks */
	#define SPI_Mode_Select      ((uint16_t)0xF7FF)
	#define I2S_Mode_Select      ((uint16_t)0x0800) 
	
	/* SPI registers Masks */
	#define CR1_CLEAR_Mask       ((uint16_t)0x3040)
	#define I2SCFGR_CLEAR_Mask   ((uint16_t)0xF040)

	/* SPI SPE mask */
	#define CR1_SPE_Set          ((uint16_t)0x0040)
	#define CR1_SPE_Reset        ((uint16_t)0xFFBF)
#endif
/*
	��SPIʱ�������2��Ƶ����֧�ֲ���Ƶ��
	�����SPI1��2��ƵʱSCKʱ�� = 42M��4��ƵʱSCKʱ�� = 21M
	�����SPI3, 2��ƵʱSCKʱ�� = 21M
*/
#define SPI_SPEED_42M		SPI_BaudRatePrescaler_2
#define SPI_SPEED_21M		SPI_BaudRatePrescaler_4
#define SPI_SPEED_5_2M		SPI_BaudRatePrescaler_8
#define SPI_SPEED_2_6M		SPI_BaudRatePrescaler_16
#define SPI_SPEED_1_3M		SPI_BaudRatePrescaler_32
#define SPI_SPEED_0_6M		SPI_BaudRatePrescaler_64

void bsp_initspibus(void);
void bsp_spiwrite0(uint8_t _ucByte);
uint8_t bsp_spiread0(void);

void bsp_spiwrite1(uint8_t _ucByte);
uint8_t bsp_spiread1(void);

void bsp_spibusenter(void);
void bsp_spibusexit(void);
uint8_t bsp_spibusbusy(void);

void bsp_setspisck(uint8_t _data);
#ifdef HARD_SPI		/* Ӳ��SPI */
	void bsp_SPI_Init(uint16_t _cr1);
#endif

#endif
