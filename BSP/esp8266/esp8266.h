/*
*********************************************************************************************************
*
*	ģ������ : ESP8266 ����WIFIģ����������
*	�ļ����� : bsp_esp8266.c
*********************************************************************************************************
*/

#ifndef __BSP_ESP8266_H
#define __BSP_ESP8266_H

#include "stm32f4xx_hal.h"

#define COM_ESP8266		COM6		/* ѡ�񴮿� */

#define AT_CR		'\r'
#define AT_LF		'\n'

#define SCK_0()					(PORT_SCK->BSRR |= PIN_SCK << 16)
#define SCK_1()					(PORT_SCK->BSRR |= PIN_SCK)

#define MOSI_0()				(PORT_MOSI->BSRR |= PIN_MOSI << 16)
#define MOSI_1()				(PORT_MOSI->BSRR |= PIN_MOSI)

#define MISO_IS_HIGH()			(HAL_GPIO_ReadPin(PORT_MISO,PIN_MISO) == GPIO_PIN_SET)


#define CH_PD_CLK_ENABLE() 		__HAL_RCC_GPIOA_CLK_ENABLE()
#define PORT_CH_PD				GPIOA
#define PIN_CH_PD				GPIO_PIN_3

#define RESET_CLK_ENABLE() 		__HAL_RCC_GPIOD_CLK_ENABLE()
#define PORT_RESET				GPIOD
#define PIN_RESET				GPIO_PIN_3

#define RESET_GPIO2_ENABLE()	__HAL_RCC_GPIOG_CLK_ENABLE()
#define PORT_GPIO2				GPIOG
#define PIN_GPIO2				GPIO_PIN_15

#define RESET_GPIO0_ENABLE() 	__HAL_RCC_GPIOG_CLK_ENABLE()
#define PORT_GPIO0				GPIOG
#define PIN_GPIO0				GPIO_PIN_8

/* Ӳ������������� -- �� 3.3V ��ʼ����  */
#define ESP_CH_PD_0()	(PORT_CH_PD->BSRR |= PIN_CH_PD << 16)
#define ESP_CH_PD_1()	(PORT_CH_PD->BSRR |= PIN_CH_PD)

/* Ӳ����λ���� -- ���Բ��� */
#define ESP_RESET_0()	(PORT_RESET->BSRR |= PIN_RESET << 16)
#define ESP_RESET_1()	(PORT_RESET->BSRR |= PIN_RESET)

/* 1��ʾ����̼�����ģʽ 0��ʾ����ATָ��ģʽ */
#define ESP_GPIO0_0()	(PORT_GPIO0->BSRR |= PIN_GPIO0 << 16)
#define ESP_GPIO0_1()	(PORT_GPIO0->BSRR |= PIN_GPIO0)
#define ESP_ENTER_ISP()	ESP_GPIO0_0()  /* ����̼�����ģʽ */
#define ESP_EXIT_ISP()	ESP_GPIO0_1()	/* �˳��̼�����ģʽ */

/* ����Ϊ�ߣ���������; */
#define ESP_GPIO2_0()	(PORT_GPIO2->BSRR |= PIN_GPIO2 << 16)
#define ESP_GPIO2_1()	(PORT_GPIO2->BSRR |= PIN_GPIO2)


/* ����������仰, �����յ����ַ����͵����Դ���1 */
#define ESP8266_TO_COM1_EN

#define ESP8266_TMR_ID	(TMR_CNT - 1)

/* �����豸�ṹ�� */
typedef struct
{
	char ssid[33];	/* SSID��һ�����߾������磨WLAN�������ơ�SSID�����ִ�Сд���ı��ַ�������󳤶�32���ַ� */
	uint8_t ecn;	/* ���ܷ�ʽ 
						0   OPEN
						1   WEP
						2   WPA_PSK
						3   WPA2_PSK
						4   WPA_WPA2_PSK
					*/
	int32_t rssi;		/* �ź�ǿ�� */
	uint8_t mac[20];	/* MAC��ַ�ַ���*/
	uint8_t ch;			/* �ŵ� */
}WIFI_AP_T;

/* ���ܷ�ʽ */
enum
{
	ECN_OPEN = 0,
	ECN_WEP = 1,
	ECN_WPA_PSK = 2,
	ECN_WPA2_PSK = 3,
	ECN_WPA_WPA2_PSK = 4,
};

/* ���ⲿ���õĺ������� */
/* ���ⲿ���õĺ������� */
void bsp_InitESP8266(void);
void ESP8266_Reset(void);
void ESP8266_PowerOn(void);
void ESP8266_PowerOff(void);
void ESP8266_EnterISP(void);
void ESP8266_ExitISP(void);
void ESP8266_SendAT(char *_Cmd);

uint8_t ESP8266_WaitResponse(char *_pAckStr, uint16_t _usTimeOut);
void ESP8266_QuitAP(void);
int16_t ESP8266_ScanAP(WIFI_AP_T *_pList, uint16_t _MaxNum);
uint16_t ESP8266_RxNew(uint8_t *_pRxBuf, uint8_t *_link_id);

uint8_t ESP8266_CreateTCPServer(uint16_t _TcpPort);
uint8_t ESP8266_CreateUDPServer(uint8_t _id, uint16_t _LaocalPort);
void ESP8266_SendTcpUdp(uint8_t _id, uint8_t *_databuf, uint16_t _len);
void ESP8266_CloseTcpUdp(uint8_t _id);
uint8_t ESP8266_GetLocalIP(char *_ip, char *_mac);
void ESP8266_QueryIPStatus(void);

uint8_t ESP8266_JoinAP(char *_ssid, char *_pwd, uint16_t _timeout);
uint8_t ESP8266_SetWiFiMode(uint8_t _mode);
uint8_t ESP8266_CIPMUX(uint8_t _mode);
uint8_t ESP8266_LinkTCPServer(uint8_t _id, char *_server_ip, uint16_t _TcpPort);

uint8_t ESP8266_GetChar(uint8_t *_data);
void ESP8266_SendBuf(uint8_t *_cmd, uint16_t _len);

uint8_t ESP8266_Set_AP_IP(char *_ip);
uint8_t ESP8266_Set_AP_NamePass(char *_name, char * _pwd, uint8_t _ch, uint8_t _ecn);

#endif


