/*
*********************************************************************************************************
*	                                  
*	ģ������ : wifi�����շ��ӿں���
*	�ļ����� : wifi_if.h
*
*********************************************************************************************************
*/

#ifndef __WIFI_IF_H
#define __WIFI_IF_H

/* TCP UDP��������ID���� */
#define LINK_ID_UDP_SERVER		0		/* UDP���� */
//#define LINK_ID_TCP_SERVER		4		/* TCP�����������տ��� */
#define LINK_ID_TCP_CLIENT		4		/* TCP�ͻ��ˣ����ӵ�Զ�̷��������Զ��㱨1 */

typedef enum 
{	
	WIFI_STOP		=  0,
	WIFI_INIT		=  5,
	WIFI_LINK_AP    =  10,
	
	WIFI_LINK_SERVER  = 20,

	WIFI_WATCH_DOG	= 30,
	
	WIFI_SOFT_AP	=  40,
	
	WIFI_READY = 50,	
	
	WIFI_REG_PACKAGE =  60,
	
	
}WIFI_STATE_T;

void wifi_Start_SoftAP_Station(void);
uint8_t wifi_LinkHomeAP(void);
void wifi_SendBuf(uint8_t *str, uint16_t len);

void wifi_LinkToServer(void);
void wifi_DebugATCommand(uint8_t *_rxbuf, uint16_t _rxlen);
uint8_t wifi_LinkSoftAP(void);
void wifi_SendUdp(uint8_t *str, uint16_t len);

uint8_t wifi_task(void);

extern uint8_t wifi_state;

#endif


