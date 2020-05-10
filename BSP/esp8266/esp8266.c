/*
*********************************************************************************************************
*
*	模块名称 : ESP8266 串口WIFI模块驱动程序
*	文件名称 : bsp_esp8266.c
*	版    本 : V1.3
*	说    明 : 封装 ESP8266 模块相关的AT命令
*
*********************************************************************************************************
*/

#include "bsp.h"

char g_EspBuf[2048];	/* 用于解码 */


/*
*********************************************************************************************************
*	函 数 名: bsp_InitESP8266
*	功能说明: 配置无线模块相关的GPIO,  该函数被 bsp_Init() 调用。
*	形    参:  无
*	返 回 值: 无
*********************************************************************************************************
*/
void bsp_InitESP8266(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;

	/* 打开GPIO时钟 */
	CH_PD_CLK_ENABLE();
	RESET_CLK_ENABLE();
	RESET_GPIO2_ENABLE();
	RESET_GPIO0_ENABLE();
	
	GPIO_InitStructure.Mode  = GPIO_MODE_OUTPUT_PP;		/* 设为输出口 */
	GPIO_InitStructure.Pull  = GPIO_PULLUP;	/* 上下拉电阻不使能 */
	GPIO_InitStructure.Speed = GPIO_SPEED_FREQ_VERY_HIGH;	/* IO口最大速度 */

	GPIO_InitStructure.Pin = PIN_CH_PD;
	HAL_GPIO_Init(PORT_CH_PD, &GPIO_InitStructure);

	GPIO_InitStructure.Pin = PIN_RESET;
	HAL_GPIO_Init(PORT_RESET, &GPIO_InitStructure);

	GPIO_InitStructure.Pin = PIN_GPIO0;
	HAL_GPIO_Init(PORT_GPIO0, &GPIO_InitStructure);

	GPIO_InitStructure.Pin = PIN_GPIO2;
	HAL_GPIO_Init(PORT_GPIO2, &GPIO_InitStructure);	

	ESP_GPIO2_1();
	
	/* CPU的串口配置已经由 bsp_uart_fifo.c 中的 bsp_InitUart() 做了 */
	ESP_CH_PD_0();

	ESP_EXIT_ISP();
	
	//ESP8266_Reset();

	comsetbaud(COM_ESP8266,115200);
}
/*
*********************************************************************************************************
*	函 数 名: ESP8266_SendBuf
*	功能说明: 发送数据到WiFi模块
*	形    参: _cmd : 数组
*			 _len : 数据长度
*	返 回 值: 无
*********************************************************************************************************
*/
void ESP8266_SendBuf(uint8_t * _cmd, uint16_t _len)
{
	#ifdef ESP8266_TO_COM1_EN
		comsendbuf(COM1, "=>", 3);		/* 打印主机发送的标识符 => */
		comsendbuf(COM1, _cmd, _len);		/* 将接收到数据打印到调试串口1 */
	#endif

	comsendbuf(COM_ESP8266, _cmd, _len);	
}

/*
*********************************************************************************************************
*	函 数 名: ESP8266_GetChar
*	功能说明: 获取WiFi模块
*	形    参: _data : 存放数据的变量地址
*	返 回 值: 0 表示无数据，1表示有数据。 数据存放在 *_data
*********************************************************************************************************
*/
uint8_t ESP8266_GetChar(uint8_t *_data)
{
	uint8_t re;

	re = comgetchar(COM_ESP8266, _data);
	if (re != 0)
	{
		#ifdef ESP8266_TO_COM1_EN
			comgetchar(COM1, _data);		/* 将接收到数据打印到调试串口1 */
		#endif
		
		return 1;
	}
	return 0;
}
/*
*********************************************************************************************************
*	函 数 名: ESP8266_PrintRxData
*	功能说明: 打印STM32从ESP8266收到的数据到COM1串口，主要用于跟踪调试
*	形    参: _ch : 收到的数据
*	返 回 值: 无
*********************************************************************************************************
*/
void ESP8266_PrintRxData(uint8_t _ch)
{
	#ifdef ESP8266_TO_COM1_EN
		comsendchar(COM1, _ch);		/* 将接收到数据打印到调试串口1 */
	#endif
}
/*
*********************************************************************************************************
*	函 数 名: ESP8266_PowerOn
*	功能说明: 给ESP8266模块上电
*	形    参: 无
*	返 回 值: 无
*********************************************************************************************************
*/
void ESP8266_PowerOn(void)
{
	/* WIFI模块上电时，会以74880波特率打印如下信息:
		 ets Jan  8 2013,rst cause:1, boot mode:(3,6)

		load 0x40100000, len 25052, room 16 
		tail 12
		chksum 0x0b
		ho 0 tail 12 room 4
		load 0x3ffe8000, len 3312, room 12 
		tail 4
		chksum 0x53
		load 0x3ffe8cf0, len 6576, room 4 
		tail 12
		chksum 0x0d
		csum 0x0d	    <-----  程序识别 csum 后，再自动切换到正常波特率 
	*/

	ESP_CH_PD_0();
	
	bsp_delayms(20);

	comsetbaud(COM_ESP8266, 74880);	/* 设置端口波特率为 74880 */

	ESP_CH_PD_1();

	ESP8266_Reset();	

	/* 等待模块完成上电，超时500ms 自动退出 */
	ESP8266_WaitResponse("user1", 1000);	/* 先等待 user1 */
	ESP8266_WaitResponse("\n", 1000);			/* 再等待回车换行字符结束 */
	
	//comSetBaud(COM_ESP8266,115200);
	comsetbaud(COM_ESP8266, 115200);		
	
	/* 等待模块完成上电，判断是否接收到 ready */
	ESP8266_WaitResponse("ready", 5000);

	// ESP8266_SendAT("AT+RST");
	// ESP8266_WaitResponse("ready", 5000);
	
	/* 关闭回显功能，主机发送的字符，模块无需返回 */
	//ESP8266_SendAT("ATE0");		
	//ESP8266_WaitResponse("OK\r\n", 100);
}
/*
*********************************************************************************************************
*	函 数 名: ESP8266_PowerOff
*	功能说明: 控制ESP8266模块关机
*	形    参: 无
*	返 回 值: 无
*********************************************************************************************************
*/
void ESP8266_PowerOff(void)
{
	ESP_CH_PD_0();
}
#if 1
	/*
	*********************************************************************************************************
	*	函 数 名: ESP8266_PowerOn
	*	功能说明: 给ESP8266模块上电
	*	形    参: 无
	*	返 回 值: 无
	*********************************************************************************************************
	*/
	void ESP8266_Reset(void)
	{
		ESP_RESET_0();
		bsp_delayms(20);
		ESP_RESET_1();

		bsp_delayms(10);
	}

	/*
	*********************************************************************************************************
	*	函 数 名: ESP8266_EnterISP
	*	功能说明: 控制ESP8266模块进入固件升级模式
	*	形    参: 无
	*	返 回 值: 无
	*********************************************************************************************************
	*/
	void ESP8266_EnterISP(void)
	{
		ESP_CH_PD_0();
		ESP_GPIO0_0();  /* 0 表示进入固件升级模式 */
		ESP_CH_PD_1();
	}

	/*
	*********************************************************************************************************
	*	函 数 名: ESP8266_ExitISP
	*	功能说明: 控制ESP8266模块退出固件升级模式
	*	形    参: 无
	*	返 回 值: 无
	*********************************************************************************************************
	*/
	void ESP8266_ExitISP(void)
	{
		ESP_CH_PD_0();
		ESP_GPIO0_1();  /* 1 表示进入用户程序（AT指令）模式 */
		ESP_CH_PD_1();
	}
#endif
/*
*********************************************************************************************************
*	函 数 名: ESP8266_9600to115200
*	功能说明: 9600波特率切换到115200
*	形    参: 无
*	返 回 值: 无
*********************************************************************************************************
*/
void ESP8266_9600to115200(void)
{
	comsetbaud(COM_ESP8266, 9600);			/* 模块缺省是9600bps */
	ESP8266_SendAT("AT+CIOBAUD=115200");	/* 按 9600bps 发送指令切换为 115200 */
	ESP8266_WaitResponse("OK\r\n", 2000);		/* 这个 OK 是模块按 9600 应答的 */
	comsetbaud(COM_ESP8266, 115200);		/* 切换STM32的波特率为 115200 */

	/* 切换为 Station模式 */
	bsp_delayms(100);
	ESP8266_SendAT("AT+CWMODE=1");
	ESP8266_WaitResponse("OK\r\n", 2000);
	bsp_delayms(1500);
	ESP8266_SendAT("AT+RST");
}	
/*
*********************************************************************************************************
*	函 数 名: ESP8266_WaitResponse
*	功能说明: 等待ESP8266返回指定的应答字符串, 可以包含任意字符。只要接收齐全即可返回。
*	形    参: _pAckStr : 应答的字符串， 长度不得超过255
*			 _usTimeOut : 命令执行超时，0表示一直等待. >０表示超时时间，单位1ms
*	返 回 值: 1 表示成功  0 表示失败
*********************************************************************************************************
*/
uint8_t ESP8266_WaitResponse(char *_pAckStr, uint16_t _usTimeOut)
{
	uint8_t ucData;
	uint16_t pos = 0;
	uint32_t len;
	uint8_t ret;

	len = strlen(_pAckStr);
	if (len > 255)
	{
		return 0;
	}

	/* _usTimeOut == 0 表示无限等待 */
	if (_usTimeOut > 0)
	{
		bsp_start_timer(ESP8266_TMR_ID, _usTimeOut);		/* 使用软件定时器3，作为超时控制 */
	}
	while (1)
	{
		bsp_idle();				/* CPU空闲执行的操作， 见 bsp.c 和 bsp.h 文件 */

		if (_usTimeOut > 0)
		{
			if (bsp_checktimer(ESP8266_TMR_ID))
			{
				ret = 0;	/* 超时 */
				break;
			}
		}

		if (ESP8266_GetChar(&ucData))
		{
			if (ucData == _pAckStr[pos])
			{
				pos++;

				if (pos == len)
				{
					ret = 1;	/* 收到指定的应答数据，返回成功 */
					break;
				}
			}
			else
			{
				pos = 0;
			}
		}
	}
	return ret;
}	
/*
*********************************************************************************************************
*	函 数 名: ESP8266_ReadLine
*	功能说明: 读取ESP8266返回的一行应答字符串(0x0D 0x0A结束)。该函数根据字符间超时判断结束。 本函数需要紧跟AT命令发送函数。
*	形    参: _pBuf : 存放模块返回的完整字符串
*			  _usBufSize : 缓冲区最大长度
*			 _usTimeOut : 命令执行超时，0表示一直等待. >0 表示超时时间，单位1ms
*	返 回 值: 0 表示错误（超时）  > 0 表示应答的数据长度
*********************************************************************************************************
*/
uint16_t ESP8266_ReadLine(char *_pBuf, uint16_t _usBufSize, uint16_t _usTimeOut)
{
	uint8_t ucData;
	uint16_t pos = 0;
	uint8_t ret;

	/* _usTimeOut == 0 表示无限等待 */
	if (_usTimeOut > 0)
	{
		bsp_start_timer(ESP8266_TMR_ID, _usTimeOut);		/* 使用软件定时器作为超时控制 */
	}
	while (1)
	{
		bsp_idle();				/* CPU空闲执行的操作， 见 bsp.c 和 bsp.h 文件 */

		if (bsp_checktimer(ESP8266_TMR_ID))
		{
			_pBuf[pos] = 0;	/* 结尾加0， 便于函数调用者识别字符串结束 */
			ret = pos;		/* 成功。 返回数据长度 */
			break;
		}

		if (ESP8266_GetChar(&ucData))
		{
			bsp_start_timer(ESP8266_TMR_ID, 500);
			_pBuf[pos++] = ucData;		/* 保存接收到的数据 */
			if (ucData == 0x0A)
			{
				_pBuf[pos] = 0;
				ret = pos;		/* 成功。 返回数据长度 */
				break;
			}
		}
	}
	return ret;
}
/*
*********************************************************************************************************
*	函 数 名: ESP8266_SendAT
*	功能说明: 向模块发送AT命令。 本函数自动在AT字符串口增加<CR>字符
*	形    参: _Str : AT命令字符串，不包括末尾的回车<CR>. 以字符0结束
*	返 回 值: 无T
*********************************************************************************************************
*/
void ESP8266_SendAT(char *_Cmd)
{
	//comClearRxFifo(COM_ESP8266);
	ESP8266_SendBuf((uint8_t *)_Cmd, strlen(_Cmd));
	ESP8266_SendBuf("\r\n", 2);
}
	
/*
*********************************************************************************************************
*	函 数 名: ESP8266_SetWiFiMode
*	功能说明: 设置WiFi模块工作模式
*	形    参: _mode : 1 = Station模式,  2 = AP模式,  3 = AP兼Station模式
*	返 回 值: 0 表示失败。 1表示成功
*********************************************************************************************************
*/
uint8_t ESP8266_SetWiFiMode(uint8_t _mode)
{
	char cmd_buf[30];
	
	if (_mode == 0 || _mode > 3)
	{
		_mode = 3;
	}
	sprintf(cmd_buf, "AT+CWMODE=%d", _mode);
	ESP8266_SendAT(cmd_buf);
	if (ESP8266_WaitResponse("OK\r\n", 2000) == 0)
	{
		return 0;
	}
	
	return 1;
}
/*
*********************************************************************************************************
*	函 数 名: ESP8266_CIPMUX
*	功能说明: 启动多连接模式
*	形    参: _mode : 0,表示关闭， 1表示启动
*	返 回 值: 0 表示失败。 1表示成功
*********************************************************************************************************
*/
uint8_t ESP8266_CIPMUX(uint8_t _mode)
{
	char cmd_buf[30];
	
	if (_mode > 0)
	{
		_mode = 1;
	}
	sprintf(cmd_buf, "AT+CIPMUX=%d", _mode);
	ESP8266_SendAT(cmd_buf);
	if (ESP8266_WaitResponse("OK\r\n", 200) == 0)
	{
		return 0;
	}
	
	return 1;
}
/*
*********************************************************************************************************
*	函 数 名: ESP8266_Set_AP_IP
*	功能说明: AT+CIPAP 设置 AP 的 IP 地
*	形    参: _ip :AP的IP地址，标准字符串
*	返 回 值: 0 表示失败。 1表示成功
*********************************************************************************************************
*/
uint8_t ESP8266_Set_AP_IP(char *_ip)
{
	char cmd_buf[30];
	
	sprintf(cmd_buf, "AT+CIPAP=\"%s\"", _ip);
	ESP8266_SendAT(cmd_buf);
	if (ESP8266_WaitResponse("OK\r\n", 500) == 0)
	{
		return 0;
	}
	
	return 1;
}
/*
*********************************************************************************************************
*	函 数 名: ESP8266_Set_AP_NamePass
*	功能说明: 设置SoftAP的名字，加密方式和密码.  加密方式为 。 
*	形    参: _name :AP的名字，字符串参数，密码最长 64 字节 ASCII
*			 _pwd : AP的密码，字符串参数，密码最长 64 字节 ASCII
*			 _ch : 通道号
*	返 回 值: 0 表示失败。 1表示成功
*********************************************************************************************************
*/
uint8_t ESP8266_Set_AP_NamePass(char *_name, char * _pwd, uint8_t _ch, uint8_t _ecn)
{
	char cmd_buf[40];

	/* AT+CWSAP="ESP8266","1234567890",5,3 */	
	sprintf(cmd_buf, "AT+CWSAP=\"%s\",\"%s\",%d,%d", _name, _pwd, _ch, _ecn);
	ESP8266_SendAT(cmd_buf);
	if (ESP8266_WaitResponse("OK\r\n", 500) == 0)
	{
		return 0;
	}
	
	return 1;
}
/*
*********************************************************************************************************
*	函 数 名: ESP8266_CreateTCPServer
*	功能说明: 创建一个TCP服务端。 必须在连接到AP之后才行。 需要先启用多连接
*	形    参：_TcpPort : TCP 端口号
*	返 回 值: 0 表示失败。 1表示创建TCP成功
*********************************************************************************************************
*/
uint8_t ESP8266_CreateTCPServer(uint16_t _TcpPort)
{
	char cmd_buf[30];
	
	/* 开启TCP server, 端口为 _TcpPort */
	sprintf(cmd_buf, "AT+CIPSERVER=1,%d", _TcpPort);
	ESP8266_SendAT(cmd_buf);	
	if (ESP8266_WaitResponse("OK\r\n", 2000) == 0)
	{
		return 0;
	}
	
	return 1;
}


/*
*********************************************************************************************************
*	函 数 名: ESP8266_CreateUDPServer
*	功能说明: 创建一个TUDP服务端。 必须在连接到AP之后才行。 需要先启用多连接
*	形    参:   _id : 连接ID, 0-4
*				_LaocalPort : UDP 端口号
*	返 回 值: 0 表示失败。 1表示创建TCP成功
*********************************************************************************************************
*/
uint8_t ESP8266_CreateUDPServer(uint8_t _id, uint16_t _LaocalPort)
{
	char cmd_buf[64];
	
	/* 多连接 UDP */
	//AT+CIPSTART=0,"UDP","255.255.255.255",8080,8080,0           <----- 没有试通
	sprintf(cmd_buf, "AT+CIPSTART=%d,\"UDP\",\"255.255.255.255\",8080,%d,2", _id, _LaocalPort);
	
	// AT+CIPSTART="UDP","255.255.255.255",8080,8080,1	
	//sprintf(cmd_buf, "AT+CIPSTART=\"UDP\",\"255.255.255.255\",8080,%d,2", _LaocalPort);
	
	ESP8266_SendAT(cmd_buf);	
	if (ESP8266_WaitResponse("OK\r\n", 3000) == 0)
	{
		return 0;
	}
	
	return 1;
}
/*
*********************************************************************************************************
*	函 数 名: ESP8266_LinkTCPServer
*	功能说明: 连接到一个TCP服务端。  仅(+CIPMUX=1) 多连接模式。
*	形    参: _id : 连接的id号
*			  _server_ip : 服务器域名或IP地址
*			  _TcpPort : TCP 端口号
*	返 回 值: 0 表示失败。 1表示创建TCP成功
*********************************************************************************************************
*/
uint8_t ESP8266_LinkTCPServer(uint8_t _id, char *_server_ip, uint16_t _TcpPort)
{
	char cmd_buf[30];
	
#if 0	/* 单连接 */
	//AT+CIPSTART="TCP","192.168.101.110",1000
	sprintf(cmd_buf, "AT+CIPSTART=\"TCP\",\"%s\",%d",_server_ip, _TcpPort);
#else	/* 多连接 */
	//AT+CIPSTART=0, "TCP","192.168.101.110",1000
	sprintf(cmd_buf, "AT+CIPSTART=%d,\"TCP\",\"%s\",%d", _id, _server_ip, _TcpPort);
#endif	
	ESP8266_SendAT(cmd_buf);	
	if (ESP8266_WaitResponse("OK\r\n", 3000) == 0)
	{
		return 0;
	}
	
	return 1;
}

/*
*********************************************************************************************************
*	函 数 名: ESP8266_SendTcpUdp
*	功能说明: 发送TCP或UDP数据包
*	形    参: _id : 多连接时，连接ID （0-4）
*			  _databuf 数据
*			  _len 数据长度
*	返 回 值: 无
*********************************************************************************************************
*/
void ESP8266_SendTcpUdp(uint8_t _id, uint8_t *_databuf, uint16_t _len)
{
	char buf[32];

	if (_len > 2048)
	{
		_len = 2048;
	}

	sprintf(buf, "AT+CIPSEND=%d,%d\r\n",_id, _len);
	ESP8266_SendBuf((uint8_t *)buf, strlen(buf));

	ESP8266_WaitResponse(">", 1000);

	ESP8266_SendBuf(_databuf, _len);
	ESP8266_WaitResponse("SEND OK", 8000);
}

/*
*********************************************************************************************************
*	函 数 名: ESP8266_CloseTcpUdp
*	功能说明: 关闭TCP或UDP连接. 用于多路连接
*	形    参: _databuf 数据
*			 _len 数据长度
*	返 回 值: 无
*********************************************************************************************************
*/
void ESP8266_CloseTcpUdp(uint8_t _id)
{
	char buf[32];

	//ESP8266_SendAT("ATE0");		/* 关闭回显功能 */
	//ESP8266_WaitResponse("SEND OK", 50);
	
	sprintf(buf, "AT+CIPCLOSE=%d", _id);
	ESP8266_SendAT(buf);	
	ESP8266_WaitResponse("OK", 200);
	
	/* ZHG : ---- 此处需要处理应答 */
}


/*
*********************************************************************************************************
*	函 数 名: ESP8266_QueryIPStatus
*	功能说明: 查询当前连接状态。（0-4）. 还不完善，未解析出当前链接的id
*	形    参: 无
*	返 回 值: 无
*********************************************************************************************************
*/
void ESP8266_QueryIPStatus(void)
{
	char buf[32];
	
	sprintf(buf, "AT+CIPSTATUS");
	ESP8266_SendAT(buf);
	
	ESP8266_WaitResponse("OK", 1000);
	
}
/*
*********************************************************************************************************
*	函 数 名: ESP8266_GetLocalIP
*	功能说明: 查询本机IP地址和MAC
*	形    参: _ssid : AP名字字符串
*			  _pwd : 密码字符串
*	返 回 值: 1 表示OK， 0 表示未知
*********************************************************************************************************
*/
uint8_t ESP8266_GetLocalIP(char *_ip, char *_mac)
{
	char buf[64];
	uint8_t i, m;
	uint8_t ret = 0;
	uint8_t temp;
	
	ESP8266_SendAT("AT+CIFSR");
	
	/*　模块将应答:
		
	+CIFSR:STAIP,"192.168.1.18"
	+CIFSR:STAMAC,"18:fe:34:a6:44:75"
	
	OK	
	*/
	
	_ip[0] = 0;
	_mac[0] = 0;
	for (i = 0; i < 4; i++)
	{
		ESP8266_ReadLine(buf, sizeof(buf), 500);
		if (memcmp(buf, "+CIFSR:STAIP", 12) == 0)
		{
			
			for (m = 0; m < 20; m++)
			{
				temp = buf[14 + m];
				_ip[m] = temp;
				if (temp == '"')
				{
					_ip[m] = 0;
					ret = 1;
					break;
				}
			}
		}
		else if (memcmp(buf, "+CIFSR:STAMAC,", 14) == 0)
		{
			for (m = 0; m < 20; m++)
			{
				temp = buf[15 + m];
				_mac[m] = temp;
				if (temp == '"')
				{
					_mac[m] = 0;
					break;
				}
			}
		}
		else if (memcmp(buf, "OK", 2) == 0)
		{
			break;
		}
	}
	return ret;
}

/*
*********************************************************************************************************
*	函 数 名: ESP8266_JoinAP
*	功能说明: 加入AP
*	形    参: _ssid : AP名字字符串
*			  _pwd : 密码字符串
*	返 回 值: 1 表示 0K  0 表示失败
*********************************************************************************************************
*/
uint8_t ESP8266_JoinAP(char *_ssid, char *_pwd, uint16_t _timeout)
{
	char buf[64];

	sprintf(buf, "AT+CWJAP=\"%s\",\"%s\"", _ssid, _pwd);
	ESP8266_SendAT(buf);
	
	if (ESP8266_ReadLine(buf, 64, _timeout))
	{
		/* ATE1的情况，有命令回显的情况 */
		if (memcmp(buf, "AT+CWJAP", 8) == 0)		/* 第1次读到的是 命令本身 */
		{
			ESP8266_ReadLine(buf, 64, _timeout);	/* 这个是回车 */
			ESP8266_ReadLine(buf, 64, _timeout);	/* 这次是读应答的OK */
			
			if (memcmp(buf, "OK", 2) == 0)
			{
				return 1;
			}
		}		
		else	/* ATE0 无回显的情况 */
		{
			ESP8266_WaitResponse("OK\r\n", _timeout);
		}

	}	
	return 0;
}
/*
*********************************************************************************************************
*	函 数 名: ESP8266_QuitAP
*	功能说明: 退出当前的AP连接
*	形    参: 无
*	返 回 值: 无
*********************************************************************************************************
*/
void ESP8266_QuitAP(void)
{
	ESP8266_SendAT("AT+ CWQAP");
}

/*
*********************************************************************************************************
*	函 数 名: ESP8266_ScanAP
*	功能说明: 扫描AP。结果存放在_pList 结构体数组. 此函数会占用最长5秒时间。直到收到OK或ERROR。
*	形    参: _pList : AP列表数组;
*			  _MaxNum : 搜索的最大AP个数。主要是防止缓冲区溢出。
*	返 回 值: -1 表示失败; 0 表示搜索到0个; 1表示1个。
*********************************************************************************************************
*/
int16_t ESP8266_ScanAP(WIFI_AP_T *_pList, uint16_t _MaxNum)
{
	uint16_t i;
	uint16_t count;
	char buf[128];
	WIFI_AP_T *p;
	char *p1, *p2;
	uint16_t timeout;

	buf[127] = 0;
	ESP8266_SendAT("AT+CWLAP");

	p = (WIFI_AP_T *)_pList;
	count = 0;
	timeout = 8000;
	for (i = 0; i < _MaxNum; i++)
	{
		ESP8266_ReadLine(buf, 128, timeout);
		if (memcmp(buf, "OK", 2) == 0)
		{
			break;
		}
		else if (memcmp(buf, "ERROR", 5) == 0)
		{
			break;
		}
		else if (memcmp(buf, "+CWLAP:", 7) == 0)
		{
			p1 = buf;

			/* +CWLAP:(4,"BaiTu",-87,"9c:21:6a:3c:89:52",1) */
			/* 解析加密方式 */
			p1 = strchr(p1, '(');	/* 搜索到(*/
			p1++;
			p->ecn = str_to_int(p1);

			/* 解析ssid */
			p1 = strchr(p1, '"');	/* 搜索到第1个分号 */
			p1++;
			p2 = strchr(p1, '"');	/* 搜索到第2个分号 */
			memcpy(p->ssid, p1, p2 - p1);
			p->ssid[p2 - p1] = 0;

			/* 解析 rssi */
			p1 = strchr(p2, ',');	/* 搜索到逗号*/
			p1++;
			p->rssi = str_to_int(p1);

			/* 解析mac */
			p1 = strchr(p1, '"');	/* 搜索到分号*/
			p1++;
			p2 = strchr(p1, '"');	/* 搜索到分号*/
			memcpy(p->mac, p1, p2 - p1);
			p->mac[p2 - p1] = 0;

			/* 解析ch */
			p1 = strchr(p2, ',');	/* 搜索到逗号*/
			p1++;
			p->ch = str_to_int(p1);

			/* 有效的AP名字 */
			count++;

			p++;
			
			timeout = 2000;
		}
	}

	return count;
}

/*
*********************************************************************************************************
*	函 数 名: ESP8266_RxNew
*	功能说明: 接收新数据帧 +IPD
*	形    参: _pRxBuf : 接收到的数据存放在此缓冲区
*			 _link_id : TCP连接的id， 识别是局域网手机，还是来自于服务器
*	返 回 值: 接收到的数据长度. 0 表示无数据
*********************************************************************************************************
*/
uint16_t ESP8266_RxNew(uint8_t *_pRxBuf, uint8_t *_link_id)
{
	uint8_t ucData;
	static uint8_t s_buf[2048];	/* 约定最长1046 */
	static uint16_t s_len = 0;
	static uint8_t s_flag = 0;
	static uint16_t s_data_len = 0;
	char *p1;
	
	/* +IPD,0,7:ledon 1 */

	while (ESP8266_GetChar(&ucData))
	{
		if (s_flag == 0)
		{
			if (s_len < sizeof(s_buf))
			{
				s_buf[s_len++] = ucData;		/* 保存接收到的数据 */
			}			
			if (ucData == '+')
			{
				s_len = 1;
				s_data_len = 0;
				s_buf[0] = 0;
			}
			if (s_len > 7 && ucData == ':')
			{
				p1 = (char *)&s_buf[5];
				*_link_id = str_to_int(p1);		/* 解析出连接id */
				
				p1 = (char *)&s_buf[7];
				s_data_len = str_to_int(p1);	/* 解析出数据包长度 */
				s_flag = 1;	
				s_len = 0;
			}
		}
		else
		{
			if (s_len < sizeof(s_buf))
			{
				s_buf[s_len++] = ucData;		/* 保存接收到的数据 */
				
				if (s_len == s_data_len)
				{
					s_flag = 0;
					s_len = 0;
					
					memcpy(_pRxBuf, s_buf, s_data_len);
					
					return s_data_len;
				}
			}	
			else
			{
				s_flag = 0;
			}
		}

	}
	return 0;
}


