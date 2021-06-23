﻿//---------------------------------------------------------------------------
// 串口控件演示程序，Copyright © Victor Chen

#ifndef Unit1H
#define Unit1H
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ExtCtrls.hpp>
#include "YbCommDevice.h"
//---------------------------------------------------------------------------
class TForm1 : public TForm
{
__published:	// IDE-managed Components
    TEdit *Edit1;
    TButton *ButtonSend;
    TLabel *Label1;
    TLabel *Label2;
    TButton *ButtonSet;
    TTimer *Timer1;
    TYbCommDevice *YbCommDevice1;
    TMemo *Memo1;
    void __fastcall ButtonSetClick(TObject *Sender);
    void __fastcall ButtonSendClick(TObject *Sender);
    void __fastcall Timer1Timer(TObject *Sender);
private:	// User declarations
public:		// User declarations
    __fastcall TForm1(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TForm1 *Form1;
//---------------------------------------------------------------------------
#endif
 