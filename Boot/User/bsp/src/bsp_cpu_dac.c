/*
*********************************************************************************************************
*
*	ģ������ : DAC���η�����
*	�ļ����� : bsp_cpu_dac.c
*	��    �� : V1.0
*	˵    �� : ʹ��STM32�ڲ�DAC������Ρ�֧��DAC1��DAC2�����ͬ�Ĳ��Ρ�
*
*	�޸ļ�¼ :
*		�汾��  ����        ����     ˵��
*		V1.0    2019-02-07  armfly  ��ʽ����
*
*	Copyright (C), 2015-2030, ���������� www.armfly.com
*
*********************************************************************************************************
*/

#include "bsp.h"
#include "param.h"

/*
	H7-TOOL ʹ��PA4���� DAC_OUT1

	DAC1ʹ����TIM6��Ϊ��ʱ������ DMAͨ��: DMA1_Stream5
//	DAC2ʹ����TIM7��Ϊ��ʱ������ DMAͨ��: DMA2_Stream6	
	
	DAC_InitStructure.DAC_OutputBuffer = DAC_OutputBuffer_Enable ������DAC������壬������������,
	���˻���֮�󣬿���0V�Ͳο���Դʱ��ʧ�����������50mV
	�������岨�νϺã���0VĿ�ⲻ������ʧ�档
	
	���ܣ�
	1��������Ҳ������Ⱥ�Ƶ�ʿɵ���
	2���������������ƫ�ƿɵ��ڣ�Ƶ�ʿɵ��ڣ�ռ�ձȿ��Ե���
	3��������ǲ������ȿɵ��ڣ�Ƶ�ʿɵ��ڣ�������ռ�ȿɵ���
	4��������DAC���ֱ����ƽ�ĺ���
	
	
	Ӳ����PG3���������ѹ���� 0-10V����10V
	
	Ӳ���� PE3/65130_SW ����DAC��·�ĵ�Դ
*/
/* ����DAC��·�ĵ�Դ PE3/65130_SW */
#define DAC_POWER_CLK_ENABLE()	__HAL_RCC_GPIOE_CLK_ENABLE()
#define DAC_POWER_GPIO			GPIOE
#define DAC_POWER_PIN			GPIO_PIN_3
#define DAC_POWER_ON()  		DAC_POWER_GPIO->BSRRL = DAC_POWER_PIN			/* DAC POWER ON */
#define DAC_POWER_OFF()  		DAC_POWER_GPIO->BSRRH = DAC_POWER_PIN			/* DAC POWER OFF */
#define DAC_POWER_IS_ON()		((DAC_POWER_GPIO->IDR & DAC_POWER_PIN) == 0)	/* �����ʹ�����������ture */

/* DAC ���Ŷ��� */
#define DACx                            DAC1
#define DACx_CHANNEL_GPIO_CLK_ENABLE()  __HAL_RCC_GPIOA_CLK_ENABLE()
#define DMAx_CLK_ENABLE()               __HAL_RCC_DMA1_CLK_ENABLE()

#define DACx_CLK_ENABLE()               __HAL_RCC_DAC12_CLK_ENABLE()
#define DACx_FORCE_RESET()              __HAL_RCC_DAC12_FORCE_RESET()
#define DACx_RELEASE_RESET()            __HAL_RCC_DAC12_RELEASE_RESET()
					  
					  /* Definition for DACx Channel Pin */
#define DACx_CHANNEL_PIN                GPIO_PIN_4
#define DACx_CHANNEL_GPIO_PORT          GPIOA
					  
/* Definition for DACx's DMA_STREAM */
#define DACx_CHANNEL                    DAC_CHANNEL_1

/* Definition for DACx's DMA_STREAM */
#define DACx_DMA_INSTANCE               DMA1_Stream5

/* Definition for DACx's NVIC */
#define DACx_DMA_IRQn                   DMA1_Stream5_IRQn
#define DACx_DMA_IRQHandler             DMA1_Stream5_IRQHandler

/* DMA���λ����� */
#define WAVE_SAMPLE_SIZE	128
uint16_t g_Wave1[WAVE_SAMPLE_SIZE];
//uint16_t g_Wave2[128];
					  
DAC_WAVE_T g_tDacWave;					  
					  
DAC_HandleTypeDef    DacHandle;
static DAC_ChannelConfTypeDef sConfig;

static void TIM6_Config(uint32_t _freq);
	
/*
*********************************************************************************************************
*	�� �� ��: bsp_InitDAC1
*	����˵��: ����PA4/DAC1�� ������DMA������ bsp_SetDAC1()�����޸����DACֵ
*	��    ��: ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void bsp_InitDAC1(void)
{	
	/* ��ѹ������̿���GPIO���� */
	{
		GPIO_InitTypeDef gpio_init;


		/* DAC power ���� */		
		DAC_POWER_CLK_ENABLE();
		DAC_POWER_OFF();
		gpio_init.Mode = GPIO_MODE_OUTPUT_PP;	/* ����������� */
		gpio_init.Pull = GPIO_NOPULL;			/* ���������費ʹ�� */
		gpio_init.Speed = GPIO_SPEED_FREQ_HIGH;  /* GPIO�ٶȵȼ� */			
		gpio_init.Pin = DAC_POWER_PIN;
		HAL_GPIO_Init(DAC_POWER_GPIO, &gpio_init);
	}
	
	/* ����DAC, �޴���������DMA */
	{		
		DacHandle.Instance = DACx;

		HAL_DAC_DeInit(&DacHandle);
		
		  /*##-1- Initialize the DAC peripheral ######################################*/
		if (HAL_DAC_Init(&DacHandle) != HAL_OK)
		{
			Error_Handler(__FILE__, __LINE__);
		}

		/*##-1- DAC channel1 Configuration #########################################*/
		sConfig.DAC_Trigger = DAC_TRIGGER_NONE;
		sConfig.DAC_OutputBuffer = DAC_OUTPUTBUFFER_ENABLE;
		//sConfig.DAC_OutputBuffer = DAC_OUTPUTBUFFER_DISABLE;

		if (HAL_DAC_ConfigChannel(&DacHandle, &sConfig, DAC_CHANNEL_1) != HAL_OK)
		{
			Error_Handler(__FILE__, __LINE__);
		}

		/*##-2- Enable DAC selected channel and associated DMA #############################*/
		if (HAL_DAC_Start(&DacHandle, DAC_CHANNEL_1) != HAL_OK)  
		{
			Error_Handler(__FILE__, __LINE__);
		}
	}
	
	g_tDacWave.Type = DAC_WAVE_NONE;
	g_tDacWave.CycleSetting = 0;
}


/*
*********************************************************************************************************
*	�� �� ��: bsp_SetDAC1
*	����˵��: ����DAC1������ݼĴ������ı������ѹ
*	��    ��: _dac : DAC���ݣ�0-4095
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void bsp_SetDAC1(uint16_t _dac)
{
	DAC1->DHR12R1 = _dac;
}

/*
*********************************************************************************************************
*	�� �� ��: bsp_SetDAC2
*	����˵��: ����DAC2������ݼĴ������ı������ѹ
*	��    ��: _dac : DAC���ݣ�0-4095
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void bsp_SetDAC2(uint16_t _dac)
{
	DAC1->DHR12R2 = _dac;
}

/*
*********************************************************************************************************
*	�� �� ��: HAL_DAC_MspInit
*	����˵��: ����DAC�õ���ʱ�ӣ����ź�DMAͨ��
*	��    ��: hdac  DAC_HandleTypeDef���ͽṹ��ָ�����
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void HAL_DAC_MspInit(DAC_HandleTypeDef *hdac)
{
	GPIO_InitTypeDef          GPIO_InitStruct;

	/*##-1- ʹ��ʱ�� #################################*/
	/* ʹ��GPIOʱ�� */
	DACx_CHANNEL_GPIO_CLK_ENABLE();
	/* ʹ��DAC����ʱ�� */
	DACx_CLK_ENABLE();

	/*##-2- ����GPIO ##########################################*/
	/* DAC Channel1 GPIO ���� */
	GPIO_InitStruct.Pin = DACx_CHANNEL_PIN;
	GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	HAL_GPIO_Init(DACx_CHANNEL_GPIO_PORT, &GPIO_InitStruct);
}

/*
*********************************************************************************************************
*	�� �� ��: HAL_DAC_MspDeInit
*	����˵��: ��λDAC
*	��    ��: hdac  DAC_HandleTypeDef���ͽṹ��ָ�����
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void HAL_DAC_MspDeInit(DAC_HandleTypeDef *hdac)
{
	/*##-1- ��λDAC���� ##################################################*/
	DACx_FORCE_RESET();
	DACx_RELEASE_RESET();

	/*##-2- ��λDAC��ӦGPIO ################################*/
	HAL_GPIO_DeInit(DACx_CHANNEL_GPIO_PORT, DACx_CHANNEL_PIN);

	/*##-3- �ر�DAC�õ�DMA Stream ############################################*/
	HAL_DMA_DeInit(hdac->DMA_Handle1);

	/*##-4- �ر�DMA�ж� ###########################################*/
	HAL_NVIC_DisableIRQ(DACx_DMA_IRQn);
}

/*
*********************************************************************************************************
*	�� �� ��: HAL_TIM_Base_MspInit
*	����˵��: ��ʼ����ʱ��ʱ��
*	��    ��: htim  TIM_HandleTypeDef���ͽṹ��ָ�����
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void HAL_TIM_Base_MspInit(TIM_HandleTypeDef *htim)
{
	/* TIM6 ʱ��ʹ�� */
	__HAL_RCC_TIM6_CLK_ENABLE();
}

/*
*********************************************************************************************************
*	�� �� ��: HAL_TIM_Base_MspDeInit
*	����˵��: ��λ��ʱ��ʱ��
*	��    ��: htim  TIM_HandleTypeDef���ͽṹ��ָ�����
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void HAL_TIM_Base_MspDeInit(TIM_HandleTypeDef *htim)
{
	/*##-1- ��λ���� ##################################################*/
	__HAL_RCC_TIM6_FORCE_RESET();
	__HAL_RCC_TIM6_RELEASE_RESET();
}

/*
*********************************************************************************************************
*	�� �� ��: TIM6_Config
*	����˵��: ����TIM6��ΪDAC����Դ
*	��    ��: _freq : ����Ƶ�ʣ���λHz
*	�� �� ֵ: ��
*********************************************************************************************************
*/
static void TIM6_Config(uint32_t _freq)
{
	static TIM_HandleTypeDef  htim;
	TIM_MasterConfigTypeDef sMasterConfig;

	/*##-1- Configure the TIM peripheral #######################################*/
	/* Time base configuration */
	htim.Instance = TIM6;
	
	if (_freq < 100)
	{
		htim.Init.Prescaler         = 100;		
		htim.Init.Period            = ((SystemCoreClock / 2) / htim.Init.Prescaler) / _freq - 1;		
		htim.Init.ClockDivision     = 0;
	}
	else
	{
		htim.Init.Period            = (SystemCoreClock / 2) / _freq - 1;
		htim.Init.Prescaler         = 0;
		htim.Init.ClockDivision     = 0;		
	}

	htim.Init.Period            = (SystemCoreClock / 2) / _freq - 1;
	htim.Init.Prescaler         = 0;
	htim.Init.ClockDivision     = 0;
	htim.Init.CounterMode       = TIM_COUNTERMODE_UP;
	htim.Init.RepetitionCounter = 0;
	HAL_TIM_Base_Init(&htim);

	/* TIM6 TRGO selection */
	sMasterConfig.MasterOutputTrigger = TIM_TRGO_UPDATE;
	sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;

	HAL_TIMEx_MasterConfigSynchronization(&htim, &sMasterConfig);

	/*##-2- Enable TIM peripheral counter ######################################*/
	HAL_TIM_Base_Start(&htim);
}

/*
*********************************************************************************************************
*	�� �� ��: bsp_StartDAC1_DMA
*	����˵��: ����PA4 ΪDAC_OUT1, ����DMA, ��ʼ�����������
*	��    ��: _BufAddr : DMA���ݻ�������ַ�� ���붨λ��0x24000000 RAM����flash����
*			  _Count  : ��������������
*			 _DacFreq : DAC��������Ƶ��, Hz,оƬ��������1MHz�� ʵ����Ե�10MHz.
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void bsp_StartDAC1_DMA(uint32_t _BufAddr, uint32_t _Count, uint32_t _DacFreq)
{		
	DAC_POWER_ON();		/* ��DAC�����· */
	
	TIM6_Config(_DacFreq);  /* DACת��Ƶ�����1M */
		
	/* ����DAC, TIM6������DMA���� */
	{		
		DacHandle.Instance = DACx;

		HAL_DAC_DeInit(&DacHandle);
		
		  /*##-1- Initialize the DAC peripheral ######################################*/
		if (HAL_DAC_Init(&DacHandle) != HAL_OK)
		{
			Error_Handler(__FILE__, __LINE__);
		}

		/* ����DMA */
		{
			static DMA_HandleTypeDef  hdma_dac1;
			
			/* ʹ��DMA1ʱ�� */
			DMAx_CLK_ENABLE();
			
			/* ���� DACx_DMA_STREAM  */
			hdma_dac1.Instance = DACx_DMA_INSTANCE;

			hdma_dac1.Init.Request  = DMA_REQUEST_DAC1;

			hdma_dac1.Init.Direction = DMA_MEMORY_TO_PERIPH;
			hdma_dac1.Init.PeriphInc = DMA_PINC_DISABLE;
			hdma_dac1.Init.MemInc = DMA_MINC_ENABLE;
			hdma_dac1.Init.PeriphDataAlignment = DMA_PDATAALIGN_HALFWORD;
			hdma_dac1.Init.MemDataAlignment = DMA_PDATAALIGN_HALFWORD;
			hdma_dac1.Init.Mode = DMA_CIRCULAR;
			hdma_dac1.Init.Priority = DMA_PRIORITY_HIGH;

			HAL_DMA_Init(&hdma_dac1);

			/* ����DMA�����DAC����� */
			__HAL_LINKDMA(&DacHandle, DMA_Handle1, hdma_dac1);
		}		

		/*##-1- DAC channel1 Configuration #########################################*/
		sConfig.DAC_Trigger = DAC_TRIGGER_T6_TRGO;
//		sConfig.DAC_OutputBuffer = DAC_OUTPUTBUFFER_ENABLE;	//GND����������ʧ������
		sConfig.DAC_OutputBuffer = DAC_OUTPUTBUFFER_DISABLE;

		if (HAL_DAC_ConfigChannel(&DacHandle, &sConfig, DAC_CHANNEL_1) != HAL_OK)
		{
			Error_Handler(__FILE__, __LINE__);
		}

		/*##-2- Enable DAC selected channel and associated DMA #############################*/
		if (HAL_DAC_Start_DMA(&DacHandle, DAC_CHANNEL_1, (uint32_t *)_BufAddr, _Count, DAC_ALIGN_12B_R) != HAL_OK)  
		{
			Error_Handler(__FILE__, __LINE__);
		}
	}
}

/*
*********************************************************************************************************
*	�� �� ��: dac1_SetSinWave
*	����˵��: DAC1������Ҳ�
*	��    ��: _vpp : ���� 0-2047;
*			  _freq : Ƶ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void dac1_SetSinWave(uint16_t _bottom, uint16_t _top, uint32_t _freq)
{	
	uint16_t i;
	uint16_t mid;	/* ��ֵ */
	uint16_t att;	/* ���� */

	mid = (_bottom + _top) / 2;	/* 0λ��ֵ */
	att = (_top - _bottom) / 2;  	/* ���Ҳ����ȣ����ֵ����2 */

	for (i = 0; i < WAVE_SAMPLE_SIZE; i++)
	{
		g_Wave1[i] = mid + (int32_t)(att * sin((i * 2 * 3.14159) / WAVE_SAMPLE_SIZE));
	}

	bsp_StartDAC1_DMA((uint32_t)&g_Wave1, WAVE_SAMPLE_SIZE, _freq * WAVE_SAMPLE_SIZE);
}

/*
*********************************************************************************************************
*	�� �� ��: dac1_SetRectWave
*	����˵��: DAC1�������
*	��    ��: _bottom : �͵�ƽʱDAC, 
*			  _top : �ߵ�ƽʱDAC
*			  _freq : Ƶ�� Hz
*			  _duty : ռ�ձ� 1% - 99%, ���ڲ��� 1%
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void dac1_SetRectWave(uint16_t _bottom, uint16_t _top, uint32_t _freq, uint16_t _duty)
{	
	uint16_t i;
	
	for (i = 0; i < (_duty * WAVE_SAMPLE_SIZE) / 100; i++)
	{
		g_Wave1[i] = _top;
	}
	for (; i < WAVE_SAMPLE_SIZE; i++)
	{
		g_Wave1[i] = _bottom;
	}
	
	bsp_StartDAC1_DMA((uint32_t)&g_Wave1, WAVE_SAMPLE_SIZE, _freq * WAVE_SAMPLE_SIZE);
}

/*
*********************************************************************************************************
*	�� �� ��: dac1_SetTriWave
*	����˵��: DAC1������ǲ�
*	��    ��: _bottom : �͵�ƽʱDAC, 
*			  _top : �ߵ�ƽʱDAC
*			  _freq : Ƶ�� Hz
*			  _duty : ռ�ձ�
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void dac1_SetTriWave(uint16_t _bottom, uint16_t _top, uint32_t _freq, uint16_t _duty)
{	
	uint32_t i;
	uint16_t dac;
	uint16_t m;
		
	/* �������ǲ����飬128���������� _bottom �� _top */		
	m = (_duty * WAVE_SAMPLE_SIZE) / 100;
	
	if (m == 0)
	{
		m = 1;
	}
	
	if (m > WAVE_SAMPLE_SIZE - 1)
	{
		m = WAVE_SAMPLE_SIZE - 1;
	}
	for (i = 0; i < m; i++)
	{
		dac = _bottom + ((_top - _bottom) * i) / m;
		g_Wave1[i] = dac;
	}
	for (; i < WAVE_SAMPLE_SIZE; i++)
	{
		dac = _top - ((_top - _bottom) * (i - m)) / (WAVE_SAMPLE_SIZE - m);
		g_Wave1[i] = dac;
	}	
	
	bsp_StartDAC1_DMA((uint32_t)&g_Wave1, WAVE_SAMPLE_SIZE, _freq * WAVE_SAMPLE_SIZE);
}

/*
*********************************************************************************************************
*	�� �� ��: dac1_StopWave
*	����˵��: ֹͣDAC1���
*	��    ��: ��
*			  _freq : Ƶ�� 0-5Hz
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void dac1_StopWave(void)
{	
//	__HAL_RCC_DAC12_FORCE_RESET();
//	__HAL_RCC_DAC12_RELEASE_RESET();
//	
//	HAL_DMA_DeInit(DacHandle.DMA_Handle1);
	
	HAL_DAC_Stop_DMA(&DacHandle, DAC_CHANNEL_1);	/* �����ڲ���ر�DAC */
	
	bsp_InitDAC1();	/* ��������DAC */
	
	bsp_SetDAC1(32767);
	
	DAC_POWER_OFF();
}

/*
*********************************************************************************************************
*	�� �� ��: dac1_DacToVolt
*	����˵��: DACֵת��Ϊ��ѹ mV   (��δ�õ���û��ʵ��4��У׼��
*	��    ��: _dac �� DACֵ��0-4095
*	�� �� ֵ: ��
*********************************************************************************************************
*/
int16_t dac1_DacToVolt(uint16_t _dac)
{
	int32_t volt;
	
	/* ����10V���� */
	{
		volt = CaculTwoPoint(g_tCalib.Dac10V.x1, g_tCalib.Dac10V.y1,
			g_tCalib.Dac10V.x2, g_tCalib.Dac10V.y2, _dac);
	}
	return volt;
}

/*
*********************************************************************************************************
*	�� �� ��: dac1_VoltToDac
*	����˵��: ��ѹ mVת��ΪDACֵ
*	��    ��: _volt �� mv��ѹֵ
*	�� �� ֵ: ��
*********************************************************************************************************
*/
int16_t dac1_VoltToDac(int16_t _volt)
{
	int32_t dac;
	int32_t x1,y1,x2,y2;
	
	if (_volt  <= g_tCalib.Dac10V.y2)
	{
		x1 = g_tCalib.Dac10V.y1;
		y1 = g_tCalib.Dac10V.x1;
		x2 = g_tCalib.Dac10V.y2;
		y2 = g_tCalib.Dac10V.x2;
	}		
	else if (_volt  <= g_tCalib.Dac10V.y3)
	{
		x1 = g_tCalib.Dac10V.y2;
		y1 = g_tCalib.Dac10V.x2;
		x2 = g_tCalib.Dac10V.y3;
		y2 = g_tCalib.Dac10V.x3;
	}	
	else
	{
		x1 = g_tCalib.Dac10V.y3;
		y1 = g_tCalib.Dac10V.x3;
		x2 = g_tCalib.Dac10V.y4;
		y2 = g_tCalib.Dac10V.x4;
	}	
	
	/* ����10V���� */
	dac = CaculTwoPoint(x1, y1, x2, y2, _volt);
	if (dac < 0)
	{
		dac = 0;
	}
	else if (dac > 4095)
	{
		dac = 4095;
	}
	return dac;
}


/*
*********************************************************************************************************
*	�� �� ��: dac1_DacToCurr
*	����˵��: DACֵת��Ϊ���� uA   (��δ�õ���û��ʵ��4��У׼��
*	��    ��: _dac �� DACֵ��0-4095
*	�� �� ֵ: ��
*********************************************************************************************************
*/
int16_t dac1_DacToCurr(uint16_t _dac)
{
	int32_t curr;
	
	curr = CaculTwoPoint(g_tCalib.Dac20mA.x1, g_tCalib.Dac20mA.y1,
			g_tCalib.Dac20mA.x2, g_tCalib.Dac20mA.y2, _dac);
	return curr;
}

/*
*********************************************************************************************************
*	�� �� ��: dac1_CurrToDac
*	����˵��: ����uAת��ΪDACֵ
*	��    ��: _curr �� uA ����ֵ
*	�� �� ֵ: ��
*********************************************************************************************************
*/
int16_t dac1_CurrToDac(int16_t _curr)
{
	int32_t dac;
	int32_t x1,y1,x2,y2;
	
	if (_curr  <= g_tCalib.Dac20mA.y2)
	{
		x1 = g_tCalib.Dac20mA.y1;
		y1 = g_tCalib.Dac20mA.x1;
		x2 = g_tCalib.Dac20mA.y2;
		y2 = g_tCalib.Dac20mA.x2;
	}		
	else if (_curr  <= g_tCalib.Dac20mA.y3)
	{
		x1 = g_tCalib.Dac20mA.y2;
		y1 = g_tCalib.Dac20mA.x2;
		x2 = g_tCalib.Dac20mA.y3;
		y2 = g_tCalib.Dac20mA.x3;
	}	
	else
	{
		x1 = g_tCalib.Dac20mA.y3;
		y1 = g_tCalib.Dac20mA.x3;
		x2 = g_tCalib.Dac20mA.y4;
		y2 = g_tCalib.Dac20mA.x4;
	}	
	
	/* ����10V���� */
	dac = CaculTwoPoint(x1, y1, x2, y2, _curr);
	if (dac < 0)
	{
		dac = 0;
	}
	else if (dac > 4095)
	{
		dac = 4095;
	}
	return dac;
}

/*
*********************************************************************************************************
*	�� �� ��: dac1_StartDacWave
*	����˵��: ��������
*	��    ��: �ޡ�  g_tDacWave ȫ�ֽṹ�д���в��β���
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void dac1_StartDacWave(void)
{
	uint16_t bottom;
	uint16_t top;
	
	g_tDacWave.CycleCount = 0;	
	
	switch (g_tDacWave.Type)
	{		
		case DAC_WAVE_NO:	
			dac1_StopWave();		
			DAC_POWER_ON();		/* ��DAC�����· */
			break;
		
		case DAC_WAVE_SIN:
			bottom = dac1_VoltToDac(g_tDacWave.VoltMin);
			top = dac1_VoltToDac(g_tDacWave.VoltMax);
			dac1_SetSinWave(bottom, top, g_tDacWave.Freq);
			break;
		
		case DAC_WAVE_SQUARE:
			bottom = dac1_VoltToDac(g_tDacWave.VoltMin);
			top = dac1_VoltToDac(g_tDacWave.VoltMax);
			dac1_SetRectWave(bottom, top, g_tDacWave.Freq, g_tDacWave.Duty);			
			break;
		
		case DAC_WAVE_TRI:
			bottom = dac1_VoltToDac(g_tDacWave.VoltMin);
			top = dac1_VoltToDac(g_tDacWave.VoltMax);
			dac1_SetTriWave(bottom, top, g_tDacWave.Freq, g_tDacWave.Duty);			
			break;
	}
}

/* DMA�������ʱ�ص� */
void HAL_DAC_ConvCpltCallbackCh1(DAC_HandleTypeDef* hdac)
{
	if (hdac == &DacHandle)
	{
		if (g_tDacWave.CycleSetting == 0)	/* һֱѭ�� */
		{
			;
		}
		else
		{
			++g_tDacWave.CycleCount;
			if (g_tDacWave.CycleCount >= g_tDacWave.CycleSetting)
			{
				HAL_DAC_Stop_DMA(&DacHandle, DAC_CHANNEL_1);
			}
		}
	}
}

/***************************** ���������� www.armfly.com (END OF FILE) *********************************/
