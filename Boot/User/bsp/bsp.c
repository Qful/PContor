/*
*********************************************************************************************************
*
*	ģ������ : BSPģ��(For STM32H7)
*	�ļ����� : bsp.c
*	��    �� : V1.0
*	˵    �� : ����Ӳ���ײ�������������ļ���ÿ��c�ļ����� #include "bsp.h" ���������е���������ģ�顣
*			   bsp = Borad surport packet �弶֧�ְ�
*	�޸ļ�¼ :
*		�汾��  ����         ����       ˵��
*		V1.0    2018-07-29  Eric2013   ��ʽ����
*
*	Copyright (C), 2018-2030, ���������� www.armfly.com
*
*********************************************************************************************************
*/
#include "bsp.h"

/*
*********************************************************************************************************
*	                                   ��������
*********************************************************************************************************
*/
static void SystemClock_Config(void);
static void CPU_CACHE_Enable(void);
static void MPU_Config(void);

/*
*********************************************************************************************************
*	�� �� ��: bsp_Init
*	����˵��: ��ʼ�����е�Ӳ���豸���ú�������CPU�Ĵ���������ļĴ�������ʼ��һЩȫ�ֱ�����ֻ��Ҫ����һ��
*	��    ��: ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void bsp_Init(void)
{
    /* ����MPU */
	MPU_Config();
	
	/* ʹ��L1 Cache */
	CPU_CACHE_Enable();

	/* 
       STM32H7xx HAL ���ʼ������ʱϵͳ�õĻ���H7�Դ���64MHz��HSIʱ��:
	   - ���ú���HAL_InitTick����ʼ���δ�ʱ���ж�1ms��
	   - ����NVIV���ȼ�����Ϊ4��
	 */
	HAL_Init();

	/* 
       ����ϵͳʱ�ӵ�400MHz
       - �л�ʹ��HSE��
       - �˺��������ȫ�ֱ���SystemCoreClock������������HAL_InitTick��
    */
	SystemClock_Config();

	/* 
	   Event Recorder��
	   - �����ڴ���ִ��ʱ�������MDK5.25�������ϰ汾��֧�֣�IAR��֧�֡�
	   - Ĭ�ϲ����������Ҫʹ�ܴ�ѡ���ؿ�V7�������û��ֲ��xx��
	*/	
#if Enable_EventRecorder == 1  
	/* ��ʼ��EventRecorder������ */
	EventRecorderInitialize(EventRecordAll, 1U);
	EventRecorderStart();
#endif
		
	bsp_InitKey();    	/* ������ʼ����Ҫ���ڵδ�ʱ��֮ǰ����Ϊ��ť�����ͨ���δ�ʱ��ɨ�� */
	bsp_InitTimer();  	/* ��ʼ���δ�ʱ�� */
	
	PERIOD_InitVar();
	
//	bsp_InitUart();		/* ��ʼ������ */
	bsp_InitLed();    	/* ��ʼ��LED */	
//	bsp_InitI2C();		/* ��ʼ��I2C���� */
//	bsp_InitSPIBus();	/* ��ʼ��SPI���� */

//	BEEP_InitHard();
//	ee_CheckOk();		/* ���EEPROM */
	
	
//	HC595_InitHard();	/* ����ʾ����ģ���ϵ�GPIOоƬ */
//	
//	bsp_InitDAC1();		/* ����DAC���� */
//	
//	bsp_InitTVCC();		/* TVCC�������� */
//	
//	bsp_InitMCP4725();	/* ʾ����ƫ�õ�ѹ */
	
//	bsp_InitExtIO();		/* ����˿ڳ�ʼ�� */
//	LCD_InitHard();
	
//	bsp_InitQSPI_W25Q256();	/* ��ʼ��QSPI */
	
//	bsp_InitRTC();		/* ��ʼ��ʱ�� */
}

/*
*********************************************************************************************************
*	�� �� ��: SystemClock_Config
*	����˵��: ��ʼ��ϵͳʱ��
*            	System Clock source            = PLL (HSE BYPASS)
*            	SYSCLK(Hz)                     = 400000000 (CPU Clock)
*           	HCLK(Hz)                       = 200000000 (AXI and AHBs Clock)
*            	AHB Prescaler                  = 2
*            	D1 APB3 Prescaler              = 2 (APB3 Clock  100MHz)
*            	D2 APB1 Prescaler              = 2 (APB1 Clock  100MHz)
*            	D2 APB2 Prescaler              = 2 (APB2 Clock  100MHz)
*            	D3 APB4 Prescaler              = 2 (APB4 Clock  100MHz)
*            	HSE Frequency(Hz)              = 25000000   (8000000)
*           	PLL_M                          = 5          (4)
*            	PLL_N                          = 160        (400)
*            	PLL_P                          = 2
*            	PLL_Q                          = 4
*            	PLL_R                          = 2
*            	VDD(V)                         = 3.3
*            	Flash Latency(WS)              = 4
*	��    ��: ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
static void SystemClock_Config(void)
{
	RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};
	RCC_OscInitTypeDef RCC_OscInitStruct = {0};
	RCC_PeriphCLKInitTypeDef PeriphClkInitStruct = {0};;
	HAL_StatusTypeDef ret = HAL_OK;

	/* ��סSCU(Supply configuration update) */
	MODIFY_REG(PWR->CR3, PWR_CR3_SCUEN, 0);

	/* 
      1��оƬ�ڲ���LDO��ѹ������ĵ�ѹ��Χ����ѡVOS1��VOS2��VOS3����ͬ��Χ��Ӧ��ͬ��Flash���ٶȣ�
         ���鿴�ο��ֲ��Table 12�ı���
		//      2������ѡ��ʹ��VOS1����ѹ��Χ1.15V - 1.26V��
    */
	__HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

	while(!__HAL_PWR_GET_FLAG(PWR_FLAG_VOSRDY)) {}
	
	/* Enable D2 domain SRAM3 Clock (0x30040000 AXI)*/
	__HAL_RCC_D2SRAM3_CLK_ENABLE();
		
//	/* Macro to configure the PLL clock source  */
//	__HAL_RCC_PLL_PLLSOURCE_CONFIG(RCC_PLLSOURCE_HSE);

	/* ʹ��HSE����ѡ��HSE��ΪPLLʱ��Դ */	
	RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
	RCC_OscInitStruct.HSEState = RCC_HSE_ON;
	RCC_OscInitStruct.HSIState = RCC_HSI_OFF;
	RCC_OscInitStruct.CSIState = RCC_CSI_OFF;
	RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
	RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
	RCC_OscInitStruct.PLL.PLLM = 5;
	RCC_OscInitStruct.PLL.PLLN = 160;
	RCC_OscInitStruct.PLL.PLLP = 2;
	RCC_OscInitStruct.PLL.PLLR = 2;
	RCC_OscInitStruct.PLL.PLLQ = 4;		
	RCC_OscInitStruct.PLL.PLLVCOSEL = RCC_PLL1VCOWIDE;
	RCC_OscInitStruct.PLL.PLLRGE = RCC_PLL1VCIRANGE_2;		
	ret = HAL_RCC_OscConfig(&RCC_OscInitStruct);
	if(ret != HAL_OK)
	{
        Error_Handler(__FILE__, __LINE__);
	}
	
	/* PLL3-Q for USB Clock = 48M */
    PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_USB;
	PeriphClkInitStruct.PLL3.PLL3M = 5;
	PeriphClkInitStruct.PLL3.PLL3N = 48;
	PeriphClkInitStruct.PLL3.PLL3P = 2;
	PeriphClkInitStruct.PLL3.PLL3Q = 5;
	PeriphClkInitStruct.PLL3.PLL3R = 2;
	PeriphClkInitStruct.PLL3.PLL3RGE = RCC_PLL3VCIRANGE_2;
	PeriphClkInitStruct.PLL3.PLL3VCOSEL = RCC_PLL3VCOWIDE;
	PeriphClkInitStruct.PLL3.PLL3FRACN = 0;
	PeriphClkInitStruct.UsbClockSelection = RCC_USBCLKSOURCE_PLL3;
	  HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct);
	  
	/* PLL3-R for LTDC */
//	PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_LTDC;
//	PeriphClkInitStruct.PLL3.PLL3M = 25;    
//	PeriphClkInitStruct.PLL3.PLL3N = 160;
//	PeriphClkInitStruct.PLL3.PLL3P = 2;
//	PeriphClkInitStruct.PLL3.PLL3Q = 2;
//	PeriphClkInitStruct.PLL3.PLL3R = 32;  
//	HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct); 		

//	/* Disable  PLL3. */
//	__HAL_RCC_PLL3_DISABLE();

	/* 
       ѡ��PLL�������Ϊϵͳʱ��
       ����RCC_CLOCKTYPE_SYSCLKϵͳʱ��
       ����RCC_CLOCKTYPE_HCLK ʱ�ӣ���ӦAHB1��AHB2��AHB3��AHB4����
       ����RCC_CLOCKTYPE_PCLK1ʱ�ӣ���ӦAPB1����
       ����RCC_CLOCKTYPE_PCLK2ʱ�ӣ���ӦAPB2����
       ����RCC_CLOCKTYPE_D1PCLK1ʱ�ӣ���ӦAPB3����
       ����RCC_CLOCKTYPE_D3PCLK1ʱ�ӣ���ӦAPB4����     
    */
	RCC_ClkInitStruct.ClockType = (RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_D1PCLK1 | RCC_CLOCKTYPE_PCLK1 | \
								 RCC_CLOCKTYPE_PCLK2  | RCC_CLOCKTYPE_D3PCLK1);

	RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
	RCC_ClkInitStruct.SYSCLKDivider = RCC_SYSCLK_DIV1;
	RCC_ClkInitStruct.AHBCLKDivider = RCC_HCLK_DIV2;
	RCC_ClkInitStruct.APB3CLKDivider = RCC_APB3_DIV2;  
	RCC_ClkInitStruct.APB1CLKDivider = RCC_APB1_DIV2; 
	RCC_ClkInitStruct.APB2CLKDivider = RCC_APB2_DIV2; 
	RCC_ClkInitStruct.APB4CLKDivider = RCC_APB4_DIV2; 
	
	/* �˺��������SystemCoreClock������������HAL_InitTick */
	ret = HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_4);
	if(ret != HAL_OK)
	{
        Error_Handler(__FILE__, __LINE__);
	}
  
    /*
      ʹ��IO�ĸ���ģʽ��Ҫʹ��IO������������������������ 
      ��1��ʹ��CSI clock
      ��2��ʹ��SYSCFG clock
      ��3��ʹ��I/O������Ԫ�� ����SYSCFG_CCCSR�Ĵ�����bit0
    */
	__HAL_RCC_CSI_ENABLE() ;

	__HAL_RCC_SYSCFG_CLK_ENABLE() ;

	HAL_EnableCompensationCell();
}

/*
*********************************************************************************************************
*	�� �� ��: Error_Handler
*	��    ��: file : Դ�����ļ����ơ��ؼ��� __FILE__ ��ʾԴ�����ļ�����
*			  line �������кš��ؼ��� __LINE__ ��ʾԴ�����к�
*	�� �� ֵ: ��
*		Error_Handler(__FILE__, __LINE__);
*********************************************************************************************************
*/
void Error_Handler(char *file, uint32_t line)
{
	/* 
		�û����������Լ��Ĵ��뱨��Դ�����ļ����ʹ����кţ����罫�����ļ����кŴ�ӡ������
		printf("Wrong parameters value: file %s on line %d\r\n", file, line) 
	*/
	
	/* ����һ����ѭ��������ʧ��ʱ������ڴ˴��������Ա����û���� */
	if (line == 0)
	{
		return;
	}
	
	while(1)
	{
	}
}

/*
*********************************************************************************************************
*	�� �� ��: MPU_Config
*	����˵��: ����MPU
*	��    ��: ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
static void MPU_Config( void )
{
	MPU_Region_InitTypeDef MPU_InitStruct;

	/* ��ֹ MPU */
	HAL_MPU_Disable();
	
#if 1
	/* Configure the MPU attributes as Device not cacheable 
	 for ETH DMA descriptors */
	MPU_InitStruct.Enable = MPU_REGION_ENABLE;
	MPU_InitStruct.BaseAddress = 0x30040000;
	MPU_InitStruct.Size = MPU_REGION_SIZE_256B;
	MPU_InitStruct.AccessPermission = MPU_REGION_FULL_ACCESS;
	MPU_InitStruct.IsBufferable = MPU_ACCESS_BUFFERABLE;
	MPU_InitStruct.IsCacheable = MPU_ACCESS_NOT_CACHEABLE;
	MPU_InitStruct.IsShareable = MPU_ACCESS_NOT_SHAREABLE;
	MPU_InitStruct.Number = MPU_REGION_NUMBER0;
	MPU_InitStruct.TypeExtField = MPU_TEX_LEVEL0;
	MPU_InitStruct.SubRegionDisable = 0x00;
	MPU_InitStruct.DisableExec = MPU_INSTRUCTION_ACCESS_ENABLE;

	HAL_MPU_ConfigRegion(&MPU_InitStruct);

	/* Configure the MPU attributes as Cacheable write through 
	 for LwIP RAM heap which contains the Tx buffers */
	MPU_InitStruct.Enable = MPU_REGION_ENABLE;
	MPU_InitStruct.BaseAddress = 0x30044000;
	MPU_InitStruct.Size = MPU_REGION_SIZE_16KB;
	MPU_InitStruct.AccessPermission = MPU_REGION_FULL_ACCESS;
	MPU_InitStruct.IsBufferable = MPU_ACCESS_NOT_BUFFERABLE;
	MPU_InitStruct.IsCacheable = MPU_ACCESS_CACHEABLE;
	MPU_InitStruct.IsShareable = MPU_ACCESS_NOT_SHAREABLE;
	MPU_InitStruct.Number = MPU_REGION_NUMBER1;
	MPU_InitStruct.TypeExtField = MPU_TEX_LEVEL0;
	MPU_InitStruct.SubRegionDisable = 0x00;
	MPU_InitStruct.DisableExec = MPU_INSTRUCTION_ACCESS_ENABLE;

	HAL_MPU_ConfigRegion(&MPU_InitStruct);
#endif

	/* ����AXI SRAM��MPU����ΪWrite through */
	MPU_InitStruct.Enable           = MPU_REGION_ENABLE;
	MPU_InitStruct.BaseAddress      = 0x24000000;
	MPU_InitStruct.Size             = MPU_REGION_SIZE_512KB;
	MPU_InitStruct.AccessPermission = MPU_REGION_FULL_ACCESS;
	MPU_InitStruct.IsBufferable     = MPU_ACCESS_NOT_BUFFERABLE;
	MPU_InitStruct.IsCacheable      = MPU_ACCESS_CACHEABLE;
	MPU_InitStruct.IsShareable      = MPU_ACCESS_NOT_SHAREABLE;
	
	//MPU_InitStruct.Number           = MPU_REGION_NUMBER1;
	MPU_InitStruct.Number           = MPU_REGION_NUMBER2;
	MPU_InitStruct.TypeExtField     = MPU_TEX_LEVEL0;
	MPU_InitStruct.SubRegionDisable = 0x00;
	MPU_InitStruct.DisableExec      = MPU_INSTRUCTION_ACCESS_ENABLE;
	HAL_MPU_ConfigRegion(&MPU_InitStruct);
		
	/* ����������չIO�ռ������ΪWrite through */
	MPU_InitStruct.Enable           = MPU_REGION_ENABLE;
	MPU_InitStruct.BaseAddress      = 0x60000000;
	MPU_InitStruct.Size             = ARM_MPU_REGION_SIZE_64KB;	// ARM_MPU_REGION_SIZE_16KB;
	MPU_InitStruct.AccessPermission = MPU_REGION_FULL_ACCESS;
	MPU_InitStruct.IsBufferable     = MPU_ACCESS_NOT_BUFFERABLE;
	MPU_InitStruct.IsCacheable      = MPU_ACCESS_NOT_CACHEABLE;		/* ������MPU_ACCESS_CACHEABLE;�����2��CS��WE�ź� */
	MPU_InitStruct.IsShareable      = MPU_ACCESS_NOT_SHAREABLE;
	MPU_InitStruct.Number           = MPU_REGION_NUMBER3;
	MPU_InitStruct.TypeExtField     = MPU_TEX_LEVEL0;
	MPU_InitStruct.SubRegionDisable = 0x00;
	MPU_InitStruct.DisableExec      = MPU_INSTRUCTION_ACCESS_ENABLE;
	HAL_MPU_ConfigRegion(&MPU_InitStruct);
	
	/*ʹ�� MPU */
	HAL_MPU_Enable(MPU_PRIVILEGED_DEFAULT);
}

/*
*********************************************************************************************************
*	�� �� ��: CPU_CACHE_Enable
*	����˵��: ʹ��L1 Cache
*	��    ��: ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
static void CPU_CACHE_Enable(void)
{
	/* ʹ�� I-Cache */
	SCB_EnableICache();

	/* ʹ�� D-Cache */
	SCB_EnableDCache();
}

/*
*********************************************************************************************************
*	�� �� ��: bsp_GetCpuID
*	����˵��: ��CPU UID
*	��    ��: _id : ����ID
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void bsp_GetCpuID(uint32_t *_id)
{
	_id[0] = *(__IO uint32_t*)(0x1FF1E800);
	_id[1] = *(__IO uint32_t*)(0x1FF1E800 + 4);
	_id[2] = *(__IO uint32_t*)(0x1FF1E800 + 8);
}

/*
*********************************************************************************************************
*	�� �� ��: bsp_RunPer10ms
*	����˵��: �ú���ÿ��10ms��Systick�жϵ���1�Ρ���� bsp_timer.c�Ķ�ʱ�жϷ������һЩ����ʱ��Ҫ���ϸ��
*			������Է��ڴ˺��������磺����ɨ�衢���������п��Ƶȡ�
*	��    ��: ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void bsp_RunPer10ms(void)
{
	bsp_KeyScan10ms();
}

/*
*********************************************************************************************************
*	�� �� ��: bsp_RunPer1ms
*	����˵��: �ú���ÿ��1ms��Systick�жϵ���1�Ρ���� bsp_timer.c�Ķ�ʱ�жϷ������һЩ��Ҫ�����Դ���������
*			 ���Է��ڴ˺��������磺��������ɨ�衣
*	��    ��: ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void bsp_RunPer1ms(void)
{
	;
}

/*
*********************************************************************************************************
*	�� �� ��: bsp_Idle
*	����˵��: ����ʱִ�еĺ�����һ����������for��whileѭ������������Ҫ���� CPU_IDLE() �������ñ�������
*			 ������ȱʡΪ�ղ������û���������ι��������CPU��������ģʽ�Ĺ��ܡ�
*	��    ��: ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void bsp_Idle(void)
{
	/* --- ι�� */

	/* --- ��CPU�������ߣ���Systick��ʱ�жϻ��ѻ��������жϻ��� */
}

/*
*********************************************************************************************************
*	�� �� ��: HAL_Delay
*	����˵��: �����ӳٺ������滻HAL�еĺ�������ΪHAL�е�ȱʡ����������systick�жϣ������USB��SD���ж���
*		���ӳٺ��������������
*	��    ��: ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void HAL_Delay(uint32_t Delay)
{
	bsp_DelayUS(Delay * 1000);
}

/*
*********************************************************************************************************
*	�� �� ��: bsp_ResetSystem
*	����˵��: CPU��λ
*	��    ��: ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void bsp_ResetSystem(void)
{
	NVIC_SystemReset();	/* ��λCPU */
}

/*
*********************************************************************************************************
*	�� �� ��: TestGpio_USB3300
*	����˵��: ��תUSB3300��ص�GPIO��������Ӳ��DEBUG
*	��    ��: ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void TestGpio_USB3300(void)
{
	GPIO_InitTypeDef GPIO_InitStruct;
	
	/* Configure USB FS GPIOs */
    __GPIOA_CLK_ENABLE();
    __GPIOB_CLK_ENABLE();
    __GPIOC_CLK_ENABLE();
    __GPIOH_CLK_ENABLE();
    __GPIOI_CLK_ENABLE();

    /* CLK */
    GPIO_InitStruct.Pin = GPIO_PIN_5;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = 0;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    /* D0 */
    GPIO_InitStruct.Pin = GPIO_PIN_3;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = 0;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    /* D1 D2 D3 D4 D5 D6 D7 */
    GPIO_InitStruct.Pin = GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_5 |
      GPIO_PIN_10 | GPIO_PIN_11 | GPIO_PIN_12 | GPIO_PIN_13;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Alternate = 0;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct); 

    /* STP */
    GPIO_InitStruct.Pin = GPIO_PIN_0;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Alternate = 0;
    HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

    /* NXT */
    GPIO_InitStruct.Pin = GPIO_PIN_4;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Alternate = 0;
    HAL_GPIO_Init(GPIOH, &GPIO_InitStruct);

    /* DIR */
    GPIO_InitStruct.Pin = GPIO_PIN_11;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Alternate = 0;
    HAL_GPIO_Init(GPIOI, &GPIO_InitStruct);
	
	while (1)
	{
		GPIOA->BSRR = ((uint32_t)GPIO_PIN_5 << 16U);
		GPIOA->BSRR = ((uint32_t)GPIO_PIN_3 << 16U);
		
		GPIOB->BSRR = ((uint32_t)GPIO_PIN_0 << 16U);
		GPIOB->BSRR = ((uint32_t)GPIO_PIN_1 << 16U);
		GPIOB->BSRR = ((uint32_t)GPIO_PIN_5 << 16U);
		GPIOB->BSRR = ((uint32_t)GPIO_PIN_10 << 16U);
		GPIOB->BSRR = ((uint32_t)GPIO_PIN_11 << 16U);
		GPIOB->BSRR = ((uint32_t)GPIO_PIN_12 << 16U);
		GPIOB->BSRR = ((uint32_t)GPIO_PIN_13 << 16U);
		
		GPIOC->BSRR = ((uint32_t)GPIO_PIN_0 << 16U);
		GPIOH->BSRR = ((uint32_t)GPIO_PIN_4 << 16U);
		GPIOI->BSRR = ((uint32_t)GPIO_PIN_11 << 16U);
		
		bsp_DelayMS(100);

		GPIOA->BSRR = GPIO_PIN_5;
		GPIOA->BSRR = GPIO_PIN_3;
		
		GPIOB->BSRR = GPIO_PIN_0;
		GPIOB->BSRR = GPIO_PIN_1;
		GPIOB->BSRR = GPIO_PIN_5;
		GPIOB->BSRR = GPIO_PIN_10;
		GPIOB->BSRR = GPIO_PIN_11;
		GPIOB->BSRR = GPIO_PIN_12;
		GPIOB->BSRR = GPIO_PIN_13;		
		
		GPIOC->BSRR = GPIO_PIN_0;
		GPIOH->BSRR = GPIO_PIN_4;
		GPIOI->BSRR = GPIO_PIN_11;
		
		bsp_DelayMS(100);		
	}
}

/***************************** ���������� www.armfly.com (END OF FILE) *********************************/