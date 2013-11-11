// Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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
//

/**
 @file
 @test
 @internalComponent
*/

#include "backedupwin.h"
#include "utils.h"


//static configuration data, definitions and default assignments
TBool CBackedupWin::iEnabled = ETrue;
TBool CBackedupWin::iRandomizePenStyle = EFalse;
TBool CBackedupWin::iRandomizeBrushStyle = EFalse;


/**
BACKED UP WIN
*/
CBackedupWin * CBackedupWin::NewLC(RWsSession& aWs, RWindowGroup* aGroup, CCompWin* aParent, CWindowGc& aGc)
	{
	CBackedupWin* self = new (ELeave) CBackedupWin(aWs, aGroup, aParent, aGc);
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}

void CBackedupWin::LoadConfiguration(const MTestStepConfigurationContext* aContext)
	{
	aContext->GetBool(KT_WservStressParamEnabled, iEnabled);
	aContext->GetBool(KT_WservStressParamRandomizePenStyle, iRandomizePenStyle);
	aContext->GetBool(KT_WservStressParamRandomizeBrushStyle, iRandomizeBrushStyle);
	}

CBackedupWin::CBackedupWin(RWsSession& aWs, RWindowGroup* aGroup, CCompWin* aParent, CWindowGc& aGc) :
	CCompWin(aWs, aGroup, aParent, aGc)
	{
	}

CBackedupWin::~CBackedupWin()
	{
	if (iBkWsGc)
		{
		iBkWsGc->Deactivate();
		delete iBkWsGc;
		}
	}

void CBackedupWin::ConstructL()
	{
	CCompWin::PreConstructL(EFalse, ETrue);
	iBgColor = TRnd::rnd();
	iFgColor = TRnd::rnd();
	//backedup window does not support transparency
	iBgColor.SetAlpha(0xFF);
	iFgColor.SetAlpha(0xFF);
	
	iPenStyle = iRandomizePenStyle ? GetRandomPenStyle() : CGraphicsContext::ESolidPen;
	iBrushStyle = iRandomizeBrushStyle ? GetRandomBrushStyle() : CGraphicsContext::ESolidBrush;
	CCompWin::PostConstructL();

	CWsScreenDevice* dev = static_cast<CWsScreenDevice*>(iWsGc.Device());
	iBkWsGc = new (ELeave) CWindowGc(dev);
	User::LeaveIfError(iBkWsGc->Construct());
	iBkWsGc->Activate(*iWindow);
	Redraw(TRect(iSize));
	}

void CBackedupWin::SetSize(const TSize & aSize)
	{
	CCompWin::SetSize(aSize);
	}

void CBackedupWin::DrawBitmap(CFbsBitGc* aGc, TRect& aClip, TPoint& aOrigin)
	{
	aGc->Reset();
	TPoint origin = iPos + aOrigin;
	aGc->SetOrigin(origin);
	
	TRect clip(origin, iSize);
	clip.Intersection(aClip);
	clip.Move(-origin);
	aGc->SetClippingRect(clip);
	aGc->SetBrushColor(KRgbWhite);
	aGc->SetDrawMode(CGraphicsContext::EDrawModeWriteAlpha);
	aGc->Clear(TRect(iSize));
	aGc->SetDrawMode(CGraphicsContext::EDrawModePEN);
	
	aGc->SetPenStyle(iPenStyle);
	aGc->SetBrushStyle(iBrushStyle);
	aGc->SetPenColor(iFgColor);
	aGc->SetBrushColor(iBgColor);
	aGc->DrawRect(TRect(iSize));
	aGc->SetPenStyle(CGraphicsContext::ENullPen);
	aGc->SetBrushColor(iFgColor);
	aGc->DrawRect(TRect(iSize.iWidth / 2 - 10, 0, iSize.iWidth / 2 + 10, iSize.iHeight));
	aGc->DrawRect(TRect(0, iSize.iHeight / 2 - 10, iSize.iWidth, iSize.iHeight / 2 + 10));
	
	CCompWin::DrawBitmap(aGc, aClip, aOrigin);
	}	

void CBackedupWin::Redraw(const TRect&)
	{
	if (iBkWsGc)
		{
		iBkWsGc->Reset();
		iBkWsGc->SetPenStyle(iPenStyle);
		iBkWsGc->SetBrushStyle(iBrushStyle);
		iBkWsGc->SetPenColor(iFgColor);
		iBkWsGc->SetBrushColor(iBgColor);
		iBkWsGc->DrawRect(TRect(iSize));
		iBkWsGc->SetPenStyle(CGraphicsContext::ENullPen);
		iBkWsGc->SetBrushColor(iFgColor);
		iBkWsGc->DrawRect(TRect(iSize.iWidth / 2 - 10, 0, iSize.iWidth / 2 + 10, iSize.iHeight));
		iBkWsGc->DrawRect(TRect(0, iSize.iHeight / 2 - 10, iSize.iWidth, iSize.iHeight / 2 + 10));
		}
	}

void CBackedupWin::DumpDetails(RFile& aFile, TInt aDepth)
	{
	TBuf8<256> buf;
	buf.SetLength(0);
	for (TInt d = 0; d < aDepth; ++d)
		{
		buf.Append(_L8("  "));
		}
	buf.Append(_L8("pen_style = ["));
	buf.AppendNum((TInt64)iPenStyle);
	buf.Append(_L8("] brush_style = ["));
	buf.AppendNum((TInt64)iBrushStyle);
	buf.Append(_L8("] FgColor = ["));
	buf.AppendNum(iFgColor.Value());
	buf.Append(_L8("] BgColor = ["));
	buf.AppendNum(iBgColor.Value());
	buf.Append(_L8("]\r\n"));
	aFile.Write(buf);
	}
