// Copyright (c) 1999-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Animated Bitmap Window
// 
//

#include "MBMCMD.H"
#include "ANIMWIN.H"


/*TestWindow11*/

TestWindow11::TestWindow11() : CTWin()
	{
	}

TestWindow11::~TestWindow11()
	{
	delete iAnim;
	delete iAnimDll;
	}

void TestWindow11::InitWinL()
	{
	iAnimDll=new(ELeave) RAnimDll(Client()->iWs);
	//__ASSERT_DEBUG(iAnimDll,Panic());
	iAnim=new(ELeave) RMbmAnim(*iAnimDll);
	//__ASSERT_DEBUG(iAnim,Panic());

	CWsBitmap *bitmap=new(ELeave) CWsBitmap(Client()->iWs);
	//__ASSERT_DEBUG(bitmap,Panic());
	iWin.SetRequiredDisplayMode(EColor256);
	User::LeaveIfError(bitmap->Load(VIDEO_MBM_NAME,0));
	iVideoSize=bitmap->SizeInPixels();
	iWin.SetExtent(TPoint(320-(iVideoSize.iWidth>>1),0),iVideoSize);
	delete bitmap;
	TRAP_IGNORE(LoadDllL());
	}

void TestWindow11::Draw()
	{
	TSize size=Size();
	TRect rect(size);

	iGc->SetPenStyle(CGraphicsContext::ENullPen);
	iGc->SetBrushStyle(CGraphicsContext::ESolidBrush);
	iGc->SetBrushColor(TRgb::Gray2(1));
	iGc->DrawRect(rect);
	}

void TestWindow11::WinKeyL(const TKeyEvent &aKey,const TTime &)
	{
	if (aKey.iCode=='p' || aKey.iCode=='P')
		{
		CheckWindowSize();
		iAnim->Activate();
		}
	else if (aKey.iCode=='s' || aKey.iCode=='S')
		iAnim->Deactivate();
	else if (aKey.iCode==',' || aKey.iCode=='<')
		iAnim->SlowDown();
	else if (aKey.iCode=='.' || aKey.iCode=='>')
		iAnim->SpeedUp();
	}

void TestWindow11::LoadDllL()
	{
	_LIT(DllName,"MbmAnim.DLL");
	iAnimDll->Load(DllName);
	iAnim->ConstructL(iWin);
	TBuf<32> name(VIDEO_MBM_NAME);
	iAnim->SetMbmFile(name);
	}

void TestWindow11::CheckWindowSize()
	{
	TSize size=iWin.Size();
	if (size.iWidth<iVideoSize.iWidth || size.iHeight<iVideoSize.iHeight)
		iWin.SetExtent(TPoint(320-(iVideoSize.iWidth>>1),0),iVideoSize);
	}


/*RMbmAnim*/

void RMbmAnim::ConstructL(const RWindowBase &aDevice)
	{
	TPtrC8 des(NULL,0);
	User::LeaveIfError(RAnim::Construct(aDevice,0,des));
	}

void RMbmAnim::SetMbmFile(const TBuf<32>& aFileName)
	{
	TPckgBuf<TBuf<32> > param;
	param()=aFileName;
	Command(EMbmOpSetFileName,param);
	}

void RMbmAnim::Activate()
	{
	Command(EMbmOpActivate);
	}

void RMbmAnim::Deactivate()
	{
	Command(EMbmOpDeactivate);
	}

void RMbmAnim::SpeedUp()
	{
	Command(EMbmOpFaster);
	}

void RMbmAnim::SlowDown()
	{
	Command(EMbmOpSlower);
	}
