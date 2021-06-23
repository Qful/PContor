/*
*********************************************************************************************************
*
*	ģ������ : TIM����ģ��
*	�ļ����� : bsp_tim_capture.c
*	��    �� : V1.0
*	˵    �� : ����TIM5 CH1-CH4ͨ����ʱ�����ܣ�DMA����ʱ�̵��ڴ档
*
*	�޸ļ�¼ :
*		�汾��  ����         ����     ˵��
*		V1.0    2019-01-29   armfly  ��ʽ����
*
*	Copyright (C), 2019-2030, ���������� www.armfly.com
*
*********************************************************************************************************
*/

#include "bsp.h"

/* 

	PH10/TIM5_CH1	- FMC_D1
	PH11/TIM5_CH2	- FMC_D3
	PH12/TIM5_CH3   - BEEP 
	PI0/TIM5_CH4	- FMC_D0	
*/

#define IR_REPEAT_SEND_EN		1	/* ����ʹ�� */
#define IR_REPEAT_FILTER		10	/* ң����108ms ��������������, ��������1��������ط� */

/* ����GPIO�˿� */
#define TIM5_CH1_CLK_ENABLE() 	__HAL_RCC_GPIOH_CLK_ENABLE()
#define TIM5_CH1_GPIO			GPIOH
#define TIM5_CH1_PIN			GPIO_PIN_10

#define TIM5_CH2_CLK_ENABLE() 	__HAL_RCC_GPIOH_CLK_ENABLE()
#define TIM5_CH2_GPIO			GPIOH
#define TIM5_CH2_PIN			GPIO_PIN_11

#define TIM5_CH3_CLK_ENABLE() 	__HAL_RCC_GPIOH_CLK_ENABLE()
#define TIM5_CH3_GPIO			GPIOH
#define TIM5_CH3_PIN			GPIO_PIN_12

#define TIM5_CH4_CLK_ENABLE() 	__HAL_RCC_GPIOI_CLK_ENABLE()
#define TIM5_CH4_GPIO			GPIOI
#define TIM5_CH4_PIN			GPIO_PIN_0

/* PB8/TIM4_CH3 �������� */
#define	IRD_TIMx_IRQHandler		TIM4_IRQHandler
#define TIMx					TIM4
#define TIMx_CHANNEL			TIM_CHANNEL_3
#define TIMx_ACTIVE_CHANNEL 	HAL_TIM_ACTIVE_CHANNEL_3

/* Definition for TIMx's NVIC */
#define TIMx_IRQn				TIM4_IRQn

#define TIMx_CLK_ENABLE()		__HAL_RCC_TIM4_CLK_ENABLE()
#define TIMx_GPIO_AF_TIMx  		GPIO_AF2_TIM4

static TIM_HandleTypeDef    s_TimHandle;
IRD_T g_tIR;

static void IRD_DecodeNec(uint16_t _width);


ALIGN_32BYTES(uint8_t TIM_CH1_Buffer[16])

/*
*********************************************************************************************************
*	�� �� ��: bsp_InitTIM5Capture
*	����˵��: ����TIM5��Ϊ���벶��
*	��    ��: ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void bsp_InitTIM5Capture(void)
{
	GPIO_InitTypeDef gpio_init;
	
	gpio_init.Mode = GPIO_MODE_AF_PP;				/* ����AFģʽ */
	gpio_init.Pull = GPIO_NOPULL;					/* ���������費ʹ�� */
	gpio_init.Speed = GPIO_SPEED_FREQ_VERY_HIGH;	/* GPIO�ٶȵȼ� */	
	
	gpio_init.Pin = TIM5_CH1_PIN;	
	HAL_GPIO_Init(TIM5_CH1_GPIO, &gpio_init);	
	
	gpio_init.Pin = TIM5_CH2_PIN;	
	HAL_GPIO_Init(TIM5_CH2_GPIO, &gpio_init);

	gpio_init.Pin = TIM5_CH3_PIN;	
	HAL_GPIO_Init(TIM5_CH3_GPIO, &gpio_init);

	gpio_init.Pin = TIM5_CH4_PIN;	
	HAL_GPIO_Init(TIM5_CH4_GPIO, &gpio_init);
}

/*
*********************************************************************************************************
*	�� �� ��: TIM5_StartWork
*	����˵��: ����TIM5��ʼ����
*	��    ��: ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void TIM5_StartWork(void)
{
	/* ����GPIO */
	{
		GPIO_InitTypeDef gpio_init;
		
		/* GPIOʱ��ʹ�� */
		TIM5_CH1_CLK_ENABLE();
		TIM5_CH2_CLK_ENABLE();
		TIM5_CH3_CLK_ENABLE();
		TIM5_CH4_CLK_ENABLE();
		
		gpio_init.Mode = GPIO_MODE_AF_PP;		/* ���ÿ�©��� */
		gpio_init.Pull = GPIO_NOPULL;			/* ���������費ʹ�� */
		gpio_init.Speed = GPIO_SPEED_FREQ_VERY_HIGH;  /* GPIO�ٶȵȼ� */
		gpio_init.Alternate = GPIO_AF2_TIM5;
		
		gpio_init.Pin = TIM5_CH1_PIN;	
		HAL_GPIO_Init(TIM5_CH1_GPIO, &gpio_init);	
		
		gpio_init.Pin = TIM5_CH2_PIN;	
		HAL_GPIO_Init(TIM5_CH2_GPIO, &gpio_init);

		gpio_init.Pin = TIM5_CH3_PIN;	
		HAL_GPIO_Init(TIM5_CH3_GPIO, &gpio_init);

		gpio_init.Pin = TIM5_CH4_PIN;	
		HAL_GPIO_Init(TIM5_CH4_GPIO, &gpio_init);
	}
  
	/* ����TIM */
	{
		TIM_IC_InitTypeDef     sICConfig;
		  
		/* Set TIMx instance */
		s_TimHandle.Instance = TIM5;

		/* ���÷�ƵΪ 1680/2�� ���������ֵ�ĵ�λ������ 10us, ��������Ƚ� 
			SystemCoreClock ����Ƶ. ����ֵ: 168000000, 180000000,192000000
		*/

		s_TimHandle.Init.Period            = 0xFFFF;
		s_TimHandle.Init.Prescaler         = (SystemCoreClock / 100000) / 2;
		s_TimHandle.Init.ClockDivision     = 0;
		s_TimHandle.Init.CounterMode       = TIM_COUNTERMODE_UP;
		s_TimHandle.Init.RepetitionCounter = 0;
		if (HAL_TIM_IC_Init(&s_TimHandle) != HAL_OK)
		{
			Error_Handler(__FILE__, __LINE__);
		}

		/*##-2- Configure the Input Capture channel ################################*/ 
		/* Configure the Input Capture of channel 2 */
		sICConfig.ICPolarity  = TIM_ICPOLARITY_BOTHEDGE;
		sICConfig.ICSelection = TIM_ICSELECTION_DIRECTTI;
		sICConfig.ICPrescaler = TIM_ICPSC_DIV1;
		sICConfig.ICFilter    = 0;   
		if (HAL_TIM_IC_ConfigChannel(&s_TimHandle, &sICConfig, TIM_CHANNEL_1) != HAL_OK)
		{
			Error_Handler(__FILE__, __LINE__);			
		}
		
		/* Non-Blocking mode: DMA */
//		HAL_StatusTypeDef HAL_TIM_IC_Start_DMA(TIM_HandleTypeDef *htim, uint32_t Channel, uint32_t *pData, uint16_t Length);
//		HAL_StatusTypeDef HAL_TIM_IC_Stop_DMA(TIM_HandleTypeDef *htim, uint32_t Channel);
				

//		/*##-3- Start the Input Capture in interrupt mode ##########################*/
//		if (HAL_TIM_IC_Start_IT(&s_TimHandle, TIMx_CHANNEL) != HAL_OK)
//		{
//			Error_Handler(__FILE__, __LINE__);
//		}	  
		
		HAL_TIM_IC_Start_DMA(&s_TimHandle, TIM_CHANNEL_1, uint32_t *pData, uint16_t Length);
		
		__HAL_RCC_TIM5_CLK_ENABLE();
	}
	
	{
		/* TIMx Peripheral clock enable */
		TIMx_CLK_ENABLE();
		
		

		HAL_NVIC_SetPriority(TIMx_IRQn, 0, 1);

		/* Enable the TIMx global Interrupt */
		HAL_NVIC_EnableIRQ(TIMx_IRQn);	
	}
	
	g_tIR.LastCapture = 0;	
	g_tIR.Status = 0;
}


/*
*********************************************************************************************************
*	�� �� ��: IRD_TIMx_IRQHandler
*	����˵��: TIM�жϷ������
*	��    ��: ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void IRD_TIMx_IRQHandler(void)
{
	HAL_TIM_IRQHandler(&s_TimHandle);
}

/*
*********************************************************************************************************
*	�� �� ��: HAL_TIM_IC_CaptureCallback
*	����˵��: TIM�жϷ�������л���ñ��������в����¼�����
*	��    ��: ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim)
{
	uint16_t NowCapture;
	uint16_t Width;
	
	if (htim->Channel == TIMx_ACTIVE_CHANNEL)
	{
		NowCapture = HAL_TIM_ReadCapturedValue(htim, TIMx_CHANNEL);

		if (NowCapture >= g_tIR.LastCapture)
		{
			Width = NowCapture - g_tIR.LastCapture;
		}
		else if (NowCapture < g_tIR.LastCapture)	/* �������ִ���󲢷�ת */
		{
			Width = ((0xFFFF - g_tIR.LastCapture) + NowCapture);
		}			
		
		if ((g_tIR.Status == 0) && (g_tIR.LastCapture == 0))
		{
			g_tIR.LastCapture = NowCapture;
			return;
		}
				
		g_tIR.LastCapture = NowCapture;	/* ���浱ǰ�������������´μ����ֵ */
		
		IRD_DecodeNec(Width);		/* ���� */		
	}
}

/*
*********************************************************************************************************
*	�� �� ��: IRD_StopWork
*	����˵��: ֹͣ�������
*	��    ��: ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void IRD_StopWork(void)
{
	HAL_TIM_IC_DeInit(&s_TimHandle);		
	
	HAL_TIM_IC_Stop_IT(&s_TimHandle, TIMx_CHANNEL);
}

/*
*********************************************************************************************************
*	�� �� ��: IRD_DecodeNec
*	����˵��: ����NEC�����ʽʵʱ����
*	��    ��: _width �����ȣ���λ 10us
*	�� �� ֵ: ��
*********************************************************************************************************
*/
static void IRD_DecodeNec(uint16_t _width)
{
	static uint16_t s_LowWidth;
	static uint8_t s_Byte;
	static uint8_t s_Bit;
	uint16_t TotalWitdh;
	
	/* NEC ��ʽ ��5�Σ�
		1��������  9ms�� + 4.5ms��
		2����8λ��ַ��  0=1.125ms  1=2.25ms    bit0�ȴ�
		3����8λ��ַ��  0=1.125ms  1=2.25ms
		4��8λ����      0=1.125ms  1=2.25ms
		5��8Ϊ���뷴��  0=1.125ms  1=2.25ms
	*/

loop1:	
	switch (g_tIR.Status)
	{
		case 0:			/* 929 �ȴ���������ź�  7ms - 11ms */
			if ((_width > 700) && (_width < 1100))
			{
				g_tIR.Status = 1;
				s_Byte = 0;
				s_Bit = 0;
			}
			break;

		case 1:			/* 413 �ж���������ź�  3ms - 6ms */
			if ((_width > 313) && (_width < 600))	/* ������ 4.5ms */
			{
				g_tIR.Status = 2;
			}
			else if ((_width > 150) && (_width < 250))	/* 2.25ms */
			{
				#ifdef IR_REPEAT_SEND_EN				
					if (g_tIR.RepeatCount >= IR_REPEAT_FILTER)
					{
						bsp_PutKey(g_tIR.RxBuf[2] + IR_KEY_STRAT);	/* ������ */
					}
					else
					{
						g_tIR.RepeatCount++;
					}
				#endif
				g_tIR.Status = 0;	/* ��λ����״̬ */
			}
			else
			{
				/* �쳣���� */
				g_tIR.Status = 0;	/* ��λ����״̬ */
			}
			break;
		
		case 2:			/* �͵�ƽ�ڼ� 0.56ms */
			if ((_width > 10) && (_width < 100))
			{		
				g_tIR.Status = 3;
				s_LowWidth = _width;	/* ����͵�ƽ��� */
			}
			else	/* �쳣���� */
			{
				/* �쳣���� */
				g_tIR.Status = 0;	/* ��λ������״̬ */	
				goto loop1;		/* �����ж�ͬ���ź� */
			}
			break;

		case 3:			/* 85+25, 64+157 ��ʼ��������32bit */						
			TotalWitdh = s_LowWidth + _width;
			/* 0�Ŀ��Ϊ1.125ms��1�Ŀ��Ϊ2.25ms */				
			s_Byte >>= 1;
			if ((TotalWitdh > 92) && (TotalWitdh < 132))
			{
				;					/* bit = 0 */
			}
			else if ((TotalWitdh > 205) && (TotalWitdh < 245))
			{
				s_Byte += 0x80;		/* bit = 1 */
			}	
			else
			{
				/* �쳣���� */
				g_tIR.Status = 0;	/* ��λ������״̬ */	
				goto loop1;		/* �����ж�ͬ���ź� */
			}
			
			s_Bit++;
			if (s_Bit == 8)	/* ����8λ */
			{
				g_tIR.RxBuf[0] = s_Byte;
				s_Byte = 0;
			}
			else if (s_Bit == 16)	/* ����16λ */
			{
				g_tIR.RxBuf[1] = s_Byte;
				s_Byte = 0;
			}
			else if (s_Bit == 24)	/* ����24λ */
			{
				g_tIR.RxBuf[2] = s_Byte;
				s_Byte = 0;
			}
			else if (s_Bit == 32)	/* ����32λ */
			{
				g_tIR.RxBuf[3] = s_Byte;
								
				if (g_tIR.RxBuf[2] + g_tIR.RxBuf[3] == 255)	/* ���У�� */
				{
					bsp_PutKey(g_tIR.RxBuf[2] + IR_KEY_STRAT);	/* ����ֵ����KEY FIFO */
					
					g_tIR.RepeatCount = 0;	/* �ط������� */										
				}
				
				g_tIR.Status = 0;	/* �ȴ���һ����� */
				break;
			}
			g_tIR.Status = 2;	/* ������һ��bit */
			break;						
	}
}

/***************************** ���������� www.armfly.com (END OF FILE) *********************************/
