/*
*********************************************************************************************************
*	                                  
*	模块名称 : CAN网络通信程序
*	文件名称 : can_network.c
*	版    本 : V1.0
*	说    明 : 实现多个CAN节点（节点的程序相同）之间的通信。
*	修改记录 :
*		版本号  日期       作者    说明
*		v1.0    2015-08-09 armfly  ST固件库V3.5.0版本。
*
*	Copyright (C), 2015-2016, 安富莱电子 www.armfly.com
*
*********************************************************************************************************
*/

/*
	应用层协议:（自定义简单协议）	
	01  01 01 --- 控制LED指示灯点亮， 第1个字节是命令代码，第2个字节表示00表示关闭，01表示点亮。
			    第3个字节表示指示灯序号(1-4)
	02  00    --- 控制蜂鸣器，第1个字节表示命令代码，第2个字节 00 表示关闭，01表示持续鸣叫， 02表示鸣叫1次
*/

#include "bsp.h"
#include "can_network.h"

uint8_t g_ucLedNo = 0;	/* 点亮的LED灯序号，0-3 */

/*
*********************************************************************************************************
*	函 数 名: can_LedOn
*	功能说明: 通过CAN1发送指令，控制LED点亮
*	形    参: _addr 设备地址 0 表示广播的
*			  _led_no ： LED指示灯序号
*	返 回 值: 无
*********************************************************************************************************
*/
void can_LedOn(uint8_t _addr, uint8_t _led_no)
{		
	uint8_t tx_buf[8];
						
	tx_buf[0] = 0x01;
	tx_buf[1] = 0x01;	/* 点亮 */
	tx_buf[2] = _led_no;
    can1_SendPacket(tx_buf, 3);
}

/*
*********************************************************************************************************
*	函 数 名: can_LedOff
*	功能说明: 通过CAN1发送指令，CAN2执行命令，控制LED关闭。
*	形    参: _addr 设备地址 0 表示广播的
*			  _led_no ： LED指示灯序号
*	返 回 值: 无
*********************************************************************************************************
*/
void can_LedOff(uint8_t _addr, uint8_t _led_no)
{		
	uint8_t tx_buf[8];
						
	tx_buf[0] = 0x01;
	tx_buf[1] = 0x00;	/* 关闭 */
	tx_buf[2] = _led_no;
    can1_SendPacket(tx_buf, 3);	
}

/*
*********************************************************************************************************
*	函 数 名: can_BeepCtrl
*	功能说明: 通过CAN2发送指令，CAN1执行命令，控制蜂鸣器
*	形    参: _addr 设备地址 0 表示广播的
*			  _cmd ： LED指示灯序号
*	返 回 值: 无
*********************************************************************************************************
*/
void can_BeepCtrl(uint8_t _addr, uint8_t _cmd)
{		
	uint8_t tx_buf[8];
						
	tx_buf[0] = 0x01;
	tx_buf[1] = _cmd;		/* 第2个字节 00 表示关闭，01表示持续鸣叫， 02表示鸣叫1次 */
    can1_SendPacket(tx_buf, 2);
}

/*
*********************************************************************************************************
*	函 数 名: can_Init
*	功能说明: 配置CAN硬件
*	形    参: 无
*	返 回 值: 无
*********************************************************************************************************
*/
void can_Init(void)
{
	bsp_InitCan1();
	bsp_InitCan2();
}     

/*
*********************************************************************************************************
*	函 数 名: can_DeInit
*	功能说明: 退出CAN硬件硬质，恢复CPU相关的GPIO为缺省；关闭CAN中断
*	形    参: 无
*	返 回 值: 无
*********************************************************************************************************
*/
void can_DeInit(void)
{	
	bsp_DeInitCan1();
	bsp_DeInitCan2();
}
	
/*
*********************************************************************************************************
*	函 数 名: can1_Analyze
*	功能说明: 解析CAN1收到的数据. 执行蜂鸣器动作
*	形    参: 无
*	返 回 值: 无
*********************************************************************************************************
*/
void can1_Analyze(void)
{		
	if (g_Can1RxHeader.DataLength == 3)
	{
		if (g_Can1RxData[0] == 0x02)
		{
			if (g_Can1RxData[1] == 0)
			{
				BEEP_Stop();		/* 关闭蜂鸣器 */
			}
			else
			{
				BEEP_Start(8, 8, g_Can1RxData[1]);	/* 鸣叫80ms，停80ms， n次 */	
			}	
		}
	}
}

/*
*********************************************************************************************************
*	函 数 名: can2_Analyze
*	功能说明: 解析CAN2收到的数据。执行LED动作
*	形    参: 无
*	返 回 值: 无
*********************************************************************************************************
*/
void can2_Analyze(void)
{
	if (g_Can2RxHeader.DataLength == 2)
	{
		if (g_Can1RxData[0] == 0x01)
		{
			if (g_Can1RxData[1] == 0)
			{
				bsp_LedOff(g_Can1RxData[2]);	/* 关闭LED */
			}
			else
			{
				bsp_LedOn(g_Can1RxData[2]);	/* 打开LED */	
			}
		}
	}
}

/***************************** 安富莱电子 www.armfly.com (END OF FILE) *********************************/


