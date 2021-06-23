/*
*********************************************************************************************************
*
*	ģ������ : NTC����ģ��
*	�ļ����� : bsp_ntc.c
*	��    �� : V1.0
*	˵    �� : 
*
*	�޸ļ�¼ :
*		�汾��    ����           ����          ˵��
*       V1.0    2017-09-20		armfly		  �׷�
*
*	Copyright (C), 2016-2020, ���������� www.armfly.com
*
*********************************************************************************************************
*/

#ifndef _BSP_NTC_H_
#define _BSP_NTC_H_

float CalculNtcTemperFloat(float _adc);

int16_t CalculNtcTemperInt(float _adc);
float CalculNtcRes(float ref_res, float _adc);
float CalculRefRes(float _adc, float _res);

#endif

/***************************** ���������� www.armfly.com (END OF FILE) *********************************/
