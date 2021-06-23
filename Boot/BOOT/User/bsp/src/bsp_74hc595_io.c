/*
*********************************************************************************************************
*
*	ģ������ : ʾ����ģ��74HC595��չ���������ģ��
*	�ļ����� : bsp_74hc595_io.c
*	��    �� : V1.0
*	˵    �� : ʹ��74HC595��չ8�������
*	�޸ļ�¼ :
*		�汾��  ����       ����    ˵��
*		v1.0    2019-08-13 armfly  
*
*	Copyright (C), 2016-2020, ���������� www.armfly.com
*
*********************************************************************************************************
*/

#include "bsp.h"

static uint8_t s_HC595;

/*
	PI4/595_LOCK
	PI9/595_SCLK
	PG5/595_SDI
	
*/
#define ALL_HC595_GPIO_CLK_ENABLE()	__HAL_RCC_GPIOI_CLK_ENABLE();	\
									__HAL_RCC_GPIOG_CLK_ENABLE();

#define HC595_LCK_GPIO	GPIOI
#define HC595_LCK_PIN	GPIO_PIN_4
#define HC595_LCK_0()  	HC595_LCK_GPIO->BSRRH = HC595_LCK_PIN	
#define HC595_LCK_1()  	HC595_LCK_GPIO->BSRRL = HC595_LCK_PIN

#define HC595_SCLK_GPIO	GPIOI
#define HC595_SCLK_PIN	GPIO_PIN_9
#define HC595_SCLK_0()	HC595_SCLK_GPIO->BSRRH = HC595_SCLK_PIN	
#define HC595_SCLK_1()	HC595_SCLK_GPIO->BSRRL = HC595_SCLK_PIN

#define HC595_SDI_GPIO	GPIOG
#define HC595_SDI_PIN	GPIO_PIN_5
#define HC595_SDI_0()  	HC595_SDI_GPIO->BSRRH = HC595_SDI_PIN	
#define HC595_SDI_1()  	HC595_SDI_GPIO->BSRRL = HC595_SDI_PIN

static void HC595_Delay(void);

/*
*********************************************************************************************************
*	�� �� ��: HC595_InitHard
*	����˵��: ��ʼ��Ӳ��
*	��    ��: ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void HC595_InitHard(void)
{
	GPIO_InitTypeDef gpio_init;

	/* ��GPIOʱ�� */
	ALL_HC595_GPIO_CLK_ENABLE();
	
	gpio_init.Mode = GPIO_MODE_OUTPUT_PP;		/* ����������� */
	gpio_init.Pull = GPIO_NOPULL;				/* ���������費ʹ�� */
	gpio_init.Speed = GPIO_SPEED_FREQ_HIGH;  	/* GPIO�ٶȵȼ� */	
	
	gpio_init.Pin = HC595_LCK_PIN;	
	HAL_GPIO_Init(HC595_LCK_GPIO, &gpio_init);		

	gpio_init.Pin = HC595_SCLK_PIN;	
	HAL_GPIO_Init(HC595_SCLK_GPIO, &gpio_init);		

	gpio_init.Pin = HC595_SDI_PIN;	
	HAL_GPIO_Init(HC595_SDI_GPIO, &gpio_init);			
	
	s_HC595 = 0;
	HC595_LockData();
}

/*
*********************************************************************************************************
*	�� �� ��: HC595_Delay
*	����˵��: �ӳ�
*	��    ��: ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
static void HC595_Delay(void)
{
	uint8_t i;
	
	for (i = 0; i < 5; i++);
}

/*
*********************************************************************************************************
*	�� �� ��: STM8_WriteGPIO
*	����˵��: ����GPIO���״̬
*	��    ��: _pin : 0-7
*			  _value : 0,1
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void HC595_WriteGPIO(uint8_t _pin, uint8_t _value)
{
	if (_value == 1)
	{
		s_HC595 |= (1 << _pin);
	}
	else
	{
		s_HC595 &= ~(1 << _pin);
	}

	//HC595_LockData();
}

/*
*********************************************************************************************************
*	�� �� ��: HC595_LockData
*	����˵��: ��������
*	��    ��: ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void HC595_LockData(void)
{
	uint8_t i;
	uint8_t dd;	
	
	dd = s_HC595;	
	for (i = 0; i < 8; i++)
	{
		if (dd & 0x80)
		{
			HC595_SDI_1();
		}
		else
		{
			HC595_SDI_0();
		}

		HC595_Delay();
		HC595_SCLK_0();			;
		HC595_Delay();
		HC595_SCLK_1();
		dd <<= 1;
	}
	
	HC595_Delay();
	HC595_LCK_1();			;
	HC595_Delay();	HC595_Delay(); HC595_Delay();
	HC595_LCK_0();	
}

/***************************** ���������� www.armfly.com (END OF FILE) *********************************/
