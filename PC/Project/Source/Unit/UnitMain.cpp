//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "UnitMain.h"
#include "UnitCalib.h"
#include "UnitProg.h"
#include "UnitUSBBus.h"
#include "UnitLua.h"
#include "UnitSet.h"
#include "UnitLuaApi.h"
#include "UnitRTT.h"
#include "UnitChangeLog.h"
#include "bsp.h"
#include "bsp_udp_modbus.h"
#include "bsp_user_lib.h"
#include "jpeg.hpp"
#include <IniFiles.hpp>
#include "stdio.h"
#include <Registry.hpp>

#include "UnitThread.h"
#include "lua_if.h"
#include "file_lib.h"
#include "USB_HID.h"

//---------------------------------------------------------------------------
#pragma package(smart_init)

#pragma link "trayicon"
#pragma resource "*.dfm"



TForm1 *Form1;


DS0_CURSOR_MEAS_T g_tCursorMeas;

void Disp_Result(char* buf, int len);
void MODBUS_Send02H(uint8_t _addr, uint16_t _reg_addr, uint16_t _reg_num);
void MODBUS_Send03H(uint8_t _addr, uint16_t _reg_addr, uint16_t _reg_num);
void MODBUS_Send04H(uint8_t _addr, uint16_t _reg_addr, uint16_t _reg_num);
void MODBUS_Send06H(uint8_t _addr, uint16_t _reg_addr, uint16_t _reg_value);

uint8_t g_pBuf[BUFFER_SIZE];
int16_t g_pWaveBuf[BUFFER_SIZE];
uint16_t j;
uint16_t m, SMA_flag, flag;

static unsigned char RxBuf [1500 * 2];
static TByteDynArray g_buf;

AnsiString g_s;


char s_udp_print_RxBuf[UDP_PRINT_BUFFSIZE];
uint32_t  s_udp_print_RxLen = 0;

//---------------------------------------------------------------------------
__fastcall TForm1::TForm1(TComponent* Owner)
	: TForm(Owner)
{

}

//---------------------------------------------------------------------------
void __fastcall TForm1::FormCreate(TObject *Sender)
{
	// ��ȡini�����ļ�
	{
		TIniFile *ini;
		int radio;
		AnsiString s1;
		int temp;

		bsp_AddLog("Form1 Create Begin");

		s1 = AppExePath() + "\\app.ini";
		ini = new TIniFile(s1);

		// ����ini�ļ�
		g_tIniParam.CommInterface = ini->ReadInteger("Main", "CommInterface", 1);
		if (g_tIniParam.CommInterface < 0 || g_tIniParam.CommInterface > 1)
		{
			g_tIniParam.CommInterface = 1;
		}
		g_tIniParam.IPAddr = ini->ReadString("Main", "IPAddr", "192.168.1.211");
		g_tIniParam.UDPPort = ini->ReadInteger("Main", "UDPPort", 30010);
		g_tIniParam.SerialPortNo = ini->ReadInteger("Main", "SerialPortNo", 1);

		temp = ini->ReadInteger("Lua", "Wordwrap", 1);
		if (temp)
		{
			CheckBoxLuaWordWrap->Checked  = true;
		}
		else
		{
			CheckBoxLuaWordWrap->Checked = false;
		}

		g_tIniParam.ReadOk = 1;
		delete  ini;

		YbCommDevice1->PortNo = g_tIniParam.SerialPortNo;

		TimerPowerOn->Enabled = true;
	}

	g_buf = RawToBytes(RxBuf, 1500);    // �� unsigned char ����ת��Ϊ TByteDynArray ����ָ��

	mWaveMid = 0;
	mWaveHigh = 10;

	DacRefreshList(0);  	// ˢ��DAC�����б�

	/* ���ȱʡ�Ĳ������ݣ�CH1�����ң�CH2�Ƿ��� */
	{
		Series1->Clear();
		for (int i = 0; i < 1 * 1024; i++)
		{
			mArrayX[i] = i;
			mArrayY[i] = 5 * sin(((float)i / 256) * (2 * 3.14159));
		}
		Series1->AddArray(mArrayX, 1 * 1024, mArrayY, 1 * 1024);

		Series2->Clear();
		for (int i = 0; i < 1 * 1024; i++)
		{
			mArrayX[i] = i;
			if ((i % 100) < 50)
			{
				mArrayY[i] = -5.0;
			}
			else
			{
				mArrayY[i] = 5.0;
			}
		}
		Series2->AddArray(mArrayX, 1 * 1024, mArrayY, 1 * 1024);

		// ���ú����귶Χ
		Chart1->BottomAxis->SetMinMax(-100, 1200);
	}

	/* ���ȱʡ�Ĳ������ݣ�D0-D13 */
//	{
//		for (int m = 0; m < 14; m++)
//		{
//			Chart2->Series[m]->Clear();
//			for (int i = 0; i < 4 * 1024; i++)
//			{
//				mArrayX[i] = i;
//				if ((i % (2 + m)) == (m + 2) / 2)
//				{
//					mArrayY[i] = 0.2 + (13 - m) * 1.0;
//				}
//				else
//				{
//					mArrayY[i] = 0.8 + (13 - m) * 1.0;
//				}
//			}
//			Chart2->Series[m]->AddArray(mArrayX, 4 * 1024, mArrayY, 4 * 1024);
//		}
//		// ���ú����귶Χ
//		Chart2->LeftAxis->SetMinMax(0, 13.8);
//		Chart2->BottomAxis->SetMinMax(-100, 1200);
//	}

	Radio_CH1Gain->ItemIndex = 0;
	Radio_CH2Gain->ItemIndex = 0;

	// MemoLuaOut ����TAB = 4�� ȱʡ��8
	{
		int TabWidth = 4;  // new tab width (9 spaces)
		TabWidth = LOWORD(GetDialogBaseUnits()) * TabWidth * 0.5;

		SendMessage(MemoLuaOut->Handle, EM_SETTABSTOPS, (WPARAM)1, (LPARAM)&TabWidth);
	}

	{
		// ��ʼִ��
		lua_DeInit();
		lua_Init();
		lua_do("print(\"lua Init\")");
	}

	// ȱʡ��ʾʾ����
	{
		//PanelLua->Visible = false;
		PanelDSO->Align = alClient;
		PanelDSO->Visible = true;

		PanelLua->Align = alClient;
	}

	bsp_AddLog("Form1 Create End");

	Form1->TimerPowerOn->Enabled = true;

	Form1->LabelCheckVer->Caption = "";	/* ��������İ�ť */
	{
		char buf[128];

		sprintf(buf, "V%d.%d.%d (%s)", PC_SOFT_VER1, PC_SOFT_VER2, PC_SOFT_VER3,  PC_SOFT_DATE);

		LabelVer->Caption = "�汾:" + (AnsiString)buf;
	}

	// װ��dll
	LoadDll();
	FindDllFunc();

	HID_Init();
}
//---------------------------------------------------------------------------

void __fastcall TForm1::FormClose(TObject *Sender, TCloseAction &Action)
{
	HID_UnInit();

	FreeDll();

	// �˳�ʱдini�ļ�
	{
		TIniFile *ini;
		int radio;
		AnsiString s1;

		s1 = AppExePath() + "\\app.ini";
		ini = new TIniFile(s1);

		/* ��ȡ�ؼ����� */
		g_tIniParam.CommInterface = FormSet->ComboBoxCommMode->ItemIndex;
		g_tIniParam.SerialPortNo = YbCommDevice1->PortNo;

		g_tIniParam.IPAddr = FormSet->EditH7IPAddr->Text;
				
		// дini�ļ�
		ini->WriteInteger("Main", "CommInterface", g_tIniParam.CommInterface);
		ini->WriteString("Main", "IPAddr", g_tIniParam.IPAddr);
		ini->WriteInteger("Main", "UDPPort", g_tIniParam.UDPPort);
		ini->WriteInteger("Main", "SerialPortNo", g_tIniParam.SerialPortNo);

		ini->WriteString("Lua", "FileName1", FormLua->RichEditFileName->Text);
		ini->WriteString("Lua", "LuaCmd1",  FormLua->EditLua1->Text);
		ini->WriteString("Lua", "LuaCmd2",  FormLua->EditLua2->Text);
		ini->WriteString("Lua", "LuaCmd3",  FormLua->EditLua3->Text);
		ini->WriteString("Lua", "LuaCmd4",  FormLua->EditLua4->Text);
		ini->WriteString("Lua", "LuaCmd5",  FormLua->EditLua5->Text);
		ini->WriteString("Lua", "LuaCmd6",  FormLua->EditLua6->Text);
		ini->WriteString("Lua", "LuaCmd7",  FormLua->EditLua7->Text);
		ini->WriteString("Lua", "LuaCmd8",  FormLua->EditLua8->Text);
		ini->WriteString("Lua", "LuaCmd9",  FormLua->EditLua9->Text);
		ini->WriteString("Lua", "LuaCmd10",  FormLua->EditLua10->Text);
		ini->WriteString("Lua", "LuaCmd11",  FormLua->EditLua11->Text);
		ini->WriteString("Lua", "LuaCmd12",  FormLua->EditLua12->Text);
		ini->WriteString("Lua", "LuaCmd13",  FormLua->EditLua13->Text);
		ini->WriteString("Lua", "LuaCmd14",  FormLua->EditLua14->Text);
		ini->WriteString("Lua", "LuaCmd15",  FormLua->EditLua15->Text);
		ini->WriteString("Lua", "LuaCmd16",  FormLua->EditLua16->Text);

		ini->WriteString("Lua", "LuaNote1",  FormLua->EditLuaNote1->Text);
		ini->WriteString("Lua", "LuaNote2",  FormLua->EditLuaNote2->Text);
		ini->WriteString("Lua", "LuaNote3",  FormLua->EditLuaNote3->Text);
		ini->WriteString("Lua", "LuaNote4",  FormLua->EditLuaNote4->Text);
		ini->WriteString("Lua", "LuaNote5",  FormLua->EditLuaNote5->Text);
		ini->WriteString("Lua", "LuaNote6",  FormLua->EditLuaNote6->Text);
		ini->WriteString("Lua", "LuaNote7",  FormLua->EditLuaNote7->Text);
		ini->WriteString("Lua", "LuaNote8",  FormLua->EditLuaNote8->Text);
		ini->WriteString("Lua", "LuaNote9",  FormLua->EditLuaNote9->Text);
		ini->WriteString("Lua", "LuaNote10",  FormLua->EditLuaNote10->Text);
		ini->WriteString("Lua", "LuaNote11",  FormLua->EditLuaNote11->Text);
		ini->WriteString("Lua", "LuaNote12",  FormLua->EditLuaNote12->Text);
		ini->WriteString("Lua", "LuaNote13",  FormLua->EditLuaNote13->Text);
		ini->WriteString("Lua", "LuaNote14",  FormLua->EditLuaNote14->Text);
		ini->WriteString("Lua", "LuaNote15",  FormLua->EditLuaNote15->Text);
		ini->WriteString("Lua", "LuaNote16",  FormLua->EditLuaNote16->Text);

		if (CheckBoxLuaWordWrap->Checked)
		{
			ini->WriteInteger("Lua", "Wordwrap", 1);
		}
		else
		{
			ini->WriteInteger("Lua", "Wordwrap", 0);
		}

		delete  ini;

		FormLua->MemoLuaTemp1->Lines->SaveToFile(AppExePath() + "\\LuaUser1.lua");
		FormLua->MemoLuaTemp2->Lines->SaveToFile(AppExePath() + "\\LuaUser2.lua");
		FormLua->MemoLuaTemp3->Lines->SaveToFile(AppExePath() + "\\LuaUser3.lua");
		FormLua->MemoLuaTemp4->Lines->SaveToFile(AppExePath() + "\\LuaUser4.lua");
		FormLua->MemoLuaTemp5->Lines->SaveToFile(AppExePath() + "\\LuaUser5.lua");
		FormLua->MemoLuaTemp6->Lines->SaveToFile(AppExePath() + "\\LuaUser6.lua");
	}
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------

char __fastcall TForm1::AsciiToHex(char cHex)
{
  if ( cHex >= '0' && cHex <= '9' )
	return cHex - '0';

  if ( cHex >= 'a' && cHex <= 'f' )
	return 10 + cHex - 'a';

  if ( cHex >= 'A' && cHex <= 'F' )
	return 10 + cHex - 'A';

  return 0xFF;
}

int __fastcall TForm1::AnsiStrToChars(AnsiString strHex, char * pChar)
{
  unsigned char ucChar;
  int k = 0;
  int ucOdd = 0;
  int i;

  for (i = 0; i < strHex.Length(); i++)
  {
	ucChar = AsciiToHex(strHex.c_str()[i]);
	if (ucChar == 0xFF)
	{
		continue;
	}
	if (ucOdd == 0)
	{
		pChar[k] = ucChar << 4;
		ucOdd = 1;
	}
	else
	{
		pChar[k++] |= ucChar;
		ucOdd = 0;
	}
  }
  return k;
}

//---------------------------------------------------------------------------

//---------------------------------------------------------------------------


//---------------------------------------------------------------------------



//---------------------------------------------------------------------------

void __fastcall TForm1::SendTcpCmd(uint32_t ip, uint32_t tcp_port)
{

}

void __fastcall TForm1::SendUdpCmd(uint32_t ip, uint32_t port, uint8_t * cmd_buf, uint16_t cmd_len)
{
//	uint8_t *p;
//	AnsiString ip_str;
//
//	// ��32λ��IP��ַת��Ϊ�ַ�����ʽ
//	ip_str =  IntToStr((uint8_t)(ip >> 24)) + '.'
//		+ IntToStr((uint8_t)(ip >> 16)) + '.' + IntToStr((uint8_t)(ip >> 8)) +
//		'.' + IntToStr((uint8_t)ip);
//#if 0
//	LabelInfoBar->Caption = "UDPɨ�� " + ip_str;
//#endif
//	// ���͵�UDP�����
//	{
//		TByteDynArray buf;
//
//		buf = RawToBytes(cmd_buf, cmd_len);
//		UDP1->SendBuffer(ip_str, port, buf);
//	}
}

void __fastcall TForm1::SendUdpCmd2(AnsiString str_ip, uint32_t port, uint8_t * cmd_buf, uint16_t cmd_len)
{
//	// ���͵�UDP�����
//	{
//		TByteDynArray buf;
//
//		buf = RawToBytes(cmd_buf, cmd_len);
//		UDP1->SendBuffer(str_ip, port, buf);
//	}
}

// ����IP��ַ�б�



void __fastcall TForm1::FormCloseQuery(TObject *Sender, bool &CanClose)
{
//	// ֪ͨ�߳�ֹͣ���ʴ���
//	Form1->YbCommDevice1->Active = false;
//	FormTcp->Thread1->Stop();
//	FormTcp->Thread1->Tag = 1;	// ��ʾֹͣ
}
//---------------------------------------------------------------------------

int __fastcall TForm1::AutoRunCheck(bool value)
{
	int ret = 0;

	TRegistry * Reg = new TRegistry;
	AnsiString keyval = Application->ExeName;
	AnsiString key1 = "Software\\Microsoft\\Windows\\CurrentVersion\\Run\\";

	Reg->RootKey = HKEY_CURRENT_USER;
	if (!Reg->OpenKey(key1, false))
	{
		//Show msg
	}
	else
	{
		if (value)   // 1 true ��ʾ�Զ�����
		{
			AnsiString curvalstr = Reg->ReadString("GOsingGOTimer");
			if (curvalstr != keyval)
			{
				Reg->WriteString("GOsingGOTimer",keyval);
			}
			Reg->CloseKey();
			ret = 1;
		}
		else
		{
			if (Reg->DeleteValue("GOsingGOTimer"))
			{
				ret = 1;
			}
		}
	}
	delete Reg;
	return ret;
}

// ���ÿ����Զ����� -- ע��ɱ���������

// tcpɨ��

//---------------------------------------------------------------------------
void __fastcall TForm1::Memo_AddLine(uint8_t* buf, int len)
{

}

// UDPɨ��


// ���Ʋ���
void __fastcall TForm1::DrawWave1(void)
{
	float min, max;
	float sum = 0;
	float avg;
	double tdiv;

	//Series1->BeginUpdate();

	min = 65535;
	max = 0;
	for (int i = 0; i < SAMPLE_BUFF_SIZE; i++)
	{
		sum += FormTcp->mAdcBuf1[i];

		if (FormTcp->mAdcBuf1[i] > max)
		{
			max = FormTcp->mAdcBuf1[i];
		}
		if (FormTcp->mAdcBuf1[i] < min)
		{
			min = FormTcp->mAdcBuf1[i];
		}
	}
	avg = sum / SAMPLE_BUFF_SIZE;
	avg = max - min;

	{
		AnsiString s;

		s.sprintf("%0.3f", min);
		Form1->LabelMin1->Caption = s;
		s.sprintf("%0.3f", max);
		Form1->LabelMax1->Caption = s;
	}

	tdiv =  ((double)1000000 / SAMPLE_FREQ);
	for (int i = 0; i < SAMPLE_BUFF_SIZE; i++)
	{
		mArrayX[i] =  i * tdiv;
		mArrayY[i] =  FormTcp->mAdcBuf1[i];
	}
	Series1->BeginUpdate();
	Series1->Clear();
	Series1->RefreshSeries();
	//SAMPLE_BUFF_SIZE - 1, ��֪��Ϊ�β��λ����ʵһ���㣬��˼�1
	Series1->AddArray(mArrayX, SAMPLE_BUFF_SIZE - 1, mArrayY, SAMPLE_BUFF_SIZE - 1);
	Series1->EndUpdate();
}

// ���Ʋ���
void __fastcall TForm1::DrawWave2(void)
{
	float min, max;
	float sum = 0;
	float avg;
	int pos = 0;
	double tdiv;

	//Series2->BeginUpdate();
	// �����ҹ�������
	min = 65535;
	max = 0;
	for (int i = 0; i < SAMPLE_BUFF_SIZE; i++)
	{
		sum += FormTcp->mAdcBuf2[i];

		if (FormTcp->mAdcBuf2[i] > max)
		{
			max = FormTcp->mAdcBuf2[i];
		}
		if (FormTcp->mAdcBuf2[i] < min)
		{
			min = FormTcp->mAdcBuf2[i];
		}
	}
	avg = sum / SAMPLE_BUFF_SIZE;
	avg = max - min;

	pos = 0;

	{
		AnsiString s;

		s.sprintf("%0.3f", min);
		Form1->LabelMin2->Caption = s;
		s.sprintf("%0.3f", max);
		Form1->LabelMax2->Caption = s;
	}

	tdiv =  ((double)1000000 / SAMPLE_FREQ);
	for (int i = 0; i < SAMPLE_BUFF_SIZE; i++)
	{
		mArrayX[i] = i * tdiv;
		mArrayY[i] =  FormTcp->mAdcBuf2[i];
	}
	Series2->BeginUpdate();
	Series2->Clear();
	Series2->RefreshSeries();
	//SAMPLE_BUFF_SIZE - 1, ��֪��Ϊ�β��λ����ʵһ���㣬��˼�1
	Series2->AddArray(mArrayX, SAMPLE_BUFF_SIZE - 1, mArrayY, SAMPLE_BUFF_SIZE - 1);
	Series2->EndUpdate();
}

//--------------------------------------------------------------------------


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



void __fastcall TForm1::Button6Click(TObject *Sender)
{
	mWaveMid = 0;
	mWaveHigh = 10;

	Chart1->Axes[0].Left->Automatic = false;
	Chart1->Axes[0].Left->Minimum = mWaveMid - mWaveHigh ;
	Chart1->Axes[0].Left->Maximum = mWaveMid + mWaveHigh;
}
//---------------------------------------------------------------------------

void __fastcall TForm1::Button5Click(TObject *Sender)
{
	Chart1->Axes[0].Left->Automatic = true;
}
//---------------------------------------------------------------------------








void __fastcall TForm1::ButtonStartDSOClick(TObject *Sender)
{

//#define REG03_DSO_MEASURE_MODE	0x019F	/* 1��ʾ�������ģʽ */
//#define REG03_CH1_DC				0x0200	/* CH1ͨ����AC/DC����л� 1��ʾDC */
//#define REG03_CH2_DC				0x0201	/* CH2ͨ����AC/DC����л� */
//#define REG03_CH1_GAIN			0x0202	/* CH1ͨ���������л�0-3 */
//#define REG03_CH2_GAIN			0x0203	/* CH2ͨ���������л�0-3 */
//#define REG03_CH1_OFFSET			0x0204	/* CH1ͨ����ֱ��ƫֵ��0-2500mV) */
//#define REG03_CH2_OFFSET			0x0205	/* CH2ͨ����ֱ��ƫֵ��0-2500mV) */

//#define REG03_DSO_FREQ_ID			0x0206	/* ʾ��������Ƶ�ʵ�λ  */
//#define REG03_DSO_SAMPLE_SIZE		0x0207	/* ������ȵ�λ */

//#define REG03_DSO_TRIG_LEVEL		0x0208	/* ������ƽ��0-65535 */
//#define REG03_DSO_TRIG_POS			0x0209	/* ����λ�� 0-100 �ٷֱ� */
//#define REG03_DSO_TRIG_MODE			0x020A	/* ����ģʽ 0=�Զ� 1=��ͨ 2=���� */
//#define REG03_DSO_TRIG_CHAN			0x020B	/* ����ͨ�� 0=CH1  1=CH2*/
//#define REG03_DSO_TRIG_EDGE			0x020C	/* �������� 0�½� 1���� */
//#define REG03_DSO_CHAN_EN			0x020D	/* ͨ��ʹ�ܿ��� bit0 = CH1  bit1 = CH2  bit2 = CH2 - CH1 */
//#define REG03_DSO_RUN				0x020E	/* ʾ�����ɼ�����,0��ֹͣ 1: ���� */
	uint16_t reg_buf[256];
	uint8_t pos = 0;

	reg_buf[pos++] = ComboBoxDSO_Mode->ItemIndex; /* 1��ʾ�������ģʽ */
	reg_buf[pos++] = 1; /* CH1ͨ����AC/DC����л� 1��ʾDC */
	reg_buf[pos++] = 1; /* CH2ͨ����AC/DC����л� */
	reg_buf[pos++] = Radio_CH1Gain->ItemIndex; /* CH1ͨ���������л�0-3 */
	reg_buf[pos++] = Radio_CH1Gain->ItemIndex; /* CH2ͨ���������л�0-3 */

	reg_buf[pos++] = 0;   /* δ�� CH1ͨ����ֱ��ƫֵ��0-2500mV) */
	reg_buf[pos++] = 0;   /* δ�� CH2ͨ����ֱ��ƫֵ��0-2500mV) */
	reg_buf[pos++] = ComboBoxFreq->ItemIndex;  		/* ʾ��������Ƶ�ʵ�λ  */
	reg_buf[pos++] = ComboBoxSampleSize->ItemIndex; /* ������ȵ�λ */
	reg_buf[pos++] = ScrollBarTrigLevel->Position;   	/* ������ƽ��������ƽ 0-65535 */
	reg_buf[pos++] = ScrollBarTrigPos->Position;   	/* ������ƽ��������ƽ��ǧ�ֱ� */
	reg_buf[pos++] = ComboBoxTrigMode->ItemIndex;	/* ����ģʽ 0=�Զ� 1=��ͨ */
	reg_buf[pos++] = ComboBoxTrigChan->ItemIndex;   /* ����ͨ�� 0=�رգ�1=CH1  2=CH2*/
	reg_buf[pos++] = ComboBoxTrigEdge->ItemIndex;   /* �������� 0�½� 1���� */
	{
		uint16_t flag = 0;

		if (CheckBoxEnCH1->Checked) flag |= (1 << 0);
		if (CheckBoxEnCH2->Checked) flag |= (1 << 1);

		reg_buf[pos++] = flag;  	/* ͨ��ʹ�ܿ��� */
	}
	reg_buf[pos++] = 1;

	udp_Send10H(REG03_DSO_MEASURE_MODE, pos, reg_buf);

	CheckBoxAutoReadWave->Checked = true;	/* �Զ���ѡ�Զ�ˢ�°�ť */
}
//---------------------------------------------------------------------------





void __fastcall TForm1::Button20Click(TObject *Sender)
{
	FormCalib->Show();
}
//---------------------------------------------------------------------------

void __fastcall TForm1::TimerTCPTimer(TObject *Sender)
{
//	Series1->BeginUpdate();
//	Series2->BeginUpdate();

	// ����Ĵ����߼��е��ң����ǲ�����д��CH1��CH2���λᴮ�ţ���ɫ������˸��ԭ��δ֪
	if (CheckBoxEnCH1->Checked == true)
	{
		if (Series1->Active == false)
		{
			Series1->Active = true;
		}
	}
	else
	{
		if (Series1->Active == true)
		{
			Series1->Active = false;
		}
	}

	if (CheckBoxEnCH2->Checked == true)
	{
		if (Series2->Active == false)
		{
			Series2->Active = true;
		}
	}
	else
	{
		if (Series2->Active == true)
		{
			Series2->Active = false;
		}
	}

	if (CheckBoxEnCH1->Checked == true &&  CheckBoxEnCH2->Checked == true)
	{
		if (FormTcp->CanDrawWave2 == 1)
		{
			FormTcp->CanDrawWave2 = 0;
			DrawWave1();
			DrawWave2();
		}
		return;
	}

	if (CheckBoxEnCH1->Checked == true)
	{
		if (FormTcp->CanDrawWave2 == 1)
		{
			FormTcp->CanDrawWave2 = 0;
			DrawWave1();
		}
	}

	if (CheckBoxEnCH2->Checked == true)
	{
		if (FormTcp->CanDrawWave2 == 1)
		{
			FormTcp->CanDrawWave2 = 0;
			DrawWave2();
		}
	}

//	Series1->EndUpdate();
//	Series2->EndUpdate();
}
//---------------------------------------------------------------------------

void __fastcall TForm1::Thread1Run(TIdThreadComponent *Sender)
{
	//FormTcp->CommTask();
}
//---------------------------------------------------------------------------

void __fastcall TForm1::Button22Click(TObject *Sender)
{
	H7_WriteRegister(REG03_CH1_DC, 0);
}
//---------------------------------------------------------------------------

void __fastcall TForm1::Button23Click(TObject *Sender)
{
	H7_WriteRegister(REG03_CH1_DC, 1);
}
//---------------------------------------------------------------------------

void __fastcall TForm1::Button29Click(TObject *Sender)
{
	H7_WriteRegister(REG03_CH2_DC, 0);
}
//---------------------------------------------------------------------------

void __fastcall TForm1::Button24Click(TObject *Sender)
{
	H7_WriteRegister(REG03_CH2_DC, 1);
}
//---------------------------------------------------------------------------

void __fastcall TForm1::ScrollBarTrigLevelChange(TObject *Sender)
{
	LabelTrigLevel->Caption = IntToStr(ScrollBarTrigLevel->Position);
	ButtonStartDSOClick(this);
}
//---------------------------------------------------------------------------

void __fastcall TForm1::ComboBoxFreqClick(TObject *Sender)
{
	H7_WriteRegister(REG03_DSO_FREQ_ID, ComboBoxFreq->ItemIndex);
}
//---------------------------------------------------------------------------

void __fastcall TForm1::ComboBoxDSO_ModeChange(TObject *Sender)
{
	if (ComboBoxDSO_Mode->ItemIndex == 1)  // �߲����ģʽ
	{
		Radio_CH1Gain->Enabled = false;
		Radio_CH2Gain->Enabled = false;

		// ����������
		{
			double min, max;

			//����������
			if (ComboBoxCurrGain->ItemIndex == 0)   // 120mA����
			{
				min = -10;
				max = 150;
			}
			else  // 1.2A����
			{
				min = -10;
				max = 1500;
			}
			Form1->Chart1->LeftAxis->SetMinMax(min, max);
			Form1->Chart1->LeftAxis->Title->Caption = "(mA)";

			min = -2;
			max = 36;
			Form1->Chart1->RightAxis->SetMinMax(min, max);
		}
	}
	else      // ʾ�����Ͷ�ͨ��ģʽ
	{
		Radio_CH1Gain->Enabled = true;
		Radio_CH2Gain->Enabled = true;
		Form1->Chart1->LeftAxis->Title->Caption = "(V)";

		Radio_CH1GainClick(this);
		Sleep(10);
		Radio_CH2GainClick(this);
    }

	//H7_WriteRegister(REG03_DSO_MEASURE_MODE, ComboBoxDSO_Mode->ItemIndex);
	ButtonStartDSOClick(this);
}
//---------------------------------------------------------------------------

void __fastcall TForm1::ComboBoxCurrGainChange(TObject *Sender)
{
	if (ComboBoxCurrGain->ItemIndex == 0)
	{
		H7_WriteRegister(REG03_DSO_CURR_GAIN, 0);
	}
	else
	{
        H7_WriteRegister(REG03_DSO_CURR_GAIN, 1);
    }

	// ������ʾ����
	if (ComboBoxDSO_Mode->ItemIndex == 1)  // �߲����ģʽ
	{
		// ����������
		{
			double min, max;

			//����������
			if (ComboBoxCurrGain->ItemIndex == 0)   // 120mA����
			{
				min = -10;
				max = 150;
			}
			else  // 1.2A����
			{
				min = -10;
				max = 1500;
			}
			Form1->Chart1->LeftAxis->SetMinMax(min, max);
		}
	}
}
//---------------------------------------------------------------------------

void __fastcall TForm1::Button30Click(TObject *Sender)
{
	if (mWaveHigh > 0.2)
	{
		if (mWaveHigh <= 1)
		{
			mWaveHigh -= 0.1;
		}
		else if (mWaveHigh <= 10)
		{
			mWaveHigh -= 1;
		}
		else if (mWaveHigh <= 100)
		{
			mWaveHigh -= 10;
		}
		else if (mWaveHigh <= 1000)
		{
			mWaveHigh -= 100;
		}
		else
		{
			mWaveHigh -= 1000;
		}
	}

	Chart1->Axes[0].Left->Automatic = false;

	Chart1->Axes[0].Left->Minimum = -10000;
	Chart1->Axes[0].Left->Maximum = mWaveMid + mWaveHigh;
	Chart1->Axes[0].Left->Minimum = mWaveMid - mWaveHigh ;
}
//---------------------------------------------------------------------------

void __fastcall TForm1::Button31Click(TObject *Sender)
{
	if (mWaveHigh > 0)
	{
		if (mWaveHigh <= 1)
		{
			mWaveHigh += 0.1;
		}
		else if (mWaveHigh <= 10)
		{
			mWaveHigh += 1;
		}
		else if (mWaveHigh <= 100)
		{
			mWaveHigh += 10;
		}
		else if (mWaveHigh <= 1000)
		{
			mWaveHigh += 100;
		}
		else
		{
			mWaveHigh += 1000;
		}
	}

	Chart1->Axes[0].Left->Automatic = false;
	Chart1->Axes[0].Left->Maximum = mWaveMid + mWaveHigh;
	Chart1->Axes[0].Left->Minimum = mWaveMid - mWaveHigh ;

}

void __fastcall TForm1::DownloadLuaFile(String FileName)
{
	/* ���ļ�, ���ļ����� */
	{
		int iFileHandle;
		int iBytesRead;
		int iFileLength;

		iFileHandle = FileOpen(FileName, fmOpenRead);
		if (iFileHandle > 0)
		{
			iFileLength = FileSeek(iFileHandle, 0, 2);

			if (iFileLength <= 64 * 1024)
			{
				FileSeek(iFileHandle, 0, 0);
				FileRead(iFileHandle, mLuaText, iFileLength);
				mLuaLen = iFileLength;

				FileClose(iFileHandle);

				//FormLua->MemoLua->Lines->LoadFromFile(FormLua->RichEditFileName->Text);
			}
			else
			{
				FileClose(iFileHandle);
				ShowMessage("�ļ�����(>64K)");
			}
		}
		else
		{
			ShowMessage("�ļ���ʧ��" + FileName);
			return;
        }
	}

	FormTcp->mLua.Func = H64_LUA_RUN_WITH_RESET;		/* ��λlua��������ִ�� */
	FormTcp->mLua.Offset = 0;
	FormTcp->mLua.DataLen = mLuaLen;

	for (int i = 0; i < mLuaLen; i++)
	{
		FormTcp->mLua.DataBuf[i] = mLuaText[i];
	}

	g_tCmd.UserCmdStatus = CMDS_DOWNLOAD_LUA;
	g_tCmd.UserReq = 1;
}

//---------------------------------------------------------------------------
// ��ť ����lua����
void __fastcall TForm1::CH1_OffsetChange(TObject *Sender)
{
	double min, max;
	double AxisMin, AxisMax;
	double middle, range;

	AxisMin = Form1->Chart1->LeftAxis->Minimum;
	AxisMax = Form1->Chart1->LeftAxis->Maximum;

	range = AxisMax - AxisMin;

	min = -(range / 2) + (range * (CH1_Offset->Position - 50)) / 100;
	max = (range / 2) + (range * (CH1_Offset->Position - 50)) / 100;
	Form1->Chart1->LeftAxis->SetMinMax(min, max);
}
//---------------------------------------------------------------------------

void __fastcall TForm1::CH2_OffsetChange(TObject *Sender)
{
	double min, max;
	double AxisMin, AxisMax;
	double middle, range;

	AxisMin = Form1->Chart1->RightAxis->Minimum;
	AxisMax = Form1->Chart1->RightAxis->Maximum;

	range = AxisMax - AxisMin;

	min = -(range / 2) + (range * (CH2_Offset->Position - 50)) / 100;
	max = (range / 2) + (range * (CH2_Offset->Position - 50)) / 100;
	Form1->Chart1->RightAxis->SetMinMax(min, max);
}
//---------------------------------------------------------------------------

void __fastcall TForm1::ComboBoxSampleSizeChange(TObject *Sender)
{
	H7_WriteRegister(REG03_DSO_SAMPLE_SIZE, ComboBoxSampleSize->ItemIndex);
}
//---------------------------------------------------------------------------

void __fastcall TForm1::ScrollBarTrigPosChange(TObject *Sender)
{
	LabelTrigPos->Caption = IntToStr(ScrollBarTrigPos->Position);
	ButtonStartDSOClick(this);
}
//---------------------------------------------------------------------------

void __fastcall TForm1::DacRefreshList(int mode)
{
	#define SQR_WAVE_FREQ_TOP       100000    // �������Ƶ��
	#define SQR_WAVE_FREQ_BOTTOM    0           // ������СƵ��
	#define SQR_WAVE_FREQ_STEP      1000        // ����Ƶ�ʵ��ڲ���

	#define SQR_WAVE_COUNT_TOP       1000       // ����������
	#define SQR_WAVE_COUNT_BOTTOM    1           // ������С����
	#define SQR_WAVE_COUNT_STEP      1           // �����������ڲ���

	int VppMin, VppMax, VppStep, VppIdx;
	int OffMin, OffMax, OffStep, OffIdx;
	int WaveVoltMi1n;     	//��������ѹ
	int WaveVoltStep; 		//���ε�ѹ���ڲ���

	/* ��ѹģʽ */
	if (mode == 0)
	{
		VppMin = 0;
		VppMax = 20000;
		VppStep = 100;

		OffMin = -10000;
		OffMax = 10000;
		OffStep = 100;

		LabelDacVpp->Caption = "����Vpp(mV)��";
		LabelDacOff->Caption = "ֱ��ƫ��(mV)��";

		ComboBoxDacVpp->Text = 1000;
		ComboBoxDacOffset->Text = 0;

		VppIdx = 100;
		OffIdx = 100;

	}
	/* ����ģʽ */
	else
	{
		VppMin = 0;
		VppMax = 20000;
		VppStep = 100;

		OffMin = 0;
		OffMax = 20000;
		OffStep = 100;

		LabelDacVpp->Caption = "����Vpp(uA)��";
		LabelDacOff->Caption = "ֱ��ƫ��(uA)��";

		ComboBoxDacVpp->Text = 4000;
		ComboBoxDacOffset->Text = 0;

		VppIdx = 100;
		OffIdx = 0;

	}

	// ���vpp  offset�б�
	{
		ComboBoxDacVpp->Clear();
		for (int i = VppMin; i <= VppMax; i += VppStep)
		{
			ComboBoxDacVpp->Items->Add(IntToStr(i));
		}
		ComboBoxDacVpp->ItemIndex = VppIdx;

		ComboBoxDacOffset->Clear();
		for (int i = OffMin; i <= OffMax; i += OffStep)
		{
			ComboBoxDacOffset->Items->Add(IntToStr(i));
		}
		ComboBoxDacOffset->ItemIndex = OffIdx;
	}

	// ���Ƶ���б�
	{
		ComboBoxSqrFreq->Clear();

		for (int i = 0; i <= SQR_WAVE_FREQ_TOP; )
		{
			ComboBoxSqrFreq->Items->Add(IntToStr(i));

			if (i < 100)
			{
				i += 1;
			}
			else if (i < 1000)
			{
				i += 100;
			}
			else if (i < 10000)
            {
                i += 1000;
            }
            else if (i < 100000)
            {
                i += 10000;
            }
            else if (i < 1000000)
            {
                i += 100000;
            }
            else
            {
                i += 1000000;
			}
        }
		ComboBoxSqrFreq->ItemIndex = (SQR_WAVE_FREQ_TOP - SQR_WAVE_FREQ_BOTTOM) / SQR_WAVE_FREQ_STEP;
		ComboBoxSqrFreq->ItemIndex = 110;
	}

	//
	{
		/* 12M  */
		ComboBoxDuty->Clear();
		for (int i = 0; i <= 100; )
		{
			ComboBoxDuty->Items->Add(IntToStr(i));
			i++;
		}
		ComboBoxDuty->ItemIndex = 50;
	}

    // ��䷽�������б�
    #if 1
    ComboBoxSqrCount->ItemIndex = 0;
    #else
    {
        ComboBoxSqrCount->Clear();
        for (int i = SQR_WAVE_COUNT_BOTTOM; i <= SQR_WAVE_COUNT_TOP; )
        {
            ComboBoxSqrCount->Items->Add(IntToStr(i));
            if (i < 100)
            {
                i += 1;
            }
            else
            {
                i += 10;
            }
        }
        ComboBoxSqrCount->ItemIndex = (SQR_WAVE_COUNT_TOP - SQR_WAVE_COUNT_BOTTOM) / SQR_WAVE_COUNT_STEP;
        ComboBoxSqrCount->ItemIndex = 0;
    }
	#endif
}

void __fastcall TForm1::DacStartOutput(void)
{
	DAC_WAVE_T tDac;
	int vpp, offset,top,bottom;

	vpp = StrToInt(ComboBoxDacVpp->Text);
	offset = StrToInt(ComboBoxDacOffset->Text);
	top = vpp / 2 + offset;
	bottom = -vpp / 2 + offset;

	tDac.Type = ComboBoxWaveType->ItemIndex;
	tDac.VoltRange = 0;	/* �޹ز���������ĺ������ݵ�ѹ��Χ�Զ��ж�������� */
	tDac.VoltMax = top;
	tDac.VoltMin = bottom;
	tDac.Freq = StrToInt(ComboBoxSqrFreq->Text);
	tDac.Duty = StrToInt(ComboBoxDuty->Text);
	tDac.CycleCount = 0;   /* �޹ز��� */
	tDac.CycleSetting = 0;
	tDac.Run = 1;
	dac1_SetWave(&tDac);

	LabelDacState->Tag = 1;
	LabelDacState->Caption = "����Ѵ�";
}

void __fastcall TForm1::DacStopOutput(void)
{
	DAC_WAVE_T tDac;
	int vpp, offset,top,bottom;

	vpp = StrToInt(ComboBoxDacVpp->Text);
	offset = StrToInt(ComboBoxDacOffset->Text);
	top = vpp / 2 + offset;
	bottom = -vpp / 2 + offset;

	tDac.Type = ComboBoxWaveType->ItemIndex;
	tDac.VoltRange = 0;	/* �޹ز���������ĺ������ݵ�ѹ��Χ�Զ��ж�������� */
	tDac.VoltMax = top;
	tDac.VoltMin = bottom;
	tDac.Freq = StrToInt(ComboBoxSqrFreq->Text);
	tDac.Duty = StrToInt(ComboBoxDuty->Text);
	tDac.CycleCount = 0;   /* �޹ز��� */
	tDac.CycleSetting = 0;
	tDac.Run = 0;
	dac1_SetWave(&tDac);

	LabelDacState->Tag = 0;
	LabelDacState->Caption = "����ѹر�";
}

void __fastcall TForm1::RadioButtonVoltClick(TObject *Sender)
{
	DacRefreshList(0);		// DAC��ѹģʽ
}
//---------------------------------------------------------------------------

void __fastcall TForm1::RadioButtonCurrClick(TObject *Sender)
{
	DacRefreshList(1);		// DAC����ģʽ
}
//---------------------------------------------------------------------------
void __fastcall TForm1::ButtonSqrStartClick(TObject *Sender)
{
	DacStartOutput();

}
//---------------------------------------------------------------------------

void __fastcall TForm1::ButtonSqrStopClick(TObject *Sender)
{
	DacStopOutput();
}
//---------------------------------------------------------------------------

void __fastcall TForm1::ComboBoxWaveTypeChange(TObject *Sender)
{
	if (LabelDacState->Tag == 1)
	{
		DacStartOutput();
	}
}
//---------------------------------------------------------------------------

void __fastcall TForm1::TimerDacTempTimer(TObject *Sender)
{
	if (LabelDacState->Tag == 1)
	{
		DacStartOutput();
	}
}
//---------------------------------------------------------------------------



void __fastcall TForm1::ComboBoxTrigChanChange(TObject *Sender)
{
	ButtonStartDSOClick(this);
}
//---------------------------------------------------------------------------

void __fastcall TForm1::ComboBoxTrigModeChange(TObject *Sender)
{
	ButtonStartDSOClick(this);
}
//---------------------------------------------------------------------------

void __fastcall TForm1::ComboBoxTrigEdgeChange(TObject *Sender)
{
	ButtonStartDSOClick(this);
}
//---------------------------------------------------------------------------


void __fastcall TForm1::ComboBoxTVCCSelect(TObject *Sender)
{
	int16_t volt;
	float ff;

	if (ComboBoxTVCC->Text == "")
	{
		return;
	}

	ff = StrToFloat(ComboBoxTVCC->Text);
	volt = ff * 1000;
	H7_WriteRegister(REG03_OUT_TVCC_MV, volt);
}
//---------------------------------------------------------------------------



void __fastcall TForm1::ButtonDacVoltClick(TObject *Sender)
{
	TButton *btn;

	btn =  (TButton *)Sender;

	EditDacInput->Text = btn->Caption;

	ButtonDACOutClick(this);
}
//---------------------------------------------------------------------------

void __fastcall TForm1::ButtonDACOutClick(TObject *Sender)
{
	int mv;

	if (LabelDacState->Tag == 0 || ComboBoxWaveType->ItemIndex != 0)
	{
		//��������Ϊ0������DAC
		ComboBoxWaveType->ItemIndex = 0;
		DacStartOutput();
	}

	mv = MyStrToInt(EditDacInput->Text);
	H7_WriteRegister(REG03_OUT_VOLT_MV, mv);
}
//---------------------------------------------------------------------------

void __fastcall TForm1::ButtonDacCurrOutClick(TObject *Sender)
{
	int ua;

	if (LabelDacState->Tag == 0 || ComboBoxWaveType->ItemIndex != 0)
	{
		//��������Ϊ0������DAC
		ComboBoxWaveType->ItemIndex = 0;
		DacStartOutput();
	}

	ua = MyStrToInt(EditDacCurr->Text);
	H7_WriteRegister(REG03_OUT_CURR_UA, ua);
}
//---------------------------------------------------------------------------

void __fastcall TForm1::ButtonDacCurrClick(TObject *Sender)
{
	TButton *btn;

	btn =  (TButton *)Sender;

	EditDacCurr->Text = btn->Caption;

	ButtonDacCurrOutClick(this);
}
//---------------------------------------------------------------------------



void __fastcall TForm1::CheckBoxD7Click(TObject *Sender)
{
	if (CheckBoxD7->Checked)
	{
		H7_WriteRegister(REG03_D7_GPIO_MODE, ES_GPIO_OUT);
	}
	else
	{
		H7_WriteRegister(REG03_D7_GPIO_MODE, ES_GPIO_IN);
	}
}
//---------------------------------------------------------------------------

void __fastcall TForm1::CheckBoxD6Click(TObject *Sender)
{
	if (CheckBoxD6->Checked)
	{
		H7_WriteRegister(REG03_D6_GPIO_MODE, ES_GPIO_OUT);
	}
	else
	{
		H7_WriteRegister(REG03_D6_GPIO_MODE, ES_GPIO_IN);
	}
}
//---------------------------------------------------------------------------

void __fastcall TForm1::CheckBoxD5Click(TObject *Sender)
{
	if (CheckBoxD5->Checked)
	{
		H7_WriteRegister(REG03_D5_GPIO_MODE, ES_GPIO_OUT);
	}
	else
	{
		H7_WriteRegister(REG03_D5_GPIO_MODE, ES_GPIO_IN);
	}
}
//---------------------------------------------------------------------------

void __fastcall TForm1::CheckBoxD4Click(TObject *Sender)
{
	if (CheckBoxD4->Checked)
	{
		H7_WriteRegister(REG03_D4_GPIO_MODE, ES_GPIO_OUT);
	}
	else
	{
		H7_WriteRegister(REG03_D4_GPIO_MODE, ES_GPIO_IN);
	}
}
//---------------------------------------------------------------------------

void __fastcall TForm1::CheckBoxD3Click(TObject *Sender)
{
	if (CheckBoxD3->Checked)
	{
		H7_WriteRegister(REG03_D3_GPIO_MODE, ES_GPIO_OUT);
	}
	else
	{
		H7_WriteRegister(REG03_D3_GPIO_MODE, ES_GPIO_IN);
	}
}
//---------------------------------------------------------------------------

void __fastcall TForm1::CheckBoxD2Click(TObject *Sender)
{
	if (CheckBoxD2->Checked)
	{
		H7_WriteRegister(REG03_D2_GPIO_MODE, ES_GPIO_OUT);
	}
	else
	{
		H7_WriteRegister(REG03_D2_GPIO_MODE, ES_GPIO_IN);
	}
}
//---------------------------------------------------------------------------

void __fastcall TForm1::CheckBoxD1Click(TObject *Sender)
{
	if (CheckBoxD1->Checked)
	{
		H7_WriteRegister(REG03_D1_GPIO_MODE, ES_GPIO_OUT);
	}
	else
	{
		H7_WriteRegister(REG03_D1_GPIO_MODE, ES_GPIO_IN);
	}
}
//---------------------------------------------------------------------------

void __fastcall TForm1::CheckBoxD0Click(TObject *Sender)
{
	if (CheckBoxD0->Checked)
	{
		H7_WriteRegister(REG03_D0_GPIO_MODE, ES_GPIO_OUT);
	}
	else
	{
		H7_WriteRegister(REG03_D0_GPIO_MODE, ES_GPIO_IN);
	}
}
//---------------------------------------------------------------------------

void __fastcall TForm1::ShapeY7MouseUp(TObject *Sender, TMouseButton Button, TShiftState Shift,
		  int X, int Y)
{
	TShape *shape;
	int idx;
	AnsiString s;
	uint8_t buf[1];

	shape = (TShape *)Sender;
	s = shape->Name;
	idx = StrToInt(&s.c_str()[6]);

	if (Button == mbLeft)
	{
		buf[0] = 1;
		H7_WriteRelay(REG01_Y00 + idx, 1, buf);
	}
	else if (Button == mbRight)
	{
		buf[0] = 0;
		H7_WriteRelay(REG01_Y00 + idx, 1, buf);
	}
}
//---------------------------------------------------------------------------


void __fastcall TForm1::SpeedButtonY7Click(TObject *Sender)
{
	TSpeedButton *btn;
	int idx;
	AnsiString s;
	uint8_t buf[1];

	btn = (TSpeedButton *)Sender;
	s = btn->Name;
	idx = StrToInt(&s.c_str()[12]);

	if (btn->Tag == 0)
	{
		btn->Tag = 1;
		buf[0] = 1;
		H7_WriteRelay(REG01_Y00 + idx, 1, buf);
	}
	else
	{
		btn->Tag = 0;
		buf[0] = 0;
		H7_WriteRelay(REG01_Y00 + idx, 1, buf);
	}
}
//---------------------------------------------------------------------------

void __fastcall TForm1::CheckBoxD9Click(TObject *Sender)
{
	if (CheckBoxD9->Checked)
	{
		H7_WriteRegister(REG03_D9_GPIO_MODE, ES_GPIO_OUT);
	}
	else
	{
		H7_WriteRegister(REG03_D9_GPIO_MODE, ES_GPIO_IN);
	}
}
//---------------------------------------------------------------------------

void __fastcall TForm1::CheckBoxD8Click(TObject *Sender)
{
	if (CheckBoxD8->Checked)
	{
		H7_WriteRegister(REG03_D8_GPIO_MODE, ES_GPIO_OUT);
	}
	else
	{
		H7_WriteRegister(REG03_D8_GPIO_MODE, ES_GPIO_IN);
	}
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



void __fastcall TForm1::TimerUpgradeTimer(TObject *Sender)
{
	TimerUpgrade->Tag++;
}
//---------------------------------------------------------------------------



void __fastcall TForm1::Button107Click(TObject *Sender)
{
	MemoLuaOut->Lines->Clear();
}
//---------------------------------------------------------------------------

void __fastcall TForm1::ButtonStopDSOClick(TObject *Sender)
{
	udp_Send06H(REG03_DSO_RUN, 0);

	CheckBoxAutoReadWave->Checked = false;	/* �Զ�ȡ���Զ�ˢ�°�ť */
}
//---------------------------------------------------------------------------




void __fastcall TForm1::Chart1MouseMove(TObject *Sender, TShiftState Shift, int X,
          int Y)
{
	double tmpX, tmpY;
	AnsiString s;

	{
		Series1->GetCursorValues(tmpX, tmpY);  // <-- get values under mouse curs
		s.sprintf("%0.3f", tmpY);
		LabelCH1Now->Caption = s;

//		s.sprintf("%0.3f", tmpX);
//		LabelT1Now->Caption = s;
	}

	{
		Series2->GetCursorValues(tmpX, tmpY);  // <-- get values under mouse curs
		s.sprintf("%0.3f", tmpY);
		LabelCH2Now->Caption = s;
	}

	if (LabelT1->Tag == 1)    // ��ʾ���������״̬
	{
		Series3->Clear();
		Series3->AddXY(tmpX, -100, clRed);
		Series3->AddXY(tmpX, 100, clRed);
	}

	if (LabelT2->Tag == 1)    // ��ʾ���������״̬
	{
		Series4->Clear();
		Series4->AddXY(tmpX, -100, clRed);
		Series4->AddXY(tmpX, 100, clRed);
	}

//	if (LabelY1->Tag == 1)    // ��ʾ���������״̬
//	{
//		Series5->Clear();
//		Series5->AddXY(-100000, tmpY, clRed);
//		Series5->AddXY(100000, tmpY, clRed);
//	}
//
//	if (LabelY2->Tag == 1)    // ��ʾ���������״̬
//	{
//		Series6->Clear();
//		Series6->AddXY(-100000, tmpY, clRed);
//		Series6->AddXY(100000, tmpY, clRed);
//	}
}
//---------------------------------------------------------------------------


void __fastcall TForm1::Chart1MouseDown(TObject *Sender, TMouseButton Button, TShiftState Shift,
          int X, int Y)
{
	double tmpX1, tmpY1;
	double tmpX2, tmpY2;
	AnsiString s;

	// mbLeft, mbRight, mbMiddle)

	// ����м����£���¼���λ��
	if (Button == mbLeft)
	{
		Series1->GetCursorValues(tmpX1, tmpY1);
		Series2->GetCursorValues(tmpX2, tmpY2);
		// ��¼��1����
		g_tCursorMeas.State = 1;
		g_tCursorMeas.T1 = tmpX1;
		g_tCursorMeas.Ch1Y1 = tmpY1;
		g_tCursorMeas.Ch2Y1 = tmpY2;

		s.sprintf("%0.3f", tmpX1);
		LabelT1Now->Caption = s;

//		Series3->Clear();
//		Series3->AddXY(tmpX1, tmpY1, clRed);
//
//		Chart1->Axes[0].Bottom->Automatic = false;
//
//		Chart1->Axes[0].Bottom->Minimum = 0;
//		Chart1->Axes[0].Bottom->Maximum = 1024;
	}

	if (Button == mbRight)
	{
		Series1->GetCursorValues(tmpX1, tmpY1);
		Series2->GetCursorValues(tmpX2, tmpY2);
		// ��¼��2����
		g_tCursorMeas.State = 0;
		g_tCursorMeas.T2 = tmpX1;
		g_tCursorMeas.Ch1Y2 = tmpY1;
		g_tCursorMeas.Ch2Y2 = tmpY2;

		s.sprintf("%0.3f", tmpX1);
		LabelT2Now->Caption = s;
	}

	if (ComboBoxDSO_Mode->ItemIndex == 0)  // ʾ����
	{
		LabelCh1Diff->Caption = s.sprintf("%0.3fV", g_tCursorMeas.Ch1Y1-g_tCursorMeas.Ch1Y2);
		LabelCh2Diff->Caption = s.sprintf("%0.3fV", g_tCursorMeas.Ch2Y1-g_tCursorMeas.Ch2Y2);
	}
	else if (ComboBoxDSO_Mode->ItemIndex == 1) // �߲����
	{
		LabelCh1Diff->Caption = s.sprintf("%0.3fmA", g_tCursorMeas.Ch1Y1-g_tCursorMeas.Ch1Y2);
		LabelCh2Diff->Caption = s.sprintf("%0.3fV", g_tCursorMeas.Ch2Y1-g_tCursorMeas.Ch2Y2);
	}

	tmpX1 = g_tCursorMeas.T2 - g_tCursorMeas.T1;
	if (tmpX1 < 0) {
		tmpX1 = -tmpX1;
	}
	LabelT2T1Diff->Caption = s.sprintf("%0.3fus", tmpX1);

	if (tmpX1 < 0.001f)
	{
		LabelT2T1Freq->Caption = s.sprintf("%0.3fHz", 0);
	}
	else
	{
		float freq;

		freq =  1000000 / (tmpX1);
		if (freq > 100 * 1000000)
		{
			freq = 0;
		}
		LabelT2T1Freq->Caption = s.sprintf("%0.3fHz", freq);
    }

}
//---------------------------------------------------------------------------


void __fastcall TForm1::LabelT1Click(TObject *Sender)
{
	double tmpX, tmpY;

	Series1->GetCursorValues(tmpX, tmpY);

	Series3->Clear();
	Series3->AddXY(tmpX, -100, clRed);
	Series3->AddXY(tmpX, 100, clRed);

	LabelT1->Tag = 1;  // ��ʾ���������״̬
}
//---------------------------------------------------------------------------

void __fastcall TForm1::LabelT2Click(TObject *Sender)
{
	double tmpX, tmpY;

	Series1->GetCursorValues(tmpX, tmpY);

	Series4->Clear();
	Series4->AddXY(tmpX, -100, clRed);
	Series4->AddXY(tmpX, 100, clRed);

	LabelT2->Tag = 1;  // ��ʾ���������״̬
}
//---------------------------------------------------------------------------

void __fastcall TForm1::LabelY1Click(TObject *Sender)
{
	double tmpX, tmpY;

	Series1->GetCursorValues(tmpX, tmpY);

	Series5->Clear();
	Series5->AddXY(-100000, tmpY, clRed);
	Series5->AddXY(100000, tmpY, clRed);

//	LabelY1->Tag = 1;  // ��ʾ���������״̬
}
//---------------------------------------------------------------------------

void __fastcall TForm1::LabelY2Click(TObject *Sender)
{
	double tmpX, tmpY;

	Series1->GetCursorValues(tmpX, tmpY);

	Series6->Clear();
	Series6->AddXY(-100000, tmpY, clRed);
	Series6->AddXY(100000, tmpY, clRed);

//	LabelY2->Tag = 1;  // ��ʾ���������״̬
}
//---------------------------------------------------------------------------

// ���ڼ�������
void SerialPortMonitor(void)
{

}

// ö��ϵͳ���ô���
void __fastcall TForm1::EnumCom(void)
{
	// ����ע������
	std::auto_ptr<TRegistry> Registry(new TRegistry(KEY_READ));

	try
	{
		// ע����·��
		Registry->RootKey = HKEY_LOCAL_MACHINE;

		// �򿪴����豸���ڵ�ע���·��
		bool bResult = Registry->OpenKey("\\HARDWARE\\DEVICEMAP\\SERIALCOMM", false);

		// ����ע���ʧ�ܣ���ʾ��Ϣ��ֱ�ӷ���
		if (!bResult)
		{
			MemoLuaOut->Lines->Add("ע���·��������.");
			return;
        }

		// ������(����)�б����Ǵ������������б�
		std::auto_ptr<TStringList> ItemNameList(new TStringList());

		// ��ȡ������(����)�б����Ǵ������������б�
		Registry->GetValueNames(ItemNameList.get());

		// ��������������ƺ�ֵ (�ַ�����ʽ)��ֵ���ַ�����ʽ���Ǵ����߼����ƣ�����COM1, COM2 ֮���
		UnicodeString sItemName, sItemValue;

		// �����ȡ������ֵ (�ַ�����ʽ)
		for (int i = 0; i < ItemNameList->Count; i++)
		{
			// ��ǰ���������ƣ�Ҳ���Ǵ�����������
			sItemName = ItemNameList->Strings[i];

			// ��ȡ��Ӧ��������ֵ (�ַ�����ʽ)��Ҳ���Ǵ����߼�����
			sItemValue = Registry->ReadString(sItemName);

			// ��ʾ�����������ƺʹ����߼�����
			//ShowHint(sItemName + "\t" + sItemValue);
			// sg �� TStringGrid ����
			MemoLuaOut->Lines->Add(sItemName + "  " + sItemValue);
		}

		// �رջ����д�����
		//Registry->CloseKey();
	}
	// �ڶ�ȡע���ʱ�����쳣
	catch(...) {
		MemoLuaOut->Lines->Add("�ڶ�ȡע���ʱ�����쳣.");
	}
}

void __fastcall TForm1::TimerPowerOnTimer(TObject *Sender)
{
	static uint8_t s_retry = 0;
	//return;      // ��ʱ������USB���ڰβ�ļ໤���� ��WIN7�´�������

	if (CheckBoxMonitorUSB->Checked == false)
	{
		return;
	}

	//if (g_tCmd.CmdStatus > 0 || g_tCmd.UserReq == 1)
	if (g_tCmd.CmdStatus > 0)
	{
		return;
	}

	if (g_tIniParam.CommInterface == 0)     //��̫��
	{
		switch (mLinkStatus)
		{
			case 0:
				TimerPowerOn->Interval = 1000;
				//Form1->LabelLink->Caption = "...";
				mLinkStatus++;
				s_retry = 0;
				break;

			case 1:
				LabelRJ45Name->Caption = "RJ45 (" + g_tIniParam.IPAddr +  ")";
				LabelLedUSB->Font->Color = clGray;  // ��̫��ʱ����ֹUSB

				if (g_tCmd.CmdStatus == 0)
				{
					/* ֪ͨ�߳̿�ʼ���� */
					g_tCmd.UserCmdStatus = CMDS_SEND_03H;
					g_tCmd.UserRegAddr = REG03_BOOT_HARD_VER;	/*  REG03_HARD_MODEL + REG03_APP_VER  */
					g_tCmd.UserRegNum = 1;
					g_tCmd.UserReq = 1;
					TimerPowerOn->Interval = 100;
					mLinkStatus++;
					s_retry = 0;
				}
				else
				{
					TimerPowerOn->Interval = 1000;
				}
				break;

			case 2:
				if (g_tCmd.RxOk == 1)
				{
					//Form1->LabelLink->Caption = "���ӳɹ�. H7-TOOL IP��ַ: " + g_tIniParam.IPAddr;
					mLinkStatus++;
					TimerPowerOn->Interval = 1000;

					LabelLedRJ45->Font->Color = clGreen;
				}
				else //if (g_tCmd.RxOk == 2)
				{
					if (++s_retry > 10)
					{
						//Form1->LabelLink->Caption = "��̫������ʧ��";
						TimerPowerOn->Interval = 500;
						mLinkStatus = 0;
						LabelLedRJ45->Font->Color = clGray;
					}
				}
				break;

		   case 3:
				TimerPowerOn->Interval = 1;
				mLinkStatus = 1;
				break;
		}
	}
	else if (g_tIniParam.CommInterface == 1) 	// USB
	{
		switch (mLinkStatus)
		{
			case 0:
				TimerPowerOn->Interval = 1000;
				//Form1->LabelLink->Caption = "...";
				mLinkStatus++;

				try
				{
					YbCommDevice1->CleanupInstance();

					YbCommDevice1->Active = false;
					YbCommDevice1->Active = true;
				}
				catch (Exception &e)
				{
					//ShowMessage(""+e.Message);
				}
				break;

			case 1:
				LabelUSBName->Caption = "USB (COM" + IntToStr(YbCommDevice1->PortNo) +  ")";

				LabelLedRJ45->Font->Color = clGray;   // USBʱ����ֹ��̫��

				if (g_tCmd.CmdStatus == 0)
				{
					/* ֪ͨ�߳̿�ʼ���� */
					g_tCmd.UserCmdStatus = CMDS_SEND_03H;
					g_tCmd.UserRegAddr = REG03_BOOT_HARD_VER;	/*  REG03_HARD_MODEL + REG03_APP_VER  */
					g_tCmd.UserRegNum = 1;
					g_tCmd.UserReq = 1;
					TimerPowerOn->Interval = 100;
					mLinkStatus++;
					s_retry = 0;
				}
				else
				{
					TimerPowerOn->Interval = 1000;
				}
				break;

			case 2:
				if (g_tCmd.RxOk == 1)
				{
					//Form1->LabelLink->Caption = "���ӳɹ�. H7-TOOL IP��ַ: " + g_tIniParam.IPAddr;
					mLinkStatus++;
					TimerPowerOn->Interval = 1000;

					LabelLedUSB->Font->Color = clGreen;
				}
				else //if (g_tCmd.RxOk == 2)
				{
					if (++s_retry > 10)
					{
						//Form1->LabelLink->Caption = "��̫������ʧ��";
						TimerPowerOn->Interval = 500;
						mLinkStatus = 0;
						LabelLedUSB->Font->Color = clGray;
					}
				}
				break;

		   case 3:
				TimerPowerOn->Interval = 1;
				mLinkStatus = 1;
				break;

		   case 10:
				try
				{
					Form1->MemoLuaOut->Lines->Add("�ر�USB����..");

					YbCommDevice1->CleanupInstance();

					YbCommDevice1->Active = false;

					Sleep(100);

					YbCommDevice1->Active = true;

					Form1->MemoLuaOut->Lines->Add("���´�USB����.");

					mLinkStatus = 1;
				}
				catch (Exception &e)
				{
					Form1->MemoLuaOut->Lines->Add(""+e.Message);
				}
				break;
		}
	}
}
//---------------------------------------------------------------------------


void __fastcall TForm1::YbCommDevice1Package(TObject *Sender, int NotifyType)
{
   //	FormCalib->Memo1->Lines->Add(IntToStr(NotifyType));
}
//---------------------------------------------------------------------------

void __fastcall TForm1::Radio_CH1GainClick(TObject *Sender)
{
	float TabelRange[] = {
			14.0,
			6.4,
			3.2,
			1.6,
			0.8,
			0.4,
			0.2,
			0.1,
	};

	if (Radio_CH1Gain->ItemIndex < 0 || Radio_CH1Gain->ItemIndex > 7)
	{
		Radio_CH1Gain->ItemIndex = 0;
	}
	H7_WriteRegister(REG03_CH1_GAIN, Radio_CH1Gain->ItemIndex);

	// ����CH1������
	{
		double min, max;
		double range;
		range = 2 * TabelRange[Radio_CH1Gain->ItemIndex];
		min = -(range / 2) + (range * (CH1_Offset->Position - 50)) / 100;
		max = (range / 2) + (range * (CH1_Offset->Position - 50)) / 100;
		Form1->Chart1->LeftAxis->SetMinMax(min, max);
	}
}
//---------------------------------------------------------------------------

void __fastcall TForm1::Radio_CH2GainClick(TObject *Sender)
{
	float TabelRange[] = {
			14.0,
			6.4,
			3.2,
			1.6,
			0.8,
			0.4,
			0.2,
			0.1,
	};

	if (Radio_CH2Gain->ItemIndex < 0 || Radio_CH2Gain->ItemIndex > 7)
	{
		Radio_CH2Gain->ItemIndex = 0;
	}
	H7_WriteRegister(REG03_CH2_GAIN, Radio_CH2Gain->ItemIndex);

	// ����CH1������
	{
		double min, max;
		double range;
		range = 2 * TabelRange[Radio_CH2Gain->ItemIndex];
		min = -(range / 2) + (range * (CH2_Offset->Position - 50)) / 100;
		max = (range / 2) + (range * (CH2_Offset->Position - 50)) / 100;
		Form1->Chart1->RightAxis->SetMinMax(min, max);
	}
}
//---------------------------------------------------------------------------



void __fastcall TForm1::Button27Click(TObject *Sender)
{
	MemoLuaOut->Lines->Clear();
	s_udp_print_RxLen = 0;
}
//---------------------------------------------------------------------------


// ��ʾUDP 777���յ�����  lua������
void DispUdpPringData(void)
{
	AnsiString s, s0;
	int count;
	char end_char;
	int i;
	int k;
	char *p;
	int fRem = 0;

	if (s_udp_print_RxLen == 0)
	{
		return;
	}

	p = s_udp_print_RxBuf;
	k = 0;
	for (i = 0; i < s_udp_print_RxLen; i++)
	{
		if (s_udp_print_RxBuf[i] == 0x0A)
		{
			s_udp_print_RxBuf[i] = 0;
			if (i > 0)
			{
				if (s_udp_print_RxBuf[i - 1] == 0x0D)
				{
                	s_udp_print_RxBuf[i - 1] = 0;
				}
			}
			s = (AnsiString)p;
			Form1->MemoLuaOut->Lines->Add(s);

			p = &s_udp_print_RxBuf[i + 1];

			if (i == s_udp_print_RxLen - 1)
			{
				fRem = 0;

				Form1->Tag = 1;     // �л���
			}
		}
		else
		{
			fRem = 1;
		}
	}

	if (fRem == 1)
	{
		s_udp_print_RxBuf[s_udp_print_RxLen] = 0;
		s = (AnsiString)p;
		if (Form1->Tag == 1)  // �л���
		{
			Form1->MemoLuaOut->Lines->Add(s);
			Form1->Tag = 0;
		}
		else
		{
			count = Form1->MemoLuaOut->Lines->Count;
			s0 = Form1->MemoLuaOut->Lines->Strings[count - 1];
			Form1->MemoLuaOut->Lines->BeginUpdate();

			Form1->MemoLuaOut->Lines->Delete(count - 1);
			Form1->MemoLuaOut->Lines->Add(s0 + s);
			// ���������BeginUpdate,��ô���������Զ�������β,���ʹ����BeginUpdate,��ô����������ù�����
			SendMessage(Form1->MemoLuaOut->Handle, EM_LINESCROLL,0,  count);
			Form1->MemoLuaOut->Lines->EndUpdate();
		}
	}

	s_udp_print_RxLen = 0;
}

void __fastcall TForm1::IdUDPServer1UDPRead(TIdUDPListenerThread *AThread, const TIdBytes AData,
          TIdSocketHandle *ABinding)
{
#if 1
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
	func = AData[pos++];
	ch = AData[pos++];
	len = AData[pos] * 256 + AData[pos + 1];
	pos += 2;

	for (int i = 0; i < len; i++)
	{
		if (s_udp_print_RxLen < UDP_PRINT_BUFFSIZE)
		{
			s_udp_print_RxBuf[s_udp_print_RxLen++] = AData[pos++];
		}
	}

	DispUdpPringData();
#else
	for (int i = 0; i < AData.Length; i++)
	{
		if (s_udp_print_RxLen < UDP_PRINT_BUFFSIZE)
		{
			s_udp_print_RxBuf[s_udp_print_RxLen++] = AData[i];
		}
	}

	DispUdpPringData();
#endif

}
//---------------------------------------------------------------------------


void __fastcall TForm1::CheckBoxUDP777Click(TObject *Sender)
{
	if (CheckBoxUDP777->Checked)
	{
		TIdSocketHandle *sh;

		IdUDPServer1->Bindings->Clear();
		sh = IdUDPServer1->Bindings->Add();
		sh->IP = "0.0.0.0";
		sh->Port = 777;

		IdUDPServer1->Active = true;
		Form1->MemoLuaOut->Lines->Add("UDP 777�����˿��Ѵ�");
	}
	else
	{
		IdUDPServer1->Active = false;
		Form1->MemoLuaOut->Lines->Add("UDP 777�����˿��ѹر�");
	}

}
//---------------------------------------------------------------------------


void __fastcall TForm1::N9Click(TObject *Sender)
{
	ShellExecute(Handle,L"open",
		L"http://www.armbbs.cn/forum.php?mod=viewthread&tid=95373&extra=page%3D1",
		NULL,NULL,SW_SHOWMAXIMIZED);
}
//---------------------------------------------------------------------------

void __fastcall TForm1::N10Click(TObject *Sender)
{

	ShellExecute(Handle,L"open",
		L"https://gitee.com/armfly",
		NULL,NULL,SW_SHOWMAXIMIZED);
}
//---------------------------------------------------------------------------




void __fastcall TForm1::CheckBoxLuaWordWrapClick(TObject *Sender)
{
	if (CheckBoxLuaWordWrap->Checked)
	{
		Form1->MemoLuaOut->WordWrap = true;
	}
	else
	{
		Form1->MemoLuaOut->WordWrap = false;
	}
}
//---------------------------------------------------------------------------

void __fastcall TForm1::MemoLuaOutClick(TObject *Sender)
{
	//PostMessage(MemoLuaOut->Handle, WM_VSCROLL, SB_BOTTOM, 0);
}
//---------------------------------------------------------------------------




void __fastcall TForm1::PanelStationResize(TObject *Sender)
{
	if (FormTcp->Thread1->Tag == 1)
	{
		return;
	}

	ChangeFace(mActiveFace);

	{
		AnsiString s;

		s = mActiveFace;
		bsp_AddLog(s.c_str());
	}
}
//---------------------------------------------------------------------------
void __fastcall TForm1::ChangeFace(String name)
{
#if 1

	mActiveFace = name;

	if (name == "DSO")
	{
		PanelLua->Hide();
		PanelDSO->Show();

		FormLua->Hide();
		FormUSBBus->Hide();
		FormProg->Hide();
		FormSet->Hide();
		FormRTT->Hide();
	}
	else if (name == "USBBus")
	{
		if (PanelDSO->Visible) PanelDSO->Hide();
		if (PanelLua->Visible == false) PanelLua->Show();

		FormLua->Hide();
		FormProg->Hide();
		FormSet->Hide();
		FormRTT->Hide();

		LockWindowUpdate(FormUSBBus->Handle);	// ��ʱ��ס���ڲ���ͼ
		FormUSBBus->Show();
		{
			FormUSBBus->Left = Form1->PanelLua->Left + Form1->PanelStation->Left;
			FormUSBBus->Top = Form1->PanelLua->Top + Form1->PanelStation->Top;
			FormUSBBus->Height = Form1->PanelStation->Height;
			FormUSBBus->Width = Form1->PanelStation->Width;
		}
		LockWindowUpdate(0);	//��ʼ�ػ���ס�Ĵ���
	}
	else if (name == "Lua")
	{
		if (PanelDSO->Visible) PanelDSO->Hide();
		if (PanelLua->Visible == false) PanelLua->Show();

		//FormLua->Show();
		if (FormUSBBus->Visible) FormUSBBus->Hide();
		if (FormProg->Visible) FormProg->Hide();
		if (FormSet->Visible) FormSet->Hide();
		FormRTT->Hide();

		LockWindowUpdate(FormLua->Handle);	// ��ʱ��ס���ڲ���ͼ
		FormLua->Show();
		{
			FormLua->Parent = Form1;
			FormLua->Left = Form1->PanelLua->Left + Form1->PanelStation->Left;
			FormLua->Top = Form1->PanelLua->Top + Form1->PanelStation->Top;
			FormLua->Height = Form1->PanelStation->Height;
			FormLua->Width = Form1->PanelStation->Width;
		}
		LockWindowUpdate(0);	//��ʼ�ػ���ס�Ĵ���
	}
	else if (name == "Programmer")
	{
		if (PanelDSO->Visible) PanelDSO->Hide();
		if (PanelLua->Visible == false) PanelLua->Show();

		FormLua->Hide();
		FormUSBBus->Hide();
		FormSet->Hide();
		FormRTT->Hide();

		//�������ڴ�С
		LockWindowUpdate(FormProg->Handle);	// ��ʱ��ס���ڲ���ͼ
		FormProg->Show();
		{
			FormProg->Left = Form1->PanelLua->Left + Form1->PanelStation->Left;
			FormProg->Top = Form1->PanelLua->Top + Form1->PanelStation->Top;
			FormProg->Height = Form1->PanelStation->Height;
			FormProg->Width = Form1->PanelStation->Width;
		}
		LockWindowUpdate(0);	//��ʼ�ػ���ס�Ĵ���
	}
	else if (name == "Setting")
	{
		if (PanelDSO->Visible) PanelDSO->Hide();
		if (PanelLua->Visible == false) PanelLua->Show();

		FormLua->Hide();
		FormUSBBus->Hide();
		FormProg->Hide();
		FormRTT->Hide();

		LockWindowUpdate(FormSet->Handle);	// ��ʱ��ס���ڲ���ͼ
		FormSet->Show();
		{
			FormSet->Left = Form1->PanelLua->Left + Form1->PanelStation->Left;
			FormSet->Top = Form1->PanelLua->Top + Form1->PanelStation->Top;
			FormSet->Height = Form1->PanelStation->Height;
			FormSet->Width = Form1->PanelStation->Width;
		}
		LockWindowUpdate(0);	//��ʼ�ػ���ס�Ĵ���
	}
	else if (name == "RTTViewer")
	{
		if (PanelDSO->Visible) PanelDSO->Hide();
		if (PanelLua->Visible == false) PanelLua->Show();

		FormLua->Hide();
		FormUSBBus->Hide();
		FormProg->Hide();
		FormSet->Hide();

		LockWindowUpdate(FormRTT->Handle);	// ��ʱ��ס���ڲ���ͼ
		FormRTT->Show();
		{
			FormRTT->Left = Form1->PanelLua->Left + Form1->PanelStation->Left;
			FormRTT->Top = Form1->PanelLua->Top + Form1->PanelStation->Top;
			FormRTT->Height = Form1->PanelStation->Height;
			FormRTT->Width = Form1->PanelStation->Width;
		}
		LockWindowUpdate(0);	//��ʼ�ػ���ס�Ĵ���
	}
#endif
}

void __fastcall TForm1::Button86Click(TObject *Sender)
{
	ChangeFace("DSO");
}
//---------------------------------------------------------------------------

void __fastcall TForm1::Button108Click(TObject *Sender)
{
	ChangeFace("USBBus");
}
//---------------------------------------------------------------------------

void __fastcall TForm1::Button109Click(TObject *Sender)
{
	ChangeFace("Lua");
}

void __fastcall TForm1::Button64Click(TObject *Sender)
{
	ChangeFace("Programmer");
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------

void __fastcall TForm1::Button9Click(TObject *Sender)
{
	ChangeFace("Setting");
}
//---------------------------------------------------------------------------

void __fastcall TForm1::Button8Click(TObject *Sender)
{
	ChangeFace("RTTViewer");
}
//---------------------------------------------------------------------------


void __fastcall TForm1::Button1Click(TObject *Sender)
{
	MemoLuaOut->Clear();
	MemoLuaOut->Lines->Add(bsp_GetLog());
}

void __fastcall TForm1::MemoAdd(AnsiString str)
{
	MemoLuaOut->Lines->Add(str);
}

//---------------------------------------------------------------------------

void __fastcall TForm1::Button2Click(TObject *Sender)
{
	if (FormLuaApi->Visible == false)
	{
		FormLuaApi->Show();
	}
	else
	{
		FormLuaApi->Hide();
	}
}
//---------------------------------------------------------------------------

void __fastcall TForm1::Button3Click(TObject *Sender)
{
	EnumCom();
}
//---------------------------------------------------------------------------

void __fastcall TForm1::LabelLedUSBClick(TObject *Sender)
{
	Form1->YbCommDevice1->SettingsDialog(this,true);

	if (Form1->YbCommDevice1->Active == true)
	{
		FormSet->ComboBoxCommMode->ItemIndex = 1;
		g_tIniParam.CommInterface = FormSet->ComboBoxCommMode->ItemIndex;

		Form1->MemoLuaOut->Lines->Add("USB����״̬����Ѵ�");
		CheckBoxMonitorUSB->Checked = true;
	}
	else
	{
		//FormSet->ComboBoxCommMode->ItemIndex = 1;
		//g_tIniParam.CommInterface = FormSet->ComboBoxCommMode->ItemIndex;
	}
}
//---------------------------------------------------------------------------

void __fastcall TForm1::LabelLedRJ45Click(TObject *Sender)
{
	FormSet->ComboBoxCommMode->ItemIndex = 0;
	g_tIniParam.CommInterface = FormSet->ComboBoxCommMode->ItemIndex;
}
//---------------------------------------------------------------------------



void __fastcall TForm1::N2Click(TObject *Sender)
{
	String s;

	s =  AppExePath() + "\\Help\\sch";
	ShellExecute(Handle, NULL, s.c_str(), NULL,NULL,SW_SHOWNORMAL);
}
//---------------------------------------------------------------------------

void __fastcall TForm1::CheckBoxMonitorUSBClick(TObject *Sender)
{
	if (CheckBoxMonitorUSB->Checked == false)
	{
		Form1->MemoLuaOut->Lines->Add("USB����״̬����ѹر�");
	}
	else
	{
		Form1->MemoLuaOut->Lines->Add("USB����״̬����Ѵ�");
	}
}
//---------------------------------------------------------------------------

void __fastcall TForm1::LabelLedUSBMouseDown(TObject *Sender, TMouseButton Button,
          TShiftState Shift, int X, int Y)
{
	if (Button == mbRight)
	{
		Form1->MemoLuaOut->Lines->Add("USB�����ѹر�");
		Form1->MemoLuaOut->Lines->Add("USB����״̬����ѹر�");

		CheckBoxMonitorUSB->Checked = false;

		LabelLedUSB->Font->Color = clGray;  // USB��Ϩ��

		FormTcp->mReqCloseCom = 1;	// ����رմ���
	}
}
//---------------------------------------------------------------------------

void __fastcall TForm1::ButtonCheckHttpClick(TObject *Sender)
{
/*
Foramt=0x01
PCSoftwareVersion=0x010401
PCSoftwareDate="2020-12-20 12:00"
FirmwareVersion=0x00000143
FirmwareDate=20201220
PCLink="http://www.armbbs.cn/forum.php?mod=attachment&aid=NTg4MTd8Yzg0NDhmMzF8MTU5MDk4OTEyNHw1OHw5ODAzNQ%3D%3D"
FWLink="http://www.armbbs.cn/forum.php?mod=attachment&aid=NTg4MTd8Yzg0NDhmMzF8MTU5MDk4OTEyNHw1OHw5ODAzNQ%3D%3D"
*/
	//LabelCheckVer->Caption = "���ڻ�ȡ��ҳ����....";
	TMemoryStream *ms = new  TMemoryStream;

	IdHTTP1->Get("http://www.armfly.com/download/H7-TOOL/H7ToolUpdate.txt", ms);//g_Url����ҳ��ַ

	if (ms->Size == 0) {
		delete ms;
        return;
	}
	const int msSize = ms->Size;

	LPSTR lpBuf = new char[msSize];

	ms->Position=0;
	ms->Read(lpBuf, msSize);

	delete ms;

	String const strText = String(lpBuf, msSize);
    delete [] lpBuf;

	//LabelCheckVer->Caption = "��ҳ���ݻ�ȡ��ϣ����ڷ���ҳ��Ԫ��...";

	{
		int pos, pos2;
		String strPCVer;
		String strDate;
		String strLink;
		int iPCNewVer;
		int iPCOldVer;
		char ver[128];
		String strNewVer;

		pos = strText.Pos("PCSoftwareVersion");
		strPCVer = strText.SubString(pos + 18, 8).Trim();

		// �°汾
		iPCNewVer = StrToInt(strPCVer);
		sprintf(ver, "V%d.%d.%d", (iPCNewVer >> 16) & 0xFF, (iPCNewVer >> 8) & 0xFF, iPCNewVer & 0xFF);
		pos = strText.Pos("PCSoftwareDate");
		strDate = strText.SubString(pos + 16, 10).Trim();
		strNewVer = (AnsiString)ver + " (" + strDate + ")";

		//����
		pos = strText.Pos("PCLink=");
		pos += 7;
		for (int i = 0; i < 256; i++)
		{
			wchar_t  ch;

			ch = strText.c_str()[pos + i];
			if (ch == L'""')
			{
				break;
			}
			strLink += ch;
		}

		// �ɰ汾
		iPCOldVer = (PC_SOFT_VER1 << 16) + (PC_SOFT_VER2 << 8) + PC_SOFT_VER3;

		if (iPCNewVer > iPCOldVer)
		{
			LabelCheckVer->Caption = "�����°汾: " + strNewVer;

			mHttpLink = strLink;
			ButtonGotoDownload->Caption = "ȥ����";
			ButtonGotoDownload->Visible = true;
		}
		else if (iPCNewVer == iPCOldVer)
		{
			LabelCheckVer->Caption = "��ǰ�������°汾";
			mHttpLink = strLink;
			ButtonGotoDownload->Caption = "��������";
			ButtonGotoDownload->Visible = true;
		}
		else
		{
			LabelCheckVer->Caption = "�������Ǿɰ汾: " + strNewVer;
		}
	}
//
//	String User=UseInfoBlock.SubString(1,14);
//	E_user->Text=User;
//	String PassWordMark="�ܡ��룺 ";
//	String PassWord=UseInfoBlock.SubString(UseInfoBlock.Pos(PassWordMark)+PassWordMark.Length(),10);
//	E_passWord->Text=PassWord;
}
//---------------------------------------------------------------------------

void __fastcall TForm1::ButtonGotoDownloadClick(TObject *Sender)
{
//[bcc32 Error] UnitMain.cpp(2680): E2034 Cannot convert 'UnicodeString' to 'const wchar_t *'
//  Full parser context
//	UnitMain.cpp(2677): parsing: void _fastcall TForm1::ButtonGotoDownloadClick(TObject *)

	ShellExecute(Handle,L"open",
		mHttpLink.c_str(),
		NULL,NULL,SW_SHOWMAXIMIZED);
}
//---------------------------------------------------------------------------

void __fastcall TForm1::Timer1Timer(TObject *Sender)
{
	Timer1->Enabled = false;
	ButtonCheckHttp->OnClick(Sender);
}
//---------------------------------------------------------------------------

void __fastcall TForm1::N3Click(TObject *Sender)
{
	FormChangeLog->Show();
}
//---------------------------------------------------------------------------


