/*
*********************************************************************************************************
*
*	ģ������ : 12bit DACоƬMCP4725����ģ��
*	�ļ����� : bsp_i2c_mcp4725.h
*	��    �� : V1.0
*	˵    �� : ͷ�ļ�
*
*	Copyright (C), 2018-2030, ���������� www.armfly.com
*
*********************************************************************************************************
*/

#ifndef _BSP_MCP4725_H
#define _BSP_MCP4725_H

#define MCP4025_SLAVE_ADDRESS    0xC0		/* I2C�ӻ���ַ */

void bsp_InitMCP4725(void);
void MCP4725_WriteData(uint8_t _ch, uint16_t _usDac);
void MCP4725_SetVolt(uint8_t _ch, uint16_t _volt);

#endif

/***************************** ���������� www.armfly.com (END OF FILE) *********************************/
