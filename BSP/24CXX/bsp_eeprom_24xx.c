/*
*********************************************************************************************************
*
*	模块名称 : 串行EEPROM 24xx驱动模块
*	文件名称 : bsp_eeprom_24xx.c
*	版    本 : V1.0
*	说    明 : 实现24xx系列EEPROM的读写操作。写操作采用页写模式提高写入效率。
*********************************************************************************************************
*/

/*
	应用说明：访问串行EEPROM前，请先调用一次 bsp_InitI2C()函数配置好I2C相关的GPIO.
*/

#include "bsp.h"
/*
*********************************************************************************************************
*	函 数 名: ee_CheckOk
*	功能说明: 判断串行EERPOM是否正常
*	形    参:  无
*	返 回 值: 1 表示正常， 0 表示不正常
*********************************************************************************************************
*/
uint8_t ee_checkok(void)
{
	if (iic_checkdevice(EE_DEV_ADDR) == 0)
	{
		return 1;
	}
	else
	{
		/* 失败后，切记发送I2C总线停止信号 */
		iic_stop();
		return 0;
	}
}

/*******************************************************************************
* 函 数 名:  ee_read_abyte
* 功    能:  在AT24CXX指定地址读出一个数据
* 参    数:  ReadAddr:开始读数的地址  
*                   
* 输    出:  无
* 返    回: 返回值  :读到的数据
* 主要步骤:  
*******************************************************************************/
uint8_t ee_read_abyte(uint16_t ReadAddr)
{				  
	uint8_t temp=0;		  	    																 
	iic_start();  
	if(EE_TYPE > AT24C16)
	{
		iic_sendbyte(0XA0);	   //发送写命令
		iic_waitack();
		iic_sendbyte(ReadAddr>>8);//发送高地址	    
	}
	else 
	{
		iic_sendbyte(0XA0+((ReadAddr/256)<<1));   //发送器件地址0XA0,写数据 
	}		
	iic_waitack(); 
	iic_sendbyte(ReadAddr%256);   //发送低地址
	iic_waitack();	    
	iic_start();  	 	   
	iic_sendbyte(0XA1);           //进入接收模式			   
	iic_waitack();	 
	temp = iic_readbyte(0);		   
	iic_stop();//产生一个停止条件	    
	return temp;
}

/*
*********************************************************************************************************
*	函 数 名: ee_ReadBytes
*	功能说明: 从串行EEPROM指定地址处开始读取若干数据
*	形    参:  _usAddress : 起始地址
*			 _usSize : 数据长度，单位为字节
*			 _pReadBuf : 存放读到的数据的缓冲区指针
*	返 回 值: 0 表示失败，1表示成功
*********************************************************************************************************
*/
uint8_t ee_readbytes(uint8_t *_pReadBuf, uint16_t _usAddress, uint16_t _usSize)
{
	while(_usSize)
	{
		*_pReadBuf++=ee_read_abyte(_usAddress++);	
		_usSize--;
	}
	return 0;
}
/*******************************************************************************
* 函 数 名:  ee_write_abyte
* 功    能:  在AT24CXX指定地址写入一个数据
* 参    数:  WriteAddr  :写入数据的目的地址  DataToWrite:要写入的数据  
*                   
* 输    出:  无
* 返    回:  无
* 主要步骤:  
*******************************************************************************/
void ee_write_abyte(uint16_t WriteAddr,uint8_t DataToWrite)
{				   	  	    																 
    iic_start();  
    if(EE_TYPE>AT24C16)
    {
        iic_sendbyte(0XA0);	    //发送写命令
        iic_waitack();
        iic_sendbyte(WriteAddr>>8);//发送高地址	  
    }
    else 
    {
        iic_sendbyte(0XA0+((WriteAddr/256)<<1));   //发送器件地址0XA0,写数据 	
    }		
    iic_waitack();	   
    iic_sendbyte(WriteAddr%256);   //发送低地址
    iic_waitack(); 	 										  		   
    iic_sendbyte(DataToWrite);     //发送字节							   
    iic_waitack();  		    	   
    iic_stop();//产生一个停止条件 
    HAL_Delay(10);	 
}
/*
*********************************************************************************************************
*	函 数 名: ee_WriteBytes
*	功能说明: 向串行EEPROM指定地址写入若干数据，采用页写操作提高写入效率
*	形    参:  _usAddress : 起始地址
*			 _usSize : 数据长度，单位为字节
*			 _pWriteBuf : 存放读到的数据的缓冲区指针
*	返 回 值: 0 表示失败，1表示成功
*********************************************************************************************************
*/
uint8_t  ee_writebytes(uint8_t *_pWriteBuf, uint16_t _usAddress, uint16_t _usSize)
{
    while(_usSize--)
    {
        ee_write_abyte(_usAddress,*_pWriteBuf);
        _usAddress++;
        _pWriteBuf++;
    }
	return 0;
}

/***************************** 安富莱电子 www.armfly.com (END OF FILE) *********************************/
