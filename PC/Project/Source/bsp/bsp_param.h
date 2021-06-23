
/*
*********************************************************************************************************
*
*	ģ������ : �����ṹ
*	�ļ����� : bsp_param.h
*	��    �� : V1.0
*	˵    �� : ͷ�ļ�
*
*	Copyright (C), 2015-2016, ���������� www.armfly.com
*
*********************************************************************************************************
*/
#ifndef __BSP_PARAM_H
#define __BSP_PARAM_H

//#define APPLICATION_ADDRESS		0x08100000
#define APPLICATION_ADDRESS		0x08020000

/* IO����ѡ�� */
typedef enum
{
	ES_GPIO_IN  = 0,		/* GPIO ���룬 FMC���� */
	ES_GPIO_OUT = 1,		/* GPIO ����� FMC���� */
	ES_FMC_OUT  = 2,		/* GPIO ���룬 FMC��� */

	ES_FMC_NOE	= 3,		/* D8ר�ã�FMC_D8��FMC_NOE���� */
	ES_FMC_NWE	= 4,		/* D9ר�ã�FMC_D9��FMC_NWE���� */

	ES_GPIO_UART  = 5,		/* ��ΪGPIO���ڹ��� */
	ES_GPIO_CAN = 6,		/* CAN���� */
}EIO_SELECT_E;

typedef struct
{
	uint8_t CpuID[12];	/* CPU ID */
	uint16_t HardVer;
	uint16_t SoftVer;
	uint8_t Mac[6];
	uint8_t LocalIP[4];
	uint8_t NetMask[4];
	uint8_t GateWay[4];
	uint16_t LocalPort;
	uint8_t SysIP[4];
	uint16_t SysPort;
}PARAM_T;

/* ȫ�ֱ��� */
typedef struct
{
	uint16_t X15_00;
	uint16_t X31_16;
	uint16_t Y15_00;
	uint16_t Y31_16;

	float CH1Volt;
	float CH2Volt;
	float HighSideVolt;
	float HighSideCurr;
	float TVCCVolt;		/* TVCCʵ���ѹ */
	float TVCCCurr;		/* TVCCʵ���ѹ */
	float NTCRes;		/* NTC���� */
	float NTCTemp;		/* NTC�¶� */
	float USBPowerVolt;
	float ExtPowerVolt;

	float ADC_CH1Volt;
	float ADC_CH2Volt;
	float ADC_HighSideVolt;
	float ADC_HighSideCurr;
	float ADC_TVCCVolt;		/* TVCCʵ���ѹ */
	float ADC_TVCCCurr;		/* TVCCʵ���ѹ */
	float ADC_NTCRes;		/* NTC���� */
	//float NTCTemp;			/* NTC�¶� */
	float ADC_USBPowerVolt;
	float ADC_ExtPowerVolt;
}AI_T;

/* У׼�����ṹ��������ADC, DAC */
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

	AN_CALIB_ADC_T NtcRes;		/* NTC���½ӿڲο�������ֵ */

	AN_CALIB_T TVCCSet;		/* TVCC������õ�ѹ */

	AN_CALIB_DAC_T Dac10V;		/* DAC��ѹͨ��У׼����������10V */
	AN_CALIB_DAC_T Dac20mA;		/* DAC����ͨ��У׼������20mA */

	AN_CALIB_T USBVolt;			/* USB�����ѹ */
	AN_CALIB_T ExtPowerVlot;	/* �ⲿ�����ѹ */
}CALIB_T;

/* ģ����У׼���� */
typedef struct
{
	int ReadOk;
	/* IP���� */
	int CommInterface;	/* ͨ�Žӿڣ�0-��̫����1-WiFi��2-USB */
	AnsiString IPAddr;			/* Ŀ��IP��ַ */
	int UDPPort;		/* UDP�˿ں� */
	int SerialPortNo;		/* ���ں� */
}INI_PARAM_T;

extern CALIB_T g_tCalib;
extern AI_T g_tAI;
extern INI_PARAM_T g_tIniParam;

#endif

/***************************** ���������� www.armfly.com (END OF FILE) *********************************/

