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

#undef SYMBIAN_GRAPHICS_GCE

#include "wsgraphicdrawercontext.h"
#include "stdgraphictestdrawer.h"
#include <bitstd.h>

MWsGraphicDrawerContext* CWsGraphicDrawerNonNgaContext::NewL()
	{
	return new(ELeave) CWsGraphicDrawerNonNgaContext();
	}

void CWsGraphicDrawerNonNgaContext::Destroy()
	{
	delete this;
	}

MWsClient& CWsGraphicDrawerNonNgaContext::Client(MWsGc& aGc)
	{
	return aGc.Client();
	}

MWsScreen& CWsGraphicDrawerNonNgaContext::Screen(MWsGc& aGc)
	{
	return aGc.Screen();
	}

const TTime& CWsGraphicDrawerNonNgaContext::Now(MWsGc& aGc) const
	{
	return aGc.Now();
	}

void CWsGraphicDrawerNonNgaContext::ScheduleAnimation(MWsGc& aGc, const TRect& aRect,const TTimeIntervalMicroSeconds& aFromNow)
	{
	aGc.ScheduleAnimation(aRect, aFromNow);
	}

void CWsGraphicDrawerNonNgaContext::ScheduleAnimation(MWsGc& aGc, const TRect& aRect,const TTimeIntervalMicroSeconds& aFromNow,const TTimeIntervalMicroSeconds& aFreq,const TTimeIntervalMicroSeconds& aStop)
	{
	aGc.ScheduleAnimation(aRect, aFromNow, aFreq, aStop);
	}

void CWsGraphicDrawerNonNgaContext::SetGcOrigin(MWsGc& aGc, const TPoint& aOrigin)
	{
	aGc.SetGcOrigin(aOrigin);
	}

TInt CWsGraphicDrawerNonNgaContext::Push(MWsGc& aGc) const
	{
	return aGc.PushBitGcSettings();
	}

void CWsGraphicDrawerNonNgaContext::Pop(MWsGc& aGc) const
	{
	aGc.PopBitGcSettings();
	}

void CWsGraphicDrawerNonNgaContext::DrawBitmap(MWsGc& aGc, const TPoint& aDestPos, const CFbsBitmap* aSourceBitmap, const TRect& aSourceRect, const CFbsBitmap* aMaskBitmap) const
	{
	CFbsBitGc& bitGc = aGc.BitGc();
	if (aMaskBitmap)
		{
		bitGc.BitBltMasked(aDestPos, aSourceBitmap, aSourceRect, aMaskBitmap, EFalse);
		}
	else
		{
		bitGc.BitBlt(aDestPos, aSourceBitmap, aSourceRect);
		}
	}

void CWsGraphicDrawerNonNgaContext::DrawFrameRate(MWsGc& aGc, const TRect& aRect, const CFrameRate& aFps) const
	{
	CFbsBitGc& bitGc = aGc.BitGc();
	bitGc.SetDrawMode(CGraphicsContext::EDrawModePEN);
	bitGc.SetPenStyle(CGraphicsContext::ENullPen);
	bitGc.SetBrushStyle(CGraphicsContext::ENullBrush);

	if ((aFps.CountSamples() > 1) && bitGc.IsFontUsed())
		{
		TBuf<20> buf;
		buf.AppendNum(aFps.Fps());
		bitGc.SetPenColor(KRgbRed);
		bitGc.DrawText(buf,NULL,TPoint(aRect.iTl.iX,aRect.iBr.iY));
		}
	}
