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

#include "edgedwin.h"
#include "utils.h"

	
/**
EDGED WIN
*/

//static configuration data, definitions and default assignments
TBool CEdgedWin::iEnabled = ETrue;
TBool CEdgedWin::iTransparent = ETrue;
TBool CEdgedWin::iRandomizeAlpha = EFalse;
TBool CEdgedWin::iRandomizePenStyle = EFalse;
TBool CEdgedWin::iRandomizeBrushStyle = EFalse;


CEdgedWin* CEdgedWin::NewLC(RWsSession& aWs, RWindowGroup* aGroup, CCompWin* aParent, CWindowGc& aGc)
	{
	CEdgedWin* self = new (ELeave) CEdgedWin(aWs, aGroup, aParent, aGc);
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}

void CEdgedWin::LoadConfiguration(const MTestStepConfigurationContext* aContext)
	{
	aContext->GetBool(KT_WservStressParamEnabled, iEnabled);
	aContext->GetBool(KT_WservStressParamTransparent, iTransparent);
	aContext->GetBool(KT_WservStressParamRandomizeAlpha, iRandomizeAlpha);
	aContext->GetBool(KT_WservStressParamRandomizePenStyle, iRandomizePenStyle);
	aContext->GetBool(KT_WservStressParamRandomizeBrushStyle, iRandomizeBrushStyle);
	}

CEdgedWin::CEdgedWin(RWsSession& aWs, RWindowGroup* aGroup, CCompWin* aParent, CWindowGc& aGc) :
	CCompWin(aWs, aGroup, aParent, aGc)
	{
	}

CEdgedWin::~CEdgedWin()
	{
	}

void CEdgedWin::ConstructL()
	{
	CCompWin::PreConstructL(iTransparent);
	iTransBgColor = TRnd::rnd();
	iTransFgColor = TRnd::rnd();
	iBgColor = TRnd::rnd();
	iFgColor = TRnd::rnd();
	if (!iTransparent || !iRandomizeAlpha) //randomizing the alpha will cause bitmap differences
		{
		iTransBgColor.SetAlpha(0xFF);
		iTransFgColor.SetAlpha(0xFF);
		}
	iBgColor.SetAlpha(0xFF); //inner rectangle should not be transparent
	iFgColor.SetAlpha(0xFF); //inner rectangle should not be transparent
		
	iPenStyle = iRandomizePenStyle ? GetRandomPenStyle() : CGraphicsContext::ESolidPen;
	iBrushStyle = iRandomizeBrushStyle ? GetRandomBrushStyle() : CGraphicsContext::ESolidBrush;
	CCompWin::PostConstructL();
	}

void CEdgedWin::SetSize(const TSize & aSize)
	{
	CCompWin::SetSize(aSize);

	iOpaqueRect.iTl.iX = 10;
	iOpaqueRect.iTl.iY = 10;
	iOpaqueRect.iBr.iX = iSize.iWidth - 10;
	iOpaqueRect.iBr.iY = iSize.iHeight - 10;
	if (iTransparent)
		{
		iTransparentRegion.Clear();
		iTransparentRegion.AddRect(TRect(TPoint(0,0), iSize));
		iTransparentRegion.SubRect(iOpaqueRect);
		iRedrawWindow->SetTransparentRegion(iTransparentRegion); //TRANSPARENCY must be defined in wsini.ini
		}
	}

void CEdgedWin::DrawBitmap(CFbsBitGc* aGc, TRect& aClip, TPoint& aOrigin)
	{
	aGc->Reset();
	TPoint origin = iPos + aOrigin;
	aGc->SetOrigin(origin);
	
	TRect clip(origin, iSize);
	clip.Intersection(aClip);
	clip.Move(-origin);	
	
	aGc->SetClippingRect(clip);
	aGc->SetPenStyle(iPenStyle);
	aGc->SetBrushStyle(iBrushStyle);
	//draw outer rectangle
	aGc->SetPenColor(iTransFgColor);
	aGc->SetBrushColor(iTransBgColor);
	aGc->DrawRect(TRect(TPoint(0,0), iSize));
		
	//draw inner rectangle
	aGc->SetPenColor(iFgColor);
	aGc->SetBrushColor(iBgColor);
	aGc->DrawRect(iOpaqueRect);
	
	CCompWin::DrawBitmap(aGc, aClip, aOrigin);
	}	

void CEdgedWin::Redraw(const TRect& aRect)
	{
	iWsGc.Activate(*iWindow);
	iWsGc.Reset();
	iWsGc.SetPenStyle(iPenStyle);
	iWsGc.SetBrushStyle(iBrushStyle);
	TBool redraw = EFalse;
	//redraw outer rectangle if in rect
	if (aRect.iTl.iX < iOpaqueRect.iTl.iX ||
	    aRect.iTl.iY < iOpaqueRect.iTl.iY ||
	    aRect.iBr.iX > iOpaqueRect.iBr.iX ||
	    aRect.iBr.iY > iOpaqueRect.iBr.iY)
	    {
	    redraw = ETrue;
	    iRedrawWindow->BeginRedraw();
		iWsGc.SetPenColor(iTransFgColor);
		iWsGc.SetBrushColor(iTransBgColor);
		iWsGc.DrawRect(TRect(TPoint(0,0), iSize));
	//	iRedrawWindow->EndRedraw() will be taken care of below
	    }
	//redraw inner rectangle
	if (redraw || aRect.Intersects(iOpaqueRect))
		{
		if (!redraw)
			{
			iRedrawWindow->BeginRedraw(iOpaqueRect);//iOpaqueRect);
			}
		iWsGc.SetPenColor(iFgColor);
		iWsGc.SetBrushColor(iBgColor);
		iWsGc.DrawRect(iOpaqueRect);
		iRedrawWindow->EndRedraw();
		}
	iWsGc.Deactivate();
	}

void CEdgedWin::DumpDetails(RFile& aFile, TInt aDepth)
	{
	TBuf8<256> buf;
	buf.SetLength(0);
	for (TInt d = 0; d < aDepth; ++d)
		{
		buf.Append(_L8("  "));
		}
	buf.Append(_L8("Transparent = ["));
	buf.AppendNum((TInt64)iTransparent);
	buf.Append(_L8("] randomize_alpha = ["));
	buf.AppendNum((TInt64)iRandomizeAlpha);
	buf.Append(_L8("] pen_style = ["));
	buf.AppendNum((TInt64)iPenStyle);
	buf.Append(_L8("] brush_style = ["));
	buf.AppendNum((TInt64)iBrushStyle);
	buf.Append(_L8("] transFgColor = ["));
	buf.AppendNum(iTransFgColor.Value());
	buf.Append(_L8("] transBgColor = ["));
	buf.AppendNum(iTransBgColor.Value());
	buf.Append(_L8("] FgColor = ["));
	buf.AppendNum(iFgColor.Value());
	buf.Append(_L8("] BgColor = ["));
	buf.AppendNum(iBgColor.Value());
	buf.Append(_L8("]\r\n"));
	aFile.Write(buf);
	}
