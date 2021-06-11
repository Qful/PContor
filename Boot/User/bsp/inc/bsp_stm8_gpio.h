/*
*********************************************************************************************************
*
*	ģ������ : STM8 GPIOоƬ����
*	�ļ����� : bsp_stm8_gpio.h
*	��    �� : V2.0
*	˵    �� : ͷ�ļ�
*
*	Copyright (C), 2015-2020, ���������� www.armfly.com
*
*********************************************************************************************************
*/


#ifndef _BSP_I2C_LED8_H
#define _BSP_I2C_LED8_H

#define I2C_DEV_ADDR			0x94		/* �豸��ַ */
#define I2C_ADDR_BYTES			1			/* ��ַ�ֽڸ��� */

/* д���� */
#define REG_ID					 0x00        /* оƬID, �̶�ֵ 0x75 */
#define REG_SOFTVER              0x01        /* ����汾 */
#define REG_CFG1				 0x02		 /* GPIO �������üĴ���, 7��0  */
#define REG_CFG2				 0x03		 /* GPIO �������üĴ���, 10��8  */
#define REG_IN1					 0x04		 /* GPIO ����״̬�Ĵ���, 7��0  */
#define REG_IN2					 0x05		 /* GPIO ����״̬�Ĵ���, 10��8  */
#define REG_OUT1				 0x06		 /* GPIO ���״̬�Ĵ���, 7��0  */
#define REG_OUT2				 0x07		 /* GPIO ���״̬�Ĵ���, 10��8  */
#define REG_PWM1_H				 0x08		 /* PWM1ռ�ձȸ��ֽ� */
#define REG_PWM1_L				 0x09		 /* PWM1ռ�ձȵ��ֽ� */
#define REG_PWM2_H				 0x0A		 /* PWM2ռ�ձȸ��ֽ� */
#define REG_PWM2_L				 0x0B		 /* PWM2ռ�ձȵ��ֽ� */

uint8_t STM8_InitHard(void);
void STM8_WriteGPIO(uint8_t _pin, uint8_t _value);

#endif


