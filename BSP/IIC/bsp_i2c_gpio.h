/*
*********************************************************************************************************
*
*	ģ������ : I2C��������ģ��
*	�ļ����� : bsp_i2c_gpio.h
*	��    �� : V1.0
*
*********************************************************************************************************
*/

#ifndef _BSP_I2C_GPIO_H
#define _BSP_I2C_GPIO_H

#include "stm32f4xx_hal.h"

#define I2C_WR	0		/* д����bit */
#define I2C_RD	1		/* ������bit */


#define GPIO_PORT_SCL_ENABLE()      	__HAL_RCC_GPIOB_CLK_ENABLE()
#define GPIO_PORT_SCL              		GPIOB
#define I2C_SCL_PIN                    	GPIO_PIN_6

#define GPIO_PORT_SDA_ENABLE()     	 	__HAL_RCC_GPIOB_CLK_ENABLE()
#define GPIO_PORT_SDA		           	GPIOB
#define I2C_SDA_PIN                    	GPIO_PIN_7


#define IIC_SCL_H  (GPIO_PORT_SCL->BSRR |= I2C_SCL_PIN)			/* SCL = 1 */
#define IIC_SCL_L  (GPIO_PORT_SCL->BSRR |= I2C_SCL_PIN << 16)			/* SCL = 0 */

#define IIC_SDA_H  (GPIO_PORT_SDA->BSRR |= I2C_SDA_PIN)			/* SDA = 1 */
#define IIC_SDA_L  (GPIO_PORT_SDA->BSRR |= I2C_SDA_PIN << 16)			/* SDA = 0 */

#define I2C_SDA_READ  HAL_GPIO_ReadPin(GPIO_PORT_SDA,I2C_SDA_PIN)	/* ��SDA����״̬ */


void bsp_initiic(void);
void iic_start(void);
void iic_stop(void);
void iic_sendbyte(uint8_t _ucByte);
uint8_t iic_readbyte(uint8_t ack);
uint8_t iic_waitack(void);
void iic_ack(void);
void iic_nack(void);
uint8_t iic_checkdevice(uint8_t _Address);

#endif
