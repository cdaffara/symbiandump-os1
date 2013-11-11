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

#define SYMBIAN_GRAPHICS_GCE

#include "wsgraphicdrawercontext.h"
#include "stdgraphictestdrawer.h"
#include <graphics/wsgraphicscontext.h>

MWsGraphicDrawerContext* CWsGraphicDrawerNgaContext::NewL()
	{
	return new(ELeave) CWsGraphicDrawerNgaContext();
	}

void CWsGraphicDrawerNgaContext::Destroy()
	{
	delete this;
	}

MWsClient& CWsGraphicDrawerNgaContext::Client(MWsGc& aGc)
	{
	return aGc.Client();
	}

MWsScreen& CWsGraphicDrawerNgaContext::Screen(MWsGc& aGc)
	{
	return aGc.Screen();
	}

const TTime& CWsGraphicDrawerNgaContext::Now(MWsGc& aGc) const
	{
	return aGc.Now();
	}

void CWsGraphicDrawerNgaContext::ScheduleAnimation(MWsGc& aGc, const TRect& aRect,const TTimeIntervalMicroSeconds& aFromNow)
	{
	aGc.ScheduleAnimation(aRect, aFromNow);
	}

void CWsGraphicDrawerNgaContext::ScheduleAnimation(MWsGc& aGc, const TRect& aRect,const TTimeIntervalMicroSeconds& aFromNow,const TTimeIntervalMicroSeconds& aFreq,const TTimeIntervalMicroSeconds& aStop)
	{
	aGc.ScheduleAnimation(aRect, aFromNow, aFreq, aStop);
	}

void CWsGraphicDrawerNgaContext::SetGcOrigin(MWsGc& aGc, const TPoint& aOrigin)
	{
	aGc.SetGcOrigin(aOrigin);
	}

TInt CWsGraphicDrawerNgaContext::Push(MWsGc& aGc) const
	{
	MWsGraphicsContext* context = aGc.ObjectInterface<MWsGraphicsContext>();
	if (context)
		{
		return context->Push();
		}
	return KErrNotSupported;
	}

void CWsGraphicDrawerNgaContext::Pop(MWsGc& aGc) const
	{
	MWsGraphicsContext* context = aGc.ObjectInterface<MWsGraphicsContext>();
	if (context)
		{
		context->Pop();
		}
	}

void CWsGraphicDrawerNgaContext::DrawBitmap(MWsGc& aGc, const TPoint& aDestPos, const CFbsBitmap* aSourceBitmap, const TRect& aSourceRect, const CFbsBitmap* aMaskBitmap) const
	{
	MWsGraphicsContext* context = aGc.ObjectInterface<MWsGraphicsContext>();
	if (context)
		{
		if (aMaskBitmap)
			{
			context->BitBltMasked(aDestPos, *aSourceBitmap, aSourceRect, *aMaskBitmap, EFalse);
			}
		else
			{
			context->BitBlt(aDestPos, *aSourceBitmap, aSourceRect);
			}
		}
	}

void CWsGraphicDrawerNgaContext::DrawFrameRate(MWsGc& aGc, const TRect& aRect, const CFrameRate& aFps) const
	{
	MWsGraphicsContext* context = aGc.ObjectInterface<MWsGraphicsContext>();
	if (context)
		{
		context->SetDrawMode(MWsGraphicsContext::EDrawModePEN);
		context->SetPenStyle(MWsGraphicsContext::ENullPen);
		context->SetBrushStyle(MWsGraphicsContext::ENullBrush);

		if ((aFps.CountSamples() > 1) && context->HasFont())
			{
			TBuf<20> buf;
			buf.AppendNum(aFps.Fps());
			context->SetPenColor(KRgbRed);
			context->DrawText(buf,NULL,TPoint(aRect.iTl.iX,aRect.iBr.iY));
			}
		}
	}
