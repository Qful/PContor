/*
*********************************************************************************************************
*
*	ģ������ : ѭ��ִ��ģ��
*	�ļ����� : bsp_period_ctrl.c
*	��    �� : V1.0
*	˵    �� : ������ִ��ĳ������. ������˸ָʾ�ƣ� ����������, ���Ʒ���ռ�ձ�
*
*	�޸ļ�¼ :
*		�汾��  ����        ����     ˵��
*		V1.0    2015-12-27 armfly  ��ʽ����
*
*	Copyright (C), 2014-2015, ���������� www.armfly.com
*
*********************************************************************************************************
*/

#include "bsp.h"

//PERIOD_CTRL_T g_tWiFiLed;	/* WiFi ָʾ�� */
PERIOD_CTRL_T g_tRunLed;		/* ���� ָʾ�� */
static void PERIOD_ScanDev(PERIOD_CTRL_T *_ptPer);

void RunLedOn(void) {bsp_LedOn(1);}
void RunLedOff(void) {bsp_LedOff(1);}

/*
*********************************************************************************************************
*	�� �� ��: PERIOD_Start
*	����˵��: 
*	��    �Σ���
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void PERIOD_InitVar(void)
{
	g_tRunLed.ucEnalbe = 0;
	g_tRunLed.OnFunc = RunLedOn;
	g_tRunLed.OffFunc = RunLedOff;
		
//	g_tWiFiLed.ucEnalbe = 0;
//	g_tWiFiLed.OnFunc = bsp_WiFiLedOn;
//	g_tWiFiLed.OffFunc = bsp_WiFiLedOff;	
}

/*
*********************************************************************************************************
*	�� �� ��: PERIOD_Scan
*	����˵��: ����10ms�ж�ִ��
*	��    ��: ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void PERIOD_Scan(void)
{
//	PERIOD_ScanDev(&g_tWiFiLed);
	PERIOD_ScanDev(&g_tRunLed);
}

/*
*********************************************************************************************************
*	�� �� ��: PERIOD_Start
*	����˵��: ������������
*	��    ��: _usOnTime : ����ʱ�䣬��λ1ms; 0 ��ʾ������
*			  _usOffTime : ֹͣʱ�䣬��λ1ms; 0 ��ʾ��������
*			 _usCycle : ���д����� 0 ��ʾ��������
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void PERIOD_Start(PERIOD_CTRL_T *_ptPer, uint16_t _usOnTime, uint16_t _usOffTime, uint16_t _usCycle)
{
	if (_usOnTime == 0)
	{
		return;
	}

	_ptPer->usOnTime = (_usOnTime + 9) / 10;
	_ptPer->usOffTime = (_usOffTime + 9) / 10;
	_ptPer->usCycle = _usCycle;
	_ptPer->usCount = 0;
	_ptPer->usCycleCount = 0;
	_ptPer->ucState = 0;
	_ptPer->ucEnalbe = 1;	/* ������ȫ�ֲ�������ʹ�ܷ�����־ */

	_ptPer->OnFunc();		/* ִ��ONʱ�ĺ��� */
}

/*
*********************************************************************************************************
*	�� �� ��: PERIOD_Stop
*	����˵��: ֹͣ��������
*	��    �Σ���
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void PERIOD_Stop(PERIOD_CTRL_T *_ptPer)
{
	_ptPer->ucEnalbe = 0;

	/* ����������Ʊ�־����ֹͣ����������ֹͣ�����ж����ֿ��� */
	_ptPer->OffFunc();		/* ִ��ONʱ�ĺ��� */;	
}

/*
*********************************************************************************************************
*	�� �� ��: PERIOD_ScanDev
*	����˵��: ÿ��10ms����1�θú��������ڿ�����Ҫ��ʱִ�е��¼����ú����� bsp_timer.c �б����á�
*	��    ��: ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
static void PERIOD_ScanDev(PERIOD_CTRL_T *_ptPer)
{
	if ((_ptPer->ucEnalbe == 0) || (_ptPer->usOffTime == 0))
	{
		return;
	}

	if (_ptPer->ucState == 0)
	{
		if (_ptPer->usOffTime > 0)	/* ��� */
		{
			if (++_ptPer->usCount >= _ptPer->usOnTime)
			{
				_ptPer->OffFunc();	/* �رն��� */
				_ptPer->usCount = 0;
				_ptPer->ucState = 1;
			}
		}
		else
		{
			;	/* �����κδ����������� */
		}
	}
	else if (_ptPer->ucState == 1)
	{
		if (++_ptPer->usCount >= _ptPer->usOffTime)
		{
			/* ��������ʱ��ֱ������stopֹͣΪֹ */
			if (_ptPer->usCycle > 0)
			{
				if (++_ptPer->usCycleCount >= _ptPer->usCycle)
				{
					/* ѭ����������ֹͣ���� */
					_ptPer->ucEnalbe = 0;
				}

				if (_ptPer->ucEnalbe == 0)
				{
					_ptPer->usOffTime = 0;
					return;
				}
			}

			_ptPer->usCount = 0;
			_ptPer->ucState = 0;

			_ptPer->OnFunc();			/* ������ִ��On���� */
		}
	}
}

/***************************** ���������� www.armfly.com (END OF FILE) *********************************/
