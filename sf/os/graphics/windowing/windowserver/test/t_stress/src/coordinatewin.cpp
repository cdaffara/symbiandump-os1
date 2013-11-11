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

#include "coordinatewin.h"
#include "utils.h"

const TInt KRegionSize = 3;


//static configuration data, definitions and default assignments
TBool CCoordinateWin::iEnabled = ETrue;
TBool CCoordinateWin::iTransparent = ETrue;


/**
COORDINATE SPACE WIN
*/
CCoordinateWin* CCoordinateWin::NewLC(RWsSession& aWs, RWindowGroup* aGroup, CCompWin* aParent, CWindowGc& aGc)
	{
	CCoordinateWin* self = new (ELeave) CCoordinateWin(aWs, aGroup, aParent, aGc);
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}

void CCoordinateWin::LoadConfiguration(const MTestStepConfigurationContext* aContext)
	{
	aContext->GetBool(KT_WservStressParamEnabled, iEnabled);
	aContext->GetBool(KT_WservStressParamTransparent, iTransparent);
	}

CCoordinateWin::CCoordinateWin(RWsSession& aWs, RWindowGroup* aGroup, CCompWin* aParent, CWindowGc& aGc) :
	CCompWin(aWs, aGroup, aParent, aGc)
	{
	}

CCoordinateWin::~CCoordinateWin()
	{
	}

void CCoordinateWin::ConstructL()
	{
	CCompWin::PreConstructL(iTransparent);
	
	CCompWin::PostConstructL();
	}

void CCoordinateWin::SetSize(const TSize & aSize)
	{
	CCompWin::SetSize(aSize);

	TPoint mid(iSize.iWidth / 2, iSize.iHeight / 2);
	iSegments[0].iBoundingRect.SetRect(TPoint(0,0), mid);
	iSegments[1].iBoundingRect.SetRect(TPoint(mid.iX, 0), TPoint(iSize.iWidth, mid.iY));
	iSegments[2].iBoundingRect.SetRect(TPoint(0,mid.iY), TPoint(mid.iX, iSize.iHeight));
	iSegments[3].iBoundingRect.SetRect(mid, TPoint(iSize.iWidth, iSize.iHeight));

	const TInt sizeRange = 50;
	for (TInt i = 0; i < 4; ++i)
		{
		TSegment * seg = &iSegments[i];
		do
			{
			seg->iRect.SetRect(seg->iBoundingRect.iTl.iX + TRnd::rnd(sizeRange), seg->iBoundingRect.iTl.iY + TRnd::rnd(sizeRange), seg->iBoundingRect.iTl.iX + TRnd::rnd(sizeRange) + sizeRange, seg->iBoundingRect.iTl.iY + TRnd::rnd(sizeRange) + sizeRange);
			seg->iOrigin.SetXY(TRnd::rnd(sizeRange) - (sizeRange/2), TRnd::rnd(sizeRange) - (sizeRange/2));
			seg->iClippingRegion.Clear();
			for (TInt r = 0; r < KRegionSize; ++r)
				{
				TInt x = seg->iBoundingRect.iTl.iX + TRnd::rnd(sizeRange * 2);
				TInt y = seg->iBoundingRect.iTl.iY + TRnd::rnd(sizeRange * 2);
				TRect rect(x, y, x + TRnd::rnd(sizeRange / 2), y + TRnd::rnd(sizeRange / 2));
				seg->iClippingRegion.AddRect(rect);
				}
			} while (seg->iClippingRegion.CheckError());
		}	

	Redraw(iSize);
	}

void CCoordinateWin::DrawBitmap(CFbsBitGc* aGc, TRect& aClip, TPoint& aOrigin)
	{
	TPoint origin = iPos + aOrigin;
	
	TRect clip(origin, iSize);
	clip.Intersection(aClip);
	clip.Move(-origin);
	
	for (TInt s = 0; s < 4; ++s)
		{
		TSegment * seg = &iSegments[s];

		aGc->Reset();
		aGc->SetOrigin(origin);
		TRect segClip(clip);
		segClip.Intersection(seg->iBoundingRect);
		aGc->SetClippingRect(segClip);
		
		aGc->SetPenStyle(CGraphicsContext::ESolidPen);
		aGc->SetBrushStyle(CGraphicsContext::ESolidBrush);
		aGc->SetPenColor(KRgbWhite);
		aGc->SetBrushColor(TRgb(0,0,0,iTransparent ? 128 : 255));
		aGc->DrawRect(seg->iBoundingRect);
		
		aGc->SetOrigin(origin + seg->iOrigin);
		aGc->SetBrushStyle(CGraphicsContext::ESolidBrush);
		aGc->SetBrushColor(KRgbDarkBlue);
		aGc->SetPenColor(KRgbBlue);
		aGc->DrawEllipse(seg->iRect);

		aGc->SetOrigin(origin);
		aGc->SetBrushStyle(CGraphicsContext::ENullBrush);
		aGc->SetPenColor(KRgbGreen);
		DrawRegion(seg->iClippingRegion, *aGc);
		aGc->SetOrigin(origin + seg->iOrigin);
		
		TRegionFix<KRegionSafe> region;
		region.Copy(seg->iClippingRegion);
		//In CFbsBitGc::SetClippingRegion, Offset is not done on the clipping region,
		//This is different with CWsGc::SetClippingRegion. Need to call the Offset here.
		region.Offset(origin + seg->iOrigin);
		region.ClipRect(TRect(TPoint(0,0), aGc->Device()->SizeInPixels()));
		
		if (region.CheckError())
			{
			__DEBUGGER();
			}
		
		aGc->SetBrushStyle(CGraphicsContext::ESolidBrush);
		aGc->SetBrushColor(KRgbDarkRed);
		aGc->SetPenColor(KRgbRed);
		
		if (!region.IsEmpty())
			{
			aGc->SetClippingRegion(&region);
			aGc->DrawEllipse(seg->iRect);
			aGc->CancelClippingRegion();
			}

		TRect rect(seg->iRect);
		rect.Shrink(8,8);
		aGc->SetBrushStyle(CGraphicsContext::ENullBrush);
		aGc->DrawEllipse(rect);
		}
	
	CCompWin::DrawBitmap(aGc, aClip, aOrigin);
	}	

void CCoordinateWin::Redraw(const TRect&)
	{
	iWsGc.Activate(*iWindow);
	for (TInt s = 0; s < 4; ++s)
		{
		TSegment * seg = &iSegments[s];

		iWsGc.Reset();

		iRedrawWindow->BeginRedraw(seg->iBoundingRect);

		iWsGc.SetPenStyle(CGraphicsContext::ESolidPen);
		iWsGc.SetBrushStyle(CGraphicsContext::ESolidBrush);
		iWsGc.SetPenColor(KRgbWhite);
		iWsGc.SetBrushColor(TRgb(0,0,0,iTransparent ? 128 : 255)); //if transparency not set this will be opaque
		iWsGc.DrawRect(seg->iBoundingRect);

		iWsGc.SetOrigin(seg->iOrigin);

		iWsGc.SetBrushStyle(CGraphicsContext::ESolidBrush);
		iWsGc.SetBrushColor(KRgbDarkBlue);
		iWsGc.SetPenColor(KRgbBlue);
		iWsGc.DrawEllipse(seg->iRect);

		iWsGc.SetOrigin(TPoint(0,0));
		iWsGc.SetBrushStyle(CGraphicsContext::ENullBrush);
		iWsGc.SetPenColor(KRgbGreen);
		DrawRegion(seg->iClippingRegion, iWsGc);
		iWsGc.SetOrigin(seg->iOrigin);

		//In CWsGc::SetClippingRegion, clipping region is offset based on the CWsGc::iOrigin.
		iWsGc.SetClippingRegion(seg->iClippingRegion);
		
		iWsGc.SetBrushStyle(CGraphicsContext::ESolidBrush);
		iWsGc.SetBrushColor(KRgbDarkRed);
		iWsGc.SetPenColor(KRgbRed);
		iWsGc.DrawEllipse(seg->iRect);
		
		iWsGc.CancelClippingRegion();
		
		TRect rect(seg->iRect);
		rect.Shrink(8,8);
		iWsGc.SetBrushStyle(CGraphicsContext::ENullBrush);
		iWsGc.DrawEllipse(rect);
		iRedrawWindow->EndRedraw();
		}
	iWsGc.Deactivate();
	}

void CCoordinateWin::DumpDetails(RFile& aFile, TInt aDepth)
	{
	TBuf8<256> buf;
	buf.SetLength(0);
	for (TInt d = 0; d < aDepth; ++d)
		{
		buf.Append(_L8("  "));
		}
	buf.Append(_L8("Transparent = ["));
	buf.AppendNum((TInt64)iTransparent);
	buf.Append(_L8("]\r\n"));
	aFile.Write(buf);
	}
