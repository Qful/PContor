/*
*********************************************************************************************************
*
*	ģ������ : BSPģ��(For STM32H7)
*	�ļ����� : bsp.h
*	��    �� : V1.0
*	˵    �� : ����Ӳ���ײ�������������ļ���ÿ��c�ļ����� #include "bsp.h" ���������е���������ģ�顣
*			   bsp = Borad surport packet �弶֧�ְ�
*	�޸ļ�¼ :
*		�汾��  ����         ����       ˵��
*		V1.0    2018-07-29  Eric2013   ��ʽ����
*
*	Copyright (C), 2018-2030, ���������� www.armfly.com
*
*********************************************************************************************************
*/

#ifndef _BSP_H
#define _BSP_H


/* ���� BSP �汾�� */
#define __STM32H7_BSP_VERSION		"1.1"

/* CPU����ʱִ�еĺ��� */
//#define CPU_IDLE()		bsp_Idle()

/* ����ȫ���жϵĺ� */
#define ENABLE_INT()	__set_PRIMASK(0)	/* ʹ��ȫ���ж� */
#define DISABLE_INT()	__set_PRIMASK(1)	/* ��ֹȫ���ж� */

/* ���������ڵ��Խ׶��Ŵ� */
#define BSP_Printf		printf
//#define BSP_Printf(...)

#define EXTI9_5_ISR_MOVE_OUT		/* bsp.h �ж�����У���ʾ�������Ƶ� stam32f4xx_it.c�� �����ظ����� */

#define ERROR_HANDLER()		Error_Handler(__FILE__, __LINE__)

/* Ĭ���ǹر�״̬ */
#define  Enable_EventRecorder  0

#if Enable_EventRecorder == 1
	#include "EventRecorder.h"
#endif

#include "stm32h7xx_hal.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>

#ifndef TRUE
	#define TRUE  1
#endif

#ifndef FALSE
	#define FALSE 0
#endif

/* �������ȼ����� */
#define NVIC_PREEMPT_PRIORITY	4

/* ͨ��ȡ��ע�ͻ������ע�͵ķ�ʽ�����Ƿ�����ײ�����ģ�� */
#include "bsp_msg.h"
#include "bsp_user_lib.h"
#include "bsp_timer.h"
#include "bsp_led.h"
#include "bsp_key.h"

#include "bsp_cpu_adc.h"
#include "bsp_cpu_dac.h"
#include "bsp_cpu_flash.h"
//#include "bsp_uart_fifo.h"
#include "bsp_cpu_rtc.h"
#include "bsp_esp32_at.h"

#include "bsp_spi_bus.h"
#include "bsp_qspi_w25q256.h"

//#include "bsp_fmc_io.h"

//#include "bsp_i2c_gpio.h"
//#include "bsp_i2c_eeprom_24xx.h"
//#include "bsp_i2c_mcp4018.h"
//#include "bsp_i2c_mcp4725.h"
//#include "bsp_power_tvcc.h"


#include "bsp_tft_st7735.h"
#include "bsp_tft_st7789.h"
#include "bsp_tft_lcd.h"

//#include "bsp_beep.h"
#include "bsp_tim_pwm.h"
#include "bsp_fmc_io.h"

#include "bsp_period_ctrl.h"

#include "bsp_tim_dma.h"
#include "bsp_tim_capture.h"

#include "bsp_74hc595_io.h"

#include "bsp_emmc.h"
#include "bsp_ntc.h"

/* �ṩ������C�ļ����õĺ��� */
void bsp_Init(void);
void bsp_Idle(void);

void bsp_GetCpuID(uint32_t *_id);
void Error_Handler(char *file, uint32_t line);
void bsp_ResetSystem(void);

#endif

/***************************** ���������� www.armfly.com (END OF FILE) *********************************/
