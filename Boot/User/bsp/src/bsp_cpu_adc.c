/*
*********************************************************************************************************
*
*	ģ������ : ʾ����ADCģ��
*	�ļ����� : bsp_adc_dso.c
*	��    �� : V1.0
*	˵    �� : ʹ��STM32�ڲ�ADC��ͬ���ɼ���·���Ρ�ռ���˲���GPIO����ʾ����ģ����������Ϸ�ʽ��
*				ʹ�� ADC_EXTERNALTRIG_T3_TRGO ��ΪADC����Դ
*
*	�޸ļ�¼ :
*		�汾��  ����        ����     ˵��
*		V1.0    2018-10-25  armfly  ��ʽ����
*
*	Copyright (C), 2018-2030, ���������� www.armfly.com
*
*********************************************************************************************************
*/

#include "bsp.h"
#include "param.h"

/*
	PF11/ADC1_INP2	---- CH1��ѹ
	PA6/ADC1_INP3	---- �߶˵���

	PC3_C/ADC3_INP1	---- CH2��ѹ
	PF5/ADC3_INP4	---- TVCC����������
	PF3/ADC3_INP5	---- NTC����������ֵ���
	PF8/ADC3_INP7	---- ���ص�ѹ
	PH2/ADC3_INP13	---- TVCC��ѹ���		
	PH3/ADC3_INP14	---- 12V�����ѹ���
	PH5/ADC3_INP16	---- USB�����ѹ���			
*/
		
/*
	H7-TOOL ʾ������·��AC/DC��ϡ��������GPIO
*/	
/* CH1 ������� */
#define G1A_PIN				0
#define G1A_0()				HC595_WriteGPIO(G1A_PIN, 0)
#define G1A_1()				HC595_WriteGPIO(G1A_PIN, 1)

#define G1B_PIN				1
#define G1B_0()				HC595_WriteGPIO(G1B_PIN, 0)
#define G1B_1()				HC595_WriteGPIO(G1B_PIN, 1)

#define G1C_PIN				2
#define G1C_0()				HC595_WriteGPIO(G1C_PIN, 0)
#define G1C_1()				HC595_WriteGPIO(G1C_PIN, 1)

/* CH2 ������� */
#define G2A_PIN				4
#define G2A_0()				HC595_WriteGPIO(G2A_PIN, 0)
#define G2A_1()				HC595_WriteGPIO(G2A_PIN, 1)

#define G2B_PIN				5
#define G2B_0()				HC595_WriteGPIO(G2B_PIN, 0)
#define G2B_1()				HC595_WriteGPIO(G2B_PIN, 1)

#define G2C_PIN				6
#define G2C_0()				HC595_WriteGPIO(G2C_PIN, 0)
#define G2C_1()				HC595_WriteGPIO(G2C_PIN, 1)

/* CH1 ����/ֱ����� */
#define AC1_PIN				3
#define AC1_0()				HC595_WriteGPIO(AC1_PIN, 0)
#define AC1_1()				HC595_WriteGPIO(AC1_PIN, 1)

/* CH2 ����/ֱ����� */
#define AC2_PIN				7
#define AC2_0()				HC595_WriteGPIO(AC2_PIN, 0)
#define AC2_1()				HC595_WriteGPIO(AC2_PIN, 1)


/* �߶˵��� ������� */
#define GC_CLK_ENABLE()		__HAL_RCC_GPIOG_CLK_ENABLE()
#define GC_GPIO				GPIOG
#define GC_PIN				GPIO_PIN_2
#define GC_0()				GC_GPIO->BSRRH = GC_PIN
#define GC_1()				GC_GPIO->BSRRL = GC_PIN


/* ʾ����ģʽ����ѹģʽ�͵���ģʽʹ�ò�ͬ��ADCͨ�� */

#define	ENABLE_DIFFERENTIAL_ENDED		0	/* 0��ʾ����Ϊ���ˣ�1��ʾ����Ϊ��� */

#define	H7_ADC_SAMPLETIME_1CYCLE_5	ADC_SAMPLETIME_1CYCLE_5

/********************************** ��ѹģʽ��GPIO���� ***********************************/

/*-------------ADC CH1ͨ��GPIO, ADCͨ����DMA���� -------------*/
#if 1
	#define ADCH1                            ADC1
	#define ADCH1_CLK_ENABLE()               __HAL_RCC_ADC12_CLK_ENABLE()
	#define ADCH1_FORCE_RESET()              __HAL_RCC_ADC12_FORCE_RESET()
	#define ADCH1_RELEASE_RESET()            __HAL_RCC_ADC12_RELEASE_RESET()
	#define ADCH1_CHANNEL                    ADC_CHANNEL_2		/* ��PF11 */
	
	/* ������� PF11/ADC1_INP2 + PF13/ADC2_INP2 + PF9/ADC3_INP2 */
	#define ADCH1_P_GPIO_CLK_ENABLE()  __HAL_RCC_GPIOF_CLK_ENABLE()
	#define ADCH1_P_GPIO_PORT          GPIOF
	//#define ADCH1_P_PIN                GPIO_PIN_11 | GPIO_PIN_9 | GPIO_PIN_13
	#define ADCH1_P_PIN                GPIO_PIN_11

	/* ��ָ��� PF12/ADC1_INN2 + PF10/ADC3_INN2 + PF14/ADC2_INN2 */
	#define ADCH1_N_GPIO_CLK_ENABLE()  __HAL_RCC_GPIOF_CLK_ENABLE()
	#define ADCH1_N_GPIO_PORT          GPIOF
	#define ADCH1_N_PIN                GPIO_PIN_12 | GPIO_PIN_10 | GPIO_PIN_14

	#define CH1_DMA_CLK_ENABLE()           __HAL_RCC_DMA1_CLK_ENABLE()
	#define CH1_DMA_Stream					 DMA1_Stream1
	#define CH1_DMA_Stream_IRQn				 DMA1_Stream1_IRQn
	#define CH1_DMA_Stream_IRQHandle         DMA1_Stream1_IRQHandler
	#define CH1_DMA_REQUEST_ADC				 DMA_REQUEST_ADC1
#else
	#define ADCH1                            ADC3
	#define ADCH1_CLK_ENABLE()               __HAL_RCC_ADC3_CLK_ENABLE()
	#define ADCH1_FORCE_RESET()              __HAL_RCC_ADC3_FORCE_RESET()
	#define ADCH1_RELEASE_RESET()            __HAL_RCC_ADC3_RELEASE_RESET()
	#define ADCH1_CHANNEL                    ADC_CHANNEL_2		/* ��PF11 */
	/* ������� PF11/ADC1_INP2 + PF13/ADC2_INP2 + PF9/ADC3_INP2 */
	#define ADCH1_P_GPIO_CLK_ENABLE()  __HAL_RCC_GPIOF_CLK_ENABLE()
	#define ADCH1_P_GPIO_PORT          GPIOF
	//#define ADCH1_P_PIN                GPIO_PIN_11 | GPIO_PIN_9 | GPIO_PIN_13
	#define ADCH1_P_PIN                GPIO_PIN_9

	/* ��ָ��� PF12/ADC1_INN2 + PF10/ADC3_INN2 + PF14/ADC2_INN2 */
	#define ADCH1_N_GPIO_CLK_ENABLE()  __HAL_RCC_GPIOF_CLK_ENABLE()
	#define ADCH1_N_GPIO_PORT          GPIOF
	#define ADCH1_N_PIN                GPIO_PIN_12 | GPIO_PIN_10 | GPIO_PIN_14

	#define CH1_DMA_CLK_ENABLE()           __HAL_RCC_DMA1_CLK_ENABLE()
	#define CH1_DMA_Stream					 DMA1_Stream1
	#define CH1_DMA_Stream_IRQn				 DMA1_Stream1_IRQn
	#define CH1_DMA_Stream_IRQHandle         DMA1_Stream1_IRQHandler
	#define CH1_DMA_REQUEST_ADC				 DMA_REQUEST_ADC1
#endif

/*------------- ADC CH2ͨ��GPIO, ADCͨ����DMA���� -------------*/
#if 1
	#define ADCH2                            ADC3
	#define ADCH2_CLK_ENABLE()               __HAL_RCC_ADC3_CLK_ENABLE()
	#define ADCH2_FORCE_RESET()              __HAL_RCC_ADC3_FORCE_RESET()
	#define ADCH2_RELEASE_RESET()            __HAL_RCC_ADC3_RELEASE_RESET()
	#define ADCH2_CHANNEL					ADC_CHANNEL_1
	/* ������� PC3_C/ADC3_INP1 */
	#define ADCH2_P_GPIO_CLK_ENABLE()  		__HAL_RCC_GPIOC_CLK_ENABLE()
	#define ADCH2_P_PIN                		GPIO_PIN_3
	#define ADCH2_P_GPIO_PORT         	 	GPIOC
	/* ��ָ��� PC2_C/ADC3_INN1 */
	#define ADCH2_N_GPIO_CLK_ENABLE()  		__HAL_RCC_GPIOC_CLK_ENABLE()
	#define ADCH2_N_PIN                		GPIO_PIN_2
	#define ADCH2_N_GPIO_PORT         	 	GPIOC

	#define CH2_DMA_CLK_ENABLE()           __HAL_RCC_DMA2_CLK_ENABLE()
	#define CH2_DMA_Stream					 DMA2_Stream1
	#define CH2_DMA_Stream_IRQn				 DMA2_Stream1_IRQn
	#define CH2_DMA_Stream_IRQHandle         DMA2_Stream1_IRQHandler
	#define CH2_DMA_REQUEST_ADC				 DMA_REQUEST_ADC3
#else
	#define ADCH2                            ADC3
	#define ADCH2_CLK_ENABLE()               __HAL_RCC_ADC3_CLK_ENABLE()
	#define ADCH2_FORCE_RESET()              __HAL_RCC_ADC3_FORCE_RESET()
	#define ADCH2_RELEASE_RESET()            __HAL_RCC_ADC3_RELEASE_RESET()
	#define ADCH2_CHANNEL					ADC_CHANNEL_2

	/* ������� PF9/ADC3_INP2 */ 
	#define ADCH2_P_GPIO_CLK_ENABLE()  		__HAL_RCC_GPIOF_CLK_ENABLE()
	#define ADCH2_P_PIN                		GPIO_PIN_9
	#define ADCH2_P_GPIO_PORT         	 	GPIOF
	/* ��ָ��� PC2_C/ADC3_INN1 */
	#define ADCH2_N_GPIO_CLK_ENABLE()  		__HAL_RCC_GPIOC_CLK_ENABLE()
	#define ADCH2_N_PIN                		GPIO_PIN_2
	#define ADCH2_N_GPIO_PORT         	 	GPIOC

	#define CH2_DMA_CLK_ENABLE()           __HAL_RCC_DMA2_CLK_ENABLE()
	#define CH2_DMA_Stream					 DMA2_Stream1
	#define CH2_DMA_Stream_IRQn				 DMA2_Stream1_IRQn
	#define CH2_DMA_Stream_IRQHandle         DMA2_Stream1_IRQHandler
	#define CH2_DMA_REQUEST_ADC				 DMA_REQUEST_ADC3

#endif


/********************************** ��ѹģʽ��GPIO���壨������********************************/


/********************************** �߶˵���ģʽ��GPIO���� (�޲�֣� ***********************************/
/*
	�߶˵����� PA6/ADC12_INP3
	���ص�ѹ�� PF8/ADC3_INP7
*/
/* ADC CH1ͨ��GPIO, ADCͨ����DMA���� */
#define CT_ADCH1                            ADC1
#define CT_ADCH1_CLK_ENABLE()               __HAL_RCC_ADC12_CLK_ENABLE()
#define CT_ADCH1_FORCE_RESET()              __HAL_RCC_ADC12_FORCE_RESET()
#define CT_ADCH1_RELEASE_RESET()            __HAL_RCC_ADC12_RELEASE_RESET()
#define CT_ADCH1_CHANNEL                    ADC_CHANNEL_3
/* ������� PA6/ADC12_INP3 */
#define CT_ADCH1_P_GPIO_CLK_ENABLE()  __HAL_RCC_GPIOA_CLK_ENABLE()
#define CT_ADCH1_P_GPIO_PORT          GPIOA
#define CT_ADCH1_P_PIN                GPIO_PIN_6
/* ��ָ���û�� */
#define CT_ADCH1_N_GPIO_CLK_ENABLE()  __HAL_RCC_GPIOA_CLK_ENABLE()
#define CT_ADCH1_N_GPIO_PORT          GPIOA
#define CT_ADCH1_N_PIN                GPIO_PIN_6

#define CT_CH1_DMA_CLK_ENABLE()           __HAL_RCC_DMA1_CLK_ENABLE()
#define CT_CH1_DMA_Stream					 DMA1_Stream1
#define CT_CH1_DMA_Stream_IRQn				 DMA1_Stream1_IRQn
#define CT_CH1_DMA_Stream_IRQHandle         DMA1_Stream1_IRQHandler
#define CT_CH1_DMA_REQUEST_ADC				 DMA_REQUEST_ADC1

/* ADC CH2ͨ��GPIO, ADCͨ����DMA���� */
#define CT_ADCH2                            ADC3
#define CT_ADCH2_CLK_ENABLE()               __HAL_RCC_ADC3_CLK_ENABLE()
#define CT_ADCH2_FORCE_RESET()              __HAL_RCC_ADC3_FORCE_RESET()
#define CT_ADCH2_RELEASE_RESET()            __HAL_RCC_ADC3_RELEASE_RESET()
#define CT_ADCH2_CHANNEL					ADC_CHANNEL_7
/* ������� PF8/ADC3_INP7 */
#define CT_ADCH2_P_GPIO_CLK_ENABLE()  		__HAL_RCC_GPIOF_CLK_ENABLE()
#define CT_ADCH2_P_PIN                		GPIO_PIN_8
#define CT_ADCH2_P_GPIO_PORT         	 	GPIOF
/* ��ָ���û�У�PF8/ADC3_INP7 */
#define CT_ADCH2_N_GPIO_CLK_ENABLE()  		__HAL_RCC_GPIOF_CLK_ENABLE()
#define CT_ADCH2_N_PIN                		GPIO_PIN_8
#define CT_ADCH2_N_GPIO_PORT         	 	GPIOF

#define CT_CH2_DMA_CLK_ENABLE()           __HAL_RCC_DMA2_CLK_ENABLE()
#define CT_CH2_DMA_Stream					 DMA2_Stream1
#define CT_CH2_DMA_Stream_IRQn				 DMA2_Stream1_IRQn
#define CT_CH2_DMA_Stream_IRQHandle          DMA2_Stream1_IRQHandler
#define CT_CH2_DMA_REQUEST_ADC				 DMA_REQUEST_ADC3
/********************************** �߶˵���ģʽ��GPIO���壨������ *******************************/

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* ADC handle declaration */
ADC_HandleTypeDef             AdcHandle1 = {0};
ADC_HandleTypeDef             AdcHandle2 = {0};

/* ADC channel configuration structure declaration */
ADC_ChannelConfTypeDef        sConfig1 = {0};
ADC_ChannelConfTypeDef        sConfig2 = {0};

/* Variable containing ADC conversions data */
ALIGN_32BYTES (uint16_t   aADCH1ConvertedData[ADC_BUFFER_SIZE]);
ALIGN_32BYTES (uint16_t   aADCH2ConvertedData[ADC_BUFFER_SIZE]);

uint16_t g_Ch1WaveBuf[ADC_BUFFER_SIZE];
uint16_t g_Ch2WaveBuf[ADC_BUFFER_SIZE];


#define SCAN_MODE_ADC1_NUM			2		/* ����ɨ��ģʽ, ADC1ͨ������ */
#define SCAN_MODE_ADC3_NUM			7		/* ����ɨ��ģʽ, ADC3ͨ������ */
#define SCAN_MODE_SAMPLE_SIZE		64		/* ����ɨ��ģʽ, ÿͨ����������. �������ƽ�� */
#define SCAN_MODE_SAMPLETIME_ADC1		ADC_SAMPLETIME_64CYCLES_5	//ADC_SAMPLETIME_810CYCLES_5
#define SCAN_MODE_SAMPLETIME_ADC3		ADC_SAMPLETIME_64CYCLES_5

const int32_t TabelFreq[FREQ_NUM] =
{
	100,
	200,
	500,
	1 * 1000,
	2 * 1000,
	5 * 1000,
	10 * 1000,
	20 * 1000,
	50 * 1000,
	100 * 1000,
	200 * 1000,
	500 * 1000,
	1 * 1000000,
	2 * 1000000,
	5 * 1000000,
	10 * 1000000,
	20 * 1000000,
};

/* ������� */
int TabelBufSize[BUFF_SIZE_NUM] =
{
	1*1024,
	2*1024,
	4*1024,
	8*1024,
	16*1024,
	32*1024,
	64*1024,
	128*1024,
	256*1024,
	512*1024,
};

DSO_T g_tDSO;

static void TIM3_Config(uint32_t _freq);
static float AdcSumAvg(uint8_t _AdcNo, uint8_t _Offset);

void bsp_StopAdcCH1(void);
void bsp_StopAdcCH2(void);

/*
*********************************************************************************************************
*	�� �� ��: DSO_TrigFinished
*	����˵��: �����ɼ�����. ���ж���ִ�е�.
*	��    ��: ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void DSO_TrigFinished(void)
{
	//DSO_LockWave();  -- ����������ط���������
	DSO_PauseADC();
	g_tDSO.TrigFlag = 1;
	
}

/*
*********************************************************************************************************
*	�� �� ��: ADC_IRQHandler
*	����˵��: ADCģ�⿴�Ź��ж�.  ; ADC1, ADC2 
*	��    ��: ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void IRQ_WatchDog(void)
{
	uint32_t TransBuffSize;	/* ͨ�Ŵ���Ĳ������- PC������õ� */
	int32_t DmaPos;
	int32_t BeginPos;
	uint32_t tmp_isr;
	uint32_t tmp_ier;
	uint8_t triged = 0;
	uint16_t *pAdcBuf;
	ADC_HandleTypeDef *hadc;
	
//	HAL_ADC_IRQHandler(&AdcHandle1);
//	HAL_ADC_IRQHandler(&AdcHandle2);	
	
	/* CH1 ���� */
	tmp_isr = AdcHandle1.Instance->ISR;
	tmp_ier = AdcHandle1.Instance->IER;
	if (((tmp_isr & ADC_FLAG_AWD1) == ADC_FLAG_AWD1) && ((tmp_ier & ADC_IT_AWD1) == ADC_IT_AWD1))
	{
		/* Clear ADC Analog watchdog flag */
		AdcHandle1.Instance->ISR = ADC_FLAG_AWD1;
		
		pAdcBuf = aADCH1ConvertedData;
		hadc = &AdcHandle1;
		DmaPos = ADC_BUFFER_SIZE - CT_CH1_DMA_Stream->NDTR;		/* DMA����λ�� */		
		triged = 1;
	}

	/* CH2 ����*/
	tmp_isr = AdcHandle2.Instance->ISR;
	tmp_ier = AdcHandle2.Instance->IER;
	if (((tmp_isr & ADC_FLAG_AWD1) == ADC_FLAG_AWD1) && ((tmp_ier & ADC_IT_AWD1) == ADC_IT_AWD1))
	{
		/* Clear ADC Analog watchdog flag */
		AdcHandle2.Instance->ISR = ADC_FLAG_AWD1;
		
		pAdcBuf = aADCH2ConvertedData;
		hadc = &AdcHandle2;		
		DmaPos = ADC_BUFFER_SIZE - CT_CH2_DMA_Stream->NDTR;		/* DMA����ʣ���ֽ��� */	
		triged = 2;
	}	

	if (triged == 1 || triged == 2)
	{
		if (DmaPos >= ADC_BUFFER_SIZE)
		{
			DmaPos = 0;
		}

		/* ����˲�����ֹ������ƽ�������� */
		/* ������ADC DMA�󣬱���ɼ�����10���������ſ�ʼ��������Ч�ж� */
		if (pAdcBuf[20] != 0 || pAdcBuf[21] != 0)
		//if (g_tDSO.DmaTransCplt > 2)
		{
			uint32_t adc_sum = 0;
			uint32_t avg_last, avg_now;
			uint8_t i;
			int32_t pos;
			int32_t now;
			uint32_t rem_time;
			
			now = DmaPos;			
			if (--now < 0)
			{
				now = ADC_BUFFER_SIZE - 1;
			}
			pos = now;
			
			adc_sum = 0;
			for (i = 0; i < 5; i++)
			{
				if (--pos < 0)
				{
					pos = ADC_BUFFER_SIZE - 1;
				}					
				adc_sum += pAdcBuf[pos];
			}
			avg_now = adc_sum / 5;
			
			adc_sum = 0;
			for (i = 0; i < 10; i++)
			{
				if (--pos < 0)
				{
					pos = ADC_BUFFER_SIZE - 1;
				}					
				adc_sum += pAdcBuf[pos];
			}
			avg_last = adc_sum / 10;
			
			/* �½��ػ������� */
			if ((avg_last > g_tDSO.TrigLevel && avg_now <= g_tDSO.TrigLevel && g_tDSO.TrigEdge == TRIG_EDGE_FALLING) ||
				(avg_last < g_tDSO.TrigLevel && avg_now >= g_tDSO.TrigLevel && g_tDSO.TrigEdge == TRIG_EDGE_RISING))				
			{
				/* Disable the ADC Analog watchdog interrupt */
				__HAL_ADC_DISABLE_IT(hadc, ADC_IT_AWD1);
				
				/* ������� */
				if (g_tDSO.SampleSizeID < BUFF_SIZE_NUM)
				{
					TransBuffSize = TabelBufSize[g_tDSO.SampleSizeID];
				}
				else
				{
					TransBuffSize = 1 * 1024;
				}
				
				/* ����ʣ��ɼ�ʱ�� us��λ */
				rem_time = ((int64_t)(TransBuffSize * (100 - g_tDSO.TrigPos) / 100) * 1000000) / TabelFreq[g_tDSO.FreqID];
				if (rem_time == 0)
				{
					DSO_TrigFinished();				
				}
				else
				{
					bsp_StartHardTimer(1, rem_time, DSO_TrigFinished);	
				}
				/* ��¼����λ�� */
				BeginPos = DmaPos - TransBuffSize * g_tDSO.TrigPos / 100;
				if (BeginPos < 0)
				{
					BeginPos = ADC_BUFFER_SIZE + BeginPos;
				}
		
				g_tDSO.DmaPos = BeginPos;	/* ���津��ǰ��λ�� */				
			}
		}
	}		
}
void ADC_IRQHandler(void)
{
	IRQ_WatchDog();
}

void ADC3_IRQHandler(void)
{
	IRQ_WatchDog();
}

/*
*********************************************************************************************************
*	�� �� ��: DSO_InitHard
*	����˵��: ���ÿ�����ͨ����Ϻ������GPIO, ����ADC
*	��    ��: ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void DSO_InitHard(void)
{
	DSO_ConfigCtrlGPIO();
	
	g_tDSO.DC1 = 0;
	g_tDSO.DC2 = 0;
	g_tDSO.Gain1 = 0;
	g_tDSO.Gain2 = 0;
	g_tDSO.FreqID = 0;
	g_tDSO.SampleSizeID = 0;
	g_tDSO.TrigChan = 0;
	g_tDSO.TrigLevel = 32767;
	g_tDSO.TrigPos = 50;
	g_tDSO.TrigMode = 0;
	g_tDSO.Run = 0;
	
	g_tDSO.MeasuteMode = 0;
	
	g_tDSO.DmaPos = 0;
	
	DSO_SetGain(1, g_tDSO.Gain1);
	DSO_SetGain(2, g_tDSO.Gain2);
}

/*
*********************************************************************************************************
*	�� �� ��: DSO_ConfigCtrlGPIO
*	����˵��: ���ÿ�����ͨ����Ϻ������GPIO
*	��    ��: ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void DSO_ConfigCtrlGPIO(void)
{
	/* ���ÿ����������ϵ�GPIO */
	GPIO_InitTypeDef gpio_init;

	/* ��GPIOʱ�� */
	GC_CLK_ENABLE();
	
	gpio_init.Mode = GPIO_MODE_OUTPUT_PP;		/* ����������� */
	gpio_init.Pull = GPIO_NOPULL;				/* ���������費ʹ�� */
	gpio_init.Speed = GPIO_SPEED_FREQ_HIGH;  	/* GPIO�ٶȵȼ� */	
	

	gpio_init.Pin = GC_PIN;	
	HAL_GPIO_Init(GC_GPIO, &gpio_init);	

	DSO_SetDC(1, 0);	/* CH1ѡ��AC��� */
	DSO_SetDC(2, 0);	/* CH1ѡ��AC��� */
	DSO_SetGain(1, 0);	/* CH1ѡ��С���� ˥��1/5, ��2������1��ʾ��˥��1;1 */
	DSO_SetGain(2, 0);	/* CH2ѡ��С���� ˥��1/5, ��2������1��ʾ��˥��1;1 */

	DSO_SetCurrGain(1);	/* ���������������Ϊ 2A */
}

/*
*********************************************************************************************************
*	�� �� ��: DSO_SetDC
*	����˵��: ����AC DC���ģʽ
*	��    ��: _ch : ͨ��1��2
*			  _mode : 0��1.  1��ʾDC��� 0��ʾAC���
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void DSO_SetDC(uint8_t _ch, uint8_t _mode)
{	
	if (_ch == 1)
	{
		if (_mode == 1)
		{
			AC1_1();
		}
		else
		{
			AC1_0();
		}
	}
	else
	{
		if (_mode == 1)
		{
			AC2_1();
		}
		else
		{
			AC2_0();
		}
	}	
	HC595_LockData();
}

/*
*********************************************************************************************************
*	�� �� ��: DSO_SetGain
*	����˵��: ��������ģʽ
*	��    ��: _ch : ͨ��1��2
*			  _gain : 0-7.  7���
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void DSO_SetGain(uint8_t _ch, uint8_t _gain)
{	
	if (_ch == 1)
	{
		if (_gain == 0)      {G1C_0(); G1B_0(); G1A_0();}
		else if (_gain == 1) {G1C_0(); G1B_0(); G1A_1();}
		else if (_gain == 2) {G1C_0(); G1B_1();	G1A_0();}
		else if (_gain == 3) {G1C_0(); G1B_1(); G1A_1();}
		else if (_gain == 4) {G1C_1(); G1B_0(); G1A_0();}
		else if (_gain == 5) {G1C_1(); G1B_0();	G1A_1();}
		else if (_gain == 6) {G1C_1(); G1B_1(); G1A_0();}
		else if (_gain == 7) {G1C_1(); G1B_1(); G1A_1();}
	}
	else if (_ch == 2)
	{
		if (_gain == 0)      {G2C_0(); G2B_0(); G2A_0();}
		else if (_gain == 1) {G2C_0(); G2B_0(); G2A_1();}
		else if (_gain == 2) {G2C_0(); G2B_1();	G2A_0();}
		else if (_gain == 3) {G2C_0(); G2B_1(); G2A_1();}
		else if (_gain == 4) {G2C_1(); G2B_0(); G2A_0();}
		else if (_gain == 5) {G2C_1(); G2B_0();	G2A_1();}
		else if (_gain == 6) {G2C_1(); G2B_1(); G2A_0();}
		else if (_gain == 7) {G2C_1(); G2B_1(); G2A_1();}	
	}
	HC595_LockData();
}


/*
*********************************************************************************************************
*	�� �� ��: DSO_SetOffset
*	����˵��: ����ֱ��ƫֵ
*	��    ��: _ch : ͨ��1��2
*			  _OffsetVolt : ֱ��ƫֵ��ѹ -1250 , +1250
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void DSO_SetOffset(uint8_t _ch, int16_t _OffsetVolt)
{	
#if	1
	;	/* Ӳ����֧��ֱ��ƫֵ���� */
#else	
	if (_ch == 1)	/* ͨ��1 */
	{
		//MCP4725_SetVolt(0, _OffsetVolt + 1250);
		STM8_WritePWM(1, _OffsetVolt + 1250);
	}
	else	/* ͨ��2 */
	{
		//MCP4725_SetVolt(1, _OffsetVolt + 1250);
		STM8_WritePWM(2, _OffsetVolt + 1250);
	}	
#endif	
}

/*
*********************************************************************************************************
*	�� �� ��: DSO_SetCurrGain
*	����˵��: ���Ƶ�������
*	��    ��: _gain : 0��ʾ200mA, 1��ʾ2A
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void DSO_SetCurrGain(uint8_t _gain)
{	
	if (_gain == 0)
	{
		GC_1();	
	}
	else
	{
		GC_0();
	}
}

/*
*********************************************************************************************************
*	�� �� ��: DSO_SetTriger
*	����˵��: ����Ӳ������������ADC���Ź�ʵ��. 
*	��    ��: g_tDSO.TrigMode   ����ģʽ
*			  g_tDSO.TrigLevel  ������ƽ��0-65535��
*			  g_tDSO.TrigPos    ����λ�ã�0-100��
*			  g_tDSO.TrigChan   ����ͨ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void DSO_SetTriger(void)
{	
	ADC_AnalogWDGConfTypeDef WdgCfg;	

	/* ADC ͨ��1ģ�⿴�Ź����� */
	WdgCfg.WatchdogNumber = ADC_ANALOGWATCHDOG_1;
	WdgCfg.Channel = ADCH1_CHANNEL;			
	WdgCfg.WatchdogMode = ADC_ANALOGWATCHDOG_ALL_REG;	// ADC_ANALOGWATCHDOG_ALL_REG;
	if (g_tDSO.TrigChan == 0)
	{
		WdgCfg.ITMode = ENABLE;		
	}
	else
	{
		WdgCfg.ITMode = DISABLE;	
	}
	
	if (g_tDSO.TrigMode == TRIG_MODE_NORMAL || g_tDSO.TrigMode == TRIG_MODE_SINGLE)
	{
		if (g_tDSO.TrigEdge == TRIG_EDGE_RISING)
		{
			WdgCfg.HighThreshold = g_tDSO.TrigLevel;
			WdgCfg.LowThreshold = 0;
		}
		else if (g_tDSO.TrigEdge == TRIG_EDGE_FALLING)
		{
			WdgCfg.HighThreshold = 65535;
			WdgCfg.LowThreshold = g_tDSO.TrigLevel;
		}
	}
	else	/* �Զ��������رտ��Ź� */
	{
		WdgCfg.HighThreshold = 65535;
		WdgCfg.LowThreshold = 0;
	}

	HAL_ADC_AnalogWDGConfig(&AdcHandle1, &WdgCfg);	

	/* ADC ͨ��2ģ�⿴�Ź����� */
	WdgCfg.WatchdogNumber = ADC_ANALOGWATCHDOG_1;
	WdgCfg.Channel = ADCH2_CHANNEL;			
	WdgCfg.WatchdogMode = ADC_ANALOGWATCHDOG_ALL_REG;	// ADC_ANALOGWATCHDOG_ALL_REG;
	if (g_tDSO.TrigChan == 1)
	{
		WdgCfg.ITMode = ENABLE;		
	}
	else
	{
		WdgCfg.ITMode = DISABLE;	
	}
	if (g_tDSO.TrigMode == TRIG_MODE_NORMAL || g_tDSO.TrigMode == TRIG_MODE_SINGLE)
	{
		if (g_tDSO.TrigEdge == TRIG_EDGE_RISING)
		{
			WdgCfg.HighThreshold = g_tDSO.TrigLevel;
			WdgCfg.LowThreshold = 0;
		}
		else if (g_tDSO.TrigEdge == TRIG_EDGE_FALLING)
		{
			WdgCfg.HighThreshold = 65535;
			WdgCfg.LowThreshold = g_tDSO.TrigLevel;
		}
	}
	else	/* �Զ��������رտ��Ź� */
	{
		WdgCfg.HighThreshold = 65535;
		WdgCfg.LowThreshold = 0;
	}	
	HAL_ADC_AnalogWDGConfig(&AdcHandle2, &WdgCfg);	

	/* NVIC configuration for ADC interrupt */
	/* Priority: high-priority */
	HAL_NVIC_SetPriority(ADC_IRQn, 0, 0);
	HAL_NVIC_EnableIRQ(ADC_IRQn);	
	
	HAL_NVIC_SetPriority(ADC3_IRQn, 0, 0);
	HAL_NVIC_EnableIRQ(ADC3_IRQn);	
}

/*
*********************************************************************************************************
*	�� �� ��: DSO_CloseTriger
*	����˵��: �ر�Ӳ���������ر�ADC���Ź�ʵ��. 
*	��    ��: g_tDSO.TrigMode   ����ģʽ
*			  g_tDSO.TrigLevel  ������ƽ��0-65535��
*			  g_tDSO.TrigPos    ����λ�ã�0-100��
*			  g_tDSO.TrigChan   ����ͨ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void DSO_CloseTriger(void)
{	
	ADC_AnalogWDGConfTypeDef WdgCfg;	

	/* ADC ͨ��1ģ�⿴�Ź����� */
	WdgCfg.WatchdogNumber = ADC_ANALOGWATCHDOG_1;
	WdgCfg.Channel = ADCH1_CHANNEL;			
	WdgCfg.WatchdogMode = ADC_ANALOGWATCHDOG_ALL_REG;	// ADC_ANALOGWATCHDOG_ALL_REG;
	WdgCfg.ITMode = DISABLE;	
	WdgCfg.HighThreshold = 65535;
	WdgCfg.LowThreshold = 0;
	HAL_ADC_AnalogWDGConfig(&AdcHandle1, &WdgCfg);	

	/* ADC ͨ��2ģ�⿴�Ź����� */
	WdgCfg.WatchdogNumber = ADC_ANALOGWATCHDOG_1;
	WdgCfg.Channel = ADCH2_CHANNEL;			
	WdgCfg.WatchdogMode = ADC_ANALOGWATCHDOG_ALL_REG;	// ADC_ANALOGWATCHDOG_ALL_REG;
	WdgCfg.ITMode = DISABLE;
	WdgCfg.HighThreshold = 65535;
	WdgCfg.LowThreshold = 0;
	HAL_ADC_AnalogWDGConfig(&AdcHandle2, &WdgCfg);	

	/* NVIC configuration for ADC interrupt */
	/* Priority: high-priority */
	HAL_NVIC_SetPriority(ADC_IRQn, 0, 0);
	HAL_NVIC_DisableIRQ(ADC_IRQn);	
	
	HAL_NVIC_SetPriority(ADC3_IRQn, 0, 0);
	HAL_NVIC_DisableIRQ(ADC3_IRQn);	
}


/*
*********************************************************************************************************
*	�� �� ��: bsp_StartAdcCH1
*	����˵��: ����CH1ͨ����GPIO, ADC, DMA
*	��    ��: ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void bsp_StartAdcCH1(void)
{
	if (g_tDSO.MeasuteMode	== 0)
	{
		/* ### - 1 - Initialize ADC peripheral #################################### */
		AdcHandle1.Instance          = ADCH1;
		if (HAL_ADC_DeInit(&AdcHandle1) != HAL_OK)
		{
			/* ADC de-initialization Error */
			Error_Handler(__FILE__, __LINE__);
		}

		AdcHandle1.Init.ClockPrescaler           = ADC_CLOCK_SYNC_PCLK_DIV4;        /* Synchronous clock mode, input ADC clock divided by 4*/
		AdcHandle1.Init.Resolution               = ADC_RESOLUTION_16B;              /* 16-bit resolution for converted data */
		AdcHandle1.Init.ScanConvMode             = DISABLE;                         /* Sequencer disabled (ADC conversion on only 1 channel: channel set on rank 1) */
		AdcHandle1.Init.EOCSelection             = ADC_EOC_SINGLE_CONV;             /* EOC flag picked-up to indicate conversion end */
		AdcHandle1.Init.LowPowerAutoWait         = DISABLE;                         /* Auto-delayed conversion feature disabled */
		AdcHandle1.Init.ContinuousConvMode       = DISABLE;                          /* Continuous mode enabled (automatic conversion restart after each conversion) */
		AdcHandle1.Init.NbrOfConversion          = 1;                               /* Parameter discarded because sequencer is disabled */
		AdcHandle1.Init.DiscontinuousConvMode    = DISABLE;                         /* Parameter discarded because sequencer is disabled */
		AdcHandle1.Init.NbrOfDiscConversion      = 1;                               /* Parameter discarded because sequencer is disabled */
		AdcHandle1.Init.ExternalTrigConv         = ADC_EXTERNALTRIG_T3_TRGO;              /* Software start to trig the 1st conversion manually, without external event */
		AdcHandle1.Init.ExternalTrigConvEdge     = ADC_EXTERNALTRIGCONVEDGE_RISING;   /* Parameter discarded because software trigger chosen */
		AdcHandle1.Init.ConversionDataManagement = ADC_CONVERSIONDATA_DMA_CIRCULAR; /* ADC DMA circular requested */
		AdcHandle1.Init.Overrun                  = ADC_OVR_DATA_OVERWRITTEN;        /* DR register is overwritten with the last conversion result in case of overrun */
		AdcHandle1.Init.OversamplingMode         = DISABLE;                         /* No oversampling */
		AdcHandle1.Init.BoostMode                = ENABLE;                          /* Enable Boost mode as ADC clock frequency is bigger than 20 MHz */
		AdcHandle1.Init.LeftBitShift             = ADC_LEFTBITSHIFT_NONE;         /* Left shift of final results */
		/* Initialize ADC peripheral according to the passed parameters */
		if (HAL_ADC_Init(&AdcHandle1) != HAL_OK)
		{
			Error_Handler(__FILE__, __LINE__);
		}


		/* ### - 2 - Start calibration ############################################ */
	#if ENABLE_DIFFERENTIAL_ENDED == 0	/* ����ģʽ */	
		if (HAL_ADCEx_Calibration_Start(&AdcHandle1, ADC_CALIB_OFFSET, ADC_SINGLE_ENDED) != HAL_OK)
	#else	/* ���ģʽ */
		HAL_ADC_Stop(&AdcHandle1);
		if (HAL_ADCEx_Calibration_Start(&AdcHandle1, ADC_CALIB_OFFSET, ADC_DIFFERENTIAL_ENDED) != HAL_OK)		
	#endif		
		{	
			Error_Handler(__FILE__, __LINE__);
		}


		/* ### - 3 - Channel configuration ######################################## */
		sConfig1.Channel      = ADCH1_CHANNEL;                /* Sampled channel number */
		sConfig1.Rank         = ADC_REGULAR_RANK_1;          /* Rank of sampled channel number ADCH1_CHANNEL */
		sConfig1.SamplingTime = H7_ADC_SAMPLETIME_1CYCLE_5;   /* Sampling time (number of clock cycles unit) */
	#if ENABLE_DIFFERENTIAL_ENDED == 0	/* ����ģʽ */			
		sConfig1.SingleDiff   = ADC_SINGLE_ENDED;            /* Single-ended input channel */
	#else	/* ���ģʽ */
		sConfig1.SingleDiff   = ADC_DIFFERENTIAL_ENDED;
	#endif
		sConfig1.OffsetNumber = ADC_OFFSET_NONE;             /* No offset subtraction */ 
		sConfig1.Offset = 0;                                 /* Parameter discarded because offset correction is disabled */
		sConfig1.OffsetRightShift       = DISABLE;                    /* No Right Offset Shift */
		sConfig1.OffsetSignedSaturation = DISABLE;                    /* No Signed Saturation */

		if (HAL_ADC_ConfigChannel(&AdcHandle1, &sConfig1) != HAL_OK)
		{
			Error_Handler(__FILE__, __LINE__);
		}
		
		
		DSO_SetTriger();	/* ���ô������򿪿��Ź� */

		/* ### - 4 - Start conversion in DMA mode ################################# */
		if (HAL_ADC_Start_DMA(&AdcHandle1,
							(uint32_t *)aADCH1ConvertedData,
							ADC_BUFFER_SIZE
						   ) != HAL_OK)
		{
			Error_Handler(__FILE__, __LINE__);
		}
	}
	else if (g_tDSO.MeasuteMode == 1)	/* �������ģʽ */
	{
		/* ### - 1 - Initialize ADC peripheral #################################### */
		AdcHandle1.Instance          = CT_ADCH1;
		if (HAL_ADC_DeInit(&AdcHandle1) != HAL_OK)
		{
			/* ADC de-initialization Error */
			Error_Handler(__FILE__, __LINE__);
		}

		AdcHandle1.Init.ClockPrescaler           = ADC_CLOCK_SYNC_PCLK_DIV4;        /* Synchronous clock mode, input ADC clock divided by 4*/
		AdcHandle1.Init.Resolution               = ADC_RESOLUTION_16B;              /* 16-bit resolution for converted data */
		AdcHandle1.Init.ScanConvMode             = DISABLE;                         /* Sequencer disabled (ADC conversion on only 1 channel: channel set on rank 1) */
		AdcHandle1.Init.EOCSelection             = ADC_EOC_SINGLE_CONV;             /* EOC flag picked-up to indicate conversion end */
		AdcHandle1.Init.LowPowerAutoWait         = DISABLE;                         /* Auto-delayed conversion feature disabled */
		AdcHandle1.Init.ContinuousConvMode       = DISABLE;                          /* Continuous mode enabled (automatic conversion restart after each conversion) */
		AdcHandle1.Init.NbrOfConversion          = 1;                               /* Parameter discarded because sequencer is disabled */
		AdcHandle1.Init.DiscontinuousConvMode    = DISABLE;                         /* Parameter discarded because sequencer is disabled */
		AdcHandle1.Init.NbrOfDiscConversion      = 1;                               /* Parameter discarded because sequencer is disabled */
		AdcHandle1.Init.ExternalTrigConv         = ADC_EXTERNALTRIG_T3_TRGO;              /* Software start to trig the 1st conversion manually, without external event */
		AdcHandle1.Init.ExternalTrigConvEdge     = ADC_EXTERNALTRIGCONVEDGE_RISING;   /* Parameter discarded because software trigger chosen */
		AdcHandle1.Init.ConversionDataManagement = ADC_CONVERSIONDATA_DMA_CIRCULAR; /* ADC DMA circular requested */
		AdcHandle1.Init.Overrun                  = ADC_OVR_DATA_OVERWRITTEN;        /* DR register is overwritten with the last conversion result in case of overrun */
		AdcHandle1.Init.OversamplingMode         = DISABLE;                         /* No oversampling */
		AdcHandle1.Init.BoostMode                = ENABLE;                          /* Enable Boost mode as ADC clock frequency is bigger than 20 MHz */
		/* Initialize ADC peripheral according to the passed parameters */
		if (HAL_ADC_Init(&AdcHandle1) != HAL_OK)
		{
			Error_Handler(__FILE__, __LINE__);
		}


		/* ### - 2 - Start calibration ############################################ */
		if (HAL_ADCEx_Calibration_Start(&AdcHandle1, ADC_CALIB_OFFSET, ADC_SINGLE_ENDED) != HAL_OK)
		{
			Error_Handler(__FILE__, __LINE__);
		}

		/* ### - 3 - Channel configuration ######################################## */
		sConfig1.Channel      = CT_ADCH1_CHANNEL;                /* Sampled channel number */
		sConfig1.Rank         = ADC_REGULAR_RANK_1;          /* Rank of sampled channel number ADCH1_CHANNEL */
		sConfig1.SamplingTime = H7_ADC_SAMPLETIME_1CYCLE_5;   /* Sampling time (number of clock cycles unit) */
		sConfig1.SingleDiff   = ADC_SINGLE_ENDED;            /* Single-ended input channel */
		sConfig1.OffsetNumber = ADC_OFFSET_NONE;             /* No offset subtraction */ 
		sConfig1.Offset = 0;                                 /* Parameter discarded because offset correction is disabled */
		if (HAL_ADC_ConfigChannel(&AdcHandle1, &sConfig1) != HAL_OK)
		{
			Error_Handler(__FILE__, __LINE__);
		}
		
		DSO_SetTriger();	/* ���ô������򿪿��Ź� */

		/* ### - 4 - Start conversion in DMA mode ################################# */
		if (HAL_ADC_Start_DMA(&AdcHandle1,
							(uint32_t *)aADCH1ConvertedData,
							ADC_BUFFER_SIZE
						   ) != HAL_OK)
		{
			Error_Handler(__FILE__, __LINE__);
		}
	}
	else if (g_tDSO.MeasuteMode == 2)	/* ���ٶ�ͨ��ɨ��ģʽ */
	{
		bsp_StopAdcCH1();
		DSO_CloseTriger();	/* �ر�ADC���Ź��ж� */		
		
		/* ### - 1 - Initialize ADC peripheral #################################### */
		AdcHandle1.Instance          = ADC1;
		if (HAL_ADC_DeInit(&AdcHandle1) != HAL_OK)
		{
			/* ADC de-initialization Error */
			Error_Handler(__FILE__, __LINE__);
		}

		/*
			PF11/ADC1_INP2	---- CH1��ѹ
			PA6/ADC1_INP3	---- �߶˵���
		
			PC3_C/ADC3_INP1	---- CH2��ѹ
			PF5/ADC3_INP4	---- TVCC����������
			PF3/ADC3_INP5	---- NTC����������ֵ���
			PF8/ADC3_INP7	---- ���ص�ѹ
			PH2/ADC3_INP13	---- TVCC��ѹ���		
			PH3/ADC3_INP14	---- 12V�����ѹ���
			PH5/ADC3_INP16	---- USB�����ѹ���			
		*/		
		AdcHandle1.Init.ClockPrescaler           = ADC_CLOCK_SYNC_PCLK_DIV4;        /* Synchronous clock mode, input ADC clock divided by 4*/
		AdcHandle1.Init.Resolution               = ADC_RESOLUTION_16B;              /* 16-bit resolution for converted data */
		AdcHandle1.Init.ScanConvMode             = ADC_SCAN_ENABLE;                          /* Sequencer disabled (ADC conversion on only 1 channel: channel set on rank 1) */
		AdcHandle1.Init.EOCSelection             = ADC_EOC_SINGLE_CONV;             /* EOC flag picked-up to indicate conversion end */
		AdcHandle1.Init.LowPowerAutoWait         = DISABLE;                         /* Auto-delayed conversion feature disabled */
		AdcHandle1.Init.ContinuousConvMode       = ENABLE;                          /* Continuous mode enabled (automatic conversion restart after each conversion) */
		AdcHandle1.Init.NbrOfConversion          = 2;                               /* Parameter discarded because sequencer is disabled */
		AdcHandle1.Init.DiscontinuousConvMode    = DISABLE;                         /* Parameter discarded because sequencer is disabled */
		AdcHandle1.Init.NbrOfDiscConversion      = 1;                               /* Parameter discarded because sequencer is disabled */
		AdcHandle1.Init.ExternalTrigConv         = ADC_SOFTWARE_START;              /* Software start to trig the 1st conversion manually, without external event */
		AdcHandle1.Init.ExternalTrigConvEdge     = ADC_EXTERNALTRIGCONVEDGE_RISING;   /* Parameter discarded because software trigger chosen */
		AdcHandle1.Init.ConversionDataManagement = ADC_CONVERSIONDATA_DMA_CIRCULAR; /* ADC DMA circular requested */
		AdcHandle1.Init.Overrun                  = ADC_OVR_DATA_OVERWRITTEN;        /* DR register is overwritten with the last conversion result in case of overrun */
		AdcHandle1.Init.BoostMode                = ENABLE;                          /* Enable Boost mode as ADC clock frequency is bigger than 20 MHz */

		AdcHandle1.Init.OversamplingMode         = ENABLE;                         
		AdcHandle1.Init.Oversampling.Ratio                 = 1023;    /* 1024-oversampling */       
		AdcHandle1.Init.Oversampling.RightBitShift         = ADC_RIGHTBITSHIFT_10;         /* 6-bit right shift of the oversampled summation */    
		AdcHandle1.Init.Oversampling.TriggeredMode         = ADC_TRIGGEREDMODE_SINGLE_TRIGGER;        /* A single trigger for all channel oversampled conversions */
		AdcHandle1.Init.Oversampling.OversamplingStopReset = ADC_REGOVERSAMPLING_CONTINUED_MODE;  /* Oversampling buffer maintained during injection sequence */ 

		/* Initialize ADC peripheral according to the passed parameters */
		if (HAL_ADC_Init(&AdcHandle1) != HAL_OK)
		{
			Error_Handler(__FILE__, __LINE__);
		}


		/* ### - 2 - Start calibration ############################################ */
		if (HAL_ADCEx_Calibration_Start(&AdcHandle1, ADC_CALIB_OFFSET, ADC_SINGLE_ENDED) != HAL_OK)
		{
			Error_Handler(__FILE__, __LINE__);
		}

		/* ### - 3 - Channel configuration ######################################## */
		sConfig1.Channel      = ADC_CHANNEL_2;                /* Sampled channel number */
		sConfig1.Rank         = ADC_REGULAR_RANK_1;          /* Rank of sampled channel number ADCH1_CHANNEL */
		sConfig1.SamplingTime = SCAN_MODE_SAMPLETIME_ADC1;   /* Sampling time (number of clock cycles unit) */
		sConfig1.SingleDiff   = ADC_SINGLE_ENDED;            /* Single-ended input channel */
		sConfig1.OffsetNumber = ADC_OFFSET_NONE;             /* No offset subtraction */ 
		sConfig1.Offset = 0;                                 /* Parameter discarded because offset correction is disabled */
		sConfig1.OffsetRightShift       = DISABLE;           /* ��ֹ���� */
		sConfig1.OffsetSignedSaturation = DISABLE;           /* ��ֹ�з��ű��� */		
		if (HAL_ADC_ConfigChannel(&AdcHandle1, &sConfig1) != HAL_OK)
		{
			Error_Handler(__FILE__, __LINE__);
		}

		sConfig1.Channel      = ADC_CHANNEL_3;              /* Sampled channel number */
		sConfig1.Rank         = ADC_REGULAR_RANK_2;          /* Rank of sampled channel number ADCH1_CHANNEL */
		if (HAL_ADC_ConfigChannel(&AdcHandle1, &sConfig1) != HAL_OK)
		{
			Error_Handler(__FILE__, __LINE__);
		}
		
		/* ### - 4 - Start conversion in DMA mode ################################# */
		if (HAL_ADC_Start_DMA(&AdcHandle1,
							(uint32_t *)aADCH1ConvertedData,
							SCAN_MODE_SAMPLE_SIZE * SCAN_MODE_ADC1_NUM
						   ) != HAL_OK)
		{
			Error_Handler(__FILE__, __LINE__);
		}
	}	
}

/*
*********************************************************************************************************
*	�� �� ��: bsp_StartAdcCH2
*	����˵��: ����CH2ͨ����GPIO, ADC, DMA
*	��    ��: ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void bsp_StartAdcCH2(void)
{
	if (g_tDSO.MeasuteMode	== 0)
	{	
		/* ### - 1 - Initialize ADC peripheral #################################### */
		AdcHandle2.Instance          = ADCH2;
		if (HAL_ADC_DeInit(&AdcHandle2) != HAL_OK)
		{
			/* ADC de-initialization Error */
			Error_Handler(__FILE__, __LINE__);
		}

		AdcHandle2.Init.ClockPrescaler           = ADC_CLOCK_SYNC_PCLK_DIV4;        /* Synchronous clock mode, input ADC clock divided by 4*/
		AdcHandle2.Init.Resolution               = ADC_RESOLUTION_16B;              /* 16-bit resolution for converted data */
		AdcHandle2.Init.ScanConvMode             = DISABLE;                         /* Sequencer disabled (ADC conversion on only 1 channel: channel set on rank 1) */
		AdcHandle2.Init.EOCSelection             = ADC_EOC_SINGLE_CONV;             /* EOC flag picked-up to indicate conversion end */
		AdcHandle2.Init.LowPowerAutoWait         = DISABLE;                         /* Auto-delayed conversion feature disabled */
		AdcHandle2.Init.ContinuousConvMode       = DISABLE;                          /* Continuous mode enabled (automatic conversion restart after each conversion) */
		AdcHandle2.Init.NbrOfConversion          = 1;                               /* Parameter discarded because sequencer is disabled */
		AdcHandle2.Init.DiscontinuousConvMode    = DISABLE;                         /* Parameter discarded because sequencer is disabled */
		AdcHandle2.Init.NbrOfDiscConversion      = 1;                               /* Parameter discarded because sequencer is disabled */
		AdcHandle2.Init.ExternalTrigConv         = ADC_EXTERNALTRIG_T3_TRGO;              /* Software start to trig the 1st conversion manually, without external event */
		AdcHandle2.Init.ExternalTrigConvEdge     = ADC_EXTERNALTRIGCONVEDGE_RISING;   /* Parameter discarded because software trigger chosen */
		AdcHandle2.Init.ConversionDataManagement = ADC_CONVERSIONDATA_DMA_CIRCULAR; /* ADC DMA circular requested */
		AdcHandle2.Init.Overrun                  = ADC_OVR_DATA_OVERWRITTEN;        /* DR register is overwritten with the last conversion result in case of overrun */
		AdcHandle2.Init.OversamplingMode         = DISABLE;                         /* No oversampling */
		AdcHandle2.Init.BoostMode                = ENABLE;                          /* Enable Boost mode as ADC clock frequency is bigger than 20 MHz */
		AdcHandle1.Init.LeftBitShift             = ADC_LEFTBITSHIFT_NONE;         /* Left shift of final results */
		/* Initialize ADC peripheral according to the passed parameters */
		if (HAL_ADC_Init(&AdcHandle2) != HAL_OK)
		{
			Error_Handler(__FILE__, __LINE__);
		}


		/* ### - 2 - Start calibration ############################################ */
	#if ENABLE_DIFFERENTIAL_ENDED == 0	/* ����ģʽ */			
		if (HAL_ADCEx_Calibration_Start(&AdcHandle2, ADC_CALIB_OFFSET, ADC_SINGLE_ENDED) != HAL_OK)
	#else	/* ���ģʽ */
		HAL_ADC_Stop(&AdcHandle2);
		if (HAL_ADCEx_Calibration_Start(&AdcHandle2, ADC_CALIB_OFFSET, ADC_DIFFERENTIAL_ENDED) != HAL_OK)
	#endif
		{
			Error_Handler(__FILE__, __LINE__);
		}

		/* ### - 3 - Channel configuration ######################################## */
		sConfig2.Channel      = ADCH2_CHANNEL;                /* Sampled channel number */
		sConfig2.Rank         = ADC_REGULAR_RANK_1;          /* Rank of sampled channel number ADCH1_CHANNEL */
		sConfig2.SamplingTime = H7_ADC_SAMPLETIME_1CYCLE_5  ;   /* Sampling time (number of clock cycles unit) */
	#if ENABLE_DIFFERENTIAL_ENDED == 0	/* ����ģʽ */			
		sConfig2.SingleDiff   = ADC_SINGLE_ENDED;            /* Single-ended input channel */
	#else	/* ���ģʽ */
		sConfig2.SingleDiff   = ADC_DIFFERENTIAL_ENDED;
	#endif		
		sConfig2.OffsetNumber = ADC_OFFSET_NONE;             /* No offset subtraction */ 
		sConfig2.Offset = 0;                                 /* Parameter discarded because offset correction is disabled */
		sConfig2.OffsetRightShift       = DISABLE;                    /* No Right Offset Shift */
		sConfig2.OffsetSignedSaturation = DISABLE;                    /* No Signed Saturation */		
		if (HAL_ADC_ConfigChannel(&AdcHandle2, &sConfig2) != HAL_OK)
		{
			Error_Handler(__FILE__, __LINE__);
		}
		
		DSO_SetTriger();	/* ���ô������򿪿��Ź� */

		/* ### - 4 - Start conversion in DMA mode ################################# */
		if (HAL_ADC_Start_DMA(&AdcHandle2,
							(uint32_t *)aADCH2ConvertedData,
							ADC_BUFFER_SIZE
						   ) != HAL_OK)
		{
			Error_Handler(__FILE__, __LINE__);
		}
	}
	else if (g_tDSO.MeasuteMode == 1)	/* �������ģʽ */
	{
		/* ### - 1 - Initialize ADC peripheral #################################### */
		AdcHandle2.Instance          = CT_ADCH2;
		if (HAL_ADC_DeInit(&AdcHandle2) != HAL_OK)
		{
			/* ADC de-initialization Error */
			Error_Handler(__FILE__, __LINE__);
		}

		AdcHandle2.Init.ClockPrescaler           = ADC_CLOCK_SYNC_PCLK_DIV4;        /* Synchronous clock mode, input ADC clock divided by 4*/
		AdcHandle2.Init.Resolution               = ADC_RESOLUTION_16B;              /* 16-bit resolution for converted data */
		AdcHandle2.Init.ScanConvMode             = DISABLE;                         /* Sequencer disabled (ADC conversion on only 1 channel: channel set on rank 1) */
		AdcHandle2.Init.EOCSelection             = ADC_EOC_SINGLE_CONV;             /* EOC flag picked-up to indicate conversion end */
		AdcHandle2.Init.LowPowerAutoWait         = DISABLE;                         /* Auto-delayed conversion feature disabled */
		AdcHandle2.Init.ContinuousConvMode       = DISABLE;                          /* Continuous mode enabled (automatic conversion restart after each conversion) */
		AdcHandle2.Init.NbrOfConversion          = 1;                               /* Parameter discarded because sequencer is disabled */
		AdcHandle2.Init.DiscontinuousConvMode    = DISABLE;                         /* Parameter discarded because sequencer is disabled */
		AdcHandle2.Init.NbrOfDiscConversion      = 1;                               /* Parameter discarded because sequencer is disabled */
		AdcHandle2.Init.ExternalTrigConv         = ADC_EXTERNALTRIG_T3_TRGO;              /* Software start to trig the 1st conversion manually, without external event */
		AdcHandle2.Init.ExternalTrigConvEdge     = ADC_EXTERNALTRIGCONVEDGE_RISING;   /* Parameter discarded because software trigger chosen */
		AdcHandle2.Init.ConversionDataManagement = ADC_CONVERSIONDATA_DMA_CIRCULAR; /* ADC DMA circular requested */
		AdcHandle2.Init.Overrun                  = ADC_OVR_DATA_OVERWRITTEN;        /* DR register is overwritten with the last conversion result in case of overrun */
		AdcHandle2.Init.OversamplingMode         = DISABLE;                         /* No oversampling */
		AdcHandle2.Init.BoostMode                = ENABLE;                          /* Enable Boost mode as ADC clock frequency is bigger than 20 MHz */
		/* Initialize ADC peripheral according to the passed parameters */
		if (HAL_ADC_Init(&AdcHandle2) != HAL_OK)
		{
			Error_Handler(__FILE__, __LINE__);
		}


		/* ### - 2 - Start calibration ############################################ */
		if (HAL_ADCEx_Calibration_Start(&AdcHandle2, ADC_CALIB_OFFSET, ADC_SINGLE_ENDED) != HAL_OK)
		{
			Error_Handler(__FILE__, __LINE__);
		}

		/* ### - 3 - Channel configuration ######################################## */
		sConfig2.Channel      = CT_ADCH2_CHANNEL;                /* Sampled channel number */
		sConfig2.Rank         = ADC_REGULAR_RANK_1;          /* Rank of sampled channel number ADCH1_CHANNEL */
		sConfig2.SamplingTime = H7_ADC_SAMPLETIME_1CYCLE_5  ;   /* Sampling time (number of clock cycles unit) */
		sConfig2.SingleDiff   = ADC_SINGLE_ENDED;            /* Single-ended input channel */
		sConfig2.OffsetNumber = ADC_OFFSET_NONE;             /* No offset subtraction */ 
		sConfig2.Offset = 0;                                 /* Parameter discarded because offset correction is disabled */
		if (HAL_ADC_ConfigChannel(&AdcHandle2, &sConfig2) != HAL_OK)
		{
			Error_Handler(__FILE__, __LINE__);
		}
		
		DSO_SetTriger();	/* ���ô������򿪿��Ź� */

		/* ### - 4 - Start conversion in DMA mode ################################# */
		if (HAL_ADC_Start_DMA(&AdcHandle2,
							(uint32_t *)aADCH2ConvertedData,
							ADC_BUFFER_SIZE
						   ) != HAL_OK)
		{
			Error_Handler(__FILE__, __LINE__);
		}
	}
	else if (g_tDSO.MeasuteMode == 2)	/* ��ͨ��ɨ��ģʽ */
	{
		bsp_StopAdcCH2();
		DSO_CloseTriger();	/* �ر�ADC���Ź��ж� */
		
		/* ### - 1 - Initialize ADC peripheral #################################### */
		AdcHandle2.Instance          = ADC3;
		if (HAL_ADC_DeInit(&AdcHandle2) != HAL_OK)
		{
			/* ADC de-initialization Error */
			Error_Handler(__FILE__, __LINE__);
		}

		/*
			PF11/ADC1_INP2	---- CH1��ѹ
			PA6/ADC1_INP3	---- �߶˵���
		
			PC3_C/ADC3_INP1	---- CH2��ѹ
			PF5/ADC3_INP4	---- TVCC����������
			PF3/ADC3_INP5	---- NTC����������ֵ���
			PF8/ADC3_INP7	---- ���ص�ѹ
			PH2/ADC3_INP13	---- TVCC��ѹ���		
			PH3/ADC3_INP14	---- 12V�����ѹ���
			PH5/ADC3_INP16	---- USB�����ѹ���			
		*/		
		AdcHandle2.Init.ClockPrescaler           = ADC_CLOCK_SYNC_PCLK_DIV4;        /* Synchronous clock mode, input ADC clock divided by 4*/
		AdcHandle2.Init.Resolution               = ADC_RESOLUTION_16B;              /* 16-bit resolution for converted data */
		AdcHandle2.Init.ScanConvMode             = ENABLE;                          /* Sequencer disabled (ADC conversion on only 1 channel: channel set on rank 1) */
		AdcHandle2.Init.EOCSelection             = ADC_EOC_SINGLE_CONV;             /* EOC flag picked-up to indicate conversion end */
		AdcHandle2.Init.LowPowerAutoWait         = DISABLE;                         /* Auto-delayed conversion feature disabled */
		AdcHandle2.Init.ContinuousConvMode       = ENABLE;                          /* Continuous mode enabled (automatic conversion restart after each conversion) */
		AdcHandle2.Init.NbrOfConversion          = 7;                               /* Parameter discarded because sequencer is disabled */
		AdcHandle2.Init.DiscontinuousConvMode    = DISABLE;                         /* Parameter discarded because sequencer is disabled */
		AdcHandle2.Init.NbrOfDiscConversion      = 1;                               /* Parameter discarded because sequencer is disabled */
		AdcHandle2.Init.ExternalTrigConv         = ADC_SOFTWARE_START;              /* Software start to trig the 1st conversion manually, without external event */
		AdcHandle2.Init.ExternalTrigConvEdge     = ADC_EXTERNALTRIGCONVEDGE_RISING;   /* Parameter discarded because software trigger chosen */
		AdcHandle2.Init.ConversionDataManagement = ADC_CONVERSIONDATA_DMA_CIRCULAR; /* ADC DMA circular requested */
		AdcHandle2.Init.Overrun                  = ADC_OVR_DATA_OVERWRITTEN;        /* DR register is overwritten with the last conversion result in case of overrun */
		AdcHandle2.Init.BoostMode                = ENABLE;                          /* Enable Boost mode as ADC clock frequency is bigger than 20 MHz */

		AdcHandle2.Init.OversamplingMode         = ENABLE;                         
		AdcHandle2.Init.Oversampling.Ratio                 = 1023;    /* 1024-oversampling */       
		AdcHandle2.Init.Oversampling.RightBitShift         = ADC_RIGHTBITSHIFT_10;         /* 6-bit right shift of the oversampled summation */    
		AdcHandle2.Init.Oversampling.TriggeredMode         = ADC_TRIGGEREDMODE_SINGLE_TRIGGER;        /* A single trigger for all channel oversampled conversions */
		AdcHandle2.Init.Oversampling.OversamplingStopReset = ADC_REGOVERSAMPLING_CONTINUED_MODE;  /* Oversampling buffer maintained during injection sequence */ 

		/* Initialize ADC peripheral according to the passed parameters */
		if (HAL_ADC_Init(&AdcHandle2) != HAL_OK)
		{
			Error_Handler(__FILE__, __LINE__);
		}


		/* ### - 2 - Start calibration ############################################ */
		if (HAL_ADCEx_Calibration_Start(&AdcHandle2, ADC_CALIB_OFFSET, ADC_SINGLE_ENDED) != HAL_OK)
		{
			Error_Handler(__FILE__, __LINE__);
		}

		/* ### - 3 - Channel configuration ######################################## */
		sConfig2.Channel      = ADC_CHANNEL_1;                /* Sampled channel number */
		sConfig2.Rank         = ADC_REGULAR_RANK_1;          /* Rank of sampled channel number ADCH1_CHANNEL */
		sConfig2.SamplingTime = SCAN_MODE_SAMPLETIME_ADC3;   /* Sampling time (number of clock cycles unit) */
		sConfig2.SingleDiff   = ADC_SINGLE_ENDED;            /* Single-ended input channel */
		sConfig2.OffsetNumber = ADC_OFFSET_NONE;             /* No offset subtraction */ 
		sConfig2.Offset = 0;                                 /* Parameter discarded because offset correction is disabled */
		if (HAL_ADC_ConfigChannel(&AdcHandle2, &sConfig2) != HAL_OK)
		{
			Error_Handler(__FILE__, __LINE__);
		}

		sConfig2.Channel      = ADC_CHANNEL_4;              /* Sampled channel number */
		sConfig2.Rank         = ADC_REGULAR_RANK_2;          /* Rank of sampled channel number ADCH1_CHANNEL */
		if (HAL_ADC_ConfigChannel(&AdcHandle2, &sConfig2) != HAL_OK)
		{
			Error_Handler(__FILE__, __LINE__);
		}

		sConfig2.Channel      = ADC_CHANNEL_5;              /* Sampled channel number */
		sConfig2.Rank         = ADC_REGULAR_RANK_3;          /* Rank of sampled channel number ADCH1_CHANNEL */
		if (HAL_ADC_ConfigChannel(&AdcHandle2, &sConfig2) != HAL_OK)
		{
			Error_Handler(__FILE__, __LINE__);
		}	

		sConfig2.Channel      = ADC_CHANNEL_7;                /* Sampled channel number */
		sConfig2.Rank         = ADC_REGULAR_RANK_4;          /* Rank of sampled channel number ADCH1_CHANNEL */
		if (HAL_ADC_ConfigChannel(&AdcHandle2, &sConfig2) != HAL_OK)
		{
			Error_Handler(__FILE__, __LINE__);
		}

		sConfig2.Channel      = ADC_CHANNEL_13;                /* Sampled channel number */
		sConfig2.Rank         = ADC_REGULAR_RANK_5;          /* Rank of sampled channel number ADCH1_CHANNEL */
		if (HAL_ADC_ConfigChannel(&AdcHandle2, &sConfig2) != HAL_OK)
		{
			Error_Handler(__FILE__, __LINE__);
		}

		sConfig2.Channel      = ADC_CHANNEL_14;                /* Sampled channel number */
		sConfig2.Rank         = ADC_REGULAR_RANK_6;          /* Rank of sampled channel number ADCH1_CHANNEL */
		if (HAL_ADC_ConfigChannel(&AdcHandle2, &sConfig2) != HAL_OK)
		{
			Error_Handler(__FILE__, __LINE__);
		}

		sConfig2.Channel      = ADC_CHANNEL_16;                /* Sampled channel number */
		sConfig2.Rank         = ADC_REGULAR_RANK_7;          /* Rank of sampled channel number ADCH1_CHANNEL */
		if (HAL_ADC_ConfigChannel(&AdcHandle2, &sConfig2) != HAL_OK)
		{
			Error_Handler(__FILE__, __LINE__);
		}		
		
		/* ### - 4 - Start conversion in DMA mode ################################# */
		if (HAL_ADC_Start_DMA(&AdcHandle2,
							(uint32_t *)aADCH2ConvertedData,
							SCAN_MODE_SAMPLE_SIZE * SCAN_MODE_ADC3_NUM
						   ) != HAL_OK)
		{
			Error_Handler(__FILE__, __LINE__);
		}	
	}
}

/*
*********************************************************************************************************
*	�� �� ��: bsp_StopAdcCH1
*	����˵��: ��λCH1ͨ�����ã�ֹͣADC�ɼ�.
*	��    ��: ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void bsp_StopAdcCH1(void)
{
	if (g_tDSO.MeasuteMode	== 0)
	{		
		/* ### - 1 - Initialize ADC peripheral #################################### */
		AdcHandle1.Instance          = ADCH1;
		if (HAL_ADC_DeInit(&AdcHandle1) != HAL_OK)
		{
			/* ADC de-initialization Error */
			Error_Handler(__FILE__, __LINE__);
		}
	}
	else if (g_tDSO.MeasuteMode == 1)
	{
		/* ### - 1 - Initialize ADC peripheral #################################### */
		AdcHandle1.Instance          = CT_ADCH1;
		if (HAL_ADC_DeInit(&AdcHandle1) != HAL_OK)
		{
			/* ADC de-initialization Error */
			Error_Handler(__FILE__, __LINE__);
		}		
	}
	else if (g_tDSO.MeasuteMode == 2)
	{
		/* ### - 1 - Initialize ADC peripheral #################################### */
		AdcHandle1.Instance          = ADC1;
		if (HAL_ADC_DeInit(&AdcHandle1) != HAL_OK)
		{
			/* ADC de-initialization Error */
			Error_Handler(__FILE__, __LINE__);
		}		
	}	
}

/*
*********************************************************************************************************
*	�� �� ��: bsp_StopAdcCH2
*	����˵��: ��λCH2ͨ�����ã�ֹͣADC�ɼ�.
*	��    ��: ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void bsp_StopAdcCH2(void)
{
	if (g_tDSO.MeasuteMode	== 0)
	{	
		/* ### - 1 - Initialize ADC peripheral #################################### */
		AdcHandle2.Instance          = ADCH2;
		if (HAL_ADC_DeInit(&AdcHandle2) != HAL_OK)
		{
			/* ADC de-initialization Error */
			Error_Handler(__FILE__, __LINE__);
		}
	}
	else if (g_tDSO.MeasuteMode == 1)
	{
		/* ### - 1 - Initialize ADC peripheral #################################### */
		AdcHandle2.Instance          = CT_ADCH2;
		if (HAL_ADC_DeInit(&AdcHandle2) != HAL_OK)
		{
			/* ADC de-initialization Error */
			Error_Handler(__FILE__, __LINE__);
		}
	}
	else if (g_tDSO.MeasuteMode == 2)
	{
		/* ### - 1 - Initialize ADC peripheral #################################### */
		AdcHandle2.Instance          = ADC3;
		if (HAL_ADC_DeInit(&AdcHandle2) != HAL_OK)
		{
			/* ADC de-initialization Error */
			Error_Handler(__FILE__, __LINE__);
		}
	}
}

/*
*********************************************************************************************************
*	�� �� ��: TIM3_Config
*	����˵��: ����TIM3��ΪADC����Դ
*	��    ��: _freq : ����Ƶ�ʣ���λHz
*	�� �� ֵ: ��
*********************************************************************************************************
*/
static void TIM3_Config(uint32_t _freq)
{
	static TIM_HandleTypeDef  htim = {0};
	TIM_MasterConfigTypeDef sMasterConfig = {0};

	htim.Instance = TIM3;
	if (_freq == 0)
	{
//		__HAL_RCC_TIM3_CLK_DISABLE();
//		HAL_TIM_Base_Stop(&htim);
		__HAL_TIM_DISABLE(&htim);
	}
	else
	{
		__HAL_RCC_TIM3_CLK_ENABLE();
		
		/*##-1- Configure the TIM peripheral #######################################*/
		/* Time base configuration */

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
}

/*
*********************************************************************************************************
*	�� �� ��: DSO_StartADC
*	����˵��: ��Ҫ���ģ����GPIO�����á�ADC�����á���ʱ���������Լ�DMA�����á�
*			- ����ADC�����ڶ���ģʽ
*			- ������ͬ���ⲿ������ ADC_EXTERNALTRIG_T4_CC4
*			- TIM1��CC1Ƶ�ʵľ����˲���Ƶ��
*
*	��    ��: _uiFreq : ����Ƶ�ʣ� Hz
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void DSO_StartADC( uint32_t _uiFreq)
{				
	TIM3_Config(0);
	DSO_StopADC();
	
	#if 1
	{
		uint32_t i;
		
		for (i = 0; i < ADC_BUFFER_SIZE; i++)
		{
			aADCH1ConvertedData[i] = 0;
			aADCH2ConvertedData[i] = 0;
		}
	}
	#endif
	
	g_tDSO.TrigFlag = 0;
	
	g_tDSO.DmaTransCplt = 0;
	
	bsp_StartAdcCH1();
	bsp_StartAdcCH2();
	
	/* ���ò���������ʱ����ʹ��TIM1 CC1 */
	DSO_SetSampRate(_uiFreq);	/* �޸Ĳ���Ƶ�ʣ�������TIM */
}

/*
*********************************************************************************************************
*	�� �� ��: DSO_StopADC
*	����˵��: �ر�ADC�������е����衣ADC, DMA, TIM
*	��    ��: ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void DSO_StopADC(void)
{	
	bsp_StopAdcCH1();
	bsp_StopAdcCH1();
}

/*
*********************************************************************************************************
*	�� �� ��: PauseADC
*	����˵��: ��ͣADC������׼���������ݡ���֤�´�DMA����������
*	��    ��: ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void DSO_PauseADC(void)
{
	TIM3_Config(0);
}

/**
* @brief  ADC MSP Init
* @param  hadc : ADC handle
* @retval None
*/
void HAL_ADC_MspInit(ADC_HandleTypeDef *hadc)
{
	if (g_tDSO.MeasuteMode	== 0)
	{		
		if (hadc->Instance == ADCH1)
		{	
			GPIO_InitTypeDef          GPIO_InitStruct;
			static DMA_HandleTypeDef  DmaHandle1 = {0};
		
			/*##-1- Enable peripherals and GPIO Clocks #################################*/
			/* Enable GPIO clock ****************************************/
			ADCH1_P_GPIO_CLK_ENABLE();
			ADCH1_N_GPIO_CLK_ENABLE();
			/* ADC Periph clock enable */
			ADCH1_CLK_ENABLE();
			/* ADC Periph interface clock configuration */
			__HAL_RCC_ADC_CONFIG(RCC_ADCCLKSOURCE_CLKP);
			/* Enable DMA clock */
			CH1_DMA_CLK_ENABLE();

			/*##- 2- Configure peripheral GPIO #########################################*/
			/* ADC Channel GPIO pin configuration */
			GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
			GPIO_InitStruct.Pull = GPIO_NOPULL;
			GPIO_InitStruct.Pin = ADCH1_P_PIN;	  
			HAL_GPIO_Init(ADCH1_P_GPIO_PORT, &GPIO_InitStruct);

			GPIO_InitStruct.Pin = ADCH1_N_PIN;	  
			HAL_GPIO_Init(ADCH1_N_GPIO_PORT, &GPIO_InitStruct);	  
			/*##- 3- Configure DMA #####################################################*/ 

			/*********************** Configure DMA parameters ***************************/
			DmaHandle1.Instance                 = CH1_DMA_Stream;
			DmaHandle1.Init.Request             = CH1_DMA_REQUEST_ADC;
			DmaHandle1.Init.Direction           = DMA_PERIPH_TO_MEMORY;
			DmaHandle1.Init.PeriphInc           = DMA_PINC_DISABLE;
			DmaHandle1.Init.MemInc              = DMA_MINC_ENABLE;
			DmaHandle1.Init.PeriphDataAlignment = DMA_PDATAALIGN_HALFWORD;
			DmaHandle1.Init.MemDataAlignment    = DMA_MDATAALIGN_HALFWORD;
//			if (g_tDSO.TrigMode == 0)
//			{
				DmaHandle1.Init.Mode                = DMA_CIRCULAR;	// DMA_CIRCULAR; DMA_NORMAL
//			}
//			else
//			{
//				DmaHandle1.Init.Mode                = DMA_NORMAL;	// DMA_CIRCULAR; DMA_NORMAL
//			}
			DmaHandle1.Init.Priority            = DMA_PRIORITY_MEDIUM;
			/* Deinitialize  & Initialize the DMA for new transfer */
			HAL_DMA_DeInit(&DmaHandle1);
			HAL_DMA_Init(&DmaHandle1);

			/* Associate the DMA handle */
			__HAL_LINKDMA(hadc, DMA_Handle, DmaHandle1);

			/* NVIC configuration for DMA Input data interrupt */
			HAL_NVIC_SetPriority(CH1_DMA_Stream_IRQn, 1, 0);
			HAL_NVIC_EnableIRQ(CH1_DMA_Stream_IRQn);  
		}
		else if (hadc->Instance == ADCH2)
		{
			GPIO_InitTypeDef          GPIO_InitStruct;
			static DMA_HandleTypeDef  DmaHandle2 = {0};

			/*##-1- Enable peripherals and GPIO Clocks #################################*/
			/* Enable GPIO clock ****************************************/
			ADCH2_P_GPIO_CLK_ENABLE();
			ADCH2_N_GPIO_CLK_ENABLE();
			/* ADC Periph clock enable */
			ADCH2_CLK_ENABLE();
			/* ADC Periph interface clock configuration */
			__HAL_RCC_ADC_CONFIG(RCC_ADCCLKSOURCE_CLKP);
			/* Enable DMA clock */
			CH2_DMA_CLK_ENABLE();

			/*##- 2- Configure peripheral GPIO #########################################*/
			/* ADC Channel GPIO pin configuration */

			GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
			GPIO_InitStruct.Pull = GPIO_NOPULL;
			GPIO_InitStruct.Pin = ADCH2_P_PIN;	  
			HAL_GPIO_Init(ADCH2_P_GPIO_PORT, &GPIO_InitStruct);

			GPIO_InitStruct.Pin = ADCH2_N_PIN;	  
			HAL_GPIO_Init(ADCH2_N_GPIO_PORT, &GPIO_InitStruct);	  
			/*##- 3- Configure DMA #####################################################*/ 

			/*********************** Configure DMA parameters ***************************/
			DmaHandle2.Instance                 = CH2_DMA_Stream;
			DmaHandle2.Init.Request             = CH2_DMA_REQUEST_ADC;
			DmaHandle2.Init.Direction           = DMA_PERIPH_TO_MEMORY;
			DmaHandle2.Init.PeriphInc           = DMA_PINC_DISABLE;
			DmaHandle2.Init.MemInc              = DMA_MINC_ENABLE;
			DmaHandle2.Init.PeriphDataAlignment = DMA_PDATAALIGN_HALFWORD;
			DmaHandle2.Init.MemDataAlignment    = DMA_MDATAALIGN_HALFWORD;
//			if (g_tDSO.TrigMode == 0)
//			{
				DmaHandle2.Init.Mode                = DMA_CIRCULAR;	// DMA_CIRCULAR; DMA_NORMAL
//			}
//			else
//			{
//				DmaHandle2.Init.Mode                = DMA_NORMAL;	// DMA_CIRCULAR; DMA_NORMAL
//			}

			DmaHandle2.Init.Priority            = DMA_PRIORITY_MEDIUM;
			/* Deinitialize  & Initialize the DMA for new transfer */
			HAL_DMA_DeInit(&DmaHandle2);
			HAL_DMA_Init(&DmaHandle2);

			/* Associate the DMA handle */
			__HAL_LINKDMA(hadc, DMA_Handle, DmaHandle2);

			/* NVIC configuration for DMA Input data interrupt */
			HAL_NVIC_SetPriority(CH2_DMA_Stream_IRQn, 1, 0);
			HAL_NVIC_EnableIRQ(CH2_DMA_Stream_IRQn);  	 
		}	
	}
	else if (g_tDSO.MeasuteMode == 1)	/* �߶˵�������ģʽ */
	{
		if (hadc->Instance == CT_ADCH1)
		{	
			GPIO_InitTypeDef          GPIO_InitStruct;
			static DMA_HandleTypeDef  DmaHandle1 = {0};
		
			/*##-1- Enable peripherals and GPIO Clocks #################################*/
			/* Enable GPIO clock ****************************************/
			CT_ADCH1_P_GPIO_CLK_ENABLE();
			CT_ADCH1_N_GPIO_CLK_ENABLE();
			/* ADC Periph clock enable */
			CT_ADCH1_CLK_ENABLE();
			/* ADC Periph interface clock configuration */
			__HAL_RCC_ADC_CONFIG(RCC_ADCCLKSOURCE_CLKP);
			/* Enable DMA clock */
			CT_CH1_DMA_CLK_ENABLE();

			/*##- 2- Configure peripheral GPIO #########################################*/
			/* ADC Channel GPIO pin configuration */
			GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
			GPIO_InitStruct.Pull = GPIO_NOPULL;
			GPIO_InitStruct.Pin = CT_ADCH1_P_PIN;	  
			HAL_GPIO_Init(CT_ADCH1_P_GPIO_PORT, &GPIO_InitStruct);

			GPIO_InitStruct.Pin = CT_ADCH1_N_PIN;	  
			HAL_GPIO_Init(CT_ADCH1_N_GPIO_PORT, &GPIO_InitStruct);	  
			/*##- 3- Configure DMA #####################################################*/ 

			/*********************** Configure DMA parameters ***************************/
			DmaHandle1.Instance                 = CT_CH1_DMA_Stream;
			DmaHandle1.Init.Request             = CT_CH1_DMA_REQUEST_ADC;
			DmaHandle1.Init.Direction           = DMA_PERIPH_TO_MEMORY;
			DmaHandle1.Init.PeriphInc           = DMA_PINC_DISABLE;
			DmaHandle1.Init.MemInc              = DMA_MINC_ENABLE;
			DmaHandle1.Init.PeriphDataAlignment = DMA_PDATAALIGN_HALFWORD;
			DmaHandle1.Init.MemDataAlignment    = DMA_MDATAALIGN_HALFWORD;
//			if (g_tDSO.TrigMode == 0)
//			{
				DmaHandle1.Init.Mode                = DMA_CIRCULAR;	// DMA_CIRCULAR; DMA_NORMAL
//			}
//			else
//			{
//				DmaHandle1.Init.Mode                = DMA_NORMAL;	// DMA_CIRCULAR; DMA_NORMAL
//			}
			DmaHandle1.Init.Priority            = DMA_PRIORITY_MEDIUM;
			/* Deinitialize  & Initialize the DMA for new transfer */
			HAL_DMA_DeInit(&DmaHandle1);
			HAL_DMA_Init(&DmaHandle1);

			/* Associate the DMA handle */
			__HAL_LINKDMA(hadc, DMA_Handle, DmaHandle1);

			/* NVIC configuration for DMA Input data interrupt */
			HAL_NVIC_SetPriority(CT_CH1_DMA_Stream_IRQn, 1, 0);
			HAL_NVIC_EnableIRQ(CT_CH1_DMA_Stream_IRQn);  
		}
		else if (hadc->Instance == CT_ADCH2)
		{
			GPIO_InitTypeDef          GPIO_InitStruct;
			static DMA_HandleTypeDef  DmaHandle2 = {0};

			/*##-1- Enable peripherals and GPIO Clocks #################################*/
			/* Enable GPIO clock ****************************************/
			CT_ADCH2_P_GPIO_CLK_ENABLE();
			CT_ADCH2_N_GPIO_CLK_ENABLE();
			/* ADC Periph clock enable */
			CT_ADCH2_CLK_ENABLE();
			/* ADC Periph interface clock configuration */
			__HAL_RCC_ADC_CONFIG(RCC_ADCCLKSOURCE_CLKP);
			/* Enable DMA clock */
			CT_CH2_DMA_CLK_ENABLE();

			/*##- 2- Configure peripheral GPIO #########################################*/
			/* ADC Channel GPIO pin configuration */

			GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
			GPIO_InitStruct.Pull = GPIO_NOPULL;
			GPIO_InitStruct.Pin = CT_ADCH2_P_PIN;	  
			HAL_GPIO_Init(CT_ADCH2_P_GPIO_PORT, &GPIO_InitStruct);

			GPIO_InitStruct.Pin = CT_ADCH2_N_PIN;	  
			HAL_GPIO_Init(CT_ADCH2_N_GPIO_PORT, &GPIO_InitStruct);	  
			/*##- 3- Configure DMA #####################################################*/ 

			/*********************** Configure DMA parameters ***************************/
			DmaHandle2.Instance                 = CT_CH2_DMA_Stream;
			DmaHandle2.Init.Request             = CT_CH2_DMA_REQUEST_ADC;
			DmaHandle2.Init.Direction           = DMA_PERIPH_TO_MEMORY;
			DmaHandle2.Init.PeriphInc           = DMA_PINC_DISABLE;
			DmaHandle2.Init.MemInc              = DMA_MINC_ENABLE;
			DmaHandle2.Init.PeriphDataAlignment = DMA_PDATAALIGN_HALFWORD;
			DmaHandle2.Init.MemDataAlignment    = DMA_MDATAALIGN_HALFWORD;
//			if (g_tDSO.TrigMode == 0)
//			{
				DmaHandle2.Init.Mode                = DMA_CIRCULAR;	// DMA_CIRCULAR; DMA_NORMAL
//			}
//			else
//			{
//				DmaHandle2.Init.Mode                = DMA_NORMAL;	// DMA_CIRCULAR; DMA_NORMAL
//			}

			DmaHandle2.Init.Priority            = DMA_PRIORITY_MEDIUM;
			/* Deinitialize  & Initialize the DMA for new transfer */
			HAL_DMA_DeInit(&DmaHandle2);
			HAL_DMA_Init(&DmaHandle2);

			/* Associate the DMA handle */
			__HAL_LINKDMA(hadc, DMA_Handle, DmaHandle2);

			/* NVIC configuration for DMA Input data interrupt */
			HAL_NVIC_SetPriority(CT_CH2_DMA_Stream_IRQn, 1, 0);
			HAL_NVIC_EnableIRQ(CT_CH2_DMA_Stream_IRQn);  	 
		}	
	}
	else if (g_tDSO.MeasuteMode == 2)	/* ����ɨ���ͨ��ģʽ - 1024������ */	
	{
		/*
			PF11/ADC1_INP2	---- CH1��ѹ
			PA6/ADC1_INP3	---- �߶˵���
		
			PC3_C/ADC3_INP1	---- CH2��ѹ
			PF5/ADC3_INP4	---- TVCC����������
			PF3/ADC3_INP5	---- NTC����������ֵ���
			PF8/ADC3_INP7	---- ���ص�ѹ
			PH2/ADC3_INP13	---- TVCC��ѹ���		
			PH3/ADC3_INP14	---- 12V�����ѹ���
			PH5/ADC3_INP16	---- USB�����ѹ���			
		*/
		if (hadc->Instance == ADC1)
		{
			GPIO_InitTypeDef          GPIO_InitStruct;
			static DMA_HandleTypeDef  DmaHandle1 = {0};
		
			/*##-1- Enable peripherals and GPIO Clocks #################################*/
			/* Enable GPIO clock ****************************************/
			/* Enable GPIO clock ****************************************/
			__HAL_RCC_GPIOA_CLK_ENABLE();
			__HAL_RCC_GPIOC_CLK_ENABLE();
			__HAL_RCC_GPIOF_CLK_ENABLE();
			__HAL_RCC_GPIOH_CLK_ENABLE();
			/* ADC Periph clock enable */
			__HAL_RCC_ADC12_CLK_ENABLE();
			/* ADC Periph interface clock configuration */
			__HAL_RCC_ADC_CONFIG(RCC_ADCCLKSOURCE_CLKP);
			/* Enable DMA clock */
			CT_CH1_DMA_CLK_ENABLE();

			/*##- 2- Configure peripheral GPIO #########################################*/
			/* ADC Channel GPIO pin configuration */
			GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
			GPIO_InitStruct.Pull = GPIO_NOPULL;
			
			GPIO_InitStruct.Pin = GPIO_PIN_11;	  
			HAL_GPIO_Init(GPIOF, &GPIO_InitStruct);

			GPIO_InitStruct.Pin = GPIO_PIN_6;	  
			HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);	  
			
			/*##- 3- Configure DMA #####################################################*/ 

			/*********************** Configure DMA parameters ***************************/
			DmaHandle1.Instance                 = CT_CH1_DMA_Stream;
			DmaHandle1.Init.Request             = CT_CH1_DMA_REQUEST_ADC;
			DmaHandle1.Init.Direction           = DMA_PERIPH_TO_MEMORY;
			DmaHandle1.Init.PeriphInc           = DMA_PINC_DISABLE;
			DmaHandle1.Init.MemInc              = DMA_MINC_ENABLE;
			DmaHandle1.Init.PeriphDataAlignment = DMA_PDATAALIGN_HALFWORD;
			DmaHandle1.Init.MemDataAlignment    = DMA_MDATAALIGN_HALFWORD;
			DmaHandle1.Init.Mode                = DMA_CIRCULAR;	// DMA_CIRCULAR; DMA_NORMAL
			DmaHandle1.Init.Priority            = DMA_PRIORITY_MEDIUM;
			/* Deinitialize  & Initialize the DMA for new transfer */
			HAL_DMA_DeInit(&DmaHandle1);
			HAL_DMA_Init(&DmaHandle1);

			/* Associate the DMA handle */
			__HAL_LINKDMA(hadc, DMA_Handle, DmaHandle1);

			/* NVIC configuration for DMA Input data interrupt */
			HAL_NVIC_SetPriority(CT_CH1_DMA_Stream_IRQn, 1, 0);
			HAL_NVIC_EnableIRQ(CT_CH1_DMA_Stream_IRQn);  
		}
		else if (hadc->Instance == ADC3)
		{
			GPIO_InitTypeDef          GPIO_InitStruct;
			static DMA_HandleTypeDef  DmaHandle2 = {0};

			/*##-1- Enable peripherals and GPIO Clocks #################################*/
			/* Enable GPIO clock ****************************************/
			__HAL_RCC_GPIOC_CLK_ENABLE();
			__HAL_RCC_GPIOF_CLK_ENABLE();
			__HAL_RCC_GPIOH_CLK_ENABLE();
			/* ADC Periph clock enable */
			__HAL_RCC_ADC3_CLK_ENABLE();
			
			/* ADC Periph interface clock configuration */
			__HAL_RCC_ADC_CONFIG(RCC_ADCCLKSOURCE_CLKP);
			/* Enable DMA clock */
			CT_CH2_DMA_CLK_ENABLE();

			/*##- 2- Configure peripheral GPIO #########################################*/
			/* ADC Channel GPIO pin configuration */

			GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
			GPIO_InitStruct.Pull = GPIO_NOPULL;

			GPIO_InitStruct.Pin = GPIO_PIN_3;	  
			HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

			GPIO_InitStruct.Pin = GPIO_PIN_5 | GPIO_PIN_3 | GPIO_PIN_8;	  
			HAL_GPIO_Init(GPIOF, &GPIO_InitStruct);	   

			GPIO_InitStruct.Pin = GPIO_PIN_2 | GPIO_PIN_3 | GPIO_PIN_5;	  
			HAL_GPIO_Init(GPIOH, &GPIO_InitStruct);	   
   			
			/*##- 3- Configure DMA #####################################################*/ 

			/*********************** Configure DMA parameters ***************************/
			DmaHandle2.Instance                 = CT_CH2_DMA_Stream;
			DmaHandle2.Init.Request             = CT_CH2_DMA_REQUEST_ADC;
			DmaHandle2.Init.Direction           = DMA_PERIPH_TO_MEMORY;
			DmaHandle2.Init.PeriphInc           = DMA_PINC_DISABLE;
			DmaHandle2.Init.MemInc              = DMA_MINC_ENABLE;
			DmaHandle2.Init.PeriphDataAlignment = DMA_PDATAALIGN_HALFWORD;
			DmaHandle2.Init.MemDataAlignment    = DMA_MDATAALIGN_HALFWORD;
			DmaHandle2.Init.Mode                = DMA_CIRCULAR;	// DMA_CIRCULAR; DMA_NORMAL
			DmaHandle2.Init.Priority            = DMA_PRIORITY_MEDIUM;
			/* Deinitialize  & Initialize the DMA for new transfer */
			HAL_DMA_DeInit(&DmaHandle2);
			HAL_DMA_Init(&DmaHandle2);

			/* Associate the DMA handle */
			__HAL_LINKDMA(hadc, DMA_Handle, DmaHandle2);

			/* NVIC configuration for DMA Input data interrupt */
			HAL_NVIC_SetPriority(CT_CH2_DMA_Stream_IRQn, 1, 0);
			HAL_NVIC_EnableIRQ(CT_CH2_DMA_Stream_IRQn);  
		}		
	}
}

/**
  * @brief ADC MSP De-Initialization
  *        This function frees the hardware resources used in this example:
  *          - Disable the Peripheral's clock
  *          - Revert GPIO to their default state
  * @param hadc: ADC handle pointer
  * @retval None
  */
void HAL_ADC_MspDeInit(ADC_HandleTypeDef *hadc)
{
	if (g_tDSO.MeasuteMode	== 0)
	{
		if (hadc->Instance == ADCH1)
		{
			/*##-1- Reset peripherals ##################################################*/
			ADCH1_FORCE_RESET();
			ADCH1_RELEASE_RESET();
			/* ADC Periph clock disable
			(automatically reset all ADC instances of the ADC common group) */
			__HAL_RCC_ADC12_CLK_DISABLE();

			/*##-2- Disable peripherals and GPIO Clocks ################################*/
			/* De-initialize the ADC Channel GPIO pin */
			HAL_GPIO_DeInit(ADCH1_P_GPIO_PORT, ADCH1_P_PIN);
			HAL_GPIO_DeInit(ADCH1_N_GPIO_PORT, ADCH1_N_PIN);
		}
		else if (hadc->Instance == ADCH2)
		{
		  /*##-1- Reset peripherals ##################################################*/
	//	  ADCH2_FORCE_RESET();
	//	  ADCH2_RELEASE_RESET();
		  /* ADC Periph clock disable
		   (automatically reset all ADC instances of the ADC common group) */
	//	  __HAL_RCC_ADC12_CLK_DISABLE();

		  /*##-2- Disable peripherals and GPIO Clocks ################################*/
		  /* De-initialize the ADC Channel GPIO pin */
			HAL_GPIO_DeInit(ADCH2_P_GPIO_PORT, ADCH2_P_PIN);
			HAL_GPIO_DeInit(ADCH2_N_GPIO_PORT, ADCH2_N_PIN);
		}
	}
	else if (g_tDSO.MeasuteMode == 1)	/* ��������ģʽ */
	{
		if (hadc->Instance == CT_ADCH1)
		{
			/*##-1- Reset peripherals ##################################################*/
			CT_ADCH1_FORCE_RESET();
			CT_ADCH1_RELEASE_RESET();
			/* ADC Periph clock disable
			(automatically reset all ADC instances of the ADC common group) */
			__HAL_RCC_ADC12_CLK_DISABLE();

			/*##-2- Disable peripherals and GPIO Clocks ################################*/
			/* De-initialize the ADC Channel GPIO pin */
			HAL_GPIO_DeInit(CT_ADCH1_P_GPIO_PORT, CT_ADCH1_P_PIN);
			HAL_GPIO_DeInit(CT_ADCH1_N_GPIO_PORT, CT_ADCH1_N_PIN);
		}
		else if (hadc->Instance == CT_ADCH2)
		{
		  /*##-1- Reset peripherals ##################################################*/
	//	  ADCH2_FORCE_RESET();
	//	  ADCH2_RELEASE_RESET();
		  /* ADC Periph clock disable
		   (automatically reset all ADC instances of the ADC common group) */
	//	  __HAL_RCC_ADC12_CLK_DISABLE();

		  /*##-2- Disable peripherals and GPIO Clocks ################################*/
		  /* De-initialize the ADC Channel GPIO pin */
			HAL_GPIO_DeInit(CT_ADCH2_P_GPIO_PORT, CT_ADCH2_P_PIN);
			HAL_GPIO_DeInit(CT_ADCH2_N_GPIO_PORT, CT_ADCH2_N_PIN);
		}		
	}
	else if (g_tDSO.MeasuteMode == 2)	/* ��ͨ��ɨ��ģʽ */
	{
		if (hadc->Instance == ADC1)
		{
			/*##-1- Reset peripherals ##################################################*/
			__HAL_RCC_ADC12_FORCE_RESET();
			__HAL_RCC_ADC12_RELEASE_RESET();
			
			/* ADC Periph clock disable
			(automatically reset all ADC instances of the ADC common group) */
			__HAL_RCC_ADC12_CLK_DISABLE();
		/*
			PF11/ADC1_INP2	---- CH1��ѹ
			PA6/ADC1_INP3	---- �߶˵���
		
			PC3_C/ADC3_INP1	---- CH2��ѹ
			PF5/ADC3_INP4	---- TVCC����������
			PF3/ADC3_INP5	---- NTC����������ֵ���
			PF8/ADC3_INP7	---- ���ص�ѹ
			PH2/ADC3_INP13	---- TVCC��ѹ���		
			PH3/ADC3_INP14	---- 12V�����ѹ���
			PH5/ADC3_INP16	---- USB�����ѹ���			
		*/
			/*##-2- Disable peripherals and GPIO Clocks ################################*/
			/* De-initialize the ADC Channel GPIO pin */
			HAL_GPIO_DeInit(GPIOF, GPIO_PIN_11);
			HAL_GPIO_DeInit(GPIOA, GPIO_PIN_6);
		}
		else if (hadc->Instance == ADC3)
		{
		  /*##-1- Reset peripherals ##################################################*/
			__HAL_RCC_ADC3_FORCE_RESET();
			__HAL_RCC_ADC3_RELEASE_RESET();
			
			/* ADC Periph clock disable
			(automatically reset all ADC instances of the ADC common group) */
			__HAL_RCC_ADC3_CLK_DISABLE();

		  /*##-2- Disable peripherals and GPIO Clocks ################################*/
		  /* De-initialize the ADC Channel GPIO pin */
			HAL_GPIO_DeInit(GPIOC, GPIO_PIN_3);
			HAL_GPIO_DeInit(GPIOF, GPIO_PIN_5);
			HAL_GPIO_DeInit(GPIOF, GPIO_PIN_3);
			HAL_GPIO_DeInit(GPIOF, GPIO_PIN_8);
			HAL_GPIO_DeInit(GPIOH, GPIO_PIN_2);
			HAL_GPIO_DeInit(GPIOH, GPIO_PIN_3);
			HAL_GPIO_DeInit(GPIOH, GPIO_PIN_5);
		}		
	}
}

/*
*********************************************************************************************************
*	�� �� ��: DSO_SetSampRate
*	����˵��: �޸Ĳ���Ƶ��. ʹ��TIM4_CC4����
*	��    ��: freq : ����Ƶ�� ��λHz
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void DSO_SetSampRate(uint32_t _ulFreq)
{
	TIM3_Config(_ulFreq);
}

/*
*********************************************************************************************************
*	�� �� ��: CH1_DMA_STREAM_IRQHANDLER
*	����˵��: CH1 DAM�жϷ������
*	��    ��: ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void CH1_DMA_Stream_IRQHandle(void)
{
	HAL_DMA_IRQHandler(AdcHandle1.DMA_Handle);
}

/*
*********************************************************************************************************
*	�� �� ��: CH1_DMA_STREAM_IRQHANDLER
*	����˵��: CH2 DAM�жϷ������,
*	��    ��: ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void CH2_DMA_Stream_IRQHandle(void)
{
	HAL_DMA_IRQHandler(AdcHandle2.DMA_Handle);
}

/*
*********************************************************************************************************
*	�� �� ��: HAL_ADC_ConvHalfCpltCallback
*	����˵��: DAM�жϷ������ص�����������cache����ˢ�µ��ڴ�
*	��    ��: ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void HAL_ADC_ConvHalfCpltCallback(ADC_HandleTypeDef* hadc)
{
	if (hadc->Instance == ADCH1 || hadc->Instance == CT_ADCH1)
	{
		/* Invalidate Data Cache to get the updated content of the SRAM on the first half of the ADC converted data buffer: 32 bytes */ 
		SCB_InvalidateDCache_by_Addr((uint32_t *)aADCH1ConvertedData,  ADC_BUFFER_SIZE);
	}
	else if (hadc->Instance == ADCH2 || hadc->Instance == CT_ADCH2)
	{
		/* Invalidate Data Cache to get the updated content of the SRAM on the first half of the ADC converted data buffer: 32 bytes */ 
		SCB_InvalidateDCache_by_Addr((uint32_t *)aADCH2ConvertedData,  ADC_BUFFER_SIZE);
	}	
}

/*
*********************************************************************************************************
*	�� �� ��: HAL_ADC_ConvCpltCallback
*	����˵��: DAM�жϷ������ص�����������cache����ˢ�µ��ڴ�
*	��    ��: ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef* hadc)
{
	if (hadc->Instance == ADCH1 || hadc->Instance == CT_ADCH1)
	{
		/* Invalidate Data Cache to get the updated content of the SRAM on the first half of the ADC converted data buffer: 32 bytes */ 
		SCB_InvalidateDCache_by_Addr((uint32_t *)(aADCH1ConvertedData + ADC_BUFFER_SIZE),  ADC_BUFFER_SIZE);
	}
	else if (hadc->Instance == ADCH2 || hadc->Instance == CT_ADCH2)
	{
		/* Invalidate Data Cache to get the updated content of the SRAM on the first half of the ADC converted data buffer: 32 bytes */ 
		SCB_InvalidateDCache_by_Addr((uint32_t *)(aADCH2ConvertedData + ADC_BUFFER_SIZE),  ADC_BUFFER_SIZE);
	}	
	
	g_tDSO.DmaTransCplt++;
}

/*
*********************************************************************************************************
*	�� �� ��: DSO_LockWave
*	����˵��: ���沨�Σ��ȵ�PC��ȡ
*	��    ��: ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void DSO_LockWave(void)
{
	uint32_t i;
	uint32_t pos;
	uint32_t TransSize;
	
	SCB_InvalidateDCache_by_Addr((uint32_t *)aADCH1ConvertedData,  2 * ADC_BUFFER_SIZE);
	SCB_InvalidateDCache_by_Addr((uint32_t *)aADCH2ConvertedData,  2 * ADC_BUFFER_SIZE);
	
	pos = g_tDSO.DmaPos;
	TransSize = TabelBufSize[g_tDSO.SampleSizeID];
	for (i = 0; i < TransSize; i++)
	{
		g_Ch1WaveBuf[i] = aADCH1ConvertedData[pos];
		g_Ch2WaveBuf[i] = aADCH2ConvertedData[pos];
		if (++pos >= ADC_BUFFER_SIZE)
		{
			pos = 0;
		}
	}
}


/*
*********************************************************************************************************
*	�� �� ��: AdcSumAvg
*	����˵��: ������ƽ��
*	��    ��: _AdcNo : adcͨ���ţ�0 =ADC1  1=ADC3
*			  _Offset : ƫ�ƣ�rank��
*	�� �� ֵ: ������
*********************************************************************************************************
*/
static float AdcSumAvg(uint8_t _AdcNo, uint8_t _Offset)
{
	float sum;
	uint16_t i;
	
	sum = 0;
	if (_AdcNo == 1)
	{
		for (i = 0; i < SCAN_MODE_SAMPLE_SIZE; i++)
		{
			sum += aADCH1ConvertedData[i * SCAN_MODE_ADC1_NUM + _Offset];
		}
	}
	else if (_AdcNo == 2)
	{
		for (i = 0; i < SCAN_MODE_SAMPLE_SIZE; i++)
		{
			sum += aADCH2ConvertedData[i * SCAN_MODE_ADC3_NUM + _Offset];
		}
	}
	
	sum = sum / SCAN_MODE_SAMPLE_SIZE;	/* ����ƽ��ֵ */
	
	return sum;
}

/*
*********************************************************************************************************
*	�� �� ��: bsp_GetAdcAvg
*	����˵��: ��ȡͨ����ADC��ֵ
*	��    ��: ģ��ͨ����
*	�� �� ֵ: ������
*********************************************************************************************************
*/
float bsp_GetAdcAvg(uint8_t _ch)
{
	/*
		PF11/ADC1_INP2	---- CH1��ѹ
		PA6/ADC1_INP3	---- �߶˵���
	
		PC3_C/ADC3_INP1	---- CH2��ѹ
		PF5/ADC3_INP4	---- TVCC����������
		PF3/ADC3_INP5	---- NTC����������ֵ���
		PF8/ADC3_INP7	---- ���ص�ѹ
		PH2/ADC3_INP13	---- TVCC��ѹ���		
		PH3/ADC3_INP14	---- 12V�����ѹ���
		PH5/ADC3_INP16	---- USB�����ѹ���			
	*/
	float ff;
	
	/* ADC1 ɨ��2��ͨ�� */
	if (_ch == AN_CH1)			/* CH1��ѹ */
	{
		ff = AdcSumAvg(1, 0);
	}
	else if (_ch == AN_HIGH_SIDE_CURR)	/* �߶˸��ص��� */
	{
		ff = AdcSumAvg(1, 1);
	}	
	
	/* ADC3 ɨ��7��ͨ�� */
	else if (_ch == AN_CH2)		/* CH2��ѹ */
	{
		ff = AdcSumAvg(2, 0);
	}	
	else if (_ch == AN_TVCC_CURR)	/* TVCC���������� */
	{
		ff = AdcSumAvg(2, 1);
	}
	else if (_ch == AN_NTC_RES)		/* NTC����������ֵ��� */
	{
		ff = AdcSumAvg(2, 2);
	}	
	else if (_ch == AN_HIGH_SIDE_VOLT)	/* �߶˸��ص�ѹ */
	{
		ff = AdcSumAvg(2, 3);
	}	
	else if (_ch == AN_TVCC_VOLT)	/* TVCC��ѹ���	*/
	{
		ff = AdcSumAvg(2, 4);
	}
	else if (_ch == AN_12V_VOLT)	/* 12V�����ѹ��� */
	{
		ff = AdcSumAvg(2, 5);
	}
	else if (_ch == AN_USB_VOLT)	/* USB�����ѹ��� */
	{
		ff = AdcSumAvg(2, 6);
	}	
	else
	{
		ff = 0;
	}
	
	return ff;
}

/*
*********************************************************************************************************
*	�� �� ��: bsp_AdcTask
*	����˵��: ɨ��ADC����.  ����10ms�ж�ִ��. ÿ�θ���1��ͨ��. 90msˢ��һ��ȫ�ֱ���
*	��    ��: ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void bsp_AdcTask10ms(void)
{
	static uint8_t s_idx = 0;
	
	if (g_tDSO.MeasuteMode != 2)
	{
		g_tVar.CH1Volt = 0;
		g_tVar.CH2Volt = 0;
		g_tVar.HighSideVolt = 0;
		g_tVar.HighSideCurr = 0;
		g_tVar.USBPowerVolt = 0;
		g_tVar.TVCCVolt = 
		g_tVar.TVCCCurr = 0;
		g_tVar.NTCRes = 0;		
		g_tVar.NTCTemp = 0;
		return;
	}
	
	if (s_idx == 0)
	{
		g_tVar.ADC_CH1Volt = bsp_GetAdcAvg(AN_CH1);
		g_tVar.CH1Volt = CaculTwoPointFloat(
			g_tCalib.CH1[g_tDSO.Gain1].x1, g_tCalib.CH1[g_tDSO.Gain1].y1, 
			g_tCalib.CH1[g_tDSO.Gain1].x2, g_tCalib.CH1[g_tDSO.Gain1].y2, 
			g_tVar.ADC_CH1Volt);
	}
	else if (s_idx == 1)
	{
		g_tVar.ADC_CH2Volt = bsp_GetAdcAvg(AN_CH2);
		g_tVar.CH2Volt = CaculTwoPointFloat(
			g_tCalib.CH2[g_tDSO.Gain2].x1, g_tCalib.CH2[g_tDSO.Gain2].y1, 
			g_tCalib.CH2[g_tDSO.Gain2].x2, g_tCalib.CH2[g_tDSO.Gain2].y2, 
			g_tVar.ADC_CH2Volt);		
	}	
	else if (s_idx == 2)
	{
		g_tVar.ADC_HighSideVolt = bsp_GetAdcAvg(AN_HIGH_SIDE_VOLT);
		g_tVar.HighSideVolt = CaculTwoPointFloat(
			g_tCalib.LoadVolt.x1, g_tCalib.LoadVolt.y1, 
			g_tCalib.LoadVolt.x2, g_tCalib.LoadVolt.y2, 
			g_tVar.ADC_HighSideVolt);	

		if (g_tVar.HighSideVolt < (float)0.3)		/* С��0.3V ��Ϊ0 */
		{
			g_tVar.HighSideVolt = 0;
		}
	}
	else if (s_idx == 3)	/* �߲���� */
	{
		float adc;
		float x1,y1,x2,y2;
		
		adc = bsp_GetAdcAvg(AN_HIGH_SIDE_CURR);
		g_tVar.ADC_HighSideCurr = adc;
		
		if (adc <= g_tCalib.LoadCurr[g_tDSO.CurrGain].x2)
		{
			x1 = g_tCalib.LoadCurr[g_tDSO.CurrGain].x1;
			y1 = g_tCalib.LoadCurr[g_tDSO.CurrGain].y1;
			x2 = g_tCalib.LoadCurr[g_tDSO.CurrGain].x2;
			y2 = g_tCalib.LoadCurr[g_tDSO.CurrGain].y2;
		}
		else if (adc <= g_tCalib.LoadCurr[g_tDSO.CurrGain].x3)
		{
			x1 = g_tCalib.LoadCurr[g_tDSO.CurrGain].x2;
			y1 = g_tCalib.LoadCurr[g_tDSO.CurrGain].y2;
			x2 = g_tCalib.LoadCurr[g_tDSO.CurrGain].x3;
			y2 = g_tCalib.LoadCurr[g_tDSO.CurrGain].y3;
		}		
		else 
		{
			x1 = g_tCalib.LoadCurr[g_tDSO.CurrGain].x3;
			y1 = g_tCalib.LoadCurr[g_tDSO.CurrGain].y3;
			x2 = g_tCalib.LoadCurr[g_tDSO.CurrGain].x4;
			y2 = g_tCalib.LoadCurr[g_tDSO.CurrGain].y4;			
		}
		g_tVar.HighSideCurr = CaculTwoPointFloat(x1, y1, x2, y2, adc);	
		if (g_tVar.HighSideCurr < (float)0.2)		/* 0.2mA������Ϊ0 */
		{
			g_tVar.HighSideCurr = 0;
		}
	}
	else if (s_idx == 4)
	{
		g_tVar.ADC_USBPowerVolt = bsp_GetAdcAvg(AN_USB_VOLT);
		g_tVar.USBPowerVolt = CaculTwoPointFloat(
			g_tCalib.USBVolt.x1, g_tCalib.USBVolt.y1, 
			g_tCalib.USBVolt.x2, g_tCalib.USBVolt.y2, 
			g_tVar.ADC_USBPowerVolt);			
	}
	else if (s_idx == 5)
	{
		g_tVar.ADC_ExtPowerVolt = bsp_GetAdcAvg(AN_12V_VOLT);
		g_tVar.ExtPowerVolt = CaculTwoPointFloat(
			g_tCalib.ExtPowerVolt.x1, g_tCalib.ExtPowerVolt.y1, 
			g_tCalib.ExtPowerVolt.x2, g_tCalib.ExtPowerVolt.y2, 
			g_tVar.ADC_ExtPowerVolt);			
	}
	else if (s_idx == 6)
	{
		g_tVar.ADC_TVCCVolt = bsp_GetAdcAvg(AN_TVCC_VOLT);
		g_tVar.TVCCVolt = CaculTwoPointFloat(
			g_tCalib.TVCCVolt.x1, g_tCalib.TVCCVolt.y1, 
			g_tCalib.TVCCVolt.x2, g_tCalib.TVCCVolt.y2, 
			g_tVar.ADC_TVCCVolt);		
	}	
	else if (s_idx == 7)	/* TVCC ���� */
	{
		float adc;
		float x1,y1,x2,y2;
		
		adc = bsp_GetAdcAvg(AN_TVCC_CURR);
		g_tVar.ADC_TVCCCurr = adc;
		
		if (adc <= g_tCalib.TVCCCurr.x2)
		{
			x1 = g_tCalib.TVCCCurr.x1;
			y1 = g_tCalib.TVCCCurr.y1;
			x2 = g_tCalib.TVCCCurr.x2;
			y2 = g_tCalib.TVCCCurr.y2;
		}
		else if (adc <= g_tCalib.TVCCCurr.x3)
		{
			x1 = g_tCalib.TVCCCurr.x2;
			y1 = g_tCalib.TVCCCurr.y2;
			x2 = g_tCalib.TVCCCurr.x3;
			y2 = g_tCalib.TVCCCurr.y3;
		}		
		else 
		{
			x1 = g_tCalib.TVCCCurr.x3;
			y1 = g_tCalib.TVCCCurr.y3;
			x2 = g_tCalib.TVCCCurr.x4;
			y2 = g_tCalib.TVCCCurr.y4;			
		}
		g_tVar.TVCCCurr = CaculTwoPointFloat(x1, y1, x2, y2, adc);	
		if (g_tVar.TVCCCurr < (float)0.05)	/* 0.05mA������Ϊ0 */
		{
			g_tVar.TVCCCurr = 0;
		}		
	}
	else if (s_idx == 8)
	{
		float ref_res;
		
		g_tVar.ADC_NTCRes = bsp_GetAdcAvg(AN_NTC_RES);		
		ref_res = CalculRefRes(g_tCalib.NtcRes.x1, g_tCalib.NtcRes.y1);	/* �����ڲ��������� */
		g_tVar.NTCRes = CalculNtcRes(ref_res, g_tVar.ADC_NTCRes);	/* ����ADC������� */		
		g_tVar.NTCTemp = CalculNtcTemperFloat(g_tVar.NTCRes);	/* ���ݵ�����õ��¶� */
	}	
	
	if (++s_idx > 8)
	{
		s_idx = 0;
	}
}

/***************************** ���������� www.armfly.com (END OF FILE) *********************************/
