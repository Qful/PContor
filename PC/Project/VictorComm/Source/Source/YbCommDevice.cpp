﻿/***************************************************************************\
*                                                                           *
*              YbCommDevice.cpp - 串行口通讯控件, 版本 1.5.0.5              *
*                            Built: Jul. 10, 2015                           *
*                     Copyright © 1997-2015, Victor Chen                    *
*                      Homepage: http://www.cppfans.com                     *
*                          Email: victor@cppfans.com                        *
*                                                                           *
\***************************************************************************/

#pragma hdrstop

#include "YbCommDevice.h"
#include "TVictorCommPkg.h"
#include "Unit_YbCommDevice_Settings.h"
#pragma package(smart_init)
//---------------------------------------------------------------------------
namespace VictorComm {
//---------------------------------------------------------------------------
//////////////////////////// TYbCommFrameSettings ///////////////////////////
//---------------------------------------------------------------------------
__fastcall TYbCommFrameSettings::TYbCommFrameSettings(TCommPackage *lpPkg)
{
  _CommPkg = lpPkg;
}
//---------------------------------------------------------------------------
void __fastcall TYbCommFrameSettings::SetValues(TYbCommFrameSettings *lpfs)
{
  FrameHead = lpfs->FrameHead;
  FrameTail = lpfs->FrameTail;
  FrameCtrl = lpfs->FrameCtrl;
}
//---------------------------------------------------------------------------
bool __fastcall TYbCommFrameSettings::fGetDblFrMark(void)const { return _CommPkg->DoubleFrameMark; }
void __fastcall TYbCommFrameSettings::fSetDblFrMark(bool b)    { _CommPkg->DoubleFrameMark = b;    }
//---------------------------------------------------------------------------
unsigned char __fastcall TYbCommFrameSettings::fGetFrameHead(void)const { return _CommPkg->FrameHead; }
unsigned char __fastcall TYbCommFrameSettings::fGetFrameTail(void)const { return _CommPkg->FrameTail; }
unsigned char __fastcall TYbCommFrameSettings::fGetFrameCtrl(void)const { return _CommPkg->FrameCtrl; }
//---------------------------------------------------------------------------
void __fastcall TYbCommFrameSettings::fSetFrameHead(unsigned char c) { _CommPkg->FrameHead = c; }
void __fastcall TYbCommFrameSettings::fSetFrameTail(unsigned char c) { _CommPkg->FrameTail = c; }
void __fastcall TYbCommFrameSettings::fSetFrameCtrl(unsigned char c) { _CommPkg->FrameCtrl = c; }
//---------------------------------------------------------------------------
//////////////////////////// TYbCustomCommDevice ////////////////////////////
//---------------------------------------------------------------------------
__fastcall TYbCustomCommDevice::TYbCustomCommDevice(TComponent* Owner)
                               :TComponent(Owner)
{
  _bUsePackage = false; //std. port
  _bSyncEvents = true; //synchronize events to main thread

  _lpfnPackageNotify = NULL; //no default
  _lpfnCommNotify    = NULL; //no default
  _lpfnAfterOpen     = NULL; //no default
  _lpfnBeforeClose   = NULL; //no default

  _hNotifyWindow = AllocateHWnd(_EvNotifyProc);

  _CommPort = new TVictorComm;
  _CommPort->OnCommNotify = _EvComm;
  _CommPort->AfterOpen    = _EvAfterOpen;
  _CommPort->BeforeClose  = _EvBeforeClose;

  _CommPackage = new TCommPackage(_CommPort);
  _CommPackage->OnCommNotify = _EvPackage;

  _FrameSettings = new TYbCommFrameSettings(_CommPackage);
}
//---------------------------------------------------------------------------

__fastcall TYbCustomCommDevice::~TYbCustomCommDevice()
{
  _CommPackage->OnCommNotify = NULL;
  _CommPackage->Active = false;

  _CommPort->BeforeClose  = NULL;
  _CommPort->AfterOpen    = NULL;
  _CommPort->OnCommNotify = NULL;
  _CommPort->Active = false;

  delete _FrameSettings;
  delete _CommPackage;
  delete _CommPort;

  DeallocateHWnd(_hNotifyWindow);
}
//---------------------------------------------------------------------------

void __fastcall TYbCustomCommDevice::fSetUsePackage(bool bup)
{
  bool bAt = fGetActive();
  _bUsePackage = bup;
  fSetActive(bAt);
}
//---------------------------------------------------------------------------
void __fastcall TYbCustomCommDevice::fSetFrameSettings(TYbCommFrameSettings *fs)
{
  _FrameSettings->SetValues(fs);
}

//---------------------------------------------------------------------------
bool __fastcall TYbCustomCommDevice::fGetActive(void)const
{
  if(!_bUsePackage)
    return _CommPort->Active;

  if(_CommPackage->Active)
   if(_CommPort->Active)
    return true;

  return false;
}
//---------------------------------------------------------------------------

void __fastcall TYbCustomCommDevice::fSetActive(bool b)
{
  bool bpka = _bUsePackage?b:false;

  if(_CommPort->Active != b)
    _CommPort->Active = b;

  if(_CommPackage->Active != bpka)
    _CommPackage->Active = bpka;
}
//---------------------------------------------------------------------------

HANDLE __fastcall TYbCustomCommDevice::fGetHandle(void)const
{
  return _CommPort->Handle;
}
//---------------------------------------------------------------------------
TCommQueue *__fastcall TYbCustomCommDevice::fGetInQueue(void)const
{
  return _CommPort->InQueue;
}
//---------------------------------------------------------------------------
TCommQueue *__fastcall TYbCustomCommDevice::fGetOutQueue(void)const
{
  return _CommPort->OutQueue;
}
//---------------------------------------------------------------------------
int __fastcall TYbCustomCommDevice::fGetPort(void)const
{
  return _CommPort->PortNo;
}
//---------------------------------------------------------------------------

void __fastcall TYbCustomCommDevice::fSetPort(int n)
{
  _CommPort->PortNo = n;
}
//---------------------------------------------------------------------------
UnicodeString __fastcall TYbCustomCommDevice::fGetPortName(void)const
{
  return _CommPort->PortName;
}
//---------------------------------------------------------------------------
void __fastcall TYbCustomCommDevice::fSetPortName(UnicodeString s)
{
  _CommPort->PortName = s;
}
//---------------------------------------------------------------------------
TYbCustomCommDevice::TBaudRate __fastcall TYbCustomCommDevice::fGetBaud(void)const
{
  switch(_CommPort->Baud)
   {
     case CBR_110   : return br110   ;
     case CBR_300   : return br300   ;
     case CBR_600   : return br600   ;
     case CBR_1200  : return br1200  ;
     case CBR_2400  : return br2400  ;
     case CBR_4800  : return br4800  ;
     case CBR_9600  : return br9600  ;
     case CBR_14400 : return br14400 ;
     case CBR_19200 : return br19200 ;
     case CBR_38400 : return br38400 ;
     case CBR_56000 : return br56000 ;
     case CBR_57600 : return br57600 ;
     case CBR_115200: return br115200;
	 case CBR_128000: return br128000;

	 case 230400: return   br230400;
	 case 256000: return   br256000;
	 case 460800: return   br460800;
	 case 921600: return   br921600;
	 case 2000000: return   br2000000;
	 case 3000000: return   br3000000;
   }
  return br115200; //default value
  //return _CommPort->Baud;
}
//---------------------------------------------------------------------------

void __fastcall TYbCustomCommDevice::fSetBaud(TYbCustomCommDevice::TBaudRate br)
{
  switch(br)
   {
     case br110   :_CommPort->Baud = CBR_110   ; return;
     case br300   :_CommPort->Baud = CBR_300   ; return;
     case br600   :_CommPort->Baud = CBR_600   ; return;
     case br1200  :_CommPort->Baud = CBR_1200  ; return;
     case br2400  :_CommPort->Baud = CBR_2400  ; return;
     case br4800  :_CommPort->Baud = CBR_4800  ; return;
     case br9600  :_CommPort->Baud = CBR_9600  ; return;
     case br14400 :_CommPort->Baud = CBR_14400 ; return;
     case br19200 :_CommPort->Baud = CBR_19200 ; return;
     case br38400 :_CommPort->Baud = CBR_38400 ; return;
     case br56000 :_CommPort->Baud = CBR_56000 ; return;
     case br57600 :_CommPort->Baud = CBR_57600 ; return;
     case br115200:_CommPort->Baud = CBR_115200; return;
     case br128000:_CommPort->Baud = CBR_128000; return;

	 case br230400:_CommPort->Baud = 230400; return;
	 case br256000:_CommPort->Baud = 256000; return;
	 case br460800:_CommPort->Baud = 460800; return;
	 case br921600:_CommPort->Baud = 921600; return;
	 case br2000000:_CommPort->Baud = 2000000; return;
	 case br3000000:_CommPort->Baud = 3000000; return;
   }
  _CommPort->Baud = CBR_115200;
}
//---------------------------------------------------------------------------

TYbCustomCommDevice::TParity __fastcall TYbCustomCommDevice::fGetParity(void)const
{
  switch(_CommPort->Parity)
   {
     case NOPARITY   : return TYbCustomCommDevice::ptNoParity   ;
     case ODDPARITY  : return TYbCustomCommDevice::ptOddParity  ;
     case EVENPARITY : return TYbCustomCommDevice::ptEvenParity ;
     case MARKPARITY : return TYbCustomCommDevice::ptMarkParity ;
     case SPACEPARITY: return TYbCustomCommDevice::ptSpaceParity;
   }
  return TYbCustomCommDevice::ptNoParity;
}
//---------------------------------------------------------------------------

void __fastcall TYbCustomCommDevice::fSetParity(TYbCustomCommDevice::TParity pt)
{
  switch(pt)
   {
     case TYbCustomCommDevice::ptNoParity   : _CommPort->Parity = NOPARITY   ; return;
     case TYbCustomCommDevice::ptOddParity  : _CommPort->Parity = ODDPARITY  ; return;
     case TYbCustomCommDevice::ptEvenParity : _CommPort->Parity = EVENPARITY ; return;
     case TYbCustomCommDevice::ptMarkParity : _CommPort->Parity = MARKPARITY ; return;
     case TYbCustomCommDevice::ptSpaceParity: _CommPort->Parity = SPACEPARITY; return;
   }
  _CommPort->Parity = NOPARITY;
}
//---------------------------------------------------------------------------

int __fastcall TYbCustomCommDevice::fGetByteSize(void)const
{
  return _CommPort->ByteSize;
}
//---------------------------------------------------------------------------

void __fastcall TYbCustomCommDevice::fSetByteSize(int bs)
{
  if(bs<0)bs=0;
  if(bs>255)bs=255;
  _CommPort->ByteSize = bs;
}
//---------------------------------------------------------------------------

TYbCustomCommDevice::TStopBits __fastcall TYbCustomCommDevice::fGetStopBits(void)const
{
  switch(_CommPort->StopBits)
   {
     case ONESTOPBIT  : return TYbCustomCommDevice::sbOneStopBit    ;//1 stop bit
     case ONE5STOPBITS: return TYbCustomCommDevice::sbOne_5_StopBits;//1.5 stop bits
     case TWOSTOPBITS : return TYbCustomCommDevice::sbTwoStopBit    ;//2 stop bits
   }
  return TYbCustomCommDevice::sbOneStopBit;
}
//---------------------------------------------------------------------------

void __fastcall TYbCustomCommDevice::fSetStopBits(TYbCustomCommDevice::TStopBits sb)
{
  switch(sb)
   {
     case TYbCustomCommDevice::sbOneStopBit    : _CommPort->StopBits = ONESTOPBIT  ; return; //1 stop bit
     case TYbCustomCommDevice::sbOne_5_StopBits: _CommPort->StopBits = ONE5STOPBITS; return; //1.5 stop bits
     case TYbCustomCommDevice::sbTwoStopBit    : _CommPort->StopBits = TWOSTOPBITS ; return; //2 stop bits
   }
  _CommPort->StopBits = ONESTOPBIT;
}
//---------------------------------------------------------------------------

TYbCustomCommDevice::TFlowControl __fastcall TYbCustomCommDevice::fGetFlCtrl(void)const
{
  switch(_CommPort->FlowControl)
   {
     case TVictorComm::fcNone         : return TYbCustomCommDevice::fcNone         ;
     case TVictorComm::fcRtsCts       : return TYbCustomCommDevice::fcRtsCts       ;
     case TVictorComm::fcXonXoff      : return TYbCustomCommDevice::fcXonXoff      ;
     case TVictorComm::fcTranXonXoff  : return TYbCustomCommDevice::fcTranXonXoff  ;
     case TVictorComm::fcRtsCtsXonXoff: return TYbCustomCommDevice::fcRtsCtsXonXoff;
   }
  return TYbCustomCommDevice::fcNone; //default value
}
//---------------------------------------------------------------------------
void __fastcall TYbCustomCommDevice::fSetFlCtrl(TYbCustomCommDevice::TFlowControl fc)
{
  switch(fc)
   {
     case TYbCustomCommDevice::fcNone         : _CommPort->FlowControl = TVictorComm::fcNone         ; return;
     case TYbCustomCommDevice::fcRtsCts       : _CommPort->FlowControl = TVictorComm::fcRtsCts       ; return;
     case TYbCustomCommDevice::fcXonXoff      : _CommPort->FlowControl = TVictorComm::fcXonXoff      ; return;
     case TYbCustomCommDevice::fcTranXonXoff  : _CommPort->FlowControl = TVictorComm::fcTranXonXoff  ; return;
     case TYbCustomCommDevice::fcRtsCtsXonXoff: _CommPort->FlowControl = TVictorComm::fcRtsCtsXonXoff; return;
   }
  _CommPort->FlowControl = TVictorComm::fcNone;
}
//---------------------------------------------------------------------------

int  __fastcall TYbCustomCommDevice::fGetAutAns(void)const
{
  return _CommPort->AutoAnswer;
}
//---------------------------------------------------------------------------
void __fastcall TYbCustomCommDevice::fSetAutAns(int iaa)
{
  if(iaa<0)iaa=0;
  if(iaa>255)iaa=255;
  _CommPort->AutoAnswer=iaa;
}
//---------------------------------------------------------------------------

TYbCustomCommDevice::TModemStatus __fastcall TYbCustomCommDevice::fGetModemStatus(void)const
{
  TYbCustomCommDevice::TModemStatus ms;
  DWORD dwMs = _CommPort->ModemStatus;

  if(dwMs & MS_CTS_ON ) ms << msCtsOn ;
  if(dwMs & MS_DSR_ON ) ms << msDsrOn ;
  if(dwMs & MS_RING_ON) ms << msRingOn;
  if(dwMs & MS_RLSD_ON) ms << msRlsdOn;

  return ms;
}
//---------------------------------------------------------------------------
bool __fastcall TYbCustomCommDevice::fGetDisableWrite(void)const
{
  return _CommPort->DisableWrite;
}
//---------------------------------------------------------------------------
void __fastcall TYbCustomCommDevice::fSetDisableWrite(bool bdw)
{
  _CommPort->DisableWrite = bdw;
}
//---------------------------------------------------------------------------
void __fastcall TYbCustomCommDevice::fSetSyncEvents(bool b)
{
  _bSyncEvents = b;
}
//---------------------------------------------------------------------------
bool __fastcall TYbCustomCommDevice::fGetDataOnly(void)const
{
  return _CommPort->DataOnly;
}
//---------------------------------------------------------------------------
void __fastcall TYbCustomCommDevice::fSetDataOnly(bool b)
{
  _CommPort->DataOnly = b;
}
//---------------------------------------------------------------------------
long __fastcall TYbCustomCommDevice::fGetInBufSize(void)const
{
  return _CommPort->InBufSize;
}
//---------------------------------------------------------------------------
void __fastcall TYbCustomCommDevice::fSetInBufSize(long ibs)
{
  _CommPort->InBufSize = ibs;
}
//---------------------------------------------------------------------------
long __fastcall TYbCustomCommDevice::fGetOutBufSize(void)const
{
  return _CommPort->OutBufSize;
}
//---------------------------------------------------------------------------
void __fastcall TYbCustomCommDevice::fSetOutBufSize(long obs)
{
  _CommPort->OutBufSize = obs;
}
//---------------------------------------------------------------------------

long __fastcall TYbCustomCommDevice::fGetHwInSize(void)const
{
  return _CommPort->HwInSize;
}
//---------------------------------------------------------------------------
void __fastcall TYbCustomCommDevice::fSetHwInSize(long his)
{
  _CommPort->HwInSize = his;
}
//---------------------------------------------------------------------------
long __fastcall TYbCustomCommDevice::fGetHwOutSize(void)const
{
  return _CommPort->HwOutSize;
}
//---------------------------------------------------------------------------
void __fastcall TYbCustomCommDevice::fSetHwOutSize(long hos)
{
  _CommPort->HwOutSize = hos;
}
//---------------------------------------------------------------------------

bool __fastcall TYbCustomCommDevice::fGetDTR(void)const //Computer status, read/write
{
  return _CommPort->DTR;
}
//---------------------------------------------------------------------------
void __fastcall TYbCustomCommDevice::fSetDTR(bool b) //Computer status, read/write
{
  _CommPort->DTR = b;
}
//---------------------------------------------------------------------------
bool __fastcall TYbCustomCommDevice::fGetRTS(void)const //Computer status, read/write
{
  return _CommPort->RTS;
}
//---------------------------------------------------------------------------
void __fastcall TYbCustomCommDevice::fSetRTS(bool b) //Computer status, read/write
{
  _CommPort->RTS = b;
}
//---------------------------------------------------------------------------
bool __fastcall TYbCustomCommDevice::fGetCTS(void)const //Modem status, read-only
{
  return _CommPort->CTS;
}
//---------------------------------------------------------------------------
bool __fastcall TYbCustomCommDevice::fGetDSR(void)const //Modem status, read-only
{
  return _CommPort->DSR;
}
//---------------------------------------------------------------------------
bool __fastcall TYbCustomCommDevice::fGetRING(void)const //Modem status, read-only
{
  return _CommPort->RING;
}
//---------------------------------------------------------------------------
bool __fastcall TYbCustomCommDevice::fGetRLSD(void)const //Modem status, read-only
{
  return _CommPort->RLSD;
}
//---------------------------------------------------------------------------

long __fastcall TYbCustomCommDevice::fGetQSize(void)const
{
  return _CommPackage->QueueSize;
}
//---------------------------------------------------------------------------
void __fastcall TYbCustomCommDevice::fSetQSize(long qs)
{
  _CommPackage->QueueSize = qs;
}
//---------------------------------------------------------------------------

long  __fastcall TYbCustomCommDevice::fGetPkgSz(void)const
{
  return _CommPackage->PackageSize;
}
//---------------------------------------------------------------------------
void __fastcall TYbCustomCommDevice::fSetPkgSz(long ps)
{
  _CommPackage->PackageSize = ps;
}
//---------------------------------------------------------------------------

TCommPackageType __fastcall TYbCustomCommDevice::fGetPkgType(void)const
{
  return _CommPackage->PackageType;
}
//---------------------------------------------------------------------------
void __fastcall TYbCustomCommDevice::fSetPkgType(TCommPackageType pt)
{
  _CommPackage->PackageType = pt;
}
//---------------------------------------------------------------------------

long __fastcall TYbCustomCommDevice::fGetPkgInterval(void)const
{
  return _CommPackage->PackageInterval;
}
//---------------------------------------------------------------------------
void __fastcall TYbCustomCommDevice::fSetPkgInterval(long i)
{
  _CommPackage->PackageInterval = i;
}
//---------------------------------------------------------------------------

void __fastcall TYbCustomCommDevice::_EvNotifyProc(Messages::TMessage &Message)
{
  switch(Message.Msg)
   {
     case nmtCommMsg: EvCommNotify((TObject*)Message.WParam, Message.LParam); break;
     case nmtPackage: EvPackageNotify((TObject*)Message.WParam, Message.LParam); break;
   }
  Message.Result = DefWindowProc(_hNotifyWindow, Message.Msg, Message.WParam, Message.LParam);
}
//---------------------------------------------------------------------------

void __fastcall TYbCustomCommDevice::_EvComm(TObject *Sender, int NotifyType)
{
  if(_bSyncEvents)
    PostMessage(_hNotifyWindow, nmtCommMsg, (WPARAM)Sender, (LPARAM)NotifyType);
  else
    EvCommNotify(Sender, NotifyType);
}
//---------------------------------------------------------------------------

void __fastcall TYbCustomCommDevice::_EvPackage(TObject *Sender, int NotifyType)
{
  if(_bSyncEvents)
    PostMessage(_hNotifyWindow, nmtPackage, (WPARAM)Sender, (LPARAM)NotifyType);
  else
    EvPackageNotify(Sender, NotifyType);
}
//---------------------------------------------------------------------------

void __fastcall TYbCustomCommDevice::_EvAfterOpen(TObject *Sender)
{
  EvAfterOpen(Sender);
}
//---------------------------------------------------------------------------

void __fastcall TYbCustomCommDevice::_EvBeforeClose(TObject *Sender)
{
  EvBeforeClose(Sender);
}
//---------------------------------------------------------------------------

void __fastcall TYbCustomCommDevice::EvCommNotify(TObject *Sender, int NotifyType)
{
  if(_lpfnCommNotify)
   {
     _lpfnCommNotify(this, NotifyType);
   }
}
//---------------------------------------------------------------------------

void __fastcall TYbCustomCommDevice::EvPackageNotify(TObject *Sender, int NotifyType)
{
  if(_lpfnPackageNotify)
   {
     _lpfnPackageNotify(this, NotifyType);
   }
}
//---------------------------------------------------------------------------

void __fastcall TYbCustomCommDevice::EvAfterOpen(TObject *Sender)
{
  if(_lpfnAfterOpen)
   {
     _lpfnAfterOpen(this);
   }
}
//---------------------------------------------------------------------------

void __fastcall TYbCustomCommDevice::EvBeforeClose(TObject *Sender)
{
  if(_lpfnBeforeClose)
   {
     _lpfnBeforeClose(this);
   }
}
//---------------------------------------------------------------------------

long __fastcall TYbCustomCommDevice::Read(void far *s, long n)
{
  if(_bUsePackage)
    return _CommPackage->ReadPackage(s,n);
  return _CommPort->Read(s,n);
}
//---------------------------------------------------------------------------

long __fastcall TYbCustomCommDevice::Write(const void far *s, long n)
{
  if(_bUsePackage)
    return _CommPackage->WritePackage(s,n);
  return _CommPort->Write(s,n);
}
//---------------------------------------------------------------------------

long __fastcall TYbCustomCommDevice::Command(const char far *s)
{
  return _CommPort->Command(s);
}
//---------------------------------------------------------------------------

bool __fastcall TYbCustomCommDevice::ReadPackage(void *lpPkg)
{
  return _CommPackage->ReadPackage(lpPkg,0)!=0;
}
//---------------------------------------------------------------------------

bool __fastcall TYbCustomCommDevice::WritePackage(const void *lpPkg)
{
  return _CommPackage->WritePackage(lpPkg,0)!=0;
}
//---------------------------------------------------------------------------

unsigned long __fastcall TYbCustomCommDevice::ReadPackage(void *lpPkg, unsigned long BufSize)
{
  return _CommPackage->ReadPackage(lpPkg, BufSize);
}
//---------------------------------------------------------------------------

unsigned long __fastcall TYbCustomCommDevice::WritePackage(const void *lpPkg, unsigned long nBytes)
{
  return _CommPackage->WritePackage(lpPkg, nBytes);
}
//---------------------------------------------------------------------------

void __fastcall TYbCustomCommDevice::PurgeRead(bool bAbort)
{
  if(_bUsePackage)
    _CommPackage->PurgeRead();
  else
    _CommPort->PurgeRead(bAbort);
}
//---------------------------------------------------------------------------

void __fastcall TYbCustomCommDevice::PurgeWrite(bool bAbort)
{
  if(_bUsePackage)
    _CommPackage->PurgeWrite();
  else
    _CommPort->PurgeWrite(bAbort);
}
//---------------------------------------------------------------------------

bool __fastcall TYbCustomCommDevice::SettingsDialog(TComponent* AOwner, bool FActive)
{
  TForm_YbCommDevice_Settings *FormSettings = new TForm_YbCommDevice_Settings(AOwner,this,FActive);
  bool bOK = FormSettings->ShowModal() == IDOK;
  delete FormSettings;
  return bOK;
}
//---------------------------------------------------------------------------

void __fastcall TYbCustomCommDevice::ResetModem(void)
{
  _CommPort->ResetModem();
}
//---------------------------------------------------------------------------

void __fastcall TYbCustomCommDevice::InitModem(void)
{
  _CommPort->InitModem();
}
//---------------------------------------------------------------------------
BOOL __fastcall TYbCustomCommDevice::EscapeCommFunction(DWORD dwFunc)
{
  return _CommPort->EscapeCommFunction(dwFunc);
}

//---------------------------------------------------------------------------
/////////////////////////////// TYbCommDevice ///////////////////////////////
//---------------------------------------------------------------------------
__fastcall TYbCommDevice::TYbCommDevice(TComponent* Owner)
                         :TYbCustomCommDevice(Owner)
{
  //do nothing
}
//---------------------------------------------------------------------------
} // namespace VictorComm
//---------------------------------------------------------------------------
