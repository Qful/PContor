/*
*********************************************************************************************************
*
*	ģ������ : USB devie ���⴮������
*	�ļ����� : usbd_user.c
*	��    �� : V1.0
*	˵    �� : ��װ���⴮�ڲ����������ṩ��APPʹ��.
*
*	�޸ļ�¼ :
*		�汾��  ����        ����     ˵��
*		V1.0    2018-12-11  armfly  ��ʽ����
*
*	Copyright (C), 2018-2030, ���������� www.armfly.com
*
*********************************************************************************************************
*/

#include "usbd_user.h"

USBD_HandleTypeDef USBD_Device;
extern PCD_HandleTypeDef hpcd;

extern void SelectCDCUart(uint8_t _com);
/*
*********************************************************************************************************
*	�� �� ��: usbd_OpenCDC
*	����˵��: ��USB
*	��    ��: _com : 1, 4
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void usbd_OpenCDC(uint8_t _com)
{
	SelectCDCUart(_com);		/* ѡ��uart1��uart4��Ϊ���⴮�� */
	
	/* Init Device Library */
	USBD_Init(&USBD_Device, &VCP_Desc, 0);

	/* Add Supported Class */
	USBD_RegisterClass(&USBD_Device, USBD_CDC_CLASS);

	/* Add CDC Interface Class */
	USBD_CDC_RegisterInterface(&USBD_Device, &USBD_CDC_fops);

	/* Start Device Process */
	USBD_Start(&USBD_Device);
}

/*
*********************************************************************************************************
*	�� �� ��: usbd_CloseCDC
*	����˵��: �ر�USB
*	��    ��: ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void usbd_CloseCDC(void)
{
	USBD_Stop(&USBD_Device);
	
	USBD_DeInit(&USBD_Device);
}

/***************************** ���������� www.armfly.com (END OF FILE) *********************************/
