/*
*********************************************************************************************************
*
*	ģ������ : TFTҺ����ʾ������ģ��
*	�ļ����� : bsp_tft_lcd.c
*	��    �� : V4.2
*	˵    �� : ֧��3.0�� 3.5�� 4.3�� 5.0�� 7.0����ʾģ��.
*			  3.0���֧�ֵ�LCD�ڲ�����оƬ�ͺ���: SPFD5420A��OTM4001A��R61509V
*	�޸ļ�¼ :
*		�汾��  ����       ����    ˵��
*		v1.0    2011-08-21 armfly  ST�̼���汾 V3.5.0�汾��
*					a) ȡ�����ʼĴ����Ľṹ�壬ֱ�Ӷ���
*		V2.0    2011-10-16 armfly  ����R61509V������ʵ��ͼ����ʾ����
*		V2.1    2012-07-06 armfly  ����RA8875������֧��4.3����
*		V2.2    2012-07-13 armfly  �Ľ�LCD_DispStr������֧��12�����ַ�;�޸�LCD_DrawRect,�����һ����������
*		V2.3    2012-08-08 armfly  ���ײ�оƬ�Ĵ���������صĺ����ŵ��������ļ���֧��RA8875
*   	V3.0    2013-05-20 ����ͼ��ṹ; �޸�	LCD_DrawIconActive  �޸�DispStr����֧���ı�͸��
*		V3.1    2013-06-12 ���LCD_DispStr()����BUG�������Ƕ�ֿ��к��ָ�������256���������ѭ����
*		V3.2    2013-06-28 ����Label�ؼ�, ����ʾ�ַ�����֮ǰ��ʱ���Զ�������������
*		V3.3    2013-06-29 FSMC��ʼ��ʱ������ʱ��дʱ��Ͷ�ʱ��ֿ����á� LCD_FSMCConfig ������
*		V3.4    2013-07-06 ������ʾ32λ��Alphaͼ��ĺ��� LCD_DrawIcon32
*		V3.5    2013-07-24 ������ʾ32λ��AlphaͼƬ�ĺ��� LCD_DrawBmp32
*		V3.6    2013-07-30 �޸� DispEdit() ֧��12�����ֶ���
*		V3.7    2014-09-06 �޸� LCD_InitHard() ͬʱ֧�� RA8875-SPI�ӿں�8080�ӿ�
*		V3.8    2014-09-15 �������ɺ���:
*					��1�� LCD_DispStrEx() �����Զ������Զ���׵���ʾ�ַ�������
*					��2�� LCD_GetStrWidth() �����ַ��������ؿ��
*		V3.9    2014-10-18
*					(1) ���� LCD_ButtonTouchDown() LCD_ButtonTouchRelease �жϴ������겢�ػ水ť
*					(2) ����3.5��LCD����
*					(3) ���� LCD_SetDirection() ������������ʾ�����򣨺��� ������̬�л���
*		V4.0   2015-04-04 
*				(1) ��ť���༭��ؼ�����RA8875���壬��Ƕ�ֿ��RA8875�ֿ�ͳһ���롣����������� 
*				    FC_RA8875_16, FC_RA8875_24,	FC_RA8875_32
*				(2) FONT_T�ṹ���ԱFontCode�������� uint16_t �޸�Ϊ FONT_CODE_Eö�٣����ڱ��������;
*				(3) �޸� LCD_DispStrEx(), �����������������������_LCD_ReadAsciiDot(), _LCD_ReadHZDot()
*				(4) LCD_DispStr() �����򻯣�ֱ�ӵ��� LCD_DispStrEx() ʵ�֡�
*				(5) LCD_DispStrEx() ����֧�� RA8875���塣
*				(6) LCD_ButtonTouchDown() ���Ӱ�����ʾ��
*		V4.1   2015-04-18 
*				(1) ���RA885 ASCII����Ŀ�ȱ�LCD_DispStrEx() ��������֧��RA8875 ASCII�䳤��ȼ��㡣
*				(2) ��� LCD_HardReset(��������֧��LCD��λ��GPIO���ƵĲ�Ʒ��STM32-V5 ����ҪGPIO���ơ�
*		V4.2   2015-07-23
*				(1) ��Ӻ���LCD_InitButton()
*				(2) h�ļ���ʹ�ܰ�����ʾ�� #define BUTTON_BEEP()	BEEP_KeyTone();
*		V4.8   2019-03-23
*				(1) ���� LCD_DispStrEx0
*	Copyright (C), 2015-2016, ���������� www.armfly.com
*
*********************************************************************************************************
*/

#include "bsp.h"
#include "fonts.h"

#define USE_RA8875

/* ����3����������Ҫ����ʹ����ͬʱ֧�ֲ�ͬ���� */
uint16_t g_LcdHeight = 128;			/* ��ʾ���ֱ���-�߶� */
uint16_t g_LcdWidth = 128;			/* ��ʾ���ֱ���-��� */
uint8_t s_ucBright;					/* �������Ȳ��� */
uint8_t g_LcdDirection = 0;				/* ��ʾ����.0��1��2��3 */

static void LCD_HardReset(void);
static void LCD_SetPwmBackLight(uint8_t _bright);
static void LCD_DispStrEx0(uint16_t _usX, uint16_t _usY, char *_ptr, FONT_T *_tFont, uint16_t _Width,
	uint8_t _Align);

#if 1
	#define LCDX_InitHard			ST7789_InitHard
	#define LCDX_GetChipDescribe	ST7789_GetChipDescribe
	#define LCDX_ClrScr				ST7789_ClrScr
	#define LCDX_PutPixel			ST7789_PutPixel
	#define LCDX_GetPixel			ST7789_GetPixel
	#define LCDX_DrawLine			ST7789_DrawLine
	#define LCDX_DrawRect			ST7789_DrawRect
	#define LCDX_FillRect			ST7789_FillRect
	#define LCDX_DrawCircle			ST7789_DrawCircle
	#define LCDX_DrawBMP			ST7789_DrawBMP
	#define LCDX_QuitWinMode		ST7789_QuitWinMode
	#define LCDX_SetDirection		ST7789_SetDirection
#else
	#define LCDX_InitHard			ST7735_InitHard
	#define LCDX_GetChipDescribe	ST7735_GetChipDescribe
	#define LCDX_ClrScr				ST7735_ClrScr
	#define LCDX_PutPixel			ST7735_PutPixel
	#define LCDX_GetPixel			ST7735_GetPixel
	#define LCDX_DrawLine			ST7735_DrawLine
	#define LCDX_DrawRect			ST7735_DrawRect
	#define LCDX_FillRect			ST7735_FillRect
	#define LCDX_DrawCircle			ST7735_DrawCircle
	#define LCDX_DrawBMP			ST7735_DrawBMP
	#define LCDX_QuitWinMode		ST7735_QuitWinMode
	#define LCDX_SetDirection		ST7735_SetDirection
#endif

/*
*********************************************************************************************************
*	�� �� ��: LCD_InitHard
*	����˵��: ��ʼ��LCD
*	��    ��: ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void LCD_InitHard(void)
{
	LCD_HardReset();	/* Ӳ����λ ��STM32-V5, V6 ���裩���������GPIO����LCD��λ�Ĳ�Ʒ */

	LCDX_InitHard();
	
	LCD_SetDirection(0);

	LCD_ClrScr(CL_BLACK);	/* ��������ʾȫ�� */

	LCD_SetBackLight(BRIGHT_DEFAULT);	 /* �򿪱��⣬����Ϊȱʡ���� */
}


/*
*********************************************************************************************************
*	�� �� ��: LCD_SetPwmBackLight
*	����˵��: ��ʼ������LCD�������GPIO,����ΪPWMģʽ��
*			���رձ���ʱ����CPU IO����Ϊ��������ģʽ���Ƽ�����Ϊ������������������͵�ƽ)����TIM3�ر� ʡ��
*	��    ��:  _bright ���ȣ�0����255������
*	�� �� ֵ: ��
*********************************************************************************************************
*/
static void LCD_SetPwmBackLight(uint8_t _bright)
{
	/* ������CPU���PWM���ƣ�PA0/TIM5_CH1/TIM2_CH1 */
	//bsp_SetTIMOutPWM(GPIOA, GPIO_PIN_0, TIM5, 1, 100, (_bright * 10000) /255);
	//bsp_SetTIMOutPWM(GPIOA, GPIO_PIN_0, TIM5, 1, 20000, (_bright * 10000) /255);
	bsp_SetTIMOutPWM(GPIOH, GPIO_PIN_9, TIM12, 2, 20000, (_bright * 10000) /255);
}

/*
*********************************************************************************************************
*	�� �� ��: LCD_SetBackLight
*	����˵��: ��ʼ������LCD�������GPIO,����ΪPWMģʽ��
*			���رձ���ʱ����CPU IO����Ϊ��������ģʽ���Ƽ�����Ϊ������������������͵�ƽ)����TIM3�ر� ʡ��
*	��    ��: _bright ���ȣ�0����255������
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void LCD_SetBackLight(uint8_t _bright)
{
	s_ucBright =  _bright;	/* ���汳��ֵ */

	LCD_SetPwmBackLight(s_ucBright);
}

/*
*********************************************************************************************************
*	�� �� ��: LCD_GetBackLight
*	����˵��: ��ñ������Ȳ���
*	��    ��: ��
*	�� �� ֵ: �������Ȳ���
*********************************************************************************************************
*/
uint8_t LCD_GetBackLight(void)
{
	return s_ucBright;
}

/*
*********************************************************************************************************
*	�� �� ��: LCD_HardReset
*	����˵��: Ӳ����λ. ��Ը�λ������GPIO���ƵĲ�Ʒ��
*	��    ��: ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void LCD_HardReset(void)
{
#if 0	
	GPIO_InitTypeDef GPIO_InitStructure;

	/* ʹ�� GPIOʱ�� */
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);
	
	/* ���ñ���GPIOΪ�������ģʽ */
	GPIO_InitStructure.GPIO_Pin = GPIO_PIN_1;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_NOPULL;
	GPIO_Init(GPIOB, &GPIO_InitStructure);

	GPIO_ResetBits(GPIOB, GPIO_PIN_1);
	bsp_DelayMS(20);
	GPIO_SetBits(GPIOB, GPIO_PIN_1);
#endif	
}

/*
*********************************************************************************************************
*	�� �� ��: LCD_SetDirection
*	����˵��: ������ʾ����ʾ���򣨺��� ������
*	��    ��: ��ʾ������� 0 ��������, 1=����180�ȷ�ת, 2=����, 3=����180�ȷ�ת
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void LCD_SetDirection(uint8_t _dir)
{
	g_LcdDirection =  _dir;		/* ������ȫ�ֱ��� */

	LCDX_SetDirection(_dir);
}

/*
*********************************************************************************************************
*	�� �� ��: LCD_GetChipDescribe
*	����˵��: ��ȡLCD����оƬ���������ţ�������ʾ
*	��    ��: char *_str : �������ַ�������˻�����
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void LCD_GetChipDescribe(char *_str)
{	
	LCDX_GetChipDescribe(_str);
}

/*
*********************************************************************************************************
*	�� �� ��: LCD_GetHeight
*	����˵��: ��ȡLCD�ֱ���֮�߶�
*	��    ��: ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
uint16_t LCD_GetHeight(void)
{
	return g_LcdHeight;
}

/*
*********************************************************************************************************
*	�� �� ��: LCD_GetWidth
*	����˵��: ��ȡLCD�ֱ���֮���
*	��    ��: ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
uint16_t LCD_GetWidth(void)
{
	return g_LcdWidth;
}

/*
*********************************************************************************************************
*	�� �� ��: LCD_DispOn
*	����˵��: ����ʾ
*	��    ��: ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void LCD_DispOn(void)
{
	;
}

/*
*********************************************************************************************************
*	�� �� ��: LCD_DispOff
*	����˵��: �ر���ʾ
*	��    ��: ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void LCD_DispOff(void)
{
	;
}

/*
*********************************************************************************************************
*	�� �� ��: LCD_ClrScr
*	����˵��: �����������ɫֵ����
*	��    ��: _usColor : ����ɫ
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void LCD_ClrScr(uint16_t _usColor)
{
	LCDX_ClrScr(_usColor);
}

/*
*********************************************************************************************************
*	�� �� ��: LCD_DispStr
*	����˵��: ��LCDָ�����꣨���Ͻǣ���ʾһ���ַ���
*	��    ��:
*		_usX : X����
*		_usY : Y����
*		_ptr  : �ַ���ָ��
*		_tFont : ����ṹ�壬������ɫ������ɫ(֧��͸��)��������롢���ּ��Ȳ���
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void LCD_DispStr(uint16_t _usX, uint16_t _usY, char *_ptr, FONT_T *_tFont)
{
	LCD_DispStrEx0(_usX, _usY, _ptr, _tFont, 0, 0);
}


/*
*********************************************************************************************************
*	�� �� ��: LCD_DispStrEx
*	����˵��: ��LCDָ�����꣨���Ͻǣ���ʾһ���ַ����� ��ǿ�ͺ�����֧����\��\�Ҷ��룬֧�ֶ��������� ֧�ֻ���
*	��    ��:
*		_usX : X����
*		_usY : Y����
*		_ptr  : �ַ���ָ��
*		_tFont : ����ṹ�壬������ɫ������ɫ(֧��͸��)��������롢���ּ��Ȳ���������ָ��RA8875�ֿ���ʾ����
*		_Width : �ַ�����ʾ����Ŀ��. 0 ��ʾ�������������򣬴�ʱ_Align��Ч
*		_Align :�ַ�������ʾ����Ķ��뷽ʽ��
*				ALIGN_LEFT = 0,
*				ALIGN_CENTER = 1,
*				ALIGN_RIGHT = 2
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void LCD_DispStrEx(uint16_t _usX, uint16_t _usY, char *_ptr, FONT_T *_tFont, uint16_t _Width,
	uint8_t _Align)
{
	uint16_t i = 0;
	char str_buf[128] = {0};
	uint16_t len;
	uint16_t x, y;
	uint8_t ch;
	
	len = 0;
	x = _usX;
	y = _usY;
	str_buf[0] = 0;

	for (i = 0; i < 1024; i++)
	{
		ch = _ptr[i];
		
		if (ch == 0)
		{
			LCD_DispStrEx0(x, y, str_buf, _tFont, _Width, _Align);
			break;
		}
		else if (ch == '\r')	/* ����ָ�����2���ַ���ʾ��ֱ��ࣨ��λ���أ� ֧���ص� */
		{
			uint8_t cap;
			
			LCD_DispStrEx0(x, y, str_buf, _tFont, _Width, _Align);
			
			len = 0;
			
			x = _usX;
			
			cap = (_ptr[i + 1] - '0') * 10 + _ptr[i + 2] - '0';		/* ��� */
			y += cap;
			i += 2;
		}
		else if (ch == '\t')	/* ����ָ�����8���ַ���ʾ X1, Y2, X2,  Y2 00 99 02 02 */
		{
			uint16_t x1, x2, y1, y2;
			
			x1 = _usX + (_ptr[i + 1] - '0') * 10 + (_ptr[i + 2] - '0');
			y1 = _usY + (_ptr[i + 3] - '0') * 10 + (_ptr[i + 4] - '0');				
			x2 = _usX + (_ptr[i + 5] - '0') * 10 + (_ptr[i + 6] - '0');
			y2 = _usY + (_ptr[i + 7] - '0') * 10 + (_ptr[i + 8] - '0');
			LCD_DrawLine(x1, y1, x2, y2, _tFont->FrontColor);
						
			i += 8;
		}
		else
		{			
			if (len < sizeof(str_buf) - 1)
			{
				str_buf[len++] = ch;
				
				str_buf[len] = 0;
			}
		}
	}
}

/*
*********************************************************************************************************
*	�� �� ��: LCD_GetFontWidth
*	����˵��: ��ȡ����Ŀ�ȣ����ص�λ)
*	��    ��:
*		_tFont : ����ṹ�壬������ɫ������ɫ(֧��͸��)��������롢���ּ��Ȳ���
*	�� �� ֵ: ����Ŀ�ȣ����ص�λ)
*********************************************************************************************************
*/
uint16_t LCD_GetFontWidth(FONT_T *_tFont)
{
	uint16_t font_width = 16;

	switch (_tFont->FontCode)
	{
		case FC_ST_12:
			font_width = 12;
			break;

		case FC_ST_16:
		case FC_RA8875_16:			
			font_width = 16;
			break;
			
		case FC_RA8875_24:			
		case FC_ST_24:
			font_width = 24;
			break;
			
		case FC_ST_32:
		case FC_RA8875_32:	
			font_width = 32;
			break;		

		case FC_ST_62X40:
			font_width = 40;
			break;	
		
		case FC_ST_96X40:
			font_width = 40;
			break;					
	}
	return font_width;
}

/*
*********************************************************************************************************
*	�� �� ��: LCD_GetFontHeight
*	����˵��: ��ȡ����ĸ߶ȣ����ص�λ)
*	��    ��:
*		_tFont : ����ṹ�壬������ɫ������ɫ(֧��͸��)��������롢���ּ��Ȳ���
*	�� �� ֵ: ����Ŀ�ȣ����ص�λ)
*********************************************************************************************************
*/
uint16_t LCD_GetFontHeight(FONT_T *_tFont)
{
	uint16_t height = 16;

	switch (_tFont->FontCode)
	{
		case FC_ST_12:
			height = 12;
			break;

		case FC_ST_16:
		case FC_RA8875_16:			
			height = 16;
			break;
			
		case FC_RA8875_24:			
		case FC_ST_24:
			height = 24;
			break;
			
		case FC_ST_32:
		case FC_RA8875_32:	
			height = 32;
			break;		

		case FC_ST_62X40:
			height = 62;
			break;		

		case FC_ST_96X40:
			height = 96;
			break;					
	}
	return height;
}

/*
*********************************************************************************************************
*	�� �� ��: LCD_GetStrWidth
*	����˵��: �����ַ������(���ص�λ)
*	��    ��:
*		_ptr  : �ַ���ָ��
*		_tFont : ����ṹ�壬������ɫ������ɫ(֧��͸��)��������롢���ּ��Ȳ���
*	�� �� ֵ: ��
*********************************************************************************************************
*/
uint16_t LCD_GetStrWidth(char *_ptr, FONT_T *_tFont)
{
	char *p = _ptr;
	uint16_t width = 0;
	uint8_t code1, code2;
	uint16_t font_width;
	uint16_t m;
	uint16_t address;
	uint8_t a_flag = 0;

	font_width = LCD_GetFontWidth(_tFont);

	while (*p != 0)
	{
		code1 = *p;	/* ��ȡ�ַ������ݣ� �����ݿ�����ascii���룬Ҳ���ܺ��ִ���ĸ��ֽ� */
		if (code1 < 0x80)	/* ASCII */
		{
			if (code1 == '\a')
			{
				a_flag = 1;
				p++;
				code1 = *p;
			}
			else
			{
				a_flag = 0;
			}
			
			switch(_tFont->FontCode)
			{
				case FC_RA8875_16:
					font_width = g_RA8875_Ascii16_width[code1 - 0x20];
					break;
				
				case FC_RA8875_24:
					if (a_flag == 0)
					{
						font_width = g_RA8875_Ascii24_width[code1 - 0x20];
					}
					else
					{
						{
							m = 0;
							while(1)
							{
							   address = m * (72 + 2);
							   m++;
							   if (code1 == g_Ascii24_VarWidth[address + 0])
							   {
								  font_width = g_Ascii24_VarWidth[address + 1];
								  break;
							   }
							   else if ((g_Ascii24_VarWidth[address + 0] == 0xFF) && (g_Ascii24_VarWidth[address + 1] == 0xFF))
							   {
	//							  /* �ֿ�������ϣ�δ�ҵ��������ȫFF */
	//							  memset(g_Ascii32_VarWidth, 0xFF, 128);
								  break;
							   }	   
						   }
						}
					}
					break;
				
				case FC_RA8875_32:
					if (a_flag == 0)
					{
						font_width = g_RA8875_Ascii32_width[code1 - 0x20];
					}
					else
					{
						{
							m = 0;
							while(1)
							{
							   address = m * (128 + 2);
							   m++;
							   if (code1 == g_Ascii32_VarWidth[address + 0])
							   {
								  font_width = g_Ascii32_VarWidth[address + 1];
								  break;
							   }
							   else if ((g_Ascii32_VarWidth[address + 0] == 0xFF) && (g_Ascii32_VarWidth[address + 1] == 0xFF))
							   {
	//							  /* �ֿ�������ϣ�δ�ҵ��������ȫFF */
	//							  memset(g_Ascii32_VarWidth, 0xFF, 128);
								  break;
							   }	   
						   }
						}
					}
					break;
				
				case FC_ST_12:
					font_width = 6;
					break;

				case FC_ST_16:		
					font_width = 8;
					break;
					
				case FC_ST_24:			
					font_width = 12;
					break;
					
				case FC_ST_32:
					font_width = 16;
					break;

				case FC_ST_62X40:
					//����������⴦��
					if (code1 == 0x5E)
					{
						font_width = 28;
					}
					else
					{
						font_width = 40;
					}
					break;

				case FC_ST_96X40:
					//����������⴦��
					if (code1 == 0x5E)
					{
						font_width = 28;
					}
					else
					{
						font_width = 40;
					}
					break;
									
				default:
					font_width = 8;
					break;					
			}
			
		}
		else	/* ���� */
		{
			code2 = *++p;
			if (code2 == 0)
			{
				break;
			}
			font_width = LCD_GetFontWidth(_tFont);
			
		}
		width += (font_width + _tFont->Space);
		p++;
	}

	return width;
}

/*
*********************************************************************************************************
*	�� �� ��: _LCD_ReadSmallDot
*	����˵��: ��ȡ1��С�����ַ��ĵ�������
*	��    ��:
*		_code : ASCII�ַ��ı��룬1�ֽڡ�1-128
*		_fontcode ���������
*		_pBuf : ��Ŷ������ַ���������
*	�� �� ֵ: ���ֿ��
*********************************************************************************************************
*/
static void _LCD_ReadSmallDot(uint8_t _code, uint8_t _fontcode, uint8_t *_pBuf)
{
#ifdef USE_SMALL_FONT	/* ʹ��CPU �ڲ�Flash С�ֿ� */
	const uint8_t *pAscDot;
	uint32_t font_bytes = 0;
	uint16_t m;
	uint16_t address;
	uint8_t fAllHz = 0;	/* 1��ʾ��������Ƕȫ����ASCII�ַ��� */
	
	pAscDot = 0;
	switch (_fontcode)
	{
		case FC_ST_12:		/* 12���� */
			font_bytes = 24 / 2;
			pAscDot = g_Ascii12;	
			fAllHz = 1;
			break;
		
		case FC_ST_16:
			/* ȱʡ��16���� */
			font_bytes = 32 / 2;
			pAscDot = g_Ascii16;
			fAllHz = 1;
			break;
		
		case FC_ST_24:
			font_bytes = 48;
			pAscDot = g_Ascii24;
			break;
		
		case FC_ST_32:
			font_bytes = 64;
			pAscDot = g_Ascii32;
			break;

		case FC_ST_62X40:
			font_bytes = 310;
			pAscDot = g_Ascii62x40;
			break;	
		
		case FC_ST_96X40:
			font_bytes = 480;
			pAscDot = g_Ascii96x40;
			break;

		case FC_RA8875_24:
			font_bytes = 72;
			pAscDot = g_Ascii24_VarWidth;
			fAllHz = 2;
			break;
		
		case FC_RA8875_32:
			font_bytes = 128;
			pAscDot = g_Ascii32_VarWidth;
			fAllHz = 2;
			break;
		
		default:
			return;
	}	

	if (fAllHz == 1)	/* ��Ƕȫ��ASCII�ַ����� */
	{
		/* ��CPU�ڲ�Flash�е�ascii�ַ������Ƶ�buf */
		memcpy(_pBuf, &pAscDot[_code * (font_bytes)], (font_bytes));		
	}
	else if (fAllHz == 2)
	{
		m = 0;
		while(1)
		{
		   address = m * (font_bytes + 2);
		   m++;
		   if (_code == pAscDot[address + 0])
		   {
			  address += 2;
			  memcpy(_pBuf, &pAscDot[address], font_bytes);
			  break;
		   }
		   else if ((pAscDot[address + 0] == 0xFF) && (pAscDot[address + 1] == 0xFF))
		   {
			  /* �ֿ�������ϣ�δ�ҵ��������ȫFF */
			  memset(_pBuf, 0xFF, font_bytes);
			  break;
		   }	   
	   }
	}
	else	/* ��Ƕ�����ַ�����ģ�������ֽ���ASCII�� */
	{
		m = 0;
		while(1)
		{
		   address = m * (font_bytes + 1);
		   m++;
		   if (_code == pAscDot[address + 0])
		   {
			  address += 1;
			  memcpy(_pBuf, &pAscDot[address], font_bytes);
			  break;
		   }
		   else if ((pAscDot[address + 0] == 0xFF) && (pAscDot[address + 1] == 0xFF))
		   {
			  /* �ֿ�������ϣ�δ�ҵ��������ȫFF */
			  memset(_pBuf, 0xFF, font_bytes);
			  break;
		   }	   
	   }
   }
#else	/* ��ȫ�ֿ� */
	uint32_t pAscDot;
	uint8_t font_bytes = 0;

	pAscDot = 0;
	switch (_fontcode)
	{
		case FC_ST_12:		/* 12���� */
			font_bytes = 12;
			#if 0
				pAscDot = ASC12_ADDR;	/* �ֿ�оƬ��16�����ַ����ÿ�,�ʻ�ϸ�ˣ������Ƿǵȿ����� */
			#else
				pAscDot = (uint32_t)&g_Ascii12[' ' * 12];	/* ʹ��CPU��Ƕ��16�����ַ� */
			#endif			
			break;
		
		case FC_ST_16:
			font_bytes = 16;
			#if 0
				pAscDot = ASC16_ADDR;	/* �ֿ�оƬ��16�����ַ����ÿ�,�ʻ�ϸ�ˣ������Ƿǵȿ����� */
			#else
				pAscDot = (uint32_t)&g_Ascii16[' ' * 16];	/* ʹ��CPU��Ƕ��16�����ַ� */
			#endif
			break;
		
		case FC_ST_24:
			font_bytes = 48;
			pAscDot = ASC24_ADDR;
			break;
		
		case FC_ST_32:
			font_bytes = 64;
			pAscDot = ASC32_ADDR;
			break;
		
		default:
			return;
	}			
	if (_code >=0x20 && _code <= 0x7E)
	{		
		pAscDot = ((uint32_t)_code - 0x20)*font_bytes + pAscDot;
	}

#ifdef USE_NOR_FONT		/* NOR Flashȫ�ֿ� */
	/* ��CPU�ڲ�Flash�е�ascii�ַ������Ƶ�buf */
	memcpy(_pBuf, (char *)pAscDot, font_bytes);	
#endif

#ifdef USE_SPI_FONT		/* ���� Flashȫ�ֿ� */
	if (_fontcode == FC_ST_12 || _fontcode == FC_ST_16)
	{
		memcpy(_pBuf, (char *)pAscDot, font_bytes);	
	}
	else
	{
		/* �ֿ�оƬ��12�����16�����ַ����ÿ�,�ʻ�ϸ�ˣ������Ƿǵȿ����� */
		sf_ReadBuffer(_pBuf, pAscDot, font_bytes);
	}
#endif
	
#endif
}

/*
*********************************************************************************************************
*	�� �� ��: _LCD_ReadAsciiDot
*	����˵��: ��ȡ1��ASCII�ַ��ĵ�������
*	��    ��:
*		_code : ASCII�ַ��ı��룬1�ֽڡ�1-128
*		_fontcode ���������
*		_pBuf : ��Ŷ������ַ���������
*	�� �� ֵ: ���ֿ��
*********************************************************************************************************
*/
static void _LCD_ReadAsciiDot(uint8_t _code, uint8_t _fontcode, uint8_t *_pBuf)
{
#ifdef USE_SMALL_FONT	/* ʹ��CPU �ڲ�Flash С�ֿ� */
	const uint8_t *pAscDot;
	uint32_t font_bytes = 0;
	uint16_t m;
	uint16_t address;
	uint8_t fAllHz = 0;	/* 1��ʾ��������Ƕȫ����ASCII�ַ��� */

	pAscDot = 0;
	switch (_fontcode)
	{
		case FC_ST_12:		/* 12���� */
			font_bytes = 24 / 2;
			pAscDot = g_Ascii12;	
			fAllHz = 1;
			break;
		
		case FC_ST_16:
			/* ȱʡ��16���� */
			font_bytes = 32 / 2;
			pAscDot = g_Ascii16;
			fAllHz = 1;
			break;
		
		case FC_ST_24:
			font_bytes = 48;
			pAscDot = g_Ascii24;
			break;
		
		case FC_ST_32:
			font_bytes = 64;
			pAscDot = g_Ascii32;
			break;

		case FC_ST_62X40:
			font_bytes = 310;
			pAscDot = g_Ascii62x40;
			break;	
		
		case FC_ST_96X40:
			font_bytes = 480;
			pAscDot = g_Ascii96x40;
			break;
		
		default:
			return;
	}	

	if (fAllHz == 1)	/* ��Ƕȫ��ASCII�ַ����� */
	{
		/* ��CPU�ڲ�Flash�е�ascii�ַ������Ƶ�buf */
		memcpy(_pBuf, &pAscDot[_code * (font_bytes)], (font_bytes));		
	}
	else	/* ��Ƕ�����ַ�����ģ�������ֽ���ASCII�� */
	{
		m = 0;
		while(1)
		{
		   address = m * (font_bytes + 1);
		   m++;
		   if (_code == pAscDot[address + 0])
		   {
			  address += 1;
			  memcpy(_pBuf, &pAscDot[address], font_bytes);
			  break;
		   }
		   else if ((pAscDot[address + 0] == 0xFF) && (pAscDot[address + 1] == 0xFF))
		   {
			  /* �ֿ�������ϣ�δ�ҵ��������ȫFF */
			  memset(_pBuf, 0xFF, font_bytes);
			  break;
		   }	   
	   }
   }
#else	/* ��ȫ�ֿ� */
	uint32_t pAscDot;
	uint8_t font_bytes = 0;

	pAscDot = 0;
	switch (_fontcode)
	{
		case FC_ST_12:		/* 12���� */
			font_bytes = 12;
			#if 0
				pAscDot = ASC12_ADDR;	/* �ֿ�оƬ��16�����ַ����ÿ�,�ʻ�ϸ�ˣ������Ƿǵȿ����� */
			#else
				pAscDot = (uint32_t)&g_Ascii12[' ' * 12];	/* ʹ��CPU��Ƕ��16�����ַ� */
			#endif			
			break;
		
		case FC_ST_16:
			font_bytes = 16;
			#if 0
				pAscDot = ASC16_ADDR;	/* �ֿ�оƬ��16�����ַ����ÿ�,�ʻ�ϸ�ˣ������Ƿǵȿ����� */
			#else
				pAscDot = (uint32_t)&g_Ascii16[' ' * 16];	/* ʹ��CPU��Ƕ��16�����ַ� */
			#endif
			break;
		
		case FC_ST_24:
			font_bytes = 48;
			pAscDot = ASC24_ADDR;
			break;
		
		case FC_ST_32:
			font_bytes = 64;
			pAscDot = ASC32_ADDR;
			break;
		
		default:
			return;
	}			
	if (_code >=0x20 && _code <= 0x7E)
	{		
		pAscDot = ((uint32_t)_code - 0x20)*font_bytes + pAscDot;
	}

#ifdef USE_NOR_FONT		/* NOR Flashȫ�ֿ� */
	/* ��CPU�ڲ�Flash�е�ascii�ַ������Ƶ�buf */
	memcpy(_pBuf, (char *)pAscDot, font_bytes);	
#endif

#ifdef USE_SPI_FONT		/* ���� Flashȫ�ֿ� */
	if (_fontcode == FC_ST_12 || _fontcode == FC_ST_16)
	{
		memcpy(_pBuf, (char *)pAscDot, font_bytes);	
	}
	else
	{
		/* �ֿ�оƬ��12�����16�����ַ����ÿ�,�ʻ�ϸ�ˣ������Ƿǵȿ����� */
		sf_ReadBuffer(_pBuf, pAscDot, font_bytes);
	}
#endif
	
#endif
}

/*
*********************************************************************************************************
*	�� �� ��: _LCD_ReadHZDot
*	����˵��: ��ȡ1�����ֵĵ�������
*	��    ��:
*		_code1, _cod2 : ��������. GB2312����
*		_fontcode ���������
*		_pBuf : ��Ŷ������ַ���������
*	�� �� ֵ: ��
*********************************************************************************************************
*/
static void _LCD_ReadHZDot(uint8_t _code1, uint8_t _code2,  uint8_t _fontcode, uint8_t *_pBuf)
{
#ifdef USE_SMALL_FONT	/* ʹ��CPU �ڲ�Flash С�ֿ� */
	uint8_t *pDot;
	uint8_t font_bytes = 0;
	uint32_t address;
	uint16_t m;

	pDot = 0;	/* �������ڱ���澯 */
	switch (_fontcode)
	{
		case FC_ST_12:		/* 12���� */
			font_bytes = 24;
			pDot = (uint8_t *)g_Hz12;	
			break;
		
		case FC_ST_16:
			font_bytes = 32;
			pDot = (uint8_t *)g_Hz16;
			break;

		case FC_ST_24:
			font_bytes = 72;
			pDot = (uint8_t *)g_Hz24;
			break;			
			
		case FC_ST_32:	
			font_bytes = 128;
			pDot = (uint8_t *)g_Hz32;
			break;						
		
		default:
			return;
	}	

	m = 0;
	while(1)
	{
		address = m * (font_bytes + 2);
		m++;
		if ((_code1 == pDot[address + 0]) && (_code2 == pDot[address + 1]))
		{
			address += 2;
			memcpy(_pBuf, &pDot[address], font_bytes);
			break;
		}
		else if ((pDot[address + 0] == 0xFF) && (pDot[address + 1] == 0xFF))
		{
			/* �ֿ�������ϣ�δ�ҵ��������ȫFF */
			memset(_pBuf, 0xFF, font_bytes);
			break;
		}
	}
#else	/* ��ȫ�ֿ� */
	uint32_t offset = 0;
	uint8_t font_bytes = 0;
		
	switch (_fontcode)
	{
		case FC_ST_12:		/* 12���� */
			font_bytes = 24;
			offset = HZK12_ADDR;	
			break;
		
		case FC_ST_16:
			font_bytes = 32;
			offset = HZK16_ADDR;
			break;

		case FC_ST_24:
			font_bytes = 72;
			offset = HZK24_ADDR;
			break;			
			
		case FC_ST_32:	
			font_bytes = 128;
			offset = HZK32_ADDR;
			break;						
		
		default:
			return;
	}			

	/* �˴���Ҫ�����ֿ��ļ����λ�ý����޸� 
		GB2312��Χ�� 0xA1A1 - 0xFEFE
		���к��ַ�Χ : 0xB0A1 - 0xF7FE
	
		GBK ��Χ�� 0x8140 - 0xFEFE 
	
		�������Զ��庺�ֱ����GBK��GB2312����ռ䣺 0x8000 - 0x813F ��319����		
	*/
	if (_code1 >=0xA1 && _code1 <= 0xA9 && _code2 >=0xA1)
	{
		offset += ((_code1 - 0xA1) * 94 + (_code2 - 0xA1)) * font_bytes;
	}
	else if (_code1 >=0xB0 && _code1 <= 0xF7 && _code2 >=0xA1)
	{
		offset += ((_code1 - 0xB0) * 94 + (_code2 - 0xA1) + 846) * font_bytes;
	}
	else	/* 2018-03-13 �����Զ��庺�ֱ��룬����ʵ������ͼ����� */
	{
		uint16_t code16;
		uint8_t *pDot;
		uint32_t address;
		uint16_t m;		
		
		code16 = _code1 * 256 + _code2;
		if (code16 >= 0x8000 && code16 <= 0x813F)	/* �Զ��庺�ֵ��󣬹̶�ʹ��CPUƬ�ڲ�С�ֿ� */
		{
			pDot = 0;	/* �������ڱ���澯 */
			switch (_fontcode)
			{
				case FC_ST_12:		/* 12���� */
					font_bytes = 24;
					pDot = (uint8_t *)g_Hz12;	
					break;
				
				case FC_ST_16:
					font_bytes = 32;
					pDot = (uint8_t *)g_Hz16;
					break;

				case FC_ST_24:
					font_bytes = 72;
					pDot = (uint8_t *)g_Hz24;
					break;			
					
				case FC_ST_32:	
					font_bytes = 128;
					pDot = (uint8_t *)g_Hz32;
					break;						
				
				default:
					break;
			}	
			
			m = 0;
			while(1)
			{
				address = m * (font_bytes + 2);
				m++;
				if ((_code1 == pDot[address + 0]) && (_code2 == pDot[address + 1]))
				{
					address += 2;
					memcpy(_pBuf, &pDot[address], font_bytes);
					break;
				}
				else if ((pDot[address + 0] == 0xFF) && (pDot[address + 1] == 0xFF))
				{
					/* �ֿ�������ϣ�δ�ҵ��������ȫFF */
					memset(_pBuf, 0xFF, font_bytes);
					break;
				}
			}	
			return;
		}
	}

#ifdef USE_NOR_FONT		/* NOR Flashȫ�ֿ� */
	/* ��CPU�ڲ�Flash�е�ascii�ַ������Ƶ�buf */
	memcpy(_pBuf, (char *)offset, font_bytes);	
#endif

#ifdef USE_SPI_FONT		/* NOR Flashȫ�ֿ� */
	sf_ReadBuffer(_pBuf, offset, font_bytes);
#endif
	
#endif
}

/*
*********************************************************************************************************
*	�� �� ��: SeachStr_a
*	����˵��: ����һ���ַ����Ƿ��С�\a��
*	��    ��:
*		_ptr  : �ַ���ָ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
//static uint8_t SeachStr_a(char *_ptr)
//{
//	uint16_t j = 0;
//	uint8_t a_flag = 0;
//	
//	while(_ptr[j] != 0)
//	{
//		if (_ptr[j] == '\a')
//		{
//			a_flag = 1;
//			break;
//		}
//		j++;
//	}
//	
//	return a_flag;
//}
		
/*
*********************************************************************************************************
*	�� �� ��: LCD_DispStrEx0
*	����˵��: ��LCDָ�����꣨���Ͻǣ���ʾһ���ַ����� ��ǿ�ͺ�����֧����\��\�Ҷ��룬֧�ֶ���������
*	��    ��:
*		_usX : X����
*		_usY : Y����
*		_ptr  : �ַ���ָ��
*		_tFont : ����ṹ�壬������ɫ������ɫ(֧��͸��)��������롢���ּ��Ȳ���������ָ��RA8875�ֿ���ʾ����
*		_Width : �ַ�����ʾ����Ŀ��. 0 ��ʾ�������������򣬴�ʱ_Align��Ч
*		_Align :�ַ�������ʾ����Ķ��뷽ʽ��
*				ALIGN_LEFT = 0,
*				ALIGN_CENTER = 1,
*				ALIGN_RIGHT = 2
*	�� �� ֵ: ��
*********************************************************************************************************
*/
static void LCD_DispStrEx0(uint16_t _usX, uint16_t _usY, char *_ptr, FONT_T *_tFont, uint16_t _Width,
	uint8_t _Align)
{
	uint32_t i;
	uint8_t code1;
	uint8_t code2;
	//uint8_t buf[32 * 32 / 8];	/* ���֧��32������ */
	uint8_t buf[96 * 40 / 8];	/* ���֧��96x40�����ַ� */
	uint8_t width;
	uint16_t m;
	uint8_t font_width = 0;
	uint8_t font_height = 0;
	uint16_t x, y;
	uint16_t offset;
	uint16_t str_width;	/* �ַ���ʵ�ʿ��  */
//	uint8_t ra8875_use = 0;
//	uint8_t ra8875_font_code = 0;
	uint16_t address;
	uint8_t a_flag = 0;
	uint8_t RA8875_flag = 0;
	
	uint8_t line_bytes;
	uint8_t asc_bytes = 0;
	uint8_t hz_bytes = 0;

	switch (_tFont->FontCode)
	{
		case FC_ST_12:		/* 12���� */
			font_height = 12;
			font_width = 12;
			asc_bytes = 1;
			hz_bytes = 2;
			break;
		
		case FC_ST_16:
			font_height = 16;
			font_width = 16;
			asc_bytes = 1;
			hz_bytes = 2;
			break;

		case FC_ST_24:
			font_height = 24;
			font_width = 24;
			asc_bytes = 2;
			hz_bytes = 3;
			break;
						
		case FC_ST_32:	
			font_height = 32;
			font_width = 32;
			asc_bytes = 2;
			hz_bytes = 4;
			break;		

		case FC_ST_62X40:	
			font_height = 62;
			font_width = 80;
			asc_bytes = 5;
			hz_bytes = 10;
			break;			

		case FC_ST_96X40:	
			font_height = 96;
			font_width = 80;
			asc_bytes = 5;
			hz_bytes = 10;
			break;		
	}
	
	str_width = LCD_GetStrWidth(_ptr, _tFont);	/* �����ַ���ʵ�ʿ��(RA8875�ڲ�ASCII������Ϊ�䳤 */
	offset = 0;
	if (_Width > str_width)
	{
		if (_Align == ALIGN_RIGHT)	/* �Ҷ��� */
		{
			offset = _Width - str_width;
		}
		else if (_Align == ALIGN_CENTER)	/* ����� */
		{
			offset = (_Width - str_width) / 2;
		}
		else	/* ����� ALIGN_LEFT */
		{
			;
		}
	}

	/* ������ɫ, �м������ұ߶���  */
	if (offset > 0)
	{
		if (_tFont->BackColor != CL_MASK)	/* ͸��ɫ */
		{
			LCD_Fill_Rect(_usX, _usY, LCD_GetFontHeight(_tFont), offset,  _tFont->BackColor);	
		}
		_usX += offset;
	}
	
	/* �Ҳ����ɫ */
	if (_Width > str_width)
	{
		if (_tFont->BackColor != CL_MASK)	/* ͸��ɫ */
		{
			LCD_Fill_Rect(_usX + str_width, _usY, LCD_GetFontHeight(_tFont), _Width - str_width - offset,  _tFont->BackColor);
		}
	}
	
	/* ʹ��CPU�ڲ��ֿ�. ������Ϣ��CPU��ȡ */
	{
		/* ��ʼѭ�������ַ� */
		while (*_ptr != 0)
		{
			code1 = *_ptr;	/* ��ȡ�ַ������ݣ� �����ݿ�����ascii���룬Ҳ���ܺ��ִ���ĸ��ֽ� */
			if (code1 < 0x80)
			{
				if (a_flag == 0)
				{
					RA8875_flag = 0;
					/* ��ascii�ַ������Ƶ�buf */
					_LCD_ReadAsciiDot(code1, _tFont->FontCode, buf);	/* ��ȡASCII�ַ����� */
					
					//����������⴦��,�����ȹ���
					if (_tFont->FontCode == FC_ST_62X40 || _tFont->FontCode == FC_ST_96X40)
					{
						if (code1 == 0x5E)
						{
							width = 28;
						}
						else
						{
							width = font_width / 2;
						}	
					}
					else
					{
						width = font_width / 2;
					}
					
					
					line_bytes = asc_bytes;
				}	
				else
				{
					if (code1 == '\a')
					{
						RA8875_flag = 0;
						_ptr++;
						code1 = *_ptr;
						if (_tFont->FontCode == FC_RA8875_32)
						{
							m = 0;
							while(1)
							{
							   address = m * (128 + 2);
							   m++;
							   if (code1 == g_Ascii32_VarWidth[address + 0])
							   {
								  font_width = g_Ascii32_VarWidth[address + 1];
								  break;
							   }
							   else if ((g_Ascii32_VarWidth[address + 0] == 0xFF) && (g_Ascii32_VarWidth[address + 1] == 0xFF))
							   {
	//							  /* �ֿ�������ϣ�δ�ҵ��������ȫFF */
	//							  memset(g_Ascii32_VarWidth, 0xFF, 128);
								  break;
							   }	   
							}
						}
						else if (_tFont->FontCode == FC_RA8875_24)
						{
							m = 0;
							while(1)
							{
							   address = m * (72 + 2);
							   m++;
							   if (code1 == g_Ascii24_VarWidth[address + 0])
							   {
								  font_width = g_Ascii24_VarWidth[address + 1];
								  break;
							   }
							   else if ((g_Ascii24_VarWidth[address + 0] == 0xFF) && (g_Ascii24_VarWidth[address + 1] == 0xFF))
							   {
	//							  /* �ֿ�������ϣ�δ�ҵ��������ȫFF */
	//							  memset(g_Ascii32_VarWidth, 0xFF, 128);
								  break;
							   }	   
						   }
						}	
						_LCD_ReadSmallDot(code1, _tFont->FontCode, buf);
						
						width = font_width;
					
						line_bytes = asc_bytes;
					}
//					else
//					{
//						RA8875_flag = 1;
//						if (_tFont->FontCode == FC_RA8875_32)
//						{
//							font_width = g_RA8875_Ascii32_width[code1 - 0x20];
//						}
//						else if (_tFont->FontCode == FC_RA8875_24)
//						{
//							font_width = g_RA8875_Ascii24_width[code1 - 0x20];
//						}
//						width = font_width;
//						line_bytes = asc_bytes;
//					}
				}
			}
			else
			{
				RA8875_flag = 0;
				code2 = *++_ptr;
				if (code2 == 0)
				{
					break;
				}
				/* ��1�����ֵĵ��� */
				_LCD_ReadHZDot(code1, code2, _tFont->FontCode, buf);
				width = font_width;
				line_bytes = hz_bytes;
			}
	
			y = _usY;
			if (RA8875_flag == 0)
			{
				/* ��ʼˢLCD */
				for (m = 0; m < font_height; m++)	/* �ַ��߶� */
				{
					x = _usX;
					for (i = 0; i < width; i++)	/* �ַ���� */
					{
						if ((buf[m * line_bytes + i / 8] & (0x80 >> (i % 8 ))) != 0x00)
						{
							LCD_PutPixel(x, y, _tFont->FrontColor);	/* ����������ɫΪ����ɫ */
						}
						else
						{
							if (_tFont->BackColor != CL_MASK)	/* ͸��ɫ */
							{
								LCD_PutPixel(x, y, _tFont->BackColor);	/* ����������ɫΪ���ֱ���ɫ */
							}
						}
		
						x++;
					}
					
					for (i = 0; i < _tFont->Space; i++)	/* �ַ���� */
					{
						if (_tFont->BackColor != CL_MASK)	/* ͸��ɫ */
						{						
							/* ������ֵ�ɫ��_tFont->usBackColor�������ּ����ڵ���Ŀ�ȣ���ô��Ҫ������֮�����(��ʱδʵ��) */
							LCD_PutPixel(x + i, y, _tFont->BackColor);	/* ����������ɫΪ���ֱ���ɫ */
						}
					}
					y++;					
				}
			}
//			else
//			{
//				if (_tFont->BackColor == CL_MASK)	/* ͸��ɫ */
//				{
//					RA8875_SetTextTransp(1);
//				}
//				RA8875_SetFrontColor(_tFont->FrontColor);			/* ��������ǰ��ɫ */
//				RA8875_SetBackColor(_tFont->BackColor);				/* �������屳��ɫ */
//				RA8875_SetFont(ra8875_font_code, 0, _tFont->Space);	/* ������룬�м�࣬�ּ�� */
//				RA8875_DispStr(_usX, _usY, (char *)&code1);
//				if (_tFont->BackColor == CL_MASK)	/* ͸��ɫ */
//				{
//					RA8875_SetTextTransp(0);
//				}	
//			}
			_usX += width + _tFont->Space;	/* �е�ַ���� */
			_ptr++;			/* ָ����һ���ַ� */
		}
	}
}

/*
*********************************************************************************************************
*	�� �� ��: LCD_PutPixel
*	����˵��: ��1������
*	��    ��:
*			_usX,_usY : ��������
*			_usColor  : ������ɫ
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void LCD_PutPixel(uint16_t _usX, uint16_t _usY, uint16_t _usColor)
{
	LCDX_PutPixel(_usX, _usY, _usColor);
}

/*
*********************************************************************************************************
*	�� �� ��: LCD_GetPixel
*	����˵��: ��ȡ1������
*	��    ��:
*			_usX,_usY : ��������
*			_usColor  : ������ɫ
*	�� �� ֵ: RGB��ɫֵ
*********************************************************************************************************
*/
uint16_t LCD_GetPixel(uint16_t _usX, uint16_t _usY)
{
	uint16_t usRGB;

	usRGB = LCDX_GetPixel(_usX, _usY);
	return usRGB;
}

/*
*********************************************************************************************************
*	�� �� ��: LCD_DrawLine
*	����˵��: ���� Bresenham �㷨����2��仭һ��ֱ�ߡ�
*	��    ��:
*			_usX1, _usY1 : ��ʼ������
*			_usX2, _usY2 : ��ֹ��Y����
*			_usColor     : ��ɫ
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void LCD_DrawLine(uint16_t _usX1 , uint16_t _usY1 , uint16_t _usX2 , uint16_t _usY2 , uint16_t _usColor)
{
	LCDX_DrawLine(_usX1 , _usY1 , _usX2, _usY2 , _usColor);
}

/*
*********************************************************************************************************
*	�� �� ��: LCD_DrawPoints
*	����˵��: ���� Bresenham �㷨������һ��㣬������Щ�����������������ڲ�����ʾ��
*	��    ��:
*			x, y     : ��������
*			_usColor : ��ɫ
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void LCD_DrawPoints(uint16_t *x, uint16_t *y, uint16_t _usSize, uint16_t _usColor)
{
	uint16_t i;

	for (i = 0 ; i < _usSize - 1; i++)
	{
		LCD_DrawLine(x[i], y[i], x[i + 1], y[i + 1], _usColor);
	}
}

/*
*********************************************************************************************************
*	�� �� ��: LCD_DrawRect
*	����˵��: ����ˮƽ���õľ��Ρ�
*	��    ��:
*			_usX,_usY: �������Ͻǵ�����
*			_usHeight : ���εĸ߶�
*			_usWidth  : ���εĿ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void LCD_DrawRect(uint16_t _usX, uint16_t _usY, uint16_t _usHeight, uint16_t _usWidth, uint16_t _usColor)
{
	LCDX_DrawRect(_usX, _usY, _usHeight, _usWidth, _usColor);
}

/*
*********************************************************************************************************
*	�� �� ��: LCD_Fill_Rect
*	����˵��: ��һ����ɫֵ���һ�����Ρ���emWin ����ͬ������ LCD_FillRect����˼����»������֡�
*	��    ��:
*			_usX,_usY: �������Ͻǵ�����
*			_usHeight : ���εĸ߶�
*			_usWidth  : ���εĿ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void LCD_Fill_Rect(uint16_t _usX, uint16_t _usY, uint16_t _usHeight, uint16_t _usWidth, uint16_t _usColor)
{
	LCDX_FillRect(_usX, _usY, _usHeight, _usWidth, _usColor);
}

/*
*********************************************************************************************************
*	�� �� ��: LCD_DrawCircle
*	����˵��: ����һ��Բ���ʿ�Ϊ1������
*	��    ��:
*			_usX,_usY  : Բ�ĵ�����
*			_usRadius  : Բ�İ뾶
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void LCD_DrawCircle(uint16_t _usX, uint16_t _usY, uint16_t _usRadius, uint16_t _usColor)
{
	LCDX_DrawCircle(_usX, _usY, _usRadius, _usColor);
}

/*
*********************************************************************************************************
*	�� �� ��: LCD_DrawBMP
*	����˵��: ��LCD����ʾһ��BMPλͼ��λͼ����ɨ�����: �����ң����ϵ���
*	��    ��:
*			_usX, _usY : ͼƬ������
*			_usHeight  : ͼƬ�߶�
*			_usWidth   : ͼƬ���
*			_ptr       : ͼƬ����ָ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void LCD_DrawBMP(uint16_t _usX, uint16_t _usY, uint16_t _usHeight, uint16_t _usWidth, uint16_t *_ptr)
{
	LCDX_DrawBMP(_usX, _usY, _usHeight, _usWidth, _ptr);
}

/*
*********************************************************************************************************
*	�� �� ��: LCD_DrawWin
*	����˵��: ��LCD�ϻ���һ������
*	��    ��: �ṹ��ָ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void LCD_DrawWin(WIN_T *_pWin)
{
	uint16_t TitleHegiht;

	TitleHegiht = 20;

	/* ���ƴ������ */
	LCD_DrawRect(_pWin->Left, _pWin->Top, _pWin->Height, _pWin->Width, WIN_BORDER_COLOR);
	LCD_DrawRect(_pWin->Left + 1, _pWin->Top + 1, _pWin->Height - 2, _pWin->Width - 2, WIN_BORDER_COLOR);

	/* ���ڱ����� */
	LCD_Fill_Rect(_pWin->Left + 2, _pWin->Top + 2, TitleHegiht, _pWin->Width - 4, WIN_TITLE_COLOR);

	/* ������� */
	LCD_Fill_Rect(_pWin->Left + 2, _pWin->Top + TitleHegiht + 2, _pWin->Height - 4 - TitleHegiht,
		_pWin->Width - 4, WIN_BODY_COLOR);

	LCD_DispStr(_pWin->Left + 3, _pWin->Top + 2, _pWin->pCaption, _pWin->Font);
}


/*
*********************************************************************************************************
*	�� �� ��: LCD_DrawIcon
*	����˵��: ��LCD�ϻ���һ��ͼ�꣬�Ľ��Զ���Ϊ����
*	��    ��: _pIcon : ͼ��ṹ
*			  _tFont : ��������
*			  _ucFocusMode : ����ģʽ��0 ��ʾ����ͼ��  1��ʾѡ�е�ͼ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void LCD_DrawIcon(const ICON_T *_tIcon, FONT_T *_tFont, uint8_t _ucFocusMode)
{
	const uint16_t *p;
	uint16_t usNewRGB;
	uint16_t x, y;		/* ���ڼ�¼�����ڵ�������� */

	p = _tIcon->pBmp;
	for (y = 0; y < _tIcon->Height; y++)
	{
		for (x = 0; x < _tIcon->Width; x++)
		{
			usNewRGB = *p++;	/* ��ȡͼ�����ɫֵ��ָ���1 */
			/* ��ͼ���4��ֱ���и�Ϊ���ǣ��������Ǳ���ͼ�� */
			if ((y == 0 && (x < 6 || x > _tIcon->Width - 7)) ||
				(y == 1 && (x < 4 || x > _tIcon->Width - 5)) ||
				(y == 2 && (x < 3 || x > _tIcon->Width - 4)) ||
				(y == 3 && (x < 2 || x > _tIcon->Width - 3)) ||
				(y == 4 && (x < 1 || x > _tIcon->Width - 2)) ||
				(y == 5 && (x < 1 || x > _tIcon->Width - 2))	||

				(y == _tIcon->Height - 1 && (x < 6 || x > _tIcon->Width - 7)) ||
				(y == _tIcon->Height - 2 && (x < 4 || x > _tIcon->Width - 5)) ||
				(y == _tIcon->Height - 3 && (x < 3 || x > _tIcon->Width - 4)) ||
				(y == _tIcon->Height - 4 && (x < 2 || x > _tIcon->Width - 3)) ||
				(y == _tIcon->Height - 5 && (x < 1 || x > _tIcon->Width - 2)) ||
				(y == _tIcon->Height - 6 && (x < 1 || x > _tIcon->Width - 2))
				)
			{
				;
			}
			else
			{
				if (_ucFocusMode != 0)	/* 1��ʾѡ�е�ͼ�� */
				{
					/* ����ԭʼ���ص����ȣ�ʵ��ͼ�걻����ѡ�е�Ч�� */
					uint16_t R,G,B;
					uint16_t bright = 15;

					/* rrrr rggg gggb bbbb */
					R = (usNewRGB & 0xF800) >> 11;
					G = (usNewRGB & 0x07E0) >> 5;
					B =  usNewRGB & 0x001F;
					if (R > bright)
					{
						R -= bright;
					}
					else
					{
						R = 0;
					}
					if (G > 2 * bright)
					{
						G -= 2 * bright;
					}
					else
					{
						G = 0;
					}
					if (B > bright)
					{
						B -= bright;
					}
					else
					{
						B = 0;
					}
					usNewRGB = (R << 11) + (G << 5) + B;
				}

				LCD_PutPixel(x + _tIcon->Left, y + _tIcon->Top, usNewRGB);
			}
		}
	}

	/* ����ͼ���µ����� */
	{
		uint16_t len;
		uint16_t width;

		len = strlen(_tIcon->Text);

		if  (len == 0)
		{
			return;	/* ���ͼ���ı�����Ϊ0������ʾ */
		}

		/* �����ı����ܿ�� */
		if (_tFont->FontCode == FC_ST_12)		/* 12���� */
		{
			width = 6 * (len + _tFont->Space);
		}
		else	/* FC_ST_16 */
		{
			width = 8 * (len + _tFont->Space);
		}


		/* ˮƽ���� */
		x = (_tIcon->Left + _tIcon->Width / 2) - width / 2;
		y = _tIcon->Top + _tIcon->Height + 2;
		LCD_DispStr(x, y, (char *)_tIcon->Text, _tFont);
	}
}

/*
*********************************************************************************************************
*	�� �� ��: LCD_Blend565
*	����˵��: ������͸���� ��ɫ���
*	��    ��: src : ԭʼ����
*			  dst : ��ϵ���ɫ
*			  alpha : ͸���� 0-32
*	�� �� ֵ: ��
*********************************************************************************************************
*/
uint16_t LCD_Blend565(uint16_t src, uint16_t dst, uint8_t alpha)
{
	uint32_t src2;
	uint32_t dst2;

	src2 = ((src << 16) |src) & 0x07E0F81F;
	dst2 = ((dst << 16) | dst) & 0x07E0F81F;
	dst2 = ((((dst2 - src2) * alpha) >> 5) + src2) & 0x07E0F81F;
	return (dst2 >> 16) | dst2;
}

/*
*********************************************************************************************************
*	�� �� ��: LCD_DrawIcon32
*	����˵��: ��LCD�ϻ���һ��ͼ��, ����͸����Ϣ��λͼ(32λ�� RGBA). ͼ���´�����
*	��    ��: _pIcon : ͼ��ṹ
*			  _tFont : ��������
*			  _ucFocusMode : ����ģʽ��0 ��ʾ����ͼ��  1��ʾѡ�е�ͼ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void LCD_DrawIcon32(const ICON_T *_tIcon, FONT_T *_tFont, uint8_t _ucFocusMode)
{
	const uint8_t *p;
	uint16_t usOldRGB, usNewRGB;
	int16_t x, y;		/* ���ڼ�¼�����ڵ�������� */
	uint8_t R1,G1,B1,A;	/* ������ɫ�ʷ��� */
	uint8_t R0,G0,B0;	/* ������ɫ�ʷ��� */

	p = (const uint8_t *)_tIcon->pBmp;
	p += 54;		/* ֱ��ָ��ͼ�������� */

	/* ����BMPλͼ���򣬴������ң���������ɨ�� */
	for (y = _tIcon->Height - 1; y >= 0; y--)
	{
		for (x = 0; x < _tIcon->Width; x++)
		{
			B1 = *p++;
			G1 = *p++;
			R1 = *p++;
			A = *p++;	/* Alpha ֵ(͸����)��0-255, 0��ʾ͸����1��ʾ��͸��, �м�ֵ��ʾ͸���� */

			if (A == 0x00)	/* ��Ҫ͸��,��ʾ���� */
			{
				;	/* ����ˢ�±��� */
			}
			else if (A == 0xFF)	/* ��ȫ��͸���� ��ʾ������ */
			{
				usNewRGB = RGB(R1, G1, B1);
				if (_ucFocusMode == 1)
				{
					usNewRGB = LCD_Blend565(usNewRGB, CL_YELLOW, 10);
				}
				LCD_PutPixel(x + _tIcon->Left, y + _tIcon->Top, usNewRGB);
			}
			else 	/* ��͸�� */
			{
				/* ���㹫ʽ�� ʵ����ʾ��ɫ = ǰ����ɫ * Alpha / 255 + ������ɫ * (255-Alpha) / 255 */
				usOldRGB = LCD_GetPixel(x + _tIcon->Left, y + _tIcon->Top);
				
				//usOldRGB = 0xFFFF;
				R0 = RGB565_R(usOldRGB);
				G0 = RGB565_G(usOldRGB);
				B0 = RGB565_B(usOldRGB);

				R1 = (R1 * A) / 255 + R0 * (255 - A) / 255;
				G1 = (G1 * A) / 255 + G0 * (255 - A) / 255;
				B1 = (B1 * A) / 255 + B0 * (255 - A) / 255;
				usNewRGB = RGB(R1, G1, B1);
				if (_ucFocusMode == 1)
				{
					usNewRGB = LCD_Blend565(usNewRGB, CL_YELLOW, 10);
				}
				LCD_PutPixel(x + _tIcon->Left, y + _tIcon->Top, usNewRGB);
			}
		}
	}

	/* ����ͼ���µ����� */
	{
		uint16_t len;
		uint16_t width;

		len = strlen(_tIcon->Text);

		if  (len == 0)
		{
			return;	/* ���ͼ���ı�����Ϊ0������ʾ */
		}

		/* �����ı����ܿ�� */
		if (_tFont->FontCode == FC_ST_12)		/* 12���� */
		{
			width = 6 * (len + _tFont->Space);
		}
		else	/* FC_ST_16 */
		{
			width = 8 * (len + _tFont->Space);
		}


		/* ˮƽ���� */
		x = (_tIcon->Left + _tIcon->Width / 2) - width / 2;
		y = _tIcon->Top + _tIcon->Height + 2;
		LCD_DispStr(x, y, (char *)_tIcon->Text, _tFont);
	}
}

/*
*********************************************************************************************************
*	�� �� ��: LCD_DrawBmp32
*	����˵��: ��LCD�ϻ���һ��32λ��BMPͼ, ����͸����Ϣ��λͼ(32λ�� RGBA)
*	��    ��: _usX, _usY : ��ʾ����
*			  _usHeight, _usWidth : ͼƬ�߶ȺͿ��
*			  _pBmp : ͼƬ���ݣ���BMP�ļ�ͷ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void LCD_DrawBmp32(uint16_t _usX, uint16_t _usY, uint16_t _usHeight, uint16_t _usWidth, uint8_t *_pBmp)
{
	const uint8_t *p;
	uint16_t usOldRGB, usNewRGB;
	int16_t x, y;		/* ���ڼ�¼�����ڵ�������� */
	uint8_t R1,G1,B1,A;	/* ������ɫ�ʷ��� */
	uint8_t R0,G0,B0;	/* ������ɫ�ʷ��� */

	p = (const uint8_t *)_pBmp;
	p += 54;		/* ֱ��ָ��ͼ�������� */

	/* ����BMPλͼ���򣬴������ң���������ɨ�� */
	for (y = _usHeight - 1; y >= 0; y--)
	{
		for (x = 0; x < _usWidth; x++)
		{
			B1 = *p++;
			G1 = *p++;
			R1 = *p++;
			A = *p++;	/* Alpha ֵ(͸����)��0-255, 0��ʾ͸����1��ʾ��͸��, �м�ֵ��ʾ͸���� */

			if (A == 0x00)	/* ��Ҫ͸��,��ʾ���� */
			{
				;	/* ����ˢ�±��� */
			}
			else if (A == 0xFF)	/* ��ȫ��͸���� ��ʾ������ */
			{
				usNewRGB = RGB(R1, G1, B1);
				//if (_ucFocusMode == 1)
				//{
				//	usNewRGB = Blend565(usNewRGB, CL_YELLOW, 10);
				//}
				LCD_PutPixel(x + _usX, y + _usY, usNewRGB);
			}
			else 	/* ��͸�� */
			{
				/* ���㹫ʽ�� ʵ����ʾ��ɫ = ǰ����ɫ * Alpha / 255 + ������ɫ * (255-Alpha) / 255 */
				usOldRGB = LCD_GetPixel(x + _usX, y + _usY);
				R0 = RGB565_R(usOldRGB);
				G0 = RGB565_G(usOldRGB);
				B0 = RGB565_B(usOldRGB);

				R1 = (R1 * A) / 255 + R0 * (255 - A) / 255;
				G1 = (G1 * A) / 255 + G0 * (255 - A) / 255;
				B1 = (B1 * A) / 255 + B0 * (255 - A) / 255;
				usNewRGB = RGB(R1, G1, B1);
				//if (_ucFocusMode == 1)
				//{
				//	usNewRGB = Blend565(usNewRGB, CL_YELLOW, 10);
				//}
				LCD_PutPixel(x + _usX, y + _usY, usNewRGB);
			}
		}
	}
}

/*
*********************************************************************************************************
*	�� �� ��: LCD_DrawLabel
*	����˵��: ����һ���ı���ǩ
*	��    ��: _pLabel : Label�ṹ��ָ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void LCD_InitLabel(LABEL_T *_pLabel, uint16_t _x, uint16_t _y, uint16_t _h, uint16_t _w, 
	char *_Text, FONT_T *_tFont)
{
	_pLabel->Left = _x;
	_pLabel->Top = _y;
	_pLabel->Height = _h;
	_pLabel->Width = _w;
	_pLabel->pCaption = _Text;
	_pLabel->Font = _tFont;
	
	_pLabel->MaxLen = 0;
}

/*
*********************************************************************************************************
*	�� �� ��: LCD_DrawLabel
*	����˵��: ����һ���ı���ǩ
*	��    ��: �ṹ��ָ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void LCD_DrawLabel(LABEL_T *_pLabel)
{
	char dispbuf[256];
	uint16_t i;
	uint16_t NewLen;

	NewLen = strlen(_pLabel->pCaption);

	if (NewLen > _pLabel->MaxLen)
	{
		LCD_DispStr(_pLabel->Left, _pLabel->Top, _pLabel->pCaption, _pLabel->Font);
		_pLabel->MaxLen = NewLen;
	}
	else
	{
		for (i = 0; i < NewLen; i++)
		{
			dispbuf[i] = _pLabel->pCaption[i];
		}
		for (; i < _pLabel->MaxLen; i++)
		{
			dispbuf[i] = ' ';		/* ĩβ���ո� */
		}
		dispbuf[i] = 0;
		LCD_DispStr(_pLabel->Left, _pLabel->Top, dispbuf, _pLabel->Font);
	}
}

/*
*********************************************************************************************************
*	�� �� ��: LCD_DrawCheckBox
*	����˵��: ����һ������
*	��    ��: �ṹ��ָ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void LCD_DrawCheckBox(CHECK_T *_pCheckBox)
{
	uint16_t x, y;

	/* Ŀǰֻ����16�����ֵĴ�С */

	/* ������� */
	x = _pCheckBox->Left;
	LCD_DrawRect(x, _pCheckBox->Top, CHECK_BOX_H, CHECK_BOX_W, CHECK_BOX_BORDER_COLOR);
	LCD_DrawRect(x + 1, _pCheckBox->Top + 1, CHECK_BOX_H - 2, CHECK_BOX_W - 2, CHECK_BOX_BORDER_COLOR);
	LCD_Fill_Rect(x + 2, _pCheckBox->Top + 2, CHECK_BOX_H - 4, CHECK_BOX_W - 4, CHECK_BOX_BACK_COLOR);

	/* �����ı���ǩ */
	x = _pCheckBox->Left + CHECK_BOX_W + 2;
	y = _pCheckBox->Top + CHECK_BOX_H / 2 - 8;
	LCD_DispStr(x, y, _pCheckBox->pCaption, _pCheckBox->Font);

	if (_pCheckBox->Checked)
	{
		FONT_T font;

	    font.FontCode = FC_ST_16;
		font.BackColor = CL_MASK;
		font.FrontColor = CHECK_BOX_CHECKED_COLOR;	/* ������ɫ */
		font.Space = 0;
		x = _pCheckBox->Left;
		LCD_DispStr(x + 3, _pCheckBox->Top + 3, "��", &font);
	}
}

/*
*********************************************************************************************************
*	�� �� ��: LCD_DrawEdit
*	����˵��: ��LCD�ϻ���һ���༭��
*	��    ��: _pEdit �༭��ṹ��ָ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void LCD_DrawEdit(EDIT_T *_pEdit)
{
	uint16_t len, x, y;
	
	/* ��XP���ƽ��༭�� */
	if (_pEdit->Focus == 0)
	{
		LCD_DrawRect(_pEdit->Left, _pEdit->Top, _pEdit->Height, _pEdit->Width, EDIT_BORDER_COLOR);
		LCD_Fill_Rect(_pEdit->Left + 1, _pEdit->Top + 1, _pEdit->Height - 2, _pEdit->Width - 2, EDIT_BACK_COLOR);
	}
	else
	{
		LCD_DrawRect(_pEdit->Left, _pEdit->Top, _pEdit->Height, _pEdit->Width, EDIT_BORDER_COLOR2);
		LCD_Fill_Rect(_pEdit->Left + 1, _pEdit->Top + 1, _pEdit->Height - 2, _pEdit->Width - 2, EDIT_BACK_COLOR2);
	}	
	
	if (_pEdit->pCaption > 0)
	{
		for (len = 0; len < 32; len++)
		{
			_pEdit->Text[len] = _pEdit->pCaption[len];
			
			if (_pEdit->pCaption[len] == 0)
			{
				break;
			}
		}
		_pEdit->Text[32] = 0;
		
		//_pEdit->pCaption = 0;
	}
	
	/* ���־��� */
	len = LCD_GetStrWidth(_pEdit->Text,  _pEdit->Font);
	x = _pEdit->Left +  (_pEdit->Width - len) / 2;
	y = _pEdit->Top + (_pEdit->Height - LCD_GetFontHeight(_pEdit->Font)) / 2;

	LCD_DispStr(x, y, _pEdit->Text, _pEdit->Font);
}

/*
*********************************************************************************************************
*	�� �� ��: LCD_EditTouchDown
*	����˵��: �жϰ�ť�Ƿ񱻰���. ��鴥�������Ƿ��ڰ�ť�ķ�Χ֮�ڡ����ػ水ť��
*	��    ��:  _edit : �༭�����
*			  _usX, _usY: ��������
*	�� �� ֵ: 1 ��ʾ�ڷ�Χ��
*********************************************************************************************************
*/
uint8_t LCD_EditTouchDown(EDIT_T *_Edit, uint16_t _usX, uint16_t _usY)
{
	if ((_usX > _Edit->Left) && (_usX < _Edit->Left + _Edit->Width)
		&& (_usY > _Edit->Top) && (_usY < _Edit->Top + _Edit->Height))
	{
		BUTTON_BEEP();	/* ������ʾ�� bsp_tft_lcd.h �ļ���ͷ����ʹ�ܺ͹ر� */
		_Edit->Focus = 1;
		LCD_DrawEdit(_Edit);
		return 1;
	}
	else
	{
		return 0;
	}
}

/*
*********************************************************************************************************
*	�� �� ��: LCD_EditTouchRelease
*	����˵��: �༭���˳��༭״̬���ػ�
*	��    ��:  _Edit : �༭�����
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void LCD_EditRefresh(EDIT_T *_Edit)
{
	_Edit->Focus = 0;
	LCD_DrawEdit(_Edit);
}

/*
*********************************************************************************************************
*	�� �� ��: LCD_InitGroupBox
*	����˵��: ��ʼ����������
*	��    ��: _pBox �����
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void LCD_InitGroupBox(GROUP_T *_pBox, uint16_t _x, uint16_t _y, uint16_t _h, uint16_t _w,
	char  *pCaption, FONT_T *Font)
{
	_pBox->Left = _x;
	_pBox->Top = _y;
	_pBox->Height = _h;
	_pBox->Width = _w;
	_pBox->pCaption = pCaption;
	_pBox->Font = Font;
}

/*
*********************************************************************************************************
*	�� �� ��: LCD_DrawGroupBox
*	����˵��: ��LCD�ϻ���һ�������
*	��    ��: _pBox �����
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void LCD_DrawGroupBox(GROUP_T *_pBox)
{
	uint16_t x, y;
	uint16_t x1,y1;		/* �������Ͻ� */
	uint16_t x2, y2;	/* �������½� */
	uint16_t len;

	
	len = LCD_GetStrWidth(_pBox->pCaption, _pBox->Font);	/* �ַ������ܿ�� */
	
	/* ����Ӱ�� */
	//LCD_DrawRect(_pBox->Left + 1, _pBox->Top + 5, _pBox->Height, _pBox->Width - 1, CL_BOX_BORDER2);
	x1 = _pBox->Left + 1;
	y1 = _pBox->Top + 5;
	x2 = _pBox->Left + 1 + _pBox->Width - 2;
	y2 = _pBox->Top + 5 + _pBox->Height - 1;
	
	LCD_DrawLine(x1, y1, x1 + 6, y1, CL_BOX_BORDER2);	/* ��1 */
	LCD_DrawLine(x1 + 8 + len + 1, y1, x2, y1, CL_BOX_BORDER2);	/* ��2 */	
	LCD_DrawLine(x1, y2, x2, y2, CL_BOX_BORDER2);	/* �� */
	LCD_DrawLine(x1, y1, x1, y2, CL_BOX_BORDER2);	/* �� */
	LCD_DrawLine(x2, y1, x2, y2, CL_BOX_BORDER2);	/* �� */	

	/* �������� */
	//LCD_DrawRect(_pBox->Left, _pBox->Top + 4, _pBox->Height, _pBox->Width - 1, CL_BOX_BORDER1);
	x1 = _pBox->Left;
	y1 = _pBox->Top + 4;
	x2 = _pBox->Left + _pBox->Width - 2;
	y2 = _pBox->Top + 4 + _pBox->Height - 1;
	
	LCD_DrawLine(x1, y1, x1 + 6, y1, CL_BOX_BORDER1);	/* ��1 */
	LCD_DrawLine(x1 + 9 + len + 1, y1, x2, y1, CL_BOX_BORDER1);	/* ��2 */	
	LCD_DrawLine(x1, y2, x2, y2, CL_BOX_BORDER1);	/* �� */
	LCD_DrawLine(x1, y1, x1, y2, CL_BOX_BORDER1);	/* �� */
	LCD_DrawLine(x2, y1, x2, y2, CL_BOX_BORDER1);	/* �� */		

	/* ��ʾ�������⣨���������Ͻǣ� */
	x = _pBox->Left + 9;
	y = _pBox->Top;
	LCD_DispStr(x, y, _pBox->pCaption, _pBox->Font);
}

/*
*********************************************************************************************************
*	�� �� ��: LCD_DispControl
*	����˵��: ���ƿؼ�
*	��    ��: _pControl �ؼ�ָ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void LCD_DispControl(void *_pControl)
{
	uint8_t id;

	id = *(uint8_t *)_pControl;	/* ��ȡID */

	switch (id)
	{
		case ID_ICON:
			//void LCD_DrawIcon(const ICON_T *_tIcon, FONT_T *_tFont, uint8_t _ucFocusMode);
			break;

		case ID_WIN:
			LCD_DrawWin((WIN_T *)_pControl);
			break;

		case ID_LABEL:
			LCD_DrawLabel((LABEL_T *)_pControl);
			break;

		case ID_BUTTON:
			LCD_DrawButton((BUTTON_T *)_pControl);
			break;

		case ID_CHECK:
			LCD_DrawCheckBox((CHECK_T *)_pControl);
			break;

		case ID_EDIT:
			LCD_DrawEdit((EDIT_T *)_pControl);
			break;

		case ID_GROUP:
			LCD_DrawGroupBox((GROUP_T *)_pControl);
			break;
	}
}

/*
*********************************************************************************************************
*	�� �� ��: LCD_InitButton
*	����˵��: ��ʼ����ť�ṹ���Ա��
*	��    ��:  _x, _y : ����
*			  _h, _w : �߶ȺͿ��
*			  _pCaption : ��ť����
*			  _pFont : ��ť����
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void LCD_InitButton(BUTTON_T *_btn, uint16_t _x, uint16_t _y, uint16_t _h, uint16_t _w, char *_pCaption, FONT_T *_pFont)
{
	_btn->Left = _x;
	_btn->Top = _y;
	_btn->Height = _h;
	_btn->Width = _w;
	_btn->pCaption = _pCaption;	
	_btn->Font = _pFont;
	_btn->Focus = 0;
}

/*
*********************************************************************************************************
*	�� �� ��: LCD_DrawButton
*	����˵��: ��LCD�ϻ���һ����ť������emwin��ť
*	��    ��:
*			_usX, _usY : ͼƬ������
*			_usHeight  : ͼƬ�߶�
*			_usWidth   : ͼƬ���
*			_ptr       : ͼƬ����ָ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void LCD_DrawButton(BUTTON_T *_pBtn)
{
	uint16_t x, y;	
	uint8_t muti_line = 0;
		
	{
		uint16_t i;
		
		for (i = 0; i < 1024; i++)
		{
			if (_pBtn->pCaption[i] == '\r' || _pBtn->pCaption[i] == '\t')
			{
				muti_line = 1;
				break;
			}
			if (_pBtn->pCaption[i] == 0)
			{
				break;
			}
		}
	}
	x = _pBtn->Left;		
	if (muti_line == 0)
	{
		y = _pBtn->Top + (_pBtn->Height - LCD_GetFontHeight(_pBtn->Font)) / 2;	/* �����ı���ֱ���� */
	}
	else
	{
		y = _pBtn->Top;		/* �����ı�,��ֱ����Ӷ�����ʼ */
	}	
		
//	if (g_ChipID == IC_8875)
//	{
//		uint8_t Arc = 5;
//				
//		if (_pBtn->Focus == 0)
//		{
//			RA8875_DrawRoundRect(_pBtn->Left, _pBtn->Top, _pBtn->Height, _pBtn->Width, Arc,  BTN_BORDER_COLOR1);
//			RA8875_FillRoundRect(_pBtn->Left + 1, _pBtn->Top + 1, _pBtn->Height - 2, _pBtn->Width - 2, Arc,  BTN_BODY_COLOR1);			
//			LCD_Fill_Rect(_pBtn->Left + Arc, _pBtn->Top + 1, _pBtn->Height / 2, _pBtn->Width - 2 * Arc, BTN_SHADOW_COLOR1);	/* ����Ӱ�Ա�ɫ */
//		}
//		else
//		{
//			RA8875_DrawRoundRect(_pBtn->Left, _pBtn->Top, _pBtn->Height, _pBtn->Width, Arc,  BTN_BORDER_COLOR2);
//			RA8875_FillRoundRect(_pBtn->Left + 1, _pBtn->Top + 1, _pBtn->Height - 2, _pBtn->Width - 2, Arc, BTN_BODY_COLOR2);			
//			LCD_Fill_Rect(_pBtn->Left + Arc, _pBtn->Top + 1, _pBtn->Height / 2, _pBtn->Width - 2 * Arc, BTN_SHADOW_COLOR2);	/* ����Ӱ�Ա�ɫ */
//		}

//		RA8875_SetTextTransp(1);		
//		LCD_DispStrEx(x, y, _pBtn->pCaption, _pBtn->Font, _pBtn->Width, ALIGN_CENTER);
//		RA8875_SetTextTransp(0);
//	}
//	else if (g_ChipID == IC_8876)
//	{
//		uint8_t Arc = 5;
//		
//		if (_pBtn->Focus == 0)
//		{
//			RA8876_DrawRoundRect(_pBtn->Left, _pBtn->Top, _pBtn->Height, _pBtn->Width, Arc,  BTN_BORDER_COLOR1);
//			RA8876_FillRoundRect(_pBtn->Left + 1, _pBtn->Top + 1, _pBtn->Height - 2, _pBtn->Width - 2, Arc,  BTN_BODY_COLOR1);			
//			LCD_Fill_Rect(_pBtn->Left + Arc, _pBtn->Top + 1, _pBtn->Height / 2, _pBtn->Width - 2 * Arc, BTN_SHADOW_COLOR1);	/* ����Ӱ�Ա�ɫ */
//		}
//		else
//		{
//			RA8876_DrawRoundRect(_pBtn->Left, _pBtn->Top, _pBtn->Height, _pBtn->Width, Arc,  BTN_BORDER_COLOR2);
//			RA8876_FillRoundRect(_pBtn->Left + 1, _pBtn->Top + 1, _pBtn->Height - 2, _pBtn->Width - 2, Arc, BTN_BODY_COLOR2);			
//			LCD_Fill_Rect(_pBtn->Left + Arc, _pBtn->Top + 1, _pBtn->Height / 2, _pBtn->Width - 2 * Arc, BTN_SHADOW_COLOR2);	/* ����Ӱ�Ա�ɫ */
//		}
//	
//		RA8876_SetTextTransp(1);
//		LCD_DispStrEx(x, y, _pBtn->pCaption, _pBtn->Font, _pBtn->Width, ALIGN_CENTER);
//		RA8876_SetTextTransp(0);
//	}
//	else
	{
		uint8_t Arc = 5;
			
		if (_pBtn->Focus == 0)
		{
			LCD_FillRoundRect(_pBtn->Left, _pBtn->Top, _pBtn->Height, _pBtn->Width, Arc,  BTN_BODY_COLOR1);			
			LCD_DrawRoundRect(_pBtn->Left, _pBtn->Top, _pBtn->Height, _pBtn->Width, Arc,  BTN_BORDER_COLOR1);											
			LCD_Fill_Rect(_pBtn->Left + Arc, _pBtn->Top + 1, _pBtn->Height / 2, _pBtn->Width - 2 * Arc, BTN_SHADOW_COLOR1);	/* ����Ӱ�Ա�ɫ */
		}
		else
		{
			LCD_FillRoundRect(_pBtn->Left, _pBtn->Top, _pBtn->Height, _pBtn->Width, Arc,  BTN_BODY_COLOR2);			
			LCD_DrawRoundRect(_pBtn->Left, _pBtn->Top, _pBtn->Height, _pBtn->Width, Arc,  BTN_BORDER_COLOR2);	
			LCD_Fill_Rect(_pBtn->Left + Arc, _pBtn->Top + 1, _pBtn->Height / 2, _pBtn->Width - 2 * Arc, BTN_SHADOW_COLOR2);	/* ����Ӱ�Ա�ɫ */
		}

		LCD_DispStrEx(x, y, _pBtn->pCaption, _pBtn->Font, _pBtn->Width, ALIGN_CENTER);
	}
}

/*
*********************************************************************************************************
*	�� �� ��: LCD_ButtonTouchDown
*	����˵��: �жϰ�ť�Ƿ񱻰���. ��鴥�������Ƿ��ڰ�ť�ķ�Χ֮�ڡ����ػ水ť��
*	��    ��:  _btn : ��ť����
*			  _usX, _usY: ��������
*	�� �� ֵ: 1 ��ʾ�ڷ�Χ��
*********************************************************************************************************
*/
uint8_t LCD_ButtonTouchDown(BUTTON_T *_btn, uint16_t _usX, uint16_t _usY)
{
	if ((_usX > _btn->Left) && (_usX < _btn->Left + _btn->Width)
		&& (_usY > _btn->Top) && (_usY < _btn->Top + _btn->Height))
	{
		BUTTON_BEEP();	/* ������ʾ�� bsp_tft_lcd.h �ļ���ͷ����ʹ�ܺ͹ر� */
		_btn->Focus = 1;
		LCD_DrawButton(_btn);
		return 1;
	}
	else
	{
		return 0;
	}
}

/*
*********************************************************************************************************
*	�� �� ��: LCD_ButtonTouchRelease
*	����˵��: �жϰ�ť�Ƿ񱻴����ͷ�. ���ػ水ť���ڴ����ͷ��¼��б����á�
*	��    ��:  _btn : ��ť����
*			  _usX, _usY: ��������
*	�� �� ֵ: 1 ��ʾ�ڷ�Χ��
*********************************************************************************************************
*/
uint8_t LCD_ButtonTouchRelease(BUTTON_T *_btn, uint16_t _usX, uint16_t _usY)
{
	/* 2016-04-24 �������� */
	if (_btn->Focus != 0)
	{
		_btn->Focus = 0;
		LCD_DrawButton(_btn);
	}
	
	if ((_usX > _btn->Left) && (_usX < _btn->Left + _btn->Width)
		&& (_usY > _btn->Top) && (_usY < _btn->Top + _btn->Height))
	{

		return 1;
	}
	else
	{
		return 0;
	}
}

/*
*********************************************************************************************************
*	�� �� ��: LCD_DrawBmpButton
*	����˵��: ��LCD�ϻ���һ��ͼƬ��ť
*	��    ��:
*			_usX, _usY : ͼƬ������
*			_usHeight  : ͼƬ�߶�
*			_usWidth   : ͼƬ���
*			_ptr       : ͼƬ����ָ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void LCD_DrawBmpButton(BMP_BUTTON_T *_pBtn)
{
//	if (_pBtn->Focus == 1)
//	{
//		RA8875_DispBmpInFlash(_pBtn->Left, _pBtn->Top, _pBtn->Height, _pBtn->Width, _pBtn->Pic2);
//	}
//	else
//	{
//		RA8875_DispBmpInFlash(_pBtn->Left, _pBtn->Top, _pBtn->Height, _pBtn->Width, _pBtn->Pic1);
//	}
}

/*
*********************************************************************************************************
*	�� �� ��: LCD_BmpButtonTouchDown
*	����˵��: �ж�ͼƬ��ť��ť�Ƿ񱻰���. ��鴥�������Ƿ��ڰ�ť�ķ�Χ֮�ڡ����ػ水ť��
*	��    ��:  _btn : ��ť����
*			  _usX, _usY: ��������
*	�� �� ֵ: 1 ��ʾ�ڷ�Χ��
*********************************************************************************************************
*/
uint8_t LCD_BmpButtonTouchDown(BMP_BUTTON_T *_btn, uint16_t _usX, uint16_t _usY)
{
	if ((_usX > _btn->Left) && (_usX < _btn->Left + _btn->Width)
		&& (_usY > _btn->Top) && (_usY < _btn->Top + _btn->Height))
	{
		BUTTON_BEEP();	/* ������ʾ�� bsp_tft_lcd.h �ļ���ͷ����ʹ�ܺ͹ر� */
		_btn->Focus = 1;
		LCD_DrawBmpButton(_btn);
		return 1;
	}
	else
	{
		return 0;
	}
}

/*
*********************************************************************************************************
*	�� �� ��: LCD_BmpButtonTouchRelease
*	����˵��: �ж�ͼƬ��ť�Ƿ񱻴����ͷ�. ���ػ水ť���ڴ����ͷ��¼��б����á�
*	��    ��:  _btn : ��ť����
*			  _usX, _usY: ��������
*	�� �� ֵ: 1 ��ʾ�ڷ�Χ��
*********************************************************************************************************
*/
uint8_t LCD_BmpButtonTouchRelease(BMP_BUTTON_T *_btn, uint16_t _usX, uint16_t _usY)
{
	_btn->Focus = 0;
	LCD_DrawBmpButton(_btn);
	
	if ((_usX > _btn->Left) && (_usX < _btn->Left + _btn->Width)
		&& (_usY > _btn->Top) && (_usY < _btn->Top + _btn->Height))
	{
		return 1;
	}
	else
	{
		return 0;
	}
}

/*
*********************************************************************************************************
*	�� �� ��: LCD_SelectTouchDown
*	����˵��: ѡ��ҪУ׼�Ĳ���
*	��    ��:  _slt : ѡ�ж���
*			  _usX, _usY: ��������
*	�� �� ֵ: 1 ��ʾ�ڷ�Χ��
*********************************************************************************************************
*/
uint8_t LCD_SelectTouchDown(SELECT_T *_slt, uint16_t _usX, uint16_t _usY)
{
	if ((_usX > _slt->Left) && (_usX < _slt->Left + _slt->Width)
		&& (_usY > _slt->Top) && (_usY < _slt->Top + _slt->Height))
	{
		BUTTON_BEEP();
		return 1;
	}
	else
	{
		return 0;
	}
}

/*
*********************************************************************************************************
*	�� �� ��: LCD_InitPannel
*	����˵��: ��ʼ�����ṹ���Ա��
*	��    ��: _panl : pannel ����
*			  _x, _y : ����
*			  _h, _w : �߶ȺͿ��
*			  _pCaption : ��ť����
*			  _pFont : ��ť����
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void LCD_InitPannel(PANNEL_T *_pnl, uint16_t _x, uint16_t _y, uint16_t _h, uint16_t _w, uint16_t _arc, uint16_t _color)
{
	_pnl->Left = _x;
	_pnl->Top = _y;
	_pnl->Height = _h;
	_pnl->Width = _w;
	_pnl->Arc = _arc;	
	_pnl->Color = _color;
}

/*
*********************************************************************************************************
*	�� �� ��: LCD_DrawPannel
*	����˵��: ��LCD�ϻ���һ�����
*	��    ��: 
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void LCD_DrawPannel(PANNEL_T *_pnl)
{
//	if (g_ChipID == IC_8875)
//	{
//		/* ����һ��Բ�Ǿ��Σ�����ɫ */
//		RA8875_FillRoundRect(_pnl->Left, _pnl->Top, _pnl->Height, _pnl->Width, _pnl->Arc, _pnl->Color);
//	}
//	else
//	{
//		;	
//	}
}

/*
*********************************************************************************************************
*	�� �� ��: LCD_PannelClick
*	����˵��: �ж�Pannel�Ƿ񱻵��. ��鴥�������Ƿ��ڰ�ť�ķ�Χ֮�ڡ�
*	��    ��:  _obj : PANNEL����
*			  _usX, _usY: ��������
*	�� �� ֵ: 1 ��ʾ�ڷ�Χ�� 0��ʾ����
*********************************************************************************************************
*/
uint8_t LCD_PannelClick(PANNEL_T *_obj, uint16_t _usX, uint16_t _usY)
{
	if ((_usX > _obj->Left) && (_usX < _obj->Left + _obj->Width)
		&& (_usY > _obj->Top) && (_usY < _obj->Top + _obj->Height))
	{
		return 1;
	}
	else
	{
		return 0;
	}
}

/*
*********************************************************************************************************
*	�� �� ��: LCD_LabelClick
*	����˵��: �ж�Label�Ƿ񱻵��. ��鴥�������Ƿ��ڰ�ť�ķ�Χ֮�ڡ�
*	��    ��:  _obj : PANNEL����
*			  _usX, _usY: ��������
*	�� �� ֵ: 1 ��ʾ�ڷ�Χ�� 0��ʾ����
*********************************************************************************************************
*/
uint8_t LCD_LabelClick(LABEL_T *_obj, uint16_t _usX, uint16_t _usY)
{
	if ((_usX > _obj->Left) && (_usX < _obj->Left + _obj->Width)
		&& (_usY > _obj->Top) && (_usY < _obj->Top + _obj->Height))
	{
		return 1;
	}
	else
	{
		return 0;
	}
}

/*
*********************************************************************************************************
*	�� �� ��: LCD_DrawArc
*	����˵��: ����һ��Բ�����ʿ�Ϊ1������
*	��    ��:
*			_usX,_usY  ��Բ�ĵ�����
*			_usRadius  ��Բ�İ뾶
*			_StartAng  : ��ʼ�Ƕ�
*			_EndAng	   : ��ֹ�Ƕ�
*			_usColor   : Բ����ɫ
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void LCD_DrawArc(uint16_t _usX, uint16_t _usY, uint16_t _usRadius, float _StartRnd, float _EndRnd, uint16_t _usColor)
{
	float CurX,CurY,rnd;
	
	rnd = _StartRnd;    
	while (rnd <= _EndRnd)        
	{
		CurX = _usRadius * cos(rnd);         
		CurY = _usRadius * sin(rnd);          
		LCD_PutPixel(_usX + CurX,_usY - CurY, _usColor);            
		rnd = rnd + 0.01f;         
	} 
}


/*
*********************************************************************************************************
*	�� �� ��: LCD_DrawQuterCircle
*	����˵��: ����һ��1/4Բ���ʿ�Ϊ1������, ʹ������㷨����
*	��    ��:
*			_usX,_usY  : Բ�ĵ�����
*			_usRadius  : Բ�İ뾶
*			_ucMode    : 0 ��ʾ���Ͻ�1/4Բ 1��ʾ���Ͻ�  2��ʾ���½� 3��ʾ���½�
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void LCD_DrawQuterCircle(uint16_t _usX, uint16_t _usY, uint16_t _usRadius, uint16_t _usColor, uint8_t _ucMode)
{
	int32_t  D;			/* Decision Variable */
	uint32_t  CurX;		/* ��ǰ X ֵ */
	uint32_t  CurY;		/* ��ǰ Y ֵ */

	D = 3 - (_usRadius << 1);
	
	CurX = 0;
	CurY = _usRadius;

	while (CurX <= CurY)
	{	
		if (_ucMode == 0)
		{
			LCD_PutPixel(_usX - CurY, _usY - CurX, _usColor);   // �� -> ��
			LCD_PutPixel(_usX - CurX, _usY - CurY, _usColor);   // �� -> ��
		}
		else if (_ucMode == 1)
		{
			LCD_PutPixel(_usX + CurX, _usY - CurY, _usColor);	// �� -> ��
			LCD_PutPixel(_usX + CurY, _usY - CurX, _usColor);	// �� -> ��	
		}
		else if (_ucMode == 2)
		{
			LCD_PutPixel(_usX + CurX, _usY + CurY, _usColor);	// �� -> ��
			LCD_PutPixel(_usX + CurY, _usY + CurX, _usColor);	// �� -> ��
		}
		else if (_ucMode == 3)
		{			
			LCD_PutPixel(_usX - CurX, _usY + CurY, _usColor);	// �� -> ��
			LCD_PutPixel(_usX - CurY, _usY + CurX, _usColor);    // �� -> ��
		}
		
		if (D < 0)
		{
			D += (CurX << 2) + 6;
		}
		else
		{
			D += ((CurX - CurY) << 2) + 10;
			CurY--;
		}
		CurX++;
	}
}

/*
*********************************************************************************************************
*	�� �� ��: LCD_FillQuterCircle
*	����˵��: ���һ��1/4Բ������㷨ʵ�֡�
*	��    ��:
*			_usX,_usY  : Բ�ĵ�����
*			_usRadius  : Բ�İ뾶
*			_usColor   : ������ɫ
*			_ucMode    : 0 ��ʾ���Ͻ�1/4Բ 1��ʾ���Ͻ�  2��ʾ���½� 3��ʾ���½�
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void LCD_FillQuterCircle(uint16_t _usX, uint16_t _usY, uint16_t _usRadius, uint16_t _usColor, uint8_t _ucMode)
{
	int32_t  D;
	uint32_t  CurX;		/* ��ǰ X ֵ */
	uint32_t  CurY;		/* ��ǰ Y ֵ */

	D = 3 - (_usRadius << 1);
	CurX = 0;
	CurY = _usRadius;

	while (CurX <= CurY)
	{			
		if (_ucMode == 0)
		{
			LCD_DrawLine(_usX - CurY, _usY - CurX, _usX, _usY - CurX, _usColor);   // �� -> ��
			LCD_DrawLine(_usX - CurX, _usY - CurY, _usX, _usY - CurY, _usColor);   // �� -> ��
		}
		else if (_ucMode == 1)
		{
			LCD_DrawLine(_usX + CurX, _usY - CurY, _usX, _usY - CurY, _usColor);	// �� -> ��
			LCD_DrawLine(_usX + CurY, _usY - CurX, _usX, _usY - CurX, _usColor);	// �� -> ��	
		}
		else if (_ucMode == 2)
		{
			LCD_DrawLine(_usX + CurX, _usY + CurY, _usX, _usY + CurY, _usColor);	// �� -> ��
			LCD_DrawLine(_usX + CurY, _usY + CurX, _usX, _usY + CurX, _usColor);	// �� -> ��
		}
		else if (_ucMode == 3)
		{			
			LCD_DrawLine(_usX - CurX, _usY + CurY, _usX, _usY + CurY, _usColor);	// �� -> ��
			LCD_DrawLine(_usX - CurY, _usY + CurX, _usX, _usY + CurX, _usColor);    // �� -> ��
		}		
		
		if (D < 0)
		{
			D += (CurX << 2) + 6;
		}
		else
		{
			D += ((CurX - CurY) << 2) + 10;
			CurY--;
		}
		CurX++;
	}
}

/*
*********************************************************************************************************
*	�� �� ��: LCD_DrawRoundRect
*	����˵��: ����Բ�Ǿ����������ʿ��1����
*	��    ��:
*			_usX,_usY:�������Ͻǵ�����
*			_usHeight :���εĸ߶�
*			_usWidth  :���εĿ��
*			_usArc    :Բ�ǵĻ��뾶
*			_usColor  :��ɫ
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void LCD_DrawRoundRect(uint16_t _usX, uint16_t _usY, uint16_t _usHeight, uint16_t _usWidth, 
	uint16_t _usRadius, uint16_t _usColor)
{
	if (_usHeight < 2 *_usRadius)
	{
		_usHeight = 2 *_usRadius;
	}

	if (_usWidth < 2 *_usRadius)
	{
		_usWidth = 2 *_usRadius;
	}	
	
	LCD_DrawQuterCircle(_usX + _usRadius, _usY + _usRadius, _usRadius, _usColor, 0);	/* ���ϽǵĻ� */
	LCD_DrawLine(_usX + _usRadius, _usY, _usX + _usWidth - _usRadius - 1, _usY, _usColor);
	
	LCD_DrawQuterCircle(_usX + _usWidth - _usRadius - 1, _usY + _usRadius, _usRadius, _usColor, 1);	/* ���ϽǵĻ� */
	LCD_DrawLine(_usX + _usWidth - 1, _usY + _usRadius, _usX + _usWidth - 1, _usY + _usHeight  - _usRadius - 1, _usColor);
	
	LCD_DrawQuterCircle(_usX + _usWidth - _usRadius - 1, _usY + _usHeight - _usRadius - 1, _usRadius, _usColor, 2);	/* ���½ǵĻ� */
	LCD_DrawLine(_usX + _usRadius, _usY + _usHeight - 1, _usX + _usWidth - _usRadius - 1, _usY + _usHeight - 1, _usColor);
			
	LCD_DrawQuterCircle(_usX + _usRadius,  _usY + _usHeight - _usRadius - 1, _usRadius, _usColor, 3);	/* ���½ǵĻ� */
	LCD_DrawLine(_usX, _usY + _usRadius, _usX,  _usY + _usHeight - _usRadius - 1, _usColor);
}


/*
*********************************************************************************************************
*	�� �� ��: LCD_FillRoundRect
*	����˵��: ���Բ�Ǿ���
*	��    ��:
*			_usX,_usY:�������Ͻǵ�����
*			_usHeight :���εĸ߶�
*			_usWidth  :���εĿ��
*			_usArc    :Բ�ǵĻ��뾶
*			_usColor  :��ɫ
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void LCD_FillRoundRect(uint16_t _usX, uint16_t _usY, uint16_t _usHeight, uint16_t _usWidth, 
	uint16_t _usRadius, uint16_t _usColor)
{
	if (_usHeight < 2 *_usRadius)
	{
		_usHeight = 2 *_usRadius;
	}

	if (_usWidth < 2 *_usRadius)
	{
		_usWidth = 2 *_usRadius;
	}	
	
	LCD_FillQuterCircle(_usX + _usRadius, _usY + _usRadius, _usRadius, _usColor, 0);	/* ���ϽǵĻ� */

	LCD_Fill_Rect(_usX + _usRadius + 1,  _usY,  _usRadius + 1, _usWidth - 2 * _usRadius - 2, _usColor);
	
	LCD_FillQuterCircle(_usX + _usWidth - _usRadius - 1, _usY + _usRadius, _usRadius, _usColor, 1);	/* ���ϽǵĻ� */

	LCD_Fill_Rect(_usX, _usY + _usRadius, _usHeight - 2 * _usRadius, _usWidth, _usColor);

	LCD_FillQuterCircle(_usX + _usWidth - _usRadius - 1, _usY + _usHeight - _usRadius - 1, _usRadius, _usColor, 2);	/* ���½ǵĻ� */

	LCD_Fill_Rect(_usX + _usRadius + 1,  _usY + _usHeight - _usRadius - 1,  _usRadius + 1, _usWidth - 2 * _usRadius - 2, _usColor);	

	LCD_FillQuterCircle(_usX + _usRadius,  _usY + _usHeight - _usRadius - 1, _usRadius, _usColor, 3);	/* ���½ǵĻ� */
}


/***************************** ���������� www.armfly.com (END OF FILE) *********************************/
