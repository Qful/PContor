//---------------------------------------------------------------------------

#include <vcl.h>
#include "winioctl.h"
#include <ClipBrd.hpp>

#pragma hdrstop

#include <IniFiles.hpp>
#include "UnitSet.h"
#include "UnitLua.h"
#include "UnitProg.h"
#include "UnitMain.h"
#include "UnitThread.h"
#include "UnitUSBBus.h"
#include "UnitRTT.h"
#include "UnitScreenShot.h"
#include "lua_if.h"
#include "file_lib.h"
#include "hex2bin.h"
#include "bsp.h"
#include "bsp_udp_modbus.h"
#include "bsp_user_lib.h"
#include "usb_hid.h"

//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TFormSet *FormSet;

const int g_BaudInt[] = {
	CBR_110, CBR_300, CBR_600, CBR_1200,
	CBR_2400, CBR_4800, CBR_9600, CBR_14400,
	CBR_19200, CBR_38400, CBR_56000, CBR_57600,
	CBR_115200, CBR_128000, CBR_256000
};
/* 0 - 2 �ֱ��Ӧ �޼��顢��У�顢żУ�� */
const int g_Parity[3] = {0, 1, 2};
const char *g_ParityText[3] = {"��У��", "��У��", "żУ��"};

//---------------------------------------------------------------------------
__fastcall TFormSet::TFormSet(TComponent* Owner)
	: TForm(Owner)
{
}

void __fastcall TFormSet::FormCreate(TObject *Sender)
{
	bsp_AddLog("FormSet Create Begin");

	FormSet->EditH7IPAddr->Text = g_tIniParam.IPAddr;
	FormSet->ComboBoxCommMode->ItemIndex = g_tIniParam.CommInterface;

//		Application->CreateForm(__classid(TFormProg), &FormProg);
//		Application->CreateForm(__classid(TFormUSBBus), &FormUSBBus);
//		Application->CreateForm(__classid(TFormLua), &FormLua);
//		Application->CreateForm(__classid(TFormSet), &FormSet);
	{
		FormProg->Parent = Form1;
		FormProg->Left = Form1->PanelLua->Left + Form1->PanelStation->Left;
		FormProg->Top = Form1->PanelLua->Top + Form1->PanelStation->Top;
		FormProg->Height = Form1->PanelStation->Height;
		FormProg->Width = Form1->PanelStation->Width;

		FormUSBBus->Parent = Form1;
		FormUSBBus->Left = Form1->PanelLua->Left + Form1->PanelStation->Left;
		FormUSBBus->Top = Form1->PanelLua->Top + Form1->PanelStation->Top;
		FormUSBBus->Height = Form1->PanelStation->Height;
		FormUSBBus->Width = Form1->PanelStation->Width;

		FormLua->Parent = Form1;
		FormLua->Left = Form1->PanelLua->Left + Form1->PanelStation->Left;
		FormLua->Top = Form1->PanelLua->Top + Form1->PanelStation->Top;
		FormLua->Height = Form1->PanelStation->Height;
		FormLua->Width = Form1->PanelStation->Width;

		FormSet->Parent = Form1;
		FormSet->Left = Form1->PanelLua->Left + Form1->PanelStation->Left;
		FormSet->Top = Form1->PanelLua->Top + Form1->PanelStation->Top;
		FormSet->Height = Form1->PanelStation->Height;
		FormSet->Width = Form1->PanelStation->Width;

		FormRTT->Parent = Form1;
		FormRTT->Left = Form1->PanelLua->Left + Form1->PanelStation->Left;
		FormRTT->Top = Form1->PanelLua->Top + Form1->PanelStation->Top;
		FormRTT->Height = Form1->PanelStation->Height;
		FormRTT->Width = Form1->PanelStation->Width;
	}

	bsp_AddLog("FormSet Create End");
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void __fastcall TFormSet::Button92Click(TObject *Sender)
{
	Form1->YbCommDevice1->SettingsDialog(this,true);

	if (Form1->YbCommDevice1->Active == true)
	{
		LabelPort->Caption = "��ǰ�����Ѵ�:" + Form1->YbCommDevice1->PortName +
			" ������: " + IntToStr(g_BaudInt[Form1->YbCommDevice1->Baud]) +
			"   ��żУ��: " + g_ParityText[Form1->YbCommDevice1->Parity];
	}
	else
	{
		LabelPort->Caption = "��ǰ�����ѹر�:" + Form1->YbCommDevice1->PortName +
			" ������: " + IntToStr(g_BaudInt[Form1->YbCommDevice1->Baud]) +
			"   ��żУ��: " + g_ParityText[Form1->YbCommDevice1->Parity];
	}
}
//---------------------------------------------------------------------------
void __fastcall TFormSet::Button101Click(TObject *Sender)
{
	try
	{
		Form1->YbCommDevice1->Active = true;
	}
	catch (Exception &e)
	{
		ShowMessage(""+e.Message);
	}

	if (Form1->YbCommDevice1->Active == true)
	{
		LabelPort->Caption = "��ǰ�����Ѵ�:" + Form1->YbCommDevice1->PortName +
			" ������: " + IntToStr(g_BaudInt[Form1->YbCommDevice1->Baud]) +
			"   ��żУ��: " + g_ParityText[Form1->YbCommDevice1->Parity];
	}
	else
	{
		LabelPort->Caption = "��ǰ�����ѹر�:" + Form1->YbCommDevice1->PortName +
			" ������: " + IntToStr(g_BaudInt[Form1->YbCommDevice1->Baud]) +
			"   ��żУ��: " + g_ParityText[Form1->YbCommDevice1->Parity];
	}
}
//---------------------------------------------------------------------------
void __fastcall TFormSet::Button102Click(TObject *Sender)
{
	try
	{
		Form1->YbCommDevice1->Active = false;
	}
	catch (Exception &e)
	{
		ShowMessage(""+e.Message);
	}

	if (Form1->YbCommDevice1->Active == true)
	{
		LabelPort->Caption = "��ǰ�����Ѵ�:" + Form1->YbCommDevice1->PortName +
			" ������: " + IntToStr(g_BaudInt[Form1->YbCommDevice1->Baud]) +
			"   ��żУ��: " + g_ParityText[Form1->YbCommDevice1->Parity];
	}
	else
	{
		LabelPort->Caption = "��ǰ�����ѹر�:" + Form1->YbCommDevice1->PortName +
			" ������: " + IntToStr(g_BaudInt[Form1->YbCommDevice1->Baud]) +
			"   ��żУ��: " + g_ParityText[Form1->YbCommDevice1->Parity];
	}
}
//---------------------------------------------------------------------------
void __fastcall TFormSet::Button105Click(TObject *Sender)
{
  int iFileHandle;
  int iBytesRead;

  Form1->OpenDialog1->InitialDir = AppExePath() + "\\EMMC\\H7-TOOL\\Firmware";
  if (Form1->OpenDialog1->Execute())
  {
		iFileHandle = FileOpen(Form1->OpenDialog1->FileName, fmOpenRead);
		if (iFileHandle > 0)
		{
			#if 0
			  mFileLength = FileSeek(iFileHandle, 0, 2);
			  if (mFileLength <= 2 * 1024 * 1024)
			  {
					FileSeek(iFileHandle,0,0);
					iBytesRead = FileRead(iFileHandle, mFileBuf, mFileLength);
			  }
			  else
			  {
				  ShowMessage("�ļ�����");
			  }
			#else         /* �Ƚ��ļ����ݺ��ļ����ȴ��뱸�ݻ�����mbakFileBuf�����ݽ��ܺ��ٷ���mFileBuf */
			  Form1->mbakFileLength = FileSeek(iFileHandle, 0, 2);
			  if (Form1->mbakFileLength <= 2 * 1024 * 1024)
			  {
					FileSeek(iFileHandle,0,0);
					iBytesRead = FileRead(iFileHandle, Form1->mbakFileBuf, Form1->mbakFileLength);
			  }
			  else
			  {
				  ShowMessage("�ļ�����");
			  }
			#endif
			  FileClose(iFileHandle);
		}
  }
}
//---------------------------------------------------------------------------
void __fastcall TFormSet::Button104Click(TObject *Sender)
{
	if (g_tIniParam.CommInterface != 1)
	{
		Form1->MemoLuaOut->Lines->Add("�����̼�����Ҫѡ��USBͨ�Žӿ�");
		return;
	}

	H7_WriteRegister(REG03_RESET_TO_BOOT, 2);

	Form1->MemoLuaOut->Lines->Add("��֪ͨH7-TOOL����Boot");

	// ֪ͨͨ�ż��������Ҫ���´򿪴�����
	Form1->mLinkStatus = 10;
	Form1->TimerPowerOn->Interval = 1000;
}
//---------------------------------------------------------------------------
/*
	У������ļ�
	mbakFileLength --> mFileBuf
*/
uint8_t DecryptFile(char *_infile, int32_t _inlen, char *_outfile, int32_t *_outlen)
{
	/* ��Դ������̼��ļ��Ͳ���У��ͼ����� */
	*_outlen = _inlen;
	memcpy(_outfile, _infile, *_outlen);
	return 1;
}

void __fastcall TFormSet::Button103Click(TObject *Sender)
{
	uint32_t outlen;

	if (g_tIniParam.CommInterface != 1)
	{
		Form1->MemoLuaOut->Lines->Add("�����̼�����Ҫѡ��USBͨ�Žӿ�");
		return;
	}

	if (Form1->YbCommDevice1->Active == false)
	{
		LabelUpgrade->Caption = "��򿪴���";
		return;
	}

	/* �����ļ� */
	if (DecryptFile(Form1->mbakFileBuf, Form1->mbakFileLength, Form1->mFileBuf, &Form1->mFileLength) == 1)
	{
		g_tCmd.CmdStatus = CMDS_PRO_UPGRADE; //��������״̬��
	}
	else
	{
		LabelUpgrade->Caption = "�����ȷ���ļ�";
	}
}
//---------------------------------------------------------------------------
void __fastcall TFormSet::Button106Click(TObject *Sender)
{
	H7_WriteRegister(REG03_RESET_TO_BOOT, 1);
}
//---------------------------------------------------------------------------
void __fastcall TFormSet::ButtonSetClockClick(TObject *Sender)
{
	uint16_t reg_buf[6];
	uint16_t addr, year, month, day, hour, min, sec, msec;
	TDateTime tt;

	tt = Now();
	tt.DecodeTime(&hour, &min, &sec, &msec);
	tt.DecodeDate(&year, &month, &day);
	reg_buf[0] =  year;
	reg_buf[1] =  month;
	reg_buf[2] =  day;
	reg_buf[3] =  hour;
	reg_buf[4] =  min;
	reg_buf[5] =  sec;
	H7_WriteMutiRegisters(REG03_RTC_YEAR, 6, reg_buf);

	Form1->MemoLuaOut->Lines->Add("ͬ��ʱ��");
	FormTcp->mDispWriteRegister = 1;
}
//---------------------------------------------------------------------------
void __fastcall TFormSet::Button3Click(TObject *Sender)
{
	FormTcp->mDevNum = 0;

	memset(&FormTcp->mDevList, sizeof(FormTcp->mDevList), 0);

	ListBox1->Items->Clear();
//	EditCpuID->Text = "";
//	EditMac->Text = "";
//	EditHardVer->Text = "";
//	EditSoftVer->Text = "";
//	EditLocalIP->Text = "";
//	EditNetMask->Text = "";
//	EditGateWay->Text = "";
//	EditJkxPort->Text = "";
//	EditSysIP->Text = "";
//	EditSysPort->Text = "";
}
//---------------------------------------------------------------------------
void __fastcall TFormSet::Button2Click(TObject *Sender)
{
	uint16_t addr, year, month, day, hour, min, sec, msec;
	TDateTime tt;
	AnsiString s;

	if (Form1->CheckBoxAutoReadWave->Checked)
	{
		Form1->CheckBoxAutoReadWave->Checked = false;
		Form1->MemoLuaOut->Lines->Add("�ѹر��Զ�ˢ�²��Σ���ʼɨ��");
	}

	if (g_tIniParam.CommInterface == 1)
	{
		Form1->MemoLuaOut->Lines->Add("ͨ�Žӿ�ѡ����̫���ٿ�ʼɨ��");
		return;
	}

	tt = Now();
	tt.DecodeTime(&hour, &min, &sec, &msec);
	tt.DecodeDate(&year, &month, &day);

	FormTcp->mDevNum = 0;
	ListBox1->Items->Clear();
	EditCpuID->Text = "";
	EditMac->Text = "";
	EditHardVer->Text = "";
	EditSoftVer->Text = "";
	EditLocalIP->Text = "";
	EditNetMask->Text = "";
	EditGateWay->Text = "";
	EditJkxPort->Text = "";
	EditSysIP->Text = "";
	EditSysPort->Text = "";

	//Data :022684DC "FF FF FF FF FF FF 03 00 00 00 0A D5 F7 "

	//udp_Send03H_MAC(MAC_FF, REG03_NET_CPU_ID0, 21);
	g_tCmd.UserCmdStatus = CMDS_UDP_FIND;
	g_tCmd.UserReq = 1;

	Form1->MemoLuaOut->Lines->Add("��ʼ�Զ�����H7-TOOL...");
	FormTcp->mDevNum = 0;	/* ��ռ�����MAC�� */
}

uint8_t StrToIP(AnsiString s, uint8_t *ip)
{
	char *p;
	char buf[64];


	p = s.c_str();

	for (int i = 0; i < sizeof(buf); i++)
	{
		buf[i] = s.c_str()[i];
	}

	p = buf;

	ip[0] = str_to_int2(p);
	p = strchr(p, '.');
	if (p == 0) return 0;
	p++;

	ip[1] = str_to_int2(p);
	p = strchr(p, '.');
	if (p == 0) return 0;
	p++;

	ip[2] = str_to_int2(p);
	p = strchr(p, '.');
	if (p == 0) return 0;
	p++;

	ip[3] = str_to_int2(p);

	return 1;	/* ok */
}

//---------------------------------------------------------------------------
void __fastcall TFormSet::Button1Click(TObject *Sender)
{
	uint16_t reg_buf[256];
	uint8_t ip_buf[4];
	uint16_t pos = 0;
	int index;
	AnsiString s1;
	char *p;
	int port;
	uint8_t mac[6];

	if (ComboBoxCommMode->ItemIndex == 0)  // ��̫��
	{
		if (ListBox1->Count == 0)
		{
			Form1->MemoLuaOut->Lines->Add("���ȵ��[�����豸]��ť");
			return;
		}

		index = ListBox1->ItemIndex;
		if (index < 0)
		{
			Form1->MemoLuaOut->Lines->Add("��ѡ��һ��H7-TOOL");
			return;
		}

		memcpy(mac, FormTcp->mDevList[index].Mac, 6);
	}

	pos = 0;
	s1 = EditLocalIP->Text;
	if (StrToIP(s1, ip_buf) == 0)
	{
		Form1->MemoLuaOut->Lines->Add("����IP��ַ��ʽ����");
		return;
	}
	reg_buf[pos++] =  (ip_buf[0] << 8) + ip_buf[1];
	reg_buf[pos++] =  (ip_buf[2] << 8) + ip_buf[3];

	s1 = EditNetMask->Text;
	if (StrToIP(s1, ip_buf) == 0)
	{
		Form1->MemoLuaOut->Lines->Add("���������ʽ����");
		return;
	}
	reg_buf[pos++] =  (ip_buf[0] << 8) + ip_buf[1];
	reg_buf[pos++] =  (ip_buf[2] << 8) + ip_buf[3];

	s1 = EditGateWay->Text;
	if (StrToIP(s1, ip_buf) == 0)
	{
		Form1->MemoLuaOut->Lines->Add("���ص�ַ��ʽ����");
		return;
	}
	reg_buf[pos++] =  (ip_buf[0] << 8) + ip_buf[1];
	reg_buf[pos++] =  (ip_buf[2] << 8) + ip_buf[3];

	s1 = EditJkxPort->Text;
	port = StrToInt(s1);
	if (port > 65535 || port < 100)
	{
		Form1->MemoLuaOut->Lines->Add("�����˿ںŸ�ʽ����");
		return;
	}
	reg_buf[pos++] = port;

	s1 = EditSysIP->Text;
	if (StrToIP(s1, ip_buf) == 0)
	{
		Form1->MemoLuaOut->Lines->Add("����ϵͳIP��ַ��ʽ����");
		return;
	}
	reg_buf[pos++] =  (ip_buf[0] << 8) + ip_buf[1];
	reg_buf[pos++] =  (ip_buf[2] << 8) + ip_buf[3];

	s1 = EditSysPort->Text;
	port = StrToInt(s1);
	if (port > 65535)
	{
		Form1->MemoLuaOut->Lines->Add("����ϵͳ�˿ںŸ�ʽ����");
		return;
	}
	reg_buf[pos++] = port;

	if (ComboBoxCommMode->ItemIndex == 0)
	{
		Form1->MemoLuaOut->Lines->Add("��ʼ�޸�IP����(ͨ����̫��)");
		udp_Send10H_MAC(mac, REG03_NET_LOCAL_IP_H, 10, reg_buf);
	}
	else if (ComboBoxCommMode->ItemIndex == 1)
	{
		Form1->MemoLuaOut->Lines->Add("��ʼ�޸�IP����(ͨ��USB)");
		udp_Send10H(REG03_NET_LOCAL_IP_H, 10, reg_buf);
	}
}
//---------------------------------------------------------------------------
void __fastcall TFormSet::ListBox1Click(TObject *Sender)
{
	int index;
	AnsiString s1;

	index = ListBox1->ItemIndex;
	if (index < 0)
	{
		return;
	}

	s1.sprintf("%02X%02X %02X%02X %02X%02X %02X%02X %02X%02X %02X%02X",
		FormTcp->mDevList[index].CpuID[0], FormTcp->mDevList[index].CpuID[1], FormTcp->mDevList[index].CpuID[2], FormTcp->mDevList[index].CpuID[3],
		FormTcp->mDevList[index].CpuID[4], FormTcp->mDevList[index].CpuID[5], FormTcp->mDevList[index].CpuID[6], FormTcp->mDevList[index].CpuID[7],
		FormTcp->mDevList[index].CpuID[8], FormTcp->mDevList[index].CpuID[9], FormTcp->mDevList[index].CpuID[10], FormTcp->mDevList[index].CpuID[11]);
	EditCpuID->Text = s1;

	s1.sprintf("%02X-%02X-%02X-%02X-%02X-%02X", FormTcp->mDevList[index].Mac[0], FormTcp->mDevList[index].Mac[1], FormTcp->mDevList[index].Mac[2],
					FormTcp->mDevList[index].Mac[3], FormTcp->mDevList[index].Mac[4], FormTcp->mDevList[index].Mac[5]);
	EditMac->Text = s1;

	s1.sprintf("%04X", FormTcp->mDevList[index].HardVer);
	EditHardVer->Text = s1;

	s1.sprintf("%04X", FormTcp->mDevList[index].SoftVer);
	EditSoftVer->Text = s1;

	s1.sprintf("%d.%d.%d.%d", FormTcp->mDevList[index].LocalIP[0], FormTcp->mDevList[index].LocalIP[1],
		FormTcp->mDevList[index].LocalIP[2], FormTcp->mDevList[index].LocalIP[3]);
	EditLocalIP->Text = s1;
	EditH7IPAddr->Text = s1;
	g_tIniParam.IPAddr = EditH7IPAddr->Text;

	s1.sprintf("%d.%d.%d.%d", FormTcp->mDevList[index].NetMask[0], FormTcp->mDevList[index].NetMask[1],
		FormTcp->mDevList[index].NetMask[2], FormTcp->mDevList[index].NetMask[3]);
	EditNetMask->Text = s1;

	s1.sprintf("%d.%d.%d.%d", FormTcp->mDevList[index].GateWay[0], FormTcp->mDevList[index].GateWay[1],
		FormTcp->mDevList[index].GateWay[2], FormTcp->mDevList[index].GateWay[3]);
	EditGateWay->Text = s1;

	s1.sprintf("%d", FormTcp->mDevList[index].LocalPort);
	EditJkxPort->Text = s1;

	s1.sprintf("%d.%d.%d.%d", FormTcp->mDevList[index].SysIP[0], FormTcp->mDevList[index].SysIP[1],
		FormTcp->mDevList[index].SysIP[2], FormTcp->mDevList[index].SysIP[3]);
	EditSysIP->Text = s1;

	s1.sprintf("%d", FormTcp->mDevList[index].SysPort);
	EditSysPort->Text = s1;
}
//---------------------------------------------------------------------------


void __fastcall TFormSet::ComboBoxCommModeChange(TObject *Sender)
{
	g_tIniParam.CommInterface = FormSet->ComboBoxCommMode->ItemIndex;
}
//---------------------------------------------------------------------------

void __fastcall TFormSet::EditH7IPAddrChange(TObject *Sender)
{
	;
}
//---------------------------------------------------------------------------

void __fastcall TFormSet::EditH7IPAddrExit(TObject *Sender)
{
	g_tIniParam.IPAddr = EditH7IPAddr->Text;
}
//---------------------------------------------------------------------------

String __fastcall TFormSet::ExecuteApp(String sCmdline)
{
	PROCESS_INFORMATION proc = {0};
	long ret,k=1;
	bool bret;

	STARTUPINFO start = {0};
	SECURITY_ATTRIBUTES sa = {0};
	HANDLE hReadPipe ;
	HANDLE hWritePipe;
	String sOutput;
	String sBuffer;
	unsigned long lngBytesRead;

	char cBuffer[1024 + 2];

	Form1->ProgressBar1->Position = 0;
	Form1->MemoLuaOut->Lines->Add(sCmdline);

	sa.nLength = sizeof(sa);
	sa.lpSecurityDescriptor=0;
	sa.bInheritHandle = TRUE;
	bret = ::CreatePipe(&hReadPipe, &hWritePipe,&sa, 0);
	if (!bret)
	{
		sOutput= "CreatePipe failed. Error: " + String(GetLastError());
		Form1->MemoLuaOut->Lines->Add(sOutput);
		return sOutput;
	}
	start.cb = sizeof(STARTUPINFOA);
	start.dwFlags = STARTF_USESTDHANDLES | STARTF_USESHOWWINDOW;
	start.hStdOutput = hWritePipe;
	start.hStdError = hWritePipe;
	start.wShowWindow = SW_HIDE;
	sBuffer = sCmdline;
	ret =::CreateProcess(0, sBuffer.c_str(), &sa, &sa, TRUE, NORMAL_PRIORITY_CLASS, 0, 0, &start, &proc);
	if (ret == 0)
	{
		sOutput= "Bad command or filename ";
		Form1->MemoLuaOut->Lines->Add(sOutput);
		return sOutput;
	}
	::CloseHandle(hWritePipe);
	do
	{
		memset(cBuffer, '\0 ',sizeof(cBuffer));
		ret = ::ReadFile(hReadPipe, cBuffer, 1024, &lngBytesRead, 0);
		cBuffer[lngBytesRead] = 0;
		sBuffer = String(cBuffer);
		sOutput = sOutput + sBuffer;

		if (mUpdateFileCmd == 1)
		{
			int pos;

			pos = 100 * mUpdatedCount / mUpdateFileCount;

			if (++mUpdatedCount >= mUpdateFileCount)
			{
				pos = 100;
			}
			Form1->ProgressBar1->Position = pos;
			//Form1->ProgressBar1->Refresh();
			//SendMessage(Form1->ProgressBar1->Handle, PBM_SETPOS , pos, 0);
		}
		Form1->MemoLuaOut->Text = Form1->MemoLuaOut->Text+sBuffer;
		SendMessage(Form1->MemoLuaOut->Handle,WM_VSCROLL, SB_BOTTOM, 0);
		Application->ProcessMessages();

	} while (ret != 0 );
	::CloseHandle(proc.hProcess);
	::CloseHandle(proc.hThread);
	::CloseHandle(hReadPipe);

	Form1->ProgressBar1->Position = 100;
	return sOutput;
}

//---------------------------------------------------------------------------

void __fastcall TFormSet::Button4Click(TObject *Sender)
{
	if (Form1->CheckBoxClear->Checked)
	{
		Form1->MemoLuaOut->Clear();
	}

	ExecuteApp(L"ipconfig /all");
}
//---------------------------------------------------------------------------

void __fastcall TFormSet::Button5Click(TObject *Sender)
{
	if (Form1->CheckBoxClear->Checked)
	{
		Form1->MemoLuaOut->Clear();
	}

	ExecuteApp(L"ping " + EditH7IPAddr->Text);
}
//---------------------------------------------------------------------------

void __fastcall TFormSet::Button6Click(TObject *Sender)
{
	if (Form1->CheckBoxClear->Checked)
	{
		Form1->MemoLuaOut->Clear();
	}

	//ExecuteApp(L"systeminfo");

	ExecuteApp(L"wmic os get caption");
	ExecuteApp(L"wmic os get osarchitecture");
}
//---------------------------------------------------------------------------

void __fastcall TFormSet::Button9Click(TObject *Sender)
{
	char buf[256];

	HID_FindDevices();

	Form1->MemoLuaOut->Lines->Add("�ҵ����ǵ��豸������" + IntToStr(g_tHID.DevCount));
	for (int i = 0; i < g_tHID.DevCount; i++)
	{
	   HID_GetName(i, buf, sizeof(buf));
	   Form1->MemoLuaOut->Lines->Add((AnsiString)buf);
	}
}
//---------------------------------------------------------------------------

void __fastcall TFormSet::Button8Click(TObject *Sender)
{
	if (Form1->CheckBoxClear->Checked)
	{
		Form1->MemoLuaOut->Clear();
	}

	Form1->MemoLuaOut->Lines->Add("���ع���������IP��������ȷʱ������ͨ�������޸�IP����");
	Form1->MemoLuaOut->Lines->Add("��Щ���������ϵͳ���ڶ����������ǽ��UDP�㲥��Ϣ�޷����͵�H7-TOOL��������������");
	Form1->MemoLuaOut->Lines->Add("ֻҪIP������ȷ��ǰ3�ֽں͵���IP��ͬ)���ܹ�pingͨ���Ͳ�Ӱ��H7-TOOLʹ�á�");
	Form1->MemoLuaOut->Lines->Add("�����������������ͨ������2�ַ�ʽ�޸�IP��ַ");
	Form1->MemoLuaOut->Lines->Add("  1 �л���USB�ڣ����IP��ַ�󣬵��޸�IP��ť");
	Form1->MemoLuaOut->Lines->Add("  2 ��H7-TOOL��Ļ�ϲ����ֶ��޸ġ� 00���泤��S -> ϵͳ���� -> �������");
	Form1->MemoLuaOut->Lines->Add("����ͨ���� ""ping H7-TOOL"" ��ť������̫���Ƿ���ͨ");
}
//---------------------------------------------------------------------------


void __fastcall TFormSet::Button10Click(TObject *Sender)
{
	String s;

	s =  AppExePath() + "\\Driver";
	ShellExecute(Handle, NULL, s.c_str(), NULL,NULL,SW_SHOWNORMAL);
}
//---------------------------------------------------------------------------

void __fastcall TFormSet::Button19Click(TObject *Sender)
{
	if (g_tIniParam.CommInterface != 1)
	{
		Form1->MemoLuaOut->Lines->Add("����U��ģʽ��Ҫѡ��USBͨ�Žӿ�");
		return;
	}

	H7_WriteRegister(REG03_RESET_TO_BOOT, JUMP_TO_EMMC);
}
//---------------------------------------------------------------------------

// ����USB���̣���H7-TOOL��EMMC����
void __fastcall TFormSet::SearchEMMC(void)
{
	wchar_t wbuf[256];
	DWORD dwLen;
	wchar_t disk[4];
	wchar_t ToolDisk[4];
	wchar_t VolumeName[128];
	DWORD nVolumeNameSize;
	AnsiString s;
	uint8_t Found = 0;

	if (Form1->CheckBoxClear->Checked)
	{
		Form1->MemoLuaOut->Clear();
	}

	Form1->MemoLuaOut->Lines->Add("����H7-TOOL��EMMC����");

	dwLen = GetLogicalDriveStrings(0, NULL);	//��ȡϵͳ�ַ�������.
	GetLogicalDriveStrings(dwLen, wbuf);		//��ȡ�̷��ַ���.

	for (int i = 0; i < dwLen; i++)
	{
		/* C:\ */
		disk[0] = wbuf[i++];
		disk[1] = wbuf[i++];
		disk[2] = wbuf[i++];
		disk[3] = 0;

		if (GetDriveType(disk) == DRIVE_REMOVABLE )
		{
			s = AnsiString(disk) + "  ���ƶ����� ";

			/*
				GetVolumeInformation(
				  lpRootPathName: PChar;               {���������������ַ���}
				  lpVolumeNameBuffer: PChar;           {�����������������}
				  nVolumeNameSize: DWORD;              {����������������Ƴ���}
				  lpVolumeSerialNumber: PDWORD;        {����������������к�}
				  var lpMaximumComponentLength: DWORD; {ϵͳ���������ļ�������}
				  var lpFileSystemFlags: DWORD;        {�ļ�ϵͳ��ʶ}
				  lpFileSystemNameBuffer: PChar;       {�ļ�����ϵͳ����}
				  nFileSystemNameSize: DWORD           {�ļ�����ϵͳ���Ƴ���}
				): BOOL;
			*/
			VolumeName[0] = 0;	  /* ����������ƶ���û��꣬������ĺ��������VolumeName��ֵ */
			if (GetVolumeInformation(disk, VolumeName, 128, 0, 0, 0, 0, 0)) //�ж������Ƿ�׼����
			{
				s += VolumeName;
			}

			if (wcscmp(VolumeName, L"EMMC") == 0)
			{
				s += " ��ʶ��H7-TOOL��EMMC����";
				Found = 1;
				wcscpy(ToolDisk, disk);
			}
			Form1->MemoLuaOut->Lines->Add(s);
		}
		else
		{
			Form1->MemoLuaOut->Lines->Add(disk);
		}
	}

	if (Found == 0)
	{
		mDiskFound = 0;
		Form1->MemoLuaOut->Lines->Add("û���ҵ�H7-TOOL�Ĵ���");
		return;
	}

	mDiskFound = 1;
	mDiskEmmc = AnsiString(ToolDisk);
}

//---------------------------------------------------------------------------

// ��������H7-TOOL�ļ���
void __fastcall TFormSet::ButtonUpdateClick(TObject *Sender)
{
	SearchEMMC();
	if (mDiskFound == 0)
	{
		return;
	}

	/*
		ROBOCOPY source destination /E /

		/E :: ������Ŀ¼�������յ���Ŀ¼��
					 /PURGE :: ɾ��Դ�в��ٴ��ڵ�Ŀ���ļ�/Ŀ¼��
					   /MIR :: ����Ŀ¼��(��ͬ�� /E �� /PURGE)��

						/IS :: ������ͬ�ļ���
						/IT :: �����ѵ������ļ���

	  ��ʼʱ��: 2020-12-17 21:51:23
			Դ= H:\H7-TOOL\
		  Ŀ��: D:\H7-TOOL\PC\H7-TOOL_PC\Win32\Debug\Backup\20201217_215121_EMMC\H7-TOOL\

		  �ļ�: *.*

		  ѡ��: *.* /S /E /DCOPY:DA /COPY:DAT /PURGE /MIR /R:1000000 /W:30
	*/

	// ��ʼ�����ļ�
	{
		String src;
		String tar;
		String cmd;

		// �����ļ�
		src = AppExePath() + "\\" + "EMMC\\H7-TOOL";
		tar = mDiskEmmc;
		tar += "\H7-TOOL";
		cmd = "cmd.exe /c ROBOCOPY \"" + src + "\" \"" + tar + "\" /E /IT /V ";

		// �����޸�������ͬ���ļ�
		if (CheckBoxDelNoModif->Checked)
		{
			cmd += "/IS ";
		}

		// ͬ��ɾ��, ɾ��PC���ϲ����ڵ��ļ�
		if (FormSet->CheckBoxDelSync->Checked)
		{
			 cmd += " /PURGE";  //: ɾ��Դ�в��ٴ��ڵ�Ŀ���ļ�/Ŀ¼��
		}

		mUpdateFileCmd = 1;

		mUpdateFileCount = 950;
		mUpdatedCount = 0;
		FormSet->ExecuteApp(cmd);

		mUpdateFileCmd = 0;

		Form1->MemoLuaOut->Lines->Add("Դ   = " + src);
		Form1->MemoLuaOut->Lines->Add("Ŀ�� = " + tar);
	}
}
//---------------------------------------------------------------------------

void __fastcall TFormSet::Button16Click(TObject *Sender)
{
	if (Form1->CheckBoxClear->Checked)
	{
		Form1->MemoLuaOut->Clear();
	}

	FormSet->ExecuteApp("cmd.exe /c " + EditCmd->Text);
}
//---------------------------------------------------------------------------


void __fastcall TFormSet::ButtonBackupClick(TObject *Sender)
{
	SearchEMMC();
	if (mDiskFound == 0)
	{
		return;
	}

	/*
		ROBOCOPY source destination /E /

		/E :: ������Ŀ¼�������յ���Ŀ¼��
					 /PURGE :: ɾ��Դ�в��ٴ��ڵ�Ŀ���ļ�/Ŀ¼��
					   /MIR :: ����Ŀ¼��(��ͬ�� /E �� /PURGE)��

						/IS :: ������ͬ�ļ���
						/IT :: �����ѵ������ļ���

	  ��ʼʱ��: 2020-12-17 21:51:23
			Դ= H:\H7-TOOL\
		  Ŀ��: D:\H7-TOOL\PC\H7-TOOL_PC\Win32\Debug\Backup\20201217_215121_EMMC\H7-TOOL\

		  �ļ�: *.*

		  ѡ��: *.* /S /E /DCOPY:DA /COPY:DAT /PURGE /MIR /R:1000000 /W:30
	*/

	// ��ʼ�����ļ�
	{
		String src;
		String tar;
		String cmd;

		// �Զ�����
		//if (FormSet->CheckBoxAutoBackup->Checked)
		{
			AnsiString now;

			now = Now().FormatString("YYYYMMDD_HH_");

			tar = AppExePath() + "\\Backup\\" + now + "EMMC\\H7-TOOL";
			src = mDiskEmmc;
			src += "\H7-TOOL";
			cmd = "cmd.exe /c ROBOCOPY \"" + src + "\" \"" + tar + "\" /MIR /V";

			mUpdateFileCmd = 1;
			mUpdateFileCount = 1100;
			mUpdatedCount = 0;
			FormSet->ExecuteApp(cmd);
			mUpdateFileCmd = 0;

			Form1->MemoLuaOut->Lines->Add("Դ   = " + src);
			Form1->MemoLuaOut->Lines->Add("Ŀ�� = " + tar);
		}
	}
}
//---------------------------------------------------------------------------

void __fastcall TFormSet::ButtonOpenBakupFolderClick(TObject *Sender)
{
	String s;

	s =  AppExePath() + "\\" + "Backup\\";

	ShellExecute(Form1->Handle, NULL, s.c_str(), NULL, NULL, SW_SHOWNORMAL);

	Form1->MemoLuaOut->Lines->Add("�����ļ���: " + s);
}

//���H7-TOOL ��emmc ��
void __fastcall TFormSet::Button11Click(TObject *Sender)
{

	String s;

	SearchEMMC();
	if (mDiskFound == 0)
	{
		return;
	}

	s = mDiskEmmc;
	ShellExecute(Form1->Handle, NULL, s.c_str(), NULL, NULL, SW_SHOWNORMAL);
}

//---------------------------------------------------------------------------

//���pc ��emmc ��
void __fastcall TFormSet::Button13Click(TObject *Sender)
{
	String s;

	s =  AppExePath() + "\\" + "EMMC\\";
	ShellExecute(Form1->Handle, NULL, s.c_str(), NULL, NULL, SW_SHOWNORMAL);
	Form1->MemoLuaOut->Lines->Add("�ļ���: " + s);
}
//---------------------------------------------------------------------------

void __fastcall TFormSet::Button14Click(TObject *Sender)
{
	if (Form1->CheckBoxClear->Checked)
	{
		Form1->MemoLuaOut->Clear();
	}

	H7_WriteRegister(REG03_RESET_TO_BOOT, 6);
	Form1->MemoLuaOut->Lines->Add("Boot�̼��ļ�: H7-TOOL��EMMC����: H7-TOOL\Firmware\H7-BOOT.bin");
	Form1->MemoLuaOut->Lines->Add("�ѷ��͸���boot�̼�ָ��, ��۲�H7-TOOL��С��Ļ");
}
//---------------------------------------------------------------------------

void __fastcall TFormSet::ButtonQuiEmmcClick(TObject *Sender)
{
	HANDLE hDevice; // handle to the drive to be examined
	BOOL bResult; // results flag
	DWORD junk; // discard results
	DWORD dwError;
	String csVolume;


	csVolume = mDiskEmmc;
	// Open the volume
	hDevice = CreateFile(csVolume.c_str(), // drive to open
		GENERIC_READ | GENERIC_WRITE,
		FILE_SHARE_READ | FILE_SHARE_WRITE, // share mode
		NULL, // default security attributes
		OPEN_EXISTING, // disposition
		0, // file attributes
		NULL); // don't copy any file's attributes

	if (hDevice == INVALID_HANDLE_VALUE) // can't open the drive
	{
		dwError = GetLastError();
		return;
	}

	//Dismount the volume
	bResult = DeviceIoControl(
		hDevice, // handle to volume
		IOCTL_STORAGE_EJECT_MEDIA, //eject USB
		NULL, // lpInBuffer
		0, // nInBufferSize
		NULL, // lpOutBuffer
		0, // nOutBufferSize
		&junk, // discard count of bytes returned
		(LPOVERLAPPED) NULL); // synchronous I/O
	if (!bResult) // IOCTL failed
	{
		dwError = GetLastError();
	}

	// Close the volume handle
	bResult = CloseHandle(hDevice);
	if (!bResult)
	{
		dwError = GetLastError();
	}

	return;
}
//---------------------------------------------------------------------------

void __fastcall TFormSet::Button18Click(TObject *Sender)
{
	// ��boot�汾
	H7_ReadRegisters(REG03_BOOT_VER, 1);
}
//---------------------------------------------------------------------------

void __fastcall TFormSet::Button15Click(TObject *Sender)
{
	// ��APP�汾
	H7_ReadRegisters(REG03_APP_VER, 1);
}
//---------------------------------------------------------------------------

void __fastcall TFormSet::Button7Click(TObject *Sender)
{
	AnsiString s1;

	s1 = AppExePath() + "\\Driver\\USB������װ˵��.txt";

	Form1->MemoLuaOut->Lines->LoadFromFile(s1);
}
//---------------------------------------------------------------------------

void __fastcall TFormSet::ButtonScreShotClick(TObject *Sender)
{
	if (Form1->CheckBoxClear->Checked)
	{
		Form1->MemoLuaOut->Clear();
	}

	/* ֪ͨ�߳̿�ʼ���� */
	g_tCmd.UserCmdStatus = CMDS_READ_SCREEN;
	g_tCmd.UserReq = 1;
}
//---------------------------------------------------------------------------

void __fastcall TFormSet::Button17MouseDown(TObject *Sender, TMouseButton Button,
          TShiftState Shift, int X, int Y)
{
	if (Form1->CheckBoxClear->Checked)
	{
		Form1->MemoLuaOut->Clear();
	}

	Form1->MemoLuaOut->Lines->Add("S ������");
	g_tCmd.DisabelAck = 1;	/* ��ֹӦ�� */
	H7_WriteRegister(REG03_SEND_KEY, 0x0001);
	g_tCmd.CmdStatus = 0;
}
//---------------------------------------------------------------------------

void __fastcall TFormSet::Button17MouseUp(TObject *Sender, TMouseButton Button, TShiftState Shift,
          int X, int Y)
{
	if (Form1->CheckBoxClear->Checked)
	{
		Form1->MemoLuaOut->Clear();
	}

	Form1->MemoLuaOut->Lines->Add("S ���ɿ�");
	g_tCmd.DisabelAck = 1;	/* ��ֹӦ�� */
	H7_WriteRegister(REG03_SEND_KEY, 0x0000);
	g_tCmd.CmdStatus = 0;
}
//---------------------------------------------------------------------------


void __fastcall TFormSet::Button20MouseDown(TObject *Sender, TMouseButton Button,
          TShiftState Shift, int X, int Y)
{
	if (Form1->CheckBoxClear->Checked)
	{
		Form1->MemoLuaOut->Clear();
	}

	Form1->MemoLuaOut->Lines->Add("C ������");
	g_tCmd.DisabelAck = 1;	/* ��ֹӦ�� */
	H7_WriteRegister(REG03_SEND_KEY, 0x0101);
	g_tCmd.CmdStatus = 0;
}
//---------------------------------------------------------------------------

void __fastcall TFormSet::Button20MouseUp(TObject *Sender, TMouseButton Button, TShiftState Shift,
          int X, int Y)
{
	if (Form1->CheckBoxClear->Checked)
	{
		Form1->MemoLuaOut->Clear();
	}

	Form1->MemoLuaOut->Lines->Add("C ���ɿ�");
	g_tCmd.DisabelAck = 1;	/* ��ֹӦ�� */
	H7_WriteRegister(REG03_SEND_KEY, 0x0100);
	g_tCmd.CmdStatus = 0;
}
//---------------------------------------------------------------------------

void __fastcall TFormSet::CheckBoxScreenClick(TObject *Sender)
{
	if (CheckBoxScreen->Checked)
	{
		Timer1->Enabled = true;
		Form1->CheckBoxMonitorUSB->Checked = false;
	}
	else
	{
		Timer1->Enabled = false;
		Form1->CheckBoxMonitorUSB->Checked = true;
	}
}
//---------------------------------------------------------------------------

void __fastcall TFormSet::Timer1Timer(TObject *Sender)
{
	/* ֪ͨ�߳̿�ʼ���� */
	g_tCmd.UserCmdStatus = CMDS_READ_SCREEN;
	g_tCmd.UserReq = 1;
}
//---------------------------------------------------------------------------

void __fastcall TFormSet::Button21Click(TObject *Sender)
{
	AnsiString name;
	AnsiString str;
	AnsiString now;

	now = Now().FormatString("YYYYMMDD_HHmmSS_");
	str.sprintf("����.bmp");
	name = AppExePath() + "\\readback\\" + now + str;

	FormSet->Image1->Picture->Bitmap->SaveToFile(name);

	Form1->MemoAdd("�����ѱ��� : " + name);
}
//---------------------------------------------------------------------------

void __fastcall TFormSet::Button12Click(TObject *Sender)
{
	String s;

	s =  AppExePath() + "\\" + "Readback\\";

	ShellExecute(Form1->Handle, NULL, s.c_str(), NULL, NULL, SW_SHOWNORMAL);

	Form1->MemoLuaOut->Lines->Add("�ļ���: " + s);
}
//---------------------------------------------------------------------------


void __fastcall TFormSet::Button22Click(TObject *Sender)
{
	// ���Ƶ�windows���а�
	Clipboard()->Assign(FormSet->Image1->Picture);

	Form1->MemoAdd("�����Ѹ��Ƶ����а�");
}
//---------------------------------------------------------------------------

void __fastcall TFormSet::Button23Click(TObject *Sender)
{
//	Graphics::TBitmap *Bitmap = new Graphics::TBitmap();
//
//	Bitmap->Width = Panel1->Width;
//	Bitmap->Height = Panel1->Height;
//	//====Bitmap->Width=Screen->Width;ȫ��Ļ��ͼ
//	//====Bitmap->Height=Screen->Height;
//
//	Form1->MemoAdd("Form1->Left=" + IntToStr(Form1->Left));
//	Form1->MemoAdd("FormSet->Left=" + IntToStr(FormSet->Left));
//	Form1->MemoAdd("PageControl1->Left=" + IntToStr(PageControl1->Left));
//	Form1->MemoAdd("Panel1->Left=" + IntToStr(Panel1->Left));
//
//	HDC hdc = GetWindowDC(0);
//	BitBlt(Bitmap->Canvas->Handle, 0, 0, Bitmap->Width, Bitmap->Height,
//		hdc,
//
//		Form1->Left + FormSet->Left + PageControl1->Left + Panel1->Left ,
//		Form1->Top + FormSet->Top + PageControl1->Top + Panel1->Top ,
//		SRCCOPY);
//
//	Clipboard()->Assign(Bitmap);
//	//====Bitmap->SaveToFile(".//screen//screen2.bmp");    //����BMP��ʽͼƬ
//	delete   Bitmap;
}

//---------------------------------------------------------------------------

void __fastcall TFormSet::ButtonInitBaseParamClick(TObject *Sender)
{
	if (IDOK == MessageBox(Handle,
		L"��Щ��������: ������������ʱ�䡢ȱʡ�������桢IP������DAP��������š����ڲ�����",
		L"�ָ���������������ֵ",  // ���ڱ���
		MB_OKCANCEL | MB_ICONINFORMATION))
	{
		Form1->MemoAdd("�ָ�H7-TOOL�Ļ�������������ֵ(������ģ����У׼����)");
		H7_WriteRegister(REG03_CALIB_KEY, 0xEF55);	// �ָ�����������ȱʡֵ
	}
}
//---------------------------------------------------------------------------

