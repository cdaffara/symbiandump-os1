// Copyright (c) 1995-2009 Nokia Corporation and/or its subsidiary(-ies).
// All rights reserved.
// This component and the accompanying materials are made available
// under the terms of "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
//
// Initial Contributors:
// Nokia Corporation - initial contribution.
//
// Contributors:
//
// Description:
// Test code to tun ANIMDLL.DLL
// 
//

#include <e32std.h>
#include <w32std.h>
#include <e32svr.h>
#include "../tlib/testbase.h"
#include "TANIM.H"
#include "ANIMDLL.H"

#define NONEXISTENT_DLL_NAME	_L("Z:\\NONEXISTENT.DLL")
#define ANIM_BITMAP		_L("Z:\\WSTEST\\TANIM.MBM")
#define ANIM_DLL_NAME	_L("ANIDLL.DLL")

enum TAnimPanic
	{
	EAnimPanicBadLoadFailed,
	};

class CTAnimClient;

class TestAnim : public RAnim
	{
public:
	TestAnim(RAnimDll &aDll);
	TInt Construct(RWindowBase &aWin,const TPoint &aPos,CFbsBitmap *aBit1,CFbsBitmap *aBit2,CFbsBitmap *aMask,TInt aInterval, CFbsFont *aFont);
	void Set(const TPoint &aPos, TInt aInterval);
	void TextPos(const TPoint &aPos);
	void SetPolyRectL(const TRect &aRect);
	void ToggleMask();
	};

class CTAnimWindow2 : public CTBlankWindow
	{
public:
	CTAnimWindow2(RAnimDll &aAnimDll);
	~CTAnimWindow2();
	void ConstructL(CTWinBase &parent, const TPoint &aPos, const TSize &aSize);
	void ConstructExtLD(CTWinBase &aParent, const TPoint &aPos, const TSize &aSize);
private:
	TestAnim iAnim;
	CFbsBitmap iAnimBitmap1;
	CFbsBitmap iAnimBitmap2;
	};

class CTAnimWindow1 : public CTTitledWindow
	{
public:
	CTAnimWindow1(RAnimDll &aAnimDll);
	~CTAnimWindow1();
	void ConstructL(CTWinBase &parent);
	void Draw();
	void AdjustAnimRect(TInt xMove,TInt yMove,TInt modifiers);
	void AdjustPolyRectL(TInt xMove,TInt yMove,TInt modifiers);
	void ToggleMask();
	TInt SubType();
private:
	TestAnim iAnim;
	TPoint iAnimPos;
	TRect iAnimPolyRect;
	CFbsBitmap iAnimBitmap1;
	CFbsBitmap iAnimBitmap2;
	CFbsBitmap iMask;
	};

class CTAnimWindowGroup : public CTWindowGroup
	{
public:
	CTAnimWindowGroup(CTClient *aClient);
	void CursorKeyL(TInt xMove,TInt yMove,TInt modifiers);
	void KeyL(const TKeyEvent &aKey, const TTime &aTime);
	TInt iCursorMode;
	};

class CTAnimClient : public CTClient
	{
public:
	CTAnimClient();
	~CTAnimClient();
	void ConstructL();
	void KeyL(const TKeyEvent &aKey,const TTime &aTime);
	void ExitL();
	void ErrorTests();
	CTBaseWin *CreateTestWindowL(TPoint pos,TSize size,CTWinBase *parent, TInt aType);
private:
	TInt iNum;
	RAnimDll iAnimDll;
	};

const TInt Xmove=8;
const TInt Ymove=6;

void AnimPanic(TInt aPanic)
	{
	User::Panic(_L("ANIM"),aPanic);
	}

//
//  Anim class
//

TestAnim::TestAnim(RAnimDll &aDll) : RAnim(aDll)
	{
	}

TInt TestAnim::Construct(RWindowBase &aWin, const TPoint &aPos, CFbsBitmap *aBit1,CFbsBitmap *aBit2,CFbsBitmap *aMask, TInt aInterval, CFbsFont *aFont)
	{
	TPckgBuf<CTAParams> params;
	params().pos=aPos;
	params().interval=aInterval;
	params().bit1=aBit1->Handle();
	params().bit2=aBit2->Handle();
	params().mask=aMask->Handle();
	params().font=aFont->Handle();
	return(RAnim::Construct(aWin,0,params));
	}

void TestAnim::Set(const TPoint &aPos, TInt aInterval)
	{
	TPckgBuf<CTAParams> params;
	params().pos=aPos;
	params().interval=aInterval;
	Command(EADllOpcodeMove,params);
	}

void TestAnim::TextPos(const TPoint &aPos)
	{
	TPckgBuf<TPoint> params;
	params()=aPos;
	Command(EADllTextPos,params);
	}

void TestAnim::SetPolyRectL(const TRect &aRect)
	{
	TPckgC<TRect> params(aRect);
	User::LeaveIfError(CommandReply(EADllOpcodePolyLineRect,params));
	}

void TestAnim::ToggleMask()
	{
	Command(EADllToggleBitmapMask);
	}

//
// Individual window sub-classes
//

CTAnimWindow1::CTAnimWindow1(RAnimDll &aAnimDll) : CTTitledWindow(), 
	iAnim(aAnimDll)
	{
	}

CTAnimWindow1::~CTAnimWindow1()
	{
	iAnim.Close();
	}

void CTAnimWindow1::ConstructL(CTWinBase &parent)
	{
	CTTitledWindow::ConstructL(parent);
	User::LeaveIfError(iAnimBitmap1.Load(ANIM_BITMAP,EMbmTanimAnim1));
	User::LeaveIfError(iAnimBitmap2.Load(ANIM_BITMAP,EMbmTanimAnim2));
	User::LeaveIfError(iMask.Load(ANIM_BITMAP,EMbmTanimMask));
	iAnimPos=TPoint(10,10);
	User::LeaveIfError(iAnim.Construct(iWin, iAnimPos,&iAnimBitmap1,&iAnimBitmap2,&iMask,1,iFont));
	iAnimPolyRect=TRect(TPoint(10,40),TSize(50,50));
	iAnim.SetPolyRectL(iAnimPolyRect);
	iAnim.TextPos(TPoint(20,50));
	}

void CTAnimWindow1::Draw()
	{
	TInt i;

	CTTitledWindow::Draw();
	iGc->SetPenColor(TRgb::Gray16(8));
	TInt hgt=iSize.iHeight-iTitleHeight;
	for(i=0;i<iSize.iWidth;i+=30)
		{
		iGc->DrawLine(TPoint(i,iTitleHeight),TPoint(iSize.iWidth,iTitleHeight+(hgt*i)/iSize.iWidth));
		iGc->DrawLine(TPoint(iSize.iWidth-i,iTitleHeight+hgt),TPoint(iSize.iWidth,iTitleHeight+(hgt*i)/iSize.iWidth));
		iGc->DrawLine(TPoint(iSize.iWidth-i,iTitleHeight+hgt),TPoint(0,iTitleHeight+hgt-(hgt*i)/iSize.iWidth));
		iGc->DrawLine(TPoint(i,iTitleHeight),TPoint(0,iTitleHeight+hgt-(hgt*i)/iSize.iWidth));
		}
	}

void CTAnimWindow1::AdjustAnimRect(TInt xMove,TInt yMove,TInt modifiers)
	{
	if (modifiers&EModifierCtrl)	// 4 times the movement
		{
		xMove<<=2;
		yMove<<=2;
		}
	iAnimPos+=TPoint(xMove,yMove);
	iAnim.Set(iAnimPos,5);
	}

void CTAnimWindow1::AdjustPolyRectL(TInt xMove,TInt yMove,TInt modifiers)
	{
	if (modifiers&EModifierCtrl)	// 4 times the movement
		{
		xMove<<=2;
		yMove<<=2;
		}
	if (modifiers&EModifierShift)	// 4 times the movement
		iAnimPolyRect.Grow(xMove,yMove);
	else
		iAnimPolyRect.Move(TPoint(xMove,yMove));
	iAnim.SetPolyRectL(iAnimPolyRect);
	}

void CTAnimWindow1::ToggleMask()
	{
	iAnim.ToggleMask();
	}

TInt CTAnimWindow1::SubType()
	{
	return(69);
	}

// CTAnimWindow2 //

CTAnimWindow2::CTAnimWindow2(RAnimDll &aAnimDll) : CTBlankWindow(), 
	iAnim(aAnimDll)
	{
	}

CTAnimWindow2::~CTAnimWindow2()
	{
	iAnim.Close();
	}

void CTAnimWindow2::ConstructL(CTWinBase &parent, const TPoint &aPos, const TSize &aSize)
	{
	CTBlankWindow::ConstructL(parent);
	iWin.SetColor(TRgb(128,128,128));
	iWin.SetVisible(EFalse);
	SetExt(aPos,aSize);
	iWin.Activate();
	User::LeaveIfError(iAnimBitmap1.Load(ANIM_BITMAP,EMbmTanimAnim1));
	User::LeaveIfError(iAnimBitmap2.Load(ANIM_BITMAP,EMbmTanimAnim2));
	User::LeaveIfError(iAnim.Construct(iWin, TPoint(10,10), &iAnimBitmap1, &iAnimBitmap2, &iAnimBitmap1, 1, iFont));
	iWin.SetVisible(ETrue);
	}

void CTAnimWindow2::ConstructExtLD(CTWinBase &aParent, const TPoint &aPos, const TSize &aSize)
//
// Call ConstructL, SetExt and either of these fail destroy this and leave
//
	{
	TRAPD(err,ConstructL(aParent, aPos, aSize));
	if (err!=KErrNone)
		{
		delete this;
		User::Leave(err);
		}
	}

//
// End of CTAnimWindow class //
//

CTAnimWindowGroup::CTAnimWindowGroup(CTClient *aClient) : CTWindowGroup(aClient)
	{
	}

void CTAnimWindowGroup::CursorKeyL(TInt xMove,TInt yMove,TInt modifiers)
	{
	if (iCursorMode==0)
		iCurWin->AdjustSizeL(xMove,yMove,modifiers);
	else if (iCurWin->SubType()==69)
		{
		if (iCursorMode==1)
			((CTAnimWindow1 *)iCurWin)->AdjustAnimRect(xMove,yMove,modifiers);
		else
			((CTAnimWindow1 *)iCurWin)->AdjustPolyRectL(xMove,yMove,modifiers);
		}
	}

void CTAnimWindowGroup::KeyL(const TKeyEvent &aKey,const TTime &aTime)
	{
	if (aKey.iModifiers&EModifierCtrl)
		{
		TInt type=0;
		switch(aKey.iCode)
			{
			case 1:
				type=1;
				break;
			case 2:
				type=2;
				break;
			}
		if (type!=0)
			{
			TPoint pos;
			TSize size;
			pos.iX=iCurWin->Size().iWidth>>2;
			pos.iY=iCurWin->Size().iHeight>>2;
			size.iWidth=iCurWin->Size().iWidth>>1;
			size.iHeight=iCurWin->Size().iHeight>>1;
			SetCurrentWindow(((CTAnimClient *)iClient)->CreateTestWindowL(pos,size,iCurWin,type));
			return;
			}
		}
	if (aKey.iModifiers&EModifierFunc)
		{
		switch(aKey.iCode)
			{
			case 'x':
				((CTAnimClient *)iClient)->ExitL();
				break;
			}
		}
	else switch(aKey.iCode)
		{
		case ' ':
			iCursorMode=(iCursorMode+1)%3;
			break;
		case 8:
			CTWin::Delete(iCurWin);
			break;
		case 9:
			if (aKey.iModifiers&EModifierShift)
				SetCurrentWindow(iCurWin->Prev());
			else
				SetCurrentWindow(iCurWin->Next());
			break;
		case 13:
			if (iCurWin->SubType()==69)
				((CTAnimWindow1 *)iCurWin)->ToggleMask();
			break;
		case EKeyLeftArrow:
			CursorKeyL(-Xmove,0,aKey.iModifiers);
			break;
		case EKeyRightArrow:
			CursorKeyL(Xmove,0,aKey.iModifiers);
			break;
		case EKeyUpArrow:
			CursorKeyL(0,-Ymove,aKey.iModifiers);
			break;
		case EKeyDownArrow:
			CursorKeyL(0,Ymove,aKey.iModifiers);
			break;
		default:
			iCurWin->WinKeyL(aKey,aTime);
			break;
		}
	}

//

CTAnimClient::CTAnimClient()
	{
	}

CTBaseWin *CTAnimClient::CreateTestWindowL(TPoint pos,TSize size,CTWinBase *parent, TInt type)
	{
	CTBaseWin *win;
	if (type==1)
		{
		win=new(ELeave) CTAnimWindow1(iAnimDll);
		win->ConstructExtLD(*parent,pos,size);
		win->Activate();
		}
	else
		{
		win=new(ELeave) CTAnimWindow2(iAnimDll);
		win->ConstructExtLD(*parent,pos,size);
		}
	win->AssignGC(*iGc);
	return(win);
	}

CTAnimClient::~CTAnimClient()
	{
	DestroyWindows();
	iAnimDll.Close();
	}

void CTAnimClient::ErrorTests()
	{
	RAnimDll animDll=RAnimDll(iWs);
	if (animDll.Load(NONEXISTENT_DLL_NAME)!=KErrNotFound)
		AnimPanic(EAnimPanicBadLoadFailed);
	animDll.Close();		// Close should be harmless, but unnecassary
	RAnimDll animDll2=RAnimDll(iWs);
	if (animDll2.Load(NONEXISTENT_DLL_NAME)!=KErrNotFound)
		AnimPanic(EAnimPanicBadLoadFailed);
	}

void CTAnimClient::ConstructL()
	{
	CTClient::ConstructL();

	iGroup=new(ELeave) CTAnimWindowGroup(this);
	iGroup->ConstructL();
	
	ErrorTests();

	iAnimDll=RAnimDll(iWs);
	User::LeaveIfError(iAnimDll.Load(ANIM_DLL_NAME));

	CreateTestWindowL(TPoint(30,20),TSize(220,140),iGroup, 1);
	CreateTestWindowL(TPoint(260,60),TSize(60,40),iGroup, 2);
	iGroup->SetCurrentWindow(iGroup->Child());
	}

void CTAnimClient::ExitL()
	{
	CActiveScheduler::Stop();
	User::Leave(0);		// Signals RunL not to do another Request()
	}

GLDEF_C CTClient *CreateClientL()
	{
	return(new(ELeave) CTAnimClient());
	}

GLDEF_C TInt E32Main()
	{
	return(TestLibStartUp(CreateClientL));
	}
