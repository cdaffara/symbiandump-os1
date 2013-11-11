// Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include "tcontextbase.h"
#include <gdi.h>
#include <bitdev.h>

CTBitGdiContext::CTBitGdiContext() :
    iError(KErrNone)
	{

	}

CTBitGdiContext* CTBitGdiContext::NewL(CFbsBitmapDevice* aDevice, TBool aActivate)
	{
	CTBitGdiContext* self = new (ELeave) CTBitGdiContext;
	CleanupStack::PushL (self);
	self->ConstructL (aDevice, aActivate);
	CleanupStack::Pop ();
	return self;
	}

void CTBitGdiContext::ConstructL(CFbsBitmapDevice* aDevice, TBool aActivate)
	{
	if(aActivate)
		{
		User::LeaveIfError(aDevice->CreateContext (iGc));
		}
	else
		{
		iGc = CFbsBitGc::NewL();
		}
	}

CTBitGdiContext::~CTBitGdiContext()
	{
	delete iGc;
	}

TInt CTBitGdiContext::Activate(CBitmapDevice *aDevice)
	{
	CFbsDevice* device = (CFbsDevice *)aDevice;
	iGc->Activate(device);
	return KErrNone;
	}

void CTBitGdiContext::SetOrigin(const TPoint& aOrigin)
	{
	iGc->SetOrigin (aOrigin);
	}

void CTBitGdiContext::SetClippingRegion(const TRegion& aRegion)
	{
	TInt result = iGc->SetClippingRegion (aRegion);
	SetError(result);
	}

void CTBitGdiContext::ResetClippingRegion()
	{
	iGc->CancelClipping();
	}

void CTBitGdiContext::SetDrawMode(DirectGdi::TDrawMode aMode)
	{
	CGraphicsContext::TDrawMode mode = CGraphicsContext::EDrawModePEN;	
	if (aMode == DirectGdi::EDrawModeWriteAlpha)
		{
		mode = CGraphicsContext::EDrawModeWriteAlpha;
		}	
	iGc->SetDrawMode(mode);
	}

void CTBitGdiContext::SetPenColor(const TRgb& aColor)
	{
	iGc->SetPenColor (aColor);
	}

void CTBitGdiContext::SetPenStyle(DirectGdi::TPenStyle aStyle)
	{
	// The CGraphicsContext::TPenStyle enumeration has the same values as the
	// DirectGdi::TPenStyle. If these change then this function will need to be updated.
	iGc->SetPenStyle(static_cast<CGraphicsContext::TPenStyle>(aStyle));
	}

void CTBitGdiContext::SetPenSize(const TSize& aSize)
	{	
	iGc->SetPenSize (aSize);
	}

void CTBitGdiContext::SetTextShadowColor(const TRgb& aColor)
	{
	iGc->SetShadowColor(aColor);
	}

void CTBitGdiContext::SetBrushColor(const TRgb& aColor)
	{
	iGc->SetBrushColor (aColor);
	}

void CTBitGdiContext::SetBrushStyle(DirectGdi::TBrushStyle aStyle)
	{
	// The CGraphicsContext::TBrushStyle enumeration has the same values as the
	// DirectGdi::TBrushStyle. If these change then this function will need to be updated.
	iGc->SetBrushStyle(static_cast<CGraphicsContext::TBrushStyle>(aStyle));
	}

void CTBitGdiContext::SetBrushOrigin(const TPoint& aOrigin)
	{
	iGc->SetBrushOrigin (aOrigin);
	}

void CTBitGdiContext::SetBrushPattern(const CFbsBitmap& aPattern)
	{
	iGc->UseBrushPattern (&aPattern);
	}

void CTBitGdiContext::SetBrushPattern(TInt aHandle)
	{
	TInt result = iGc->UseBrushPattern(aHandle);
	SetError(result);
	}

void CTBitGdiContext::ResetBrushPattern()
	{
	iGc->DiscardBrushPattern ();
	}

void CTBitGdiContext::SetFont(const CFont* aFont)
	{
	iGc->UseFont (aFont);
	}

void CTBitGdiContext::ResetFont()
	{
	iGc->DiscardFont ();
	}

void CTBitGdiContext::Reset()
	{
	iGc->Reset ();
	}

TRgb CTBitGdiContext::BrushColor()
	{
	return iGc->BrushColor ();
	}

TRgb CTBitGdiContext::PenColor()
	{
	return iGc->PenColor ();
	}

TRgb CTBitGdiContext::TextShadowColor()
	{
	TRgb shadowColor;
	iGc->GetShadowColor(shadowColor);
	return shadowColor;
	}

void CTBitGdiContext::Clear(const TRect& aRect)
	{
	iGc->Clear (aRect);
	}

void CTBitGdiContext::Clear()
	{
	iGc->Clear ();
	}

void CTBitGdiContext::MoveTo(const TPoint& aPoint)
	{
	iGc->MoveTo (aPoint);
	}

void CTBitGdiContext::MoveBy(const TPoint& aVector)
	{
	iGc->MoveBy (aVector);
	}

void CTBitGdiContext::Plot(const TPoint& aPoint)
	{
	iGc->Plot (aPoint);
	}

void CTBitGdiContext::DrawLine(const TPoint& aStart, const TPoint& aEnd)
	{
	iGc->DrawLine (aStart, aEnd);
	}

void CTBitGdiContext::DrawLineTo(const TPoint& aPoint)
	{
	iGc->DrawLineTo (aPoint);
	}

void CTBitGdiContext::DrawLineBy(const TPoint& aVector)
	{
	iGc->DrawLineBy (aVector);
	}

void CTBitGdiContext::DrawRect(const TRect& aRect)
	{
	iGc->DrawRect (aRect);
	}

void CTBitGdiContext::DrawRoundRect(const TRect& aRect,
		const TSize& aCornerSize)
	{
	iGc->DrawRoundRect (aRect, aCornerSize);
	}

void CTBitGdiContext::DrawPolyLine(const CArrayFix<TPoint>& aPointList)
	{
	iGc->DrawPolyLine(&aPointList);
	}

void CTBitGdiContext::DrawPolyLineNoEndPoint(const CArrayFix<TPoint>& aPointList)
	{
	iGc->DrawPolyLineNoEndPoint(&aPointList);
	}

void CTBitGdiContext::DrawPolygon(const CArrayFix<TPoint>& aPoints, DirectGdi::TFillRule aRule)
	{
	TInt result = iGc->DrawPolygon(&aPoints, static_cast<CGraphicsContext::TFillRule>(aRule));
	SetError(result);
	}

void CTBitGdiContext::DrawArc(const TRect& aRect, const TPoint& aStart,
		const TPoint& aEnd)
	{
	iGc->DrawArc (aRect, aStart, aEnd);
	}

void CTBitGdiContext::DrawPie(const TRect& aRect, const TPoint& aStart,
		const TPoint& aEnd)
	{
	iGc->DrawPie (aRect, aStart, aEnd);
	}

void CTBitGdiContext::DrawEllipse(const TRect& aRect)
	{
	iGc->DrawEllipse (aRect);
	}

void CTBitGdiContext::BitBltMasked(const TPoint& aDestPos, 
		const CFbsBitmap& aBitmap, const TRect& aSourceRect, 
		const CFbsBitmap& aAlpha, const TPoint& aAlphaPos)
	{
	// This overload of BitBltMasked maps to AlphaBlendBitmaps in BitGDI, which has the same signiature.
	TInt result = iGc->AlphaBlendBitmaps(aDestPos, &aBitmap, aSourceRect, &aAlpha, aAlphaPos);
	SetError(result);
	}

void CTBitGdiContext::BitBlt(const TPoint& aPoint, const CFbsBitmap& aBitmap)
	{
	iGc->BitBlt (aPoint, &aBitmap);
	}

void CTBitGdiContext::DrawBitmap(const TRect& aDestRect,
		const CFbsBitmap& aSource)
	{
	iGc->DrawBitmap (aDestRect, &aSource);
	}

void CTBitGdiContext::BitBlt(const TPoint& aDestPos,
		const CFbsBitmap& aBitmap, const TRect& aSrcRect)
	{
	iGc->BitBlt (aDestPos, &aBitmap, aSrcRect);
	}

void CTBitGdiContext::BitBltMasked(const TPoint& aDestPos,
		const CFbsBitmap& aBitmap, const TRect& aSrcRect,
		const CFbsBitmap& aMask, TBool aInvertMask)
	{
	iGc->BitBltMasked (aDestPos, &aBitmap, aSrcRect, &aMask, aInvertMask);
	}

void CTBitGdiContext::DrawBitmap(const TRect& aDestRect,
		const CFbsBitmap& aBitmap, const TRect& aSrcRect)
	{
	iGc->DrawBitmap (aDestRect, &aBitmap, aSrcRect);
	}

void CTBitGdiContext::DrawBitmapMasked(const TRect& aDestRect,
		const CFbsBitmap& aBitmap, const TRect& aSrcRect,
		const CFbsBitmap& aMask, TBool aInvertMask)
	{
	iGc->DrawBitmapMasked (aDestRect, &aBitmap, aSrcRect, &aMask, aInvertMask);
	}

void CTBitGdiContext::CopyRect(const TPoint& aOffset, const TRect& aRect)
	{
	iGc->CopyRect (aOffset, aRect);
	}

TBool CTBitGdiContext::HasBrushPattern() const
	{
	return iGc->IsBrushPatternUsed ();
	}

TBool CTBitGdiContext::HasFont() const
	{
	return iGc->IsFontUsed ();
	}

void CTBitGdiContext::ExternalizeL(RWriteStream& aWriteStream)
	{
	iGc->ExternalizeL (aWriteStream);
	}

void CTBitGdiContext::InternalizeL(RReadStream& aReadStream)
	{
	iGc->InternalizeL (aReadStream);
	}

void CTBitGdiContext::SetCharJustification(TInt aExcessWidth, TInt aNumGaps)
	{
	iGc->SetCharJustification (aExcessWidth, aNumGaps);
	}

void CTBitGdiContext::SetWordJustification(TInt aExcessWidth, TInt aNumChars)
	{
	iGc->SetWordJustification (aExcessWidth, aNumChars);
	}

void CTBitGdiContext::SetUnderlineStyle(DirectGdi::TFontUnderline aUnderlineStyle)
	{
	iGc->SetUnderlineStyle (static_cast<TFontUnderline>(aUnderlineStyle));
	}

void CTBitGdiContext::SetStrikethroughStyle(DirectGdi::TFontStrikethrough aStrikethroughStyle)
	{
	iGc->SetStrikethroughStyle (static_cast<TFontStrikethrough>(aStrikethroughStyle));
	}

void CTBitGdiContext::UpdateJustification(const TDesC& aText, const DirectGdi::TTextParameters* aParam)
	{
	iGc->UpdateJustification(aText, reinterpret_cast<const CGraphicsContext::TTextParameters*>(aParam));
	}

void CTBitGdiContext::UpdateJustificationVertical(const TDesC& aText, const DirectGdi::TTextParameters* aParam, TBool aUp)
	{
	iGc->UpdateJustificationVertical(aText, reinterpret_cast<const CGraphicsContext::TTextParameters*>(aParam), aUp);
	}

void CTBitGdiContext::SetFontNoDuplicate(const CFont* /*aFont*/)
	{
	}

void CTBitGdiContext::SetError(TInt aErr)
	{
	if (KErrNone == iError)
		{
		iError = aErr;
		}	
	}


TInt CTBitGdiContext::GetError()
	{
	TInt err = iError;
	iError = KErrNone;
	return err;
	}

// text drawing
void CTBitGdiContext::DrawText(const TDesC& aText, const DirectGdi::TTextParameters* aParam)
	{
	iGc->DrawText(aText, reinterpret_cast<const CGraphicsContext::TTextParameters*>(aParam));
	}

void CTBitGdiContext::DrawText(const TDesC& aText, const DirectGdi::TTextParameters* aParam, const TPoint& aPosition)
	{
	iGc->DrawText(aText, reinterpret_cast<const CGraphicsContext::TTextParameters*>(aParam), aPosition);
	}

void CTBitGdiContext::DrawText(const TDesC& aText, const DirectGdi::TTextParameters* aParam, const TRect& aBox)
	{
	iGc->DrawText(aText, reinterpret_cast<const CGraphicsContext::TTextParameters*>(aParam), aBox);
	}

void CTBitGdiContext::DrawText(const TDesC& aText, const DirectGdi::TTextParameters* aParam, const TRect& aBox, TInt aBaselineOffset, 
		DirectGdi::TTextAlign aAlignment, TInt aMargin)
	{
	iGc->DrawText(aText, reinterpret_cast<const CGraphicsContext::TTextParameters*>(aParam), aBox, aBaselineOffset, static_cast<CGraphicsContext::TTextAlign>(aAlignment), aMargin);
	}

void CTBitGdiContext::DrawText(const TDesC& aText, const DirectGdi::TTextParameters* aParam, const TRect& aBox, TInt aBaselineOffset, 
		TInt aTextWidth, DirectGdi::TTextAlign aAlignment, TInt aMargin)
	{
	iGc->DrawText(aText, reinterpret_cast<const CGraphicsContext::TTextParameters*>(aParam), aBox, aBaselineOffset, aTextWidth, static_cast<CGraphicsContext::TTextAlign>(aAlignment), aMargin);
	}

void CTBitGdiContext::DrawTextVertical(const TDesC& aText, const DirectGdi::TTextParameters* aParam, TBool aUp)
	{
	iGc->DrawTextVertical(aText, reinterpret_cast<const CGraphicsContext::TTextParameters*>(aParam), aUp);
	}

void CTBitGdiContext::DrawTextVertical(const TDesC& aText, const DirectGdi::TTextParameters* aParam, const TPoint& aPosition, TBool aUp)
	{
	iGc->DrawTextVertical(aText, reinterpret_cast<const CGraphicsContext::TTextParameters*>(aParam), aPosition, aUp);
	}

void CTBitGdiContext::DrawTextVertical(const TDesC& aText, const DirectGdi::TTextParameters* aParam, const TRect& aBox, TBool aUp)
	{
	iGc->DrawTextVertical(aText, reinterpret_cast<const CGraphicsContext::TTextParameters*>(aParam), aBox, aUp);
	}

void CTBitGdiContext::DrawTextVertical(const TDesC& aText, const DirectGdi::TTextParameters* aParam, const TRect& aBox, TInt aBaselineOffset, 
			TBool aUp, DirectGdi::TTextAlign aVerticalAlignment, TInt aMargin)
	{
	iGc->DrawTextVertical(aText, reinterpret_cast<const CGraphicsContext::TTextParameters*>(aParam), aBox, aBaselineOffset, aUp,
			static_cast<CGraphicsContext::TTextAlign>(aVerticalAlignment), aMargin);
	}

void CTBitGdiContext::DrawTextVertical(const TDesC& aText, const DirectGdi::TTextParameters* aParam, const TRect& aBox, TInt aBaselineOffset, 
		TInt aTextWidth, TBool aUp, DirectGdi::TTextAlign aVerticalAlignment, TInt aMargin)
	{
	iGc->DrawTextVertical(aText, reinterpret_cast<const CGraphicsContext::TTextParameters*>(aParam), aBox, aBaselineOffset, aTextWidth, aUp,
			static_cast<CGraphicsContext::TTextAlign>(aVerticalAlignment), aMargin);
	}

void CTBitGdiContext::DrawResource(const TPoint& aPos, 
								const TDrawableSourceAndEquivRotatedBmps& aSource, 
								DirectGdi::TGraphicsRotation aRotation)
	{
	switch (aRotation)
		{
		case DirectGdi::EGraphicsRotation90:
			iGc->BitBlt(aPos, aSource.iBmpRot90);
			break;
			
		case DirectGdi::EGraphicsRotation180:
			iGc->BitBlt(aPos, aSource.iBmpRot180);
			break;
			
		case DirectGdi::EGraphicsRotation270:
			iGc->BitBlt(aPos, aSource.iBmpRot270);
			break;
			
		default: // DirectGdi::EGraphicsRotationNone		
			iGc->BitBlt(aPos, aSource.iBmpRotNone);
			break;
		}
	}

void CTBitGdiContext::DrawResource(const TRect& aDestRect, 
								const TDrawableSourceAndEquivRotatedBmps& aSource,
								DirectGdi::TGraphicsRotation aRotation)
	{
	switch (aRotation)
		{
		case DirectGdi::EGraphicsRotation90:
			iGc->DrawBitmap(aDestRect, aSource.iBmpRot90);
			break;
			
		case DirectGdi::EGraphicsRotation180:
			iGc->DrawBitmap(aDestRect, aSource.iBmpRot180);
			break;
			
		case DirectGdi::EGraphicsRotation270:
			iGc->DrawBitmap(aDestRect, aSource.iBmpRot270);
			break;
			
		default: // DirectGdi::EGraphicsRotationNone
			iGc->DrawBitmap(aDestRect, aSource.iBmpRotNone);
			break;
		}
	}

void CTBitGdiContext::DrawResource(const TRect& aDestRect, 
								const TDrawableSourceAndEquivRotatedBmps& aSource, 
								const TRect& aSrcRect,
								DirectGdi::TGraphicsRotation aRotation)
	{
	switch (aRotation)
		{
		case DirectGdi::EGraphicsRotation90:
			{
			// Adjust the src rect to take account of the rotated bitmap			
			TRect rect = aSrcRect;
			rect.iBr.iX = aSrcRect.iTl.iX + aSrcRect.Height();
			rect.iBr.iY = aSrcRect.iTl.iY + aSrcRect.Width();
			iGc->DrawBitmap(aDestRect, aSource.iBmpRot90, rect);
			break;
			}
			
		case DirectGdi::EGraphicsRotation180:
			iGc->DrawBitmap(aDestRect, aSource.iBmpRot180, aSrcRect);
			break;
			
		case DirectGdi::EGraphicsRotation270:
			{
			// Adjust the src rect to take account of the rotated bitmap			
			TRect rect = aSrcRect;
			rect.iBr.iX = aSrcRect.iTl.iX + aSrcRect.Height();
			rect.iBr.iY = aSrcRect.iTl.iY + aSrcRect.Width();
			iGc->DrawBitmap(aDestRect, aSource.iBmpRot270, rect);
			break;
			}
			
		default: // DirectGdi::EGraphicsRotationNone
			iGc->DrawBitmap(aDestRect, aSource.iBmpRotNone, aSrcRect);
			break;
		}
	}

void CTBitGdiContext::DrawResource(const TRect& aDestRect, 
								const TDrawableSourceAndEquivRotatedBmps& aSource, 
								const TDesC8& /*aParam*/)
	{			
	// NB we only support drawing drawable sources as images currently
	iGc->DrawBitmap(aDestRect, aSource.iBmpRotNone);		
	}

TInt CTBitGdiContext::GetInterface(TUid /*aInterfaceId*/, TAny*& aInterface)
	{
	aInterface = NULL;
	return KErrNotSupported;	
	}

void CTBitGdiContext::CopySettings(const CTContextBase& aGc)
	{
	CTBitGdiContext* gc = (CTBitGdiContext*)&aGc;
	iGc->CopySettings(*(gc->iGc));
	}
