/*
*********************************************************************************************************
*
*	ģ������ : H7-TOOL ��չIO��������
*	�ļ����� : bsp_fmc_io.h
*	˵    �� :
*
*	Copyright (C), 2015-2020, ���������� www.armfly.com
*
*********************************************************************************************************
*/
#ifndef __BSP_FMC_IO_H
#define __BSP_FMC_IO_H

#define  EIO_READ_FMC()	 *(uint16_t *)0x60001000

/* ���ⲿ���õĺ������� */
enum
{
	EIO_D0 = 0,
	EIO_D1,
	EIO_D2,
	EIO_D3,
	EIO_D4,
	EIO_D5,
	EIO_D6,
	EIO_D7,
	
	EIO_D8,		
	EIO_D9,		
	
	EIO_D10,	/* TTL_TX */
	EIO_D11,	/* TTL_RX */
	
	EIO_D12,	/* CAM_TX */
	EIO_D13,	/* CAM_RX */
	
	
	EIO_485_TXEN = 100,		/* RS485������� */
};

/* IO����ѡ�� */
typedef enum
{	
	ES_GPIO_IN  = 0,		/* GPIO ���룬 FMC���� */
	ES_GPIO_OUT = 1,		/* GPIO ����� FMC���� */
	ES_FMC_OUT  = 2,		/* GPIO ���룬 FMC��� */
	
	ES_FMC_NOE	= 3,		/* D8ר�ã�FMC_D8��FMC_NOE���� */
	ES_FMC_NWE	= 4,		/* D9ר�ã�FMC_D9��FMC_NWE���� */
	
	ES_GPIO_UART  = 5,		/* ��ΪGPIO���ڹ��� */
	ES_GPIO_CAN = 6,		/* CAN���� */
}EIO_SELECT_E;

void bsp_InitExtIO(void);
void EIO_ConfigPort(uint8_t _eio, EIO_SELECT_E _mode);
void EIO_SetOutLevel(uint8_t _eio, uint8_t _level);
uint8_t EIO_GetInputLevel(uint8_t _eio);
uint8_t EIO_GetOutLevel(uint8_t _eio);

void EIO_D0_Config(EIO_SELECT_E _mode);
void EIO_D1_Config(EIO_SELECT_E _mode);
void EIO_D2_Config(EIO_SELECT_E _mode);
void EIO_D3_Config(EIO_SELECT_E _mode);
void EIO_D4_Config(EIO_SELECT_E _mode);
void EIO_D5_Config(EIO_SELECT_E _mode);
void EIO_D6_Config(EIO_SELECT_E _mode);
void EIO_D7_Config(EIO_SELECT_E _mode);
void EIO_D8_Config(EIO_SELECT_E _mode);
void EIO_D9_Config(EIO_SELECT_E _mode);
void EIO_D10_Config(EIO_SELECT_E _mode);
void EIO_D11_Config(EIO_SELECT_E _mode);
void EIO_D12_Config(EIO_SELECT_E _mode);
void EIO_D13_Config(EIO_SELECT_E _mode);

uint16_t EIO_ReadFMC(void);

#endif

/***************************** ���������� www.armfly.com (END OF FILE) *********************************/
