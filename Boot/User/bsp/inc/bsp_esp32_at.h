/*
*********************************************************************************************************
*
*	ģ������ : ESP32 ����WIFIģ����������
*	�ļ����� : bsp_esp32_at.h
*	��    �� : V1.3
*	˵    �� : ͷ�ļ�
*
*	Copyright (C), 2015-2020, ���������� www.armfly.com
*
*********************************************************************************************************
*/

#ifndef __BSP_ESP32_H
#define __BSP_ESP32_H

#define COM_ESP32		COM4		/* ESP32���� */
#define COM_DEBUG		COM2		/* ���Դ��� */

/* ����������仰, �����յ����ַ����͵����Դ���1 */
#define ESP32_TO_COM1_EN

#define SSID_MAX_LEN		32		/* SSID�32���ַ����洢ʱ��Ҫ32+1�ռ䣬ĩβ��0 */
#define PASSWORD_MAX_LEN	64		/* WIFI ���� �64���ַ����洢ʱ��Ҫ32+1�ռ䣬ĩβ��0 */

/* �豸�ṹ�� */
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

/* ESP32_PT_WaitResonse �����ķ���ֵ���� */
enum
{
	PT_NULL = 0,	/* �ղ�������Ҫ�����ȴ� */
	PT_TIMEOUT,		/* ִ�г�ʱ */ 
	
	PT_OK,			/* �ɹ�ִ�� */
	PT_ERR,			/* ִ��ʧ�� */
};

/* ESP32_QueryIPStatus �����ķ���ֵ���� */
enum
{	
	IPS_GET_IP = '2',	/* ���IP */
	IPS_LINK_OK = '3',	/* �������� */
	IPS_LINK_LOST = '4',	/* ʧȥ���ӣ�ģ����ܿ��Ź���λ */
	
	IPS_BUSY = '8',		/* ģ���ڲ�æ�� zhg �Զ����״̬������������ȷӦ�� */
	IPS_TIMEOUT = '9'	/* ����Ӧ��ʱ�� zhg �Զ����״̬ */
};

/* ESP32_RxData �����ķ���ֵ���� */
enum
{	
	ESP_RX_NONE = 0,	/* û�ж����ֽ� */
	ESP_RX_BYTE,		/* ��ʾ���յ��ֽڣ����ݲ�ȫ��δ���롣 �����������жϳ�ʱ�� */
	ESP_RX_IPD,			/* +IPD���ݰ�, TCP��UDP���ݰ�  */
	ESP_RX_OTHER		/* �յ��س����н�����Ӧ���ַ���  */
};

/* ���ڷ��������� */
#define ACK_MAX_LEN		128
typedef struct
{
	uint8_t RxBuf[ACK_MAX_LEN];
	uint8_t Len1;
	uint8_t Len2;
	uint8_t Len3;
	
	char *pStr1;
	char *pStr2;
	char *pStr3;		

	int32_t LastTime;
	uint16_t Timeout;
	
	uint8_t RunFirst;
}ESP32_PT_T;

/* ���ⲿ���õĺ������� */
void bsp_InitESP32(void);
void ESP32_Reset(void);
uint8_t ESP32_PowerOn(void);
void ESP32_PowerOff(void);
void ESP32_EnterISP(void);
void ESP32_ExitISP(void);
void ESP32_SendAT(char *_Cmd);

uint8_t ESP32_WaitResponse(char *_pAckStr, uint16_t _usTimeOut);
void ESP32_QuitAP(void);
int16_t ESP32_ScanAP(WIFI_AP_T *_pList, uint16_t _MaxNum);
uint8_t ESP32_RxData(uint8_t *_buf, uint16_t *_len, uint16_t _buf_size, uint8_t *_link_id);

uint8_t ESP32_CreateTCPServer(uint16_t _TcpPort);
uint8_t ESP32_CreateUDPServer(uint8_t _id, uint16_t _LaocalPort);
uint8_t ESP32_SendTcpUdp(uint8_t _id, uint8_t *_databuf, uint16_t _len);
uint8_t ESP32_SendUdp(char *_RemoteIP, uint16_t _RemotePort, uint8_t *_databuf, uint16_t _len);
void ESP32_CloseTcpUdp(uint8_t _id);
uint8_t ESP32_GetLocalIP(char *_ip, char *_mac);
uint8_t ESP32_QueryIPStatus(void);

uint8_t ESP32_JoinAP(char *_ssid, char *_pwd, uint16_t _timeout);
uint8_t ESP32_SetWiFiMode(uint8_t _mode);
uint8_t ESP32_CIPMUX(uint8_t _mode);
uint8_t ESP32_LinkTCPServer(uint8_t _id, char *_server_ip, uint16_t _TcpPort);

uint8_t ESP32_GetChar(uint8_t *_data);
void ESP32_SendBuf(uint8_t *_cmd, uint16_t _len);

uint8_t ESP32_Set_AP_IP(char *_ip);
uint8_t ESP32_Set_AP_NamePass(char *_name, char * _pwd, uint8_t _ch, uint8_t _ecn);

uint8_t ESP32_ValidSSID(char *_ssid);
uint8_t ESP32_ValidPassword(char *_pass);

uint8_t ESP32_SetLocalIP(uint8_t *_ip, uint8_t *_netmask, uint8_t *_gateway);

uint8_t ESP32_GetMac(uint8_t *_mac);
uint8_t ESP32_SetMac(uint8_t *_mac);

uint16_t ESP32_ReadLine(char *_pBuf, uint16_t _usBufSize, uint16_t _usTimeOut);
uint16_t ESP32_ReadLineNoWait(char *_pBuf, uint16_t _usBufSize);
uint8_t WIFI_CheckAck(uint8_t *_str, int32_t _timeout);

void ESP32_Reset(void);
void ESP32_EnterAT(void);
void ESP32_EnterISP(void);
	
#define RX_MSG_MAX	32
extern uint8_t g_RxMsgBuf[RX_MSG_MAX]; 
extern uint8_t g_RxMsgLen;

extern ESP32_PT_T s_tAT;	/* ���ڷ�����ģʽִ��ATָ�� */

#endif

/***************************** ���������� www.armfly.com (END OF FILE) *********************************/
