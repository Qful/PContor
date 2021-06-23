/*
*********************************************************************************************************
*
*	ģ������ : UDP����ģ��
*	�ļ����� : net_udp.c
*	��    �� : V1.0
*	˵    �� : ʹ��UDP�㲥�������������ڵ��豸��������IP�Ȳ���
*
*	�޸ļ�¼ :
*		�汾��  ����        ����     ˵��
*		V1.0    2016-11-22  armfly  ��ʽ����
*
*	Copyright (C), 2016-2020, ���������� www.armfly.com
*
*********************************************************************************************************
*/

#include "bsp.h"
#include "udp.h"
#include "net_udp.h"
#include "param.h"
#include "local_reg_addr.h"
#include "tcp_modbus_slave.h"


#define	MODS_ADDR	0x01	/* MODBSU ��վ��ַ */

#define UDP_TX_SIZE	(256 + 8)

struct udp_pcb *g_udp_pcb;
struct pbuf *p_udp_tx; 

static uint8_t udp_tx_buf[UDP_TX_SIZE];
static uint16_t udp_tx_len;


static void udp_server_recv(void *arg, struct udp_pcb *pcb, struct pbuf *p_rx, struct ip_addr *addr, u16_t port);


/*
*********************************************************************************************************
*	�� �� ��: UDP_server_init
*	����˵��: ����UDP���������˿ںŹ̶� 17262
*	��    ��: ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/	
void UDP_server_init(void)
{
	g_udp_pcb = udp_new();	//����udp���ƿ�
	//udp_bind(pcb,IP_ADDR_ANY,UDP_LOCAL_PORT); 	

	p_udp_tx = pbuf_alloc(PBUF_RAW, sizeof(udp_tx_buf), PBUF_RAM);  // ����ָ�����ͷ���һ��pbuf�ṹ��  // struct pbuf *p_tx;
	p_udp_tx->payload = (void *)udp_tx_buf; 
	
	//g_udp_pcb->so_options |= SOF_BROADCAST;
	udp_bind(g_udp_pcb, IP_ADDR_ANY, 30010);	/* �󶨱���IP��ַ�Ͷ˿ںţ���Ϊudp�������� */
	udp_recv(g_udp_pcb, udp_server_recv, NULL); 	/* ����UDP�ε�ʱ�Ļص����� */
	

}

/*
*********************************************************************************************************
*	�� �� ��: udp_server_recv
*	����˵��: ���յ�UDP���ݰ��Ļص�����
*	��    ��: ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
static void udp_server_recv(void *arg, struct udp_pcb *pcb, struct pbuf *p_rx, struct ip_addr *addr, u16_t port)
{
	struct ip_addr destAddr = *addr; 	/* ��ȡԶ������ IP��ַ */
	
	//pcb->local_ip;
	IP4_ADDR(&destAddr, 255, 255, 255, 255);  	//��������ӿڵ�ip��ַ
	if (p_rx != NULL)
	{
		/* ����UDP���ݰ� */
		{
			const uint8_t mac_ff[6]= {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
			uint16_t i;
			
			g_tModbus.TCP_Flag = 0;
			if ( p_rx->len >= 8)
			{
				/* 6�ֽ�MAC��ַ�� 0xFFFFFF �ǹ㲥MAC. */	
				if (memcmp(p_rx->payload, mac_ff, 6) == 0 || memcmp(p_rx->payload, &g_tVar.MACaddr, 6) == 0)
				{
					/* ��MACǰ׺��udp���ݰ� */
					MODBUS_Poll((uint8_t *)p_rx->payload + 6, p_rx->len - 6);
					
					for (i = 0; i < g_tModbus.TxCount; i++)
					{
						g_tModbus.TxBuf[g_tModbus.TxCount - i - 1 + 6] = g_tModbus.TxBuf[g_tModbus.TxCount - i - 1];
					}
					
					//MODS_Analyze((uint8_t *)p_rx->payload + 6, p_rx->len - 6, &udp_tx_buf[6], &udp_tx_len);	/* ����MODBUS����֡ */
					if (g_tModbus.TxCount > 0)
					{
						memcpy(g_tModbus.TxBuf, &g_tVar.MACaddr, 6);	/* ����MAC�ŵ�Ӧ�����ݰ�ǰ׺ */
						g_tModbus.TxCount += 6;
					}
				}
				else	/* ����MACǰ׺ */
				{
					MODBUS_Poll(p_rx->payload, p_rx->len);		/* ����MODBUS����֡ */
				}
			}
		}		
		
		if (g_tModbus.TxCount > 0)
		{
			/* ׼��Ӧ������ */
			p_udp_tx->payload =  (void *)g_tModbus.TxBuf; 
			p_udp_tx->len = g_tModbus.TxCount;
			p_udp_tx->tot_len = g_tModbus.TxCount;
			
			udp_sendto(pcb, p_udp_tx, &destAddr, port); /* ���յ��������ٷ��ͳ�ȥ */	
		}
		
		pbuf_free(p_rx); 	/* �ͷŸ�UDP�� */
	}
}


/***************************** ���������� www.armfly.com (END OF FILE) *********************************/
