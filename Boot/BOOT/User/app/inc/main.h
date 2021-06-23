/*
*********************************************************************************************************
*
*	ģ������ : mainģ��
*	�ļ����� : main.h
*	��    �� : V1.0
*
*	Copyright (C), 2013-2014, ���������� www.armfly.com
*
*********************************************************************************************************
*/

#ifndef _MAIN_H_
#define _MAIN_H_

/* STM32H750�� FALASH ������СΪ128KB�� boot����Ҫռ��128KB  */
#define APPLICATION_ADDRESS		0x08020000 		// ѡ��128K�Ժ��������ΪAPP��������2MB-128KB)
//#define APPLICATION_ADDRESS		0x08100000 	// ѡ���2��BANK ��1MB) ��ΪAPP������
#define APPLICATION_SIZE		(2 * 1024 * 1024 - 128 * 1024)		// Ӧ�ó����������	

/* UI��ɫ���� */
#define CL_MAIN_BACK		CL_BUTTON_GREY
#define CL_MAIN_FONT		CL_GREY

typedef struct 
{
	uint32_t CPU_Sn[3];
	
	uint16_t BootSystem;
	uint16_t BootStatus;
	
	uint16_t ProgType;	/* APP��������: 0 = CPU�ڲ�Flash ; 1 = QSPI Flash */
	uint32_t ProgAddr;	/* APP�����ַ 32λ */	
	uint32_t ProgSize;	/* APP���򳤶� 32λ */
	
	uint32_t FlashAddr;
	uint8_t ProgDatabuf[2048];
	uint16_t ProgDataLen;
	uint16_t WriteLen;
	
	uint8_t DispDir;
	
	uint8_t Mode;		/* 0��ʾSTM32�̼������� 1��ʾESP32 WiFi�̼����� */
}VAR_T;


extern VAR_T g_tVar;

#endif

/***************************** ���������� www.armfly.com (END OF FILE) *********************************/

