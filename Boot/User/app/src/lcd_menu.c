/*
*********************************************************************************************************
*
*	ģ������ : LCDҺ���˵�(���̿��ƣ�
*	�ļ����� : lcd_menu.c
*	��    �� : V1.0
*	˵    �� : ��
*	�޸ļ�¼ :
*		�汾��  ����       ����    ˵��
*		v1.0    2015-04-25 armfly  ST�̼���汾 V2.1.0
*
*	Copyright (C), 2014-2015, ���������� www.armfly.com
*
*********************************************************************************************************
*/

#include "bsp.h"
#include "fonts.h"
#include "ui_def.h"
#include "lcd_menu.h"

/*
*********************************************************************************************************
*	�� �� ��: LCD_InitMenu
*	����˵��: ����16�����ֳ�ʼ���˵��ṹ
*	��    ��: _pMenu : �˵��ṹ��ָ��; ������������ȫ�ֲ˵��ṹ�������
*			 _Text : �˵��ı�������ָ�롣 �����������ռ䱣������
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void LCD_InitMenu(MENU_T *_pMenu, char **_Text)
{
  	uint8_t i;
	
	_pMenu->Text = (uint8_t **)_Text;			/* �˵��ı� */
	
	for (i = 0; i < 255; i++)
	{
		if (_pMenu->Text[i][0] == '&')
		{
			_pMenu->Count = i;
			break;
		}
	}
	
	_pMenu->Cursor = 0;		/* ��ǰ��Ļ��1�ж�Ӧ������ */
	_pMenu->Offset = 0;		/* ѡ���е����� */
}

/*
*********************************************************************************************************
*	�� �� ��: LCD_DispMenu16
*	����˵��: ��ʾ�˵� 16�����֡� �׵ױ�ʾѡ��
*	��    ��: _pMenu : �˵���������
*			  _Count : �˵������
*			  _Cursor : �����
*			  _FocusLine :  ������(0-3)
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void LCD_DispMenu(MENU_T *_pMenu)
{
	uint8_t i;
	uint8_t FontHeight;
	uint16_t  y;
	uint8_t line_cap1, line_cap2;
		
	FontHeight = LCD_GetFontWidth(&_pMenu->Font);

	line_cap1 = _pMenu->LineCap / 2;			/* �˵��ı�ǰ�ĸ߶� */
	line_cap2 = _pMenu->LineCap - line_cap1;	/* �˵��ı���ĸ߶� */
	for (i = 0; i < _pMenu->ViewLine; i++)
	{
		if (i >= _pMenu->Count)
		{
			break;
		}
		
  		if (i + _pMenu->Offset == _pMenu->Cursor)
		{  		
			/* ����Ϊ���� */
			_pMenu->Font.FrontColor = CL_MENU_TEXT2;
			_pMenu->Font.BackColor = CL_MENU_BACK2;
		}
		else
		{
			/* �ָ�������ɫ */
			_pMenu->Font.FrontColor = CL_MENU_TEXT1;
			_pMenu->Font.BackColor = CL_MENU_BACK1;
		}
		
		y = _pMenu->Top + i * (FontHeight + _pMenu->LineCap);
		
		/* ���ǰ���� */
		LCD_Fill_Rect(_pMenu->Left, y,  line_cap1, _pMenu->Width, _pMenu->Font.BackColor);
		
		/* ˢ���ı� */
		LCD_DispStrEx(_pMenu->Left, y + line_cap1, (char *)_pMenu->Text[_pMenu->Offset + i], &_pMenu->Font, 
			_pMenu->Width, ALIGN_LEFT);
		
		/* ��κ󱳾� */
		LCD_Fill_Rect(_pMenu->Left, y + line_cap1 + FontHeight, line_cap2, _pMenu->Width, _pMenu->Font.BackColor);
	}
}

/*
*********************************************************************************************************
*	�� �� ��: LCD_MoveDownMenu
*	����˵��: �����ƶ�ѡ�еĲ˵���, ��ˢ����ʾ.
*	��    ��: _pMenu : �˵��ṹ��ָ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void LCD_MoveDownMenu(MENU_T *_pMenu)
{
	if (_pMenu->Cursor < _pMenu->Count - 1)
	{
		_pMenu->Cursor++;

		if (_pMenu->Cursor - _pMenu->Offset >= _pMenu->ViewLine)
		{
			_pMenu->Offset++;
		}
		
		LCD_DispMenu(_pMenu);	/* ˢ����ʾ */
	}
	else	
	{
		_pMenu->Cursor = 0;
		_pMenu->Offset = 0;
		LCD_DispMenu(_pMenu);	/* ˢ����ʾ */
	}	
}

/*
*********************************************************************************************************
*	�� �� ��: LCD_MoveUpMenu
*	����˵��: �����ƶ�ѡ�еĲ˵���, ��ˢ����ʾ.
*	��    ��: _pMenu : �˵��ṹ��ָ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void LCD_MoveUpMenu(MENU_T *_pMenu)
{
	if (_pMenu->Cursor > 0)
	{
		_pMenu->Cursor--;

		if (_pMenu->Cursor < _pMenu->Offset)
		{
			_pMenu->Offset--;
		}

		LCD_DispMenu(_pMenu);	/* ˢ����ʾ */						
	}	
	else	
	{
		_pMenu->Cursor = _pMenu->Count - 1;
		if (_pMenu->Count >  _pMenu->ViewLine)
		{
			_pMenu->Offset = _pMenu->Count - _pMenu->ViewLine;
		}
		else
		{
			_pMenu->Offset = 0;
		}
		LCD_DispMenu(_pMenu);	/* ˢ����ʾ */
	}
}

/***************************** ���������� www.armfly.com (END OF FILE) *********************************/
