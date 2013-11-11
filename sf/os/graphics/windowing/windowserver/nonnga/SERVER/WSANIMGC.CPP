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

CWsAnimGc::CWsAnimGc() : iUserActive(EFalse), iUseDrawingRegion(EFalse)
	{
	__DECLARE_NAME(_S("CWsAnimGc"));
	}

void CWsAnimGc::SetDrawingRegions(const TRegion *aRegion)
	{
	if (aRegion)
		{
		iDrawingRegion.Copy(*aRegion);
		iDrawingRegion.Offset(iWin->Origin());
		}
	iDrawingRegion.Intersect(*iBaseRegion);
	iDrawingRegion.Tidy();
	iGc->SetClippingRegion(iDrawingRegion);
	iUseDrawingRegion=ETrue;
	}

void CWsAnimGc::Activate(CWsClientWindow *aWindow, CWsAnim *aWsAnim, const TRegion *aRegion, CFbsBitGc *aGc)
	{
	iOwningWsAnim=aWsAnim;
	if (iWin)
		iOwningWsAnim->Panic();
	iWin=aWindow;
	iBaseRegion=aRegion ? aRegion : &iWin->VisibleRegion();

	iGc = aGc;
	iUserActive = ETrue;
	Reset();
	}

void CWsAnimGc::Deactivate()
	{
	if (iUseDrawingRegion)
		{
		iDrawingRegion.Close();
		}
	iUseDrawingRegion=EFalse;
	CloseCurrentFont();

	iWin=NULL;
	iGc=NULL;
	iUserActive = EFalse;
	}

TBool CWsAnimGc::IsActive() const
	{
	return (iUserActive || (iGc != NULL));
	}

void CWsAnimGc::UserActivate(CWsClientWindow *aWindow, CWsAnim *aWsAnim)
	{
	if (iWin)
		{
		UserDeactivate();
		iOwningWsAnim->Panic();
		}
	iOwningWsAnim=aWsAnim;
	iWin=aWindow;
	iUserActive = ETrue;
	}

void CWsAnimGc::UserDeactivate()
	{
	CloseCurrentFont();

	if (iWin && iOwningWsAnim)
		{
		TRegionFix<1> animRegion(iOwningWsAnim->BestRect());
		iWin->ScheduleRegionUpdate(&animRegion);
		}

	iWin = NULL;
	iUserActive = EFalse;
	}

void CWsAnimGc::AnimDeleted(CWsAnim *aWsAnim)
	{	
	if (iOwningWsAnim==aWsAnim)
		iOwningWsAnim=NULL;
	}

CGraphicsDevice* CWsAnimGc::Device() const
	{
	if (!iUserActive)
		iOwningWsAnim->Panic(EWservPanicAnimLeave);

	return iGc ? iGc->Device() : NULL;
	}

void CWsAnimGc::SetClippingRect(const TRect& aRect)
	{
	if (!iUserActive)
		iOwningWsAnim->Panic(EWservPanicAnimLeave);

	if (iGc)
		{
		iClippingRect=aRect;
		iClippingRect.Intersection(TRect(iWin->Size()));
		iGc->SetClippingRect(iClippingRect);
		}
	}

TInt CWsAnimGc::SetClippingRegion(const TRegion &aRegion)
//This function is virtual and so cannot have an 'L' at the end of it's name
	{
	if (iWin==NULL)
		User::Leave(KErrGeneral);

	if (iGc)
		SetDrawingRegions(&aRegion);
	return KErrNone;
	}

void CWsAnimGc::CancelClippingRegion()
	{
	if (!iUserActive)
		iOwningWsAnim->Panic(EWservPanicAnimLeave);

	if (iGc)
		{
		iDrawingRegion.Clear();
		iUseDrawingRegion=EFalse;
		iGc->SetClippingRegion(iBaseRegion);
		}
	}

void CWsAnimGc::CancelClippingRect()
	{
	if (!iUserActive)
		iOwningWsAnim->Panic(EWservPanicAnimLeave);

	if (iGc)
		{
		iClippingRect=TRect(iWin->Size());
		iGc->SetClippingRect(iClippingRect);
		}
	}

void CWsAnimGc::SetDrawMode(TDrawMode aDrawingMode)
	{
	if (!iUserActive)
		iOwningWsAnim->Panic(EWservPanicAnimLeave);

	if (iGc)
		iGc->SetDrawMode(aDrawingMode);
	}

void CWsAnimGc::UseFont(const CFont *aFont)
	{
	CloseCurrentFont();
	iFont=(CAnimFbsFont *)aFont;
	iFont->Open();
	if (!iUserActive)
		iOwningWsAnim->Panic(EWservPanicAnimLeave);
	if (iGc)
		iGc->UseFontNoDuplicate(iFont);
	}

void CWsAnimGc::DiscardFont()
	{
	CloseCurrentFont();
	if (!iUserActive)
		iOwningWsAnim->Panic(EWservPanicAnimLeave);
	if (iGc)
		iGc->DiscardFont();
	}

void CWsAnimGc::SetUnderlineStyle(TFontUnderline aUnderlineStyle)
	{
	if (!iUserActive)
		iOwningWsAnim->Panic(EWservPanicAnimLeave);
	if (iGc)
		iGc->SetUnderlineStyle(aUnderlineStyle);
	}

void CWsAnimGc::SetStrikethroughStyle(TFontStrikethrough aStrikethroughStyle)
	{
	if (!iUserActive)
		iOwningWsAnim->Panic(EWservPanicAnimLeave);
	if (iGc)
		iGc->SetStrikethroughStyle(aStrikethroughStyle);
	}

void CWsAnimGc::SetWordJustification(TInt aExcessWidth,TInt aNumGaps)
	{
	if (!iUserActive)
		iOwningWsAnim->Panic(EWservPanicAnimLeave);
	if (iGc)
		iGc->SetWordJustification(aExcessWidth,aNumGaps);
	}

void CWsAnimGc::SetCharJustification(TInt aExcessWidth,TInt aNumChars)
	{
	if (!iUserActive)
		iOwningWsAnim->Panic(EWservPanicAnimLeave);
	if (iGc)
		iGc->SetCharJustification(aExcessWidth,aNumChars);
	}

void CWsAnimGc::SetPenColor(const TRgb &aColor)
	{
	if (!iUserActive)
		iOwningWsAnim->Panic(EWservPanicAnimLeave);
	if (iGc)
		iGc->SetPenColor(aColor);
	}

void CWsAnimGc::SetPenStyle(TPenStyle aPenStyle)
	{
	if (!iUserActive)
		iOwningWsAnim->Panic(EWservPanicAnimLeave);
	if (iGc)
		iGc->SetPenStyle(aPenStyle);
	}

void CWsAnimGc::SetPenSize(const TSize& aSize)
	{
	if (!iUserActive)
		iOwningWsAnim->Panic(EWservPanicAnimLeave);
	if (iGc)
		iGc->SetPenSize(aSize);
	}

void CWsAnimGc::SetBrushColor(const TRgb &aColor)
	{
	if (!iUserActive)
		iOwningWsAnim->Panic(EWservPanicAnimLeave);
	if (iGc)
		iGc->SetBrushColor(aColor);
	}

void CWsAnimGc::SetBrushStyle(TBrushStyle aBrushStyle)
	{
	if (!iUserActive)
		iOwningWsAnim->Panic(EWservPanicAnimLeave);
	if (iGc)
		iGc->SetBrushStyle(aBrushStyle);
	}

void CWsAnimGc::SetBrushOrigin(const TPoint &aOrigin)
	{
	if (!iUserActive)
		iOwningWsAnim->Panic(EWservPanicAnimLeave);
	if (iGc)
		iGc->SetBrushOrigin(aOrigin);
	}

void CWsAnimGc::UseBrushPattern(const CFbsBitmap *aBitmap)
	{
	if (!iUserActive)
		iOwningWsAnim->Panic(EWservPanicAnimLeave);
	if (iGc)
		iGc->UseBrushPattern(aBitmap);
	}

void CWsAnimGc::DiscardBrushPattern()
	{
	if (!iUserActive)
		iOwningWsAnim->Panic(EWservPanicAnimLeave);
	if (iGc)
		iGc->DiscardBrushPattern();
	}

void CWsAnimGc::SetFaded(TBool aFaded)
	{
	if (!iUserActive)
		iOwningWsAnim->Panic(EWservPanicAnimLeave);
	if (iGc)
		iGc->SetFaded(aFaded);
	}

void CWsAnimGc::SetFadingParameters(TUint8 aBlackMap,TUint8 aWhiteMap)
	{
	if (!iUserActive)
		iOwningWsAnim->Panic(EWservPanicAnimLeave);
	if (iGc)
		iGc->SetFadingParameters(aBlackMap,aWhiteMap);
	}

//
// Drawing commands 								//
//

void CWsAnimGc::DrawArc(const TRect &aRect,const TPoint &aStart,const TPoint &aEnd)
	{
	if (!iUserActive)
		iOwningWsAnim->Panic(EWservPanicAnimLeave);
	if (iGc)
		iGc->DrawArc(aRect,aStart,aEnd);
	}

void CWsAnimGc::DrawPie(const TRect &aRect,const TPoint &aStart,const TPoint &aEnd)
	{
	if (!iUserActive)
		iOwningWsAnim->Panic(EWservPanicAnimLeave);
	if (iGc)
		iGc->DrawPie(aRect,aStart,aEnd);
	}

void CWsAnimGc::DrawLine(const TPoint &aPoint1,const TPoint &aPoint2)
	{
	if (!iUserActive)
		iOwningWsAnim->Panic(EWservPanicAnimLeave);
	if (iGc)
		iGc->DrawLine(aPoint1, aPoint2);
	}

void CWsAnimGc::DrawLineTo(const TPoint &aPoint)
	{
	if (!iUserActive)
		iOwningWsAnim->Panic(EWservPanicAnimLeave);
	if (iGc)
		iGc->DrawLineTo(aPoint);
	}

void CWsAnimGc::DrawLineBy(const TPoint &aPoint)
	{
	if (!iUserActive)
		iOwningWsAnim->Panic(EWservPanicAnimLeave);
	if (iGc)
		iGc->DrawLineBy(aPoint);
	}

void CWsAnimGc::DrawEllipse(const TRect &aRect)
	{
	if (!iUserActive)
		iOwningWsAnim->Panic(EWservPanicAnimLeave);
	if (iGc)
		iGc->DrawEllipse(aRect);
	}

void CWsAnimGc::DrawRect(const TRect &aRect)
	{
	if (!iUserActive)
		iOwningWsAnim->Panic(EWservPanicAnimLeave);
	if (iGc)
		iGc->DrawRect(aRect);
	}

void CWsAnimGc::DrawRoundRect(const TRect &aRect,const TSize &aCornerSize)
	{
	if (!iUserActive)
		iOwningWsAnim->Panic(EWservPanicAnimLeave);
	if (iGc)
		iGc->DrawRoundRect(aRect, aCornerSize);
	}

void CWsAnimGc::Clear(const TRect &aRect)
	{
	if (!iUserActive)
		iOwningWsAnim->Panic(EWservPanicAnimLeave);
	if (iGc)
		iGc->Clear(aRect);
	}

void CWsAnimGc::Clear()
	{
	if (!iUserActive)
		iOwningWsAnim->Panic(EWservPanicAnimLeave);
	if (iGc)
		iGc->Clear();
	}

void CWsAnimGc::BitBlt(const TPoint &aPoint,const CFbsBitmap *aBitmap)
	{
	if (!iUserActive)
		iOwningWsAnim->Panic(EWservPanicAnimLeave);
	if (iGc)
		iGc->BitBlt(aPoint, aBitmap);
	}

void CWsAnimGc::BitBlt(const TPoint &aDestination,const CFbsBitmap *aBitmap,const TRect &aSource)
	{
	if (!iUserActive)
		iOwningWsAnim->Panic(EWservPanicAnimLeave);
	if (iGc)
		iGc->BitBlt(aDestination, aBitmap, aSource);
	}

void CWsAnimGc::BitBltMasked(const TPoint& aPoint,const CFbsBitmap* aBitmap,const TRect& aSourceRect,const CFbsBitmap* aMaskBitmap,TBool aInvertMask)
	{
	if (!iUserActive)
		iOwningWsAnim->Panic(EWservPanicAnimLeave);
	if (iGc)
		iGc->BitBltMasked(aPoint, aBitmap, aSourceRect, aMaskBitmap, aInvertMask);
	}

void CWsAnimGc::DrawBitmap(const TPoint &aTopLeft, const CFbsBitmap *aSource)
	{
	if (!iUserActive)
		iOwningWsAnim->Panic(EWservPanicAnimLeave);
	if (iGc)
		iGc->DrawBitmap(aTopLeft, aSource);
	}

void CWsAnimGc::DrawBitmap(const TRect &aDestRect, const CFbsBitmap *aSource)
	{
	if (!iUserActive)
		iOwningWsAnim->Panic(EWservPanicAnimLeave);
	if (iGc)
		iGc->DrawBitmap(aDestRect, aSource);
	}

void CWsAnimGc::DrawBitmap(const TRect &aDestRect, const CFbsBitmap *aSource, const TRect &aSrcRect)
	{
	if (!iUserActive)
		iOwningWsAnim->Panic(EWservPanicAnimLeave);
	if (iGc)
		iGc->DrawBitmap(aDestRect, aSource, aSrcRect);
	}

void CWsAnimGc::DrawBitmapMasked(const TRect &aDestRect, const CFbsBitmap *aBitmap, const TRect &aSrcRect, const CFbsBitmap* aMaskBitmap, TBool aInvertMask)
	{
	if (!iUserActive)
		iOwningWsAnim->Panic(EWservPanicAnimLeave);
	if (iGc)
		iGc->DrawBitmapMasked(aDestRect, aBitmap, aSrcRect, aMaskBitmap, aInvertMask);
	}

void CWsAnimGc::DrawBitmapMasked(const TRect& aDestRect,const CWsBitmap* aBitmap,const TRect& aSrcRect,const CWsBitmap* aMaskBitmap, TBool aInvertMask)
	{
	if (!iUserActive)
		iOwningWsAnim->Panic(EWservPanicAnimLeave);
	if (iGc)
		iGc->DrawBitmapMasked(aDestRect, aBitmap, aSrcRect, aMaskBitmap, aInvertMask);
	}

void CWsAnimGc::DrawPolyLine(const TPoint* aPointList,TInt aNumPoints)
	{
	if (!iUserActive)
		iOwningWsAnim->Panic(EWservPanicAnimLeave);
	if (iGc)
		iGc->DrawPolyLine(aPointList, aNumPoints);
	}

void CWsAnimGc::DrawPolyLine(const CArrayFix<TPoint> *aPointList)
	{
	if (!iUserActive)
		iOwningWsAnim->Panic(EWservPanicAnimLeave);
	if (iGc)
		iGc->DrawPolyLine(aPointList);
	}

TInt CWsAnimGc::DrawPolygon(const TPoint* aPointList,TInt aNumPoints,TFillRule aFillRule)
	{
	TInt result = KErrNone;
	if (!iUserActive)
		iOwningWsAnim->Panic(EWservPanicAnimLeave);
	if (iGc)
		result = iGc->DrawPolygon(aPointList, aNumPoints, aFillRule);
	return result;
	}

TInt CWsAnimGc::DrawPolygon(const CArrayFix<TPoint> *aPointList,TFillRule aFillRule)
	{
	TInt result = KErrNone;
	if (!iUserActive)
		iOwningWsAnim->Panic(EWservPanicAnimLeave);
	if (iGc)
		result = iGc->DrawPolygon(aPointList, aFillRule);
	return result;
	}

void CWsAnimGc::DrawText(const TDesC &aString,const TPoint &aPosition)
	{
	if (!iFont)
		iOwningWsAnim->Panic(EWservPanicNoFont);
	if (iGc)
		iGc->DrawText(aString, aPosition);
	}

void CWsAnimGc::DrawText(const TDesC &aString,const TRect &aBox,TInt aBaselineOffset,TTextAlign aHoriz,TInt aLeftMrg)
	{
	if (!iFont)
		iOwningWsAnim->Panic(EWservPanicNoFont);
	if (iGc)
		iGc->DrawText(aString, aBox, aBaselineOffset, aHoriz, aLeftMrg);	
	}

void CWsAnimGc::MoveTo(const TPoint &aPoint)
	{
	if (!iUserActive)
		iOwningWsAnim->Panic(EWservPanicAnimLeave);
	if (iGc)
		iGc->MoveTo(aPoint);
	}

void CWsAnimGc::MoveBy(const TPoint &aPoint)
	{
	if (!iUserActive)
		iOwningWsAnim->Panic(EWservPanicAnimLeave);
	if (iGc)
		iGc->MoveBy(aPoint);
	}

void CWsAnimGc::Plot(const TPoint &aPoint)
	{
	if (!iUserActive)
		iOwningWsAnim->Panic(EWservPanicAnimLeave);
	if (iGc)
		iGc->Plot(aPoint);
	}

void CWsAnimGc::SetOrigin(const TPoint &aPoint)
	{
	if (!iUserActive)
		iOwningWsAnim->Panic(EWservPanicAnimLeave);
	if (iGc)
		iGc->SetOrigin(iWin->Origin() + aPoint);
	}

void CWsAnimGc::CopyRect(const TPoint& aOffset,const TRect& aRect)
	{
	if (!iUserActive)
		iOwningWsAnim->Panic(EWservPanicAnimLeave);
	if (iGc)
		iGc->CopyRect(aOffset, aRect);
	}

void CWsAnimGc::Reset()
	{
	CloseCurrentFont();
	if (!iUserActive)
		iOwningWsAnim->Panic(EWservPanicAnimLeave);
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
	TInt result = KErrNone;
	if (!iUserActive)
		iOwningWsAnim->Panic(EWservPanicAnimLeave);
	if (iGc)
		result = iGc->AlphaBlendBitmaps(aDestPt, aSrcBmp, aSrcRect, aAlphaBmp, aAlphaPt);
	return result;
	}

/*Body stub implementation. The alternate AlphaBlendBitmap function using CFbsBitmaps should instead be used*/
TInt CWsAnimGc::AlphaBlendBitmaps(const TPoint& /*aDestPt*/, const CWsBitmap* /*aSrcBmp*/,	const TRect& /*aSrcRect*/, const CWsBitmap* /*aAlphaBmp*/,	const TPoint& /*aAlphaPt*/)
	{
	return KErrNone;		
	};

/**
@see CGraphicsContext::MapColors()*/
void CWsAnimGc::MapColors(const TRect &aRect,const TRgb *aColors,TInt aNumPairs,TBool aMapForwards)
	{
	if (!iUserActive)
		iOwningWsAnim->Panic(EWservPanicAnimLeave);
	if (iGc)
		iGc->MapColors(aRect, aColors, aNumPairs, aMapForwards);
	}
	
/**
@see CGraphicsContext::DrawTextVertical()*/
void CWsAnimGc::DrawTextVertical(const TDesC& aText,const TPoint& aPos,TBool aUp)
	{
	if (!iFont) 
		iOwningWsAnim->Panic(EWservPanicNoFont);
	if (iGc)
		iGc->DrawTextVertical(aText, aPos, aUp);
	}

/**
@see CGraphicsContext::DrawTextVertical()*/
void CWsAnimGc::DrawTextVertical(const TDesC& aText,const TRect& aBox,TInt aBaselineOffset,TBool aUp,TTextAlign aVert,TInt aMargin)
	{
	if (!iFont) 
		iOwningWsAnim->Panic(EWservPanicNoFont);
	if (iGc)
		iGc->DrawTextVertical(aText, aBox, aBaselineOffset, aUp, aVert, aMargin);
	}
	
