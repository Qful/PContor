//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "UnitProg.h"
#include "UnitMain.h"
#include "UnitThread.h"
#include "UnitUSBBus.h"
#include "UnitSet.h"
#include "lua_if.h"
#include "file_lib.h"
#include "hex2bin.h"
#include "bsp.h"
#include "bsp_udp_modbus.h"
#include "bsp_user_lib.h"

//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TFormUSBBus *FormUSBBus;

//---------------------------------------------------------------------------
__fastcall TFormUSBBus::TFormUSBBus(TComponent* Owner)
	: TForm(Owner)
{
}

//---------------------------------------------------------------------------
void __fastcall TFormUSBBus::AddLuaSrc(AnsiString str)
{
//	int line;
//
//	line = SendMessage(MemoI2C->Handle,EM_EXLINEFROMCHAR, MemoI2C->SelStart, 0);
	MemoI2C->Lines->Add(str);
}

//---------------------------------------------------------------------------
void __fastcall TFormUSBBus::Button67Click(TObject *Sender)
{
	AnsiString s1;

//	if (FormSet->ComboBoxCommMode->ItemIndex != 0)
//	{
//		Form1->MemoLuaOut->Lines->Add("�ù��ܽ�֧����̫������ģʽ");
//		return;
//	}

//	if (Form1->CheckBoxUDP777->Checked == false)
//	{
//		Form1->MemoLuaOut->Lines->Add("���UDP�����˿�");
//		return;
//	}

	//����������lus
	s1 = AppExePath();
	s1 = s1 + "\\USBBus\\usb_bus.lua";
	Form1->DownloadLuaFile(s1);
}
//---------------------------------------------------------------------------
void __fastcall TFormUSBBus::ComboBoxBusVoltChange(TObject *Sender)
{
	int16_t volt;
	float ff;

	ff = StrToFloat(ComboBoxBusVolt->Text);
	volt = ff * 1000;
	H7_WriteRegister(REG03_OUT_TVCC_MV, volt);
}
//---------------------------------------------------------------------------
//ɨ��i2c�豸
void __fastcall TFormUSBBus::Button65Click(TObject *Sender)
{
	AnsiString s;
	uint32_t freq;

	if (Form1->CheckBoxClear->Checked)
	{
		Form1->MemoLuaOut->Clear();
	}

	freq = MyStrToInt(ComboBoxI2CClock->Text) * 1000;
	s.sprintf("I2C_ScanDevice(%d)", freq);
	//udp_Send65H((char *)s.c_str(), s.Length());
	FormTcp->DoLuaSend64H(s);
}
//---------------------------------------------------------------------------

void __fastcall TFormUSBBus::FormCreate(TObject *Sender)
{
	bsp_AddLog("FormUSBBus Create Begin");

	bsp_AddLog("FormUSBBus Create End");
}
//---------------------------------------------------------------------------

//�޸�����ʱ��
void __fastcall TFormUSBBus::ComboBoxI2CClockChange(TObject *Sender)
{
	AnsiString s;
	uint32_t freq;

	freq = MyStrToInt(ComboBoxI2CClock->Text) * 1000;
	s.sprintf("I2C_InitSlave(%d)", freq);
	//udp_Send65H((char *)s.c_str(), s.Length());
	FormTcp->DoLuaSend64H(s);
}
//---------------------------------------------------------------------------


void __fastcall TFormUSBBus::ButtonI2CReadClick(TObject *Sender)
{
	AnsiString s;
	uint8_t addr;
	uint16_t num;
	AnsiString d;

	addr = MyStrToInt(ComboBoxI2CAddr->Text);
	num = MyStrToInt(EditI2CReadNum->Text);
	d = ComboBoxI2CDispWidth->Text;
	s.sprintf("I2C_ReadData(0x%02X, %d, %s)", addr, num, d.c_str());

	//udp_Send65H((char *)s.c_str(), s.Length());
	FormTcp->DoLuaSend64H(s);
}
//---------------------------------------------------------------------------

void __fastcall TFormUSBBus::Button5Click(TObject *Sender)
{
	AnsiString s;
	AnsiString d;
	uint8_t addr;

	addr = MyStrToInt(ComboBoxI2CAddr->Text);
	d = EditI2CWriteData1->Text;

	d = AnsiStringToLuaSting(d);
	s.sprintf("I2C_WriteData(0x%02X, \"%s\")", addr, d.c_str());

	//udp_Send65H((char *)s.c_str(), s.Length());
	FormTcp->DoLuaSend64H(s);
}
//---------------------------------------------------------------------------

// ��lua����
void __fastcall TFormUSBBus::Button1Click(TObject *Sender)
{
	AnsiString s;
	AnsiString d;
	uint8_t addr;

	addr = MyStrToInt(ComboBoxI2CAddr->Text);
	d = EditI2CWriteData0->Text;

	d = AnsiStringToLuaSting(d);
	s.sprintf("I2C_WriteData(0x%02X, \"%s\")", addr, d.c_str());

	AddLuaSrc(s);
}
//---------------------------------------------------------------------------

void __fastcall TFormUSBBus::Button4Click(TObject *Sender)
{
	AnsiString s;
	AnsiString d;
	uint8_t addr;

	addr = MyStrToInt(ComboBoxI2CAddr->Text);
	d = EditI2CWriteData1->Text;

	d = AnsiStringToLuaSting(d);
	s.sprintf("I2C_WriteData(0x%02X, \"%s\")", addr, d.c_str());

	AddLuaSrc(s);
}
//---------------------------------------------------------------------------


void __fastcall TFormUSBBus::ButtonI2CSendClick(TObject *Sender)
{
	AnsiString s;
	AnsiString d;
	uint8_t addr;

	addr = MyStrToInt(ComboBoxI2CAddr->Text);
	d = EditI2CWriteData0->Text;

	d = AnsiStringToLuaSting(d);
	s.sprintf("I2C_WriteData(0x%02X, \"%s\")", addr, d.c_str());

	//udp_Send65H((char *)s.c_str(), s.Length());
	FormTcp->DoLuaSend64H(s);
}
//---------------------------------------------------------------------------

void __fastcall TFormUSBBus::Button3Click(TObject *Sender)
{
	AnsiString s;
	uint8_t addr;
	uint16_t num;
	AnsiString d;

	addr = MyStrToInt(ComboBoxI2CAddr->Text);
	num = MyStrToInt(EditI2CReadNum->Text);
	d = ComboBoxI2CDispWidth->Text;
	s.sprintf("I2C_ReadData(0x%02X, %d, %s)", addr, num, d.c_str());

	AddLuaSrc(s);
}
//---------------------------------------------------------------------------

void __fastcall TFormUSBBus::Button6Click(TObject *Sender)
{
	MemoI2C->Lines->Clear();
}
//---------------------------------------------------------------------------

void __fastcall TFormUSBBus::Button2Click(TObject *Sender)
{
	AnsiString s;

	s = MemoI2C->Text;
	//udp_Send65H((char *)s.c_str(), s.Length());
	FormTcp->DoLuaSend64H(s);
}
//---------------------------------------------------------------------------

void __fastcall TFormUSBBus::Button7Click(TObject *Sender)
{
	AnsiString s;
	uint32_t delay;

	delay = MyStrToInt(EditI2CDelay->Text);
	s.sprintf("delayms(%d)", delay);

	AddLuaSrc(s);
}
//---------------------------------------------------------------------------

void __fastcall TFormUSBBus::Button9Click(TObject *Sender)
{
	AnsiString s;
	uint32_t count;

	count = MyStrToInt(EditI2CFor->Text);
	s.sprintf("for i = 1, %d, 1 do\r\nend", count);

	AddLuaSrc(s);
}
//---------------------------------------------------------------------------

void __fastcall TFormUSBBus::Button10Click(TObject *Sender)
{
	AnsiString s;
	uint32_t t1,t2;
	uint32_t cn;

	t1 = MyStrToInt(EditI2CBeepStart->Text);
	t2 = MyStrToInt(EditI2CBeepStop->Text);
	cn = MyStrToInt(EditI2CBeepCycle->Text);

	s.sprintf("beep(%d, %d, %d)", t1, t2, cn);

	AddLuaSrc(s);
}
//---------------------------------------------------------------------------



void __fastcall TFormUSBBus::Button11Click(TObject *Sender)
{
	AnsiString s;

	s.sprintf("if ( ) then\r\nelse\r\nend");

	AddLuaSrc(s);
}
//---------------------------------------------------------------------------



// ����SPI
void __fastcall TFormUSBBus::Button12Click(TObject *Sender)
{
	AnsiString s;
	uint32_t baud, phase, pol;

	if (ComboBoxSPIPhasePol->ItemIndex == 0)
	{
		pol = 0;
		phase = 0;
	}
	else if (ComboBoxSPIPhasePol->ItemIndex == 1)
	{
		pol = 0;
		phase = 1;
	}
	else if (ComboBoxSPIPhasePol->ItemIndex == 2)
	{
		pol = 1;
		phase = 0;
	}
	else if (ComboBoxSPIPhasePol->ItemIndex == 3)
	{
		pol = 1;
		phase = 0;
	}
	baud = ComboBoxSPIFreq->ItemIndex;

	s.sprintf("SPI_Init(%d, %d, %d) --Ƶ��ID, ʱ����λ, ʱ�Ӽ���", baud, phase, pol);

	AddLuaSrc(s);

	//udp_Send65H((char *)s.c_str(), s.Length());
	FormTcp->DoLuaSend64H(s);
}
//---------------------------------------------------------------------------

void __fastcall TFormUSBBus::Button16Click(TObject *Sender)
{
	AnsiString s;
	AnsiString d;
	uint8_t ch;

	ch = ComboBoxSpiCsCh->ItemIndex;

	d = EditSPIWriteData0->Text;

	d = AnsiStringToLuaSting(d);
	s.sprintf("SPI_WriteData(0x%02X, \"%s\")", ch, d.c_str());

	AddLuaSrc(s);
}
//---------------------------------------------------------------------------

void __fastcall TFormUSBBus::Button13Click(TObject *Sender)
{
	AnsiString s;
	AnsiString d;
	uint8_t ch;

	ch = ComboBoxSpiCsCh->ItemIndex;

	d = EditSPIWriteData0->Text;

	d = AnsiStringToLuaSting(d);
	s.sprintf("SPI_WriteData(0x%02X, \"%s\")", ch, d.c_str());

	//udp_Send65H((char *)s.c_str(), s.Length());
	FormTcp->DoLuaSend64H(s);
}
//---------------------------------------------------------------------------

//д&��
void __fastcall TFormUSBBus::Button18Click(TObject *Sender)
{
	AnsiString s;
	AnsiString d;
	uint8_t ch;
	uint16_t rdnum;

	ch = ComboBoxSpiCsCh->ItemIndex;

	d = EditSPIWriteData1->Text;

	rdnum = MyStrToInt(EditSPIReadNum1->Text);	// д�����ֽ���

	d = AnsiStringToLuaSting(d);
	s.sprintf("SPI_WriteReadData(0x%02X, \"%s\", %d)", ch, d.c_str(), rdnum);

	AddLuaSrc(s);
}
//---------------------------------------------------------------------------

void __fastcall TFormUSBBus::Button19Click(TObject *Sender)
{
	AnsiString s;
	AnsiString d;
	uint8_t ch;
	uint16_t rdnum;

	ch = ComboBoxSpiCsCh->ItemIndex;

	d = EditSPIWriteData1->Text;

	rdnum = MyStrToInt(EditSPIReadNum1->Text);	// д�����ֽ���

	d = AnsiStringToLuaSting(d);
	s.sprintf("SPI_WriteReadData(0x%02X, \"%s\", %d)", ch, d.c_str(), rdnum);

	//udp_Send65H((char *)s.c_str(), s.Length());
	FormTcp->DoLuaSend64H(s);
}
//---------------------------------------------------------------------------

void __fastcall TFormUSBBus::Button17Click(TObject *Sender)
{
	AnsiString s;
	uint8_t ch;
	uint16_t num;
	AnsiString d;

	ch = ComboBoxSpiCsCh->ItemIndex;
	num = MyStrToInt(EditSPIOnlyReadNum->Text);
	d = ComboBoxI2CDispWidth->Text;
	s.sprintf("SPI_ReadData(0x%02X, %d, %s)", ch, num, d.c_str());

	AddLuaSrc(s);
}
//---------------------------------------------------------------------------

void __fastcall TFormUSBBus::Button15Click(TObject *Sender)
{
	AnsiString s;
	uint8_t ch;
	uint16_t num;
	AnsiString d;

	ch = ComboBoxSpiCsCh->ItemIndex;
	num = MyStrToInt(EditSPIOnlyReadNum->Text);
	d = ComboBoxI2CDispWidth->Text;
	s.sprintf("SPI_ReadData(0x%02X, %d, %s)", ch, num, d.c_str());

	//udp_Send65H((char *)s.c_str(), s.Length());
	FormTcp->DoLuaSend64H(s);
}
//---------------------------------------------------------------------------

void __fastcall TFormUSBBus::Button14Click(TObject *Sender)
{
	AnsiString s;

	s.sprintf("function fun1(void)\r\n\r\nend");

	AddLuaSrc(s);
}
//---------------------------------------------------------------------------

void __fastcall TFormUSBBus::Button22Click(TObject *Sender)
{
	Form1->MemoAdd("I2C���������Ŷ���");
	Form1->MemoAdd(" D9 : I2C_SCL, ��©�ڲ�������������2Kŷ");
	Form1->MemoAdd(" D7 : I2C_SDA, ��©�ڲ�������������2Kŷ");
}
//---------------------------------------------------------------------------

void __fastcall TFormUSBBus::Button23Click(TObject *Sender)
{
	Form1->MemoAdd("SPI���������Ŷ���");
	Form1->MemoAdd(" D0 : CS0 Ƭѡ0");
	Form1->MemoAdd(" D1 : CS1 Ƭѡ1");
	Form1->MemoAdd(" D5 : SCK ʱ��");
	Form1->MemoAdd(" D2 : SDO MOSI");
	Form1->MemoAdd(" D1 : SDI MISI");
}
//---------------------------------------------------------------------------

void __fastcall TFormUSBBus::ComboBoxSPIPhasePolChange(TObject *Sender)
{
	if (ComboBoxSPIPhasePol->ItemIndex == 0)
	{
		ImageSPI_00->Visible = true;
		ImageSPI_01->Visible = false;
		ImageSPI_10->Visible = false;
		ImageSPI_11->Visible = false;
	}
	else if (ComboBoxSPIPhasePol->ItemIndex == 1)
	{
		ImageSPI_00->Visible = false;
		ImageSPI_01->Visible = true;
		ImageSPI_10->Visible = false;
		ImageSPI_11->Visible = false;
	}
	else if (ComboBoxSPIPhasePol->ItemIndex == 2)
	{
		ImageSPI_00->Visible = false;
		ImageSPI_01->Visible = false;
		ImageSPI_10->Visible = true;
		ImageSPI_11->Visible = false;
	}
	else if (ComboBoxSPIPhasePol->ItemIndex == 3)
	{
		ImageSPI_00->Visible = false;
		ImageSPI_01->Visible = false;
		ImageSPI_10->Visible = false;
		ImageSPI_11->Visible = true;
	}

}
//---------------------------------------------------------------------------

