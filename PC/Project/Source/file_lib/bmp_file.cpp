/*
*********************************************************************************************************
*
*	模块名称 : BMP文件处理模块
*	文件名称 : bmp_file.c
*	版    本 : V1.0
*	说    明 : 主要实现将屏幕截图保存为BMP文件
*
*	修改记录 :
*		版本号  日期        作者     说明
*		V1.0    2013-03-01 armfly  正式发布
*
*	Copyright (C), 2013-2014, 安富莱电子 www.armfly.com
*
*********************************************************************************************************
*/

#include "bmp_file.h"

/*
	BMP文件的组成结构 :

	位图文件头（bitmap-file header）
	位图信息头（bitmap-information header）
	彩色表/调色板（color table）
	位图数据（bitmap-data）

	如果图像是单色、16色和256色，则紧跟着调色板的是位图数据，位图数据是指向调色板的索引序号。
	如果位图是16位、24位和32位色，则图像文件中不保留调色板，即不存在调色板，图像的颜色直接在位图数据中给出。
	16位图像使用2字节保存颜色值，常见有两种格式：
		5位红 5位绿 5位蓝 即555格式。555格式只使用了15 位，最后一位保留，设为0。
		5位红 6位绿 5位蓝 即565格式。
	24位图像使用3字节保存颜色值，每一个字节代表一种颜色，按红、绿、蓝排列。
	32位图像使用4字节保存颜色值，每一个字节代表一种颜色，除了原来的红、绿、蓝，还有Alpha通道，即透明色。

*/

/* BMP文件头结构 必须添加__packed关键字表示结构体成员间紧密排列 */
typedef struct
{
	/* 14字节 位图文件头 */
	uint16_t bfType;		/* 2字节 位图类别， 在Windows中，此字段的值总为'BM'*/
	uint32_t bfSize;		/* bmp文件大小 4字节 */
	uint16_t bfReserved1;	/* 保留，每字节以"00"填写 2字节 */
	uint16_t bfReserved2; 	/* 同上 2字节 */
	uint32_t bfOffBits;		/* 记录图像数据区的起始位置(图象数据相对于文件头字节的偏移量)。 4字节 */

	/* 40字节 位图信息头 */
	uint32_t biSize;		/* 4字节 本结构的大小，在Windows中，总为28h，40字节 */
	int32_t  biWidth;		/* 4字节 BMP图像的宽度，单位像素 */
	int32_t  biHeight;		/* 4字节 BMP图像的高度，单位像素 */
	uint16_t biPlanes;		/* 2字节 目标设备的级别(色彩平面数)，固定为1 */
	uint16_t biBitCount;	/* 2字节 BMP图像的色深，即一个像素用多少位表示。常见的有 1 4 8 16 24 32 */
	uint32_t biCompression;	/* 4字节 压缩类型，0(不压缩), 1(BI_RLE8), 2(BI_RLE4) */
	uint32_t biSizeImage;	/* 4字节 表示位图数据区域的大小以字节为单位 */
	int32_t  biXPelsPerMeter;	/* 4字节 用象素/米表示的水平分辨率 */
	int32_t  biYPelsPerMeter;	/* 4字节 用象素/米表示的垂直分辨率 */
	uint32_t biClrUsed;			/* 4字节 实际使用色彩数目，则由位数定 */
	uint32_t biClrImportant;	/* 4字节 图像中重要的色彩数目。0表示调色板内所有的颜色都是重要的  */
}BMP_T;

/*
*********************************************************************************************************
*	函 数 名: MakeBmpHeader
*	功能说明: 生成BMP文件头结构。 固定按24位色， RGB = 888 结构。存储低字节在前，B前，G中，R后
*	形    参: _pBmp : BMP文件头结构体指针
*			 _Height : 图像高度
*			 _Width :  图像宽度
*	返 回 值: 无
*********************************************************************************************************
*/
//static void MakeBmpHeader(__packed BMP_T *_pBmp, uint16_t _Width, uint16_t _Height)
void MakeBmpHeader(uint8_t *_pBmp, uint16_t _Width, uint16_t _Height)
{
	/* 14字节 位图文件头 */
	*_pBmp++ = 'B';  /* 2字节 位图类别， 在Windows中，此字段的值总为'BM'*/
	*_pBmp++ = 'M';
	Uint32ToLEBuf((_Height * _Width * 3) + 54, _pBmp); _pBmp += 4;  /* bmp文件大小 4字节 */
	*_pBmp++ = 0; /* 保留，每字节以"00"填写 2字节 */
	*_pBmp++ = 0;
	*_pBmp++ = 0; /* 保留，每字节以"00"填写 2字节 */
	*_pBmp++ = 0;
	/* 记录图像数据区的起始位置(图象数据相对于文件头字节的偏移量)。 4字节 */
	Uint32ToLEBuf(54, _pBmp); _pBmp += 4;

	/* 40字节 位图信息头 */
	Uint32ToLEBuf(40, _pBmp); _pBmp += 4;			/* 4字节 本结构的大小，在Windows中，总为28h，40字节 */
	Uint32ToLEBuf(_Width, _pBmp); _pBmp += 4;		/* 4字节 BMP图像的宽度，单位像素 */
	Uint32ToLEBuf(_Height, _pBmp); _pBmp += 4;	/* 4字节 BMP图像的高度，单位像素 */
	Uint16ToLEBuf(1, _pBmp); _pBmp += 2;			/* 2字节 目标设备的级别(色彩平面数)，固定为1 */
	Uint16ToLEBuf(24, _pBmp); _pBmp += 2;			/* 2字节 BMP图像的色深，即一个像素用多少位表示。常见的有 1 4 8 16 24 32 */
	Uint32ToLEBuf(0, _pBmp); _pBmp += 4;			/* 4字节 压缩类型，0(不压缩), 1(BI_RLE8), 2(BI_RLE4) */
	Uint32ToLEBuf((_Height * _Width * 3), _pBmp); _pBmp += 4;	/* 4字节 表示位图数据区域的大小以字节为单位 */
	Uint32ToLEBuf(0, _pBmp); _pBmp += 4;	/* 4字节 用象素/米表示的水平分辨率 */
	Uint32ToLEBuf(0, _pBmp); _pBmp += 4;	/* 4字节 用象素/米表示的垂直分辨率 */
	Uint32ToLEBuf(0, _pBmp); _pBmp += 4;	/* 4字节 实际使用色彩数目，0则由位数定 */
	Uint32ToLEBuf(0, _pBmp); _pBmp += 4;	/* 4字节 图像中重要的色彩数目。0表示调色板内所有的颜色都是重要的  */
}

/*
*********************************************************************************************************
*	函 数 名: SaveScreenToBmp
*	功能说明: 将当前屏幕保存为BMP文件头。结合FatFS文件系统使用。
*	形    参: _index : 文件序号
*	返 回 值: 无
*********************************************************************************************************
*/
uint16_t H7GetPixel(uint16_t x, uint16_t y, uint8_t *_buf)
{
	uint32_t idx;

	/* 读取1行数据到缓冲区 (注意：BMP文件扫描次序，从左到右，从下到上。和LCD逻辑坐标是垂直翻转的) */
	idx =  2 * ((239 - y) * 240 + x);
	return (_buf[idx] + 256 * _buf[idx + 1]);
}

uint32_t SaveScreenToBmp(uint32_t _Height, uint32_t _Width, uint8_t *_pRGB565, uint8_t *_pBMP)
{
	uint8_t tBmpHead[54];
	uint8_t LineData[800 * 3]; 	/* 按最大 800像素宽度分配缓冲区 */
	uint16_t i,j;
	uint16_t pixel;
	uint16_t *pU16;

	MakeBmpHeader(tBmpHead, _Width, _Height);	/* 构建BMP文件头 */

	/* 写bmp文件头 */
	memcpy(_pBMP, tBmpHead, 54);
	_pBMP += 54;

	/* 开始读取屏幕数据，并写入文件 */
	for (i = 0; i <  _Height; i++)
	{
		for (j = 0; j < _Width; j++)
		{
			//pixel = LCD_GetPixel(j, tBmpHead.biHeight - i - 1);		/* 读取屏幕1个像素 （RGB = 565结构）*/
			pixel =  H7GetPixel(j, i, _pRGB565);

			LineData[3 * j + 2] = ((pixel & 0xF800) >> 8);		/* R */
			LineData[3 * j + 1] = ((pixel & 0x07E0) >> 3);		/* G */
			LineData[3 * j + 0] = ((pixel & 0x001F) << 3);		/* B */
		}

		memcpy(_pBMP, (char *)LineData, _Width * 3);
		_pBMP  += _Width * 3;
	}

	return 54 + _Height * _Width * 3;
}

/***************************** 安富莱电子 www.armfly.com (END OF FILE) *********************************/
