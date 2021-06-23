/*
*********************************************************************************************************
*
*	ģ������ : ST7789 TFT SPI�ӿ���������
*	�ļ����� : bsp_tft_st7789.c
*	��    �� : V1.0
*	˵    �� : SPI�ӿڣ���ʾ����ICΪST7789���ֱ���Ϊ240*240,1.3��ISP
*	�޸ļ�¼ :
*		�汾��  ����       ����    ˵��
*		V1.0	2018-12-06 armfly 
*		V1.1	2019-03-25 armfly ���SPI���Ż�ִ���ٶ�
*
*	Copyright (C), 2018-2030, ���������� www.armfly.com
*
*********************************************************************************************************
*/

#include "bsp.h"
#include "fonts.h"
#include "param.h"

/*
	H7-TOOL LCD���߷���
	
	PF3  --->  LCD_RS
	PG2	 --->  LCD_CS
	PC12 --->  LCD_SDA		SPI3_MOSI
	PC10 --->  LCD_SCK		SPI3_SCK
	
	PH9	 --->  BACK_LIGHT	TIM12_CH2
	
	��2������reset
	
	PB6  --->  LCD_RESET
*/


#define ALL_LCD_GPIO_CLK_ENABLE()	__HAL_RCC_GPIOC_CLK_ENABLE();	\
									__HAL_RCC_GPIOF_CLK_ENABLE();	\
									__HAL_RCC_GPIOG_CLK_ENABLE();	\
									__HAL_RCC_GPIOB_CLK_ENABLE();

/* LCD_RS �Ĵ���ѡ�� */
#define LCD_RS_GPIO		GPIOF
#define LCD_RS_PIN		GPIO_PIN_3
#define LCD_RS_0()  	LCD_RS_GPIO->BSRRH = LCD_RS_PIN	
#define LCD_RS_1()  	LCD_RS_GPIO->BSRRL = LCD_RS_PIN

/* LCD_CS SPIƬѡ*/
#define LCD_CS_GPIO		GPIOG
#define LCD_CS_PIN		GPIO_PIN_2
#define LCD_CS_0()  	LCD_CS_GPIO->BSRRH = LCD_CS_PIN	
#define LCD_CS_1()  	LCD_CS_GPIO->BSRRL = LCD_CS_PIN

/* SPI �ӿ� PC12/SPI3_MOSI��  PC10/SPI3_SCK */
#define LCD_SCK_GPIO	GPIOC
#define LCD_SCK_PIN		GPIO_PIN_10
#define LCD_SCK_0()  	LCD_SCK_GPIO->BSRRH = LCD_SCK_PIN	
#define LCD_SCK_1()  	LCD_SCK_GPIO->BSRRL = LCD_SCK_PIN

#define LCD_SDA_GPIO	GPIOC
#define LCD_SDA_PIN		GPIO_PIN_12
#define LCD_SDA_0()  	LCD_SDA_GPIO->BSRRH = LCD_SDA_PIN	
#define LCD_SDA_1()  	LCD_SDA_GPIO->BSRRL = LCD_SDA_PIN

/* LCD_RESET ��λ */
#define LCD_RESET_GPIO	GPIOB
#define LCD_RESET_PIN	GPIO_PIN_6
#define LCD_RESET_0()  	LCD_RESET_GPIO->BSRRH = LCD_RESET_PIN	
#define LCD_RESET_1()  	LCD_RESET_GPIO->BSRRL = LCD_RESET_PIN

static void ST7789_ConfigGPIO(void);
static void ST7789_initial(void);
static void ST7789_SendByteQuick(uint8_t data);

/*
*********************************************************************************************************
*	�� �� ��: ST7789_InitHard
*	����˵��: ��ʼ��LCD
*	��    ��: ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void ST7789_InitHard(void)
{
	ST7789_ConfigGPIO();			/* ����429 CPU�ڲ�LTDC */
	
	ST7789_initial();
	
	g_LcdHeight = 240;			/* ��ʾ���ֱ���-�߶� */
	g_LcdWidth = 240;			/* ��ʾ���ֱ���-��� */
}

/*
*********************************************************************************************************
*	�� �� ��: ST7789_ConfigLTDC
*	����˵��: ����LTDC
*	��    ��: ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
static void ST7789_ConfigGPIO(void)
{
	/* ����GPIO */
	{
		GPIO_InitTypeDef gpio_init;

		/* ��GPIOʱ�� */
		ALL_LCD_GPIO_CLK_ENABLE();
		
		LCD_CS_1();
		LCD_SCK_1();
		
		gpio_init.Mode = GPIO_MODE_OUTPUT_PP;		/* ����������� */
		gpio_init.Pull = GPIO_NOPULL;				/* ���������費ʹ�� */
		gpio_init.Speed = GPIO_SPEED_FREQ_HIGH;  	/* GPIO�ٶȵȼ� */	
		
		gpio_init.Pin = LCD_RS_PIN;	
		HAL_GPIO_Init(LCD_RS_GPIO, &gpio_init);	
		
		gpio_init.Pin = LCD_CS_PIN;	
		HAL_GPIO_Init(LCD_CS_GPIO, &gpio_init);	

		gpio_init.Pin = LCD_SCK_PIN;	
		HAL_GPIO_Init(LCD_SCK_GPIO, &gpio_init);	

		gpio_init.Pin = LCD_SDA_PIN;	
		HAL_GPIO_Init(LCD_SDA_GPIO, &gpio_init);	

		gpio_init.Pin = LCD_RESET_PIN;	
		HAL_GPIO_Init(LCD_RESET_GPIO, &gpio_init);	
	}
}

/*дָ� LCD ģ��*/
void Lcd_WriteIndex(uint8_t data1)
{
	char i;
	
	LCD_RS_0();	
	LCD_CS_0();

	ST7789_SendByteQuick(data1);
	
	LCD_CS_1();
}

/* �Ż����룬���ٲ��� */
static void ST7789_SendByteQuick(uint8_t data)
{
	uint8_t bit;

	if (data & 0x80)
	{
		LCD_SDA_1();
	}
	else
	{
		LCD_SDA_0();
	}
	LCD_SCK_0(); LCD_SCK_1();
	
	/* bit6 */
	bit = (data & 0xC0);
	if (bit == 0x80)
	{
		LCD_SDA_0();
	}
	else if (bit == 0x40)
	{
		LCD_SDA_1();
	}
	LCD_SCK_0();LCD_SCK_1();
	
	/* bit5 */
	data <<= 1;
	bit = (data & 0xC0);
	if (bit == 0x80)
	{
		LCD_SDA_0();
	}
	else if (bit == 0x40)
	{
		LCD_SDA_1();
	}
	LCD_SCK_0();LCD_SCK_1();	

	/* bit4 */
	data <<= 1;
	bit = (data & 0xC0);
	if (bit == 0x80)
	{
		LCD_SDA_0();
	}
	else if (bit == 0x40)
	{
		LCD_SDA_1();
	}
	LCD_SCK_0();LCD_SCK_1();
	
	/* bit3 */
	data <<= 1;
	bit = (data & 0xC0);
	if (bit == 0x80)
	{
		LCD_SDA_0();
	}
	else if (bit == 0x40)
	{
		LCD_SDA_1();
	}
	LCD_SCK_0();LCD_SCK_1();	
	
	/* bit2 */
	data <<= 1;
	bit = (data & 0xC0);
	if (bit == 0x80)
	{
		LCD_SDA_0();
	}
	else if (bit == 0x40)
	{
		LCD_SDA_1();
	}
	LCD_SCK_0();LCD_SCK_1();

	/* bit1 */
	data <<= 1;
	bit = (data & 0xC0);
	if (bit == 0x80)
	{
		LCD_SDA_0();
	}
	else if (bit == 0x40)
	{
		LCD_SDA_1();
	}
	LCD_SCK_0();LCD_SCK_1();	

	/* bit0 */
	data <<= 1;
	bit = (data & 0xC0);
	if (bit == 0x80)
	{
		LCD_SDA_0();
	}
	else if (bit == 0x40)
	{
		LCD_SDA_1();
	}
	LCD_SCK_0();LCD_SCK_1();		
}

/*д���ݵ� LCD ģ��*/
void Lcd_WriteData(uint8_t data1)
{
	char i;
	
	LCD_RS_1();		
	LCD_CS_0();
	ST7789_SendByteQuick(data1);
	LCD_CS_1();
}

// ��д2���ֽڣ��� 16 λ�����ݵ�LCDģ��
void Lcd_WriteData_16(uint16_t data2)
{	
	LCD_RS_1();	
	LCD_CS_0();

	ST7789_SendByteQuick(data2 >> 8);
	ST7789_SendByteQuick(data2);
	
	LCD_CS_1();
}

//LCD ��ʼ��
static void ST7789_initial(void)
{
	bsp_DelayUS(10 * 1000);		/* �ȴ���Դ�ȶ� */
	LCD_RESET_0();				/* �͵�ƽ����λ�� ֻ��Ҫ����10us */
	bsp_DelayUS(20);			/* �ӳ� 20us */
	LCD_RESET_1(); 				/* �ߵ�ƽ����λ���� */
	bsp_DelayUS(10 * 1000);		/* ��λ֮��Ҫ��ȴ�����5ms, �˴��ȴ�10ms */
	
	Lcd_WriteIndex(0x36); 		/* ɨ�跽�� */
	Lcd_WriteData(0x00);

	Lcd_WriteIndex(0x3A); 
	Lcd_WriteData(0x05);

	Lcd_WriteIndex(0xB2);
	Lcd_WriteData(0x0C);
	Lcd_WriteData(0x0C);
	Lcd_WriteData(0x00);
	Lcd_WriteData(0x33);
	Lcd_WriteData(0x33);

	Lcd_WriteIndex(0xB7); 
	Lcd_WriteData(0x35);  

	Lcd_WriteIndex(0xBB);
	Lcd_WriteData(0x19);

	Lcd_WriteIndex(0xC0);
	Lcd_WriteData(0x2C);

	Lcd_WriteIndex(0xC2);
	Lcd_WriteData(0x01);

	Lcd_WriteIndex(0xC3);
	Lcd_WriteData(0x12);   

	Lcd_WriteIndex(0xC4);
	Lcd_WriteData(0x20);  

	Lcd_WriteIndex(0xC6); 
	Lcd_WriteData(0x0F);    

	Lcd_WriteIndex(0xD0); 
	Lcd_WriteData(0xA4);
	Lcd_WriteData(0xA1);

	Lcd_WriteIndex(0xE0);
	Lcd_WriteData(0xD0);
	Lcd_WriteData(0x04);
	Lcd_WriteData(0x0D);
	Lcd_WriteData(0x11);
	Lcd_WriteData(0x13);
	Lcd_WriteData(0x2B);
	Lcd_WriteData(0x3F);
	Lcd_WriteData(0x54);
	Lcd_WriteData(0x4C);
	Lcd_WriteData(0x18);
	Lcd_WriteData(0x0D);
	Lcd_WriteData(0x0B);
	Lcd_WriteData(0x1F);
	Lcd_WriteData(0x23);

	Lcd_WriteIndex(0xE1);
	Lcd_WriteData(0xD0);
	Lcd_WriteData(0x04);
	Lcd_WriteData(0x0C);
	Lcd_WriteData(0x11);
	Lcd_WriteData(0x13);
	Lcd_WriteData(0x2C);
	Lcd_WriteData(0x3F);
	Lcd_WriteData(0x44);
	Lcd_WriteData(0x51);
	Lcd_WriteData(0x2F);
	Lcd_WriteData(0x1F);
	Lcd_WriteData(0x1F);
	Lcd_WriteData(0x20);
	Lcd_WriteData(0x23);

	Lcd_WriteIndex(0x21); 

	Lcd_WriteIndex(0x11); 


	Lcd_WriteIndex(0x29);
}

/*
*********************************************************************************************************
*	�� �� ��: ST7789_GetChipDescribe
*	����˵��: ��ȡLCD����оƬ���������ţ�������ʾ
*	��    ��: char *_str : �������ַ�������˻�����
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void ST7789_GetChipDescribe(char *_str)
{
	strcpy(_str, "ST7789");
}

/*
*********************************************************************************************************
*	�� �� ��: ST7789_SetDispWin
*	����˵��: ������ʾ���ڣ����봰����ʾģʽ��
*	��    ��:  
*		_usX : ˮƽ����
*		_usY : ��ֱ����
*		_usHeight: ���ڸ߶�
*		_usWidth : ���ڿ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void ST7789_SetDispWin(uint16_t _usX, uint16_t _usY, uint16_t _usHeight, uint16_t _usWidth)
{
	/* �Ż����룬���겻�仯���򲻸��� */
	static uint16_t s_x1 = 9999;
	static uint16_t s_x2 = 9999;
	static uint16_t s_y1 = 9999;
	static uint16_t s_y2 = 9999;
	uint16_t x1,x2,y1,y2;
		
	/* ���� X ��ʼ�������ĵ�ַ */
	if (g_tParam.DispDir == 3)
	{
		_usX += 319 - 239;
	}
	x1 = _usX;
	x2 = _usX + _usWidth - 1;
	if (s_x1 != x1 || s_x2 != x2)
	{
		Lcd_WriteIndex(0x2a); 
		Lcd_WriteData_16(x1);
		Lcd_WriteData_16(x2);			
		s_x1 = x1;
		s_x2 = x2;
	}
	
	/* ���� Y��ʼ�������ĵ�ַ */
	if (g_tParam.DispDir == 1)
	{
		/*
			The address ranges are X=0 to X=239 (Efh) and Y=0 to Y=319 (13Fh).	
		*/
		_usY += 319 - 239;
	}
	y1 = _usY;
	y2 = _usY + 0 + _usHeight - 1;	
	if (s_y1 != y1 || s_y2 != y2)
	{
		Lcd_WriteIndex(0x2b);
		Lcd_WriteData_16(y1);
		Lcd_WriteData_16(y2);
		s_y1 = y1;
		s_y2 = y2;		
	}
	
	Lcd_WriteIndex(0x2c); /* д���ݿ�ʼ */
}

/*
*********************************************************************************************************
*	�� �� ��: ST7789_QuitWinMode
*	����˵��: �˳�������ʾģʽ����Ϊȫ����ʾģʽ
*	��    ��: ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void ST7789_QuitWinMode(void)
{
	ST7789_SetDispWin(0, 0, g_LcdHeight, g_LcdWidth);
}

/*
*********************************************************************************************************
*	�� �� ��: ST7789_DispOn
*	����˵��: ����ʾ
*	��    ��: ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void ST7789_DispOn(void)
{
}

/*
*********************************************************************************************************
*	�� �� ��: ST7789_DispOff
*	����˵��: �ر���ʾ
*	��    ��: ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void ST7789_DispOff(void)
{
}

/*
*********************************************************************************************************
*	�� �� ��: ST7789_ClrScr
*	����˵��: �����������ɫֵ����
*	��    ��: _usColor : ����ɫ
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void ST7789_ClrScr(uint16_t _usColor)
{
	ST7789_FillRect(0, 0, g_LcdHeight, g_LcdWidth, _usColor);
}

/*
*********************************************************************************************************
*	�� �� ��: ST7789_PutPixel
*	����˵��: ��1������
*	��    ��:
*			_usX,_usY : ��������
*			_usColor  : ������ɫ ( RGB = 565 ��ʽ)
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void ST7789_PutPixel(uint16_t _usX, uint16_t _usY, uint16_t _usColor)
{
	ST7789_SetDispWin(_usX, _usY, 1, 1);
	Lcd_WriteData_16(_usColor);
}

/*
*********************************************************************************************************
*	�� �� ��: ST7789_GetPixel
*	����˵��: ��ȡ1������
*	��    ��:
*			_usX,_usY : ��������
*			_usColor  : ������ɫ
*	�� �� ֵ: RGB��ɫֵ
*********************************************************************************************************
*/
uint16_t ST7789_GetPixel(uint16_t _usX, uint16_t _usY)
{
	return CL_BLUE;
}


/*
*********************************************************************************************************
*	�� �� ��: ST7789_DrawLine
*	����˵��: ���� Bresenham �㷨����2��仭һ��ֱ�ߡ�
*	��    ��:
*			_usX1, _usY1 : ��ʼ������
*			_usX2, _usY2 : ��ֹ��Y����
*			_usColor     : ��ɫ
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void ST7789_DrawLine(uint16_t _usX1 , uint16_t _usY1 , uint16_t _usX2 , uint16_t _usY2 , uint16_t _usColor)
{
	int32_t dx , dy ;
	int32_t tx , ty ;
	int32_t inc1 , inc2 ;
	int32_t d , iTag ;
	int32_t x , y ;

	/* ���� Bresenham �㷨����2��仭һ��ֱ�� */

	ST7789_PutPixel(_usX1 , _usY1 , _usColor);

	/* ��������غϣ���������Ķ�����*/
	if ( _usX1 == _usX2 && _usY1 ==  _usY2 )
	{
		return;
	}

	iTag = 0 ;
	/* dx = abs ( _usX2 - _usX1 ); */
	if (_usX2 >= _usX1)
	{
		dx = _usX2 - _usX1;
	}
	else
	{
		dx = _usX1 - _usX2;
	}

	/* dy = abs ( _usY2 - _usY1 ); */
	if (_usY2 >=  _usY1)
	{
		dy = _usY2 - _usY1;
	}
	else
	{
		dy = _usY1 - _usY2;
	}

	if ( dx < dy )   /*���dyΪ�Ƴ������򽻻��ݺ����ꡣ*/
	{
		uint16_t temp;

		iTag = 1 ;
		temp = _usX1; _usX1 = _usY1; _usY1 = temp;
		temp = _usX2; _usX2 = _usY2; _usY2 = temp;
		temp = dx; dx = dy; dy = temp;
	}
	tx = _usX2 > _usX1 ? 1 : -1 ;    /* ȷ������1���Ǽ�1 */
	ty = _usY2 > _usY1 ? 1 : -1 ;
	x = _usX1 ;
	y = _usY1 ;
	inc1 = 2 * dy ;
	inc2 = 2 * ( dy - dx );
	d = inc1 - dx ;
	while ( x != _usX2 )     /* ѭ������ */
	{
		if (d < 0 )
		{
			d +=  inc1 ;
		}
		else
		{
			y +=  ty ;
			d +=  inc2 ;
		}
		if (iTag )
		{
			ST7789_PutPixel(y , x , _usColor) ;
		}
		else
		{
			ST7789_PutPixel(x , y , _usColor) ;
		}
		x +=  tx ;
	}	
}

/*
*********************************************************************************************************
*	�� �� ��: ST7789_DrawHLine
*	����˵��: ����һ��ˮƽ��. ʹ��STM32F429 DMA2DӲ������.
*	��    ��:
*			_usX1, _usY1 : ��ʼ������
*			_usLen       : �ߵĳ���
*			_usColor     : ��ɫ
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void ST7789_DrawHLine(uint16_t _usX, uint16_t _usY, uint16_t _usLen , uint16_t _usColor)
{
#if 0
	ST7789_FillRect(_usX, _usY, 1, _usLen, _usColor);
#else	
	uint16_t i;
	
	for (i = 0; i < _usLen; i++)
	{	
		ST7789_PutPixel(_usX + i , _usY , _usColor);
	}
#endif	
}

/*
*********************************************************************************************************
*	�� �� ��: ST7789_DrawVLine
*	����˵��: ����һ����ֱ�ߡ� ʹ��STM32F429 DMA2DӲ������.
*	��    ��:
*			_usX, _usY : ��ʼ������
*			_usLen       : �ߵĳ���
*			_usColor     : ��ɫ
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void ST7789_DrawVLine(uint16_t _usX , uint16_t _usY , uint16_t _usLen , uint16_t _usColor)
{
#if 0
	ST7789_FillRect(_usX, _usY, _usLen, 1, _usColor);
#else	
	uint16_t i;
	
	for (i = 0; i < _usLen; i++)
	{	
		ST7789_PutPixel(_usX, _usY + i, _usColor);
	}
#endif	
}
/*
*********************************************************************************************************
*	�� �� ��: ST7789_DrawPoints
*	����˵��: ���� Bresenham �㷨������һ��㣬������Щ�����������������ڲ�����ʾ��
*	��    ��:
*			x, y     : ��������
*			_usColor : ��ɫ
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void ST7789_DrawPoints(uint16_t *x, uint16_t *y, uint16_t _usSize, uint16_t _usColor)
{
	uint16_t i;

	for (i = 0 ; i < _usSize - 1; i++)
	{
		ST7789_DrawLine(x[i], y[i], x[i + 1], y[i + 1], _usColor);
	}
}

/*
*********************************************************************************************************
*	�� �� ��: ST7789_DrawRect
*	����˵��: ����ˮƽ���õľ��Ρ�
*	��    ��:
*			_usX,_usY: �������Ͻǵ�����
*			_usHeight : ���εĸ߶�
*			_usWidth  : ���εĿ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void ST7789_DrawRect(uint16_t _usX, uint16_t _usY, uint16_t _usHeight, uint16_t _usWidth, uint16_t _usColor)
{
	/*
	 ---------------->---
	|(_usX��_usY)        |
	V                    V  _usHeight
	|                    |
	 ---------------->---
		  _usWidth
	*/
	ST7789_DrawHLine(_usX, _usY, _usWidth, _usColor);
	ST7789_DrawVLine(_usX +_usWidth - 1, _usY, _usHeight, _usColor);
	ST7789_DrawHLine(_usX, _usY + _usHeight - 1, _usWidth, _usColor);
	ST7789_DrawVLine(_usX, _usY, _usHeight, _usColor);
}

/*
*********************************************************************************************************
*	�� �� ��: ST7789_FillRect
*	����˵��: ��һ����ɫֵ���һ�����Ρ�ʹ��STM32F429�ڲ�DMA2DӲ�����ơ�
*	��    ��:
*			_usX,_usY: �������Ͻǵ�����
*			_usHeight : ���εĸ߶�
*			_usWidth  : ���εĿ��
*			_usColor  : ��ɫ����
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void ST7789_FillRect(uint16_t _usX, uint16_t _usY, uint16_t _usHeight, uint16_t _usWidth, uint16_t _usColor)
{
	uint32_t i;
	
	ST7789_SetDispWin(_usX, _usY, _usHeight, _usWidth);
	
	LCD_RS_1();
	LCD_CS_0();
	for (i = 0; i < _usHeight * _usWidth; i++)
	{
		ST7789_SendByteQuick(_usColor >> 8);
		ST7789_SendByteQuick(_usColor);
	}
	LCD_CS_1();
}

/*
*********************************************************************************************************
*	�� �� ��: ST7789_DrawCircle
*	����˵��: ����һ��Բ���ʿ�Ϊ1������
*	��    ��:
*			_usX,_usY  : Բ�ĵ�����
*			_usRadius  : Բ�İ뾶
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void ST7789_DrawCircle(uint16_t _usX, uint16_t _usY, uint16_t _usRadius, uint16_t _usColor)
{
	int32_t  D;			/* Decision Variable */
	uint32_t  CurX;		/* ��ǰ X ֵ */
	uint32_t  CurY;		/* ��ǰ Y ֵ */

	D = 3 - (_usRadius << 1);
	CurX = 0;
	CurY = _usRadius;

	while (CurX <=  CurY)
	{
		ST7789_PutPixel(_usX + CurX, _usY + CurY, _usColor);
		ST7789_PutPixel(_usX + CurX, _usY - CurY, _usColor);
		ST7789_PutPixel(_usX - CurX, _usY + CurY, _usColor);
		ST7789_PutPixel(_usX - CurX, _usY - CurY, _usColor);
		ST7789_PutPixel(_usX + CurY, _usY + CurX, _usColor);
		ST7789_PutPixel(_usX + CurY, _usY - CurX, _usColor);
		ST7789_PutPixel(_usX - CurY, _usY + CurX, _usColor);
		ST7789_PutPixel(_usX - CurY, _usY - CurX, _usColor);

		if (D < 0)
		{
			D +=  (CurX << 2) + 6;
		}
		else
		{
			D +=  ((CurX - CurY) << 2) + 10;
			CurY--;
		}
		CurX++;
	}
}

/*
*********************************************************************************************************
*	�� �� ��: ST7789_DrawBMP
*	����˵��: ��LCD����ʾһ��BMPλͼ��λͼ����ɨ����򣺴����ң����ϵ���
*	��    ��:  
*			_usX, _usY : ͼƬ������
*			_usHeight  ��ͼƬ�߶�
*			_usWidth   ��ͼƬ���
*			_ptr       ��ͼƬ����ָ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void ST7789_DrawBMP(uint16_t _usX, uint16_t _usY, uint16_t _usHeight, uint16_t _usWidth, uint16_t *_ptr)
{
	uint16_t i, k, y;
	const uint16_t *p;

	p = _ptr;
	y = _usY;
	for (i = 0; i < _usHeight; i++)
	{
		for (k = 0; k < _usWidth; k++)
		{
			ST7789_PutPixel(_usX + k, y, *p++);
		}
		
		y++;
	}
}

/*
*********************************************************************************************************
*	�� �� ��: ST7789_SetDirection
*	����˵��: ������ʾ����ʾ���򣨺��� ������
*	��    ��: ��ʾ������� 0 ��������, 1 = ����180�ȷ�ת, 2 = ����, 3 = ����180�ȷ�ת
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void ST7789_SetDirection(uint8_t _dir)
{
	uint16_t temp;

	/*
		Bit D7-  MY , Page Address Order 
			��0�� = Top to Bottom (When MADCTL D7=��0��). 
			��1�� = Bottom to Top (When MADCTL D7=��1��). 
		Bit D6-  MX,  Column Address Order 
			��0�� = Left to Right (When MADCTL D6=��0��). 
			��1�� = Right to Left (When MADCTL D6=��1��). 
		Bit D5-  MV , Page/Column Order 
			��0�� = Normal Mode (When MADCTL D5=��0��). 
			��1�� = Reverse Mode (When MADCTL D5=��1��) 
		Note: Bits D7 to D5, alse refer to section 8.12 Address Control 
		
		Bit D4- Line Address Order 
			��0�� = LCD Refresh Top to Bottom (When MADCTL D4=��0��) 
			��1�� = LCD Refresh Bottom to Top (When MADCTL D4=��1��) 
		Bit D3- RGB/BGR Order 
			��0�� = RGB (When MADCTL D3=��0��) 
			��1�� = BGR (When MADCTL D3=��1��) 
		Bit D2- Display Data Latch Data Order 
			��0�� = LCD Refresh Left to Right (When MADCTL D2=��0��) 
			��1�� = LCD Refresh Right to Left (When MADCTL D2=��1��) 
	*/
	
	/*
		D5  D6  D7
		MV  MX  MY 
		 0  0  0  - ���� ����
		 0  1  1  - X���� Y����    ������ת180��
		 1  0  1  - X-Y������Y��������
		 1  1  0  - X-Y������X��������180��
	*/
	
	/*
		The address ranges are X=0 to X=239 (Efh) and Y=0 to Y=319 (13Fh).	
	*/
	
	if (_dir ==  0 || _dir ==  1)		/* ������ ����180�� */
	{
		if (g_LcdWidth < g_LcdHeight)
		{
			temp = g_LcdWidth;
			g_LcdWidth = g_LcdHeight;
			g_LcdHeight = temp;			
		}
		
		if (_dir == 0)
		{
			Lcd_WriteIndex(0x36); 		/* ɨ�跽�� */
			Lcd_WriteData((0 << 5) | (0 << 6) | (0 << 7));	
		}	
		else
		{
			Lcd_WriteIndex(0x36); 		/* ɨ�跽�� */
			Lcd_WriteData((0 << 5) | (1 << 6) | (1 << 7));				
		}
	}
	else if (_dir ==  2 || _dir ==  3)	/* ����, ����180��*/
	{
		if (g_LcdWidth > g_LcdHeight)
		{
			temp = g_LcdWidth;
			g_LcdWidth = g_LcdHeight;
			g_LcdHeight = temp;			
		}
		
		if (_dir == 3)
		{
			Lcd_WriteIndex(0x36); 		/* ɨ�跽�� */
			Lcd_WriteData((1 << 5) | (0 << 6) | (1 << 7));					
		}	
		else
		{
			Lcd_WriteIndex(0x36); 		/* ɨ�跽�� */
			Lcd_WriteData((1 << 5) | (1 << 6) | (0 << 7));			
		}		
	}
}


/***************************** ���������� www.armfly.com (END OF FILE) *********************************/
