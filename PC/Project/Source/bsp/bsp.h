
/*
*********************************************************************************************************
*
*	ģ������ : ָ������ģ��
*	�ļ����� : bsp_finger.h
*	��    �� : V1.0
*	˵    �� : ͷ�ļ�
*
*	Copyright (C), 2015-2016, ���������� www.armfly.com
*
*********************************************************************************************************
*/
#ifndef __BSP_H
#define __BSP_H

#include <vcl.h>
#include "string.h"
#include "bsp_user_lib.h"
#include "bsp_timer.h"
#include "bsp_dso_ctrl.h"
#include "bsp_dac_ctrl.h"
#include "bsp_param.h"

#include "stdio.h"

/* Signed integer types  */
#if 0
typedef   char     int8_t;
typedef   signed short    int16_t;
typedef   signed int     int32_t;
typedef   signed long long    int64_t;

/* Unsigned integer types  */
typedef unsigned char     uint8_t;
typedef unsigned short    uint16_t;
typedef unsigned int     uint32_t;
#else
typedef unsigned int     uint32_t;
#endif

#define DISABLE_INT()
#define ENABLE_INT()

#define BUFFER_SIZE  256



void bsp_Idle(void);

void RS485_SendBuf1(uint8_t *_buf, uint8_t _len);
uint8_t comGetChar(uint8_t *_pData);

void bsp_ClearLog(void);
void bsp_AddLog(const char *_log);
AnsiString bsp_GetLog(void);

#endif

/***************************** ���������� www.armfly.com (END OF FILE) *********************************/

