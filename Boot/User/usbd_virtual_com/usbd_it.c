/*
*********************************************************************************************************
*
*	ģ������ : USB device �жϷ������
*	�ļ����� : usb_it.c
*	��    �� : V1.0
*	˵    �� : ���ļ����USB device�жϷ������ֻ�轫���ļ����빤�̼��ɣ������ٵ� stm32f4xx_it.c �������ЩISR����
*
*	�޸ļ�¼ :
*		�汾��  ����        ����     ˵��
*		V1.0    2018-09-08 armfly  ��ʽ����
*
*	Copyright (C), 2018-2030, ���������� www.armfly.com
*
*********************************************************************************************************
*/
/* Includes ------------------------------------------------------------------ */
#include "usbd_user.h"

/** @addtogroup STM32H7xx_HAL_Applications
  * @{
  */

/* Private typedef ----------------------------------------------------------- */
/* Private define ------------------------------------------------------------ */
/* Private macro ------------------------------------------------------------- */
/* Private variables --------------------------------------------------------- */
extern PCD_HandleTypeDef hpcd;

/* UART handler declared in "usbd_cdc_interface.c" file */
extern UART_HandleTypeDef *NowUartHandle;

/* TIM handler declared in "usbd_cdc_interface.c" file */
extern TIM_HandleTypeDef TimHandle;
/* Private function prototypes ----------------------------------------------- */
#ifdef USE_USB_FS
void OTG_FS_IRQHandler(void);
#else
void OTG_HS_IRQHandler(void);
#endif
void USARTx_DMA_TX_IRQHandler(void);
void USARTx_IRQHandler(void);
void TIMx_IRQHandler(void);

/**
  * @brief  This function handles USB-On-The-Go FS/HS global interrupt request.
  * @param  None
  * @retval None
  */
#ifdef USE_USB_FS
void OTG_FS_IRQHandler(void)
#else
void OTG_HS_IRQHandler(void)
#endif
{
  HAL_PCD_IRQHandler(&hpcd);
}

/******************************************************************************/
/* STM32H7xx Peripherals Interrupt Handlers */
/* Add here the Interrupt Handler for the used peripheral(s) (PPP), for the */
/* available peripheral interrupt handler's name please refer to the startup */
/* file (startup_stm32h7xx.s).  */
/******************************************************************************/
/**
  * @brief  This function handles DMA interrupt request.
  * @param  None
  * @retval None
  */
void A_USARTx_DMA_TX_IRQHandler(void)
{
  HAL_DMA_IRQHandler(NowUartHandle->hdmatx);
}

//void B_USARTx_DMA_TX_IRQHandler(void)
//{
//  HAL_DMA_IRQHandler(NowUartHandle->hdmatx);
//}


/**
  * @brief  This function handles UART interrupt request.  
  * @param  None
  * @retval None
  */
void A_USARTx_IRQHandler(void)
{
  HAL_UART_IRQHandler(NowUartHandle);
}

void B_USARTx_IRQHandler(void)
{
  HAL_UART_IRQHandler(NowUartHandle);
}

/**
  * @brief  This function handles TIM interrupt request.
  * @param  None
  * @retval None
  */
void TIMx_IRQHandler(void)
{
  HAL_TIM_IRQHandler(&TimHandle);
}

/**
  * @}
  */

/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
