/*
*********************************************************************************************************
*
*	ģ������ : LCDҺ����ʾ�� �˵����
*	�ļ����� : lcd_menu.h
*	��    �� : V1.0
*	˵    �� : ͷ�ļ�
*	�޸ļ�¼ :
*		�汾��  ����       ����    ˵��
*		v1.0    2015-04-25 armfly  ST�̼���汾 V2.1.0
*
*	Copyright (C), 2015-2016, ���������� www.armfly.com
*
*********************************************************************************************************
*/

#ifndef _LCD_MENU_H
#define _LCD_MENU_H

#define CL_MENU_TEXT1	CL_WHITE
#define CL_MENU_BACK1	MAIN_BACK_COLOR

/* ���ѡ�� */
#define CL_MENU_TEXT2	CL_BLACK
#define CL_MENU_BACK2	CL_YELLOW

/* �˵�ѡ�У���������һ����ɫ */
#define CL_MENU_TEXT3	CL_BLACK
#define CL_MENU_BACK3	RGB(142,209,158)


#define MENU_LEFT		(0 + 5)
#define MENU_TOP		(0 + 5)
#define MENU_WIDTH		(240 - 10)
#define MENU_HEIGHT 	(240 - 50)
#define MENU_CAP		26

/* �˵��ṹ */
typedef struct
{
	uint8_t **Text;		/* �˵��ı� */
	uint8_t ViewLine;	/* �������� */
	uint8_t Count;		/* �˵������ */
	uint8_t Offset;		/* ��ǰ��Ļ��1�ж�Ӧ������ */
	uint8_t Cursor;		/* ѡ���е����� */
	
	uint16_t Left;		/* X ���� */
	uint16_t Top;		/* Y ���� */
	uint16_t Height;	/* �߶� */
	uint16_t Width;		/* ��� */
	uint16_t LineCap;	/* �м�� */
	
	FONT_T Font;		/* ���� */
}MENU_T;

/* �˵���ʾ�� */
void LCD_InitMenu(MENU_T *_pMenu, char **_Text);
void LCD_DispMenu(MENU_T *_pMenu);
void LCD_MoveDownMenu(MENU_T *_pMenu);
void LCD_MoveUpMenu(MENU_T *_pMenu);

#endif

/***************************** ���������� www.armfly.com (END OF FILE) *********************************/
