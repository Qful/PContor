/*
*********************************************************************************************************
*
*	ģ������ : TIM��ʱDMA����ģ��
*	�ļ����� : bps_tim_dma.c
*	��    �� : V1.3
*	˵    �� : TIM��ʱ����DMA���䣬FMC�����GPIO
*
*	�޸ļ�¼ :
*		�汾��  ����        ����     ˵��
*		V1.0    2019-01-20  Eric    ��ʽ����
*
*	Copyright (C), 2019-2020, ���������� www.armfly.com
*
*********************************************************************************************************
*/
#include "bsp.h"

/* ����Cache���API��������32�ֽڶ���, �õ�SRAM3 */
#if defined ( __ICCARM__ )
#pragma location = 0x38000000
uint8_t SRC_Buffer_Toggle[16]  =
                              { 
                                0x00U,     
                                0xFFU,   
                                0x00U,   
                                0xFFU,   
                                0x00U,  
                                0xFFU,   
                                0x00U,  
                                0xFFU,   
                                0x00U, 
                                0xFFU,   
                                0x00U,  
                                0xFFU,  
                                0x00U, 
                                0xFFU,  
                                0x00U,  
                                0xFFU, 
                              };

#elif defined ( __CC_ARM )
//ALIGN_32BYTES(__attribute__((section (".RAM_D3"))) uint32_t SRC_Buffer_Toggle[16]) =
const uint32_t SRC_Buffer_Toggle[16] =
                                                                  { 
                                                                    0x00U,     
                                                                    0xFFU,   
                                                                    0x00U,   
                                                                    0xFFU,   
                                                                    0x00U,  
                                                                    0xFFU,   
                                                                    0x00U,  
                                                                    0xFFU,   
                                                                    0x00U, 
                                                                    0xFFU,   
                                                                    0x00U,  
                                                                    0xFFU,  
                                                                    0x00U, 
                                                                    0xFFU,  
                                                                    0x00U,  
                                                                    0xFFU, 
                                                                  };
#endif


/*
*********************************************************************************************************
*	�� �� ��: TIM12_Config
*	����˵��: ����TIM12�����ڴ���DMAMUX����������
*	��    ��: ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void TIM12_Config(void)
{
    TIM_HandleTypeDef  htim ={0};
    TIM_MasterConfigTypeDef sMasterConfig;
    TIM_OC_InitTypeDef sConfig;
    
  	__HAL_RCC_TIM12_CLK_ENABLE();

    /*-----------------------------------------------------------------------
		bsp.c �ļ��� void SystemClock_Config(void) ������ʱ�ӵ���������: 

        System Clock source       = PLL (HSE)
        SYSCLK(Hz)                = 400000000 (CPU Clock)
        HCLK(Hz)                  = 200000000 (AXI and AHBs Clock)
        AHB Prescaler             = 2
        D1 APB3 Prescaler         = 2 (APB3 Clock  100MHz)
        D2 APB1 Prescaler         = 2 (APB1 Clock  100MHz)
        D2 APB2 Prescaler         = 2 (APB2 Clock  100MHz)
        D3 APB4 Prescaler         = 2 (APB4 Clock  100MHz)

        ��ΪAPB1 prescaler != 1, ���� APB1�ϵ�TIMxCLK = APB1 x 2 = 200MHz;
        ��ΪAPB2 prescaler != 1, ���� APB2�ϵ�TIMxCLK = APB2 x 2 = 200MHz;
        APB4�����TIMxCLKû�з�Ƶ�����Ծ���100MHz;

        APB1 ��ʱ���� TIM2, TIM3 ,TIM4, TIM5, TIM6, TIM7, TIM12, TIM13, TIM14��LPTIM1
        APB2 ��ʱ���� TIM1, TIM8 , TIM15, TIM16��TIM17

        APB4 ��ʱ���� LPTIM2��LPTIM3��LPTIM4��LPTIM5

    TIM12CLK = 200MHz/(Period + 1) / (Prescaler + 1) = 5MHz
    ����bsp_InitTimDMA1��DMAMUX1ѡ�����˫���ش�����ÿ��ʱ�ӿ��Դ������Ρ�
	----------------------------------------------------------------------- */    
    htim.Instance = TIM12;

	htim.Init.Period            = 200 / 1 - 1;	
	htim.Init.Prescaler         = 0;
	htim.Init.ClockDivision     = 0;
	htim.Init.CounterMode       = TIM_COUNTERMODE_UP;
	htim.Init.RepetitionCounter = 0;
	HAL_TIM_Base_Init(&htim);
    
    sConfig.OCMode     = TIM_OCMODE_PWM1;
    sConfig.OCPolarity = TIM_OCPOLARITY_LOW;

    /* ѡ��OC1 */
    sConfig.Pulse =  (htim.Init.Period + 1) / 2 - 1;  
    if(HAL_TIM_OC_ConfigChannel(&htim, &sConfig, TIM_CHANNEL_1) != HAL_OK)
    {
		Error_Handler(__FILE__, __LINE__);
    }

    if(HAL_TIM_OC_Start(&htim, TIM_CHANNEL_1) != HAL_OK)
    {
		Error_Handler(__FILE__, __LINE__);
    }
    
    /* TIM12 TRGO ����DMAMUX1���������� HAL_DMAMUX1_REQ_GEN_TIM12_TRGO */
	sMasterConfig.MasterOutputTrigger = TIM_TRGO_OC1REF;
    sMasterConfig.MasterOutputTrigger2 = TIM_TRGO2_RESET;
	sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;

	HAL_TIMEx_MasterConfigSynchronization(&htim, &sMasterConfig);
}

/*
*********************************************************************************************************
*	�� �� ��: bsp_InitTimDMA1
*	����˵��: ��ʱ����DMA����FMC
*	��    ��: ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void bsp_InitTimDMA1(void)
{
    DMA_HandleTypeDef DMA_Handle = {0};
    HAL_DMA_MuxRequestGeneratorConfigTypeDef dmamux_ReqGenParams = {0};
    
    /* ʹ��ʱ�� */
    __HAL_RCC_DMA1_CLK_ENABLE();

     /*##-1- ����DMA ##################################################*/
    /* ʹ�õ�DMA1����DMAMUX1����������ʹ�õ�ͨ��0����DMA_REQUEST_GENERATOR0 */
    DMA_Handle.Instance                 = DMA1_Stream1;
    DMA_Handle.Init.Request             = DMA_REQUEST_GENERATOR0;  
    DMA_Handle.Init.Direction           = DMA_MEMORY_TO_PERIPH;
    DMA_Handle.Init.PeriphInc           = DMA_PINC_DISABLE;
    DMA_Handle.Init.MemInc              = DMA_MINC_ENABLE;
    DMA_Handle.Init.PeriphDataAlignment = DMA_PDATAALIGN_WORD;		//DMA_PDATAALIGN_BYTE;
    DMA_Handle.Init.MemDataAlignment    = DMA_MDATAALIGN_WORD;	//DMA_PDATAALIGN_BYTE;
    DMA_Handle.Init.Mode                = DMA_CIRCULAR;
    DMA_Handle.Init.Priority            = DMA_PRIORITY_VERY_HIGH;	//DMA_PRIORITY_LOW;
    DMA_Handle.Init.FIFOMode            = DMA_FIFOMODE_DISABLE;
    DMA_Handle.Init.FIFOThreshold       = DMA_FIFO_THRESHOLD_FULL;
    DMA_Handle.Init.MemBurst            = DMA_MBURST_SINGLE; // DMA_MBURST_SINGLE;
    DMA_Handle.Init.PeriphBurst         = DMA_MBURST_SINGLE;	//DMA_PBURST_SINGLE;
    
    /* ��ʼ�� */
    HAL_DMA_Init(&DMA_Handle);

    /*##-2- ����DMAMUX1 ##################################################*/
    /* ����ѡ��LPTIM1��TIM12���� */
    //dmamux_ReqGenParams.SignalID  =    HAL_DMAMUX1_REQ_GEN_LPTIM1_OUT;  /* ���󴥷���ѡ��LPTIM1_OUT */
    dmamux_ReqGenParams.SignalID  = HAL_DMAMUX1_REQ_GEN_TIM12_TRGO;   /* ���󴥷���ѡ��TIM12_TRGO */
    dmamux_ReqGenParams.Polarity  = HAL_DMAMUX_REQ_GEN_RISING; /* TIM����������غ��½��ؾ��ɴ���  */
    dmamux_ReqGenParams.RequestNumber = 3;                             /* �����󣬴������1��DMA���� */

    HAL_DMAEx_ConfigMuxRequestGenerator(&DMA_Handle, &dmamux_ReqGenParams);
    HAL_DMAEx_EnableMuxRequestGenerator (&DMA_Handle);

    /*##-3- ����DMA #####################################################*/
    HAL_DMA_Start(&DMA_Handle, (uint32_t)SRC_Buffer_Toggle, (uint32_t)0x60000000, 8);

    /* LPTIM1��TIM12�������˳�ʼ����������� */
//    LPTIM1_Config();
    TIM12_Config();
}

/***************************** ���������� www.armfly.com (END OF FILE) *********************************/
