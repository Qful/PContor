/*
*********************************************************************************************************
*
*	ģ������ : ����EEPROM 24xx02����ģ��
*	�ļ����� : bsp_eeprom_24xx.h
*	��    �� : V1.0
*	˵    �� : ͷ�ļ�
*
*	Copyright (C), 2012-2013, ���������� www.armfly.com
*
*********************************************************************************************************
*/

#ifndef _BSP_EEPROM_24XX_H
#define _BSP_EEPROM_24XX_H

/* H7-TOOL��װ����24C16 (2K�ֽڣ� */

//#define AT24C02
//#define AT24C04
#define AT24C16		
//#define AT24C128

#ifdef AT24C02
	#define EE_MODEL_NAME		"AT24C02"
	#define EE_DEV_ADDR			0xA0		/* �豸��ַ */
	#define EE_PAGE_SIZE		8			/* ҳ���С(�ֽ�) */
	#define EE_SIZE				256			/* ������(�ֽ�) */
	#define EE_ADDR_BYTES		1			/* ��ַ�ֽڸ��� */
#endif

#ifdef AT24C04
	#define EE_MODEL_NAME		"AT24C04"
	#define EE_DEV_ADDR			0xA0		/* �豸��ַ */
	#define EE_PAGE_SIZE		16			/* ҳ���С(�ֽ�) */
	#define EE_SIZE				512			/* ������(�ֽ�) */
	#define EE_ADDR_BYTES		1			/* ��ַ�ֽڸ��� */
#endif

#ifdef AT24C16
	#define EE_MODEL_NAME		"AT24C16"
	#define EE_DEV_ADDR			0xA0		/* �豸��ַ */
	#define EE_PAGE_SIZE		16			/* ҳ���С(�ֽ�) */
	#define EE_SIZE				2048		/* ������(�ֽ�) */
	#define EE_ADDR_BYTES		1			/* ��ַ�ֽڸ��� */
#endif

#ifdef AT24C128
	#define EE_MODEL_NAME		"AT24C128"
	#define EE_DEV_ADDR			0xA0		/* �豸��ַ */
	#define EE_PAGE_SIZE		64			/* ҳ���С(�ֽ�) */
	#define EE_SIZE				(16*1024)	/* ������(�ֽ�) */
	#define EE_ADDR_BYTES		2			/* ��ַ�ֽڸ��� */
#endif

uint8_t ee_CheckOk(void);
uint8_t ee_ReadBytes(uint8_t *_pReadBuf, uint16_t _usAddress, uint16_t _usSize);
uint8_t ee_WriteBytes(uint8_t *_pWriteBuf, uint16_t _usAddress, uint16_t _usSize);

#endif

/***************************** ���������� www.armfly.com (END OF FILE) *********************************/
