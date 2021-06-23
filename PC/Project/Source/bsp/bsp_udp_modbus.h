/*
*********************************************************************************************************
*
*	ģ������ : UDP MODBUS
*	�ļ����� : bsp_udp_modbus.h
*
*	Copyright (C), 2016-2020, ���������� www.armfly.com
*
*********************************************************************************************************
*/

#ifndef __BSP_UDP_MODBUS_H
#define __BSP_UDP_MODBUS_H

#define TX_BUF_SIZE		2 * 1024

#define REG_NUM_MAX		1 * 1024

/* ���ں�ͨ���߳̽��� */
typedef struct
{
	uint32_t CmdStatus;
	uint16_t RegAddr;	/* �Ĵ�����ַ */
	uint16_t RegValue;	/* �Ĵ���ֵ */
	uint16_t RegValueBuf[REG_NUM_MAX];
	uint16_t RegNum;	/* �Ĵ������� */

	uint8_t RxOk;

	uint8_t UserReq;		/* ������������. ɨ���϶ִ�� */
	uint32_t UserCmdStatus;
	uint16_t UserRegAddr;	/* �Ĵ�����ַ */
	uint16_t UserRegValue;	/* �Ĵ���ֵ */
	uint16_t UserRegNum;	/* �Ĵ������� */
	uint16_t UserRegValueBuf[REG_NUM_MAX];

	uint8_t err;
	uint8_t DisabelAck;		  /* ����ҪӦ�� */

}MODBUS_CMD_T;

/* 64H֡�ӹ����붨�� */
enum
{
    H64_LUA_RUN_WITH_RESET = 0, /* ����lua��λlua������ִ��, */
    H64_LUA_RUN_NO_RESET   = 1, /* ����lua�󣬲���λlua����ֱ��ִ�� */

    H64_LIST_FILE          = 2, /* listĿ¼�µ��ļ� */
    H64_GET_FILE_MD5       = 3, /* ��ȡ�ļ�MD5�� */
    H64_DEL_FILE           = 4, /* ɾ���ļ� */
    H64_DEL_DIR            = 5, /* ɾ��Ŀ¼���ļ� */
    H64_READ_FILE          = 6, /* ���ļ����� */
    H64_WRITE_FILE         = 7, /* �����ļ���д���ļ��� */
    H64_CREATE_FOLDER      = 8, /* �����ļ��� */
};

void udp_Send06H_MAC(uint8_t *_mac, uint16_t _reg_addr, uint16_t _reg_value);
void udp_Send10H_MAC(uint8_t *_mac, uint16_t _reg_addr, uint16_t _reg_num, uint16_t *_buf);
void udp_Send03H_MAC(uint8_t *_mac, uint16_t _reg_addr, uint16_t _reg_num);

void udp_Send03H(uint16_t _reg_addr, uint16_t _reg_num);
void udp_Send06H(uint16_t _reg_addr, uint16_t _reg_value);
void udp_Send10H(uint16_t _reg_addr, uint16_t _reg_num, uint16_t *_buf);
void udp_Send0FH(uint16_t _reg_addr, uint16_t _reg_num, uint16_t *_buf);

void udp_Send64H(uint16_t _Func, uint32_t _TotalLen, uint32_t _Offset, uint32_t _Len, uint8_t *_DataBuf);
void udp_Send65H(char *_LuaStr, uint32_t _LuaLen);
void udp_Send15H(uint16_t _data_len, uint16_t _record_id, uint16_t _record_Len,
	uint16_t _valid_Len, uint8_t *buf);

void udp_Send60H(uint32_t _ChEn, uint32_t _SampleSize, uint16_t _PackageLen,  uint32_t _Offset);
void udp_Send66H(uint16_t _func, uint32_t _addr, uint32_t _len, uint8_t *_buf);

void H7_WriteRegister(uint16_t _reg_addr, uint16_t _reg_value);
void H7_WriteMutiRegisters(uint16_t _reg_addr, uint16_t _reg_num, uint16_t *_buf);
void H7_ReadRegisters(uint16_t _reg_addr, uint16_t _reg_num);
void H7_WriteRelay(uint16_t _reg_addr, uint16_t _reg_num, uint8_t *_buf);

extern uint8_t MAC_FF[6];

extern AnsiString g_udp_ip;

extern MODBUS_CMD_T g_tCmd;


#endif
