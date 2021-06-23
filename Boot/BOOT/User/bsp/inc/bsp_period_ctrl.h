/*
*********************************************************************************************************
*
*	ģ������ : �����Կ���ĳ��IO. ������˸ָʾ��
*	�ļ����� : bsp_period_ctrl.h
*	��    �� : V1.0
*	˵    �� : ͷ�ļ�
*
*	Copyright (C), 2015-2020, ���������� www.armfly.com
*
*********************************************************************************************************
*/

#ifndef __BSP_PERIOD_CTRL_H
#define __BSP_PERIOD_CTRL_H

typedef struct
{
	/* ������һ������ָ�� */
	void (*OnFunc)(void);	/* �ص��������������LED�ĺ��� */
	void (*OffFunc)(void);	/* �ص�����������Ϩ��LED�ĺ��� */
		
	uint8_t ucEnalbe;
	uint8_t ucState;
	uint16_t usOnTime;
	uint16_t usOffTime;
	uint16_t usCycle;
	uint16_t usCount;
	uint16_t usCycleCount;
}PERIOD_CTRL_T;

/* ���ⲿ���õĺ������� */
void PERIOD_InitVar(void);
void PERIOD_Scan(void);
void PERIOD_Start(PERIOD_CTRL_T *_ptPer, uint16_t _usOnTime, uint16_t _usOffTime, uint16_t _usCycle);
void PERIOD_Stop(PERIOD_CTRL_T *_ptPer);

extern PERIOD_CTRL_T g_tWiFiLed;	/* WiFi ָʾ�� */
extern PERIOD_CTRL_T g_tRunLed;		/* ���� ָʾ�� */

#endif

/***************************** ���������� www.armfly.com (END OF FILE) *********************************/
