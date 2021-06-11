/*
*********************************************************************************************************
*
*	ģ������ : USB devie ���⴮������
*	�ļ����� : usbd_user.h
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

  
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __USBD_USER_H
#define __USBD_USER_H

/* Includes ------------------------------------------------------------------*/
#include "stm32h7xx_hal.h"
#include "usbd_core.h"
#include "usbd_desc.h"
#include "usbd_cdc.h" 
#include "usbd_cdc_interface.h"

void usbd_OpenCDC(uint8_t _com);
void usbd_CloseCDC(void);

#endif /* __MAIN_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
