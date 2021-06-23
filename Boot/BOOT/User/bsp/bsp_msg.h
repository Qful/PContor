/*
*********************************************************************************************************
*
*	ģ������ : ��Ϣ����ģ��
*	�ļ����� : bsp_msg.h
*	��    �� : V1.0
*	˵    �� : ͷ�ļ�
*
*	Copyright (C), 2013-2014, ���������� www.armfly.com
*
*********************************************************************************************************
*/

#ifndef __BSP_MSG_H
#define __BSP_MSG_H

#define MSG_FIFO_SIZE    40	   		/* ��Ϣ���� */

enum 
{
	MSG_NONE = 0,
	
	MSG_CAN1_RX,
	MSG_CAN2_RX,
	
	MSG_RESET_CPU,			/* ��λCPU */
	MSG_RESET_TO_APP,		/* ��λCPU ����APP */
	
	MSG_SAVE_PARAM,			/* ���������Ϣ */
	
	MSG_FILE_IFNO,			/* �ļ���Ϣ */
	MSG_ERASE_SECTOR_ERR,	/* ����FLASH���� */
	MSG_ERASE_SECTOR_OK,	/* ����FLASH ok */
	MSG_TRANS_FILE,			/* �����ļ� */
	MSG_TRANS_ERROR,		/* �����ļ�ʧ�� */
};

/* ����FIFO�õ����� */
typedef struct
{
	uint16_t MsgCode;		/* ��Ϣ���� */
	uint32_t MsgParam;		/* ��Ϣ��������, Ҳ������ָ�루ǿ��ת���� */
}MSG_T;

/* ����FIFO�õ����� */
typedef struct
{
	MSG_T Buf[MSG_FIFO_SIZE];	/* ��Ϣ������ */
	uint8_t Read;					/* ��������ָ��1 */
	uint8_t Write;					/* ������дָ�� */
	uint8_t Read2;					/* ��������ָ��2 */
}MSG_FIFO_T;

/* ���ⲿ���õĺ������� */
void bsp_InitMsg(void);
void bsp_PutMsg(uint16_t _MsgCode, uint32_t _MsgParam);
uint8_t bsp_GetMsg(MSG_T *_pMsg);
uint8_t bsp_GetMsg2(MSG_T *_pMsg);
void bsp_ClearMsg(void);

#endif

/***************************** ���������� www.armfly.com (END OF FILE) *********************************/
