//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "UnitLuaApi.h"
#include "UnitMain.h"
#include "bsp.h"
#include "bsp_udp_modbus.h"
#include "bsp_user_lib.h"

//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TFormLuaApi *FormLuaApi;
//---------------------------------------------------------------------------
__fastcall TFormLuaApi::TFormLuaApi(TComponent* Owner)
	: TForm(Owner)
{
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
//���Դ��, ��������
void __fastcall TFormLuaApi::AddLuaSrc0(AnsiString str)
{
	Memo1->Lines->Add(str);
	MemoTemp->Lines->Add(str);
}

//---------------------------------------------------------------------------
//���Դ��, ������
void __fastcall TFormLuaApi::AddLuaSrc(AnsiString str)
{
	Memo1->Lines->Add(str);

	MemoTemp->Lines->Add(str);
	// �Զ����Ƶ����а�
	MemoTemp->SelectAll();
	MemoTemp->CopyToClipboard();
	MemoTemp->Clear();
}

//---------------------------------------------------------------------------
//��Ӱ�����MemoOut
void __fastcall TFormLuaApi::AddLuaToMemo(AnsiString str)
{
	Form1->MemoLuaOut->Lines->Add(str);
}

void __fastcall TFormLuaApi::Button3Click(TObject *Sender)
{
	if (CheckBoxClear->Checked)
	{
		Memo1->Lines->Clear();
	}

	AddLuaSrc0("--����ģ������·");
	AddLuaSrc0("function start_dso(void)");
	AddLuaSrc0("  write_reg16(0x01FF, 2) -- ����ģʽ 0:ʾ���� 1:���ص��� 2:��·����ɨ��");
	AddLuaSrc0("  write_reg16(0x0200, 1) -- CH1���,0:AC 1:DC");
	AddLuaSrc0("  write_reg16(0x0201, 1) -- CH2���,0:AC 1:DC");
	AddLuaSrc0("  --����ȡֵ 0:��13.8V 1:��6.4V 2:��3.2V 3:��1.6V 4:��800mV 5:��400mV 6:��200mV 7:��100mV");
	AddLuaSrc0("  write_reg16(0x0202, 0) -- CH1����");
	AddLuaSrc0("  write_reg16(0x0203, 0) -- CH2����");
	AddLuaSrc0("  write_reg16(0x0204, 0) -- CH1ͨ��ֱ��ƫֵ��δ��");
	AddLuaSrc0("  write_reg16(0x0205, 0) -- CH2ͨ��ֱ��ƫֵ��δ��");
	AddLuaSrc0("  write_reg16(0x0206, 12) --����Ƶ�� 0:100 1:200 2:500 3:1K 4:2K 5:5K 6:10K 7:20K");
	AddLuaSrc0("                          --8:50K 9:100K 10:200K 11:500K 12:1M 13:2M 14:5M");
	AddLuaSrc0("  write_reg16(0x0207, 0) --������� 0:1K 1:2K 3:4K 4:8K 5:16K 6:32K");
	AddLuaSrc0("  write_reg16(0x0208, 32768) --������ƽADC 0-65535");
	AddLuaSrc0("  write_reg16(0x0209, 50) --����λ�ðٷֱ� 0-100");
	AddLuaSrc0("  write_reg16(0x020A, 0) --����ģʽ 0:�Զ� 1:��ͨ 2:����");
	AddLuaSrc0("  write_reg16(0x020B, 0) --����ͨ�� 0:CH1 1:CH2");
	AddLuaSrc0("  write_reg16(0x020C, 0) --�������� 0:�½��� 1:������ ");
	AddLuaSrc0("  write_reg16(0x020D, 0x03) --ͨ��ʹ�ܿ��� bit0 = CH1  bit1 = CH2");
	AddLuaSrc0("  write_reg16(0x020E, 1) --�ɼ����� 0:ֹͣ 1:����");
	AddLuaSrc("end");

}
//---------------------------------------------------------------------------

void __fastcall TFormLuaApi::ButtonReadAdcClick(TObject *Sender)
{
	int ch;
	AnsiString str;

	if (CheckBoxClear->Checked)
	{
		Memo1->Lines->Clear();
	}
	const char *AnCh[9] = {
		"0 - CH1��ѹ",
		"1 - CH2��ѹ",
		"2 - �ฺ߲�ص�ѹ",
		"3 - �߶˸��ص���",
		"4 - TVCC��ѹ",
		"5 - TVCC����",
		"6 - NTC����������ֵ",
		"7 - �ⲿ�����ѹ",
		"8 - USB�����ѹ",
	};


	ch = ComboBoxAiCh->ItemIndex;
	str = AnCh[ch];
	AddLuaSrc("read_adc(" + IntToStr(ch)+ ") --" + str);
}
//---------------------------------------------------------------------------

void __fastcall TFormLuaApi::Button1Click(TObject *Sender)
{
	int ch;
	AnsiString str;

	if (CheckBoxClear->Checked)
	{
		Memo1->Lines->Clear();
	}
	const char *AnCh[9] = {
		"0 - CH1��ѹ",
		"1 - CH2��ѹ",
		"2 - �ฺ߲�ص�ѹ",
		"3 - �߶˸��ص���",
		"4 - TVCC��ѹ",
		"5 - TVCC����",
		"6 - NTC����������ֵ",
		"7 - �ⲿ�����ѹ",
		"8 - USB�����ѹ",
	};


	ch = ComboBoxAiCh->ItemIndex;
	str = AnCh[ch];
	AddLuaSrc("read_analog(" + IntToStr(ch)+ ") --" + str);
}
//---------------------------------------------------------------------------

void __fastcall TFormLuaApi::Button2Click(TObject *Sender)
{
	if (CheckBoxClear->Checked)
	{
		Memo1->Lines->Clear();
	}
	AddLuaSrc("dac_on() --����DAC��Դ");
}
//---------------------------------------------------------------------------

void __fastcall TFormLuaApi::Button4Click(TObject *Sender)
{
	if (CheckBoxClear->Checked)
	{
		Memo1->Lines->Clear();
	}
	AddLuaSrc("dac_off() --�ر�DAC��Դ");
}
//---------------------------------------------------------------------------

void __fastcall TFormLuaApi::Button5Click(TObject *Sender)
{
	if (CheckBoxClear->Checked)
	{
		Memo1->Lines->Clear();
	}

	AddLuaSrc("dac_volt(" + EditMV->Text + ") --����DAC�����ѹmV");
}
//---------------------------------------------------------------------------

void __fastcall TFormLuaApi::Button6Click(TObject *Sender)
{
	if (CheckBoxClear->Checked)
	{
		Memo1->Lines->Clear();
	}

	AddLuaSrc("dac_curr(" + EditUA->Text + ") --����DAC�������uA");
}
//---------------------------------------------------------------------------

void __fastcall TFormLuaApi::Button7Click(TObject *Sender)
{
	AnsiString src;

	if (CheckBoxClear->Checked)
	{
		Memo1->Lines->Clear();
	}

	src = "set_tvcc(" + EditTVCC->Text + ") --����TVCC�����ѹ0��1.2-5.5V";
	AddLuaSrc(src);
}
//---------------------------------------------------------------------------

void __fastcall TFormLuaApi::Button8Click(TObject *Sender)
{
	AnsiString src;
	int pin;
	int mode;

	if (CheckBoxClear->Checked)
	{
		Memo1->Lines->Clear();
	}

	pin = ComboBoxGpioId->ItemIndex;
	mode =  ComboBoxGpioMode->ItemIndex;

	src = "gpio_cfg(" + IntToStr(pin) + ", " + IntToStr(mode) + ") --����ģʽ"
		+ ComboBoxGpioId->Text + " = " + ComboBoxGpioMode->Text;
	AddLuaSrc(src);
}
//---------------------------------------------------------------------------

void __fastcall TFormLuaApi::Button9Click(TObject *Sender)
{
	AnsiString src;
	int pin;
	int mode;

	if (CheckBoxClear->Checked)
	{
		Memo1->Lines->Clear();
	}

	pin = ComboBoxGpioId->ItemIndex;
	mode =  ComboBoxGpioMode->ItemIndex;

	src = "gpio_write(" + IntToStr(pin) + ", " + "1) --�������"
		+ ComboBoxGpioId->Text + " = 1";
	AddLuaSrc(src);
}
//---------------------------------------------------------------------------

void __fastcall TFormLuaApi::Button10Click(TObject *Sender)
{
	AnsiString src;
	int pin;
	int mode;

	if (CheckBoxClear->Checked)
	{
		Memo1->Lines->Clear();
	}

	pin = ComboBoxGpioId->ItemIndex;
	mode =  ComboBoxGpioMode->ItemIndex;

	src = "gpio_write(" + IntToStr(pin) + ", " + "0) --�������"
		+ ComboBoxGpioId->Text + " = 0";
	AddLuaSrc(src);
}
//---------------------------------------------------------------------------

void __fastcall TFormLuaApi::Button11Click(TObject *Sender)
{
	AnsiString src;
	int pin;
	int mode;

	if (CheckBoxClear->Checked)
	{
		Memo1->Lines->Clear();
	}

	pin = ComboBoxGpioId->ItemIndex;
	mode =  ComboBoxGpioMode->ItemIndex;

	src = "pin = gpio_read(" + IntToStr(pin) + ") --��GPIO״̬";
	AddLuaSrc(src);
}
//---------------------------------------------------------------------------

void __fastcall TFormLuaApi::Button12Click(TObject *Sender)
{
	AnsiString src;
	int pin;
	int mode;

	if (CheckBoxClear->Checked)
	{
		Memo1->Lines->Clear();
	}

	pin = ComboBoxGpioId->ItemIndex;
	mode =  ComboBoxGpioMode->ItemIndex;

	src = "pa = read_bus() --�����ȫ����GPIO״̬";
	AddLuaSrc(src);
}
//---------------------------------------------------------------------------


void __fastcall TFormLuaApi::ComboBoxStringFuncClick(TObject *Sender)
{
	if (CheckBoxClear->Checked)
	{
		Memo1->Lines->Clear();
	}

	//	string.byte �ַ�ת����ascii����
	//	string.char Ascii��ֵת�����ַ�
	//	string.sub ��ȡ�ַ���
	//	string.find �ַ������Һ���
	//	string.match �ַ������Һ���
	//	string.gsub �ַ����滻
	//	string.format �ַ�����ʽ������
	//	string.len �����ַ�������=#s
	//	string.rep �ַ����ظ�n�β�ƴ�ӷ���
	//	string.lower תСд
	//	string.upper ת��д
	//	string.reverse ��ת�ַ���

	if (ComboBoxStringFunc->ItemIndex == 0)
	{
		//	string.byte �ַ�ת����ascii����
		AddLuaSrc0("string.byte(s [, i [, j]])");
		AddLuaSrc0("");
		AddLuaSrc0("string.byte���������ַ�ת����ascii���֣�");
		AddLuaSrc0("sΪĿ���ַ�����iΪ������ʼλ��(��1��ʼ)��jΪ��������λ��");
		AddLuaSrc0("����");
		AddLuaSrc0("--Ĭ��Ϊ��1������a��asciiֵ");
		AddLuaSrc0("local r = string.byte('abcdefg') --97");

		AddLuaSrc0("--������2(b)������4(d)Ҳ���Ƿֱ𷵻�bcd��asciiֵ");
		AddLuaSrc0("local r1,r2,r3 = string.byte('abcdefg',2,4) --98,99,100");
	}
	else if (ComboBoxStringFunc->ItemIndex == 1)
	{
		//	string.char Ascii��ֵת�����ַ�
		AddLuaSrc0("string.char(...)");
		AddLuaSrc0("");
		AddLuaSrc0("string.char�ǰ�ascii��ֵת�����ַ�");
		AddLuaSrc0("����");
		AddLuaSrc0("--����98����Ӧ���ַ�");
		AddLuaSrc0("local r = string.char(98) --a");

		AddLuaSrc0("--����98,,99,100��Ӧ���ַ�������һ�𷵻�");
		AddLuaSrc0("local r = string.char(98,99,100) --abc");
	}
	else if (ComboBoxStringFunc->ItemIndex == 2)
	{
		AddLuaSrc0("string.sub (s, i [, j])");
		AddLuaSrc0("");
		AddLuaSrc0("��ȡ�ַ���(�ַ����ָ�ַ�����ȡ)��iΪ��ʼ��������ѡ����jΪ��������(����)��������Ϊ��������һ���ַ�����Ϊ1�����һ���ַ�Ϊ-1");
		AddLuaSrc0("����");
		AddLuaSrc0("local res,s");
		AddLuaSrc0("s = 'www.armfly.com'");
		AddLuaSrc0("res = string.sub(s,5)     --armfly.com");
		AddLuaSrc0("res = string.sub(s,5,-1)  --armfly.com");
		AddLuaSrc0("");
		AddLuaSrc0("--��ȡ��3λ");
		AddLuaSrc0("res = string.sub(s,-3)    --com");
		AddLuaSrc0("");
		AddLuaSrc0("--��ȡǰ3λ");
		AddLuaSrc0("res = string.sub(s,1,3)   --www");
	}
	else if (ComboBoxStringFunc->ItemIndex == 3)
	{
	//	string.find �ַ������Һ���
		AddLuaSrc0("string.find (s, pattern [, init [, plain]])");
		AddLuaSrc0("");
		AddLuaSrc0("�ַ������Һ����Ҳ�������nil���ҵ��˷��ؿ�ʼλ�úͽ���λ�ã�");
		AddLuaSrc0("initΪ�����￪ʼĬ��Ϊ1��plainĬ��Ϊfalse��ʾ����ģʽƥ�䣬");
		AddLuaSrc0("�����Ϊtrue���ʾ���ı�ƥ��(Ҳ���ǹر�����ƥ��)");
		AddLuaSrc0("����");
		AddLuaSrc0("local str = 'i love programming,11,22,%d+aa'");
		AddLuaSrc0("local s = string.find(str,'222')    --nil");
		AddLuaSrc0("s = string.find(str,'pro')  --8");
		AddLuaSrc0("s = string.find(str,\",%d+\")    --19(ƥ�䵽��,11)");
		AddLuaSrc0("s = string.find(str,\",%d+\",1,true) --25(���ڹر���ģʽƥ��,����ƥ�䵽��,%d+)");
	}
	else if (ComboBoxStringFunc->ItemIndex == 4)
	{
	//	string.match �ַ������Һ���
		AddLuaSrc0("string.match (s, pattern [, init])");
		AddLuaSrc0("");
		AddLuaSrc0("����string.find��ֻ࣬�����ܰѲ���ƥ�䵽�Ľ��������");
		AddLuaSrc0("����");
		AddLuaSrc0("local s,res,res1,res2");
		AddLuaSrc0("s = 'http://www.armfly.com'");
		AddLuaSrc0("");
		AddLuaSrc0("--����û�в��񣬷���ȫ��ƥ��");
		AddLuaSrc0("--�����http://www.armfly.com");
		AddLuaSrc0("res = string.match(s,'http://%a+\.%a+\.com')");
		AddLuaSrc0("");
		AddLuaSrc0("--����в�����ֱ𷵻ز�����");
		AddLuaSrc0("--�����www    armfly");
		AddLuaSrc0("res1,res2 = string.match(s,'http://(%a+)\.(%a+)\.com')");
	}
	else if (ComboBoxStringFunc->ItemIndex == 5)
	{
	//	string.gsub �ַ����滻
		AddLuaSrc0("string.gsub (s, pattern, repl [, n])");
		AddLuaSrc0("");
		AddLuaSrc0("�������ַ����滻����ѡ����n�����滻���ٴ�Ĭ��ȫ���滻��");
		AddLuaSrc0("�����滻����ַ�����Ҳ����ָ���ڶ�������ֵΪ�滻�Ĵ�����");
		AddLuaSrc0("����");
		AddLuaSrc0("local s,res,res1,res2");
		AddLuaSrc0("s = 'http://www.armfly.com'");
		AddLuaSrc0("");
		AddLuaSrc0("--�����http://test.armfly.com��1");
		AddLuaSrc0("res,count = string.gsub(s,'www','test')");
		AddLuaSrc0("");
		AddLuaSrc0("--�����滻");
		AddLuaSrc0("--�����test.freecls.abc");
		AddLuaSrc0("res = string.gsub(s,'^http://%w+\.(%w+)\.com$','test.%1.abc')");
		AddLuaSrc0("");
		AddLuaSrc0("--w�滻��t������ֻ�滻2��");
		AddLuaSrc0("--�����http://ttw.armfly.com");
		AddLuaSrc0("res = string.gsub(s,'w','t',2)");

	}
	else if (ComboBoxStringFunc->ItemIndex == 6)
	{
	//	string.format �ַ�����ʽ������
		AddLuaSrc0("string.format (formatstring, ������)");
		AddLuaSrc0("�ַ�����ʽ������c���Ե�sprintf��˵�ϻ�������������");
		AddLuaSrc0("");
		AddLuaSrc0("local s = string.format('%d%s',123,'armfly')   --123armlfy");
		AddLuaSrc0("");
		AddLuaSrc0("s = string.format('%0.2f',1.234343)     --1.23(����2λ)");
		AddLuaSrc0("");
		AddLuaSrc0("--ת��16���ƣ�%XΪ��д��16����");
		AddLuaSrc0("local s = string.format('%X',140)       --8C");
		AddLuaSrc0("local s = string.format('%x',140)       --8c");
		AddLuaSrc0("local s = string.format('%04x',140)     --008c");
	}
	else if (ComboBoxStringFunc->ItemIndex == 7)
	{
	//	string.len �����ַ�������=#s
		AddLuaSrc0("string.len(s)");
		AddLuaSrc0("");
		AddLuaSrc0("�����ַ�������=#s");
	}
	else if (ComboBoxStringFunc->ItemIndex == 8)
	{
	//	string.rep �ַ����ظ�n�β�ƴ�ӷ���
		AddLuaSrc0("string.rep(s,n)");
		AddLuaSrc0("");
		AddLuaSrc0("�ַ����ظ�n�β�ƴ�ӷ���");
	}
	else if (ComboBoxStringFunc->ItemIndex == 9)
	{
	//	string.lower תСд
		AddLuaSrc0("string.lower(s)");
		AddLuaSrc0("");
		AddLuaSrc0("תСд");
	}
	else if (ComboBoxStringFunc->ItemIndex == 10)
	{
	//	string.upper ת��д
		AddLuaSrc0("string.upper(s)");
		AddLuaSrc0("");
		AddLuaSrc0("ת��д");
	}
	else if (ComboBoxStringFunc->ItemIndex == 11)
	{
	//	string.reverse ��ת�ַ���
		AddLuaSrc0("string.reverse(s)");
		AddLuaSrc0("");
		AddLuaSrc0("��ת�ַ���");
	}
}
//---------------------------------------------------------------------------

void __fastcall TFormLuaApi::Button13Click(TObject *Sender)
{
	if (CheckBoxClear->Checked)
	{
		Memo1->Lines->Clear();
	}

	AddLuaSrc0("����");
	AddLuaSrc0("1.������������");
	AddLuaSrc0("2.����û����Ϊnil,��ֵΪnil��ͬ��ɾ��");
	AddLuaSrc0("3.lua��nil,false��Ϊ��,������Ϊtrue");
	AddLuaSrc0("4.Lua�еı���ȫ��ȫ�ֱ���,������ local ��ʽ����Ϊ�ֲ�����");
	AddLuaSrc0("5.#ta�ַ����ĳ��ȣ�Ҳ���Ի�ȡ�������������Ӧ�ĸ���");
	AddLuaSrc0("");
	AddLuaSrc0("����");
	AddLuaSrc0("--ȫ�ָ�ֵ");
	AddLuaSrc0("x = 1");
	AddLuaSrc0("");
	AddLuaSrc0("--�ֲ������ֵ");
	AddLuaSrc0("local x,y = 1,2");
	AddLuaSrc0("");
	AddLuaSrc0("local ta = {1,2,3,name='������'}");

	AddLuaSrc0("--3");
	AddLuaSrc0("print(#ta)");
}
//---------------------------------------------------------------------------

void __fastcall TFormLuaApi::Button14Click(TObject *Sender)
{
	if (CheckBoxClear->Checked)
	{
		Memo1->Lines->Clear();
	}

	AddLuaSrc0("ע�͡�����ע�͡�ȡ������ע��");
	AddLuaSrc0("����");
	AddLuaSrc0("ע�͵���");
	AddLuaSrc0("--local a = 1");
	AddLuaSrc0("");
	AddLuaSrc0("ע�Ͷ���");
	AddLuaSrc0("--[[");
	AddLuaSrc0("local a = 1");
	AddLuaSrc0("--]]");

	AddLuaSrc0("ȡ��ע�Ͷ���");
	AddLuaSrc0("---[[");
	AddLuaSrc0("local a = 1");
	AddLuaSrc0("--]]");
}
//---------------------------------------------------------------------------

void __fastcall TFormLuaApi::Button15Click(TObject *Sender)
{
	if (CheckBoxClear->Checked)
	{
		Memo1->Lines->Clear();
	}

	AddLuaSrc0("����");
	AddLuaSrc0("if");
	AddLuaSrc0("and or not");
	AddLuaSrc0(">= <= == > < ~=");
	AddLuaSrc0("");
	AddLuaSrc0("����");
	AddLuaSrc0("if (a >= 0) then   --���ڵ���");
	AddLuaSrc0("  b = b + 1");
	AddLuaSrc0("else");
	AddLuaSrc0("  b = b - 1");
	AddLuaSrc0("end");
	AddLuaSrc0("");
	AddLuaSrc0("if (a ~= 0) then   --������");
	AddLuaSrc0("end");
	AddLuaSrc0("");
	AddLuaSrc0("if (a==b and c == 0) then --�߼���");
	AddLuaSrc0("");
	AddLuaSrc0("elseif (a~=b) then  --������");
	AddLuaSrc0("");
	AddLuaSrc0("elseif (not a) then  --�߼���");
	AddLuaSrc0("");
	AddLuaSrc0("else");
	AddLuaSrc0("");
	AddLuaSrc0("end");
}
//---------------------------------------------------------------------------

void __fastcall TFormLuaApi::Button16Click(TObject *Sender)
{
	if (CheckBoxClear->Checked)
	{
		Memo1->Lines->Clear();
	}

	AddLuaSrc0("ѭ��");
	AddLuaSrc0("����");
	AddLuaSrc0("local a=10");
	AddLuaSrc0("while (a < 20)");
	AddLuaSrc0("do");
	AddLuaSrc0("   a = a + 1");
	AddLuaSrc0("   print(a)");
	AddLuaSrc0("end");
	AddLuaSrc0("");
	AddLuaSrc0("local t = {1,2,3}");
	AddLuaSrc0("for i,v in pairs(t) do");
	AddLuaSrc0("    print(v)");
	AddLuaSrc0("end");
	AddLuaSrc0("");
	AddLuaSrc0("--ÿ�μ�1");
	AddLuaSrc0("for i=0,5,1 do");
	AddLuaSrc0("    print(i)");
	AddLuaSrc0("end");
	AddLuaSrc0("");
	AddLuaSrc0("--ÿ�μ�1");
	AddLuaSrc0("--5,4,3,2,1");
	AddLuaSrc0("for i=5,1,-1 do");
	AddLuaSrc0("    print(i)");
	AddLuaSrc0("end");
}
//---------------------------------------------------------------------------

void __fastcall TFormLuaApi::Button17Click(TObject *Sender)
{
	if (CheckBoxClear->Checked)
	{
		Memo1->Lines->Clear();
	}

	AddLuaSrc0("����");
	AddLuaSrc0("����");
	AddLuaSrc0("--��������������2������������2��ֵ�ú�");
	AddLuaSrc0("function add(num1, num2)");
	AddLuaSrc0("   return num1 + num2");
	AddLuaSrc0("end");
	AddLuaSrc0("");
	AddLuaSrc0("--�Ѻ�����ֵ��һ��������Ȼ����Ե���������");
	AddLuaSrc0("local myprint = function(str)");
	AddLuaSrc0("   print(str)");
	AddLuaSrc0("end");
	AddLuaSrc0("");
	AddLuaSrc0("function say(arg1,func)");
	AddLuaSrc0("    func(arg1)");
	AddLuaSrc0("end");
	AddLuaSrc0("");
	AddLuaSrc0("--www.freecls.com");
	AddLuaSrc0("say('www.freecls.com',myprint)");
	AddLuaSrc0("");
	AddLuaSrc0("--�ɱ����");
	AddLuaSrc0("function average(...)");
	AddLuaSrc0("   result = 0");
	AddLuaSrc0("   local arg={...}");
	AddLuaSrc0("   for i,v in ipairs(arg) do");
	AddLuaSrc0("      result = result + v");
	AddLuaSrc0("   end");
	AddLuaSrc0("   print(\"�ܹ����� \" .. #arg .. \" ����\")");
	AddLuaSrc0("   return result/#arg");
	AddLuaSrc0("end");
	AddLuaSrc0("");
	AddLuaSrc0("function what()");
	AddLuaSrc0("    return '������','www.armlfy.com'\");");
	AddLuaSrc0("end");
	AddLuaSrc0("");
	AddLuaSrc0("--���ض������");
	AddLuaSrc0("local name,url = what()");
	AddLuaSrc0("print(name,url)");
}
//---------------------------------------------------------------------------

#define RGB565(R, G, B) (((R >> 3) << 11) | ((G >> 2) << 5) | (B >> 3)) /* ��8λR,G,Bת��Ϊ 16λRGB565��ʽ */
void __fastcall TFormLuaApi::Button18Click(TObject *Sender)
{
	if (ColorDialog1->Execute())
	{
		uint32_t color;
		uint8_t R,G,B;

		color = ColorDialog1->Color;

		R = color >> 0;
		G = color >> 8;
		B = color >> 16;
		EditColorRGB->Text = "RGB565(" + IntToStr(R) + ", "+ IntToStr(G) + ", "+ IntToStr(B) + ")";

		color = RGB565(R,G,B);
		EditColorHex->Text = "0x" + IntToHex((int64_t)color, 4);
	}
}
//---------------------------------------------------------------------------

void __fastcall TFormLuaApi::Button19Click(TObject *Sender)
{
	if (CheckBoxClear->Checked)
	{
		Memo1->Lines->Clear();
	}

	Memo1->Lines->Add("Lua GUI API �����б�");
	Memo1->Lines->Add("");
	Memo1->Lines->Add("��������RGB565(r, g, b)   --��RGB888��ɫת��ΪRGB565");
	Memo1->Lines->Add("  -- bg = RGB565(0xFF, 0, 0) --��ɫ");
	Memo1->Lines->Add("  -- lcd_clr(RGB565(0xFF, 0xFF, 0xFF)) --��������ɫ");
	Memo1->Lines->Add("");

	Memo1->Lines->Add("��������lcd_refresh()  --ˢ�½���");
	Memo1->Lines->Add("");

	Memo1->Lines->Add("��������lcd_clr(color)  --������color��RG565��ɫ��ʽ");
	Memo1->Lines->Add("  1��lcd_clr(RGB565(0xFF, 0xFF, 0xFF))");
	Memo1->Lines->Add("  2��lcd_clr(0x1234)");
	Memo1->Lines->Add("");

	Memo1->Lines->Add("��������lcd_disp_str(x, y, str, fontsize, front_color, back_color, width, align)");
	Memo1->Lines->Add("  -- ��ʾ�ַ���");
	Memo1->Lines->Add("  -- x, y ����ʾ����, ���ص�λ, 0-239");
	Memo1->Lines->Add("  -- str ���ַ���, ֧��GBK��������");
	Memo1->Lines->Add("  -- fontsize �������С. 12=12����16=16����24=24����32��ʾ32����");
	Memo1->Lines->Add("  -- front_color ��������ɫ, ������ RGB(0,0,0)");
	Memo1->Lines->Add("  -- back_color ��������ɫ");
	Memo1->Lines->Add("  -- width �����");
	Memo1->Lines->Add("  -- align ������. 0=�����, 1=����, 2=�Ҷ���");
	Memo1->Lines->Add("");

	Memo1->Lines->Add("��������lcd_fill_rect(x, y, h, w, color)");
	Memo1->Lines->Add("  -- �����δ���");
	Memo1->Lines->Add("  -- x, y ���������Ͻ�, ���ص�λ, 0-239");
	Memo1->Lines->Add("  -- h, w �����θ߶ȺͿ��, ���ص�λ, 1-240");
	Memo1->Lines->Add("  -- color �������ɫ");
	Memo1->Lines->Add("");

	Memo1->Lines->Add("��������lcd_draw_rect(x, y, h, w, color)");
	Memo1->Lines->Add("  -- ���ƾ��α߿�");
	Memo1->Lines->Add("  -- x, y ���������Ͻ�����, ���ص�λ, 0-239");
	Memo1->Lines->Add("  -- h, w �����θ߶ȺͿ��, ���ص�λ, 1-240");
	Memo1->Lines->Add("  -- color ���߿���ɫ");
	Memo1->Lines->Add("");

	Memo1->Lines->Add("��������lcd_draw_circle(x, y, r, color)");
	Memo1->Lines->Add("  -- ����ԲȦ");
	Memo1->Lines->Add("  -- x, y ��Բ������, ���ص�λ, 0-239");
	Memo1->Lines->Add("  -- r ��Բ�뾶, ���ص�λ");
	Memo1->Lines->Add("  -- color ���߿���ɫ");
	Memo1->Lines->Add("");

	Memo1->Lines->Add("��������lcd_draw_line(x1, y1, x2, y2, color)");
	Memo1->Lines->Add("  -- ����ֱ��");
	Memo1->Lines->Add("  -- x1, y1 ����1��������, ���ص�λ, 0-239");
	Memo1->Lines->Add("  -- x2, y2 ����2��������, ���ص�λ, 0-239");
	Memo1->Lines->Add("  -- color ��ֱ����ɫ");
	Memo1->Lines->Add("");

	Memo1->Lines->Add("��������lcd_disp_label(x, y, h, w, color, str, fontzize, front_color, back_color)");
	Memo1->Lines->Add("  --���Ʊ�ǩ(�������¾���)");
	Memo1->Lines->Add("  -- x, y ���������Ͻ�, ���ص�λ, 0-239");
	Memo1->Lines->Add("  -- h, w �����θ߶ȺͿ��, ���ص�λ, 1-240");
	Memo1->Lines->Add("  -- color �������ɫ");
	Memo1->Lines->Add("  -- str ���ַ���");
	Memo1->Lines->Add("  -- fontzize �������С 12 16 24 32");
	Memo1->Lines->Add("  -- front_color ��������ɫ");
	Memo1->Lines->Add("  -- back_color �����屳����ɫ");
	Memo1->Lines->Add("");
}
//---------------------------------------------------------------------------

void __fastcall TFormLuaApi::Button20Click(TObject *Sender)
{
	int i,k, m, n;
    AnsiString s;
	char aBuf[94*2];
    char tempbuf[94 * 2];

	if (CheckBoxClear->Checked)
	{
		Memo1->Lines->Clear();
	}

//	Memo1->Lines->Add("\
//    ��λ���ǹ��ұ�׼��Ϣ�����ú��ֱ���GB2312-80�к��ֵ���λ���롣\
//������λ�÷�Ϊ94����ÿ����94���ַ������ı����Ǵ�1~94,\
//λ�ı����1~64����λ��ĵ�1���ֽ�������,��2�ֽ���λ�롣\
//�����ֻ������е�����ASCII�ַ�����ȥ160������λ�롣\
//��16����1λ��Ӧ����(��)��֮ǰ���Ǹ���������ĸ����(ȫ��)��\r\n\
//	����һ���ֿ��������Ϊ55, ���ָ���Ϊ: (55-16+1)x94 = 3760\r\n\
//    ����һ�����ֿ��������Ϊ87, ���ָ���Ϊ: (87-16+1)x94 = 6768\
//");


    for(i=0;i<128;i++){
        if(i<' '){
            aBuf[i]=' ';
        }
		else{
            aBuf[i]=i;
        }
    }
    aBuf[128]=0;
  //  s.sprintf(aBuf);
  //  FormView->Image1->Canvas->TextOutA(0,0,s);

	Memo1->Lines->Add("");
	Memo1->Lines->Add("��ASCII����");
    m = 0;
    for(i=1;i<128;i++)
    {
        tempbuf[m++] = i;
    }
	tempbuf[m++] = 0;
	Memo1->Lines->Add((AnsiString)tempbuf);

	Memo1->Lines->Add("");
	Memo1->Lines->Add("��ȫ���ַ�����");
    for(i=0;i<9;i++){
        m = 0;
        for(k=0;k<94;k++){
            aBuf[2*k]=i+1+160;
            aBuf[2*k+1]=k+1+160;

            tempbuf[m++] = i+1+160;
            tempbuf[m++] = k+1+160;
        }
        tempbuf[m++] = 0;

		Memo1->Lines->Add((AnsiString)tempbuf);
       //s.sprintf(aBuf);
        //FormView->Image1->Canvas->TextOutA(0,(i+1)*12,s);
    }

//	Memo1->Lines->Add("");
//	Memo1->Lines->Add("����������");
//    for(i=16;i<88;i++){
//    	m = 0;
//        for(k=0;k<94;k++){
//            aBuf[2*k]=i+160;
//            aBuf[2*k+1]=k+1+160;
//
//            tempbuf[m++] = i+160;
//            tempbuf[m++] = k+1+160;
//        }
//        tempbuf[m++] = 0;
//
//        Memo1->Lines->Add((AnsiString)tempbuf);
//        //s.sprintf(aBuf);
//        //FormView->Image1->Canvas->TextOutA(0,(i-6)*12,s);
//	}
}
//---------------------------------------------------------------------------


