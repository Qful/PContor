/*
*********************************************************************************************************
*
*	ģ������ : web server ��ʾ
*	�ļ����� : http_server.c
*	��    �� : V1.0
*	˵    �� : �ṩWEB���������ܡ���ҪĿ���ǲ���DM9000AEP������·��DM9161 PHY��·��
*	�޸ļ�¼ :
*		�汾��  ����        ����     ˵��
*		V1.0    2013-01-01 armfly  ��ʽ����
*
*	Copyright (C), 2012-2013, ���������� www.armfly.com
*
*********************************************************************************************************
*/

#include "stdio.h"
#include "bsp.h"
#include "num_pad.h"
#include "param.h"

/* for LwIP */
//#include "netconf.h"
//#include "main_lwip.h"
//#include "httpd_w.h"
#include "lwip/opt.h"
#include "lwip/init.h"
#include "netif/etharp.h"
#include "lwip/netif.h"
#include "lwip/timeouts.h"
#include "lwip/dhcp.h"

#include "ethernetif.h"
#include "app_ethernet.h"
#include "http_cgi_ssi.h"

void lwip_start(void);
void lwip_pro(void);

/*
*********************************************************************************************************
*	�� �� ��: WebServer
*	����˵��: web����������
*	��    ��: ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void WebServer(void)
{
	uint8_t ucKeyCode;		/* �������� */
	uint8_t ucTouch;		/* �����¼� */
	int16_t tpX, tpY;
	uint16_t x, y;
	uint16_t usLineCap = 18;
	char buf[128];
	uint8_t fRefresh;
	FONT_T tFont;		/* ��������ṹ����� */

	/* ����������� */
	{
		tFont.FontCode = FC_ST_16;	/* ������� 16���� */
		tFont.FrontColor = CL_BLACK;	/* ������ɫ */
		tFont.BackColor = FORM_BACK_COLOR;	/* ���ֱ�����ɫ */
		tFont.Space = 0;				/* ���ּ�࣬��λ = ���� */

		LCD_ClrScr(FORM_BACK_COLOR);  	/* ������������ɫ */

		LCD_DispStr(5, 3, "�߼�������ʾ����", &tFont);
	}

	/* LwIP ��ʼ�� */
	{
		/* ����������ߣ��˺���ִ��ʱ����� */
		/* �������������ȫ�ֱ��� g_tParam.lwip_ip, g_tParam.lwip_net_mask, g_tParam.lwip_gateway */
		lwip_start();
	}

	LCD_ClrScr(FORM_BACK_COLOR);  	/* ������������ɫ */

	fRefresh = 1;
	while(1)
	{
		bsp_Idle();

		uip_pro();
		lwip_pro();

		if (fRefresh)	/* ˢ���������� */
		{
			fRefresh = 0;
		}
		
		if (PHYLinkChanged == 1)	/* DM9162����״̬�仯 */
		{
			PHYLinkChanged = 0;

			fRefresh = 1;
		}

		ucKeyCode = bsp_GetKey();	/* ��ȡ��ֵ, �޼�����ʱ���� KEY_NONE = 0 */
		if (ucKeyCode != KEY_NONE)
		{
			/* �м����� */
			switch (ucKeyCode)
			{
				case  KEY_DOWN_S:		/* S�� */
					break;

				case  KEY_DOWN_C:		/* C�� */
					break;
				
				default:
					break;
			}
		}
	}
}

/*
*********************************************************************************************************
*	������ LwIP ���ֵĴ���
*********************************************************************************************************
*/

struct netif gnetif;

/*
*********************************************************************************************************
*	�� �� ��: uip_ChangeNetParam
*	����˵��: ����������������� �������������ȫ�ֱ��� g_tParam.uip_ip, g_tParam.uip_net_mask, 
*			  g_tParam.uip_gateway 
*	��    ��: ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void lwip_ChangeNetParam(void)
{
//	//uip_ipaddr(ipaddr, 192,168,1,10);	/* ȱʡIP */
//	uip_sethostaddr((uint16_t *)g_tParam.uip_ip);

//	//uip_ipaddr(ipaddr, 255,255,255,0);	/* ��ַ���� */
//	uip_setnetmask((uint16_t *)g_tParam.uip_net_mask);

//	//uip_ipaddr(ipaddr, 192,168,1,1);	/* Ĭ������ */
//	uip_setdraddr((uint16_t *)g_tParam.uip_gateway);
}

/**
  * @brief  Setup the network interface
  * @param  None
  * @retval None
  */
/*Static IP ADDRESS: IP_ADDR0.IP_ADDR1.IP_ADDR2.IP_ADDR3 */
#define IP_ADDR0   ((uint8_t) 192U)
#define IP_ADDR1   ((uint8_t) 168U)
#define IP_ADDR2   ((uint8_t) 1U)
#define IP_ADDR3   ((uint8_t) 86U)
   
/*NETMASK*/
#define NETMASK_ADDR0   ((uint8_t) 255U)
#define NETMASK_ADDR1   ((uint8_t) 255U)
#define NETMASK_ADDR2   ((uint8_t) 255U)
#define NETMASK_ADDR3   ((uint8_t) 0U)

/*Gateway Address*/
#define GW_ADDR0   ((uint8_t) 192U)
#define GW_ADDR1   ((uint8_t) 168U)
#define GW_ADDR2   ((uint8_t) 1U)
#define GW_ADDR3   ((uint8_t) 1U) 

static void Netif_Config(void)
{
  ip_addr_t ipaddr;
  ip_addr_t netmask;
  ip_addr_t gw;
	
#if LWIP_DHCP 
  ip_addr_set_zero_ip4(&ipaddr);
  ip_addr_set_zero_ip4(&netmask);
  ip_addr_set_zero_ip4(&gw);
#else	

  /* IP address default setting */
  IP4_ADDR(&ipaddr, IP_ADDR0, IP_ADDR1, IP_ADDR2, IP_ADDR3);
  IP4_ADDR(&netmask, NETMASK_ADDR0, NETMASK_ADDR1 , NETMASK_ADDR2, NETMASK_ADDR3);
  IP4_ADDR(&gw, GW_ADDR0, GW_ADDR1, GW_ADDR2, GW_ADDR3); 
	
#endif
  
  /* add the network interface */    
  netif_add(&gnetif, &ipaddr, &netmask, &gw, NULL, &ethernetif_init, &ethernet_input);
  
  /*  Registers the default network interface */
  netif_set_default(&gnetif);
  
  ethernet_link_status_updated(&gnetif);
  
#if LWIP_NETIF_LINK_CALLBACK  
  netif_set_link_callback(&gnetif, ethernet_link_status_updated);
#endif
}

/*
*********************************************************************************************************
*	�� �� ��: lwip_start
*	����˵��: ����lwip_start,  �������������ȫ�ֱ��� g_tParam.lwip_ip, g_tParam.lwip_net_mask, 
*			  g_tParam.lwip_gateway 
*	��    ��: ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void lwip_start(void)
{
	/* Initialize the LwIP stack */
	lwip_init();

	/* Configure the Network interface */
	Netif_Config();

	/* Http webserver Init */
	http_server_init();
}

/*
*********************************************************************************************************
*	�� �� ��: lwip_pro
*	����˵��: lwip ��ѯ�����뵽��ѭ����
*	��    ��: ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void lwip_pro(void)
{
    /* Read a received packet from the Ethernet buffers and send it 
       to the lwIP for handling */
    ethernetif_input(&gnetif);

    /* Handle timeouts */
    sys_check_timeouts();

#if LWIP_NETIF_LINK_CALLBACK     
    Ethernet_Link_Periodic_Handle(&gnetif);
#endif
    
#if LWIP_DHCP   
    DHCP_Periodic_Handle(&gnetif);
#endif 
}



/***************************** ���������� www.armfly.com (END OF FILE) *********************************/
