/*
*********************************************************************************************************
*
*	ģ������ : ST7735 TFT SPI�ӿ���������
*	�ļ����� : bsp_tft_st7735.c
*	��    �� : V1.0
*	˵    �� : SPI�ӿڣ���ʾ����ICΪST7735���ֱ���Ϊ128*128
*	�޸ļ�¼ :
*		�汾��  ����       ����    ˵��
*		V1.0	2018-12-06 armfly 
*
*	Copyright (C), 2018-2030, ���������� www.armfly.com
*
*********************************************************************************************************
*/

#include "bsp.h"
#include "fonts.h"

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

static void ST7735_ConfigGPIO(void);
static void ST7735_initial(void);

/*
*********************************************************************************************************
*	�� �� ��: ST7735_InitHard
*	����˵��: ��ʼ��LCD
*	��    ��: ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void ST7735_InitHard(void)
{
	ST7735_ConfigGPIO();			/* ����429 CPU�ڲ�LTDC */
	
	ST7735_initial();
	
	g_LcdHeight = 128;			/* ��ʾ���ֱ���-�߶� */
	g_LcdWidth = 128;			/* ��ʾ���ֱ���-��� */
}

/*
*********************************************************************************************************
*	�� �� ��: ST7735_ConfigLTDC
*	����˵��: ����LTDC
*	��    ��: ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
static void ST7735_ConfigGPIO(void)
{
	/* ����GPIO */
	{
		GPIO_InitTypeDef gpio_init;

		/* ��GPIOʱ�� */
		ALL_LCD_GPIO_CLK_ENABLE();
		
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
void transfer_command(int data1)
{
	char i;
	
	LCD_CS_0();
	LCD_RS_0();
	for (i = 0; i < 8; i++)
	{
		LCD_SCK_0();
		if (data1 & 0x80)
			LCD_SDA_1();
		else 
			LCD_SDA_0();
		LCD_SCK_1();
		data1 = data1 <<= 1;
	}
}

/*д���ݵ� LCD ģ��*/
void transfer_data(int data1)
{
	char i;
	
	LCD_CS_0();
	LCD_RS_1();
	for (i = 0; i < 8; i++)
	{
		LCD_SCK_0();
		if (data1 & 0x80)
			LCD_SDA_1();
		else
			LCD_SDA_0();
		LCD_SCK_1();
		data1 = data1 <<= 1;
	}
}

// ��д2���ֽڣ��� 16 λ�����ݵ�LCDģ��
void transfer_data_16(uint16_t data2)
{
	transfer_data(data2 >> 8);
	transfer_data(data2);
}

//LCD ��ʼ��
static void ST7735_initial(void)
{
	bsp_DelayUS(50 * 1000);
	LCD_RESET_0();	/* �͵�ƽ����λ */
	bsp_DelayUS(1000);
	LCD_RESET_1(); /* �ߵ�ƽ����λ���� */
	bsp_DelayUS(10 * 1000);
	
	//��ʼ��ʼ����
	transfer_command(0x11);
	transfer_command(0xb1);
	transfer_data(0x01);
	transfer_data(0x2c);
	transfer_data(0x2d);
	transfer_command(0xb2);
	transfer_data(0x01);
	transfer_data(0x2c);
	transfer_data(0x2d);
	transfer_command(0xb3);
	transfer_data(0x01);
	transfer_data(0x2c);
	transfer_data(0x2d);
		
	transfer_data(0x01);
	transfer_data(0x2d);
	transfer_data(0x2d);
	transfer_command(0xb4);
	transfer_data(0x02);
	transfer_command(0xb6);
	transfer_data(0xb4);
	transfer_data(0xf0);
	transfer_command(0xc0);
	transfer_data(0xa2);
	transfer_data(0x02);
	transfer_data(0x84);
	transfer_command(0xc1);
	transfer_data(0xc5);
	transfer_command(0xc2);
	transfer_data(0x0a);
	transfer_data(0x00);
	transfer_command(0xc3);
	transfer_data(0x8a);
	transfer_data(0x2a);
	transfer_command(0xc4);
	transfer_data(0x8a);
	transfer_data(0xee);
	transfer_command(0xc5);
	transfer_data(0x0e);
	transfer_command(0x36); //��ɨ��˳����ɨ��˳�򣬺��/����
	transfer_data(0x08); 
	//MX = 1���е�ַ˳�򣺴����ң���MY = 1���е�ַ˳�򣺴��ϵ��£���MV = 0�����ţ���ML = 0(����ˢ�£�
	//���ϵ��£���RGB = 1������Ϊ RGB����MH = 0������ˢ��˳�򣺴����ң�
	//���壺"normal"���ǡ�0xc8��---��������;
	//���壺"CW180"���ǡ�0x08"---���������Ż�����ת 180 ������;
	//���壺"CCW90�����ǡ�0xa8"---�����Ż�������ʱ��ת 90 �Ⱥ��;
	//���壺��CW90�����ǡ�0x68"---�����Ż�����˳ת 90 �Ⱥ��;
	transfer_command(0xff);
	transfer_data(0x40);
	transfer_data(0x03);
	transfer_data(0x1a);

	transfer_command(0xfc);
	transfer_data(0x11);
	transfer_data(0x17);
	transfer_command(0xf0);
	transfer_data(0x01);
	transfer_command(0x3a);
	transfer_data(0x05);
	transfer_command(0xf6);
	transfer_data(0x00);
	transfer_command(0xe0);
	transfer_data(0x02);
	transfer_data(0x1c);
	transfer_data(0x07);
	transfer_data(0x12);
	transfer_data(0x37);
	transfer_data(0x32);
	transfer_data(0x29);
	transfer_data(0x2d);
	transfer_data(0x29);
	transfer_data(0x25);
	transfer_data(0x2b);
	transfer_data(0x39);
	transfer_data(0x00);
	transfer_data(0x01);
	transfer_data(0x03);
	transfer_data(0x10);
	transfer_command(0xe1);
	transfer_data(0x0b);
	transfer_data(0x14);
	transfer_data(0x09);
	transfer_data(0x26);
	transfer_data(0x27);
	transfer_data(0x22);
	transfer_data(0x1c);
	transfer_data(0x20);
	transfer_data(0x1d);
	transfer_data(0x1a);
	transfer_data(0x25);
	transfer_data(0x2d);
	transfer_data(0x06);
	transfer_data(0x06);

	transfer_data(0x02);
	transfer_data(0x0f);
	transfer_command(0x2a); //���� X ��ַ�Ŀ�ʼ������λ��
	transfer_data(0x00);
	transfer_data(0x00);
	transfer_data(0x00);
	transfer_data(0x7F);
	transfer_command(0x2b); //���� Y ��ַ�Ŀ�ʼ������λ��
	transfer_data(0x00);
	transfer_data(0x00);
	transfer_data(0x00);
	transfer_data(0x7F);
	transfer_command(0x29); //����ʾ
}

//���崰�����꣺��ʼ���꣨XS,YS)�Լ����ڴ�С��x_total,y_total)
//void lcd_address(int XS,int YS, int x_total, int y_total)
//{
//	int XE,YE;
//	
//	XS = XS+2;
//	YS = YS+1;
//	XE = XS + x_total - 1;
//	YE = YS + y_total - 1;
//	transfer_command(0x2a); // ���� X ��ʼ�������ĵ�ַ
//	transfer_data_16(XS); // X ��ʼ��ַ(16 λ��
//	transfer_data_16(XE); // X ������ַ(16 λ��
//	transfer_command(0x2b); // ���� Y ��ʼ�������ĵ�ַ
//	transfer_data_16(YS); // Y ��ʼ��ַ(16 λ��
//	transfer_data_16(YE); // Y ������ַ(16 λ
//	transfer_command(0x2c); // д���ݿ�ʼ
//}

////����ɫ�� 8 λ�����ݣ����� 8 �����ص㣩ת���ɲ�ɫ�����ݴ����Һ����
//void mono_transfer_data(int mono_data,int font_color,int back_color)
//{
//	int i;
//	
//	for(i = 0; i < 8; i++)
//	{
//		if(mono_data&0x80)
//		{
//			transfer_data_16(font_color); //�������� 1 ʱ����ʾ������ɫ
//		}
//		else	
//		{
//			transfer_data_16(back_color); //�������� 0 ʱ����ʾ��ɫ
//		}
//		mono_data <<= 1;
//	}
//}

////��ʾ��һɫ��
//void display_color(int XS,int YS,int x_total,int y_total,int color)
//{
//	int i,j;
//	
//	lcd_address(XS,YS,x_total,y_total);
//	for (i = 0; i < 128; i++)
//	{
//		for(j = 0;j<128;j++)
//		{
//			transfer_data_16(color);
//		}
//	}
//}

////��ʾһ��ȫ����ͼ
//void display_image(uint8_t *dp)
//{
//	uint8_t i,j;
//	
//	lcd_address(0,0,128,128);
//	for (i = 0; i < 128; i++)
//	{
//		for(j = 0;j<128;j++)
//		{
//			transfer_data(*dp); //��һ�����ص�ͼƬ���ݵĸ�λ
//			dp++;
//			transfer_data(*dp); //��һ�����ص�ͼƬ���ݵĵ�λ
//			dp++;
//		}
//	}
//}


// ��д2���ֽڣ��� 16 λ�����ݵ�LCDģ��
void ST7735_WriteData16(uint16_t data2)
{
	transfer_data(data2 >> 8);
	transfer_data(data2);
}

/*
*********************************************************************************************************
*	�� �� ��: ST7735_GetChipDescribe
*	����˵��: ��ȡLCD����оƬ���������ţ�������ʾ
*	��    ��: char *_str : �������ַ�������˻�����
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void ST7735_GetChipDescribe(char *_str)
{
	strcpy(_str, "ST7735");
}

/*
*********************************************************************************************************
*	�� �� ��: ST7735_SetDispWin
*	����˵��: ������ʾ���ڣ����봰����ʾģʽ��
*	��    ��:  
*		_usX : ˮƽ����
*		_usY : ��ֱ����
*		_usHeight: ���ڸ߶�
*		_usWidth : ���ڿ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void ST7735_SetDispWin(uint16_t _usX, uint16_t _usY, uint16_t _usHeight, uint16_t _usWidth)
{
	/* ���� X ��ʼ�������ĵ�ַ */
	transfer_command(0x2a); 	
	transfer_data_16(_usX + 2);
	transfer_data_16(_usX + 2 + _usWidth - 1);
	
	/* ���� Y��ʼ�������ĵ�ַ */
	transfer_command(0x2b);
	transfer_data_16(_usY + 1);
	transfer_data_16(_usY + 1 + _usHeight - 1);
	
	transfer_command(0x2c); /* д���ݿ�ʼ */
}

/*
*********************************************************************************************************
*	�� �� ��: ST7735_QuitWinMode
*	����˵��: �˳�������ʾģʽ����Ϊȫ����ʾģʽ
*	��    ��: ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void ST7735_QuitWinMode(void)
{
	ST7735_SetDispWin(0, 0, g_LcdHeight, g_LcdWidth);
}

/*
*********************************************************************************************************
*	�� �� ��: ST7735_DispOn
*	����˵��: ����ʾ
*	��    ��: ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void ST7735_DispOn(void)
{
}

/*
*********************************************************************************************************
*	�� �� ��: ST7735_DispOff
*	����˵��: �ر���ʾ
*	��    ��: ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void ST7735_DispOff(void)
{
}

/*
*********************************************************************************************************
*	�� �� ��: ST7735_ClrScr
*	����˵��: �����������ɫֵ����
*	��    ��: _usColor : ����ɫ
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void ST7735_ClrScr(uint16_t _usColor)
{
	ST7735_FillRect(0, 0, g_LcdHeight, g_LcdWidth, _usColor);
}

/*
*********************************************************************************************************
*	�� �� ��: ST7735_PutPixel
*	����˵��: ��1������
*	��    ��:
*			_usX,_usY : ��������
*			_usColor  : ������ɫ ( RGB = 565 ��ʽ)
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void ST7735_PutPixel(uint16_t _usX, uint16_t _usY, uint16_t _usColor)
{
	ST7735_SetDispWin(_usX, _usY, 1, 1);
	transfer_data_16(_usColor);
}

/*
*********************************************************************************************************
*	�� �� ��: ST7735_GetPixel
*	����˵��: ��ȡ1������
*	��    ��:
*			_usX,_usY : ��������
*			_usColor  : ������ɫ
*	�� �� ֵ: RGB��ɫֵ
*********************************************************************************************************
*/
uint16_t ST7735_GetPixel(uint16_t _usX, uint16_t _usY)
{
	return CL_BLUE;
}


/*
*********************************************************************************************************
*	�� �� ��: ST7735_DrawLine
*	����˵��: ���� Bresenham �㷨����2��仭һ��ֱ�ߡ�
*	��    ��:
*			_usX1, _usY1 : ��ʼ������
*			_usX2, _usY2 : ��ֹ��Y����
*			_usColor     : ��ɫ
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void ST7735_DrawLine(uint16_t _usX1 , uint16_t _usY1 , uint16_t _usX2 , uint16_t _usY2 , uint16_t _usColor)
{
	int32_t dx , dy ;
	int32_t tx , ty ;
	int32_t inc1 , inc2 ;
	int32_t d , iTag ;
	int32_t x , y ;

	/* ���� Bresenham �㷨����2��仭һ��ֱ�� */

	ST7735_PutPixel(_usX1 , _usY1 , _usColor);

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
			ST7735_PutPixel(y , x , _usColor) ;
		}
		else
		{
			ST7735_PutPixel(x , y , _usColor) ;
		}
		x +=  tx ;
	}	
}

/*
*********************************************************************************************************
*	�� �� ��: ST7735_DrawHLine
*	����˵��: ����һ��ˮƽ��. ʹ��STM32F429 DMA2DӲ������.
*	��    ��:
*			_usX1, _usY1 : ��ʼ������
*			_usLen       : �ߵĳ���
*			_usColor     : ��ɫ
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void ST7735_DrawHLine(uint16_t _usX, uint16_t _usY, uint16_t _usLen , uint16_t _usColor)
{
#if 0
	ST7735_FillRect(_usX, _usY, 1, _usLen, _usColor);
#else	
	uint16_t i;
	
	for (i = 0; i < _usLen; i++)
	{	
		ST7735_PutPixel(_usX + i , _usY , _usColor);
	}
#endif	
}

/*
*********************************************************************************************************
*	�� �� ��: ST7735_DrawVLine
*	����˵��: ����һ����ֱ�ߡ� ʹ��STM32F429 DMA2DӲ������.
*	��    ��:
*			_usX, _usY : ��ʼ������
*			_usLen       : �ߵĳ���
*			_usColor     : ��ɫ
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void ST7735_DrawVLine(uint16_t _usX , uint16_t _usY , uint16_t _usLen , uint16_t _usColor)
{
#if 0
	ST7735_FillRect(_usX, _usY, _usLen, 1, _usColor);
#else	
	uint16_t i;
	
	for (i = 0; i < _usLen; i++)
	{	
		ST7735_PutPixel(_usX, _usY + i, _usColor);
	}
#endif	
}
/*
*********************************************************************************************************
*	�� �� ��: ST7735_DrawPoints
*	����˵��: ���� Bresenham �㷨������һ��㣬������Щ�����������������ڲ�����ʾ��
*	��    ��:
*			x, y     : ��������
*			_usColor : ��ɫ
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void ST7735_DrawPoints(uint16_t *x, uint16_t *y, uint16_t _usSize, uint16_t _usColor)
{
	uint16_t i;

	for (i = 0 ; i < _usSize - 1; i++)
	{
		ST7735_DrawLine(x[i], y[i], x[i + 1], y[i + 1], _usColor);
	}
}

/*
*********************************************************************************************************
*	�� �� ��: ST7735_DrawRect
*	����˵��: ����ˮƽ���õľ��Ρ�
*	��    ��:
*			_usX,_usY: �������Ͻǵ�����
*			_usHeight : ���εĸ߶�
*			_usWidth  : ���εĿ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void ST7735_DrawRect(uint16_t _usX, uint16_t _usY, uint16_t _usHeight, uint16_t _usWidth, uint16_t _usColor)
{
	/*
	 ---------------->---
	|(_usX��_usY)        |
	V                    V  _usHeight
	|                    |
	 ---------------->---
		  _usWidth
	*/
	ST7735_DrawHLine(_usX, _usY, _usWidth, _usColor);
	ST7735_DrawVLine(_usX +_usWidth - 1, _usY, _usHeight, _usColor);
	ST7735_DrawHLine(_usX, _usY + _usHeight - 1, _usWidth, _usColor);
	ST7735_DrawVLine(_usX, _usY, _usHeight, _usColor);
}

/*
*********************************************************************************************************
*	�� �� ��: ST7735_FillRect
*	����˵��: ��һ����ɫֵ���һ�����Ρ�ʹ��STM32F429�ڲ�DMA2DӲ�����ơ�
*	��    ��:
*			_usX,_usY: �������Ͻǵ�����
*			_usHeight : ���εĸ߶�
*			_usWidth  : ���εĿ��
*			_usColor  : ��ɫ����
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void ST7735_FillRect(uint16_t _usX, uint16_t _usY, uint16_t _usHeight, uint16_t _usWidth, uint16_t _usColor)
{
	uint32_t i;
	
	ST7735_SetDispWin(_usX, _usY, _usHeight, _usWidth);
	
	for (i = 0; i < _usHeight * _usWidth; i++)
	{
		transfer_data_16(_usColor);
	}
	
}

/*
*********************************************************************************************************
*	�� �� ��: ST7735_DrawCircle
*	����˵��: ����һ��Բ���ʿ�Ϊ1������
*	��    ��:
*			_usX,_usY  : Բ�ĵ�����
*			_usRadius  : Բ�İ뾶
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void ST7735_DrawCircle(uint16_t _usX, uint16_t _usY, uint16_t _usRadius, uint16_t _usColor)
{
	int32_t  D;			/* Decision Variable */
	uint32_t  CurX;		/* ��ǰ X ֵ */
	uint32_t  CurY;		/* ��ǰ Y ֵ */

	D = 3 - (_usRadius << 1);
	CurX = 0;
	CurY = _usRadius;

	while (CurX <=  CurY)
	{
		ST7735_PutPixel(_usX + CurX, _usY + CurY, _usColor);
		ST7735_PutPixel(_usX + CurX, _usY - CurY, _usColor);
		ST7735_PutPixel(_usX - CurX, _usY + CurY, _usColor);
		ST7735_PutPixel(_usX - CurX, _usY - CurY, _usColor);
		ST7735_PutPixel(_usX + CurY, _usY + CurX, _usColor);
		ST7735_PutPixel(_usX + CurY, _usY - CurX, _usColor);
		ST7735_PutPixel(_usX - CurY, _usY + CurX, _usColor);
		ST7735_PutPixel(_usX - CurY, _usY - CurX, _usColor);

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
*	�� �� ��: ST7735_DrawBMP
*	����˵��: ��LCD����ʾһ��BMPλͼ��λͼ����ɨ����򣺴����ң����ϵ���
*	��    ��:  
*			_usX, _usY : ͼƬ������
*			_usHeight  ��ͼƬ�߶�
*			_usWidth   ��ͼƬ���
*			_ptr       ��ͼƬ����ָ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void ST7735_DrawBMP(uint16_t _usX, uint16_t _usY, uint16_t _usHeight, uint16_t _usWidth, uint16_t *_ptr)
{
	uint16_t i, k, y;
	const uint16_t *p;

	p = _ptr;
	y = _usY;
	for (i = 0; i < _usHeight; i++)
	{
		for (k = 0; k < _usWidth; k++)
		{
			ST7735_PutPixel(_usX + k, y, *p++);
		}
		
		y++;
	}
}

/*
*********************************************************************************************************
*	�� �� ��: ST7735_SetDirection
*	����˵��: ������ʾ����ʾ���򣨺��� ������
*	��    ��: ��ʾ������� 0 ��������, 1 = ����180�ȷ�ת, 2 = ����, 3 = ����180�ȷ�ת
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void ST7735_SetDirection(uint8_t _dir)
{
	uint16_t temp;
	
	if (_dir ==  0 || _dir ==  1)		/* ������ ����180�� */
	{
		if (g_LcdWidth < g_LcdHeight)
		{
			temp = g_LcdWidth;
			g_LcdWidth = g_LcdHeight;
			g_LcdHeight = temp;			
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
	}
}


/***************************** ���������� www.armfly.com (END OF FILE) *********************************/
