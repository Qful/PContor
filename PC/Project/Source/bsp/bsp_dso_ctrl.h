/*
*********************************************************************************************************
*
*	ģ������ : DSOʾ��������ģ��
*	�ļ����� : bsp_dso_ctrl.c
*	��    �� : V1.0
*	˵    �� : ʾ��������
*
*	Copyright (C), 2019-2030, ���������� www.armfly.com
*
*********************************************************************************************************
*/

#ifndef __BSP_DSO_CTRL_H
#define __BSP_DSO_CTRL_H

typedef struct
{
	int VoltDiv;
	int WaveOffset;
	float VoltRMS;
	float VoltAvg;
	float Freq;
	float Duty;
	float VoltMin;
	float VoltMax;
}DSO_CH_T;

typedef struct
{
	DSO_CH_T CH1;
	DSO_CH_T CH2;
	int TimeDiv;
}DSO_T;

AnsiString TimeDivToStr(int div);
AnsiString VoltDivToStr(int _div);

extern DSO_T g_tDSO;

#define TIME_DIV_NUM	22           /* ʱ�䵵λ���� */
#define VOLT_DIV_NUM	12           /* ��ѹ��λ���� */
#define FREQ_NUM		17         	/* ����Ƶ�ʵ�λ */

#define BUFF_SIZE_NUM  10 			/* ������� */

#define DSO_PACKAGE_SIZE	1024		/* */


extern int TabelTimeDiv[TIME_DIV_NUM];
extern int TabelVoltDiv[VOLT_DIV_NUM];
extern int TabelBufSize[BUFF_SIZE_NUM];
extern int TabelFreq[FREQ_NUM];

#endif

/***************************** ���������� www.armfly.com (END OF FILE) *********************************/
