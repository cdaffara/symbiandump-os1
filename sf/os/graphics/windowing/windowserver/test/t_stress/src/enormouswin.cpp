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

#include "enormouswin.h"
#include "utils.h"


/**
ENORMOUS WIN
*/

//static configuration data, definitions and default assignments
TBool CEnormousWin::iEnabled = ETrue;
TBool CEnormousWin::iTransparent = ETrue;
TBool CEnormousWin::iRandomizePenStyle = EFalse;
TBool CEnormousWin::iRandomizeBrushStyle = EFalse;


CEnormousWin* CEnormousWin::NewLC(RWsSession& aWs, RWindowGroup* aGroup, CCompWin* aParent, CWindowGc& aGc)
	{
	CEnormousWin* self = new (ELeave) CEnormousWin(aWs, aGroup, aParent, aGc);
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}

void CEnormousWin::LoadConfiguration(const MTestStepConfigurationContext* aContext)
	{
	aContext->GetBool(KT_WservStressParamEnabled, iEnabled);
	aContext->GetBool(KT_WservStressParamTransparent, iTransparent);
	aContext->GetBool(KT_WservStressParamRandomizePenStyle, iRandomizePenStyle);
	aContext->GetBool(KT_WservStressParamRandomizeBrushStyle, iRandomizeBrushStyle);
	}

CEnormousWin::CEnormousWin(RWsSession& aWs, RWindowGroup* aGroup, CCompWin* aParent, CWindowGc& aGc) :
	CCompWin(aWs, aGroup, aParent, aGc)
	{
	}

CEnormousWin::~CEnormousWin()
	{
	}

void CEnormousWin::ConstructL()
	{
	CCompWin::PreConstructL(iTransparent);
	iRedrawWindow->EnableRedrawStore(EFalse);
	TPoint pos = iPos;
	pos.iX *= EScale;
	pos.iY *= EScale;
	SetPos(pos);
	TSize size = iSize;
	size.iWidth *= EScale;
	size.iHeight *= EScale;
	SetSize(size);
	iPenStyle = iRandomizePenStyle ? GetRandomPenStyle() : CGraphicsContext::ESolidPen;
	iBrushStyle = iRandomizeBrushStyle ? GetRandomBrushStyle() : CGraphicsContext::ESolidBrush;
	CCompWin::PostConstructL();
	}

void CEnormousWin::DrawBitmap(CFbsBitGc* aGc, TRect& aClip, TPoint& aOrigin)
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

	TInt left = clip.iTl.iX / ESegmentSize;
	TInt top = clip.iTl.iY / ESegmentSize;
	TInt right = clip.iBr.iX / ESegmentSize + 1;
	TInt bottom = clip.iBr.iY / ESegmentSize + 1;
	TRgb fg;
	TRgb bg;

	for (TInt y = top; y < bottom; ++y)
		{
		TInt g = (y * 31) & 0xFF;
		for (TInt x = left; x < right; ++x)
			{
			TRect rect(x * ESegmentSize, y * ESegmentSize, (x + 1) * ESegmentSize, (y + 1) * ESegmentSize);
			TInt r = (x * 25) & 0xFF;
			TInt b = ((x + y) * 28) & 0xFF;
			TInt col = ((b << 16) + (g << 8) + r) | 0x101010;
			bg = col;
			fg = 0xFFFFFF ^ col;
			bg.SetAlpha(0xFF);
			fg.SetAlpha(0xFF);
			if (iTransparent && (x & y & 1))
				{
				bg.SetAlpha(0x80);
				fg.SetAlpha(0x80);
				}
			aGc->SetPenColor(fg);
			aGc->SetBrushColor(bg);
			aGc->DrawRect(rect);
			}
		}
	
	CCompWin::DrawBitmap(aGc, aClip, aOrigin);
	}	

void CEnormousWin::Redraw(const TRect& aRect)
	{
	iWsGc.Activate(*iWindow);
	iWsGc.Reset();
	
	iWsGc.SetPenStyle(iPenStyle);
	iWsGc.SetBrushStyle(iBrushStyle);

	TInt left = aRect.iTl.iX / ESegmentSize;
	TInt top = aRect.iTl.iY / ESegmentSize;
	TInt right = aRect.iBr.iX / ESegmentSize + 1;
	TInt bottom = aRect.iBr.iY / ESegmentSize + 1;
	TRgb fg;
	TRgb bg;

	for (TInt y = top; y < bottom; ++y)
		{
		TInt g = (y * 31) & 0xFF;
		for (TInt x = left; x < right; ++x)
			{
			TRect rect(x * ESegmentSize, y * ESegmentSize, (x + 1) * ESegmentSize, (y + 1) * ESegmentSize);
			iRedrawWindow->BeginRedraw(rect);
			TInt r = (x * 25) & 0xFF;
			TInt b = ((x + y) * 28) & 0xFF;
			TInt col = ((b << 16) + (g << 8) + r) | 0x101010;
			bg = col;
			fg = 0xFFFFFF ^ col;
			bg.SetAlpha(0xFF);
			fg.SetAlpha(0xFF);
			if (iTransparent && (x & y & 1))
				{
				bg.SetAlpha(0x80);
				fg.SetAlpha(0x80);
				}
			iWsGc.SetPenColor(fg);
			iWsGc.SetBrushColor(bg);
			iWsGc.DrawRect(rect);
			iRedrawWindow->EndRedraw();
			}
		}
	iWsGc.Deactivate();
	}

void CEnormousWin::DumpDetails(RFile& aFile, TInt aDepth)
	{
	TBuf8<256> buf;
	buf.SetLength(0);
	for (TInt d = 0; d < aDepth; ++d)
		{
		buf.Append(_L8("  "));
		}
	buf.Append(_L8("Transparent = ["));
	buf.AppendNum((TInt64)iTransparent);
	buf.Append(_L8("] pen_style = ["));
	buf.AppendNum((TInt64)iPenStyle);
	buf.Append(_L8("] brush_style = ["));
	buf.AppendNum((TInt64)iBrushStyle);
	buf.Append(_L8("]\r\n"));
	aFile.Write(buf);
	}
