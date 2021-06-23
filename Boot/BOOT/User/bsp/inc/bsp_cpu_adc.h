/*
*********************************************************************************************************
*
*	ģ������ : ʾ����ģ��ADC�ײ������
*	�ļ����� : bsp_cpu_adc.h
*	˵    �� : ͷ�ļ�
*
*	Copyright (C), 2015-2020, ���������� www.armfly.com
*
*********************************************************************************************************
*/

#ifndef __BSP_CPU_ADC_H
#define __BSP_CPU_ADC_H

#define FREQ_NUM 	17
extern const int32_t TabelFreq[FREQ_NUM];

#define BUFF_SIZE_NUM  10 			/* ������� */
extern int TabelBufSize[BUFF_SIZE_NUM];

/* ADC���ƽṹ */
typedef struct 
{
	uint16_t DC1;
	uint16_t DC2;
	uint16_t Gain1;
	uint16_t Gain2;
	int16_t Offset1;
	int16_t Offset2;

	uint16_t FreqID;
	uint16_t SampleSizeID;
	uint16_t TrigLevel;
	uint16_t TrigPos;
	uint16_t TrigMode;
	uint16_t TrigChan;
	uint16_t TrigEdge;
	uint16_t ChanEnable;
	uint16_t Run;
	
	uint16_t MeasuteMode;	/* 1��ʾ�������ģʽ */
	uint16_t CurrGain;	/* ��������(����) */
	
	uint32_t DmaPos;
	
	uint8_t TrigFlag;	/* ������־ */
	
	uint8_t DmaTransCplt;	/* DMA�������һ��, ����ʱ������DMA�ɼ���һ�ֲ���Ч */
}DSO_T;

/* ����ģʽ */
enum
{
	TRIG_MODE_AUTO = 0,		/* �Զ����� */
	TRIG_MODE_NORMAL = 1,	/* ��ͨ���� */	
	TRIG_MODE_SINGLE = 2,	/* ���δ��� */
};

/* ����ģʽ */
enum
{
	TRIG_EDGE_FALLING = 0,	/* �½��ش��� */	
	TRIG_EDGE_RISING = 1,	/* �����ش��� */	
};


/* ADCͨ�����塣 ���ڵ��ٶ�ͨ��ɨ��ģʽ */
	/*
		PF11/ADC1_INP2	---- CH1��ѹ
		PA6/ADC1_INP3	---- �߶˵���
	
		PC3_C/ADC3_INP1	---- CH2��ѹ
		PF5/ADC3_INP4	---- TVCC����������
		PF3/ADC3_INP5	---- NTC����������ֵ���
		PF8/ADC3_INP7	---- ���ص�ѹ
		PH2/ADC3_INP13	---- TVCC��ѹ���		
		PH3/ADC3_INP14	---- 12V�����ѹ���
		PH5/ADC3_INP16	---- USB�����ѹ���			
	*/
typedef enum
{
	AN_CH1 = 0,			/* CH1��ѹ */
	AN_CH2 = 1,			/* CH2��ѹ */
	AN_HIGH_SIDE_VOLT,	/* �߶˸��ص�ѹ */	
	AN_HIGH_SIDE_CURR,	/* �߶˸��ص��� */

	AN_TVCC_VOLT,	/* TVCC��ѹ���	*/
	AN_TVCC_CURR,	/* TVCC�������	*/
	AN_NTC_RES,		/* NTC����������ֵ��� */
	AN_12V_VOLT,	/* 12V�����ѹ��� */
	AN_USB_VOLT,	/* USB�����ѹ��� */
}ADC_CHAN_E;

/* Definition of ADCH1 conversions data table size */
#define ADC_BUFFER_SIZE   	((uint32_t)  32 * 1024)   /* Size of array aADCH1ConvertedData[], Aligned on cache line size */
#define DSO_PACKAGE_SIZE	1024

/* Variable containing ADC conversions data */
extern ALIGN_32BYTES (uint16_t   aADCH1ConvertedData[ADC_BUFFER_SIZE]);

extern ALIGN_32BYTES (uint16_t   aADCH2ConvertedData[ADC_BUFFER_SIZE]);

extern uint16_t g_Ch1WaveBuf[ADC_BUFFER_SIZE];
extern uint16_t g_Ch2WaveBuf[ADC_BUFFER_SIZE];
extern DSO_T g_tDSO;

void DSO_InitHard(void);
void DSO_ConfigCtrlGPIO(void);
void DSO_SetDC(uint8_t _ch, uint8_t _mode);
void DSO_SetGain(uint8_t _ch, uint8_t _gain);
void DSO_SetOffset(uint8_t _ch, int16_t _OffsetVolt);
void DSO_SetCurrGain(uint8_t _gain);
void DSO_SetTriger(void);

void DSO_StartADC(uint32_t _uiFreq);
void DSO_PauseADC(void);
void DSO_StopADC(void);

void DSO_SetSampRate(uint32_t _ulFreq);
void DSO_LockWave(void);

float bsp_GetAdcAvg(uint8_t _ch);
void bsp_AdcTask10ms(void);	


#endif
