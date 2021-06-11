/*
*********************************************************************************************************
*
*	ģ������ : ������
*	�ļ����� : main.c
*	��    �� : V1.3
*	˵    �� : 
*
*	�޸ļ�¼ :
*		�汾��  ����        ����     ˵��
*		V1.0    2013-01-01 armfly  ��ʽ����
*
*	Copyright (C), 2013-2014, ���������� www.armfly.com
*
*********************************************************************************************************
*/

#include "bsp.h"		/* printf����������������ڣ����Ա����������ļ� */
#include "main.h"
#include "usbd_user.h"

typedef void (*pFunction)(void);

//__attribute__((zero_init)) uint32_t g_JumpInit;		/* ���ᱻ��������ʼ��Ϊ0��ע��noinit����� */
//uint32_t g_JumpInit;		/* ���ᱻ��������ʼ��Ϊ0��ע��noinit����� */

#define g_JumpInit *(uint32_t *)0x20000000

void status_Upgrade(void);
static uint8_t CheckAppOk(void);
static void JumpToApp(void);
static void DispInfo(char *_str, uint8_t _line);
static void DispErrInfo(char *_str, uint8_t _line);
static void DispBootVersion(void);

uint8_t s_MainStatus;

VAR_T g_tVar;

uint8_t s_PCUpdate;

/*
*********************************************************************************************************
*	�� �� ��: main
*	����˵��: c�������
*	��    ��: ��
*	�� �� ֵ: �������(���账��)
*********************************************************************************************************
*/
int main(void)
{		
#if 1
    s_PCUpdate = 0;
    
	if (g_JumpInit == 0x11223344)	/* �����λ���ٽ���APP���ṩһ���ɾ���CPU������APP */
	{
		g_JumpInit = 0;		
		
		JumpToApp();	/* ȥִ��APP���� */
	}
	else if (g_JumpInit == 0x5AA51234)	/* APP���������̼�, ֱ�ӽ���̼�����״̬ */
	{
		g_JumpInit = 0;
		s_MainStatus = 2;	
        s_PCUpdate = 1;
	}
	else
	{
		s_MainStatus = 0;	/* ȥ����ϵ�ʱ�Ƿ��м����� */
	}
#endif
	
	bsp_Init();		/* ���в�����LCD��ʼ�� */
	
	bsp_GetCpuID(g_tVar.CPU_Sn);	/* ��ȡCPU ID */
	
	while (1)
	{
		switch (s_MainStatus)
		{
			case 0:
				bsp_DelayMS(100);	/* �ȴ�100ms������Ƿ��а������� */
				if (bsp_GetKeyState(KID_S) || bsp_GetKeyState(KID_C))
				{
					bsp_ClearKey();
					/* ����S����C���ϵ磬�����̼��������� */
					s_MainStatus = 2;
				}			
				else
				{
					s_MainStatus = 1; 
				}
				break;
				
			case 1:
				if (CheckAppOk())		/* �г�������ת */
				{
					g_JumpInit = 0x11223344;
					
					NVIC_SystemReset();	/* ��λCPU */
				}	
				else	/* û�г���ֱ�ӽ���̼��������� */
				{
					s_MainStatus = 2;
				}
				break;
				
			case 2:		/* �̼��������� */		
				LCD_InitHard();		/* bootֻ������Ҫ�̼�����ʱ�ų�ʼ��LCD */
				status_Upgrade();
				break;
			
			default:
				;
				break;
		}
	}
}

/*
*********************************************************************************************************
*	�� �� ��: status_Upgrade
*	����˵��: USB���������̼�״̬
*	��    ��: ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void status_Upgrade(void)
{
	uint8_t ucKeyCode;		/* �������� */
	uint8_t fRefresh;
	
	g_tVar.Mode = 0;
	usbd_OpenCDC(8);		/* ����USB���⴮�� */
	DispBootVersion();		/* ��ʾ�̼��汾 */
    
//    if (s_PCUpdate == 1)
//    {
//        bsp_DelayMS(2000);
//        usbd_OpenCDC(8);		/* ����USB���⴮�� */
//    }
	
	fRefresh = 1;
	while (s_MainStatus == 2)
	{
		bsp_Idle();

		if (fRefresh)	/* ˢ���������� */
		{
			fRefresh = 0;
		}

		{
			MSG_T msg;
			char buf[100];			
			
			if (bsp_GetMsg(&msg))
			{
				switch (msg.MsgCode)
				{
					case MSG_FILE_IFNO:
						{							
							if (g_tVar.ProgType == 0)
							{
								DispInfo("Ŀ��: CPU Flash", 0);
							}
							else if (g_tVar.ProgType == 1)
							{
								DispInfo("Ŀ��: QSPI Flash", 0);
							}
							else
							{
								DispInfo("Ŀ��: δ֪", 0);
							}							
							sprintf(buf, "Ŀ���ַ: 0x%08X", g_tVar.ProgAddr);
							DispInfo(buf, 1);
							sprintf(buf, "�ļ���С: %dB", g_tVar.ProgSize);
							DispInfo(buf, 2);														
						}						
						break;
						
					case MSG_ERASE_SECTOR_OK:
//						sprintf(buf, "���������ɹ�: %d", msg.MsgParam);
//						DispInfo(buf, 4);						
						break;

					case MSG_ERASE_SECTOR_ERR:
						sprintf(buf, "��������ʧ��: %d", msg.MsgParam);
						DispErrInfo(buf, 4);						
						break;
					
					case MSG_TRANS_FILE:	/* �����ļ� */
						{
							char buf[128];
							
							DispInfo("��ʼ��������", 3);
							sprintf(buf, "����: %3d%%  0x%08X, %4d",msg.MsgParam, g_tVar.FlashAddr, g_tVar.WriteLen);
							DispInfo(buf, 4);
						}
						break;
					
					case MSG_TRANS_ERROR:	/* дflash�ļ�ʧ�� */
						{
							char buf[128];
							
							sprintf(buf, "дFlashʧ��: %08X, %4d", g_tVar.FlashAddr, g_tVar.WriteLen);
							DispErrInfo(buf, 5);
						}
						break;
						
					case MSG_RESET_TO_APP:	/* ��λCPU����APP */
						bsp_DelayMS(100);
                        usbd_CloseCDC();
                        bsp_DelayMS(2000);
                    
						g_JumpInit = 0x11223344;
						NVIC_SystemReset();	/* ��λCPU */
						break;
						
					default:
						break;
				}
			}
		}
		
		ucKeyCode = bsp_GetKey();	/* ��ȡ��ֵ, �޼�����ʱ���� KEY_NONE = 0 */
		if (ucKeyCode != KEY_NONE)
		{
			/* �м����� */
			switch (ucKeyCode)
			{
				case  KEY_DOWN_S:		/* S������ */								
					break;

				case  KEY_LONG_S:	/* S������ */									
					break;				

				case  KEY_UP_C:		/* C���� */	
					break;

				case  KEY_LONG_C:	/* C���ͷ� */
					break;					
				
				default:
					break;
			}
		}
	}
}

/*
*********************************************************************************************************
*	�� �� ��: DispInfo
*	����˵��: ��ʾ������ʾ��Ϣ
*	��    ��: _str : ��ʾ����
*			  _line : �к� 0-8
*	�� �� ֵ: ��
*********************************************************************************************************
*/
static void DispInfo(char *_str, uint8_t _line)
{
	FONT_T tFont;		/* ��������ṹ����� */

	/* ����������� */
	{
		tFont.FontCode = FC_ST_16;	/* ������� 16���� */
		tFont.FrontColor = CL_MAIN_BACK;	/* ������ɫ */
		tFont.BackColor = CL_MAIN_FONT;	/* ���ֱ�����ɫ */
		tFont.Space = 0;				/* ���ּ�࣬��λ = ���� */
	}
		
	LCD_DispStrEx(5, 50 + 25 * _line, _str, &tFont, 200, 0);
}

/*
*********************************************************************************************************
*	�� �� ��: DispErrInfo
*	����˵��: ��ʾ����������Ϣ����ɫ��ʾ��
*	��    ��: _str : ��ʾ����
*			  _line : �к� 0-8
*	�� �� ֵ: ��
*********************************************************************************************************
*/
static void DispErrInfo(char *_str, uint8_t _line)
{
	FONT_T tFont;		/* ��������ṹ����� */

	/* ����������� */
	{
		tFont.FontCode = FC_ST_16;	/* ������� 16���� */
		tFont.FrontColor = CL_RED;	/* ������ɫ */
		tFont.BackColor = CL_MAIN_BACK;	/* ���ֱ�����ɫ */
		tFont.Space = 0;				/* ���ּ�࣬��λ = ���� */
	}
		
	LCD_DispStrEx(5, 50 + 25 * _line, _str, &tFont, 200, 0);
}

/*
*********************************************************************************************************
*	�� �� ��: DispBootVersion
*	����˵��: ��ʾBoot�̼��汾�� �̼��汾��Ƕ�������� startup_stm32h743xx.s
*	��    ��: ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
static void DispBootVersion(void)
{
	FONT_T tFont;		/* ��������ṹ����� */
	char buf[32];
	uint32_t ver;

	LCD_ClrScr(CL_MAIN_BACK);  	/* ������������ɫ */
	
	/* ����������� */
	{
		tFont.FontCode = FC_ST_16;	/* ������� 16���� */
		tFont.FrontColor = CL_MAIN_FONT;	/* ������ɫ */
		tFont.BackColor = CL_MAIN_BACK;	/* ���ֱ�����ɫ */
		tFont.Space = 0;				/* ���ּ�࣬��λ = ���� */
	}
	ver = *(uint32_t *)(0x08000000 + 28);
	sprintf(buf, "H7-TOOL Boot Version: %X.%02X", ver >> 8, ver & 0xFF);
	LCD_DispStr(5, 2, buf, &tFont);
	
	LCD_DispStrEx(5, 2 + 20, "STM32 APP �̼�����", &tFont, 230, 0);
}

/*
*********************************************************************************************************
*	�� �� ��: CheckAppOk
*	����˵��: ���APP�����Ƿ���Ч��
*	��    ��: ��
*	�� �� ֵ: ����1��ʾOK��0��ʾʧ��
*********************************************************************************************************
*/
static uint8_t CheckAppOk(void)
{
#if 1	
	uint32_t StackAddr;
	
	//StackAddr = *(__IO uint32_t*)APPLICATION_ADDRESS + 128 * 1024;
	StackAddr = *(__IO uint32_t*)APPLICATION_ADDRESS + 128 * 1024;
	
	// 0x2408EDF8   0x20020000
	if ((StackAddr & 0x2FF00000) == 0x24000000 || (StackAddr & 0x2FF00000) == 0x20000000)
	{
		return 1;
	}
	return 0;
#else		/* ���APPУ�飨�Ƿ�Ϸ����� δ���� */
	uint32_t ProgLen = *(uint32_t *)0x08008010;		/* app������Ч���� */
	uint32_t ProgCrc = *(uint32_t *)0x08008014;		/* app����CRCУ�� */
	uint16_t crc1, crc2;
	uint32_t flash_size;
	
	flash_size = bsp_GetFlashSize();	/* falsh ���� */
	
	if (ProgLen > flash_size - 8 * 1024)
	{
		return 0;
	}
	
	crc1 = CRC16_Modbus((uint8_t *)(0x08002000 + 0x100), ProgLen / 2);
	crc2 = CRC16_Modbus((uint8_t *)(0x08002000 + ProgLen / 2), ProgLen / 2);
	
	if (crc1 == (ProgCrc >> 16) && crc2 == (ProgCrc & 0xFFFF))
	{
		return 1;
	}
	else
	{
		return 0;
	}
#endif
}


/*
*********************************************************************************************************
*	�� �� ��: JumpToApp
*	����˵��: ��ת��APP���� 0x0800 2000
*	��    ��: ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
static void JumpToApp(void)
{
	pFunction Jump_To_Application;
	uint32_t StackAddr;
	uint32_t ResetVector;
	
	/* ���ж� */
	DISABLE_INT();
	
	SCB_DisableDCache();
	
	StackAddr = *(__IO uint32_t*)APPLICATION_ADDRESS;			//0x08002000;
	ResetVector = *(__IO uint32_t*)(APPLICATION_ADDRESS + 4);	//0x08002004;
	
	/* Initialize user application's Stack Pointer */
	__set_MSP(StackAddr);		/* ������ջָ�� */

	/* �����ж��������ַ */
//	*(uint32_t *)0xE000ED08 = StackAddr;

	/* ����ָ�븳ֵ */
	Jump_To_Application = (pFunction)ResetVector;		
	Jump_To_Application();
}


/*
*********************************************************************************************************
*	�� �� ��: DebugWriteCpuFlash
*	����˵��: ����дCPU flash����
*	��    ��: ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void DebugWriteCpuFlash(void)
{
	uint32_t addr = 0x08100000;
	uint16_t i;
	uint16_t j;
	uint8_t buf0[1024];
	uint8_t buf1[1024];
	uint8_t err = 0;
	
	for (i = 0; i < 1024; i++)
	{
		buf0[i] = i;
	}
	
	addr = 0x08100000;
	err = 0;
	for (i = 0; i < 8; i++)
	{
		if (bsp_EraseCpuFlash(addr + i * 128 * 1024) != 0)
		{
			err = 1;
			break;
		}
		
		for (j = 0; j < 128; j++)
		{
			if (bsp_WriteCpuFlash(addr + j * 1024, buf0, 1024) != 0)
			{
				err = 2;
				break;
			}
		}
		
		for (j = 0; j < 128; j++)
		{
			memcpy(buf1, (uint8_t *)(addr + j * 1024), 1024);
			
			if (memcmp(buf0, buf1, 1024) != 0)
			{
				err = 3;
				break;
			}
		}			
		
		if (err != 0)
		{
			break;
		}
	}
	
	while(1);
}

#if 0	// ���������ݴ�
					if (g_tVar.ProgType == 0)		/* дCPU Flash */
						{
							DispInfo("��ʼ����CPU Flash", 3);
	
							bsp_DelayMS(200);	/* �ȴ�Ӧ���PC�����ݷ������ */
							if (g_tVar.ProgAddr >= APPLICATION_ADDRESS && 
								g_tVar.ProgSize > 1024 &&
								g_tVar.ProgAddr + g_tVar.ProgSize < CPU_FLASH_BASE_ADDR + CPU_FLASH_SIZE)
							{
								uint8_t i;
								uint8_t n;
								uint8_t err = 0;
								
								n = (g_tVar.ProgSize + CPU_FLASH_SECTOR_SIZE - 1) / CPU_FLASH_SECTOR_SIZE;	/* ��Ҫ�������������� */
								
								/* ��1������128K��boot���򣬺���15����������Ӧ�ó��� */
								for (i = 0; i < n; i++)
								{
									if (bsp_EraseCpuFlash(APPLICATION_ADDRESS + i * 128 * 1024) != 0)
									{
										/* ʧ�� */
										err = 1;
										break;
									}
									else
									{
										{
											char buf[128];
											
											sprintf(buf, "���������ɹ�: %d", i);
											DispInfo(buf, 4);
										}
									}
								}	
								if (err == 1)
								{
									{
										char buf[128];
										
										sprintf(buf, "��������ʧ��: %d", i);
										DispInfo(buf, 4);
									}
								}
								else
								{
									g_tVar.BootStatus = 0;	/* ������� - PC����ȡ�ñ�־�ж��Ƿ���� */
								}
							}		
							else
							{
								if (g_tVar.ProgSize < 1024)
								{
									DispErrInfo("�����С����ȷ", 3);
								}
								else
								{
									DispErrInfo("Ŀ���ַ����ȷ", 3);
								}
							}
						}
						else if (g_tVar.ProgType == 1)	/* дQSPI Flash */
						{
							;	/* ��ʱ�ò��� */
							DispErrInfo("��֧�ֵ��ļ�����", 3);
						}
#endif
	
/***************************** ���������� www.armfly.com (END OF FILE) *********************************/
