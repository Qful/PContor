/*
*********************************************************************************************************
*
*	ģ������ : ESP32 ����WIFIģ����������
*	�ļ����� : bsp_esp32_at.c
*	��    �� : V2.3
*	˵    �� : ��װ ESP32 ģ����ص�AT����
*
*	�޸ļ�¼ :
*		�汾��  ����        ����     ˵��
*		V1.0    2014-11-29  armfly  ��ʽ����
*		V1.1    2014-12-11  armfly  �޸� ESP32_WaitResponse() ����, ʵ�������ַ��жϡ�����TCP���ݷ��ͺ���.
*		V1.2    2014-12-22  armfly  ����GPIO2�� GPIO0 ���ŵ����á���Ӧ�°�Ӳ����
*		V1.3	2015-07-24  armfly	
*					(1) ���Ӻ��� uint8_t ESP32_CreateTCPServer(void);
*					(2) �޸�ESP32_JoinAP() ���ӷ���ֵ
*		V1.4	2015-12-22  armfly  �ش���£�֧�ֶ����ӣ��޸��˺ܶຯ��
*		V1.5	2015-12-27  armfly  ��������ǰ��������ջ�����������ָ�ʧ��
*		V2.0	2015-12-31  armfly  ��� ESP32_PT_JoinAP������������ģʽ��
*		V2.1	2016-01-01  armfly  ��� smart link�������ú�����
*		V2.2	2016-01-19  armfly 
*						- ��� ESP32_QueryIPStatus ������ֵ�� IPS_BUSY��Ӧ��ģ���ڲ�æ�����.
*						- ESP32_SendTcpUdp(), ��Ҫ���� busy s... �����
*						- ESP32_RxNew() ����Ϊ ESP32_RxData�����βΣ�֧�ֽ���TCP UDP���ݰ�����ָͨ�����
*		V3.0	2019-09-14  arfmly  ����esp8266���������޸ģ���Ϊesp32. �󲿷ֹ��ܲ��.
*
*	Copyright (C), 2015-2020, ���������� www.armfly.com
*
*********************************************************************************************************
*/

#include "bsp.h"

#include "param.h"
#include "modbus_slave.h"

/* ESP32 ģ�����ͼ
	ESP32ģ��   
		UTXD   ---  PH14/UART4_RX
		URXD   ---  PH13/UART4_TX
		GND    ---  GND
		
		CH_PD  ---  PF6/ESP32-S_RESET  ( 0��ʾ����  1��ʾ�����ϵ繤����
		GPIO0  ---  PG4/BOOT_OPTION ( 0�������ϵͳ������1��ʾ���������û�����ATָ�)


	ģ��ȱʡ������ 9600;  ֧�ֵķ�Χ��110~460800bps          ---- �����ӻὫģ�鲨�����л�Ϊ 115200
	�ڰ����ϵ��ʼ��boot rom��һ��log����Ҫ�� 74880 �Ĳ�������������ӡ�������Ǵ�ӡ����������.

	----------- PD = 1 ֮�� 74880bps ��ӡ�������� ------

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
	csum 0x0d

	----------- ֮���� 9600bps ��ӡ ---------------

	[Vendor:www.ai-thinker.com Version:0.9.2.4]

	ready


	ʹ�ô��ڳ����ն����ʱ����Ҫ���� �ն� - ���� - ģʽ ҳ�湴ѡ������ģʽ��.


	���޸Ĳ����ʡ�
	AT+CIOBAUD=?     ---- ��ѯ�������
	+CIOBAUD:(9600-921600)

	OK

	AT+CIOBAUD=115200
	BAUD->115200

	��ѡ�� WIFI Ӧ��ģʽ ��
	AT+CWMODE=1
		1   Station ģʽ
		2   AP ģʽ
		3   AP �� Station ģʽ

	���г���ǰ���� AP��
	AT+CWLAP=<ssid>,< mac >,<ch>
	AT+CWLAP

	��AT+CWJAP���� AP��
	AT+CWJAP=<ssid>,< pwd >
	
	
	"AT+CWJAP="Tenda_4FD138","123456887mdh"

*/

/*
	��ð汾��Ϣ
	AT+GMR
	AT version:0.21.0.0
	SDK version:0.9.5
	
	
	
	AT+CWMODE?     
	1�� stationģʽ
	2�� softAPģʽ
	3�� softAp + station
*/

/*
ESP32������BOOTģʽ����MTDO��GPIO15��,GPIO0,GPIO2����
 13     15       14
MTDO   GPIO0   GPIO2   Mode  Description
 L       L       H     UART Download code from UART 
 L       H       H     Flash Boot from SPI Flash 
 H       x       x     SDIO Boot from SD-card 
 
�ϵ�ʱ��ӡ����Ϣ��boot mode:(x,y)�� x��3λΪ {MTDO, GPIO0, GPIO2}.
*/

#define AT_CR		'\r'
#define AT_LF		'\n'

#define ALL_ESP_GPIO_CLK_ENABLE()	__HAL_RCC_GPIOF_CLK_ENABLE();__HAL_RCC_GPIOG_CLK_ENABLE()

/* Ӳ������������� -- �� 3.3V ��ʼ����  */
#define GPIO_CH_PD		GPIOF
#define PIN_CH_PD		GPIO_PIN_6
#define ESP_CH_PD_0()	GPIO_CH_PD->BSRRH = PIN_CH_PD
#define ESP_CH_PD_1()	GPIO_CH_PD->BSRRL = PIN_CH_PD


/* 1��ʾ����̼�����ģʽ 0��ʾ����ATָ��ģʽ */
#define GPIO_GPIO0		GPIOG
#define PIN_GPIO0		GPIO_PIN_4
#define ESP_GPIO0_0()	GPIO_GPIO0->BSRRH = PIN_GPIO0	/* �̼�����ģʽ */
#define ESP_GPIO0_1()	GPIO_GPIO0->BSRRL = PIN_GPIO0	/* ��������ģʽ */

uint8_t g_EspBuf[2048];	/* ���ڽ��� */


/* ����ģ���������͵���Ϣ */
uint8_t g_RxMsgBuf[RX_MSG_MAX]; 
uint8_t g_RxMsgLen;

static void ESP32_CH_PD_0(void)
{
	ESP_CH_PD_0();
}

static void ESP32_CH_PD_1(void)
{
	ESP_CH_PD_1();
}

/*
*********************************************************************************************************
*	�� �� ��: bsp_InitESP32
*	����˵��: ��������ģ����ص�GPIO,  �ú����� bsp_Init() ���á�
*	��    ��:  ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void bsp_InitESP32(void)
{
	GPIO_InitTypeDef gpio_init;

	/* ��1������GPIOʱ�� */
	ALL_ESP_GPIO_CLK_ENABLE();
	
	gpio_init.Mode = GPIO_MODE_OUTPUT_PP;	/* ���ÿ�©��� */
	gpio_init.Pull = GPIO_NOPULL;			/* ���������費ʹ�� */
	gpio_init.Speed = GPIO_SPEED_FREQ_HIGH;  /* GPIO�ٶȵȼ� */
	
	gpio_init.Pin = PIN_CH_PD;	
	HAL_GPIO_Init(GPIO_CH_PD, &gpio_init);	
	
	gpio_init.Pin = PIN_GPIO0;	
	HAL_GPIO_Init(GPIO_GPIO0, &gpio_init);	

	ESP_CH_PD_0();		/* ����ESPģ�鴦�ڵ���״̬ */

	ESP_GPIO0_1();		/* 1��ʾ�������� */
}

/*
*********************************************************************************************************
*	�� �� ��: WIFI_CheckAck
*	����˵��: ���Ӧ��
*	��    ��: _str : �ȴ����ַ���
*			  _timeout : �ȴ���ʱ��ms
*	�� �� ֵ: 0:����������  1:�ɹ���⵽�ַ���  2:��ʱ��
*********************************************************************************************************
*/
uint8_t WIFI_CheckAck(uint8_t *_str, int32_t _timeout)
{	
	uint8_t ch;
	static uint8_t s_cmp_len = 0;
	static int32_t s_time = 0;
	
	if (_str[0] == 0)
	{
		s_cmp_len = 0;
		s_time = bsp_GetRunTime();
		return 0;
	}
	
	if (ESP32_GetChar(&ch))
	{
		if (ch == _str[s_cmp_len])
		{				
			s_cmp_len++;						
			if (_str[s_cmp_len] == 0)
			{			
				return 1;	/* �յ���ȷӦ�� */
			}
		}
		else
		{
			s_cmp_len = 0;
		}
	}
	
	if (_timeout > 0)
	{
		if (bsp_CheckRunTime(s_time) > _timeout)
		{
			return 2;
		}
	}
	return 0;
}

/*
*********************************************************************************************************
*	�� �� ��: ESP32_SendBuf
*	����˵��: �������ݵ�WiFiģ��
*	��    ��: _cmd : ����
*			 _len : ���ݳ���
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void ESP32_SendBuf(uint8_t * _cmd, uint16_t _len)
{
	//#ifdef ESP32_TO_COM1_EN
	if (g_tVar.WiFiDebugEn == 1)	/* RS485��������ATָ��ʱ������WiFi Debug״̬������120���Զ��˳� */
	{
//		{
//			char buf[20];
//			
//			static int32_t s_time = 0;
//				
//			if (bsp_CheckRunTime(s_time) > 10)
//			{		
//				sprintf(buf, "\r\n(%d)=>", bsp_GetRunTime()); 
//				comSendBuf(COM_DEBUG, (uint8_t *)buf, strlen(buf));
//			}
//			s_time = bsp_GetRunTime();
//		}
//		comSendBuf(COM_DEBUG, _cmd, _len);		/* �����յ����ݴ�ӡ�����Դ���1 */
	}
	//#endif

	comSendBuf(COM_ESP32, _cmd, _len);	
}

/*
*********************************************************************************************************
*	�� �� ��: ESP32_GetChar
*	����˵��: �������ݵ�WiFiģ��
*	��    ��: _data : ������ݵı�����ַ
*	�� �� ֵ: 0 ��ʾ�����ݣ�1��ʾ�����ݡ� ���ݴ���� *_data
*********************************************************************************************************
*/
extern uint8_t link_id;
uint8_t ESP32_GetChar(uint8_t *_data)
{
	uint8_t re;

	re = comGetChar(COM_ESP32, _data);
	if (re != 0)
	{	
		if (g_tVar.WiFiDebugEn == 1)	/* RS485��������ATָ��ʱ������WiFi Debug״̬������120���Զ��˳� */
		{
//			{
//				static int32_t s_time = 0;
//				
//				if (bsp_CheckRunTime(s_time) > 20)
//				{
//					char buf[20];
//					
//					sprintf(buf, "\r\n(%d)<==", bsp_GetRunTime()); 
//					comSendBuf(COM_DEBUG, (uint8_t *)buf, strlen(buf));						
//				}
//				s_time = bsp_GetRunTime();
//				comSendChar(COM_DEBUG, *_data);		/* �����յ����ݴ�ӡ�����Դ���1 */
//			}			
		}

		/* 	
			#define RX_MSG_MAX	32;
			uint8_t g_RxMsgBuf[RX_MSG_MAX]; 
			uint8_t g_RxMsgLen;
		*/
		{
			uint8_t ucData;
			static uint8_t s_flag = 0;	/* IPD ����������Ϣ */
			static uint16_t s_data_len = 0;		/* UDP TCP���ݳ��� */
			char *p1;
			static int32_t s_last_rx_time = 0;
			static uint16_t s_ipd_pos = 0;
	
			/* +IPD,0,7:ledon 1 */
			ucData = *_data;
			
			/* ����ϴ��յ������ݾ�񳬹�100ms����������֡ͬ�� */
			if (bsp_CheckRunTime(s_last_rx_time) > 1000)
			{
				s_flag = 0;
				g_RxMsgLen = 0;
				s_ipd_pos = 0;
			}
			
			if (s_flag == 0)
			{
				if (ucData == 0x0D || ucData == 0xA)
				{
					if (s_ipd_pos >= 2)
					{
						g_RxMsgLen = s_ipd_pos;	/* ���յ������ݰ���Ӧ�� */
						
						/* ʶ�������Ϣ - �˴������������ϲ㴦�� */
						{
							/* TCP�������ر���TCP���� */
							if (g_RxMsgLen >= 8 && memcmp(g_RxMsgBuf, "4,CLOSED", 8) == 0)
							{			
								g_tVar.RemoteTCPServerOk = 0;
							}
							
							/* WIFI·�������� */
							if (g_RxMsgLen >= 15 && memcmp(g_RxMsgBuf, "WIFI DISCONNECT", 15) == 0)
							{
								g_tVar.HomeWiFiLinkOk = 0;
								g_tVar.RemoteTCPServerOk = 0;
							}
						}
						
						s_ipd_pos = 0;
					}
					else
					{
						g_RxMsgLen = 0;
						s_data_len = 0;		
						s_ipd_pos = 0;
					}
				}
				else
				{
					if (s_ipd_pos < RX_MSG_MAX)
					{
						g_RxMsgBuf[s_ipd_pos++] = ucData;		/* ������յ������� */
					}
					
					if (g_RxMsgBuf[0] == '+' && s_ipd_pos > 7 && ucData == ':')
					{
						p1 = (char *)&g_RxMsgBuf[5];
						link_id = str_to_int(p1);		/* ����������id */
						
						p1 = (char *)&g_RxMsgBuf[7];
						s_data_len = str_to_int(p1);	/* ���������ݰ����� */
						s_flag = 1;		/* �������ݰ�����״̬ */
						s_ipd_pos = 0;
					}
				}										
			}
			else	/* ���ǽ��� +IPD���ݰ��ķ�֮ */
			{
				if (s_ipd_pos < RX_BUF_SIZE)
				{
					g_EspBuf[s_ipd_pos++] = ucData;	/* ������յ���UDP,TCP������ */
					
					if (s_ipd_pos == s_data_len)
					{
						s_flag = 0;
						g_tModS.RxCount = s_data_len;		/* wifi_poll �ᴦ��modbus֡ */						
						g_tVar.WiFiRecivedIPD = 1;				/* �յ�UDP, TCP���ݰ� */
						
						s_ipd_pos = 0;
					}
				}	
				else
				{
					s_flag = 0;
				}
			}

			s_last_rx_time = bsp_GetRunTime();		
		}
		
		return 1;
	}
	return 0;
}

/*
*********************************************************************************************************
*	�� �� ��: ESP32_PowerOn
*	����˵��: ��ESP32ģ���ϵ�
*	��    ��: ��
*	�� �� ֵ: 0 ��ʾ�ϵ�ʧ�ܣ�1��ʾOK����⵽ģ��
*********************************************************************************************************
*/
uint8_t ESP32_PowerOn(void)
{
	/*
		2018-08-08 �ɹ�ESP-01ģ�飬�̼����¹�����ӡ��Ϣ���£�
	���ϵ����壬��������ϣ�266ms  ( 74880 bsp)
	������ready�źš�

	 ets Jan  8 2013,rst cause:1, boot mode:(3,6)

	load 0x40100000, len 1856, room 16 
	tail 0
	chksum 0x63
	load 0x3ffe8000, len 776, room 8 
	tail 0
	chksum 0x02
	load 0x3ffe8310, len 552, room 8 
	tail 0
	chksum 0x79
	csum 0x79

	2nd boot version : 1.5
	  SPI Speed      : 40MHz
	  SPI Mode       : DIO
	  SPI Flash Size & Map: 8Mbit(512KB+512KB)
	jump to run user1 @ 1000


	(219)<==rf cal sector: 249
	rf[112] : 00
	rf[113] : 00
	rf[114] : 01

	SDK ver: 1.5.4.1(39cb9a32) compiled @ Jul  1 2016 20:04:35
	phy ver: 972, pp ver: 10.1
	*/
	
	/*  �ɰ汾
		ESP-01 ģ���ϵ�ʱ������74880�����ʴ�ӡ������Ϣ:
	
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
		csum 0x0d	    <-----  ����ʶ�� csum �����Զ��л�������������
	*/

	/* 
		ESP-07 ģ���ϵ�ʱ������74880�����ʴ�ӡ������Ϣ:  (ʵ�� 310ms���յ�ready��
	
		 ets Jan  8 2013,rst cause:1, boot mode:(3,7)

		load 0x40100000, len 816, room 16 
		tail 0
		chksum 0x8d
		load 0x3ffe8000, len 788, room 8 
		tail 12
		chksum 0xcf
		ho 0 tail 12 room 4
		load 0x3ffe8314, len 288, room 12 
		tail 4
		chksum 0xcf
		csum 0xcf

		2nd boot version : 1.2
		  SPI Speed      : 40MHz
		  SPI Mode       : QIO
		  SPI Flash Size : 4Mbit
		jump to run user1
	*/	
	uint8_t re;	

	ESP32_CH_PD_0();

	bsp_DelayMS(20);


	ESP32_CH_PD_1();
	
#if 0
	bsp_SetUartBaud(74880);
	ESP32_WaitResponse("phy ver", 5000);
#endif	
	
	/* �ȴ�ģ������ϵ磬�ж��Ƿ���յ� ready */
	comSetBaud(COM_ESP32, 115200);
	re = ESP32_WaitResponse("ready", 300);
	if (re == 0)
	{
		return 0;
	}
	
	/* �رջ��Թ��ܣ��������͵��ַ���ģ�����践�� */	
	ESP32_SendAT("ATE0");		
	ESP32_WaitResponse("OK\r\n", 100);
	
	return 1;
}

/*
*********************************************************************************************************
*	�� �� ��: ESP32_PowerOff
*	����˵��: ����ESP32ģ��ػ�
*	��    ��: ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void ESP32_PowerOff(void)
{
	ESP32_CH_PD_0();
}

/*
*********************************************************************************************************
*	�� �� ��: ESP32_Reset
*	����˵��: ��λESP32ģ��
*	��    ��: ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void ESP32_Reset(void)
{
	ESP32_CH_PD_0();
	bsp_DelayMS(20);
	ESP32_CH_PD_1();

	bsp_DelayMS(10);
}

/*
*********************************************************************************************************
*	�� �� ��: ESP32_EnterISP
*	����˵��: ����ESP32ģ�����̼�����ģʽ
*	��    ��: ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void ESP32_EnterISP(void)
{
	ESP_CH_PD_0();
	ESP_GPIO0_0();  /* 0 ��ʾ����̼�����ģʽ */
	bsp_DelayMS(10);
	ESP_CH_PD_1();
}

/*
*********************************************************************************************************
*	�� �� ��: ESP32_EnterAT
*	����˵��: ����ESP32ģ�����ATģʽ
*	��    ��: ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void ESP32_EnterAT(void)
{
	ESP_CH_PD_0();
	ESP_GPIO0_1();  /* 1 ��ʾ�����û�����ATָ�ģʽ */
	bsp_DelayMS(10);
	ESP_CH_PD_1();
}

/*
*********************************************************************************************************
*	�� �� ��: ESP32_9600to115200
*	����˵��: 9600�������л���115200
*	��    ��: ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void ESP32_9600to115200(void)
{
	comSetBaud(COM_ESP32, 9600);
	ESP32_SendAT("AT+CIOBAUD=115200");	/* �� 9600bps ����ָ���л�Ϊ 115200 */
	ESP32_WaitResponse("OK\r\n", 2000);		/* ��� OK ��ģ�鰴 9600 Ӧ��� */
	comSetBaud(COM_ESP32, 115200);		/* �л�STM32�Ĳ�����Ϊ 115200 */

	/* �л�Ϊ Stationģʽ */
	bsp_DelayMS(100);
	ESP32_SendAT("AT+CWMODE=1");
	ESP32_WaitResponse("OK\r\n", 2000);
	bsp_DelayMS(1500);
	ESP32_SendAT("AT+RST");
}

/*
*********************************************************************************************************
*	�� �� ��: ESP32_WaitResponse
*	����˵��: �ȴ�ESP32����ָ����Ӧ���ַ���, ���԰��������ַ���ֻҪ������ȫ���ɷ��ء�
*	��    ��: _pAckStr : Ӧ����ַ����� ���Ȳ��ó���255. ֧�ּ�����ַ���\t���.
*			 _usTimeOut : ����ִ�г�ʱ��0��ʾһֱ�ȴ�. >����ʾ��ʱʱ�䣬��λ1ms
*	�� �� ֵ: 1 ��ʾ�ɹ�  0 ��ʾʧ��
*********************************************************************************************************
*/
uint8_t ESP32_WaitResponse(char *_pAckStr, uint16_t _usTimeOut)
{
	uint8_t ucData;
	uint16_t pos = 0;
	uint32_t len;
	uint8_t ret;
	int32_t time1;

	len = strlen(_pAckStr);
	if (len > 255)
	{
		return 0;
	}

	time1 = bsp_GetRunTime();
	while (1)
	{
		bsp_Idle();				/* CPU����ִ�еĲ����� �� bsp.c �� bsp.h �ļ� */

		if (_usTimeOut > 0)		/* _usTimeOut == 0 ��ʾ���޵ȴ� */
		{
			if (bsp_CheckRunTime(time1) >= _usTimeOut)
			{
				ret = 0;	/* ��ʱ */
				break;
			}
		}

		if (ESP32_GetChar(&ucData))
		{
			
			{
				if (ucData == _pAckStr[pos])
				{
					pos++;

					if (pos == len)
					{
						ret = 1;	/* �յ�ָ����Ӧ�����ݣ����سɹ� */
						break;
					}
				}
				else
				{
					pos = 0;
				}
			}
		}
	}
	return ret;
}

/*
*********************************************************************************************************
*	�� �� ��: ESP32_ReadLine
*	����˵��: ��ȡESP32���ص�һ��Ӧ���ַ���(0x0D 0x0A����)���ú��������ַ��䳬ʱ�жϽ����� ��������Ҫ����AT����ͺ�����
*	��    ��: _pBuf : ���ģ�鷵�ص������ַ���
*			  _usBufSize : ��������󳤶�
*			 _usTimeOut : ����ִ�г�ʱ��0��ʾһֱ�ȴ�. >0 ��ʾ��ʱʱ�䣬��λ1ms
*	�� �� ֵ: 0 ��ʾ���󣨳�ʱ��  > 0 ��ʾӦ������ݳ���
*********************************************************************************************************
*/
uint16_t ESP32_ReadLine(char *_pBuf, uint16_t _usBufSize, uint16_t _usTimeOut)
{
	uint8_t ucData;
	uint16_t pos = 0;
	uint8_t ret;
	int32_t time1;	
	uint16_t time_out;

	time1 = bsp_GetRunTime();
	time_out = _usTimeOut;	/* �״γ�ʱ */
	while (1)
	{
		bsp_Idle();				/* CPU����ִ�еĲ����� �� bsp.c �� bsp.h �ļ� */

		if (_usTimeOut > 0)
		{
			if (bsp_CheckRunTime(time1) >= time_out)
			{
				_pBuf[pos] = 0;	/* ��β��0�� ���ں���������ʶ���ַ������� */
				ret = pos;		/* �ɹ��� �������ݳ��� */
				break;
			}
		}

		if (ESP32_GetChar(&ucData))
		{
			time1 = bsp_GetRunTime();	/* �յ����ַ����ַ��䳬ʱ����Ϊ 0.5�� */
			time_out  = 500;
			
			if (pos < _usBufSize)
			{
				_pBuf[pos++] = ucData;		/* ������յ������� */
			}
			if (ucData == 0x0A)
			{
				_pBuf[pos] = 0;
				ret = pos;			/* �ɹ��� �������ݳ��� */
				break;
			}
		}
	}
	return ret;
}

/*
*********************************************************************************************************
*	�� �� ��: ESP32_ReadLine
*	����˵��: ��ȡESP32���ص�һ��Ӧ���ַ���(0x0D 0x0A����)���ú��������ַ��䳬ʱ�жϽ����� ��������Ҫ����AT����ͺ�����
*	��    ��: _pBuf : ���ģ�鷵�ص������ַ���
*			  _usBufSize : ��������󳤶�
*			 _usTimeOut : ����ִ�г�ʱ��0��ʾһֱ�ȴ�. >0 ��ʾ��ʱʱ�䣬��λ1ms
*	�� �� ֵ: 0��ʾδ������������  > 0 ��ʾӦ������ݳ���
*********************************************************************************************************
*/
uint16_t ESP32_ReadLineNoWait(char *_pBuf, uint16_t _usBufSize)
{
	static uint16_t s_pos = 0;
	uint8_t ucData;
	uint8_t ret = 0;
	
	if (ESP32_GetChar(&ucData))
	{
		if (s_pos < _usBufSize)
		{
			_pBuf[s_pos++] = ucData;		/* ������յ������� */
		}
		if (ucData == 0x0A)
		{
			_pBuf[s_pos] = 0;
			ret = s_pos;			/* �ɹ��� �������ݳ��� */
			s_pos = 0;
			
		}
	}
	return ret;
}

/*
*********************************************************************************************************
*	�� �� ��: ESP32_SendAT
*	����˵��: ��ģ�鷢��AT��� �������Զ���AT�ַ���������<CR>�ַ�
*	��    ��: _Str : AT�����ַ�����������ĩβ�Ļس�<CR>. ���ַ�0����
*	�� �� ֵ: ��T
*********************************************************************************************************
*/
void ESP32_SendAT(char *_Cmd)
{
	//comClearRxFifo(COM_ESP32);
	
	/* �ȴ��ϰ�������� */
	while(comTxEmpty(COM_ESP32)==0);
	
	ESP32_SendBuf((uint8_t *)_Cmd, strlen(_Cmd));
	ESP32_SendBuf("\r\n", 2);
}

/*
*********************************************************************************************************
*	�� �� ��: ESP32_SetWiFiMode
*	����˵��: ����WiFiģ�鹤��ģʽ
*	��    ��: _mode : 1 = Stationģʽ,  2 = APģʽ,  3 = AP��Stationģʽ
*	�� �� ֵ: 0 ��ʾʧ�ܡ� 1��ʾ�ɹ�
*********************************************************************************************************
*/
uint8_t ESP32_SetWiFiMode(uint8_t _mode)
{
	char cmd_buf[30];
	
	if (_mode == 0 || _mode > 3)
	{
		_mode = 3;
	}
	sprintf(cmd_buf, "AT+CWMODE_CUR=%d", _mode);
	ESP32_SendAT(cmd_buf);
	if (ESP32_WaitResponse("OK\r\n", 2000) == 0)
	{
		return 0;
	}
	
	return 1;
}

/*
*********************************************************************************************************
*	�� �� ��: ESP32_SetMac
*	����˵��: ����MAC��ַ
*	��    ��: _mac : 
*	�� �� ֵ: 0 ��ʾʧ�ܡ� 1��ʾ�ɹ�
*********************************************************************************************************
*/
uint8_t ESP32_GetMac(uint8_t *_mac)
{
	/*
		AT+CIPSTAMAC_CUR?
	
		+CIPSTAMAC_CUR:"18:fe:34:d1:b0:07"
		OK	
	*/	
	char buf[64];
	
	memset(_mac, 0, 6);
	ESP32_SendAT("AT+CIPSTAMAC_CUR?");
	while (1)
	{
		if (ESP32_ReadLine(buf, sizeof(buf), 200))
		{
			if (memcmp(buf, "+CIPSTAMAC_CUR:", 15) == 0)
			{
				/* ����mac��ַ */
				char *p = &buf[16];
				
				_mac[0] = TwoCharToInt(p); p += 3;
				_mac[1] = TwoCharToInt(p); p += 3;
				_mac[2] = TwoCharToInt(p); p += 3;
				_mac[3] = TwoCharToInt(p); p += 3;
				_mac[4] = TwoCharToInt(p); p += 3;
				_mac[5] = TwoCharToInt(p);
			}
			else if (memcmp(buf, "OK", 2) == 0)
			{
				return 1;
			}			
		}
		else	/* ����ʱ */
		{
			break;
		}
	}
	return 0;
}

/*
*********************************************************************************************************
*	�� �� ��: ESP32_SetMac
*	����˵��: ����MAC��ַ. ESP32 MAC   bit 0 �����Ǽ���
*	��    ��: _mac : 
*	�� �� ֵ: 0 ��ʾʧ�ܡ� 1��ʾ�ɹ�
*********************************************************************************************************
*/
uint8_t ESP32_SetMac(uint8_t *_mac)
{
	/*
		AT+CIPSTAMAC_CUR="18:fe:35:98:d3:7b"
	*/	
	char cmd_buf[64];
	
	sprintf(cmd_buf, "AT+CIPSTAMAC_CUR=\"%02x:%02x:%02x:%02x:%02x:%02x\"",
		_mac[0], _mac[1], _mac[2], _mac[3], _mac[4], _mac[5]);	
	ESP32_SendAT(cmd_buf);
	if (ESP32_WaitResponse("OK\r\n", 200) == 0)
	{
		return 0;
	}	
	return 1;
}

/*
*********************************************************************************************************
*	�� �� ��: ESP32_CIPMUX
*	����˵��: ����������ģʽ
*	��    ��: _mode : 0,��ʾ�رգ� 1��ʾ����
*	�� �� ֵ: 0 ��ʾʧ�ܡ� 1��ʾ�ɹ�
*********************************************************************************************************
*/
uint8_t ESP32_CIPMUX(uint8_t _mode)
{
	char cmd_buf[30];
	
	if (_mode > 0)
	{
		_mode = 1;
	}
	sprintf(cmd_buf, "AT+CIPMUX=%d", _mode);
	ESP32_SendAT(cmd_buf);
	if (ESP32_WaitResponse("OK\r\n", 200) == 0)
	{
		return 0;
	}
	
	return 1;
}

/*
*********************************************************************************************************
*	�� �� ��: ESP32_Set_AP_IP
*	����˵��: AT+CIPAP ���� AP �� IP��ַ
*	��    ��: _ip :AP��IP��ַ����׼�ַ���
*	�� �� ֵ: 0 ��ʾʧ�ܡ� 1��ʾ�ɹ�
*********************************************************************************************************
*/
uint8_t ESP32_Set_AP_IP(char *_ip)
{
	char cmd_buf[30];
	
	sprintf(cmd_buf, "AT+CIPAP=\"%s\"", _ip);
	ESP32_SendAT(cmd_buf);
	if (ESP32_WaitResponse("OK\r\n", 500) == 0)
	{
		return 0;
	}
	
	return 1;
}

/*
*********************************************************************************************************
*	�� �� ��: ESP32_Set_AP_NamePass
*	����˵��: ����SoftAP�����֣����ܷ�ʽ������.  ���ܷ�ʽΪ �� 
*	��    ��: _name :AP�����֣��ַ�������������� 64 �ֽ� ASCII
*			 _pwd : AP�����룬�ַ�������������� 64 �ֽ� ASCII
*			 _ch : ͨ����
*	�� �� ֵ: 0 ��ʾʧ�ܡ� 1��ʾ�ɹ�
*********************************************************************************************************
*/
uint8_t ESP32_Set_AP_NamePass(char *_name, char * _pwd, uint8_t _ch, uint8_t _ecn)
{
	char cmd_buf[40];

	/* AT+CWSAP="ESP32","1234567890",5,3 */	
	sprintf(cmd_buf, "AT+CWSAP_DEF=\"%s\",\"%s\",%d,%d", _name, _pwd, _ch, _ecn);
	ESP32_SendAT(cmd_buf);
	if (ESP32_WaitResponse("OK\r\n", 2000) == 0)
	{
		return 0;
	}
	
	return 1;
}

/*
*********************************************************************************************************
*	�� �� ��: ESP32_CreateTCPServer
*	����˵��: ����һ��TCP����ˡ� ���������ӵ�AP֮����С� ��Ҫ�����ö�����
*	��    �Σ�_TcpPort : TCP �˿ں�
*	�� �� ֵ: 0 ��ʾʧ�ܡ� 1��ʾ����TCP�ɹ�
*********************************************************************************************************
*/
uint8_t ESP32_CreateTCPServer(uint16_t _TcpPort)
{
	char cmd_buf[30];
	
	/* ����TCP server, �˿�Ϊ _TcpPort */
	sprintf(cmd_buf, "AT+CIPSERVER=1,%d", _TcpPort);
	ESP32_SendAT(cmd_buf);	
	if (ESP32_WaitResponse("OK\r\n", 2000) == 0)
	{
		return 0;
	}
	
	return 1;
}


/*
*********************************************************************************************************
*	�� �� ��: ESP32_CreateUDPServer
*	����˵��: ����һ��TUDP����ˡ� ���������ӵ�AP֮����С� ��Ҫ�����ö�����
*	��    ��:   _id : ����ID, 0-4
*				_LaocalPort : UDP �˿ں�
*	�� �� ֵ: 0 ��ʾʧ�ܡ� 1��ʾ����TCP�ɹ�
*********************************************************************************************************
*/
uint8_t ESP32_CreateUDPServer(uint8_t _id, uint16_t _LaocalPort)
{
	char cmd_buf[64];
	
	/* ������ UDP */
	//AT+CIPSTART=0,"UDP","255.255.255.255",8080,8080,0           <----- û����ͨ
	sprintf(cmd_buf, "AT+CIPSTART=%d,\"UDP\",\"255.255.255.255\",8080,%d,2", _id, _LaocalPort);
	
	ESP32_SendAT(cmd_buf);	
	if (ESP32_WaitResponse("OK\r\n", 3000) == 0)
	{
		return 0;
	}
	
	return 1;
}


/*
*********************************************************************************************************
*	�� �� ��: ESP32_LinkTCPServer
*	����˵��: ���ӵ�һ��TCP����ˡ�  ��(+CIPMUX=1) ������ģʽ��
*	��    ��: _id : ���ӵ�id��
*			  _server_ip : ������������IP��ַ
*			  _TcpPort : TCP �˿ں�
*	�� �� ֵ: 0 ��ʾʧ�ܡ� 1��ʾ����TCP�ɹ�
*********************************************************************************************************
*/
uint8_t ESP32_LinkTCPServer(uint8_t _id, char *_server_ip, uint16_t _TcpPort)
{
	char cmd_buf[64];
	
#if 0	/* ������ */
	//AT+CIPSTART="TCP","192.168.101.110",1000
	sprintf(cmd_buf, "AT+CIPSTART=\"TCP\",\"%s\",%d",_server_ip, _TcpPort);
#else	/* ������ */
	//AT+CIPSTART=0, "TCP","192.168.101.110",1000
	sprintf(cmd_buf, "AT+CIPSTART=%d,\"TCP\",\"%s\",%d", _id, _server_ip, _TcpPort);
#endif	
	ESP32_SendAT(cmd_buf);	
	if (ESP32_WaitResponse("OK\r\n", 3000) == 0)
	{
		return 0;
	}
	
	return 1;
}

/*
*********************************************************************************************************
*	�� �� ��: ESP32_SendUdp
*	����˵��: ����UDP���ݰ�. ��ռ�����Ӻ�
*	��    ��: _databuf ����
*			   _len ���ݳ���
*			 
*	�� �� ֵ: 0 ��ʾʧ�ܣ� 1��ʾOK
*********************************************************************************************************
*/
uint8_t ESP32_SendUdp(char *_RemoteIP, uint16_t _RemotePort, uint8_t *_databuf, uint16_t _len)
{
	char buf[48];

	if (_len > 2048)
	{
		_len = 2048;
	}
	/* AT+CIPSEND=0,"192.168.168.168",6200,200 */
	sprintf(buf, "AT+CIPSEND=0,%d,\"%s\",%d\r\n",_len, _RemoteIP, _RemotePort);
	ESP32_SendBuf((uint8_t *)buf, strlen(buf));

	/* ģ���ȷ���OK, Ȼ�󷵻� > ��ʾ�ȴ��������� */
	if (ESP32_WaitResponse(">", 50) == 0)
	{
		return 0;
	}
	
	ESP32_SendBuf(_databuf, _len);
	
	/* 2018-08-21 �ڵȴ�SEND OK��ʱ�䣨���20ms���ڣ��������ͻ����� +IPD ���ݰ�. */	
	ESP32_WaitResponse("SEND OK\r\n", 200); 
	
	return 1;
}

/*
*********************************************************************************************************
*	�� �� ��: ESP32_SendTcpUdp
*	����˵��: ����TCP��UDP���ݰ�
*	��    ��: _id : ������ʱ������ID ��0-4��
*			  _databuf ����
*			  _len ���ݳ���
*	�� �� ֵ: 0 ��ʾʧ�ܣ� 1��ʾOK
*********************************************************************************************************
*/
uint8_t ESP32_SendTcpUdp(uint8_t _id, uint8_t *_databuf, uint16_t _len)
{
	char buf[32];

	if (_len > 2048)
	{
		_len = 2048;
	}

	sprintf(buf, "AT+CIPSEND=%d,%d\r\n",_id, _len);
	ESP32_SendBuf((uint8_t *)buf, strlen(buf));

	/* ģ���ȷ���OK, Ȼ�󷵻� > ��ʾ�ȴ��������� */
	if (ESP32_WaitResponse(">", 200) == 0)		/* 2018-12-12, �����ӳ� 50 -> 200 */
	{
		return 0;
	}
	
	ESP32_SendBuf(_databuf, _len);
	
	/* 2018-08-21 �ڵȴ�SEND OK��ʱ�䣨���20ms���ڣ��������ͻ����� +IPD ���ݰ�. */
	
	return ESP32_WaitResponse("SEND OK\r\n", 1000); 	/* 2018-12-12�� �����ӳ٣�200 -> 1000 */
}

/*
*********************************************************************************************************
*	�� �� ��: ESP32_CloseTcpUdp
*	����˵��: �ر�TCP��UDP����. ���ڶ�·����
*	��    ��: _databuf ����
*			 _len ���ݳ���
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void ESP32_CloseTcpUdp(uint8_t _id)
{
	char buf[32];

	//ESP32_SendAT("ATE0");		/* �رջ��Թ��� */
	//ESP32_WaitResponse("SEND OK", 50);
	
	sprintf(buf, "AT+CIPCLOSE=%d", _id);
	ESP32_SendAT(buf);	
	ESP32_WaitResponse("OK", 1000);
	
	/* ZHG : ---- �˴���Ҫ����Ӧ�� */
}


/*
*********************************************************************************************************
*	�� �� ��: ESP32_QueryIPStatus
*	����˵��: ��ѯ��ǰ����״̬����0-4��. �������ƣ�δ��������ǰ���ӵ�id
*	��    ��: ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
uint8_t ESP32_QueryIPStatus(void)
{
	char buf[64];
	uint8_t i;
	uint8_t ret = IPS_TIMEOUT;

	
	ESP32_SendAT("AT+CIPSTATUS");
	
	/*��ģ�齫Ӧ��:
		
		AT+CIPSTATUS
		STATUS:4

		OK
	
	
	��һ���쳣�����ģ���ڲ�æ��
	�ֻ�Ƶ���ر�TCP���ӣ������������ݣ����ܵ���wifiģ����ַ�������æ�����
	
	busy s...
	*/
	
	for (i = 0; i < 8; i++)
	{
		ESP32_ReadLine(buf, sizeof(buf), 50);		/* 100ms��ʱ */
		if (memcmp(buf, "STATUS:", 7) == 0)
		{			
			ret = buf[7];
		}
		else if (memcmp(buf, "OK", 2) == 0)
		{
			break;
		}
		else if (memcmp(buf, "busy", 4) == 0)
		{
			ret = IPS_BUSY;
		}		
	}
	return ret;	
}

/*
*********************************************************************************************************
*	�� �� ��: ESP32_GetLocalIP
*	����˵��: ��ѯ����IP��ַ��MAC
*	��    ��: _ssid : AP�����ַ���
*			  _pwd : �����ַ���
*	�� �� ֵ: 1 ��ʾOK�� 0 ��ʾδ֪
*********************************************************************************************************
*/
uint8_t ESP32_GetLocalIP(char *_ip, char *_mac)
{
	char buf[64];
	uint8_t i, m;
	uint8_t ret = 0;
	uint8_t temp;
	
	ESP32_SendAT("AT+CIFSR");
	
	/*��ģ�齫Ӧ��:
		
	+CIFSR:STAIP,"192.168.1.18"
	+CIFSR:STAMAC,"18:fe:34:a6:44:75"
	
	OK	
	*/
	
	_ip[0] = 0;
	_mac[0] = 0;
	for (i = 0; i < 6; i++)
	{
		ESP32_ReadLine(buf, sizeof(buf), 500);
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
*	�� �� ��: ESP32_SetLocalIP
*	����˵��: ���ñ���IP��ַ��MAC
*	��    ��: _ip : IP��ַ 
*			  _gateway : ����
*			  _netmask : ��������
*	�� �� ֵ: 1 ��ʾOK�� 0 ��ʾδ֪
*********************************************************************************************************
*/
uint8_t ESP32_SetLocalIP(uint8_t *_ip, uint8_t *_netmask, uint8_t *_gateway)
{
	char buf[64];
	
	// AT+CIPSTA_DEF="192.168.6.100","192.168.6.1","255.255.255.0"
	
	sprintf(buf, "AT+CIPSTA_DEF=\"%d.%d.%d.%d\",\"%d.%d.%d.%d\",\"%d.%d.%d.%d\"",
		_ip[0], _ip[1], _ip[2], _ip[3],
		_gateway[0], _gateway[1], _gateway[2], _gateway[3],
		_netmask[0], _netmask[1], _netmask[2], _netmask[3]
	);
	ESP32_SendAT(buf);
	
	return ESP32_WaitResponse("OK", 200);	
}

/*
*********************************************************************************************************
*	�� �� ��: ESP32_JoinAP
*	����˵��: ����AP
*	��    ��: _ssid : AP�����ַ���
*			  _pwd : �����ַ���
*			  _timeout : ��ʱ��ms
*	�� �� ֵ: 
*				0 ��ʾ 0K, 
*				1 ���ӳ�ʱ
*				2 �������
*				3 �Ҳ���Ŀ��AP
*				4 ����ʧ��
*********************************************************************************************************
*/

uint8_t ESP32_JoinAP(char *_ssid, char *_pwd, uint16_t _timeout)
{

	/*  ����Ѿ��������ˣ���᷵��������Ϣ��
	
		(268)=>AT+CWJAP="MERCURY_603","123456887af"
		(269)=>

		(2488)<==WIFI CONNECTED

		(3208)<==WIFI GOT IP

		(3274)<==
		OK
	

		OK
		(320583)=>AT+CWJAP="Tenda_446248","123456887af"
		(320634)=>

		(320648)<==
		WIFI DISCONNECT

		(322820)<==WIFI CONNECTED
		WIFI GOT IP

		(323649)<==
		OK	
		
		----------- SSID �� ���벻�Ե���� ----------
		(158892)=>AT+CWJAP="Tenda_446248","123456887af"
		(158893)=>


		(173898)<==+CWJAP:3

		FAIL
	*/

	char buf[64];
	uint8_t err_code = 4;

	if (ESP32_ValidSSID(_ssid) == 0 || ESP32_ValidPassword(_pwd) == 0)
	{
		//printf("WiFi SSID����������쳣\r\n");
		return 0;
	}
	
	sprintf(buf, "AT+CWJAP_CUR=\"%s\",\"%s\"", _ssid, _pwd);
	ESP32_SendAT(buf);
	
	while (1)
	{
		if (ESP32_ReadLine(buf, 64, _timeout))
		{
			if (memcmp(buf, "AT+CWJAP_CUR", 12) == 0)	/* ATE1�����������1�ζ������� ����� */
			{
				;
			}
			else if (memcmp(buf, "WIFI CONNECTED", 14) == 0)
			{
				;
			}
			else if (memcmp(buf, "OK", 2) == 0)
			{
				return 0;	/* ����AP OK */
			}		
			else if (memcmp(buf, "+CWJAP:", 7) == 0)
			{
				err_code = buf[7] - '0';	/* ������룬ASCII */
			}
			else if (memcmp(buf, "FAIL", 4) == 0)
			{
				break;
			}			
		}
		else	/* ����ʱ */
		{
			err_code = 1;
			break;
		}
	}	
	return err_code;
}

/*
*********************************************************************************************************
*	�� �� ��: ESP32_QuitAP
*	����˵��: �˳���ǰ��AP����
*	��    ��: ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void ESP32_QuitAP(void)
{
	ESP32_SendAT("AT+ CWQAP");
}

/*
*********************************************************************************************************
*	�� �� ��: ESP32_ScanAP
*	����˵��: ɨ��AP����������_pList �ṹ������. �˺�����ռ���5��ʱ�䡣ֱ���յ�OK��ERROR��
*	��    ��: _pList : AP�б�����;
*			  _MaxNum : ���������AP��������Ҫ�Ƿ�ֹ�����������
*	�� �� ֵ: -1 ��ʾʧ��; 0 ��ʾ������0��; 1��ʾ1����
*********************************************************************************************************
*/
int16_t ESP32_ScanAP(WIFI_AP_T *_pList, uint16_t _MaxNum)
{
	uint16_t i;
	uint16_t count;
	char buf[128];
	WIFI_AP_T *p;
	char *p1, *p2;
	uint16_t timeout;

	buf[127] = 0;
	ESP32_SendAT("AT+CWLAP");

	p = (WIFI_AP_T *)_pList;
	count = 0;
	timeout = 8000;
	for (i = 0; i < _MaxNum; i++)
	{
		ESP32_ReadLine(buf, 128, timeout);
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
			/* �������ܷ�ʽ */
			p1 = strchr(p1, '(');	/* ������(*/
			p1++;
			p->ecn = str_to_int(p1);

			/* ����ssid */
			p1 = strchr(p1, '"');	/* ��������1���ֺ� */
			p1++;
			p2 = strchr(p1, '"');	/* ��������2���ֺ� */
			memcpy(p->ssid, p1, p2 - p1);
			p->ssid[p2 - p1] = 0;

			/* ���� rssi */
			p1 = strchr(p2, ',');	/* ����������*/
			p1++;
			p->rssi = str_to_int(p1);

			/* ����mac */
			p1 = strchr(p1, '"');	/* �������ֺ�*/
			p1++;
			p2 = strchr(p1, '"');	/* �������ֺ�*/
			memcpy(p->mac, p1, p2 - p1);
			p->mac[p2 - p1] = 0;

			/* ����ch */
			p1 = strchr(p2, ',');	/* ����������*/
			p1++;
			p->ch = str_to_int(p1);

			/* ��Ч��AP���� */
			count++;

			p++;
			
			timeout = 2000;
		}
	}

	return count;
}

/*
*********************************************************************************************************
*	�� �� ��: ESP32_RxData
*	����˵��: ��ȡwifiģ�����ݣ��������������ͣ�+IPD���ݰ� ��������ָ���Ӧ�����
*	��    ��: _pRxBuf : ���յ������ݴ���ڴ˻��������ѹ���0x0D,0x0A,  +IPD���ַ���
*			  _pDataLen : ���ݳ��ȴ��ڴ˻����� ��
*			 _link_id : ���ӵ�id�� 
*	�� �� ֵ:  ESP_RX_NONE  ��ʾ������  
*			  ESP_RX_BYTE   ��ʾ���յ��ֽڣ����ݲ�ȫ��δ���롣 �����������жϳ�ʱ��
*			  ESP_RX_IPD    +IPD���ݰ��� _pRxBuf �����Ч��TCP UDP�����壬 _link_id�������ID
*			  ESP_RX_OTHER  ��ʾ���յ������ַ��������� busy, connected, wdt reset)
*********************************************************************************************************
*/
uint8_t ESP32_RxData(uint8_t *_buf, uint16_t *_len, uint16_t _buf_size, uint8_t *_link_id)
{
	uint8_t ucData;
	uint8_t rx_flag = 0;
	static uint8_t s_flag = 0;
	static uint16_t s_data_len = 0;		/* UDP TCP���ݳ��� */
	char *p1;
	static int32_t s_last_rx_time = 0;
	
	/* +IPD,0,7:ledon 1 */

	while (ESP32_GetChar(&ucData))
	{
		rx_flag = 1;	/* ��ʾ�յ�1���ֽ� */
		
		/* ����ϴ��յ������ݾ�񳬹�100ms����������֡ͬ�� */
		if (bsp_CheckRunTime(s_last_rx_time) > 1000)
		{
			s_flag = 0;
			*_len = 0;
		}
		
		if (s_flag == 0)
		{
			if (*_len < _buf_size)
			{
				if (ucData == 0x0D || ucData == 0xA)
				{
					if (*_len >= 2)
					{
						s_data_len = 0;							
						return ESP_RX_OTHER;		/* ���յ������ݰ���Ӧ�� */
					}
					else
					{
						*_len = 0;
						s_data_len = 0;					
					}
				}
				else
				{
					_buf[(*_len)++] = ucData;		/* ������յ������� */

					
					if (_buf[0] == '+' && (*_len) > 7 && ucData == ':')
					{
						p1 = (char *)&_buf[5];
						*_link_id = str_to_int(p1);		/* ����������id */
						
						p1 = (char *)&_buf[7];
						s_data_len = str_to_int(p1);	/* ���������ݰ����� */
						s_flag = 1;		/* �������ݰ�����״̬ */
						*_len = 0;
					}
				}								
			}			
		}
		else	/* ���ǽ��� +IPD���ݰ��ķ�֮ */
		{
			if (*_len < _buf_size)
			{
				_buf[(*_len)++] = ucData;		/* ������յ������� */
				
				if (*_len == s_data_len)
				{
					s_flag = 0;
					//*_len = 0;
					
					return ESP_RX_IPD;  /* ��ʾ���յ�+IPD���ݰ� */
				}
			}	
			else
			{
				s_flag = 0;
			}
		}

		s_last_rx_time = bsp_GetRunTime();		
	}
	
	if (rx_flag == 1)
	{
		return ESP_RX_BYTE;	/* ��ʾ�յ�����1���ֽ� */
	}
	
	return ESP_RX_NONE;	/* ��ʾδ�յ��κ����� */
}

/*
*********************************************************************************************************
*	�� �� ��: ESP32_GetIPStatus
*	����˵��: ��ѯ����״̬
*	��    ��: _mac : 
*	�� �� ֵ: 0 ��ʾʧ�ܡ� 1��ʾ�ɹ�
*********************************************************************************************************
*/
uint8_t ESP32_GetIPStatus(uint8_t *_mac)
{
	/*
	��ȷ��Ӧ�����
	
	(4451)=>AT+CIPSTATUS
	(4451)=>
	STATUS:3		<- 
	+CIPSTATUS:0,"UDP","255.255.255.255",8080,6200,0
	+CIPSTATUS:4,"TCP","192.168.1.3",9800,37299,0

	OK
	
	----------------------
	
	STATUS: ����
		2: ESP32 station ������AP�����IP��ַ
		3: �ѽ���TCP��UDP����
		4��ESP32 �ѶϿ���������
		5��δ���ӵ�AP
	
	
	----------------------- ��������λ�� ---
	AT+CIPSTATUS
	STATUS:5

	OK
	*/	
	char buf[64];
	
	memset(_mac, 0, 6);
	ESP32_SendAT("AT+CIPSTAMAC_CUR?");
	while (1)
	{
		if (ESP32_ReadLine(buf, sizeof(buf), 200))
		{
			if (memcmp(buf, "+CIPSTAMAC_CUR:", 15) == 0)
			{
				/* ����mac��ַ */
				char *p = &buf[16];
				
				_mac[0] = TwoCharToInt(p); p += 3;
				_mac[1] = TwoCharToInt(p); p += 3;
				_mac[2] = TwoCharToInt(p); p += 3;
				_mac[3] = TwoCharToInt(p); p += 3;
				_mac[4] = TwoCharToInt(p); p += 3;
				_mac[5] = TwoCharToInt(p);
			}
			else if (memcmp(buf, "OK", 2) == 0)
			{
				return 1;
			}			
		}
		else	/* ����ʱ */
		{
			break;
		}
	}
	return 0;
}

/*
*********************************************************************************************************
*	�� �� ��: ESP32_ValidSSID
*	����˵��: �ж�ssid�ַ����Ƿ���ȷ
*	��    ��: _ssid : AP�����ַ���
*	�� �� ֵ: 1 ��ʾ 0K  0 ��ʾ��Ч
*********************************************************************************************************
*/
uint8_t ESP32_ValidSSID(char *_ssid)
{
	uint8_t i;
	
	for (i = 0; i < SSID_MAX_LEN + 2; i++)
	{
		if (_ssid[i] > 127)
		{
			return 0;
		}
		
		if (_ssid[i] == 0)
		{
			break;
		}
	}
	
	if (i <= SSID_MAX_LEN)
	{
		return 1;
	}
	return 0;
}

/*
*********************************************************************************************************
*	�� �� ��: ESP32_ValidSSID
*	����˵��: �ж�wifi�����ַ����Ƿ���ȷ
*	��    ��: _pass : AP�����ַ���
*	�� �� ֵ: 1 ��ʾ 0K  0 ��ʾ��Ч
*********************************************************************************************************
*/
uint8_t ESP32_ValidPassword(char *_pass)
{
	uint8_t i;
	
	for (i = 0; i < PASSWORD_MAX_LEN + 2; i++)
	{
		if (_pass[i] > 127)
		{
			return 0;
		}
		
		if (_pass[i] == 0)
		{
			break;
		}
	}
	
	if (i <= PASSWORD_MAX_LEN)
	{
		return 1;
	}
	return 0;
}

#if 0
/*
*********************************************************************************************************
*	����Ĵ�����Ƿ������������߳�Ӧ�á� 
***************************************************************************************
*/

/*
*********************************************************************************************************
*	�� �� ��: ESP32_PT_JoinAP
*	����˵��: ����AP�� �����̡߳�����ָ����������ء�
*	��    ��: _ssid : AP�����ַ���
*			  _pwd : �����ַ���
*	�� �� ֵ: 1 ��ʾ 0K  0 ��ʾʧ��
*********************************************************************************************************
*/
void ESP32_PT_JoinAP(char *_ssid, char *_pwd, uint16_t _timeout)
{
	char buf[64];

	if (ESP32_ValidSSID(_ssid) == 0 || ESP32_ValidPassword(_pwd) == 0)
	{
		printf("WiFi SSID����������쳣\r\n");
		return;
	}
	
	sprintf(buf, "AT+CWJAP=\"%s\",\"%s\"", _ssid, _pwd);
	ESP32_SendAT(buf);
	
	s_tAT.Len1 = 0;
	s_tAT.Len2 = 0;
	s_tAT.Timeout = _timeout;
	
	s_tAT.pStr1 = "OK\r\n";
	s_tAT.pStr2 = "FAIL\r\n";
	
	s_tAT.LastTime = bsp_GetRunTime();
}

/*
*********************************************************************************************************
*	�� �� ��: ESP32_PT_WaitJoinAP
*	����˵��: �ȴ�ģ��Ӧ��
*	��    ��: _StrOk : �յ����ַ�������ʾִ��OK
*	�� �� ֵ: PT_NULL ��ʾ��Ҫ�����ȴ�  1 ��ʾ 0K  2 ��ʾʧ��  3 ��ʾ��ʱ
*********************************************************************************************************
*/
uint8_t ESP32_PT_WaitResonse(void)
{
	{	
		uint8_t ucData;
		uint8_t ch;
		
		if (ESP32_GetChar(&ucData))
		{							
			/* �Ƚ�ִ�гɹ���Ӧ���ַ��� */
			ch = s_tAT.pStr1[s_tAT.Len1];
			if (ucData == ch)
			{
				s_tAT.Len1++;
				
				if (s_tAT.Len1 == strlen(s_tAT.pStr1))
				{
					return PT_OK;
				}
			}
			else
			{
				s_tAT.Len1 = 0;
			}

			/* �Ƚ�ִ��ʧ�ܵ�Ӧ���ַ��� */
			ch = s_tAT.pStr2[s_tAT.Len2];
			if (ucData == ch)
			{
				s_tAT.Len2++;
				
				if (s_tAT.Len2 == strlen(s_tAT.pStr2))
				{
					return PT_ERR;
				}
			}
			else
			{
				s_tAT.Len2 = 0;
			}		
		}

		if (s_tAT.Timeout > 0)
		{
			if (bsp_CheckRunTime(s_tAT.LastTime) >= s_tAT.Timeout)
			{
				return PT_TIMEOUT;	/* ���ʱ */
			}
		}
	}
	return PT_NULL;
}

/*
*********************************************************************************************************
*	�� �� ��: ESP32_PT_SmartStrat
*	����˵��: ֪ͨģ�������������״̬
*	��    ��: mode : �������ӵķ�ʽ��
*				0: ʹ�� ���ſ� AI-LINK����
*				1: ʹ�� ESP-TOUCH����
*				2: ʹ�� AIR-KISS
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void ESP32_PT_SmartStrat(uint8_t _mode)
{
	ESP32_SendAT("AT+CWSMARTSTART=0");

	s_tAT.Len1 = 0;
	s_tAT.Len2 = 0;
	s_tAT.Len3 = 0;	
	
	s_tAT.pStr1 = "\r\nSSID:";
	s_tAT.pStr2 = "\r\nPASSWORD:";
	s_tAT.pStr3 = "OK\r\n";	
	
	s_tAT.RunFirst = 1;
}

/*
*********************************************************************************************************
*	�� �� ��: ESP32_PT_SmartWait
*	����˵��: ��ȡ�ֻ�������ssid �����롣  ESP32_PT_SmartStratִ��һ�κ�����ִ��ESP32_PT_SmartWait������
*	��    ��: _ssid : ����ssdi�Ļ�����
*			 _password : ��������Ļ������� Ӧ�ó�������Ҫ����  PASSWORD_MAX_LEN + 1�Ļ������������
*	�� �� ֵ: 0 ��ʾ��Ч�� 1��ʾ�ɹ����á�
*********************************************************************************************************
*/
uint8_t ESP32_PT_SmartWait(char *_ssid,  char *_password)
{
	/* ָ��ִ�й��� : 

	AT+CWSMARTSTART=0

	OK
	
	-------���ֻ�����������Ϣ�����ǣ�ģ��᷵�� --------
	SMART SUCCESS
	SSID:Tenda_4FD138
	PASSWORD:123456887mdh

	OK
			
	*/	
	uint8_t ucData;
	uint8_t ch;
	static uint8_t s_ssid_len;
	static uint8_t s_password_len;
	static uint8_t s_RxSSID;
	static uint8_t s_RxPASSWORD;
	
	if (s_tAT.RunFirst == 1)
	{
		s_tAT.RunFirst = 0;
		
		s_ssid_len = 0;
		s_password_len = 0;
		
		s_RxSSID = 0;
		s_RxPASSWORD = 0;		
	}
	
	if (ESP32_GetChar(&ucData))
	{				
		/* ���ձ��� SSID */
		if (s_RxSSID == 1)
		{
			if (s_ssid_len < SSID_MAX_LEN)
			{
				_ssid[s_ssid_len++] = ucData;
			}
			
			if (ucData == 0x0D || ucData == 0x0A)
			{
				_ssid[s_ssid_len - 1] = 0;	/* �ַ���ĩβ��0 */
				s_RxSSID = 2;		/* ��ʾ�ɹ����յ�SSID */
			}
		}

		/* ���ձ������� */
		if (s_RxPASSWORD == 1)
		{
			if (s_password_len < PASSWORD_MAX_LEN)
			{			
				_password[s_password_len++] = ucData;
			}
			
			if (ucData == 0x0D || ucData == 0x0A)
			{
				_password[s_password_len - 1] = 0;	/* �ַ���ĩβ��0 */
				s_RxPASSWORD = 2;		/* ��ʾ�ɹ����յ�����*/
			}
		}		
		
		/* �Ƚϵ�1���ַ��� SSID: */
		ch = s_tAT.pStr1[s_tAT.Len1];
		if (ucData == ch)
		{
			s_tAT.Len1++;
			
			if (s_tAT.Len1 == strlen(s_tAT.pStr1))
			{
				s_RxSSID = 1;	/* �Ժ��������SSID, ֱ�� 0x0D 0x0A����  */
			}
		}
		else
		{
			s_tAT.Len1 = 0;
		}

		/* �Ƚϵ�2���ַ���  PASSWORD: */
		ch = s_tAT.pStr2[s_tAT.Len2];
		if (ucData == ch)
		{
			s_tAT.Len2++;
			
			if (s_tAT.Len2 == strlen(s_tAT.pStr2))
			{
				s_RxPASSWORD = 1;	/* �Ժ������������, ֱ�� 0x0D 0x0A����  */
			}
		}
		else
		{
			s_tAT.Len2 = 0;
		}
		
		/* �Ƚϵ�3���ַ��� OK */
		ch = s_tAT.pStr3[s_tAT.Len3];
		if (ucData == ch)
		{
			s_tAT.Len3++;
			
			if (s_tAT.Len3 == strlen(s_tAT.pStr3))
			{
				
				if (s_RxSSID == 2 && s_RxPASSWORD == 2)
				{				
					return PT_OK;
				}
				
				/* ���Ե�һ��OK, ����ATָ��� OK */
				s_tAT.Len3 = 0;
			}
		}
		else
		{
			s_tAT.Len3 = 0;
		}			
	}
	return PT_NULL;
}
#endif

/***************************** ���������� www.armfly.com (END OF FILE) *********************************/
