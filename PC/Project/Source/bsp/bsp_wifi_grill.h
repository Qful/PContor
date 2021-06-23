/*
*********************************************************************************************************
*
*	ģ������ : ��¯״̬�ĳ������壨���ڽ��룩
*	�ļ����� : bsp_wifi_grill.h
*	��    �� : V1.2
*	˵    �� : ͷ�ļ�
*
*********************************************************************************************************
*/

#ifndef __BSP_WIFI_GRILL_H
#define __BSP_WIFI_GRILL_H

enum POWER_MODE_EN
{
	POWER_MODE_OFF = 0,		/* �ر�ģʽ */
	POWER_MODE_ON,
	POWER_MODE_FAN,
	POWER_MODE_REMAIN,
};

// 0:����  1:�ɹ�   2�����֮0����3�����֮1���� 4:���֮2����5:���֮3����6�����ʧ�ܣ�7,����֮��
enum FIRE_STATE_EN
{
	FIRE_STATE_OFF = 0,
	FIRE_STATE_ON,
	FIRE_STATE_STEP_0,
	FIRE_STATE_STEP_1,
	FIRE_STATE_STEP_2,
	FIRE_STATE_STEP_3,
	FIRE_STATE_FAIL,
	FIRE_STATE_FAN,
};

enum FIRE_STATE_EN2
{
	FIRE2_STATE_DEFAULT = 0,
	FIRE2_STATE_OFF,
	FIRE2_STATE_STARTUP,
	FIRE2_STATE_RUNNING,
	FIRE2_STATE_COOLDOWN,
	FIRE2_STATE_FAIL,
};


#endif

/***************************** ���������� www.armfly.com (END OF FILE) *********************************/
