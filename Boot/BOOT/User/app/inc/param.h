/*
*********************************************************************************************************
*
*	ģ������ : Ӧ�ó������ģ��
*	�ļ����� : param.h
*	��    �� : V1.0
*	˵    �� : ͷ�ļ�
*
*	Copyright (C), 2012-2013, ���������� www.armfly.com
*
*********************************************************************************************************
*/

#ifndef __PARAM_H
#define __PARAM_H

#define PARAM_ADDR				0			/* ������������ַ */
#define PARAM_SIZE				256			/* ���ռ䣬���ڱ����� */

#define PARAM_CALIB_ADDR		1024		/* ������������ַ */
#define PARAM_CALIB_SIZE		512			/* ���ռ䣬���ڱ����� */

#define PARAM_VER				0x00000100				/* ���������汾 100 */

#define CALIB_VER				0x00000200				/* У׼�����汾 200 */

/* ��δ���� ���򻺴� */
#define APP_BUF_ADDR			0x08000000 + 1 * 1024 * 1024

/* ������״̬�ֶ���, MS = Main Status */
enum
{
	MS_MAIN_MENU = 0,	/* ������ */
	
	MS_LOGIC,			/* �߼�������״̬ */
	
	MS_DAP,				/* DAP */
	MS_PROG,			/* �ѻ������� */
	
//	MS_HARDWARE_INFO,	/* Ӳ����Ϣ */
	
	MS_SYSTEM_SET,		/* ϵͳ���� */
	
	MS_RESERVED,			/* �����Ĺ���״̬ */
	
	MS_DAC_TEST,
	MS_GPIO_TEST,
	MS_ESP32_TEST,
	MS_HARD_INFO,
	
	MS_DSO,				/* ʾ�������� */
	MS_VOLT_METER,
	MS_CURRENT_METER,
	MS_NTC_METER,
};

/* У׼�����ṹ������У׼�� ͨ��У׼������ADC */
typedef struct
{
	float x1;
	float y1;
	float x2;
	float y2;
}
AN_CALIB_T;

/* У׼�����ṹ�����У׼�����DAC��·�����Բ��õ����� */
typedef struct
{
	int16_t x1;
	int16_t y1;
	int16_t x2;
	int16_t y2;
	int16_t x3;
	int16_t y3;
	int16_t x4;
	int16_t y4;	
}
AN_CALIB_DAC_T;

/* У׼�����ṹ�����У׼��������������Բ��õ����� */
typedef struct
{
	float x1;
	float y1;
	float x2;
	float y2;
	float x3;
	float y3;
	float x4;
	float y4;	
}
AN_CALIB_ADC_T;

/* ȫ�ֲ��� */
typedef struct
{
	uint32_t UpgradeFlag;		/*�������,0x55AAA55A��ʾ��Ҫ����APP��0xFFFF��ʾ�������*/
	uint32_t ParamVer;			/* �������汾���ƣ������ڳ�������ʱ�������Ƿ�Բ��������������� */
	
	uint8_t DispDir;			/* ��ʾ���� */
	
	uint8_t Addr485;
	
	uint8_t LocalIPAddr[4];		/* ����IP��ַ */
	uint8_t NetMask[4];			/* �������� */
	uint8_t Gateway[4];			/* ���� */
	uint16_t LocalTcpPort;		/* ����TCP�˿� */	
	
	uint8_t RemoteIPAddr[4];	/* Զ��(ǰ�ã�IP��ַ */
	uint16_t RemoteTcpPort;		/* Զ�ˣ�ǰ�ã�TCP�˿� */

	uint8_t WorkMode;	/* 0��ʾ��Ϊ�ӻ��� 1��ʾ������ѯǰ�õ�Ԫ������ֵ���� */

	uint8_t AP_SSID[32+1];		/* AP���� */
	uint8_t AP_PASS[16+1];		/* AP���� */
	uint8_t WiFiIPAddr[4];		/* IP��ַ  192.168.1.50 */
	uint8_t WiFiNetMask[4];		/* �������� 255.255.255.0 */	
	uint8_t WiFiGateway[4];		/* ���� 192.168.1.1 */	
	uint8_t ServerIP[32+1];		/* TCP������IP��ַ��������ASCII */
	uint16_t ServerTCPPort;		/* TCP������ TCP�����˿� */
	uint8_t DHCPEn;				/* DHCPʹ��  */
	uint8_t  ReportOnEvent;		/* �¼��ϱ����� 0��ֹ 1ʹ�� */
	uint16_t ReportPeriod;		/* ��ʱ�ϱ�����: ��, 0��ʾ�ر� */
	uint16_t ReprotID;			/* �ϱ�֡�еĴ�վ��ַ�� ��06H������ */
	uint16_t ReportOnTime;		/* ��ʱ�ϱ�֡���ƣ�0��ֹ��1��ʽ1 */
	
	uint8_t WiFiMac[6];			/* WiFi MAC */
	uint8_t APSelfEn;			/* ��������AP */
	uint16_t LocalTCPPort;		/* ����TCP�����˿� 6200 */
	uint16_t LocalUDPPort;		/* ����UDP�����˿� 7200 */
	uint8_t ModbusTCPEn;		/* 0��ʾMODBUS RTU, 1��ʾMODBUS TCP */
	uint8_t LoginPackage;		/* 0��ʾ��ֹע�������¼�� 1��ʾ��¼����ע��� */
	uint16_t LoginLen;			/* ע������ȣ��ֽ� */
	
	uint32_t TestWord;		/*�����Ե�Ԫ�����ڼ��eepromg���� */
	uint8_t NtcType;		/* NTC������������ 0 = 10K_B3950��1 = 100K_B3950 */
}
PARAM_T;

/* ģ����У׼���� */
typedef struct
{
	uint32_t CalibVer;		/* У׼�����汾. �������� */
	AN_CALIB_T CH1[8];		/* CH1 ʾ����ͨ��ADCУ׼����, ��Ӧ8��Ӳ������ */
	AN_CALIB_T CH2[8];		/* CH1 ʾ����ͨ��ADCУ׼����, ��Ӧ8��Ӳ������ */
	
	AN_CALIB_T LoadVolt;		/* ���ص�ѹУ׼���� */
	AN_CALIB_ADC_T LoadCurr[2];		/* ���ص���У׼������2������  */
	
	AN_CALIB_T TVCCVolt;	/* TVCC���ӵ�ѹ */
	AN_CALIB_ADC_T TVCCCurr;	/* TVCC���� */	
	
	AN_CALIB_T NtcRes;		/* NTC���½ӿڲο�������ֵ */
	
	AN_CALIB_T TVCCSet;		/* TVCC������õ�ѹ */
	
	AN_CALIB_DAC_T Dac10V;	/* DAC��ѹͨ��У׼����������10V */
	AN_CALIB_DAC_T Dac20mA;	/* DAC����ͨ��У׼������20mA */

	AN_CALIB_T USBVolt;			/* USB�����ѹ */
	AN_CALIB_T ExtPowerVolt;	/* �ⲿ�����ѹ */
}CALIB_T;

/* ȫ�ֱ��� */
typedef struct
{
	/* MCU ID */
	uint32_t CPU_Sn[3];
	
	uint8_t WiFiDebugEn;
	uint8_t RemoteTCPServerOk;
	uint8_t HomeWiFiLinkOk;
	
	uint8_t WiFiRecivedIPD;		
	
	uint8_t MACaddr[6];			/* ��̫��MAC��ַ */
	
	
	uint8_t InputState[32];
	uint8_t RelayState[32];
	
	float CH1Volt;
	float CH2Volt;
	float HighSideVolt;
	float HighSideCurr;
	float USBPowerVolt;
	float ExtPowerVolt;
	float TVCCVolt;		/* TVCCʵ���ѹ */
	float TVCCCurr;		/* TVCCʵ���ѹ */
	float NTCRes;		/* NTC���� */
	float NTCTemp;		/* NTC�¶� */

	float ADC_CH1Volt;
	float ADC_CH2Volt;
	float ADC_HighSideVolt;
	float ADC_HighSideCurr;
	float ADC_USBPowerVolt;
	float ADC_ExtPowerVolt;
	float ADC_TVCCVolt;		/* TVCCʵ���ѹ */
	float ADC_TVCCCurr;		/* TVCCʵ���ѹ */
	float ADC_NTCRes;		/* NTC���� */	
	
	uint16_t OutTVCCDac;	/* ���TVCC dacֵ */
	uint16_t OutTVCCmV;		/* ���TVCC mVֵ */
	
	uint16_t OutVoltDAC;	/* �����ѹ dacֵ */
	int16_t OutVoltmV;		/* �����ѹ mVֵ �з����� ֧�ָ���ѹ*/

	uint16_t OutCurrDAC;	/* ������� dacֵ */
	uint16_t OutCurruA;		/* ������� uAֵ */
	
	uint8_t LuaRunOnce;
	
	uint8_t CalibEnable;
	
	uint8_t GpioMode[16];	/* ����GPIOģʽ���������룬��������⹦�� */
	
}VAR_T;

extern PARAM_T g_tParam;
extern CALIB_T g_tCalib;
extern VAR_T g_tVar;

void LoadParam(void);
void SaveParam(void);
void InitCalibParam(void);
void WriteParamUint16(uint16_t _addr, uint16_t _value);
void SaveCalibParam(void);

#endif
