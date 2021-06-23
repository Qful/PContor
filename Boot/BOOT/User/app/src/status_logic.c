/*
*********************************************************************************************************
*
*	ģ������ : �߼�������������
*	�ļ����� : status_logic.c
*	��    �� : V1.0
*	˵    �� : �߼������ǹ��ܡ�ʾ���������⴮�ڡ�����CAN��I2C��������SPI��������PWM��������GPIO����
*				DAC���ơ�������⡢���������롢Ƶ�ʼ�.
*	�޸ļ�¼ :
*		�汾��  ����        ����     ˵��
*		V1.0    2018-12-06 armfly  ��ʽ����
*
*	Copyright (C), 2018-2030, ���������� www.armfly.com
*
*********************************************************************************************************
*/
#include "bsp.h"
#include "param.h"
#include "main.h"
#include "status_logic.h"

#define FORM_BACK_COLOR	CL_BLUE

/*
*********************************************************************************************************
*	�� �� ��: status_LogicMain
*	����˵��: �߼�������״̬.
*	��    ��: ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void status_LogicMain(void)
{
	uint8_t ucKeyCode;		/* �������� */
	uint8_t fRefresh;
	FONT_T tFont;		/* ��������ṹ����� */

	/* ����������� */
	{
		tFont.FontCode = FC_ST_16;	/* ������� 16���� */
		tFont.FrontColor = CL_BLACK;	/* ������ɫ */
		tFont.BackColor = FORM_BACK_COLOR;	/* ���ֱ�����ɫ */
		tFont.Space = 0;				/* ���ּ�࣬��λ = ���� */

		LCD_ClrScr(FORM_BACK_COLOR);  	/* ������������ɫ */

		LCD_DispStr(5, 3, "�߼�������ʾ����", &tFont);
	}

	fRefresh = 1;
	while (g_MainStatus == MS_LOGIC)
	{
		bsp_Idle();

		if (fRefresh)	/* ˢ���������� */
		{
			fRefresh = 0;
		}

		ucKeyCode = bsp_GetKey();	/* ��ȡ��ֵ, �޼�����ʱ���� KEY_NONE = 0 */
		if (ucKeyCode != KEY_NONE)
		{
			/* �м����� */
			switch (ucKeyCode)
			{
				case  KEY_DOWN_S:		/* S�� */
					break;

				case  KEY_DOWN_C:		/* C�� */
					break;
				
				default:
					break;
			}
		}
	}
}

/***************************** ���������� www.armfly.com (END OF FILE) *********************************/
