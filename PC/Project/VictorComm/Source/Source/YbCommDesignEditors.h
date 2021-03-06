﻿//---------------------------------------------------------------------------

#ifndef YbCommDesignEditorsH
#define YbCommDesignEditorsH
//---------------------------------------------------------------------------
#if(__BCPLUSPLUS__>=0x0560)
# include <DesignEditors.hpp>
#else
# include <dsgnintf.hpp>
#endif
//---------------------------------------------------------------------------
namespace Ybcommdesigneditors {
//---------------------------------------------------------------------------
class PACKAGE TYbCommDeviceEditor:public TComponentEditor
{
public:
  void          __fastcall ExecuteVerb(int Index);
  UnicodeString __fastcall GetVerb(int Index);
  int           __fastcall GetVerbCount(void);
};
//---------------------------------------------------------------------------
} using namespace Ybcommdesigneditors;
//---------------------------------------------------------------------------
#endif
