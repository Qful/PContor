/*
*********************************************************************************************************
*
*	ģ������ : HEX��ʽתΪbin��ʽ
*	�ļ����� : hex2bin.c
*	��    �� : V1.0
*	˵    �� : ����������ԭ��
*   �޸ļ�¼ :
* 			V1.0  2020-06-07  �װ�
*
*********************************************************************************************************
*/

#include "bsp.h"

/* hex����
:02 0000 04 0802 F0
:10 0000 00 A8FF0724AD0202087DE00308F1160208 EC
:10 0010 00 79E0030833260208E5E1040821010000 25
:10 26D0 00 01FF01FF01FF01FF01FF01FF01FF01FF FA
:04 26E0 00 01180000DD
:04 0000 05 08020299 52
:00 0000 01 FF

 - ��1���ֽ� 0x02��ʾ�������ݵĳ��ȣ�
 - ��2��3�ֽ� 0x00 0x00��ʾ��ʼ��ַ��
 - ��4�ֽ� 0x04��ʾ��������:
	'00' Data Rrecord��������¼���ݣ�HEX�ļ��Ĵ󲿷ּ�¼�������ݼ�¼
	'01' End of File Record: ������ʶ�ļ������������ļ�����󣬱�ʶHEX�ļ��Ľ�β
	'02' Extended Segment Address Record: ������ʶ��չ�ε�ַ�ļ�¼
	'03' Start Segment Address Record:��ʼ�ε�ַ��¼
	'04' Extended Linear Address Record: ������ʶ��չ���Ե�ַ�ļ�¼
	'05' Start Linear Address Record:��ʼ���Ե�ַ��¼

 - ����0802
 - F0ΪУ��͡������ = 0x100 - �ۼӺ�
*/

static unsigned char AsciiToBin(char _Asc);
static short TwoAsciiToBin(char _Asc1, char _Asc2);
static int HexSumOk(char *_HexBuf, unsigned int _Bytes);
static void HexBufToBinBuf(char *_hex, unsigned char *_out, unsigned short _binlen);
static int AddHexLine(unsigned char *_hex, unsigned short _len, unsigned char *_bin, unsigned int _BinBufSize);

int s_status = 0;
uint32_t s_pos = 0;
uint32_t g_StartAddress = 0;
static uint32_t s_StartAddress = 0;
static uint32_t s_BaseAddress = 0;

/*
*********************************************************************************************************
*	�� �� ��: MyHexToBin
*	����˵��: ��Hex��ʽ������_Hexת��Ϊbin��ʽ������_Bin,
*	��    ��: __BinMaxLen : _BinBuf size
*	�� �� ֵ: 10 ����, > 1��ʾbin����
*********************************************************************************************************
*/
int MyHexToBin(char *_HexBuf, int _HexBufSize, char *_BinBuf, int _BinBufSize,
	unsigned char _InitValue, unsigned int *_StartAddress)
{
	char *p1, *p2;
	unsigned char LineBuf[1024];
	unsigned short LineLen;
	unsigned char ch;
	int result = 0;

	p1 = _HexBuf;
	p2 = p1;

	*_StartAddress = 0;

	memset(_BinBuf, _InitValue, _BinBufSize);	/* ���ȱʡֵ��0xFF��0x00 */
	s_status = 0;
	s_pos = 0;
	s_StartAddress = 0;
	s_BaseAddress = 0;
	while (_HexBufSize)
	{
		ch = *p2;
		if (ch == 0x0D || ch == 0x0A || ch == 0x00)
		{
			if ( p2 > p1)
			{
				if (HexSumOk(p1, p2 - p1))
				{
					if (p2 - p1 > sizeof(LineBuf) * 2)
					{
						return 0;	/* �г���̫�� */
					}
					HexBufToBinBuf(p1 + 1, LineBuf, p2 - p1 - 1);
					LineLen = (p2 - p1) / 2 - 1;

					result = AddHexLine(LineBuf, LineLen, _BinBuf, _BinBufSize);
					if (result == 0)   	/* ���� */
					{
						;
					}
					else if (result == 1)  /* �������� */
					{
						break;
					}
					else if (result == 2) /* ��ʽ�쳣 */
					{
						return 0;
					}
				}
				else
				{
					return 0;
				}
			}
			
			if (ch == 0)
			{
				break;
			}

			p1 = p2 + 1;
		}
		p2++;
		_HexBufSize--;
	}
	*_StartAddress = g_StartAddress;
	return s_pos;
}

/*
	0=����  1=��������  2=��ʽ�쳣
*/
static int AddHexLine(unsigned char *_hex, unsigned short _len, unsigned char *_bin,
	unsigned int _BinBufSize)
{
//	:02 0000 04 0802 F0
//	:10 0000 00 A8FF0724AD0202087DE00308F1160208 EC
	uint8_t len;
	uint16_t offset;
	uint8_t DataType;

	uint8_t ret = 0;
	static uint32_t s_LastPos = 0;

	int i;

	len = _hex[0];
	offset = _hex[1] * 256 + _hex[2];
	DataType = _hex[3];
//	'00' Data Rrecord��������¼���ݣ�HEX�ļ��Ĵ󲿷ּ�¼�������ݼ�¼
//	'01' End of File Record: ������ʶ�ļ������������ļ�����󣬱�ʶHEX�ļ��Ľ�β
//	'02' Extended Segment Address Record: ������ʶ��չ�ε�ַ�ļ�¼
//	'03' Start Segment Address Record:��ʼ�ε�ַ��¼
//	'04' Extended Linear Address Record: ������ʶ��չ���Ե�ַ�ļ�¼
//	'05' Start Linear Address Record:��ʼ���Ե�ַ��¼
	switch (DataType)
	{
		case 0x00:
			if (s_status == 0)
			{
				s_status = 1;

				s_StartAddress = s_BaseAddress + offset;

				g_StartAddress = s_StartAddress;
			}

			s_pos = s_BaseAddress + offset - s_StartAddress;
			for (i = 0; i < len; i++)
			{
				_bin[s_pos++] = _hex[4 + i];
			}
			break;

		case 0x01:
			ret = 1;    	/* hex�ļ��������� */
			break;

		case 0x02:
			break;

		case 0x03:
			break;

		case 0x04:
			s_BaseAddress = (_hex[4] << 24) + (_hex[5] << 16);
			break;

		case 0x05:			/* */
			break;

		default:
            ret = 2;
			break;
	}
	return ret;
}

static void HexBufToBinBuf(char *_hex, unsigned char *_out, unsigned short _hexlen)
{
	int i;

	for (i = 0; i < _hexlen / 2; i++)
	{
		_out[i] = TwoAsciiToBin(_hex[2 * i], _hex[2 * i + 1]);
	}
}

/*
*********************************************************************************************************
*	�� �� ��: AsciiToBin
*	����˵��: 1�ֽ�ASCII�ַ���ת��Ϊ1�ֽڶ����� 0-F
*	��    ��: ��
*	�� �� ֵ: 0x00-0x0F�� 0xFF��ʾ�쳣
*********************************************************************************************************
*/
static unsigned char AsciiToBin(char _Asc)
{
	if (_Asc >= '0' && _Asc <= '9')
	{
		return _Asc - '0';
	}
	else if (_Asc >= 'a' && _Asc <= 'f')
	{
		return _Asc - 'a' + 0x0A;
	}
	else if (_Asc >= 'A' && _Asc <= 'F')
	{
		return _Asc - 'A' + 0x0A;
	}
	else
	{
		return 0xFF;	/* �쳣ֵ */
	}
}

/*
*********************************************************************************************************
*	�� �� ��: AsciiToBin
*	����˵��: 2�ֽ�ASCII�ַ���ת��Ϊ������
*	��    ��: ��
*	�� �� ֵ: 0 OK������ֵ��ʾ����
*********************************************************************************************************
*/
static short TwoAsciiToBin(char _Asc1, char _Asc2)
{
	unsigned char c1,c2;

	c1 = AsciiToBin(_Asc1);
	c2 = AsciiToBin(_Asc2);
	if (c1 == 0xFF || c2 == 0xFF)
	{
		return -1;
	}

	return ((c1 << 4) | c2);
}

/*
*********************************************************************************************************
*	�� �� ��: BEBufToFloat
*	����˵��: ��Hex��ʽ������_Hexת��Ϊbin��ʽ������_Bin,
*	��    ��: __BinMaxLen : _BinBuf size
*	�� �� ֵ: 1 OK��0 ����
*********************************************************************************************************
*/
static int HexSumOk(char *_HexBuf, unsigned int _Bytes)
{
	unsigned char sum = 0;
	int i;
	unsigned char c1, c2;
	unsigned short cc;
	unsigned char endch;

	if (_Bytes == 0)
	{
		return 1;	/* ���з���OK */
	}

	//:00 0000 01 FF
	if (_HexBuf[0] != ':' || _Bytes < 11 || ((_Bytes % 2) == 0))
	{
	   return 0;	/* ��ʽ���� */
	}

	for (i = 0; i < _Bytes / 2 - 1; i++)
	{
		cc = TwoAsciiToBin(_HexBuf[2 * i + 1], _HexBuf[2 * i + 2]);
		if (cc < 0)
		{
			return 0;
		}
		sum += cc;
	}
	endch = TwoAsciiToBin(_HexBuf[2 * i + 1], _HexBuf[2 * i + 2]);

	sum = 0x100 - sum;
	if (endch == sum)
	{
		return 1;	/* У��ͨ�� */
	}
	return 0;
}

/***************************** ���������� www.armfly.com (END OF FILE) *********************************/
