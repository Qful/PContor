/*
*********************************************************************************************************
*
*	ģ������ : GUI��ɫģ��
*	�ļ����� : gui_conf.h
*	��    �� : V1.0
*
*	Copyright (C), 2018-2025, ���������� www.armfly.com
*
********************************************************************************************************
*/

#ifndef _GUI_CONF_H_
#define _GUI_CONF_H_

#define SETTING_TIMEOUT			30			/* �˵�������ʱ */
#define LCD_WAKE_UP_TIMEOUT		180			/* LCD���⻽�ѳ�ʱ 3���� */

#define MAIN_BACK_COLOR  RGB(5,54,131)
#define MAIN_TEXT_COLOR  CL_WHITE

#define WIN_GRID_COLOR   CL_WHITE			/* ���δ���դ����ɫ */

#define WIN_BACK_COLOR	MAIN_BACK_COLOR		/* ���δ��ڱ�����ɫ */


#define WIN_WAVE_COLOR	CL_YELLOW			/* ����������ɫ */

#define SURGE_WIN_BACK_COLOR	RGB(193,227,148)	/* SURGE���ڱ���ɫ */
#define SURGE_TITLE_TEXT_COLOR	RGB(0,0,0)			/* SURGE����������ɫ */
#define SURGE_REC_TEXT1_COLOR	RGB(0,0,0)			/* SURGE ��¼��1��������ɫ */
#define SURGE_REC_TEXT2_COLOR	RGB(73,65,73)		/* SURGE ��¼������������ɫ */

#define ALARM_WIN_BACK_COLOR	RGB(255,189,89)		/* ALARM ���ڱ���ɫ */
#define ALARM_TITLE_TEXT_COLOR	RGB(0,0,0)			/* ALARM ����������ɫ */
#define ALARM_REC_TEXT1_COLOR	RGB(0,0,0)			/* ALARM ��¼��1��������ɫ */
#define ALARM_REC_TEXT2_COLOR	RGB(73,65,73)		/* ALARM ��¼������������ɫ */

#define CLOCK_WIN_BACK_COLOR	RGB(224,243,199)	/* ʱ�Ӵ��ڱ�����ɫ */
#define CLOCK_TEXT_COLOR		RGB(73,65,73)		/* ʱ��������ɫ */

/* IO������ɫ */
#define IO_WIN_BACK_COLOR		RGB(178,191,221)
#define IO_WIN_TEXT_COLOR		RGB(51,51,51)
#define IO_DISABLE_TEXT_COLOR	RGB(100,100,100)		/* ͨ����Ч����ɫ */

#define RES_WIN_BACK_COLOR		RGB(252,252,252)	/* ����������״̬���ڱ�����ɫ */
#define RES_TEXT_COLOR			RGB(73,65,73)		/* ����������״̬����������ɫ */
#define RES_ERR_TEXT_COLOR		RGB(255,0,0)		/* ����ʱ�ı���ɫ */

#define DEL_REC_WIN_BACK_COLOR	RGB(224,243,199)	/* ɾ����¼���ڱ�����ɫ */
#define DEL_REC_TEXT_COLOR		RGB(255,0,0)		/* ɾ���ı���ɫ */

#define TRIGER_WIN_BACK_COLOR	RGB(224,243,199)	/* �����������ڱ�����ɫ */
#define TRIGER_TEXT_COLOR		RGB(73,65,73)		/* ���������ı���ɫ */

#define	BAUD_WIN_BACK_COLOR		TRIGER_WIN_BACK_COLOR	/* �����ʴ��ڱ��� */
#define BAUD_TEXT_COLOR 		TRIGER_TEXT_COLOR		/* ���������� */

#define ADDR_WIN_BACK_COLOR 	TRIGER_WIN_BACK_COLOR		/* ��ַ�������� */
#define ADDR_TEXT_COLOR 		TRIGER_TEXT_COLOR		/* ��ַ���� */

#define TEMP_WIN_BACK_COLOR		RGB(224,243,199)	/* �¶ȴ��ڱ�����ɫ */
#define TEMP_TEXT_COLOR			RGB(73,65,73)		/* �¶ȵ����ı���ɫ */

/* �˵���ɫ */
#define MENU_WIN_BACK_COLOR 	IO_WIN_BACK_COLOR			/* �˵����ڱ���ɫ */
#define MENU_TEXT_COLOR 		IO_WIN_TEXT_COLOR			/* �˵�������ɫ */

#define TOP_BAR_BACK_COLOR		MAIN_BACK_COLOR			/* �ײ�BAR������ɫ */
#define TOP_BAR_TEXT_COLOR		MAIN_TEXT_COLOR			/* �ײ�BAR������ɫ */

#define BOTTOM_BAR_BACK_COLOR		MAIN_BACK_COLOR			/* �ײ�BAR������ɫ */
#define BOTTOM_BAR_TEXT_COLOR		MAIN_TEXT_COLOR			/* �ײ�BAR������ɫ */


/* ���岨�δ��ڵĴ�С��ƫ��λ�� */
#define DOT_Y		4		/* ÿС����5���� */
#define DOT_X		4		/* ÿС����10���� */

#define GRID_Y		(DOT_Y * 10)		/* Y�ᣬ��դ��25���أ��ڷ�5��С�� */
#define GRID_X		(DOT_X * 10)		/* X�ᣬ��դ��50����, �ڷ�5��С�� */

#define WIN_LEFT	26
#define WIN_TOP		18

#define WIN_WIDTH	(GRID_X * 8)
#define WIN_HEIGHT	(GRID_Y * 6)

/*��ͨ������- �������� */
#define IO_RECT1_LEFT		0
#define IO_RECT1_TOP		0
#define IO_RECT1_WIDTH		WIN_WIDTH+WIN_LEFT+1
#define IO_RECT1_HEIGHT 	WIN_HEIGHT+WIN_TOP+32

/*��ͨ������- �������� */
#define IO_RECT2_LEFT		IO_RECT1_WIDTH
#define IO_RECT2_TOP		TRIGER_TOP + TRIGER_HEIGHT+1
#define IO_RECT2_WIDTH		TRIGER_WIDTH
#define IO_RECT2_HEIGHT 	40

/*��ͨ������- ������ʾ��Ϣ�� */
#define IO_WIN_LEFT			5
#define IO_WIN_TOP			(WIN_TOP - 1)
#define IO_WIN_WIDTH		(SURGE_LEFT - 10)
#define IO_WIN_HEIGHT 		225

#define IO_TEXT_LEFT		(IO_WIN_LEFT + 10)
#define IO_TEXT_TOP			(IO_WIN_TOP + 10)

#define IO_TEXT_LEFT2		(IO_WIN_LEFT + 5)
#define IO_TEXT_LEFT3		(IO_WIN_LEFT + (IO_WIN_WIDTH - 10) / 2)

#define IO_TEXT_Y_CAP		24

/* �ӵص��� */
#define RES_VALUE_LEFT		5
#define RES_VALUE_TOP		252

#define RES_INFO_LEFT		RES_VALUE_LEFT
#define RES_INFO_TOP		272

/* ����SURGE ���ڵĴ�С��ƫ��λ�� */
#define SURGE_LEFT		(WIN_LEFT + WIN_WIDTH + 5)
#define SURGE_TOP		(WIN_TOP - 1)
#define SURGE_WIDTH		125
#define SURGE_HEIGHT 	134

#define SURGE_LINE_CAP1	15
#define SURGE_LINE_CAP2	14

/* ���� ALARM���ڵĴ�С��ƫ��λ�� */
#define ALARM_LEFT		SURGE_LEFT
#define ALARM_TOP		(SURGE_TOP + SURGE_HEIGHT + 4)
#define ALARM_WIDTH		SURGE_WIDTH
#define ALARM_HEIGHT 	74

#define TRIGER_LEFT		370
#define TRIGER_TOP		250
#define TRIGER_WIDTH	105
#define TRIGER_HEIGHT 	20

#define ADDR_LEFT		TRIGER_LEFT
#define ADDR_TOP		(TRIGER_TOP + 23)
#define ADDR_WIDTH		TRIGER_WIDTH
#define ADDR_HEIGHT 	20

#define BAUD_LEFT		TRIGER_LEFT
#define BAUD_TOP		(TRIGER_TOP + 23 * 2)
#define BAUD_WIDTH		TRIGER_WIDTH
#define BAUD_HEIGHT 	20

#define CLOCK_LEFT		5
#define CLOCK_TOP		(320 - 24)
#define CLOCK_WIDTH		160
#define CLOCK_HEIGHT 	20

#define RESLINK_LEFT	5
#define RESLINK_TOP		(IO_WIN_TOP + IO_WIN_HEIGHT + 5)
#define RESLINK_WIDTH	160
#define RESLINK_HEIGHT 	20

#define TMODE_LEFT		(CLOCK_LEFT + CLOCK_WIDTH + 10)
#define TMODE_TOP		CLOCK_TOP
#define TMODE_WIDTH		20
#define TMODE_HEIGHT 	20

#define WMODE_LEFT		(TMODE_LEFT + TMODE_WIDTH + 10)
#define WMODE_TOP		CLOCK_TOP
#define WMODE_WIDTH		20
#define WMODE_HEIGHT 	20

// -125.1��
#define TEMP_LEFT		(WMODE_LEFT + WMODE_WIDTH + 20)
#define TEMP_TOP		(CLOCK_TOP + 3)
#define TEMP_WIDTH		64
#define TEMP_HEIGHT 	16

#define TOP_BAR_LEFT		0
#define TOP_BAR_TOP			0
#define TOP_BAR_HEIGHT		24
#define TOP_BAR_WIDTH		480

#define BOTTOM_BAR_LEFT		0
#define BOTTOM_BAR_TOP		(320 - BOTTOM_BAR_HEIGHT)
#define BOTTOM_BAR_HEIGHT	24
#define BOTTOM_BAR_WIDTH	480

/* ϵͳ�������ұ߲����� */
#define SYS_BOX_LEFT	150
#define SYS_BOX_TOP		TOP_BAR_HEIGHT
#define SYS_BOX_HEIGHT	(320 - TOP_BAR_HEIGHT - BOTTOM_BAR_HEIGHT)
#define SYS_BOX_WIDTH	(480 - SYS_BOX_LEFT)
#define SYS_BOX_BACK_COLOR	IO_WIN_BACK_COLOR

void FillTopBar(uint16_t _color);
void FillBottomBar(uint16_t _color);
void FillMidRect(uint16_t _color);
void DispStrInTopBar(char * _str);
void DispStrInBottomBar(char * _str);


#endif

/***************************** ���������� www.armfly.com (END OF FILE) *********************************/

