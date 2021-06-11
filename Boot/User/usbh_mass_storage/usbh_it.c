/*
*********************************************************************************************************
*
*	模块名称 : USB host中断服务程序
*	文件名称 : usbh_it.c
*	版    本 : V1.0
*	说    明 : 本文件存放USB host中断服务程序。只需将该文件加入工程即可，无需再到 stm32f4xx_it.c 中添加这些ISR程序
*
*	修改记录 :
*		版本号  日期        作者     说明
*		V1.0    2018-09-08  armfly  正式发布
*
*	Copyright (C), 2018-2030, 安富莱电子 www.armfly.com
*
*********************************************************************************************************
*/

#include "usbh_core.h"

extern HCD_HandleTypeDef hhcd;

/*
*********************************************************************************************************
*	函 数 名: OTG_FS_IRQHandler, OTG_HS_IRQHandler
*	功能说明: USB HOST中断服务程序
*	形    参: 无
*	返 回 值: 无
*********************************************************************************************************
*/
#ifdef USE_USBH_HS
void OTG_HS_IRQHandler(void)
{
	HAL_HCD_IRQHandler(&hhcd);
}
#endif
/***************************** 安富莱电子 www.armfly.com (END OF FILE) *********************************/
