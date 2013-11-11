// Copyright (c) 1996-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Interface code for animated DLL's GC
// 
//

#include <e32std.h>
#include "server.h"
#include "playbackgc.h"
#include "ANIM.H"
#include "wsfont.h"
#include "bitgditomwsgraphicscontextmappings.h"
#include "devicemap.h"

CWsAnimGc::CWsAnimGc(CWsAnim& aWsAnim) : iOwningWsAnim(aWsAnim), iUserActive(EFalse), iUseDrawingRegion(EFalse)
	{
	__DECLARE_NAME(_S("CWsAnimGc"));
	}

CWsAnimGc::~CWsAnimGc()
	{
	iDrawingRegion.Close();
	}

void CWsAnimGc::Activate(const TRegion* aRegion, MWsGraphicsContext* aGc)
	{
	iBaseRegion = aRegion ? aRegion : iOwningWsAnim.ClientWindow()->DrawingRegion();
	iGc = aGc;
	iUserActive = ETrue;
	Reset();
	}

void CWsAnimGc::Deactivate()
	{
	CloseCurrentFont();
	iGc = NULL;
	iUserActive = EFalse;
	iUseDrawingRegion = EFalse;
	iDrawingRegion.Close();
	}

TBool CWsAnimGc::IsActive() const
	{
	return (iUserActive || (iGc != NULL));
	}

void CWsAnimGc::UserActivate()
	{
	if (iUserActive)
		{
		// Already active
		UserDeactivate();
		iOwningWsAnim.SessionPanic();
		}
	iUserActive = ETrue;
	}

void CWsAnimGc::UserDeactivate()
	{
	CloseCurrentFont();

	if (iUserActive)
		{
		iUserActive = EFalse;
		}
	}

CGraphicsDevice* CWsAnimGc::Device() const
	{
	if (!iUserActive)
		iOwningWsAnim.Panic(EWservPanicAnimLeave);

	return NULL; //deprecated
	}

void CWsAnimGc::SetClippingRect(const TRect& aRect)
	{
	if (!iUserActive)
		iOwningWsAnim.Panic(EWservPanicAnimLeave);

	if (iGc)
		{
		iClippingRect = aRect;
		iClippingRect.Move(iOwningWsAnim.ClientWindow()->Origin());
		iClippingRect.Intersection(iOwningWsAnim.ClientWindow()->AbsRect());

		STACK_REGION region;
		region.Copy(*iBaseRegion);
		if (iUseDrawingRegion)
			region.Intersect(iDrawingRegion);
		region.ClipRect(iClippingRect);
		region.Tidy();

		iGc->SetClippingRegion(region);
		region.Close();

		if (iGc->GetError())
			iClippingRect = iOwningWsAnim.ClientWindow()->AbsRect(); // On error revert to the default clipping rect
		}
	}

TInt CWsAnimGc::SetClippingRegion(const TRegion &aRegion)
	{
	if (!iUserActive)
		return KErrGeneral;

	TInt error = KErrNone;
	if (iGc)
		{
		STACK_REGION region;
		region.Copy(aRegion);
		region.Offset(iOwningWsAnim.ClientWindow()->Origin());
		region.Intersect(*iBaseRegion);
		region.ClipRect(iClippingRect);
		region.Tidy();

		iGc->SetClippingRegion(region);
		region.Close();

		error = iGc->GetError();
		if (!error)
			{ // Need to keep a copy of the region in screen coordinates
			iDrawingRegion.Copy(aRegion);
			iDrawingRegion.Offset(iOwningWsAnim.ClientWindow()->Origin());
			iUseDrawingRegion = ETrue;
			}
		}
	return error;
	}

void CWsAnimGc::CancelClippingRegion()
	{
	if (!iUserActive)
		iOwningWsAnim.Panic(EWservPanicAnimLeave);

	if (iGc)
		{
		iDrawingRegion.Close();
		iUseDrawingRegion = EFalse;

		STACK_REGION region;
		region.Copy(*iBaseRegion);
		region.ClipRect(iClippingRect);
		region.Tidy();
		iGc->SetClippingRegion(region);
		region.Close();
		}
	}

void CWsAnimGc::CancelClippingRect()
	{
	if (!iUserActive)
		iOwningWsAnim.Panic(EWservPanicAnimLeave);

	if (iGc)
		{
		iClippingRect = iOwningWsAnim.ClientWindow()->AbsRect(); // Revert to the default clipping rect

		STACK_REGION region;
		region.Copy(*iBaseRegion);
		if (iUseDrawingRegion)
			region.Intersect(iDrawingRegion);
		region.ClipRect(iClippingRect);
		region.Tidy();
		iGc->SetClippingRegion(region);
		region.Close();
		}
	}

void CWsAnimGc::SetDrawMode(TDrawMode aDrawingMode)
	{
	if (!iUserActive)
		iOwningWsAnim.Panic(EWservPanicAnimLeave);

	if (iGc)
		iGc->SetDrawMode(BitGdiToMWsGraphicsContextMappings::LossyConvert(aDrawingMode));
	}

void CWsAnimGc::UseFont(const CFont *aFont)
	{
	CloseCurrentFont();
	iFont=(CAnimFbsFont *)aFont;
	iFont->Open();
	if (!iUserActive)
		iOwningWsAnim.Panic(EWservPanicAnimLeave);
	if (iGc)
		iGc->SetFontNoDuplicate(iFont);
	}

void CWsAnimGc::DiscardFont()
	{
	CloseCurrentFont();
	if (!iUserActive)
		iOwningWsAnim.Panic(EWservPanicAnimLeave);
	if (iGc)
		iGc->ResetFont();
	}

void CWsAnimGc::SetUnderlineStyle(TFontUnderline aUnderlineStyle)
	{
	if (!iUserActive)
		iOwningWsAnim.Panic(EWservPanicAnimLeave);
	if (iGc)
		iGc->SetUnderlineStyle(BitGdiToMWsGraphicsContextMappings::Convert(aUnderlineStyle));
	}

void CWsAnimGc::SetStrikethroughStyle(TFontStrikethrough aStrikethroughStyle)
	{
	if (!iUserActive)
		iOwningWsAnim.Panic(EWservPanicAnimLeave);
	if (iGc)
		iGc->SetStrikethroughStyle(BitGdiToMWsGraphicsContextMappings::Convert(aStrikethroughStyle));
	}

void CWsAnimGc::SetWordJustification(TInt aExcessWidth,TInt aNumGaps)
	{
	if (!iUserActive)
		iOwningWsAnim.Panic(EWservPanicAnimLeave);
	if (iGc)
		iGc->SetWordJustification(aExcessWidth,aNumGaps);
	}

void CWsAnimGc::SetCharJustification(TInt aExcessWidth,TInt aNumChars)
	{
	if (!iUserActive)
		iOwningWsAnim.Panic(EWservPanicAnimLeave);
	if (iGc)
		iGc->SetCharJustification(aExcessWidth,aNumChars);
	}

void CWsAnimGc::SetPenColor(const TRgb &aColor)
	{
	if (!iUserActive)
		iOwningWsAnim.Panic(EWservPanicAnimLeave);
	if (iGc)
		iGc->SetPenColor(aColor);
	}

void CWsAnimGc::SetPenStyle(TPenStyle aPenStyle)
	{
	if (!iUserActive)
		iOwningWsAnim.Panic(EWservPanicAnimLeave);
	if (iGc)
		iGc->SetPenStyle(BitGdiToMWsGraphicsContextMappings::Convert(aPenStyle));
	}

void CWsAnimGc::SetPenSize(const TSize& aSize)
	{
	if (!iUserActive)
		iOwningWsAnim.Panic(EWservPanicAnimLeave);
	if (iGc)
		iGc->SetPenSize(aSize);
	}

void CWsAnimGc::SetBrushColor(const TRgb &aColor)
	{
	if (!iUserActive)
		iOwningWsAnim.Panic(EWservPanicAnimLeave);
	if (iGc)
		iGc->SetBrushColor(aColor);
	}

void CWsAnimGc::SetBrushStyle(TBrushStyle aBrushStyle)
	{
	if (!iUserActive)
		iOwningWsAnim.Panic(EWservPanicAnimLeave);
	if (iGc)
		iGc->SetBrushStyle(BitGdiToMWsGraphicsContextMappings::Convert(aBrushStyle));
	}

void CWsAnimGc::SetBrushOrigin(const TPoint &aOrigin)
	{
	if (!iUserActive)
		iOwningWsAnim.Panic(EWservPanicAnimLeave);
	if (iGc)
		iGc->SetBrushOrigin(aOrigin);
	}

void CWsAnimGc::UseBrushPattern(const CFbsBitmap *aBitmap)
	{
	if (!iUserActive)
		iOwningWsAnim.Panic(EWservPanicAnimLeave);
	if (iGc)
		iGc->SetBrushPattern(*aBitmap);
	}

void CWsAnimGc::DiscardBrushPattern()
	{
	if (!iUserActive)
		iOwningWsAnim.Panic(EWservPanicAnimLeave);
	if (iGc)
		iGc->ResetBrushPattern();
	}

void CWsAnimGc::SetFaded(TBool /*aFaded*/) // Deprecated
	{
	if (!iUserActive)
		iOwningWsAnim.Panic(EWservPanicAnimLeave);
	// Do nothing
	}

void CWsAnimGc::SetFadingParameters(TUint8 /*aBlackMap*/,TUint8 /*aWhiteMap*/) // Deprecated
	{
	if (!iUserActive)
		iOwningWsAnim.Panic(EWservPanicAnimLeave);
	// Do nothing 
	}

//
// Drawing commands 								//
//

void CWsAnimGc::DrawArc(const TRect &aRect,const TPoint &aStart,const TPoint &aEnd)
	{
	if (!iUserActive)
		iOwningWsAnim.Panic(EWservPanicAnimLeave);
	if (iGc)
		iGc->DrawArc(aRect,aStart,aEnd);
	}

void CWsAnimGc::DrawPie(const TRect &aRect,const TPoint &aStart,const TPoint &aEnd)
	{
	if (!iUserActive)
		iOwningWsAnim.Panic(EWservPanicAnimLeave);
	if (iGc)
		iGc->DrawPie(aRect,aStart,aEnd);
	}

void CWsAnimGc::DrawLine(const TPoint &aPoint1,const TPoint &aPoint2)
	{
	if (!iUserActive)
		iOwningWsAnim.Panic(EWservPanicAnimLeave);
	if (iGc)
		iGc->DrawLine(aPoint1, aPoint2);
	}

void CWsAnimGc::DrawLineTo(const TPoint &aPoint)
	{
	if (!iUserActive)
		iOwningWsAnim.Panic(EWservPanicAnimLeave);
	if (iGc)
		iGc->DrawLineTo(aPoint);
	}

void CWsAnimGc::DrawLineBy(const TPoint &aPoint)
	{
	if (!iUserActive)
		iOwningWsAnim.Panic(EWservPanicAnimLeave);
	if (iGc)
		iGc->DrawLineBy(aPoint);
	}

void CWsAnimGc::DrawEllipse(const TRect &aRect)
	{
	if (!iUserActive)
		iOwningWsAnim.Panic(EWservPanicAnimLeave);
	if (iGc)
		iGc->DrawEllipse(aRect);
	}

void CWsAnimGc::DrawRect(const TRect &aRect)
	{
	if (!iUserActive)
		iOwningWsAnim.Panic(EWservPanicAnimLeave);
	if (iGc)
		iGc->DrawRect(aRect);
	}

void CWsAnimGc::DrawRoundRect(const TRect &aRect,const TSize &aCornerSize)
	{
	if (!iUserActive)
		iOwningWsAnim.Panic(EWservPanicAnimLeave);
	if (iGc)
		iGc->DrawRoundRect(aRect, aCornerSize);
	}

void CWsAnimGc::Clear(const TRect &aRect)
	{
	if (!iUserActive)
		iOwningWsAnim.Panic(EWservPanicAnimLeave);
	if (iGc)
		iGc->Clear(aRect);
	}

void CWsAnimGc::Clear()
	{
	if (!iUserActive)
		iOwningWsAnim.Panic(EWservPanicAnimLeave);
	if (iGc)
		iGc->Clear();
	}

void CWsAnimGc::BitBlt(const TPoint &aPoint,const CFbsBitmap *aBitmap)
	{
	if (!iUserActive)
		iOwningWsAnim.Panic(EWservPanicAnimLeave);
	if (iGc)
		iGc->BitBlt(aPoint, *aBitmap);
	}

void CWsAnimGc::BitBlt(const TPoint &aDestination,const CFbsBitmap *aBitmap,const TRect &aSource)
	{
	if (!iUserActive)
		iOwningWsAnim.Panic(EWservPanicAnimLeave);
	if (iGc)
		iGc->BitBlt(aDestination, *aBitmap, aSource);
	}

void CWsAnimGc::BitBltMasked(const TPoint& aPoint,const CFbsBitmap* aBitmap,const TRect& aSourceRect,const CFbsBitmap* aMaskBitmap,TBool aInvertMask)
	{
	if (!iUserActive)
		iOwningWsAnim.Panic(EWservPanicAnimLeave);
	if (iGc)
		iGc->BitBltMasked(aPoint, *aBitmap, aSourceRect, *aMaskBitmap, aInvertMask);
	}

void CWsAnimGc::DrawBitmap(const TPoint &aTopLeft, const CFbsBitmap *aSource)
	{
	if (!iUserActive)
		iOwningWsAnim.Panic(EWservPanicAnimLeave);
	if (iGc)
		{
		// DrawBitmap(TPoint&, ) uses the size of the bitmap in twips, but 
		// MWsGraphicsContext::DrawBitmap() takes a TRect in pixels, so we need to convert
		TRect destRect(iOwningWsAnim.ClientWindow()->Screen()->DeviceMap().TwipsToPixels(aSource->SizeInTwips()));
		destRect.Move(aTopLeft); //aTopLeft is defined in pixels, that's why we're not converting it
		iGc->DrawBitmap(destRect, *aSource);
		}
	}

void CWsAnimGc::DrawBitmap(const TRect &aDestRect, const CFbsBitmap *aSource)
	{
	if (!iUserActive)
		iOwningWsAnim.Panic(EWservPanicAnimLeave);
	if (iGc)
		iGc->DrawBitmap(aDestRect, *aSource);
	}

void CWsAnimGc::DrawBitmap(const TRect &aDestRect, const CFbsBitmap *aSource, const TRect &aSrcRect)
	{
	if (!iUserActive)
		iOwningWsAnim.Panic(EWservPanicAnimLeave);
	if (iGc)
		iGc->DrawBitmap(aDestRect, *aSource, aSrcRect);
	}

void CWsAnimGc::DrawBitmapMasked(const TRect &aDestRect, const CFbsBitmap *aBitmap, const TRect &aSrcRect, const CFbsBitmap* aMaskBitmap, TBool aInvertMask)
	{
	if (!iUserActive)
		iOwningWsAnim.Panic(EWservPanicAnimLeave);
	if (iGc)
		iGc->DrawBitmapMasked(aDestRect, *aBitmap, aSrcRect, *aMaskBitmap, aInvertMask);
	}

void CWsAnimGc::DrawBitmapMasked(const TRect& aDestRect,const CWsBitmap* aBitmap,const TRect& aSrcRect,const CWsBitmap* aMaskBitmap, TBool aInvertMask)
	{
	if (!iUserActive)
		iOwningWsAnim.Panic(EWservPanicAnimLeave);
	if (iGc)
		iGc->DrawBitmapMasked(aDestRect, *aBitmap, aSrcRect, *aMaskBitmap, aInvertMask);
	}

void CWsAnimGc::DrawPolyLine(const TPoint* aPointList,TInt aNumPoints)
	{
	if (!iUserActive)
		iOwningWsAnim.Panic(EWservPanicAnimLeave);
	if (iGc)
		{
		TArrayWrapper<TPoint> points(aPointList, aNumPoints);
		iGc->DrawPolyLine(points);
		}
	}

void CWsAnimGc::DrawPolyLine(const CArrayFix<TPoint> *aPointList)
	{
	if (!iUserActive)
		iOwningWsAnim.Panic(EWservPanicAnimLeave);
	if (iGc)
		iGc->DrawPolyLine(aPointList->Array());
	}

TInt CWsAnimGc::DrawPolygon(const TPoint* aPointList,TInt aNumPoints,TFillRule aFillRule)
	{
	TInt result = KErrGeneral;
	if (!iUserActive)
		iOwningWsAnim.Panic(EWservPanicAnimLeave);
	if (iGc)
		{
		TArrayWrapper<TPoint> pointsArray(aPointList, aNumPoints);
		iGc->DrawPolygon(pointsArray, BitGdiToMWsGraphicsContextMappings::Convert(aFillRule));
		result = iGc->GetError();
		}
	return result;
	}

TInt CWsAnimGc::DrawPolygon(const CArrayFix<TPoint> *aPointList,TFillRule aFillRule)
	{
	if (!iUserActive)
		iOwningWsAnim.Panic(EWservPanicAnimLeave);
	if(!aPointList)
		return KErrArgument;
	TInt result = KErrGeneral;
	if (iGc)
		{
		iGc->DrawPolygon(aPointList->Array(), BitGdiToMWsGraphicsContextMappings::Convert(aFillRule));
		result = iGc->GetError(); 
		}
	return result;
	}

void CWsAnimGc::DrawText(const TDesC &aString,const TPoint &aPosition)
	{
	if (!iFont)
		iOwningWsAnim.Panic(EWservPanicNoFont);
	if (iGc)
		iGc->DrawText(aString, NULL, aPosition);
	}

void CWsAnimGc::DrawText(const TDesC &aString,const TRect &aBox,TInt aBaselineOffset,TTextAlign aHoriz,TInt aLeftMrg)
	{
	if (!iFont)
		iOwningWsAnim.Panic(EWservPanicNoFont);
	if (iGc)
		iGc->DrawText(aString, NULL, aBox, aBaselineOffset, BitGdiToMWsGraphicsContextMappings::Convert(aHoriz), aLeftMrg);	
	}

void CWsAnimGc::MoveTo(const TPoint &aPoint)
	{
	if (!iUserActive)
		iOwningWsAnim.Panic(EWservPanicAnimLeave);
	if (iGc)
		iGc->MoveTo(aPoint);
	}

void CWsAnimGc::MoveBy(const TPoint &aPoint)
	{
	if (!iUserActive)
		iOwningWsAnim.Panic(EWservPanicAnimLeave);
	if (iGc)
		iGc->MoveBy(aPoint);
	}

void CWsAnimGc::Plot(const TPoint &aPoint)
	{
	if (!iUserActive)
		iOwningWsAnim.Panic(EWservPanicAnimLeave);
	if (iGc)
		iGc->Plot(aPoint);
	}

void CWsAnimGc::SetOrigin(const TPoint &aPoint)
	{
	if (!iUserActive)
		iOwningWsAnim.Panic(EWservPanicAnimLeave);
	if (iGc)
		iGc->SetOrigin(iOwningWsAnim.ClientWindow()->Origin() + aPoint);
	}

void CWsAnimGc::CopyRect(const TPoint& aOffset,const TRect& aRect)
	{
	if (!iUserActive)
		iOwningWsAnim.Panic(EWservPanicAnimLeave);
	if (iGc)
		iGc->CopyRect(aOffset, aRect);
	}

void CWsAnimGc::Reset()
	{
	CloseCurrentFont();
	if (!iUserActive)
		iOwningWsAnim.Panic(EWservPanicAnimLeave);
	if (iGc)
		iGc->Reset();
	SetOrigin(TPoint(0,0));
	CancelClippingRect();
	CancelClippingRegion();
	}

void CWsAnimGc::CloseCurrentFont()
	{
	if (iFont)
		{
		iFont->Close();
		iFont=NULL;
		}
	}

/**
@see CBitmapContext::AlphaBlendBitmaps()*/
TInt CWsAnimGc::AlphaBlendBitmaps(const TPoint& aDestPt, const CFbsBitmap* aSrcBmp,	const TRect& aSrcRect, const CFbsBitmap* aAlphaBmp,	const TPoint& aAlphaPt)	
	{
	TInt result = KErrGeneral;
	if (!iUserActive)
		iOwningWsAnim.Panic(EWservPanicAnimLeave);
	if (iGc)
		{
		iGc->BitBltMasked(aDestPt, *aSrcBmp, aSrcRect, *aAlphaBmp, aAlphaPt);
		result = iGc->GetError();
		}
	return result;
	}

/*Body stub implementation. The alternate AlphaBlendBitmap function using CFbsBitmaps should instead be used*/
TInt CWsAnimGc::AlphaBlendBitmaps(const TPoint& /*aDestPt*/, const CWsBitmap* /*aSrcBmp*/,	const TRect& /*aSrcRect*/, const CWsBitmap* /*aAlphaBmp*/,	const TPoint& /*aAlphaPt*/)
	{
	return KErrNone;		
	};

/**
@see CGraphicsContext::MapColors()*/
void CWsAnimGc::MapColors(const TRect& /*aRect*/,const TRgb* /*aColors*/,TInt /*aNumPairs*/,TBool /*aMapForwards*/)
	{
	if (!iUserActive)
		iOwningWsAnim.Panic(EWservPanicAnimLeave);
	}

/**
@see CGraphicsContext::DrawTextVertical()*/
void CWsAnimGc::DrawTextVertical(const TDesC& aText,const TPoint& aPos,TBool aUp)
	{
	if (!iFont) 
		iOwningWsAnim.Panic(EWservPanicNoFont);
	if (iGc)
		iGc->DrawTextVertical(aText, NULL, aPos, aUp);
	}

/**
@see CGraphicsContext::DrawTextVertical()*/
void CWsAnimGc::DrawTextVertical(const TDesC& aText,const TRect& aBox,TInt aBaselineOffset,TBool aUp,TTextAlign aVert,TInt aMargin)
	{
	if (!iFont) 
		iOwningWsAnim.Panic(EWservPanicNoFont);
	if (iGc)
		iGc->DrawTextVertical(aText, NULL, aBox, aBaselineOffset, aUp, BitGdiToMWsGraphicsContextMappings::Convert(aVert), aMargin);
	}
	
