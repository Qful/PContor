/*
*********************************************************************************************************
*
*	ģ������ : UDP MODBUS��������
*	�ļ����� : bsp_udp_modbus.c
*	��    �� : V1.0
*	˵    �� : MODBUS��ַ����ֶ���6�ֽ�MAC��ַ���  ֧�� 03H, 06H, 10Hָ� �����������б仯�����Ĵ�����ַ��
*				����
*
*	Copyright (C), 2014-2015, ���������� www.armfly.com
*
*********************************************************************************************************
*/

#include "bsp.h"
#include "bsp_udp_modbus.h"
#include "UnitMain.h"
#include "UnitThread.h"
#include "UnitSet.h"

AnsiString g_udp_ip = "255.255.255.255";
int g_udp_port = 30010;

uint8_t MAC_FF[6] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
//uint16_t g_RegAddr;
//uint16_t g_RegNum;
//uint8_t g_RxOk;

MODBUS_CMD_T g_tCmd;
uint8_t s_TxBuf[TX_BUF_SIZE];
uint16_t s_TxLen;

/*
*********************************************************************************************************
*	�� �� ��: H7_WriteRegisters
*	����˵��: д�Ĵ���������06ָ��д�����Ĵ��������ȴ�Ӧ��
*	��    ��: _reg_addr���Ĵ�����ַ
			  _reg_value���Ĵ���ֵ
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void H7_WriteRegister(uint16_t _reg_addr, uint16_t _reg_value)
{
	g_tCmd.UserRegAddr = _reg_addr;
	g_tCmd.UserRegValue = _reg_value;
	g_tCmd.UserCmdStatus = CMDS_SEND_06H;
	g_tCmd.UserReq = 1;
	g_tCmd.CmdStatus = 0;
}

/*
*********************************************************************************************************
*	�� �� ��: H7_WriteMutiRegister
*	����˵��: д����Ĵ���
*	��    ��:  _reg_addr���Ĵ�����ַ
			 _reg_num���Ĵ�������
			 _buf : �Ĵ���ֵ���飬ÿ���Ĵ���2�ֽ�
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void H7_WriteMutiRegisters(uint16_t _reg_addr, uint16_t _reg_num, uint16_t *_buf)
{
	uint16_t i;

	g_tCmd.UserRegAddr = _reg_addr;

	for (i = 0; i < _reg_num; i++)
	{
		g_tCmd.UserRegValueBuf[i] = _buf[i];
	}
	g_tCmd.UserRegNum = _reg_num;
	g_tCmd.UserCmdStatus = CMDS_SEND_10H;
	g_tCmd.UserReq = 1;
}

/*
*********************************************************************************************************
*	�� �� ��: H7_ReadRegisters
*	����˵��: ���Ĵ���������03ָ��д�����Ĵ��������ȴ�Ӧ��
*	��    ��: _reg_addr���Ĵ�����ַ
			  _reg_num���Ĵ�������
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void H7_ReadRegisters(uint16_t _reg_addr, uint16_t _reg_num)
{
	g_tCmd.UserRegAddr = _reg_addr;
	g_tCmd.UserRegNum = _reg_num;
	g_tCmd.UserCmdStatus = CMDS_SEND_03H;
	g_tCmd.UserReq = 1;
}

/*
*********************************************************************************************************
*	�� �� ��: H7_WriteRelay
*	����˵��: д�������Ĵ���������D0-D7)���
*	��    ��:  _reg_addr���Ĵ�����ַ
			   _reg_num���Ĵ�������. �����ֽڸ���
			   _buf : �Ĵ���ֵ���飬 ÿ���ֽڱ�ʾ8��IO��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void H7_WriteRelay(uint16_t _reg_addr, uint16_t _reg_num, uint8_t *_buf)
{
	uint16_t i;
	uint8_t bytes;

	bytes = (_reg_num + 7) / 8;
	g_tCmd.UserRegAddr = _reg_addr;

	for (i = 0; i < bytes; i++)
	{
		g_tCmd.UserRegValueBuf[i] = _buf[i];
	}
	g_tCmd.UserRegNum = _reg_num;
	g_tCmd.UserCmdStatus = CMDS_SEND_0FH;
	g_tCmd.UserReq = 1;
}


/*
*********************************************************************************************************
*	�� �� ��: comSendBuf
*	����˵��: ���ʹ�������
*	��    ��: _port : ���ں�
*			  _buf : �����͵����ݻ�����
*			  _len : ���ݳ���
*	�� �� ֵ: len ��Ӧ�����ݳ���
*			  0 ��ָ��¼��ʧ��
*********************************************************************************************************
*/
void Series_SendBuf(uint8_t *_buf, uint16_t _len)
{
	AnsiString s;
	uint16_t  i;

	//TFT485_ClearRxFifo();
	try
	{
		if (Form1->YbCommDevice1->Active == true)
		{
			Form1->YbCommDevice1->Write((const void *)_buf, _len);
		}
	}
	catch (Exception &e)
	{
	}
}

// ��ָ�����ڶ�ȡ1���ֽ�
uint8_t comGetChar(uint8_t *_pData)
{
	uint8_t buf[10]; //�յ����ֽ������ᳬ�����ڻ��������, ���Է���һ������������ͬ��Buf
	int n;

	try
	{
		if (Form1->YbCommDevice1->Active == false)
			return 0;

		n = Form1->YbCommDevice1->Read(buf, 1); //�� 1 ���ֽ�
		if (n == 1)
		{
			*_pData = buf[0];
			return 1;
		}
		else
		{
			return 0;
		}
	}
	catch (Exception &e)
	{
		return 0;
	}
}


/*
*********************************************************************************************************
*	�� �� ��: udp_SendBuf
*	����˵��: �������ݡ� ͨ���߳��������
*	��    ��: _buf : ������;    _len : ���ݳ���
*	�� �� ֵ: ��
*********************************************************************************************************
*/
static void udp_SendBuf(uint8_t *_buf, uint16_t _len)
{
	if (FormSet->EditScanPort == NULL)
	{
		return;
	}

	// ���͵�UDP�����
	if (g_tIniParam.CommInterface == 1)     /* USB���⴮�� */
	{
		Series_SendBuf(_buf,  _len);
	}
	else       /* ���ں�WIFI , TCP UDPЭ�� */
	{
		TByteDynArray buf;

		g_udp_port = StrToInt(FormSet->EditScanPort->Text);

		buf = RawToBytes(_buf, _len);
		FormTcp->UDP1->SendBuffer(FormSet->EditH7IPAddr->Text, g_udp_port, buf);

		//Form1->Memo_AddLine(_buf,  _len);
	}
}

/*
*********************************************************************************************************
*	�� �� ��: udp_SendBuf_mac
*	����˵��: ��������
*	��    ��: _buf : ������;    _len : ���ݳ���
*	�� �� ֵ: ��
*********************************************************************************************************
*/
static void udp_SendBuf_mac(uint8_t *_buf, uint16_t _len)
{
	// ���͵�UDP�����
	{
		TByteDynArray buf;

		g_udp_port = StrToInt(FormSet->EditScanPort->Text);

		buf = RawToBytes(_buf, _len);
		//Form1->UDP1->SendBuffer("255.255.255.255", g_udp_port, buf);
		//Form1->UDP1->SendBuffer("192.168.1.211", g_udp_port, buf);
		Form1->Memo_AddLine(_buf,  _len);

		FormTcp->UDP1->Broadcast(buf,  g_udp_port,   "255.255.255.255");
	}
}

/*
*********************************************************************************************************
*	�� �� ��: MODBUS_Send06H
*	����˵��: ����06ָ��д�����Ĵ��������ȴ�Ӧ��
*	��    ��: _addr : �ӻ���485��ַ
			  _reg_addr���Ĵ�����ַ
			 _reg_value���Ĵ�������
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void udp_Send06H(uint16_t _reg_addr, uint16_t _reg_value)
{
	uint8_t txbuf[32];
	uint8_t pos = 0;
	uint16_t crc;

	g_tCmd.RegAddr = _reg_addr;
	g_tCmd.RegValue =  _reg_value;
	g_tCmd.RxOk = 0;

	//memcpy(txbuf,  _mac, 6);
	pos = 0;
	txbuf[pos++] = 1;
	txbuf[pos++] = 0x06;			/* ������ 10*/
	txbuf[pos++] = _reg_addr >> 8;			/* ��ַ�Ĵ��� ��00 70 */
	txbuf[pos++] = _reg_addr;
	txbuf[pos++] = _reg_value >> 8;
	txbuf[pos++] = _reg_value;
	crc = CRC16_Modbus(txbuf, pos);
	txbuf[pos++] = crc >> 8;
	txbuf[pos++] = crc;
	udp_SendBuf(txbuf,  pos);
}

/*
*********************************************************************************************************
*	�� �� ��: MODBUS_Send06H
*	����˵��: ����06ָ��д�����Ĵ��������ȴ�Ӧ��
*	��    ��: _addr : �ӻ���485��ַ
			  _reg_addr���Ĵ�����ַ
			 _reg_value���Ĵ�������
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void udp_Send06H_MAC(uint8_t *_mac, uint16_t _reg_addr, uint16_t _reg_value)
{
	uint8_t txbuf[32];
	uint8_t pos = 0;
	uint16_t crc;

	g_tCmd.RegAddr = _reg_addr;
	g_tCmd.RegValue =  _reg_value;
	g_tCmd.RxOk = 0;

	memcpy(txbuf,  _mac, 6);
	pos = 6;
	txbuf[pos++] = 1;
	txbuf[pos++] = 0x06;			/* ������ 10*/
	txbuf[pos++] = _reg_addr >> 8;			/* ��ַ�Ĵ��� ��00 70 */
	txbuf[pos++] = _reg_addr;
	txbuf[pos++] = _reg_value >> 8;
	txbuf[pos++] = _reg_value;
	crc = CRC16_Modbus(&txbuf[6], pos - 6);
	txbuf[pos++] = crc >> 8;
	txbuf[pos++] = crc;
	udp_SendBuf_mac(txbuf,  pos);
}

/*
*********************************************************************************************************
*	�� �� ��: MODBUS_Send10H
*	����˵��: ����03ָ����������룩�����ȴ�Ӧ��
*	��    ��: _addr : �ӻ���485��ַ
			  _reg_addr���Ĵ�����ַ
			 _reg_num���Ĵ�������
			 _buf : �Ĵ���ֵ���飬ÿ���Ĵ���2�ֽ�
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void udp_Send10H(uint16_t _reg_addr, uint16_t _reg_num, uint16_t *_buf)
{
	uint8_t txbuf[2048];
	uint16_t pos = 0;
	int i;
	uint16_t crc;

	g_tCmd.RegAddr = _reg_addr;
	g_tCmd.RegNum =  _reg_num;
	for (i = 0; i < _reg_num; i++)
	{
		g_tCmd.RegValueBuf[i] = _buf[i];
	}
	g_tCmd.RxOk = 0;

	pos = 0;
	txbuf[pos++] = 1;
	txbuf[pos++] = 0x10;			/* ������ 10*/
	txbuf[pos++] = _reg_addr >> 8;			/* ��ַ�Ĵ��� ��00 70 */
	txbuf[pos++] = _reg_addr;
	txbuf[pos++] = _reg_num >> 8;
	txbuf[pos++] = _reg_num;
	txbuf[pos++] = _reg_num * 2;    /* �ֽ��� */
	for (i = 0; i < _reg_num; i++)
	{
		txbuf[pos++] = _buf[i] >> 8;
		txbuf[pos++] = _buf[i];
	}
	crc = CRC16_Modbus(txbuf, pos);
	txbuf[pos++] = crc >> 8;
	txbuf[pos++] = crc;
	udp_SendBuf(txbuf,  pos);
}

/*
*********************************************************************************************************
*	�� �� ��: udp_Send10H_MAC
*	����˵��: ����10ָ��,
*	��    ��: _addr : �ӻ���485��ַ
			  _reg_addr���Ĵ�����ַ
			 _reg_num���Ĵ�������
			 _buf : �Ĵ���ֵ���飬ÿ���Ĵ���2�ֽ�
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void udp_Send10H_MAC(uint8_t *_mac, uint16_t _reg_addr, uint16_t _reg_num, uint16_t *_buf)
{
	uint8_t txbuf[256];
	uint8_t pos = 0;
	int i;
	uint16_t crc;

	g_tCmd.RegAddr = _reg_addr;
	g_tCmd.RegNum =  _reg_num;
	for (i = 0; i < _reg_num; i++)
	{
		g_tCmd.RegValueBuf[i] = _buf[i];
	}
	g_tCmd.RxOk = 0;

	memcpy(txbuf,  _mac, 6);
	pos = 6;
	txbuf[pos++] = 1;
	txbuf[pos++] = 0x10;			/* ������ 10*/
	txbuf[pos++] = _reg_addr >> 8;			/* ��ַ�Ĵ��� ��00 70 */
	txbuf[pos++] = _reg_addr;
	txbuf[pos++] = _reg_num >> 8;
	txbuf[pos++] = _reg_num;
	txbuf[pos++] = _reg_num * 2;    /* �ֽ��� */
	for (i = 0; i < _reg_num; i++)
	{
		txbuf[pos++] = _buf[i] >> 8;
		txbuf[pos++] = _buf[i];
	}
	crc = CRC16_Modbus(&txbuf[6], pos - 6);
	txbuf[pos++] = crc >> 8;
	txbuf[pos++] = crc;
	udp_SendBuf_mac(txbuf,  pos);
}


/*
*********************************************************************************************************
*	�� �� ��: udp_Send0FH
*	����˵��: ����0Fָ����Ƽ̵�������������ȴ�Ӧ��
*	��    ��:
			  _reg_addr���Ĵ�����ַ
			 _reg_num���Ĵ�������
			 _buf : �Ĵ���ֵ���飬ÿ���Ĵ���2�ֽ�
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void udp_Send0FH(uint16_t _reg_addr, uint16_t _reg_num, uint16_t *_buf)
{
	uint8_t txbuf[2048];
	uint16_t pos = 0;
	int i;
	uint16_t crc;
	uint16_t bytes;

	bytes = (_reg_num + 7) / 8;

	g_tCmd.RegAddr = _reg_addr;
	g_tCmd.RegNum =  _reg_num;
	for (i = 0; i < bytes; i++)
	{
		g_tCmd.RegValueBuf[i] = _buf[i];
	}
	g_tCmd.RxOk = 0;

	pos = 0;
	txbuf[pos++] = 1;
	txbuf[pos++] = 0x0F;			/* ������ */
	txbuf[pos++] = _reg_addr >> 8;			/* ��ַ�Ĵ��� ��00 70 */
	txbuf[pos++] = _reg_addr;
	txbuf[pos++] = _reg_num >> 8;
	txbuf[pos++] = _reg_num;
	txbuf[pos++] = bytes;    /* �ֽ��� */
	for (i = 0; i < bytes; i++)
	{
		txbuf[pos++] = _buf[i];
	}
	crc = CRC16_Modbus(txbuf, pos);
	txbuf[pos++] = crc >> 8;
	txbuf[pos++] = crc;
	udp_SendBuf(txbuf,  pos);
}

/*
*********************************************************************************************************
*	�� �� ��: udp_Send03H
*	����˵��: ����03ָ���ȡ�Ĵ�������
*	��    ��: _mac : �ӻ���mac��ַ
*			  _reg_addr���Ĵ�����ַ
*			 _reg_num���Ĵ�������
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void udp_Send03H(uint16_t _reg_addr, uint16_t _reg_num)
{
	uint8_t txbuf[32];
	uint8_t pos = 0;
	uint16_t crc;

	g_tCmd.RegAddr = _reg_addr;
	g_tCmd.RegNum =  _reg_num;
	g_tCmd.RxOk = 0;

	pos = 0;
	txbuf[pos++] = 1;
	txbuf[pos++] = 0x03;			/* ������ 10*/
	txbuf[pos++] = _reg_addr >> 8;	/* ��ַ�Ĵ��� ��00 70 */
	txbuf[pos++] = _reg_addr;
	txbuf[pos++] = _reg_num >> 8;
	txbuf[pos++] = _reg_num;
	crc = CRC16_Modbus(txbuf, pos);
	txbuf[pos++] = crc >> 8;
	txbuf[pos++] = crc;

	udp_SendBuf(txbuf,  pos);
}

/*
*********************************************************************************************************
*	�� �� ��: udp_Send03H_MAC
*	����˵��: ����03ָ���ȡ�Ĵ�������
*	��    ��: _mac : �ӻ���mac��ַ
*			  _reg_addr���Ĵ�����ַ
*			 _reg_num���Ĵ�������
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void udp_Send03H_MAC(uint8_t * _mac, uint16_t _reg_addr, uint16_t _reg_num)
{
	uint8_t txbuf[32];
	uint8_t pos = 0;
	uint16_t crc;

	g_tCmd.RegAddr = _reg_addr;
	g_tCmd.RegNum =  _reg_num;
	g_tCmd.RxOk = 0;

	memcpy(txbuf,  _mac, 6);
	pos = 6;
	txbuf[pos++] = 1;
	txbuf[pos++] = 0x03;			/* ������ 10*/
	txbuf[pos++] = _reg_addr >> 8;	/* ��ַ�Ĵ��� ��00 70 */
	txbuf[pos++] = _reg_addr;
	txbuf[pos++] = _reg_num >> 8;
	txbuf[pos++] = _reg_num;
	crc = CRC16_Modbus(&txbuf[6], pos - 6);
	txbuf[pos++] = crc >> 8;
	txbuf[pos++] = crc;

	udp_SendBuf_mac(txbuf,  pos);
}

/*
*********************************************************************************************************
*	�� �� ��: udp_Send64H
*	����˵��: ����64hָ�����Lua����
*	��    ��: _
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void udp_Send64H(uint16_t _Func, uint32_t _TotalLen, uint32_t _Offset, uint32_t _Len, uint8_t *_DataBuf)
{
/*
	0100 0000 ; С�����ܳ��� 4�ֽ�
	0000 0000 : С����ƫ�Ƶ�ַ 4�ֽ�
	0020 0000 : �������򳤶� 4�ֽ� ������ż��
*/
	uint8_t pos = 0;
	uint16_t crc;
	uint32_t i;

	if (_Len > TX_BUF_SIZE - 16)
	{
		return;
	}

	g_tCmd.RxOk = 0;

	s_TxLen = 0;
	s_TxBuf[s_TxLen++] = 1;
	s_TxBuf[s_TxLen++] = 0x64;			/* ������ */
	s_TxBuf[s_TxLen++] = _Func >> 8;
	s_TxBuf[s_TxLen++] = _Func >> 0;

	s_TxBuf[s_TxLen++] = _TotalLen >> 24;
	s_TxBuf[s_TxLen++] = _TotalLen >> 16;
	s_TxBuf[s_TxLen++] = _TotalLen >> 8;
	s_TxBuf[s_TxLen++] = _TotalLen >> 0;

	s_TxBuf[s_TxLen++] = _Offset >> 24;
	s_TxBuf[s_TxLen++] = _Offset >> 16;
	s_TxBuf[s_TxLen++] = _Offset >> 8;
	s_TxBuf[s_TxLen++] = _Offset >> 0;

	s_TxBuf[s_TxLen++] = _Len >> 24;
	s_TxBuf[s_TxLen++] = _Len >> 16;
	s_TxBuf[s_TxLen++] = _Len >> 8;
	s_TxBuf[s_TxLen++] = _Len >> 0;

	for (i = 0; i < _Len; i++)
	{
		s_TxBuf[s_TxLen++] = _DataBuf[i];
	}

	crc = CRC16_Modbus(s_TxBuf, s_TxLen);
	s_TxBuf[s_TxLen++] = crc >> 8;
	s_TxBuf[s_TxLen++] = crc;

	udp_SendBuf(s_TxBuf,  s_TxLen);

	// DEBUG
	#if 0
		//;_Func, uint32_t _TotalLen, uint32_t _Offset, uint32_t _Len, uint8_t *_DataBuf
		{
			AnsiString s;

			s.sprintf("_Func = %X, _TotalLen = %d, _Offset = %d(%X), _Len = %d",
				_Func, _TotalLen, _Offset, _Offset, _Len);
			Form1->MemoLuaOut->Lines->Add(s);
		}
	#endif
}

/*
*********************************************************************************************************
*	�� �� ��: udp_Send65H
*	����˵��: ����65hָ�������ʱLua����
*	��    ��:
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void udp_Send65H(char *_LuaStr, uint32_t _LuaLen)
{
/*
	01
	65
	00 18
	xxxxxxx
	CRC
*/
	uint8_t pos = 0;
	uint16_t crc;
	uint32_t i;

	if (_LuaLen > TX_BUF_SIZE - 16)
	{
		return;
	}

	g_tCmd.RxOk = 0;

	s_TxLen = 0;
	s_TxBuf[s_TxLen++] = 1;
	s_TxBuf[s_TxLen++] = 0x65;			/* ������ */
	s_TxBuf[s_TxLen++] = _LuaLen >> 8;
	s_TxBuf[s_TxLen++] = _LuaLen >> 16;

	for (i = 0; i < _LuaLen; i++)
	{
		s_TxBuf[s_TxLen++] =  _LuaStr[i];
	}
	s_TxBuf[s_TxLen++] = 0;

	crc = CRC16_Modbus(s_TxBuf, s_TxLen);
	s_TxBuf[s_TxLen++] = crc >> 8;
	s_TxBuf[s_TxLen++] = crc;

	udp_SendBuf(s_TxBuf,  s_TxLen);
}


/*
*********************************************************************************************************
*	�� �� ��: udp_Send66H
*	����˵��: ����66hָ�SWD
*	��    ��:
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void udp_Send66H(uint16_t _func, uint32_t _addr, uint32_t _len, uint8_t *_buf)
{
    /*
        ��������: С��������
            01  ; վ��
			66  ; ������
            0001  ; �ӹ���,
				- 0��ʾ���ڴ� ��ʼ
				- 1��ʾ���ڴ� ����
				- 2��ʾд�ڴ棬δ��
            0000 0000 : ƫ�Ƶ�ַ 4�ֽ�
            0020 0000 : ���ݳ��� 4�ֽ�
            ... ����
            CCCC      : CRC16

        �ӻ�Ӧ��:
            01  ; �ӻ���ַ
            66  ; ������
            0000  ; �ӹ���

			00  ; ִ�н����0��ʾOK  1��ʾ����
            CCCC : CRC16
	*/
	uint8_t pos = 0;
	uint16_t crc;
	uint32_t i;

	if (_len > TX_BUF_SIZE - 16)
	{
		return;
	}

	g_tCmd.RxOk = 0;

	s_TxLen = 0;
	s_TxBuf[s_TxLen++] = 1;
	s_TxBuf[s_TxLen++] = 0x66;			/* ������ */
	s_TxBuf[s_TxLen++] = _func >> 8;
	s_TxBuf[s_TxLen++] = _func >> 16;

	s_TxBuf[s_TxLen++] = _addr >> 24;
	s_TxBuf[s_TxLen++] = _addr >> 16;
	s_TxBuf[s_TxLen++] = _addr >> 8;
	s_TxBuf[s_TxLen++] = _addr >> 0;

	s_TxBuf[s_TxLen++] = _len >> 24;
	s_TxBuf[s_TxLen++] = _len >> 16;
	s_TxBuf[s_TxLen++] = _len >> 8;
	s_TxBuf[s_TxLen++] = _len >> 0;

	if (_func == 2)
	{
		for (i = 0; i < _len; i++)
		{
			s_TxBuf[s_TxLen++] =  _buf[i];
		}

	}

	crc = CRC16_Modbus(s_TxBuf, s_TxLen);
	s_TxBuf[s_TxLen++] = crc >> 8;
	s_TxBuf[s_TxLen++] = crc;

	udp_SendBuf(s_TxBuf,  s_TxLen);
}


/*
*********************************************************************************************************
*	�� �� ��: udp_Send15H
*	����˵��: ����15hָ����ع̼�
*	��    ��:
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void udp_Send15H(uint16_t _data_len, uint16_t _record_id, uint16_t _record_Len,
	uint16_t _valid_Len, uint8_t *buf)
{
	uint16_t i;
	uint16_t crc;

	g_tCmd.RxOk = 0;

	s_TxLen = 0;
	s_TxBuf[s_TxLen++] = 1;			/* 485��ַ */
	s_TxBuf[s_TxLen++] = 0x15;			/* ������ 10*/
	s_TxBuf[s_TxLen++] = _data_len;  /* ���ݳ���*/
	s_TxBuf[s_TxLen++] = 6;               /* �������� */
	s_TxBuf[s_TxLen++] =  1 >> 8;//_file_Hao >> 8;			/* �ļ���*/
	s_TxBuf[s_TxLen++] =  1; //_file_Hao;
	s_TxBuf[s_TxLen++] = _record_id >> 8;     /*  ��¼��*/
	s_TxBuf[s_TxLen++] = _record_id;
	s_TxBuf[s_TxLen++] = _valid_Len >> 8;     /*  ��¼���� */
	s_TxBuf[s_TxLen++] = _valid_Len;
	for (i = 0; i < _valid_Len; i++)          /* �������� */
	{
		s_TxBuf[s_TxLen++] = buf[i];
	}
	s_TxLen = _record_Len + 10;			/* ����ÿ�����ݵȳ� */
	crc = CRC16_Modbus(s_TxBuf, s_TxLen);
	s_TxBuf[s_TxLen++] = crc >> 8;
	s_TxBuf[s_TxLen++] = crc;

	udp_SendBuf(s_TxBuf,  s_TxLen);
}

/*
*********************************************************************************************************
*	�� �� ��: udp_Send60H
*	����˵��: ����60hָ�������, ���ò���
*	��    ��:
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void udp_Send60H(uint32_t _ChEn, uint32_t _SampleSize, uint16_t _PackageLen,  uint32_t _Offset)
{
	/*
		PC���� 60H
			01  ; �ӻ���ַ
			60  ; ������
			00  : 00��ʾPC�·���01��ʾ�豸Ӧ�� �����������˹�������
			01 00 00 00  : ͨ����ʹ�ܱ�־ 32bit��bit0��ʾCH1��bit1��ʾCH2
			00 00 04 00: ÿ��ͨ����������
			01 00 : ÿͨ�Ű���������. ��λΪ1��������
			00 00 00 00 : ͨ������ƫ�� ��������λ�������ط���
			CC CC : CRC16

		�ӻ�����Ӧ��: 60H
			01  ; �ӻ���ַ
			60  ; ������
			01  : 00��ʾPC�·���01��ʾ�豸Ӧ�� �����������˹�������
			01 00 00 00  : ͨ����ʹ�ܱ�־ 32bit��bit0��ʾCH1��bit1��ʾCH2
			00 00 04 00 : ÿ��ͨ����������
			01 00 : ÿͨ�Ű���������. ��λΪ1��������
			00 00 00 00 : ͨ������ƫ�� ��������λ�������ط���
			CCCC : CRC16

		�ӻ�Ӧ��: ��Ȼ��ʼ�������Ӧ��)
			01  ; �ӻ���ַ
			61  ; ������
			00  ; ͨ���ţ�00��ʾͨ��1,01��ʾͨ��2,
			00 00 00 00 : ƫ�Ƶ�ַ��������λ��
			01 00 : �������ݳ��ȡ�������λ��ÿ������4�ֽڡ�0x100��ʾ1024�ֽڡ�
			..... : ������
			CCCC : CRC16
	*/
	uint16_t i;
	uint16_t crc;

	g_tCmd.RxOk = 0;

	s_TxLen = 0;
	s_TxBuf[s_TxLen++] = 1;			/* 485��ַ */
	s_TxBuf[s_TxLen++] = 0x60;			/* ������ */
	s_TxBuf[s_TxLen++] = 0;  		   /* 00��ʾPC�·���01��ʾ�豸Ӧ�� �����������˹������� */
	s_TxBuf[s_TxLen++] = _ChEn >> 24;  /* ͨ��ʹ�ܱ�־ */
	s_TxBuf[s_TxLen++] = _ChEn >> 15;
	s_TxBuf[s_TxLen++] = _ChEn >> 8;
	s_TxBuf[s_TxLen++] = _ChEn >> 0;

	s_TxBuf[s_TxLen++] = _SampleSize >> 24;  /* ÿͨ�Ű���������. ��λΪ1�������� */
	s_TxBuf[s_TxLen++] = _SampleSize >> 15;
	s_TxBuf[s_TxLen++] = _SampleSize >> 8;
	s_TxBuf[s_TxLen++] = _SampleSize >> 0;

	s_TxBuf[s_TxLen++] = _PackageLen >> 8;    /* ÿ��ͨ���������� */
	s_TxBuf[s_TxLen++] = _PackageLen >> 0;

	s_TxBuf[s_TxLen++] = _Offset >> 24;  /* ͨ������ƫ�� */
	s_TxBuf[s_TxLen++] = _Offset >> 15;
	s_TxBuf[s_TxLen++] = _Offset >> 8;
	s_TxBuf[s_TxLen++] = _Offset >> 0;

	crc = CRC16_Modbus(s_TxBuf, s_TxLen);
	s_TxBuf[s_TxLen++] = crc >> 8;
	s_TxBuf[s_TxLen++] = crc;

	udp_SendBuf(s_TxBuf,  s_TxLen);
}

/***************************** ���������� www.armfly.com (END OF FILE) *********************************/
