/*
*********************************************************************************************************
*
*	模块名称 : I2C总线驱动模块
*	文件名称 : bsp_i2c_gpio.c
*	版    本 : V1.0
*	说    明 : 用gpio模拟i2c总线, 适用于STM32F4系列CPU。该模块不包括应用层命令帧，仅包括I2C总线基本操作函数。
*********************************************************************************************************
*/

/*
	应用说明：
	在访问I2C设备前，请先调用 i2c_CheckDevice() 检测I2C设备是否正常，该函数会配置GPIO
*/

#include "bsp.h"

/*
*********************************************************************************************************
*	函 数 名: i2c_Delay
*	功能说明: I2C总线位延迟，最快400KHz
*	形    参:  无
*	返 回 值: 无
*********************************************************************************************************
*/
static void iic_delay(uint8_t time)
{
   uint16_t i=0;  
   while(time--)
   {
      i=10;  //自己定义
      while(i--) ;    
   }
}

/*
*********************************************************************************************************
*	函 数 名: bsp_InitI2C
*	功能说明: 配置I2C总线的GPIO，采用模拟IO的方式实现
*	形    参:  无
*	返 回 值: 无
*********************************************************************************************************
*/
void bsp_initiic(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;

	GPIO_PORT_SCL_ENABLE();	/* 打开GPIO时钟 */
	GPIO_PORT_SDA_ENABLE();	/* 打开GPIO时钟 */

	GPIO_InitStructure.Pin		= I2C_SCL_PIN;
	GPIO_InitStructure.Mode		= GPIO_MODE_OUTPUT_PP;
	GPIO_InitStructure.Pull		= GPIO_PULLUP;
	GPIO_InitStructure.Speed 	= GPIO_SPEED_FREQ_HIGH;
	HAL_GPIO_Init(GPIO_PORT_SCL, &GPIO_InitStructure);

	GPIO_InitStructure.Pin		= I2C_SDA_PIN;
	GPIO_InitStructure.Mode		= GPIO_MODE_OUTPUT_PP;
	GPIO_InitStructure.Pull		= GPIO_PULLUP;
	GPIO_InitStructure.Speed 	= GPIO_SPEED_FREQ_HIGH;
	HAL_GPIO_Init(GPIO_PORT_SDA, &GPIO_InitStructure);
	
    IIC_SCL_H;
    IIC_SDA_H;
}
/*
*********************************************************************************************************
*	函 数 名: sda_in
*	功能说明: CPU发起I2C总线启动信号
*	形    参:  无
*	返 回 值: 无
*********************************************************************************************************
*/
void sda_in(void)
{
	GPIO_InitTypeDef  GPIO_InitStruct;
	GPIO_PORT_SDA_ENABLE();	/* 打开GPIO时钟 */
	GPIO_InitStruct.Pin 	= I2C_SDA_PIN ;
	GPIO_InitStruct.Mode 	= GPIO_MODE_INPUT;
	GPIO_InitStruct.Pull 	= GPIO_PULLUP;
	HAL_GPIO_Init(GPIO_PORT_SDA, &GPIO_InitStruct);
}
/*
*********************************************************************************************************
*	函 数 名: sda_out
*	功能说明: CPU发起I2C总线启动信号
*	形    参:  无
*	返 回 值: 无
*********************************************************************************************************
*/
void sda_out(void)
{
	GPIO_InitTypeDef  GPIO_InitStructure;
	GPIO_PORT_SDA_ENABLE();	/* 打开GPIO时钟 */
	GPIO_InitStructure.Pin		= I2C_SDA_PIN;
	GPIO_InitStructure.Mode		= GPIO_MODE_OUTPUT_PP;
	GPIO_InitStructure.Pull		= GPIO_PULLUP;
	GPIO_InitStructure.Speed 	= GPIO_SPEED_FREQ_HIGH;
	HAL_GPIO_Init(GPIO_PORT_SDA, &GPIO_InitStructure);
}

/*
*********************************************************************************************************
*	函 数 名: i2c_Start
*	功能说明: CPU发起I2C总线启动信号
*	形    参:  无
*	返 回 值: 无
*********************************************************************************************************
*/
void iic_start(void)
{
	sda_out();
	/* 当SCL高电平时，SDA出现一个下跳沿表示I2C总线启动信号 */
    IIC_SCL_H;
    IIC_SDA_H;
    iic_delay(4);
    IIC_SDA_L;
    iic_delay(4);
    IIC_SCL_L;     //钳住I2C总线，准备发送或接收数据 
}

/*
*********************************************************************************************************
*	函 数 名: i2c_Stop
*	功能说明: CPU发起I2C总线停止信号
*	形    参:  无
*	返 回 值: 无
*********************************************************************************************************
*/
void iic_stop(void)
{
	sda_out();
	IIC_SCL_L;
	IIC_SDA_L;
	iic_delay(4);
	IIC_SCL_H;
	IIC_SDA_H;//发送I2C总线结束信号
	iic_delay(4);
}

/*
*********************************************************************************************************
*	函 数 名: i2c_SendByte
*	功能说明: CPU向I2C总线设备发送8bit数据
*	形    参:  _ucByte ： 等待发送的字节
*	返 回 值: 无
*********************************************************************************************************
*/
void iic_sendbyte(uint8_t _ucByte)
{
	uint8_t i;
	sda_out();
    IIC_SCL_L;//拉低时钟开始数据传输
    for(i=0;i<8;i++)
    {      
        if(_ucByte &0x80)
            IIC_SDA_H;
        else
            IIC_SDA_L;
        iic_delay(2);   //对TEA5767这三个延时都是必须的
        IIC_SCL_H;
        iic_delay(2);   //对TEA5767这三个延时都是必须的
        IIC_SCL_L;	
        if(i == 7)
        {
            IIC_SDA_H;
        }
        _ucByte<<=1; 
        iic_delay(2);
    }	 
}

/*
*********************************************************************************************************
*	函 数 名: i2c_ReadByte
*	功能说明: CPU从I2C总线设备读取8bit数据
*	形    参:  无
*	返 回 值: 读到的数据
*********************************************************************************************************
*/
uint8_t iic_readbyte(uint8_t ack)
{
	uint8_t i;
	uint8_t value = 0;
	sda_in();
	for(i=0;i<8;i++ )
	{
		IIC_SCL_L; 
		iic_delay(2);
		IIC_SCL_H;
		value<<=1;
		if(I2C_SDA_READ)
			value++;   
		iic_delay(1); 
	}					 
	if (!ack)
	{
		iic_nack();//发送nACK
	}
	else
	{
		iic_ack(); //发送ACK   
	}
    return value;
}

/*
*********************************************************************************************************
*	函 数 名: i2c_WaitAck
*	功能说明: CPU产生一个时钟，并读取器件的ACK应答信号
*	形    参:  无
*	返 回 值: 返回0表示正确应答，1表示无器件响应
*********************************************************************************************************
*/
uint8_t iic_waitack(void)
{
	uint8_t ucErrTimer;
	sda_in();
	IIC_SCL_H;
	iic_delay(1);	
	IIC_SDA_H;
	iic_delay(1);	
	
	while(I2C_SDA_READ)
	{
		ucErrTimer++;
		if(ucErrTimer>250)
		{
			iic_stop();
		 return 1;
		}
	}
	IIC_SCL_L;//时钟输出0 	   
	return 0;
}

/*
*********************************************************************************************************
*	函 数 名: i2c_Ack
*	功能说明: CPU产生一个ACK信号
*	形    参:  无
*	返 回 值: 无
*********************************************************************************************************
*/
void iic_ack(void)
{
	IIC_SCL_L;
	sda_out();
	IIC_SDA_L;
	iic_delay(2);
	IIC_SCL_H;
	iic_delay(2);
	IIC_SCL_L;	
}

/*
*********************************************************************************************************
*	函 数 名: i2c_NAck
*	功能说明: CPU产生1个NACK信号
*	形    参:  无
*	返 回 值: 无
*********************************************************************************************************
*/
void iic_nack(void)
{
	IIC_SCL_L;
	sda_out();
	IIC_SDA_H;;
	iic_delay(2);
	IIC_SCL_H;
	iic_delay(2);
	IIC_SCL_L;
}

/*
*********************************************************************************************************
*	函 数 名: i2c_CheckDevice
*	功能说明: 检测I2C总线设备，CPU向发送设备地址，然后读取设备应答来判断该设备是否存在
*	形    参:  _Address：设备的I2C总线地址
*	返 回 值: 返回值 0 表示正确， 返回1表示未探测到
*********************************************************************************************************
*/
uint8_t iic_checkdevice(uint8_t _Address)
{
	uint8_t ucAck;
	
	iic_start();		/* 发送启动信号 */

	/* 发送设备地址+读写控制bit（0 = w， 1 = r) bit7 先传 */
	iic_sendbyte (_Address | I2C_WR);
	ucAck = iic_waitack();	/* 检测设备的ACK应答 */

	iic_stop();			/* 发送停止信号 */

	return ucAck;
}
