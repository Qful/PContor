/*
*********************************************************************************************************
*
*	ģ������ : LEDָʾ������ģ��
*	�ļ����� : bsp_led.c
*	��    �� : V1.0
*	˵    �� : ����LEDָʾ��
*
*	�޸ļ�¼ :
*		�汾��  ����        ����     ˵��
*		V1.0    2018-09-05 armfly  ��ʽ����
*
*	Copyright (C), 2015-2030, ���������� www.armfly.com
*
*********************************************************************************************************
*/

#include "bsp.h"

#define ALL_LED_GPIO_CLK_ENABLE()	__HAL_RCC_GPIOI_CLK_ENABLE()

#define GPIO_LED1	GPIOI
#define PIN_LED1	GPIO_PIN_8

#define LED1_ON()  	GPIO_LED1->BSRR = ((uint32_t)PIN_LED1 << 16U)				/* LED1 = 0 */
#define LED1_OFF()  GPIO_LED1->BSRR = PIN_LED1				/* LED1 = 1 */

#define LED1_IS_ON()	((GPIO_LED1->IDR & PIN_LED1) == 0)	/* ����ѵ���������1 */


/*
*********************************************************************************************************
*	�� �� ��: bsp_InitLed
*	����˵��: ����LEDָʾ����ص�GPIO,  �ú����� bsp_Init() ���á�
*	��    ��:  ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void bsp_InitLed(void)
{
	GPIO_InitTypeDef gpio_init;

	/* ��1������GPIOʱ�� */
	ALL_LED_GPIO_CLK_ENABLE();
	
	bsp_LedOff(1);
	
	gpio_init.Mode = GPIO_MODE_OUTPUT_PP;	/* ���ÿ�©��� */
	gpio_init.Pull = GPIO_NOPULL;			/* ���������費ʹ�� */
	gpio_init.Speed = GPIO_SPEED_FREQ_HIGH;  /* GPIO�ٶȵȼ� */	
	gpio_init.Pin = PIN_LED1;	
	HAL_GPIO_Init(GPIO_LED1, &gpio_init);		
}

/*
*********************************************************************************************************
*	�� �� ��: bsp_LedOn
*	����˵��: ����ָ����LEDָʾ�ơ�
*	��    ��:  _no : ָʾ����ţ���Χ 1 - 4
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void bsp_LedOn(uint8_t _no)
{
	if (_no == 1)
	{
		LED1_ON();
	}
}

/*
*********************************************************************************************************
*	�� �� ��: bsp_LedOff
*	����˵��: Ϩ��ָ����LEDָʾ�ơ�
*	��    ��:  _no : ָʾ����ţ���Χ 1 - 4
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void bsp_LedOff(uint8_t _no)
{
	if (_no == 1)
	{
		LED1_OFF();
	}
}

/*
*********************************************************************************************************
*	�� �� ��: bsp_LedToggle
*	����˵��: ��תָ����LEDָʾ�ơ�
*	��    ��:  _no : ָʾ����ţ���Χ 1 - 4
*	�� �� ֵ: ��������
*********************************************************************************************************
*/
void bsp_LedToggle(uint8_t _no)
{
	if (bsp_IsLedOn(_no))
	{
		bsp_LedOff(_no);
	}
	else
	{
		bsp_LedOn(_no);
	}
}


/*
*********************************************************************************************************
*	�� �� ��: bsp_IsLedOn
*	����˵��: �ж�LEDָʾ���Ƿ��Ѿ�������
*	��    ��:  _no : ָʾ����ţ���Χ 1 - 4
*	�� �� ֵ: 1��ʾ�Ѿ�������0��ʾδ����
*********************************************************************************************************
*/
uint8_t bsp_IsLedOn(uint8_t _no)
{
	if (_no == 1)
	{
		return LED1_IS_ON();
	}
	
	return 0;
}


/*
*********************************************************************************************************
*	�� �� ��: bsp_TestSDIO
*	����˵��: ��תSDIO��GPIO. debug ��
*	��    ��:  ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void bsp_TestSDIO(void)
{
	GPIO_InitTypeDef gpio_init;
	
	__HAL_RCC_GPIOA_CLK_ENABLE();
	
	gpio_init.Mode = GPIO_MODE_OUTPUT_PP;	/* ���ÿ�©��� */
	gpio_init.Pull = GPIO_NOPULL;			/* ���������費ʹ�� */
	gpio_init.Speed = GPIO_SPEED_FREQ_HIGH;  /* GPIO�ٶȵȼ� */	
	gpio_init.Pin = GPIO_PIN_8 | GPIO_PIN_9 | GPIO_PIN_10 | GPIO_PIN_11 | GPIO_PIN_12;;	
	HAL_GPIO_Init(GPIOC, &gpio_init);	

	gpio_init.Pin = GPIO_PIN_2;
	HAL_GPIO_Init(GPIOD, &gpio_init);	
	
	while(1)
	{
		GPIOC->BSRR = ((uint32_t)(GPIO_PIN_8 | GPIO_PIN_9 | GPIO_PIN_10 | GPIO_PIN_11 | GPIO_PIN_12) << 16U);
		GPIOD->BSRR = ((uint32_t)(GPIO_PIN_2) << 16U);
		bsp_DelayMS(100);
		GPIOC->BSRR =  GPIO_PIN_8 | GPIO_PIN_9 | GPIO_PIN_10 | GPIO_PIN_11 | GPIO_PIN_12;
		GPIOD->BSRR = GPIO_PIN_2;
		bsp_DelayMS(100);		
	}
}

/***************************** ���������� www.armfly.com (END OF FILE) *********************************/
