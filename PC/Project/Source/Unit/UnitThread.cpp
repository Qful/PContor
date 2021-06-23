//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "UnitMain.h"
#include "UnitCalib.h"
#include "UnitProg.h"
#include "UnitLua.h"
#include "UnitThread.h"
#include "UnitUSBBus.h"
#include "UnitSet.h"
#include "bsp.h"
#include "bsp_udp_modbus.h"
#include "bsp_user_lib.h"
#include "file_lib.h"
#include "YbCommDevice.h"
#include "bmp_file.h"

//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TFormTcp *FormTcp;

#define H64_MAX_LEN_USB		(1536 - 18)
#define H64_MAX_LEN_RJ45	1024 			/* (1472 - 18) */

#define DYN_ARRAY_SIZE	(16 * 1024)
static unsigned char s_RxBuf [DYN_ARRAY_SIZE];
static uint32_t  s_RxLen = 0;
static TByteDynArray g_buf;

static char s_USB_RxBuf [DYN_ARRAY_SIZE];
static uint32_t  s_USB_RxLen = 0;
static TByteDynArray g_SerialPortBuf;
int g_usb_rx_len = 0;

int g_H64DataMaxLen;

extern char g_BinFile[8 * 1024 * 1024];

TBitmap *g_bmp = new TBitmap;

//---------------------------------------------------------------------------
__fastcall TFormTcp::TFormTcp(TComponent* Owner)
	: TForm(Owner)
{

}

void __fastcall TFormTcp::FormCreate(TObject *Sender)
{
	//g_buf = RawToBytes(RxBuf, 1500);    // �� unsigned char ����ת��Ϊ TByteDynArray ����ָ��

	bsp_AddLog("FormTcp Create Begin");

	g_buf.set_length(DYN_ARRAY_SIZE);
	g_SerialPortBuf.set_length(DYN_ARRAY_SIZE);

	bsp_AddLog("FormTcp Create End");
	Thread1->Start();  //��FormSet �������������߳�
}

//װ���ļ������ֵ�������
void LoadFolderNameToMem(String FileName)
{
	AnsiString s;
	AnsiString path0;
	AnsiString name;
	int i;

	path0 = AppExePath();	//ExtractFilePath(Application->ExeName);
	name =  FileName;

	/*
		path0 =  /EMMC/H7-TOOL/Programmer/User/Demo
		0:/H7-TOOL/EMMC/H7-TOOL/Programmer/User/Demo
	*/
	//s = "0:/H7-TOOL" + (AnsiString)&name.c_str()[path0.Length()];
	s = "0:/" + (AnsiString)&name.c_str()[path0.Length() + 6];

	FormTcp->mLua.Func = H64_CREATE_FOLDER;	/* �����ļ��� */

	FormTcp->mLua.DataLen = s.Length() + 1;
	for (i = 0; i < FormTcp->mLua.DataLen; i++)
	{
		if (s.c_str()[i] == '\\')
		{
			s.c_str()[i] = '/';
		}
		FormTcp->mLua.DataBuf[i] = s.c_str()[i];
	}
	FormTcp->mLua.DataBuf[i] = 0;

	//FormProg->LabelTransFile->Caption = s;
	//if (FormProg->CheckBoxDispFileLog->Checked)
	{
		Form1->MemoLuaOut->Lines->Add(s);
	}
}

//װ���ļ����ݵ�������
void LoadFileDataToMem(String FileName)
{
	AnsiString s;
	char s2[120];
	AnsiString path0;
	AnsiString name;
	int i;
	char MD5[16];
	int32_t FileSize;

	path0 = AppExePath();	//ExtractFilePath(Application->ExeName);
	name =  FileName;

	//s = "0:/H7-TOOL" + (AnsiString)&name.c_str()[path0.Length()];
	s = "0:/" + (AnsiString)&name.c_str()[path0.Length() + 6];

	FormTcp->mLua.Func = H64_WRITE_FILE;	/* �����ļ��� */

	FormTcp->mLua.DataLen = 0;

	/* �ļ���С��4�ֽ�) + MD5 (16�ֽڣ� + �ļ������� + �ļ��������255�ֽڣ���0�������� + �ļ����� */
	{
		FileSize = GetFileMD5(FileName, MD5);

		Uint32ToBEBuf(FileSize,  &FormTcp->mLua.DataBuf[FormTcp->mLua.DataLen]);
		FormTcp->mLua.DataLen += 4;

		memcpy(&FormTcp->mLua.DataBuf[FormTcp->mLua.DataLen], MD5, 16);
		FormTcp->mLua.DataLen += 16;
	}

	/* �ļ������� */
	FormTcp->mLua.DataBuf[FormTcp->mLua.DataLen++] = s.Length();
	/* �ļ��� */
	for (i = 0; i < s.Length(); i++)
	{
		if (s.c_str()[i] == '\\')
		{
			s.c_str()[i] = '/';
		}
		FormTcp->mLua.DataBuf[FormTcp->mLua.DataLen++] = s.c_str()[i];
	}

//	HexBufToAscii(MD5, s2, 16);
//	Form1->MemoLuaOut->Lines->Add("д�ļ�: " + s + " (" + IntToStr(FileSize) + ", MD5: "
//		+ (AnsiString)s2 + ")" );
//	FormProg->LabelTransFile->Caption = "д�ļ�: " + s + " (" + IntToStr(FileSize) + ", MD5: "
//		+ (AnsiString)s2 + ")";
	s = s + " (" + IntToStr(FileSize) + ")";

	//if (FormProg->CheckBoxDispFileLog->Checked)
	{
		Form1->MemoLuaOut->Lines->Add(s);
	}

	/* ��������ļ����� */
	{
		int iFileHandle;
		int iBytesRead;
		int iFileLength;

		iFileHandle = FileOpen(FileName, fmOpenRead);
		if (iFileHandle > 0)
		{
			iFileLength = FileSeek(iFileHandle, 0, 2);

			if (iFileLength <= LUA_DATA_MAX_SIZE - 256)
			{
				FileSeek(iFileHandle, 0, 0);
				FileRead(iFileHandle, &FormTcp->mLua.DataBuf[FormTcp->mLua.DataLen], iFileLength);
				FileClose(iFileHandle);

				FormTcp->mLua.DataLen += iFileLength;

				//���Դ��룬д���ļ�debug
//				FileName = FileName + ".Read";
//				iFileHandle = FileCreate(FileName);
//				FileWrite(iFileHandle, FormTcp->mLua.DataBuf, FormTcp->mLua.DataLen);
//				FileClose(iFileHandle);
			}
			else
			{
				Form1->MemoLuaOut->Lines->Add("�ļ�����");
			}
		}
	}
}

//--����оƬ�ļ�--------------------------------------------------------------------
void __fastcall TFormTcp::UpdateChipFile(void)
{
	static int s_idx = 0;
	static int s_time = 0;
	static int s_retry = 0;
	static int s_head_len = 0;		/* �ļ�ͷ���� */

	switch (g_tCmd.CmdStatus)
	{
		case CMDS_UPDATE_CHIP_FILE:
			s_idx = 0;
			g_tCmd.CmdStatus++;

			if (g_tIniParam.CommInterface == 0)
			{
				g_H64DataMaxLen = H64_MAX_LEN_RJ45;
			}
			else
			{
				g_H64DataMaxLen = H64_MAX_LEN_USB;
			}
			Form1->ProgressBar1->Position = 0;	// ������
			s_time = bsp_GetRunTime();
			break;

		case CMDS_UPDATE_CHIP_FILE + 1:
			{
				String name;

				if (FormProg->ListFileName->Strings[s_idx] == "")
				{
					name = FormProg->ListFilePath->Strings[s_idx];
				}
				else
				{
					name = FormProg->ListFilePath->Strings[s_idx] + "/" +
						FormProg->ListFileName->Strings[s_idx];
				}

				if (FormProg->ListFileType->Strings[s_idx] == "Dir")	// �ļ���
				{
					LoadFolderNameToMem(name);
				}
				else 	// �ļ�
				{
					LoadFileDataToMem(name);
				}
			}
			g_tCmd.CmdStatus++;
			break;

		case CMDS_UPDATE_CHIP_FILE + 2:
			mLua.Offset = 0;

			s_head_len = mLua.DataBuf[20] + 21;
			s_retry = 0;
			g_tCmd.CmdStatus++;
			break;

		case CMDS_UPDATE_CHIP_FILE + 3:
			g_tCmd.err = 0;

			/* �ļ���С��4�ֽ�) + MD5 (16�ֽڣ� + �ļ������� + �ļ��������255�ֽڣ���0�������� + �ļ����� */
			if (mLua.Offset == 0)  /* ��1��, �ļ�ͷ + ���1K���� */
			{
				if (mLua.DataLen <= g_H64DataMaxLen + s_head_len)
				{
					mLua.PackagLen = mLua.DataLen;
				}
				else
				{
					mLua.PackagLen = g_H64DataMaxLen + s_head_len;
				}

				udp_Send64H(mLua.Func, mLua.DataLen, 0, mLua.PackagLen, &mLua.DataBuf[mLua.Offset]);
				//Form1->MemoLuaOut->Lines->Add(" -- Addr = " + IntToStr(0) +
				//	" len = " + IntToStr(mLua.PackagLen));
			}
			else  	/* ��2���Ժ� */
			{
				{
					int remain;

					remain = mLua.DataLen - mLua.Offset;	// ʣ���ֽ�
					if (remain > g_H64DataMaxLen)
					{
						mLua.PackagLen = g_H64DataMaxLen;
					}
					else
					{
						mLua.PackagLen = remain;
					}
				}
				udp_Send64H(mLua.Func, mLua.DataLen, mLua.Offset - s_head_len, mLua.PackagLen, &mLua.DataBuf[mLua.Offset]);
				//Form1->MemoLuaOut->Lines->Add(" -- Addr = " + IntToStr((int)mLua.Offset - s_head_len) +
				//	" len = " + IntToStr(mLua.PackagLen));   // debug
			}
			mLastTime = bsp_GetRunTime();
			g_tCmd.CmdStatus++;
			break;

		case CMDS_UPDATE_CHIP_FILE + 4:
			if (g_tCmd.RxOk == 1)
			{
				if (g_tCmd.err == 0)  	/* �ļ�д��ɹ� */
				{
					mLua.Offset += mLua.PackagLen;
					if (mLua.Offset < mLua.DataLen)
					{
						g_tCmd.CmdStatus--;  	// ��������
						break;
					}
					else
					{
						if (++s_idx >= FormProg->ListFileName->Count)
						{
							g_tCmd.CmdStatus++;   // �������
						}
						else
						{
							g_tCmd.CmdStatus =  CMDS_UPDATE_CHIP_FILE + 1;	// ������һ���ļ�
						}
						Form1->MemoLuaOut->Lines->Add("  ����ɹ�");
					}
				}
				else if (g_tCmd.err == 1)  	/* �ļ�������ͬ */
				{
					if (++s_idx >= FormProg->ListFileName->Count)
					{
						g_tCmd.CmdStatus++;   // �������
					}
					else
					{
						g_tCmd.CmdStatus =  CMDS_UPDATE_CHIP_FILE + 1;	// ������һ���ļ�
					}
					Form1->MemoLuaOut->Lines->Add("  �ļ���ͬ");
				}
				else 	/* д����� 2д�ļ�ʧ�� 3����Ŀ¼ʧ�� 4 MD5У��ʧ�� */
				{
					String name;

					name = (AnsiString)(char *)FormTcp->mLua.DataBuf;

					g_tCmd.CmdStatus = 0;

					if (g_tCmd.err == 2)  Form1->MemoLuaOut->Lines->Add("  д�ļ�ʧ�� ");
					else if (g_tCmd.err == 3)  Form1->MemoLuaOut->Lines->Add("  ����Ŀ¼ʧ�� ");
					else if (g_tCmd.err == 4)  Form1->MemoLuaOut->Lines->Add("  MD5У��ʧ�� ");
					else Form1->MemoLuaOut->Lines->Add("  ����д����� ");
				}
				Form1->ProgressBar1->Position = 100 * s_idx / FormProg->ListFileName->Count;	// ������
				break;
			}
			if (bsp_CheckRunTime(mLastTime) > 3000)
			{
				if (++s_retry > 3)
				{
					String name;

					name = (AnsiString)(char *)FormTcp->mLua.DataBuf;

					g_tCmd.CmdStatus = 0;
					//FormProg->LabelTransFile->Caption =  "ͨ�ų�ʱ " + name;
					Form1->MemoLuaOut->Lines->Add("--ͨ�ų�ʱ����ֹ");
				}
				else
				{
					Form1->MemoLuaOut->Lines->Add("--�����ش�");
					g_tCmd.CmdStatus--;  	// ��������
				}
			}
			break;

		case CMDS_UPDATE_CHIP_FILE + 5:
			{
				AnsiString s1;
				s_time = bsp_GetRunTime() - s_time;
				//Form1->MemoLuaOut->Lines->Add("���سɹ� (" + IntToStr(s_time) + " ms)");
				s1 = "����ɹ� (" + s1.sprintf("%d.%03d", s_time / 1000, s_time % 1000) + " s) ��"
					+ IntToStr(FormProg->ListFileName->Count) + "���ļ�";
				Form1->ProgressBar1->Position = 100;	// ������
				Form1->ProgressBar1->Refresh();

				Form1->MemoLuaOut->Lines->Add(s1);
			}

			g_tCmd.CmdStatus = 0;


			break;
	}
}

/*----LUA��������----------------------------------*/
void __fastcall TFormTcp::DownLuaFile(void)
{
	static int s_scan_status = 0;
	static int s_retry = 0;
	static int s_LastTime;
	static int s_idx;

	switch (g_tCmd.CmdStatus)
	{
		case CMDS_DOWNLOAD_LUA:
			mLua.Offset = 0;
			g_tCmd.CmdStatus++;
			break;

		case CMDS_DOWNLOAD_LUA + 1:
			if (mLua.DataLen <= 1024)
			{
				mLua.PackagLen = mLua.DataLen;
			}
			else
			{
				int remain;

				remain = mLua.DataLen - mLua.Offset;	// ʣ���ֽ�
				if (remain > 1024)
				{
					mLua.PackagLen = 1024;
				}
				else
				{
					mLua.PackagLen = remain;
                }
            }
			udp_Send64H(mLua.Func, mLua.DataLen, mLua.Offset, mLua.PackagLen, &mLua.DataBuf[mLua.Offset]);
			mLastTime = bsp_GetRunTime();
			g_tCmd.CmdStatus++;
			break;

		case CMDS_DOWNLOAD_LUA + 2:
			if (g_tCmd.RxOk == 1)
			{
				mLua.Offset += mLua.PackagLen;
				if (mLua.Offset < mLua.DataLen)
				{
					g_tCmd.CmdStatus--;  	// ��������
				}
				else
				{
					g_tCmd.CmdStatus++;   // �������
				}
			}
			if (bsp_CheckRunTime(mLastTime) > 500)
			{
				g_tCmd.CmdStatus = 0;
				if (mLua.Func == 1)
				{
					Form1->MemoLuaOut->Lines->Add("ͨ�ų�ʱ������ʧ��");
				}
			}
			break;

		case CMDS_DOWNLOAD_LUA + 3:
			if (mLua.Func == H64_LUA_RUN_WITH_RESET)
			{
				Form1->MemoLuaOut->Lines->Add("���سɹ�");
			}
			g_tCmd.CmdStatus = 0;
			break;
	}
}

/************************�������� *************************/
void __fastcall TFormTcp::UpdateFirmware(void)
{
	static int s_scan_status = 0;
	static int s_retry = 0;
	static int s_LastTime;
	static int s_idx;

	switch (g_tCmd.CmdStatus)
	{
		case CMDS_PRO_UPGRADE - 5:     /* ����������1.дapp����BOOT��־ */
			udp_Send06H(REG03_RESET_TO_BOOT, 2);   /* ֪ͨAPP����boot�̼�����״̬ */
			s_LastTime = bsp_GetRunTime();
			g_tCmd.CmdStatus++;
			break;

		case CMDS_PRO_UPGRADE - 4:
			if (bsp_CheckRunTime(s_LastTime) > 500)
			{
				FormSet->LabelUpgrade->Caption = "�豸��Ӧ��";
				Form1->MemoLuaOut->Lines->Add(FormSet->LabelUpgrade->Caption);
				g_tCmd.CmdStatus = CMDS_PRO_UPGRADE_ERR;
				break;
			}

			if (g_tCmd.RxOk == 1)
			{
				g_tCmd.CmdStatus++;
				s_LastTime = bsp_GetRunTime();
			}
			break;

		case CMDS_PRO_UPGRADE - 3:  	/* ����boot��USB���ڻḴλ */
			if (bsp_CheckRunTime(s_LastTime) > 2000)
			{
				g_tCmd.CmdStatus++;
			}
			break;

		case CMDS_PRO_UPGRADE - 2:  	/* ����boot��USB���ڻḴλ */
			if (Form1->YbCommDevice1->Active == false)
			{
				Form1->YbCommDevice1->Active = true;
			}
			Form1->YbCommDevice1->Active = true;
			Form1->YbCommDevice1->Parity = 0;
			g_tCmd.CmdStatus++;
			s_LastTime = bsp_GetRunTime();
			break;

		case CMDS_PRO_UPGRADE - 1:  	/* ����boot��USB���ڻḴλ */
			if (bsp_CheckRunTime(s_LastTime) > 2000)
			{
				g_tCmd.CmdStatus++;
			}
			break;

		case CMDS_PRO_UPGRADE:             	 /* ����������1.дapp����BOOT��־ */
			mUpgradeTime = 0;		/* ������ʱ��ʼΪ0*/
			RecordID = 0;
			if (Form1->mFileLength == 0)
			{
				FormSet->LabelUpgrade->Caption = "����ļ���";
                mUserCmdFlag = 0;			 /* �˳�ѭ�� */
				break;
			}

			{
				Form1->MemoLuaOut->Lines->Add("�����ļ���:" + ExtractFileName(Form1->OpenDialog1->FileName));

			}
			{
				uint16_t reg_buf[6];

				reg_buf[0] = 0; /* APP��������: 0 = CPU�ڲ�Flash ; 1 = QSPI Flash */
				reg_buf[1] = (APPLICATION_ADDRESS >> 24) * 256 + ((APPLICATION_ADDRESS >> 16) & 0xFF);
				reg_buf[2] = (APPLICATION_ADDRESS >> 8) * 256 + ((APPLICATION_ADDRESS >> 0) & 0xFF);
				reg_buf[3] = (Form1->mFileLength >> 24) * 256 + ((Form1->mFileLength >> 16) & 0xFF);
				reg_buf[4] = (Form1->mFileLength >> 8) * 256 + ((Form1->mFileLength >> 0) & 0xFF);
				udp_Send10H(REG03_BOOT_PROG_TYPE, 5, reg_buf);
			}
			Form1->TimerUpgrade->Tag = 0;
			FormSet->LabelUpgrade->Caption = "1.�����ļ���Ϣ";
			Form1->MemoLuaOut->Lines->Add(FormSet->LabelUpgrade->Caption);
			s_LastTime = bsp_GetRunTime();
			g_tCmd.CmdStatus++;            		 /* ������һ���ȴ�Ӧ�� */
			break;

		case CMDS_PRO_UPGRADE + 1:
			if (bsp_CheckRunTime(s_LastTime) > 500)
			{
				FormSet->LabelUpgrade->Caption = "�豸��Ӧ��";
				Form1->MemoLuaOut->Lines->Add(FormSet->LabelUpgrade->Caption);
				g_tCmd.CmdStatus = CMDS_PRO_UPGRADE_ERR;
				break;
			}

			if (g_tCmd.RxOk == 1)
			{
				FormSet->LabelUpgrade->Caption = " -- �ļ���Ϣ����OK";
				Form1->MemoLuaOut->Lines->Add(FormSet->LabelUpgrade->Caption);
				g_tCmd.CmdStatus++;
				s_idx = 0;
				Form1->MemoLuaOut->Lines->Add("2.��ʼ����Flash");
				s_retry = 0;
			}
			break;

		case CMDS_PRO_UPGRADE + 2:
			udp_Send06H(REG03_BOOT_ERASE_SECTOR, s_idx);
			s_LastTime = bsp_GetRunTime();
			g_tCmd.CmdStatus++;            		 /* ������һ���ȴ�Ӧ�� */
			break;

		case CMDS_PRO_UPGRADE + 3:
			if (bsp_CheckRunTime(s_LastTime) > 5000)
			{
				Form1->MemoLuaOut->Lines->Add(" --����ʧ��, " + IntToStr(s_idx));
				if (++s_retry >= 3)
				{
					g_tCmd.CmdStatus = CMDS_PRO_UPGRADE_ERR;
				}
				else
				{
					g_tCmd.CmdStatus--;	/* ���� */
				}
				break;
			}

			if (g_tCmd.RxOk == 1)
			{
				int n;

				Form1->MemoLuaOut->Lines->Add(" --���������ɹ�, " + IntToStr(s_idx));
				n = Form1->mFileLength / (128 * 1024);
				if (Form1->mFileLength % (128 * 1024))     /* ����128K������ */
				{
					n++;
				}
				if (++s_idx >= n)
				{
					g_tCmd.CmdStatus++;  /* ������� */
					s_retry = 0;
					Form1->MemoLuaOut->Lines->Add("3.��ʼ��������ļ�...");
				}
				else
				{
					g_tCmd.CmdStatus--;	/* ���� */
				}
			}
			break;

		case CMDS_PRO_UPGRADE + 4:             /* ����������4.���ͳ���� */
			if (RecordID == Form1->mFileLength / UPGRADE_PACKET_LEN)  	/* ���һ�����ݵ����� */
			{
				/* ����1��������,
				����2����¼��,
				����3����¼����,
				����4����Ч���ݳ���,
				����5�����ݰ� */
				udp_Send15H(12 + UPGRADE_PACKET_LEN, RecordID , UPGRADE_PACKET_LEN,  Form1->mFileLength % UPGRADE_PACKET_LEN, &Form1->mFileBuf[RecordID * UPGRADE_PACKET_LEN]);
			}
			else
			{
				udp_Send15H(12 + UPGRADE_PACKET_LEN, RecordID, UPGRADE_PACKET_LEN, UPGRADE_PACKET_LEN, &Form1->mFileBuf[RecordID * UPGRADE_PACKET_LEN]);
			}
			s_LastTime = bsp_GetRunTime();
			g_tCmd.CmdStatus++;
			break;

		case CMDS_PRO_UPGRADE + 5:
			if ((bsp_CheckRunTime(s_LastTime) > 1000))   /* ��ʱ��û��Ӧ�� */
			{
				FormSet->LabelUpgrade->Caption = " --Ӧ��ʱ";
				Form1->MemoLuaOut->Lines->Add(FormSet->LabelUpgrade->Caption);
				if (++s_retry >= 3)
				{
					uint32_t waddr;
					char buf[32];

					waddr = RecordID * UPGRADE_PACKET_LEN + 0x08020000;
					sprintf(buf, "0x%08X", waddr);
					FormSet->LabelUpgrade->Caption = " --����д���쳣 " + (AnsiString)buf;
					Form1->MemoLuaOut->Lines->Add(FormSet->LabelUpgrade->Caption);
					g_tCmd.CmdStatus = CMDS_PRO_UPGRADE_ERR;
					break;
				}
				else
				{
					g_tCmd.CmdStatus--;   /* ���� */
				}
			}

			if (g_tCmd.RxOk == 1)
			{
				if (++RecordID > (Form1->mFileLength / UPGRADE_PACKET_LEN))  /* �������� */
				{
					/* ��ʼ��ʱ����¼���������õ�ʱ�� */
					Form1->TimerUpgrade->Enabled = false;

					g_tCmd.CmdStatus++;
					s_retry = 0;
				}
				else
				{
					/* ��ʼ��ʱ����¼���������õ�ʱ�� */
					Form1->TimerUpgrade->Enabled = true;

					/* ����������ʾ�ٷֱ� */
					{
						AnsiString s;

						s.sprintf("���ȣ�%d%%", RecordID * UPGRADE_PACKET_LEN * 100  / Form1->mFileLength);
						FormSet->LabelUpgrade->Caption = s;
						/* ��������ʱ�� */
						s.sprintf("��ʱ��%d��", Form1->TimerUpgrade->Tag);
						FormSet->LabelUPDTime->Caption = s;
					}

					s_retry = 0;
					g_tCmd.CmdStatus--;
				}
			}
			break;

		case CMDS_PRO_UPGRADE + 6:    				   /* ����������5.�رճ���������־ */
			udp_Send06H(REG03_BOOT_TRANS_END, 1); 	/* ֪ͨ�豸����������� */
			s_LastTime = bsp_GetRunTime();
			g_tCmd.CmdStatus++;
			break;

		case CMDS_PRO_UPGRADE + 7:
			if ((bsp_CheckRunTime(s_LastTime) > 500))   /* ��ʱ��û��Ӧ�� */
			{
				if (++s_retry >= 3)
				{
					Form1->MemoLuaOut->Lines->Add("����д���쳣");
					g_tCmd.CmdStatus = CMDS_PRO_UPGRADE_ERR;
					break;
				}
				else
				{
					g_tCmd.CmdStatus--;   /* ���� */
				}
			}

			if (g_tCmd.RxOk == 1)
			{
				Form1->MemoLuaOut->Lines->Add("4.���������");
				s_retry = 0;

//				if (FormSet->CheckBoxAutoUpgrade->Checked)  // �Զ�����
//				{
//					g_tCmd.CmdStatus = CMDS_PRO_UPGRADE + 20;
//				}
//				else
				{
					g_tCmd.CmdStatus++;
				}
			}
			break;

		case CMDS_PRO_UPGRADE + 8:
			udp_Send06H(REG03_RESET_TO_BOOT, 1); 	/* ֪ͨϵͳ��λ */
			s_LastTime = bsp_GetRunTime();
			g_tCmd.CmdStatus++;
			break;

		case CMDS_PRO_UPGRADE + 9:
			if ((bsp_CheckRunTime(s_LastTime) > 200))   /* ��ʱ��û��Ӧ�� */
			{
				if (++s_retry >= 3)
				{
					FormSet->LabelUpgrade->Caption = "��λʧ��";
					Form1->MemoLuaOut->Lines->Add(FormSet->LabelUpgrade->Caption);
					g_tCmd.CmdStatus = CMDS_PRO_UPGRADE_ERR;
					break;
				}
				else
				{
					g_tCmd.CmdStatus--;   /* ���� */
				}
			}

			if (g_tCmd.RxOk == 1)
			{
				FormSet->LabelUpgrade->Caption = "5.�����ɹ����Ѹ�λ";
				Form1->MemoLuaOut->Lines->Add(FormSet->LabelUpgrade->Caption);
				g_tCmd.CmdStatus = CMDS_PRO_UPGRADE_OK;
			}
			break;

		case CMDS_PRO_UPGRADE_ERR:
		case CMDS_PRO_UPGRADE_OK:
//			if (FormSet->CheckBoxAutoUpgrade->Checked)  // �Զ�����
//			{
//				g_tCmd.CmdStatus = CMDS_PRO_UPGRADE + 20;
//			}
//			else
			{
				g_tCmd.CmdStatus = 0;
				g_tCmd.UserReq = 0;
			}
			break;

		case CMDS_PRO_UPGRADE + 20: 	/* �ȴ����ڰε� */
			FormSet->LabelUpgrade->Caption = "6.�ȴ�USB���Ƴ�";
			Form1->MemoLuaOut->Lines->Add(FormSet->LabelUpgrade->Caption);
			g_tCmd.CmdStatus++;
			s_retry = 0;
			break;

		case CMDS_PRO_UPGRADE + 21:		/* ����03�����ж�USB���Ƿ�ε� */
			udp_Send03H(REG03_BOOT_HARD_VER, 1);
			s_LastTime = bsp_GetRunTime();
			g_tCmd.CmdStatus++;
			break;

		case CMDS_PRO_UPGRADE + 22:
			if ((bsp_CheckRunTime(s_LastTime) > 100))   /* ��ʱ��û��Ӧ�� */
			{
				if (++s_retry >= 3)
				{
					FormSet->LabelUpgrade->Caption = "-USB���Ѿ��Ƴ�";
					Form1->MemoLuaOut->Lines->Add(FormSet->LabelUpgrade->Caption);
					g_tCmd.CmdStatus++;
					break;
				}
				else
				{
					g_tCmd.CmdStatus--;   /* ���� */
				}
			}

			if (g_tCmd.RxOk == 1)
			{
				g_tCmd.CmdStatus--;   /* ���� */
			}
			break;

	   case CMDS_PRO_UPGRADE + 23:
			FormSet->LabelUpgrade->Caption = "7.�ȴ�USB�߲���";
			Form1->MemoLuaOut->Lines->Add(FormSet->LabelUpgrade->Caption);
			g_tCmd.CmdStatus++;
			s_retry = 0;
			break;

	   case CMDS_PRO_UPGRADE + 24:
			Form1->YbCommDevice1->Active = false;
			Form1->YbCommDevice1->Active = true;
			g_tCmd.CmdStatus++;
			s_retry = 0;
			break;

	   case CMDS_PRO_UPGRADE + 25:
			udp_Send03H(REG03_BOOT_HARD_VER, 1);
			s_LastTime = bsp_GetRunTime();
			g_tCmd.CmdStatus++;
			break;

		case CMDS_PRO_UPGRADE + 26:
			if ((bsp_CheckRunTime(s_LastTime) > 100))   /* ��ʱ��û��Ӧ�� */
			{
				if (++s_retry >= 10)
				{
//					if (FormSet->CheckBoxAutoUpgrade->Checked)  // �Զ�����
//					{
//						g_tCmd.CmdStatus -= 2;
//					}
//					else
					{
						g_tCmd.CmdStatus = 0;
						g_tCmd.UserReq = 0;
					}
					break;
				}
				else
				{
					g_tCmd.CmdStatus--;   /* ���� */
				}
			}

			if (g_tCmd.RxOk == 1)
			{
				g_tCmd.CmdStatus =  CMDS_PRO_UPGRADE;
			}
			break;
		}
}

//--��оƬ���ļ�--------------------------------------------------------------------
void __fastcall TFormTcp::ReadChipToFile(void)
{
	static int s_idx = 0;
	static int s_time = 0;
	static int s_retry = 0;
	static int s_head_len = 0;		/* �ļ�ͷ���� */

	switch (g_tCmd.CmdStatus)
	{
		case CMDS_SWD_READ_FLASH:
			s_idx = 0;
			g_tCmd.CmdStatus++;

			mLua.Func = 0;   /* ����1�� */
			mLua.FlashBaseAddr = MyStrToInt(FormProg->EditReadAddr->Text)
				+  MyStrToInt(FormProg->EditReadOffset->Text) * 1024;
			mLua.Offset = 0;
			mLua.PackagLen = 1024;
			mLua.DataLen = 1024 * MyStrToInt(FormProg->EditReadFlashSize->Text);	//�ļ�����
			Form1->ProgressBar1->Position = 0;	// ������
			s_time = bsp_GetRunTime();
			break;

		case CMDS_SWD_READ_FLASH + 1:
			g_tCmd.CmdStatus++;
			break;

		case CMDS_SWD_READ_FLASH + 2:
			s_retry = 0;
			g_tCmd.CmdStatus++;
			break;

		case CMDS_SWD_READ_FLASH + 3:
			g_tCmd.err = 0;
//void udp_Send66H(uint16_t _func, uint32_t _addr, uint32_t _len, uint8_t *_buf)
			udp_Send66H(mLua.Func, mLua.FlashBaseAddr + mLua.Offset, mLua.PackagLen, 0);
			mLastTime = bsp_GetRunTime();
			g_tCmd.CmdStatus++;
			break;

		case CMDS_SWD_READ_FLASH + 4:
			if (g_tCmd.RxOk == 1)
			{
				if (g_tCmd.err == 0)  	/* �ɹ� */
				{
					mLua.Func = 1;		/* ����2�����Ժ�İ� */
					mLua.Offset += mLua.PackagLen;
					if (mLua.Offset < mLua.DataLen)
					{
						g_tCmd.CmdStatus--;  	// ��������
					}
					else
					{
						g_tCmd.CmdStatus++;   // �������
					}
				}
				else     /* ʧ�� */
				{
					g_tCmd.CmdStatus = CMDS_SWD_READ_FLASH + 10;
					Form1->MemoLuaOut->Lines->Add("���洢��ʧ�� ");
				}
				Form1->ProgressBar1->Position = 100 * mLua.Offset / mLua.DataLen;	// ������
				break;
			}
			if (bsp_CheckRunTime(mLastTime) > 1000)
			{
				if (++s_retry > 3)
				{
					g_tCmd.CmdStatus = 0;
					//Form1->LabelTransFile->Caption =  "ͨ�ų�ʱ ";
					Form1->MemoLuaOut->Lines->Add("ͨ�ų�ʱ ");
				}
				else
				{
					g_tCmd.CmdStatus--;  	// ��������
				}
			}
			break;

		case CMDS_SWD_READ_FLASH + 5:
			{
				AnsiString s1;
				s_time = bsp_GetRunTime() - s_time;

				s1 = "����ɹ� (" + s1.sprintf("%d.%03d", s_time / 1000, s_time % 1000) + " s) "
					+ IntToStr((int)mLua.DataLen / 1024) + "KB";
				Form1->ProgressBar1->Position = 100;	// ������
				Form1->ProgressBar1->Refresh();

				Form1->MemoLuaOut->Lines->Add(s1);

				// ���浽�ļ� read_0x08000000_1024kb.bin
				{
					AnsiString name;
					AnsiString str;
					AnsiString now;

					now = Now().FormatString("YYYYMMDD_HHmmSS_");
					str.sprintf("read_0x%08X_%dkB.bin", mLua.FlashBaseAddr, mLua.DataLen / 1024);
					name = AppExePath() + "\\readback\\" + now + str;

					SaveMemToFile(name, mLua.DataBuf, mLua.DataLen);

					Form1->MemoLuaOut->Lines->Add("���浽�ļ�: " + name);
				}

			}
			g_tCmd.CmdStatus = CMDS_SWD_READ_FLASH + 10;
			break;

		case CMDS_SWD_READ_FLASH + 10:
			g_tCmd.CmdStatus = 0;
			break;
	}
}

//--��H7-TOOL����Ļ--------------------------------------------------------------------
void __fastcall TFormTcp::ReadScreen(void)
{
	static int s_idx = 0;
	static int s_time = 0;
	static int s_retry = 0;
	static int s_head_len = 0;		/* �ļ�ͷ���� */

	switch (g_tCmd.CmdStatus)
	{
		case CMDS_READ_SCREEN:
			s_idx = 0;
			g_tCmd.CmdStatus++;

			mLua.Func = 0;   /* ����1�� */
			mLua.FlashBaseAddr = 0;   	// ��ʼ��ַ = 0
			mLua.Offset = 0;
			mLua.PackagLen = 960;	//
			mLua.DataLen = 120 * 960;			//�ļ�����  240 * 240 =  120 * 960
			//Form1->ProgressBar1->Position = 0;	// ������
			s_time = bsp_GetRunTime();
			break;

		case CMDS_READ_SCREEN + 1:
			g_tCmd.CmdStatus++;
			break;

		case CMDS_READ_SCREEN + 2:
			s_retry = 0;
			g_tCmd.CmdStatus++;
			break;

		case CMDS_READ_SCREEN + 3:
			g_tCmd.err = 0;
			mLua.Func = H66_READ_DISP_MEM;	// ���Դ�
			udp_Send66H(mLua.Func, mLua.FlashBaseAddr + mLua.Offset, mLua.PackagLen, 0);
			mLastTime = bsp_GetRunTime();
			g_tCmd.CmdStatus++;
			break;

		case CMDS_READ_SCREEN + 4:
			if (g_tCmd.RxOk == 1)
			{
				if (g_tCmd.err == 0)  	/* �ɹ� */
				{
					mLua.Func = H66_READ_DISP_MEM;		/* ����2�����Ժ�İ� */
					mLua.Offset += mLua.PackagLen;
					if (mLua.Offset < mLua.DataLen)
					{
						g_tCmd.CmdStatus--;  	// ��������
					}
					else
					{
						g_tCmd.CmdStatus++;   // �������
					}
				}
				else     /* ʧ�� */
				{
					g_tCmd.CmdStatus = CMDS_READ_SCREEN + 10;
					//Form1->MemoLuaOut->Lines->Add("����Ļ����ʧ�� ");
				}
				//Form1->ProgressBar1->Position = 100 * mLua.Offset / mLua.DataLen;	// ������
				break;
			}
			if (bsp_CheckRunTime(mLastTime) > 1000)
			{
				if (++s_retry > 3)
				{
					g_tCmd.CmdStatus = 0;
					//Form1->LabelTransFile->Caption =  "ͨ�ų�ʱ ";
					//Form1->MemoLuaOut->Lines->Add("ͨ�ų�ʱ ");
				}
				else
				{
					g_tCmd.CmdStatus--;  	// ��������
				}
			}
			break;

		case CMDS_READ_SCREEN + 5:
			{
				{
					TMemoryStream *ms = new TMemoryStream;
					int bmpLen;

					bmpLen = SaveScreenToBmp(240, 240, mLua.DataBuf, g_BinFile);

					ms->Write(g_BinFile, bmpLen);

					//��ͼ�����е�λͼ��ʾ��Image����
					ms->Position = 0;
					FormSet->Image1->Picture->Bitmap->LoadFromStream(ms);

					delete ms;
				}
			}
			g_tCmd.CmdStatus = CMDS_READ_SCREEN + 10;
			break;

		case CMDS_READ_SCREEN + 10:
			g_tCmd.CmdStatus = 0;
			break;
	}
}

//---------------------------------------------------------------------------
void __fastcall TFormTcp::UserCmd(void)
{
//	static int s_scan_status = 0;
//	static int s_retry = 0;
//	static int s_LastTime;
//	static int s_idx;

	if (g_tCmd.CmdStatus >= CMDS_DOWNLOAD_LUA && g_tCmd.CmdStatus < CMDS_DOWNLOAD_LUA + 99)
	{
		DownLuaFile();   	// ����luaС����
	}
	else if (g_tCmd.CmdStatus >= CMDS_PRO_UPGRADE && g_tCmd.CmdStatus <= CMDS_PRO_UPGRADE + 99)
	{
		UpdateFirmware();	// �����̼�
	}
	else if (g_tCmd.CmdStatus >= CMDS_UPDATE_CHIP_FILE && g_tCmd.CmdStatus <= CMDS_UPDATE_CHIP_FILE + 99)
	{
		UpdateChipFile();	// ����оƬ�����ļ�
	}
	else if (g_tCmd.CmdStatus >= CMDS_SWD_READ_FLASH && g_tCmd.CmdStatus <= CMDS_SWD_READ_FLASH + 99)
	{
		ReadChipToFile();	// ��оƬ���ݵ��ļ�
	}
	else if (g_tCmd.CmdStatus >= CMDS_READ_SCREEN && g_tCmd.CmdStatus <= CMDS_READ_SCREEN + 99)
	{
		ReadScreen();		// ����Ļ��ͼ
	}
	else
	{

	switch (g_tCmd.CmdStatus)
	{
		/*---- 03h -------*/
		case CMDS_SEND_03H:
			udp_Send03H(g_tCmd.RegAddr, g_tCmd.RegNum);
			g_tCmd.CmdStatus++;
			mLastTime = bsp_GetRunTime();
			break;

		case CMDS_SEND_03H + 1:
			if (g_tCmd.RxOk == 1)
			{
				g_tCmd.CmdStatus = 0;
			}
			if (bsp_CheckRunTime(mLastTime) > 1000)
			{
				g_tCmd.RxOk = 2;	/* ��ʱ */
				g_tCmd.CmdStatus = 0;
			}
			break;

		/*---- 06h -------*/
		case CMDS_SEND_06H:
			udp_Send06H(g_tCmd.RegAddr, g_tCmd.RegValue);
			g_tCmd.CmdStatus++;
			mLastTime = bsp_GetRunTime();
			break;

		case CMDS_SEND_06H + 1:
			if (g_tCmd.DisabelAck == 1)   /* ����ҪӦ��������PCģ�ⰴ�� */
			{
				g_tCmd.DisabelAck = 0;
				g_tCmd.CmdStatus = 0;
				break;
			}

			if (g_tCmd.RxOk == 1)
			{
				g_tCmd.CmdStatus = 0;
			}
			if (bsp_CheckRunTime(mLastTime) > 1000)
			{
				g_tCmd.CmdStatus = 0;
			}
			break;

		/*---- 10h -------*/
		case CMDS_SEND_10H:
			udp_Send10H(g_tCmd.RegAddr, g_tCmd.RegNum, g_tCmd.RegValueBuf);
			g_tCmd.CmdStatus++;
			mLastTime = bsp_GetRunTime();
			break;

		case CMDS_SEND_10H + 1:
			if (g_tCmd.RxOk == 1)
			{
				g_tCmd.CmdStatus = 0;

				if (mDispWriteRegister == 1)
				{
					mDispWriteRegister = 0;
					Form1->MemoLuaOut->Lines->Add("д�Ĵ����ɹ�");
				}
			}
			if (bsp_CheckRunTime(mLastTime) > 1000)
			{
				g_tCmd.CmdStatus = 0;
			}
			break;

		/*---- 0FH -------*/
		case CMDS_SEND_0FH:
			udp_Send0FH(g_tCmd.RegAddr, g_tCmd.RegNum, g_tCmd.RegValueBuf);
			g_tCmd.CmdStatus++;
			mLastTime = bsp_GetRunTime();
			break;

		case CMDS_SEND_0FH + 1:
			if (g_tCmd.RxOk == 1)
			{
				g_tCmd.CmdStatus = 0;
			}
			if (bsp_CheckRunTime(mLastTime) > 1000)
			{
				g_tCmd.CmdStatus = 0;
			}
			break;

		/*-------��̫���Զ�����----------*/
		case CMDS_UDP_FIND:
			udp_Send03H_MAC(MAC_FF, REG03_NET_CPU_ID0, 21);
			g_tCmd.CmdStatus++;
			mLastTime = bsp_GetRunTime();
			break;

		case CMDS_UDP_FIND + 1:
			if (bsp_CheckRunTime(mLastTime) > 100)
			{
				g_tCmd.CmdStatus = 0;
			}
			break;

		/*----ɨ�貨������-���ɰ汾��1��1��ģʽ��---------------------------------*/
		case CMDS_READ_WAVE:
			if (FormTcp->CanDrawWave2 == 0)
			{
				g_tCmd.CmdStatus++;
			}
			break;

		case CMDS_READ_WAVE + 1:
			if (g_tCmd.UserReq == 1)    /* ������Ӧ�û�ָ�� */
			{
				g_tCmd.CmdStatus = 0;
				break;
			}

			/* �Զ�����  ��ͨ����  ���δ���  */
//			if (Form1->ComboBoxTrigMode->Text == "�Զ�����")
			{
              	udp_Send06H(REG03_WAVE_LOCK, 1);
				g_tCmd.CmdStatus++;
			}
//			else      /* ��ͨ���� */
//			{
//				mCmdStatus =  CMDS_READ_WAVE + 3;
//				mWaveReg = REG03_CH1_WAVE_0;
//			}
			mLastTime = bsp_GetRunTime();
			break;

		/* ������1 */
		case CMDS_READ_WAVE + 2:
			if (g_tCmd.RxOk == 1)
			{
				Form1->YbCommDevice1->Tag = 0;
				g_tCmd.CmdStatus++;
				mWaveReg = REG03_CH1_WAVE_0;
			}
			if (bsp_CheckRunTime(mLastTime) > 1000)
			{
				Form1->YbCommDevice1->Tag++;   /* ���ڴ���ͳ�� */
				g_tCmd.CmdStatus = 0;
			}
			break;

		case CMDS_READ_WAVE + 3:
			if (g_tCmd.UserReq == 1)    /* ������Ӧ�û�ָ�� */
			{
				g_tCmd.CmdStatus = 0;
				break;
			}
			udp_Send03H(mWaveReg, DSO_PACKAGE_SIZE / 2);
			mLastTime = bsp_GetRunTime();
			g_tCmd.CmdStatus++;
			break;

		case CMDS_READ_WAVE + 4:
			if (g_tCmd.RxOk == 1)
			{
				mWaveReg += 1;
				if (mWaveReg < REG03_CH1_WAVE_0 + SAMPLE_BUFF_SIZE / (DSO_PACKAGE_SIZE / 4))
				{
					g_tCmd.CmdStatus--;
				}
				else
				{
					g_tCmd.CmdStatus++;
                }
			}
 			if (bsp_CheckRunTime(mLastTime) > 2000)
			{
				g_tCmd.CmdStatus = 0;
			}
			break;

		/* ������2 */
		case CMDS_READ_WAVE + 5:
			// DrawWave1();
			//CanDrawWave1 = 1;
			g_tCmd.CmdStatus++;
			mWaveReg =  REG03_CH2_WAVE_0;
			break;

		case CMDS_READ_WAVE + 6:
			if (g_tCmd.UserReq == 1)    /* ������Ӧ�û�ָ�� */
			{
				g_tCmd.CmdStatus = 0;
				break;
			}
			udp_Send03H(mWaveReg, DSO_PACKAGE_SIZE / 2);
			mLastTime = bsp_GetRunTime();
			g_tCmd.CmdStatus++;
			break;

		case CMDS_READ_WAVE + 7:
			if (g_tCmd.RxOk == 1)
			{
				mWaveReg += 1;
				if (mWaveReg < REG03_CH2_WAVE_0 + SAMPLE_BUFF_SIZE / (DSO_PACKAGE_SIZE / 4))
				{
					g_tCmd.CmdStatus--;
				}
				else
				{
					g_tCmd.CmdStatus++;
                }
			}
 			if (bsp_CheckRunTime(mLastTime) > 100)
			{
				g_tCmd.CmdStatus = 0;
			}
			break;

		case CMDS_READ_WAVE + 8:
			CanDrawWave2 = 1;
			g_tCmd.CmdStatus = CMDS_READ_AI;
			break;

		/*----ɨ�貨������-���°汾��1��1��ģʽ������Ч�����ã�ԭ��δ�飬��ʱ������---------------------------------*/
		case CMDS_AUTO_WAVE:
			if (FormTcp->CanDrawWave2 == 0)
			{
				g_tCmd.CmdStatus++;
			}
			break;

		case CMDS_AUTO_WAVE + 1:
			if (g_tCmd.UserReq == 1)    /* ������Ӧ�û�ָ�� */
			{
				g_tCmd.CmdStatus = 0;
				break;
			}

			udp_Send06H(REG03_WAVE_LOCK, 1);
			g_tCmd.CmdStatus++;
			mLastTime = bsp_GetRunTime();
			break;

		case CMDS_AUTO_WAVE + 2:
			if (g_tCmd.RxOk == 1)
			{
				g_tCmd.CmdStatus++;
			}
			if (bsp_CheckRunTime(mLastTime) > 300)
			{
				g_tCmd.CmdStatus = 0;
			}
			break;

		case CMDS_AUTO_WAVE + 3:
			if (g_tCmd.UserReq == 1)    /* ������Ӧ�û�ָ�� */
			{
				g_tCmd.CmdStatus = 0;
				break;
			}
			//udp_Send60H(uint32_t _ChEn, uint32_t _SampleSize, uint16_t _PackageLen,  uint32_t _Offset)
			{
				uint32_t _ChEn = 0;
				uint32_t _SampleSize;
				uint16_t _PackageLen = 256;
				uint32_t _Offset = 0;

				if (Form1->CheckBoxEnCH1->Checked)
				{
					_ChEn |= 0x01;
				}
				if (Form1->CheckBoxEnCH2->Checked)
				{
					_ChEn |= 0x02;
				}
				_SampleSize = TabelBufSize[Form1->ComboBoxSampleSize->ItemIndex];
				udp_Send60H(_ChEn, _SampleSize, _PackageLen, _Offset);

				H60_ChEn = _ChEn;
				H60_SampleSize = _SampleSize;
				H60_PackageLen = _PackageLen;
				H60_Offset = _Offset;
            }

			mLastTime = bsp_GetRunTime();
			g_tCmd.CmdStatus++;
			break;

		case CMDS_AUTO_WAVE + 4:
			if (g_tCmd.RxOk == 1)
			{
				g_tCmd.RxOk = 0;
				g_tCmd.CmdStatus++;
				mLastTime = bsp_GetRunTime();
			}
			if (bsp_CheckRunTime(mLastTime) > 100)
			{
				g_tCmd.CmdStatus = 0;
			}
			break;

		case CMDS_AUTO_WAVE + 5:	/* �ȴ�������� */
			if (H60_Ok == 1)
			{
				H60_Ok = 0;
				CanDrawWave2 = 1;
				g_tCmd.CmdStatus = CMDS_READ_AI;
			}
			if (bsp_CheckRunTime(mLastTime) > 300)
			{
				g_tCmd.CmdStatus = 0;
			}
			break;

		/*----��ȡģ������У׼���ֵ��---------------------------------*/
		case CMDS_READ_AI:
			if (g_tCmd.UserReq == 1)    /* ������Ӧ�û�ָ�� */
			{
				g_tCmd.CmdStatus = 0;
				break;
			}

			udp_Send03H(REG03_X15_00, 24);
			mLastTime = bsp_GetRunTime();
			g_tCmd.CmdStatus++;
			break;

		case CMDS_READ_AI + 1:
			if (g_tCmd.RxOk == 1)
			{
				g_tCmd.CmdStatus = CMDS_READ_ADC;
			}
 			if (bsp_CheckRunTime(mLastTime) > 100)
			{
				g_tCmd.CmdStatus = 0;
			}
			break;

		/*----��ȡADC---------------------------------*/
		case CMDS_READ_ADC:
			if (g_tCmd.UserReq == 1)    /* ������Ӧ�û�ָ�� */
			{
				g_tCmd.CmdStatus = 0;
				break;
			}

			udp_Send03H(REG03_ADC_CH1, 18);
			mLastTime = bsp_GetRunTime();
			g_tCmd.CmdStatus++;
			break;

		case CMDS_READ_ADC + 1:
			if (g_tCmd.RxOk == 1)
			{
				g_tCmd.CmdStatus = 0;
			}
			if (bsp_CheckRunTime(mLastTime) > 100)
			{
				g_tCmd.CmdStatus = 0;
			}
			break;



/*********************������������*******************/

		case 0:
		default:
			/* �û�ָ�������, ���������� */
			if (g_tCmd.CmdStatus == 0)
			{
				if (g_tCmd.UserReq == 1)
				{
					int i;

					g_tCmd.UserReq = 0;

					g_tCmd.CmdStatus = g_tCmd.UserCmdStatus;
					g_tCmd.RegAddr = g_tCmd.UserRegAddr;	/* �Ĵ�����ַ */
					g_tCmd.RegValue = g_tCmd.UserRegValue;	/* �Ĵ���ֵ */
					g_tCmd.RegNum = g_tCmd.UserRegNum;	/* �Ĵ������� */
					for (i = 0; i < g_tCmd.UserRegNum; i++)
					{
						g_tCmd.RegValueBuf[i] = g_tCmd.UserRegValueBuf[i];
					}
				}
				else
				{
					if (Form1->CheckBoxAutoReadWave > 0) {
						if (Form1->CheckBoxAutoReadWave->Checked)
						{
							g_tCmd.CmdStatus = CMDS_READ_WAVE;
						}
					}
				}
			}
			break;
	}
	}
}

//---------------------------------------------------------------------------
void __fastcall TFormTcp::Sacn(void)
{

}

/*
	������
(29 984ms) PC <-- Dev : 01 03 04 00 00 00 00 FA 33    01 03 A2 74 00 02 A7 A9   CRC ����

(30 125ms) PC <-- Dev : 01 03 04 00 00 00 00 FA 33    01 01 01 F4 00 01 BD C4   CRC ����

(30 250ms) PC <-- Dev : 01 01 01 01 90 48 01 01 01 F9 00 01 2C 07   CRC ����
�H?
(30 375ms) PC <-- Dev : 01 01 01 00 51 88 01 01 01 FE 00 01 9D C6   CRC ����
*/
// У������֡������1��ʾ����oK, 0 ��ʾ����
uint8_t M_crc16(uint8_t *_buf, uint16_t _num)
{
	uint16_t crc;

	if (CRC16_Modbus(_buf, _num) == 0)
	{
		return 1;
	}
	return 0;
}

// ������������֡�� ���ص�1����Ч֡���ȡ� 0 ��ʾ��Ч
int16_t CheckPackage(uint8_t *_buf, uint16_t _len)
{
	uint16_t len;
	uint16_t bytes;

	if (_buf[1] == 0x01)
	{
		/* 01 01 01 01 90 48 */
		len = _buf[2];
		if (_len >= 5 + len && M_crc16(_buf, 5 + len) == 1) 	/* 01 Ӧ�� */
		{
			return 5 + len;
		}

		/* 01 01 01 F9 00 01 2C 07  */
		if (_len >= 8 && M_crc16(_buf, 8) == 1) 	/* 01 ���� */
		{
			return 8;
		}

		if (_len >= 8)
		{
			return -1;
		}
	}

	else if (_buf[1] == 0x02)
	{
		/* FC 02 01 00 90 24 */
		len = _buf[2];
		if (_len >= 5 + len && M_crc16(_buf, 5 + len) == 1) 	/* 01 Ӧ�� */
		{
			return 5 + len;
		}

		/* FC 02 00 01 00 04 3D E4  */
		if (_len >= 8 && M_crc16(_buf, 8) == 1) 	/* 01 ���� */
		{
			return 8;
		}

		if (_len >= 8)
		{
			return -1;
		}
	}

	else if (_buf[1] == 0x03 || _buf[1] == 0x04)
	{
		/* 01 03 04 00 00 00 00 FA 33 */
		len = _buf[2];
		if (_len >= 5 + len  && M_crc16(_buf, 5 + len) == 1) 	/* 03 Ӧ�� */
		{
			return 5 + len;
		}

		/* 01 03 A2 74 00 02 A7 A9 */
		if (_len >= 8 && M_crc16(_buf, 8) == 1) 	/* 03 ���� */
		{
			return 8;
		}

		if (_len >= 8)
		{
			return -1;
		}
	}

	else if (_buf[1] == 0x05 || _buf[1] == 0x06)
	{
		/* FC 06 10 05 00 07 C9 24    */
		/* FC 05 00 01 FF 00 C8 17 */
		if (_len >= 8 && M_crc16(_buf, 8) == 1) 	/* 05 ���� or Ӧ�� */
		{
			return 8;
		}

		if (_len >= 8)
		{
			return -1;
		}
	}

	else if (_buf[1] == 0x10)
	{
		/* FC 10 10 15 00 08 10 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 96 13  */
		len =  BEBufToUint16(&_buf[4]);
		if (len > 0)
		{
			bytes = 2 * len + 1;
			if (_len >= bytes + 8 && M_crc16(_buf, bytes + 8) == 1) 	/* 0F ���� */
			{
				return bytes + 8;
			}
		}

		/* FC 10 10 15 00 08 C1 26   */
		if (_len >= 8 && M_crc16(_buf, 8) == 1) 	/* 0F Ӧ�� */
		{
			return 8;
		}

		if (_len >= 8)
		{
			return -1;
		}
	}

	else if (_buf[1] == 0x0F)
	{
		/* FC 0F 00 01 00 40 08 00 00 00 00 00 00 00 00 2B 26 */
		len =  BEBufToUint16(&_buf[4]);
		if (len > 0)
		{
			bytes = (len + 7) / 8;
			if (_len >= bytes + 8 && M_crc16(_buf, bytes + 8) == 1) 	/* 0F ���� */
			{
				return bytes + 8;
			}
		}

		/* FC 0F 00 01 00 40 10 16   */
		if (_len >= 8 && M_crc16(_buf, 8) == 1) 	/* 0F Ӧ�� */
		{
			return 8;
		}

		if (_len >= 8)
		{
			return -1;
		}
	}

	else if (_buf[1] == 0x60)
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
		*/
		if (_len >= 19 && M_crc16(_buf, 19) == 1) 	/* Ӧ���쳣 */
		{
			return 19;
		}

		/*
		�ӻ�����Ӧ��: 60H -

			01  ; �ӻ���ַ
			60  ; ������
			01  : 00��ʾPC�·���01��ʾ�豸Ӧ�� �����������˹�������
			01 00 00 00  : ͨ����ʹ�ܱ�־ 32bit��bit0��ʾCH1��bit1��ʾCH2
			00 00 04 00 : ÿ��ͨ����������
			01 00 : ÿͨ�Ű���������. ��λΪ1��������
			00 00 00 00 :
		*/
		if (_len >= 19 && M_crc16(_buf, 19) == 1) 	/* Ӧ���쳣 */
		{
			return 19;
		}

		if (_len >= 19)
		{
			return -1;
		}
	}

	else if (_buf[1] == 0x61)
	{
		/*
            H7-TOOL��������:
                01  ; �ӻ���ַ ��Ϊ1
                61  ; ������
                00  ; ͨ���ţ�00��ʾprint
                01  : ���ȸ��ֽ�
                08  : ���ȵ��ֽ�
                xx xx xx xx ... : ������
				CC CC : CRC16
		*/
		len = BEBufToUint16(&_buf[3]);
		if (_len >= len + 7 && M_crc16(_buf, len + 7) == 1)
		{
			return len + 7;
		}

		if (_len >= len + 7)
		{
			return -1;
		}
	}

	else if (_buf[1] == 0x64)
	{
		/*
		�ӻ�Ӧ��:
			01  ; �ӻ���ַ
			64  ; ������
			00  ; ִ�н����0��ʾOK  1��ʾ����
			CCCC : CRC16
		*/
		if (_len >= 5 && M_crc16(_buf, 5) == 1) 	/* Ӧ���쳣 */
		{
			return 5;
		}

		/*
			��������: С��������
			01  ; վ��
			64  ; ������
			0100 0000 ; �ܳ��� 4�ֽ�
			0000 0000 : ƫ�Ƶ�ַ 4�ֽ�
			0020 0000 : �������ݳ��� 4�ֽ�
			xx ... xx : �������ݣ�n��
			CCCC      : CRC16
		*/
		len = BEBufToUint32(&_buf[10]);
		if (_len >= len + 16)
		{
			if (M_crc16(_buf, len + 16) == 1)
			{
				return len + 16;
			}
			else
			{
				 return -1;
			}
		}

	}

	else if (_buf[1] == 0x65)
	{
		/*
		�ӻ�Ӧ��:
			01  ; �ӻ���ַ
			65  ; ������
			00  ; ִ�н����0��ʾOK  1��ʾ����
			CCCC : CRC16
		*/
		if (_len >= 5 && M_crc16(_buf, 5) == 1) 	/* Ӧ���쳣 */
		{
			return 5;
		}

		/*
		65H - ִ����ʱ��LUA��������֡������
			01  ; �ӻ���ַ
			65  ; ������
			0100 : �������ݳ��� 2�ֽ�
			xxxx : �ű����ݣ�0����
			CCCC : CRC16
		*/
		len = BEBufToUint16(&_buf[2]);
		if (_len >= len + 6)
		{
			if (M_crc16(_buf, len + 6) == 1)
			{
				return len + 6;
			}
			else
			{
				 return -1;
			}
		}
	}
	else if (_buf[1] == 0x66)
	{
		/*
			��������: С��������
				01  ; վ��
				66  ; ������
				0000  ; �ӹ���,
					- 0��ʾ���ڴ棨��Ҫ��ʼ��pg_init())
					- 1��ʾ���ڴ� (�������������ʼ����
					- 2 д

					- 0x0100   ����ʾ�������������ڽ���
				0000 0000 : ƫ�Ƶ�ַ 4�ֽ�
				0020 0000 : ���ݳ��� 4�ֽ�
				CCCC      : CRC16

			�ӻ�Ӧ��:
				01  ; �ӻ���ַ
				66  ; ������
				0000  ; �ӹ���
				0000 0000 : ƫ�Ƶ�ַ 4�ֽ�
				0020 0000 : ���ݳ��� 4�ֽ�

				00  ; ִ�н����0��ʾOK  1��ʾ����

				XX XX ... XX : ����

				CCCC : CRC16
		*/
		if (_len >= 5 && M_crc16(_buf, 5) == 1) 	/* Ӧ���쳣 */
		{
			return 5;
		}

		len = BEBufToUint32(&_buf[8]);
		if (_len >= len + 15)
		{
			if (M_crc16(_buf, len + 15) == 1)
			{
				return len + 15;
			}
			else
			{
				return -1;
			}
		}
	}


//	if (_buf[1] == 0x81 || _buf[1] == 0x82 || _buf[1] == 0x83
//		|| _buf[1] == 0x84 || _buf[1] == 0x85 || _buf[1] == 0x86
//		|| _buf[1] == 0x90 || _buf[1] == 0x8F || _buf[1] == (0x80 + 0x61))
	if (_buf[1] & 0x80)
	{
		/* FC 83 10 C1 26  */
		if (_len >= 5 && M_crc16(_buf, 5) == 1) 	/* Ӧ���쳣 */
		{
			return 5;
		}

		if (_len >= 5)
		{
			 return -1;
		}
	}
	return 0;
}

void AnalyzePackage(uint8_t *_rxbuf, uint16_t _rxlen)
{
	switch (_rxbuf[1])
	{
		case 0x03:		// 03Hָ���Ӧ��
			FormTcp->MODH_03H(_rxbuf, _rxlen);
			g_tCmd.RxOk = 1;
			break;

		case 0x06:		// 06Hָ���Ӧ��
			FormTcp->MODH_06H(_rxbuf, _rxlen);
			g_tCmd.RxOk = 1;
			break;

		case 0x10:		// 10Hָ���Ӧ��
			FormTcp->MODH_10H(_rxbuf, _rxlen);
			g_tCmd.RxOk = 1;
			break;

		case 0x0F:		// 0FHָ���Ӧ��
			g_tCmd.RxOk = 1;
			break;

		case 0x15:		// 15Hָ���Ӧ��
			g_tCmd.RxOk = 1;
			break;

		case 0x60:
			g_tCmd.RxOk = 1;
			break;

		case 0x61:  	// Modbus print
			FormTcp->MODH_61H(_rxbuf, _rxlen);
			break;

		case 0x64:
			g_tCmd.err = _rxbuf[4];

			#if 0
			{
				AnsiString s;
				uint32_t total, off, len;

				total = BEBufToUint32(_rxbuf + 5);
				off = BEBufToUint32(_rxbuf + 5 + 4);
				len = BEBufToUint32(_rxbuf + 5 + 8);
				s.sprintf("  rx total = %X, off = %d(%X), len = %d\r\n",
					total, off, off, len);
				Form1->MemoLuaOut->Lines->Add(s);
			}
			#endif
			g_tCmd.RxOk = 1;
			break;

		case 0x65:
			g_tCmd.RxOk = 1;
			break;

		case 0x66:
			FormTcp->MODH_66H(_rxbuf, _rxlen);
			/*
				��������: С��������
					01  ; վ��
					66  ; ������
					0000  ; �ӹ���,
						- 0��ʾ���ڴ�
						- 1��ʾд�ڴ�
					0000 0000 : ƫ�Ƶ�ַ 4�ֽ�
					0020 0000 : ���ݳ��� 4�ֽ�
					CCCC      : CRC16

				�ӻ�Ӧ��:
					01  ; �ӻ���ַ
					66  ; ������
					0000  ; �ӹ���
					0000 0000 : ƫ�Ƶ�ַ 4�ֽ�
					0020 0000 : ���ݳ��� 4�ֽ�

					00  ; ִ�н����0��ʾOK  1��ʾ����

					CCCC : CRC16
			*/
			g_tCmd.RxOk = 1;
			g_tCmd.err = _rxbuf[12];
			break;

		case 0x83:    /* �쳣 */
		case 0x86:
		case 0x90:
			{
				AnsiString s;

				s.sprintf("ִ���쳣 0x%X", _rxbuf[1]);
				FormCalib->Memo1->Lines->Add(s);
			}
			break;

		 default:
			break;
	}
}

// ������յ�����
void __fastcall TFormTcp::DealRxData(void)
{
	if (s_RxLen == 0)
	{
		return;
	}

	try
	{
		// ����MAC��֡
		if (CRC16_Modbus(s_RxBuf, s_RxLen) == 0)
		{
			if (s_RxLen == 1029) {
				 s_RxLen++;
				 s_RxLen--;
			}
			AnalyzePackage(s_RxBuf, s_RxLen);


			#if 0
			{
				char buf[128];

				sprintf(buf,  "s_RxLenOk = %02X %02X  %d",
					s_RxBuf[0], s_RxBuf[1],	s_RxLen);

				Form1->MemoLuaOut->Lines->Add((AnsiString)buf);
			}
			#endif

			s_RxLen = 0;

			return;
		}
		else
		{
			#if 0
			{
				char buf[128];

				sprintf(buf,  "s_RxLenErr = %02X %02X  %d",
					s_RxBuf[0], s_RxBuf[1],	s_RxLen);

				Form1->MemoLuaOut->Lines->Add((AnsiString)buf);
			}
			#endif
		}

		{
			{
				int ok_len;
				uint16_t ok_pos = 0;

				/* ֡ͬ�� */
				if (s_RxBuf[0] != 1 && s_RxBuf[0] != 0xC8)
				{
					s_RxLen = 0;
					Form1->MemoLuaOut->Lines->Add("�յ������ݰ�֡��ͬ��");
					return;
				}

				for (;;)
				{
					if (s_RxBuf[0] == 0xC8)   // ��MAC��֡
					{
						ok_len = CheckPackage(&s_RxBuf[6], s_RxLen);
						if (ok_len > 0)
						{
							int i;
							AnalyzePackage(&s_RxBuf[6], ok_len);   	/* �������� */
							/* ����ǰ�� */
							for (i = 0; i < s_RxLen - ok_len + 6; i++)
							{
								s_RxBuf[i] = s_RxBuf[i + ok_len + 6];
							}
							s_RxLen = s_RxLen - ok_len - 6;
						}
						else  if (ok_len == -1)
						{
							s_RxLen = 0;
							break;
						}
						else
						{
							break;
						}
					}
					else if (s_RxBuf[0] == 0x01)  // ����ͨ��֡
					{
						ok_len = CheckPackage(s_RxBuf, s_RxLen);
						if (ok_len > 0)
						{
							int i;
							AnalyzePackage(s_RxBuf, ok_len);   	/* �������� */

							#if 0
							{
								char buf[128];

								sprintf(buf,  " s_RxLenPP = %02X %02X  %d",
									s_RxBuf[0], s_RxBuf[1],	ok_len);

								Form1->MemoLuaOut->Lines->Add((AnsiString)buf);
							}
							#endif

							/* ����ǰ�� */
							for (i = 0; i < s_RxLen - ok_len; i++)
							{
								s_RxBuf[i] = s_RxBuf[i + ok_len];
							}
							s_RxLen -= ok_len;
						}
						else  if (ok_len == -1)
						{
							s_RxLen = 0;
							break;
						}
						else
						{
							break;
						}
					}
					else
					{
						s_RxLen = 0;
						return;
					}
				}
			}
		}
	}
	catch (Exception &e)
	{
		Form1->MemoLuaOut->Lines->Add("���������쳣");
	}
}

// ����USB���ڽ��յ�����  lua������
void __fastcall TFormTcp::DispUartData(void)
{
	int i;
	char *p;

	if (s_USB_RxLen == 0)
	{
		return;
	}
	s_USB_RxBuf[s_USB_RxLen] = 0;
	p = s_USB_RxBuf;
	for (i = 0; i < s_USB_RxLen; i++)
	{
		if (s_USB_RxBuf[i] == 0x0A)
		{
			s_USB_RxBuf[i] = 0;
			Form1->MemoLuaOut->Lines->Add((AnsiString)p);
			p = &s_USB_RxBuf[i + 1];
		}
	}
	s_USB_RxLen = 0;
}

// ͨ���߳���ѭ��
void __fastcall TFormTcp::CommTaskLoop(void)
{
	;
}

//---------------------------------------------------------------------------

void __fastcall TFormTcp::SendTcpCmd(uint32_t ip, uint32_t tcp_port)
{

}

void __fastcall TFormTcp::SendUdpCmd(uint32_t ip, uint32_t port, uint8_t * cmd_buf, uint16_t cmd_len)
{
	uint8_t *p;
	AnsiString ip_str;

	// ��32λ��IP��ַת��Ϊ�ַ�����ʽ
	ip_str =  IntToStr((uint8_t)(ip >> 24)) + '.'
		+ IntToStr((uint8_t)(ip >> 16)) + '.' + IntToStr((uint8_t)(ip >> 8)) +
		'.' + IntToStr((uint8_t)ip);
#if 0
	LabelInfoBar->Caption = "UDPɨ�� " + ip_str;
#endif
	// ���͵�UDP�����
	{
		TByteDynArray buf;

		buf = RawToBytes(cmd_buf, cmd_len);
		FormTcp->UDP1->SendBuffer(ip_str, port, buf);
	}
}

void __fastcall TFormTcp::SendUdpCmd2(AnsiString str_ip, uint32_t port, uint8_t * cmd_buf, uint16_t cmd_len)
{
	// ���͵�UDP�����
	{
		TByteDynArray buf;

		buf = RawToBytes(cmd_buf, cmd_len);
		FormTcp->UDP1->SendBuffer(str_ip, port, buf);
	}
}

void __fastcall TFormTcp::MODH_03H(uint8_t *_rx_buf, uint16_t _len)
{
	AnsiString s1,s2;
	AnsiString ip;
	int pos = 0;
	char *p;
	float f1;

	switch (g_tCmd.RegAddr)
	{
		case REG03_NET_CPU_ID0:        // UDP���� ,��MAC��ַ
			{
				if (_len != 21 * 2 + 5)
				{
					break;
				}
				/* ǰ6�ֽ���mac��ַ�����˵��ظ��� */
				#if 0
				{
					uint8_t mac[6];
					int i;

					p = &_rx_buf[0] - 6;
					memcpy(mac, p, 6);

					for (i = 0; i < mDevNum; i++)
					{
						if (memcmp(mac,  mDevList[i].Mac, 6) == 0)
						{
							return;
						}
					}
				}
				#endif

				pos = 0;
				p = &_rx_buf[3];

				memcpy(mDevList[mDevNum].CpuID,  p, 12);  p += 12;
				mDevList[mDevNum].HardVer = BEBufToUint16(p); p += 2;
				mDevList[mDevNum].SoftVer = BEBufToUint16(p); p += 2;
				memcpy(mDevList[mDevNum].Mac,  p, 6); p += 6;
				memcpy(mDevList[mDevNum].LocalIP,  p, 4); p += 4;
				memcpy(mDevList[mDevNum].NetMask,  p, 4); p += 4;
				memcpy(mDevList[mDevNum].GateWay,  p, 4); p += 4;
				mDevList[mDevNum].LocalPort = BEBufToUint16(p); p += 2;
				memcpy(mDevList[mDevNum].SysIP,  p, 4); p += 4;
				mDevList[mDevNum].SysPort = BEBufToUint16(p);

				s1.sprintf("MAC = %02X-%02X-%02X-%02X-%02X-%02X", mDevList[mDevNum].Mac[0], mDevList[mDevNum].Mac[1], mDevList[mDevNum].Mac[2],
						mDevList[mDevNum].Mac[3], mDevList[mDevNum].Mac[4], mDevList[mDevNum].Mac[5]);
				s2 = s1;

				s1.sprintf("IP = %d.%d.%d.%d", mDevList[mDevNum].LocalIP[0], mDevList[mDevNum].LocalIP[1], mDevList[mDevNum].LocalIP[2],
						mDevList[mDevNum].LocalIP[3]);
				s2 += "," + s1;

				s1.sprintf("HardVer = %04X", mDevList[mDevNum].HardVer);
				s2 += "," + s1;

				s1.sprintf("SoftVer = %04X", mDevList[mDevNum].SoftVer);
				s2 += "," + s1;

				mDevNum++;

				if (mDevNum > MAX_NUM)
				{
					mDevNum = MAX_NUM;
				}

				{
					uint16_t addr, year, month, day, hour, min, sec, msec;
					TDateTime tt;
					AnsiString s1;

					tt = Now();
					tt.DecodeTime(&hour, &min, &sec, &msec);
					tt.DecodeDate(&year, &month, &day);

					s1.sprintf("%02d:%02d:%02d, ", hour, min, sec);
					FormSet->ListBox1->Items->Add(s1 + s2);


					//FormSet->LabelResult->Caption = "��ɨ�赽: " + IntToStr(mDevNum);

					Form1->MemoLuaOut->Lines->Add(s1 + s2);
					//Form1->MemoLuaOut->Lines->Add(FormSet->LabelResult->Caption);
				}

				if (mDevNum > 0)
				{
					FormSet->ListBox1->ItemIndex = 0;
					FormSet->ListBox1Click(this);
				}
			}
			break;

		case REG03_X15_00:	//REG03_AI_CH1:
			if (_rx_buf[2] != ((24 * 2) % 256))
			{
				break;
			}
//	float CH1Volt;
//	float CH2Volt;
//	float HighSideVolt;
//	float HighSideCurr;
//	float TVCCVolt;		/* TVCCʵ���ѹ */
//	float TVCCCurr;		/* TVCCʵ���ѹ */
//	float NTCRes;		/* NTC���� */
//	float NTCTemp;		/* NTC�¶� */
//	float USBPowerVolt;
//	float ExtPowerVolt;
			p = &_rx_buf[3];
			g_tAI.X15_00 = BEBufToUint16(p); p += 2;
			g_tAI.X31_16 = BEBufToUint16(p); p += 2;
			g_tAI.Y15_00 = BEBufToUint16(p); p += 2;
			g_tAI.Y31_16 = BEBufToUint16(p); p += 2;

			if (g_tAI.X15_00 & 0x200) Form1->ShapeX9->Brush->Color = clRed; else Form1->ShapeX9->Brush->Color = clGray;
			if (g_tAI.X15_00 & 0x100) Form1->ShapeX8->Brush->Color = clRed; else Form1->ShapeX8->Brush->Color = clGray;
			if (g_tAI.X15_00 & 0x80) Form1->ShapeX7->Brush->Color = clRed; else Form1->ShapeX7->Brush->Color = clGray;
			if (g_tAI.X15_00 & 0x40) Form1->ShapeX6->Brush->Color = clRed; else Form1->ShapeX6->Brush->Color = clGray;
			if (g_tAI.X15_00 & 0x20) Form1->ShapeX5->Brush->Color = clRed; else Form1->ShapeX5->Brush->Color = clGray;
			if (g_tAI.X15_00 & 0x10) Form1->ShapeX4->Brush->Color = clRed; else Form1->ShapeX4->Brush->Color = clGray;
			if (g_tAI.X15_00 & 0x08) Form1->ShapeX3->Brush->Color = clRed; else Form1->ShapeX3->Brush->Color = clGray;
			if (g_tAI.X15_00 & 0x04) Form1->ShapeX2->Brush->Color = clRed; else Form1->ShapeX2->Brush->Color = clGray;
			if (g_tAI.X15_00 & 0x02) Form1->ShapeX1->Brush->Color = clRed; else Form1->ShapeX1->Brush->Color = clGray;
			if (g_tAI.X15_00 & 0x01) Form1->ShapeX0->Brush->Color = clRed; else Form1->ShapeX0->Brush->Color = clGray;

			if (g_tAI.Y15_00 & 0x200) Form1->ShapeY9->Brush->Color = clRed; else Form1->ShapeY9->Brush->Color = clGray;
			if (g_tAI.Y15_00 & 0x100) Form1->ShapeY8->Brush->Color = clRed; else Form1->ShapeY8->Brush->Color = clGray;
			if (g_tAI.Y15_00 & 0x80) Form1->ShapeY7->Brush->Color = clRed; else Form1->ShapeY7->Brush->Color = clGray;
			if (g_tAI.Y15_00 & 0x40) Form1->ShapeY6->Brush->Color = clRed; else Form1->ShapeY6->Brush->Color = clGray;
			if (g_tAI.Y15_00 & 0x20) Form1->ShapeY5->Brush->Color = clRed; else Form1->ShapeY5->Brush->Color = clGray;
			if (g_tAI.Y15_00 & 0x10) Form1->ShapeY4->Brush->Color = clRed; else Form1->ShapeY4->Brush->Color = clGray;
			if (g_tAI.Y15_00 & 0x08) Form1->ShapeY3->Brush->Color = clRed; else Form1->ShapeY3->Brush->Color = clGray;
			if (g_tAI.Y15_00 & 0x04) Form1->ShapeY2->Brush->Color = clRed; else Form1->ShapeY2->Brush->Color = clGray;
			if (g_tAI.Y15_00 & 0x02) Form1->ShapeY1->Brush->Color = clRed; else Form1->ShapeY1->Brush->Color = clGray;
			if (g_tAI.Y15_00 & 0x01) Form1->ShapeY0->Brush->Color = clRed; else Form1->ShapeY0->Brush->Color = clGray;

			f1 = BEBufToFloat(p); s1.sprintf("%8.3fV", f1); Form1->LabelCH1_Volt->Caption = s1;  g_tAI.CH1Volt = f1; p += 4;
			f1 = BEBufToFloat(p); s1.sprintf("%8.3fV", f1); Form1->LabelCH2_Volt->Caption = s1;  g_tAI.CH2Volt = f1; p += 4;
			f1 = BEBufToFloat(p); s1.sprintf("%8.2fV", f1); Form1->LabelLoad_Volt->Caption = s1;  g_tAI.HighSideVolt = f1; p += 4;
			f1 = BEBufToFloat(p); s1.sprintf("%8.2fmA", f1); Form1->LabelLoad_Curr->Caption = s1;  g_tAI.HighSideCurr = f1; p += 4;
			f1 = BEBufToFloat(p); s1.sprintf("%8.3fV", f1); Form1->LabelTVCC_Volt->Caption = s1;  g_tAI.TVCCVolt = f1; p += 4;
			f1 = BEBufToFloat(p); s1.sprintf("%8.3fmA", f1); Form1->LabelTVCC_Curr->Caption = s1;  g_tAI.TVCCCurr = f1; p += 4;
			f1 = BEBufToFloat(p); s1.sprintf("%8.3fK��", f1); Form1->LabelNTC_Res->Caption = s1;  g_tAI.NTCRes = f1; p += 4;
			f1 = BEBufToFloat(p); s1.sprintf("%8.2f��", f1); Form1->LabelNTC_Temp->Caption = s1;  g_tAI.NTCTemp = f1; p += 4;
			f1 = BEBufToFloat(p); s1.sprintf("%8.2fV", f1);  g_tAI.USBPowerVolt = f1; p += 4;
			f1 = BEBufToFloat(p); s1.sprintf("%8.2fV", f1);  g_tAI.ExtPowerVolt = f1; p += 4;
			break;

		case REG03_ADC_CH1:
			if (_rx_buf[2] != ((18 * 2) % 256))
			{
				break;
			}
			p = &_rx_buf[3];
			f1 = BEBufToFloat(p); s1.sprintf("%8.1f", f1); Form1->LabelCH1_Adc->Caption = s1;  g_tAI.ADC_CH1Volt = f1; p += 4;
			f1 = BEBufToFloat(p); s1.sprintf("%8.1f", f1); Form1->LabelCH2_Adc->Caption = s1;  g_tAI.ADC_CH2Volt = f1; p += 4;
			f1 = BEBufToFloat(p); s1.sprintf("%8.1f", f1); Form1->LabelLoad_Volt_Adc->Caption = s1;  g_tAI.ADC_HighSideVolt = f1; p += 4;
			f1 = BEBufToFloat(p); s1.sprintf("%8.1f", f1); Form1->LabelLoad_Curr_Adc->Caption = s1;  g_tAI.ADC_HighSideCurr = f1; p += 4;
			f1 = BEBufToFloat(p); s1.sprintf("%8.1f", f1); Form1->LabelTVCC_Volt_Adc->Caption = s1;  g_tAI.ADC_TVCCVolt = f1; p += 4;
			f1 = BEBufToFloat(p); s1.sprintf("%8.1f", f1); Form1->LabelTVCC_Curr_Adc->Caption = s1;  g_tAI.ADC_TVCCCurr = f1; p += 4;
			f1 = BEBufToFloat(p); s1.sprintf("%8.1f", f1); Form1->LabelNTC_Adc->Caption = s1;  g_tAI.ADC_NTCRes = f1; p += 4;
			f1 = BEBufToFloat(p); s1.sprintf("%8.1f", f1);  g_tAI.ADC_USBPowerVolt = f1; p += 4;
			f1 = BEBufToFloat(p); s1.sprintf("%8.1f", f1);  g_tAI.ADC_ExtPowerVolt = f1; p += 4;
			break;

		case REG03_CALIB_CH1_R1_X1:		/* У׼���� 232�� */
			/* ����У׼���� */
			if (_rx_buf[2] != ((232 * 2) % 256))
			{
				break;
			}
			{
				p = &_rx_buf[3];
				int i;
				for (i = 0; i < 8; i++)
				{
					g_tCalib.CH1[i].x1 = BEBufToFloat(p); p += 4;
					g_tCalib.CH1[i].y1 = BEBufToFloat(p); p += 4;
					g_tCalib.CH1[i].x2 = BEBufToFloat(p); p += 4;
					g_tCalib.CH1[i].y2 = BEBufToFloat(p); p += 4;
				}
				for (i = 0; i < 8; i++)
				{
					g_tCalib.CH2[i].x1 = BEBufToFloat(p); p += 4;
					g_tCalib.CH2[i].y1 = BEBufToFloat(p); p += 4;
					g_tCalib.CH2[i].x2 = BEBufToFloat(p); p += 4;
					g_tCalib.CH2[i].y2 = BEBufToFloat(p); p += 4;
				}
				g_tCalib.LoadVolt.x1 = BEBufToFloat(p); p += 4;
				g_tCalib.LoadVolt.y1 = BEBufToFloat(p); p += 4;
				g_tCalib.LoadVolt.x2 = BEBufToFloat(p); p += 4;
				g_tCalib.LoadVolt.y2 = BEBufToFloat(p); p += 4;

				g_tCalib.LoadCurr[0].x1 = BEBufToFloat(p); p += 4;
				g_tCalib.LoadCurr[0].y1 = BEBufToFloat(p); p += 4;
				g_tCalib.LoadCurr[0].x2 = BEBufToFloat(p); p += 4;
				g_tCalib.LoadCurr[0].y2 = BEBufToFloat(p); p += 4;
				g_tCalib.LoadCurr[0].x3 = BEBufToFloat(p); p += 4;
				g_tCalib.LoadCurr[0].y3 = BEBufToFloat(p); p += 4;
				g_tCalib.LoadCurr[0].x4 = BEBufToFloat(p); p += 4;
				g_tCalib.LoadCurr[0].y4 = BEBufToFloat(p); p += 4;

				g_tCalib.LoadCurr[1].x1 = BEBufToFloat(p); p += 4;
				g_tCalib.LoadCurr[1].y1 = BEBufToFloat(p); p += 4;
				g_tCalib.LoadCurr[1].x2 = BEBufToFloat(p); p += 4;
				g_tCalib.LoadCurr[1].y2 = BEBufToFloat(p); p += 4;
				g_tCalib.LoadCurr[1].x3 = BEBufToFloat(p); p += 4;
				g_tCalib.LoadCurr[1].y3 = BEBufToFloat(p); p += 4;
				g_tCalib.LoadCurr[1].x4 = BEBufToFloat(p); p += 4;
				g_tCalib.LoadCurr[1].y4 = BEBufToFloat(p); p += 4;

				g_tCalib.TVCCVolt.x1 = BEBufToFloat(p); p += 4;
				g_tCalib.TVCCVolt.y1 = BEBufToFloat(p); p += 4;
				g_tCalib.TVCCVolt.x2 = BEBufToFloat(p); p += 4;
				g_tCalib.TVCCVolt.y2 = BEBufToFloat(p); p += 4;

				g_tCalib.TVCCCurr.x1 = BEBufToFloat(p); p += 4;
				g_tCalib.TVCCCurr.y1 = BEBufToFloat(p); p += 4;
				g_tCalib.TVCCCurr.x2 = BEBufToFloat(p); p += 4;
				g_tCalib.TVCCCurr.y2 = BEBufToFloat(p); p += 4;
				g_tCalib.TVCCCurr.x3 = BEBufToFloat(p); p += 4;
				g_tCalib.TVCCCurr.y3 = BEBufToFloat(p); p += 4;
				g_tCalib.TVCCCurr.x4 = BEBufToFloat(p); p += 4;
				g_tCalib.TVCCCurr.y4 = BEBufToFloat(p); p += 4;

				g_tCalib.TVCCSet.x1 = BEBufToFloat(p); p += 4;
				g_tCalib.TVCCSet.y1 = BEBufToFloat(p); p += 4;
				g_tCalib.TVCCSet.x2 = BEBufToFloat(p); p += 4;
				g_tCalib.TVCCSet.y2 = BEBufToFloat(p); p += 4;

				g_tCalib.Dac10V.x1 = BEBufToUint16(p); p += 2;
				g_tCalib.Dac10V.y1 = (int16_t)BEBufToUint16(p); p += 2;
				g_tCalib.Dac10V.x2 = BEBufToUint16(p); p += 2;
				g_tCalib.Dac10V.y2 = (int16_t)BEBufToUint16(p); p += 2;
				g_tCalib.Dac10V.x3 = BEBufToUint16(p); p += 2;
				g_tCalib.Dac10V.y3 = (int16_t)BEBufToUint16(p); p += 2;
				g_tCalib.Dac10V.x4 = BEBufToUint16(p); p += 2;
				g_tCalib.Dac10V.y4 = (int16_t)BEBufToUint16(p); p += 2;

				g_tCalib.Dac20mA.x1 = BEBufToUint16(p); p += 2;
				g_tCalib.Dac20mA.y1 = (int16_t)BEBufToUint16(p); p += 2;
				g_tCalib.Dac20mA.x2 = BEBufToUint16(p); p += 2;
				g_tCalib.Dac20mA.y2 = (int16_t)BEBufToUint16(p); p += 2;
				g_tCalib.Dac20mA.x3 = BEBufToUint16(p); p += 2;
				g_tCalib.Dac20mA.y3 = (int16_t)BEBufToUint16(p); p += 2;
				g_tCalib.Dac20mA.x4 = BEBufToUint16(p); p += 2;
				g_tCalib.Dac20mA.y4 = (int16_t)BEBufToUint16(p); p += 2;

				g_tCalib.NtcRes.x1 = BEBufToFloat(p); p += 4;
				g_tCalib.NtcRes.y1 = BEBufToFloat(p); p += 4;
				g_tCalib.NtcRes.x2 = BEBufToFloat(p); p += 4;
				g_tCalib.NtcRes.y2 = BEBufToFloat(p); p += 4;
				g_tCalib.NtcRes.x3 = BEBufToFloat(p); p += 4;
				g_tCalib.NtcRes.y3 = BEBufToFloat(p); p += 4;
				g_tCalib.NtcRes.x4 = BEBufToFloat(p); p += 4;
				g_tCalib.NtcRes.y4 = BEBufToFloat(p); p += 4;

				FormCalib->FileCalibToEdit();
			}

			{
				FormCalib->Memo1->Lines->Add("-----------------------------------");
				FormCalib->Memo1->Lines->Add("��У׼�����ɹ�");
				int i;
				FormCalib->Memo1->Lines->Add("                x1(adc)      y1(ģ����)   x2(adc)      y2(ģ����)");
				for (i = 0; i < 8; i++)
				{
					s1.sprintf("CH1_R%d    = %12.3f,%12.3f,%12.3f,%12.3f", i + 1, g_tCalib.CH1[i].x1, g_tCalib.CH1[i].y1,
							g_tCalib.CH1[i].x2, g_tCalib.CH1[i].y2);
					FormCalib->Memo1->Lines->Add(s1);
				}
				FormCalib->Memo1->Lines->Add("");
				for (i = 0; i < 8; i++)
				{
					s1.sprintf("CH2_R%d    = %12.3f,%12.3f,%12.3f,%12.3f", i + 1, g_tCalib.CH2[i].x1, g_tCalib.CH2[i].y1,
							g_tCalib.CH2[i].x2, g_tCalib.CH2[i].y2);
					FormCalib->Memo1->Lines->Add(s1);
				}

				FormCalib->Memo1->Lines->Add("");

				s1.sprintf("LoadVolt  = %12.3f,%12.3f,%12.3f,%12.3f",  g_tCalib.LoadVolt.x1, g_tCalib.LoadVolt.y1,
							g_tCalib.LoadVolt.x2, g_tCalib.LoadVolt.y2);
				FormCalib->Memo1->Lines->Add(s1);

				s1.sprintf("LoadCurr1 = %12.3f,%12.3f,%12.3f,%12.3f", g_tCalib.LoadCurr[0].x1, g_tCalib.LoadCurr[0].y1,
							g_tCalib.LoadCurr[0].x2, g_tCalib.LoadCurr[0].y2);
				FormCalib->Memo1->Lines->Add(s1);
				s1.sprintf("            %12.3f,%12.3f,%12.3f,%12.3f", g_tCalib.LoadCurr[0].x3, g_tCalib.LoadCurr[0].y3,
							g_tCalib.LoadCurr[0].x4, g_tCalib.LoadCurr[0].y4);
				FormCalib->Memo1->Lines->Add(s1);

				s1.sprintf("LoadCurr2 = %12.3f,%12.3f,%12.3f,%12.3f", g_tCalib.LoadCurr[1].x1, g_tCalib.LoadCurr[1].y1,
							g_tCalib.LoadCurr[1].x2, g_tCalib.LoadCurr[1].y2);
				FormCalib->Memo1->Lines->Add(s1);
				s1.sprintf("            %12.3f,%12.3f,%12.3f,%12.3f", g_tCalib.LoadCurr[1].x3, g_tCalib.LoadCurr[1].y3,
							g_tCalib.LoadCurr[1].x4, g_tCalib.LoadCurr[1].y4);
				FormCalib->Memo1->Lines->Add(s1);

				s1.sprintf("TVCCVolt  = %12.3f,%12.3f,%12.3f,%12.3f", g_tCalib.TVCCVolt.x1, g_tCalib.TVCCVolt.y1,
							g_tCalib.TVCCVolt.x2, g_tCalib.TVCCVolt.y2);
				FormCalib->Memo1->Lines->Add(s1);

				s1.sprintf("TVCCCurr  = %12.3f,%12.3f,%12.3f,%12.3f", g_tCalib.TVCCCurr.x1, g_tCalib.TVCCCurr.y1,
							g_tCalib.TVCCCurr.x2, g_tCalib.TVCCCurr.y2);
				FormCalib->Memo1->Lines->Add(s1);
				s1.sprintf("          = %12.3f,%12.3f,%12.3f,%12.3f", g_tCalib.TVCCCurr.x3, g_tCalib.TVCCCurr.y3,
							g_tCalib.TVCCCurr.x4, g_tCalib.TVCCCurr.y4);
				FormCalib->Memo1->Lines->Add(s1);

				FormCalib->Memo1->Lines->Add("");
				s1.sprintf("TVCCSet   = %12.3f,%12.3f,%12.3f,%12.3f", g_tCalib.TVCCSet.x1, g_tCalib.TVCCSet.y1,
							g_tCalib.TVCCSet.x2, g_tCalib.TVCCSet.y2);
				FormCalib->Memo1->Lines->Add(s1);

				FormCalib->Memo1->Lines->Add("");
				s1.sprintf("Dac10V  = %6d,%6d,%6d,%6d,",
							g_tCalib.Dac10V.x1, g_tCalib.Dac10V.y1,
							g_tCalib.Dac10V.x2, g_tCalib.Dac10V.y2
							);
				s2.sprintf("%6d,%6d,%6d,%6d",
							g_tCalib.Dac10V.x3, g_tCalib.Dac10V.y3,
							g_tCalib.Dac10V.x4, g_tCalib.Dac10V.y4
							);
				FormCalib->Memo1->Lines->Add(s1 + s2);

				s1.sprintf("Dac20mA = %6d,%6d,%6d,%6d,",
							g_tCalib.Dac20mA.x1, g_tCalib.Dac20mA.y1,
							g_tCalib.Dac20mA.x2, g_tCalib.Dac20mA.y2
							);
				s2.sprintf("%6d,%6d,%6d,%6d",
							g_tCalib.Dac20mA.x3, g_tCalib.Dac20mA.y3,
							g_tCalib.Dac20mA.x4, g_tCalib.Dac20mA.y4
							);
				FormCalib->Memo1->Lines->Add(s1 + s2);
				FormCalib->Memo1->Lines->Add("");

				s1.sprintf("NtcRes    = %12.3f,%12.3f,%12.3f,%12.3f", g_tCalib.NtcRes.x1, g_tCalib.NtcRes.y1,
							g_tCalib.NtcRes.x2, g_tCalib.NtcRes.y2);
				FormCalib->Memo1->Lines->Add(s1);
				s1.sprintf("            %12.3f,%12.3f,%12.3f,%12.3f", g_tCalib.NtcRes.x3, g_tCalib.NtcRes.y3,
							g_tCalib.NtcRes.x4, g_tCalib.NtcRes.y4);
				FormCalib->Memo1->Lines->Add(s1);

				FormCalib->Memo1->Lines->Add("");
			}
			break;

		case REG03_BOOT_STATUS:
			p = &_rx_buf[3];
			mBootStatus = BEBufToUint16(p);
			break;

		case REG03_APP_VER:
			p = &_rx_buf[3];
			{
				uint16_t ver;
				char buf[32];

				ver = BEBufToUint16(p);
				sprintf(buf, "V%X.%02X", ver >> 8, ver & 0xFF);
				Form1->MemoLuaOut->Lines->Add("��ǰAPP�汾 : " + (AnsiString)buf);
				if (ver < FW_APP_VER)
				{
					Form1->MemoLuaOut->Lines->Add("APP�̼���Ҫ����");
				}
				else
				{
					Form1->MemoLuaOut->Lines->Add("APP�̼��������£��������");
				}
			}
			break;

		case REG03_BOOT_VER:
			p = &_rx_buf[3];
			{
				uint16_t ver;
				char buf[32];

				ver = BEBufToUint16(p);
				sprintf(buf, "V%X.%02X", ver >> 8, ver & 0xFF);
				Form1->MemoLuaOut->Lines->Add("��ǰBOOT�汾 : " + (AnsiString)buf);
				if (ver < FW_BOOT_VER)
				{
					Form1->MemoLuaOut->Lines->Add("BOOT�̼���Ҫ����");
				}
				else
				{
					Form1->MemoLuaOut->Lines->Add("BOOT�̼��������£��������");
				}
			}
			break;

		default:
			if (g_tCmd.RegAddr >= REG03_CH1_WAVE_0 && g_tCmd.RegAddr < REG03_CH1_WAVE_0 + 512)
			{
				int i;
				int idx;
				int bytes;

				bytes = _rx_buf[2];
				p = &_rx_buf[3];
				// �����豸ʱ�������л���������״̬����ʱ�п�������Ӧ�������������¸����쳣
				if (bytes == (DSO_PACKAGE_SIZE % 256))
				{
					idx = (g_tCmd.RegAddr - REG03_CH1_WAVE_0) * (DSO_PACKAGE_SIZE / 4);

					for (i = 0; i < g_tCmd.RegNum / 2; i++)
					{
						mAdcBuf1[idx + i] = BEBufToFloat(p);
						p += 4;
					}
				}
			}
			if (g_tCmd.RegAddr >= REG03_CH2_WAVE_0 && g_tCmd.RegAddr < REG03_CH2_WAVE_0 + 512)
			{
				int i;
				int idx;
				int bytes;

				bytes = _rx_buf[2];
                p = &_rx_buf[3];
				if (bytes == (DSO_PACKAGE_SIZE  % 256))
				{
					idx = (g_tCmd.RegAddr - REG03_CH2_WAVE_0) * (DSO_PACKAGE_SIZE / 4);

					for (i = 0; i < g_tCmd.RegNum / 2; i++)
					{
						mAdcBuf2[idx + i] = BEBufToFloat(p);
						p += 4;
					}
				}
			}
			break;
	}
}

extern char s_udp_print_RxBuf[UDP_PRINT_BUFFSIZE];
extern uint32_t  s_udp_print_RxLen;
extern void DispUdpPringData(void);
void __fastcall TFormTcp::MODH_61H(uint8_t *_rx_buf, uint16_t _len)
{
	/*
		H7-TOOL��������:    PC����Ӧ��
			01  ; �ӻ���ַ ��Ϊ1
			61  ; ������
			00  ; ͨ���ţ�00��ʾprint
			01  : ���ȸ��ֽ�
			08  : ���ȵ��ֽ�
			xx xx xx xx ... : ������
			CC CC : CRC16
	*/
	/*
		H7-TOOL��������:
			01  ; �ӻ���ַ ��Ϊ1
			61  ; ������
			00  ; ͨ���ţ�00��ʾprint
			01  : ���ȸ��ֽ�
			08  : ���ȵ��ֽ�
			xx xx xx xx ... : ������
			CC CC : CRC16
	*/
	uint8_t func;
	uint8_t ch;
	uint16_t len;
	uint16_t pos;

	pos = 1;
	func = _rx_buf[pos++];
	ch = _rx_buf[pos++];
	len = _rx_buf[pos] * 256 + _rx_buf[pos + 1];
	pos += 2;

	for (int i = 0; i < len; i++)
	{
		if (s_udp_print_RxLen < UDP_PRINT_BUFFSIZE)
		{
			s_udp_print_RxBuf[s_udp_print_RxLen++] = _rx_buf[pos++];
		}
	}

	DispUdpPringData();
}

void __fastcall TFormTcp::MODH_66H(uint8_t *_rx_buf, uint16_t _len)
{
    /*
        ��������: С��������
            01  ; վ��
            66  ; ������
            0000  ; �ӹ���,
                - 0��ʾ���ڴ�
                - 1��ʾд�ڴ�
            0000 0000 : ƫ�Ƶ�ַ 4�ֽ�
            0020 0000 : ���ݳ��� 4�ֽ�
            CCCC      : CRC16

		�ӻ�Ӧ��:
            01  ; �ӻ���ַ
            66  ; ������
            0000  ; �ӹ���
			0000 0000 : ƫ�Ƶ�ַ 4�ֽ�
			0020 0000 : ���ݳ��� 4�ֽ�
            00  ; ִ�н����0��ʾOK  1��ʾ����

            CCCC : CRC16
	*/
	int pos = 0;
	char *p;
	uint16_t func;
	uint32_t offset;
	uint32_t len;
	uint8_t ack;
	uint32_t i;

	p = &_rx_buf[2];
	func = BEBufToUint16(p); p += 2;
	offset = BEBufToUint32(p); p += 4;
	len = BEBufToUint32(p); p += 4;
	ack = p[0]; p++;

	offset = offset - mLua.FlashBaseAddr;

	if (offset + len > sizeof(mLua.DataBuf))
	{
		return;
	}

	if (func == H66_READ_MEM_INIT || func == H66_READ_MEM
		|| func == H66_READ_DISP_MEM)  /* ��flash */
	{
		if (ack == 0)
		{
			for (i = 0; i < len; i++)
			{
				mLua.DataBuf[offset + i] = *p;
				p++;
			}
		}
	}
	else
	{
		return;
	}
}

void __fastcall TFormTcp::MODH_06H(uint8_t *_rx_buf, uint16_t _len)
{
	uint16_t reg;
	uint16_t value;

	reg = BEBufToUint16(&_rx_buf[2]);
	value = BEBufToUint16(&_rx_buf[4]);
	if (reg >= REG03_CALIB_KEY && reg <= REG03_CALIB_PARAM_END)
	{
		AnsiString s;

		s.sprintf("д��ɹ�. 06Hָ��, �Ĵ���0x%04X = 0x%04X", reg, value);

		if (FormCalib->Visible == false)
		{
			Form1->MemoAdd(s);
		}
		else
		{
			FormCalib->Memo1->Lines->Add(s);
		}
	}
}

void __fastcall TFormTcp::MODH_10H(uint8_t *_rx_buf, uint16_t _len)
{
	uint16_t reg;

	reg = BEBufToUint16(&_rx_buf[2]);
	if (reg >= REG03_CALIB_KEY && reg <= REG03_CALIB_PARAM_END)
	{
		AnsiString s;

		s.sprintf("д��ɹ�. 10Hָ��, �Ĵ���0x%04X", reg);
		FormCalib->Memo1->Lines->Add(s);
	}
	else if (reg == REG03_NET_LOCAL_IP_H)
	{
        Form1->MemoLuaOut->Lines->Add("�޸�IP�����ɹ�");
	}
}

void __fastcall TFormTcp::Thread1Run(TIdThreadComponent *Sender)
{
	FormTcp->CommTask();
}
//---------------------------------------------------------------------------

void __fastcall TFormTcp::FormDestroy(TObject *Sender)
{
	Thread1->Terminate();
}

// DoLua �� ����ʼ��lua
void __fastcall TFormTcp::DoLuaSend64H(AnsiString s)
{
	int i;
	FormTcp->mLua.Func = H64_LUA_RUN_NO_RESET;
	FormTcp->mLua.Offset = 0;

	for (i = 0; i < s.Length(); i++)
	{
		FormTcp->mLua.DataBuf[i] = s.c_str()[i];
	}
	FormTcp->mLua.DataBuf[i] = 0;
	FormTcp->mLua.DataLen = i + 1;

	g_tCmd.UserCmdStatus = CMDS_DOWNLOAD_LUA;
	g_tCmd.UserReq = 1;
}

// DoLua �� ��ʼ��lua
void __fastcall TFormTcp::InitAndDoLuaSend64H(AnsiString s)
{
	int i;
	FormTcp->mLua.Func = H64_LUA_RUN_WITH_RESET;
	FormTcp->mLua.Offset = 0;

	for (i = 0; i < s.Length(); i++)
	{
		FormTcp->mLua.DataBuf[i] = s.c_str()[i];
	}
	FormTcp->mLua.DataBuf[i] = 0;
	FormTcp->mLua.DataLen = i + 1;

	g_tCmd.UserCmdStatus = CMDS_DOWNLOAD_LUA;
	g_tCmd.UserReq = 1;
}

//---------------------------------------------------------------------------

// ͨ���߳�
void __fastcall TFormTcp::CommTask(void)
{
	UnicodeString PeerIP;   	// �Է���IP
	unsigned short PeerPort;	// �Է��Ķ˿�

	AnsiString s;
	int read_len = 0;
	AnsiString id;

	static uint32_t s_rx_pos = 0;
	static uint32_t s_run_time = 0;

	if (FormSet == NULL || Form1 == NULL || FormLua == NULL
		|| FormCalib == NULL || FormProg == NULL)
	{
		return;
	}

	try
	{
		if (g_tIniParam.CommInterface == 1)     /* ͨ�Žӿ�ΪUSB���⴮�� */
		{
			// ���͵�UDP�����
			if (Form1->YbCommDevice1->Active == true)
			{
				read_len = Form1->YbCommDevice1->Read((uint8_t *)&g_buf[0], 2048); //�� 1 ���ֽ�

				// ����رմ���
				if (FormTcp->mReqCloseCom == 1)
				{
					FormTcp->mReqCloseCom = 0;
					Form1->YbCommDevice1->Active = false;
				}


				if (read_len > 0)
				{
					Form1->TimerPowerOn->Tag = 0;  // ����USB���߼���
				}

				if (read_len > 0 && read_len < DYN_ARRAY_SIZE)
				{
					for (int i = 0; i < read_len; i++)
					{
						if (s_RxLen < DYN_ARRAY_SIZE)
						{
							s_RxBuf[s_RxLen++] = g_buf[i];
						}
					}

					Thread1->Synchronize(DealRxData); 	// �߳�ͬ���������������ʾ
				}
			}
			else
			{
				Sleep(100);
				return;
			}
		}
		else       /* ͨ�Žӿ�Ϊ���ں�WIFI  */
		{
			// ��ȡUDP���ݰ� - ��Ҫͨ�Žӿ�
			read_len = FormTcp->UDP1->ReceiveBuffer(g_buf, PeerIP, PeerPort, 0);
			if (read_len > 0 && read_len < DYN_ARRAY_SIZE)
			{

				for (int i = 0; i < read_len; i++)
				{
					if (s_RxLen < DYN_ARRAY_SIZE)
					{
						s_RxBuf[s_RxLen++] = g_buf[i];
					}
				}

				Thread1->Synchronize(DealRxData); 	// �߳�ͬ���������������ʾ
			}

		}
	}
	catch (Exception &e)
	{
		Form1->MemoLuaOut->Lines->Add("��ȡ�����쳣");
	}

	try
	{
		UserCmd();   /* �û���ʱ���� */

		if (g_tCmd.CmdStatus > 0)
		{

		}
		else
		{
			Sleep(2);
		}
	}
	catch (Exception &e)
	{
		Form1->MemoLuaOut->Lines->Add("ִ��UserCmd�쳣");
		g_tCmd.CmdStatus = 0;
		g_tCmd.UserReq = 0;
	}
}
