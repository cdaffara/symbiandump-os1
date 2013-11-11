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
#include <graphics/directgdicontext.h>
#include <graphics/directgdidriver.h>
#include <e32base.h>

CTestDirectGdiContext::CTestDirectGdiContext()
	{

	}

CTestDirectGdiContext* CTestDirectGdiContext::NewL()
	{
	CTestDirectGdiContext* self = new(ELeave) CTestDirectGdiContext;
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop();
	return self;
	}

void CTestDirectGdiContext::ConstructL()
	{
	iGdiDriver = CDirectGdiDriver::Static();
	if (iGdiDriver==NULL)
		User::Leave(KErrNotFound);
	TInt result = iGdiDriver->Open();
	if (result != KErrNone)
		User::Leave(result);

	iGc = CDirectGdiContext::NewL(*iGdiDriver);
	if (iGc==NULL)
		User::Leave(KErrNotFound);
	}

CTestDirectGdiContext::~CTestDirectGdiContext()
	{
	delete iGc;
	if(iGdiDriver)
		{
		iGdiDriver->Close();
		}	
	}

void CTestDirectGdiContext::SetOrigin(const TPoint& aOrigin)
	{
	iGc->SetOrigin(aOrigin);
	}

void CTestDirectGdiContext::SetClippingRegion(const TRegion& aRegion)
	{
	iGc->SetClippingRegion(aRegion);
	}

void CTestDirectGdiContext::ResetClippingRegion()
	{
	iGc->ResetClippingRegion();
	}

void CTestDirectGdiContext::SetDrawMode(DirectGdi::TDrawMode aMode)
	{
	iGc->SetDrawMode(aMode);
	}

void CTestDirectGdiContext::SetPenColor(const TRgb& aColor)
	{
	iGc->SetPenColor(aColor);
	}

void CTestDirectGdiContext::SetPenStyle(DirectGdi::TPenStyle aStyle)
	{
	iGc->SetPenStyle(aStyle);
	}

void CTestDirectGdiContext::SetPenSize(const TSize& aSize)
	{
	iGc->SetPenSize(aSize);
	}

void CTestDirectGdiContext::SetTextShadowColor(const TRgb& aColor)
	{
	iGc->SetTextShadowColor(aColor);
	}

void CTestDirectGdiContext::SetBrushColor(const TRgb& aColor)
	{
	iGc->SetBrushColor(aColor);
	}

void CTestDirectGdiContext::SetBrushStyle(DirectGdi::TBrushStyle aStyle)
	{
	iGc->SetBrushStyle(aStyle);
	}

void CTestDirectGdiContext::SetBrushOrigin(const TPoint& aOrigin)
	{
	iGc->SetBrushOrigin(aOrigin);
	}

void CTestDirectGdiContext::SetBrushPattern(const CFbsBitmap& aPattern)
	{
	iGc->SetBrushPattern(aPattern);
	}

void CTestDirectGdiContext::SetBrushPattern(TInt aHandle)
	{
	iGc->SetBrushPattern(aHandle);
	}

void CTestDirectGdiContext::ResetBrushPattern()
	{
	iGc->ResetBrushPattern();
	}

void CTestDirectGdiContext::SetFont(const CFont* aFont)
	{
	iGc->SetFont(aFont);
	}

void CTestDirectGdiContext::ResetFont()
	{
	iGc->ResetFont();
	}

void CTestDirectGdiContext::Reset()
	{
	iGc->Reset();
	}

TRgb CTestDirectGdiContext::BrushColor()
	{
	return iGc->BrushColor();
	}

TRgb CTestDirectGdiContext::PenColor()
	{
	return iGc->PenColor();
	}

TRgb CTestDirectGdiContext::TextShadowColor()
	{
	return iGc->TextShadowColor();
	}

void CTestDirectGdiContext::Clear(const TRect& aRect)
	{
	iGc->Clear(aRect);
	}

void CTestDirectGdiContext::Clear()
	{
	iGc->Clear();
	}

void CTestDirectGdiContext::MoveTo(const TPoint& aPoint)
	{
	iGc->MoveTo(aPoint);
	}

void CTestDirectGdiContext::MoveBy(const TPoint& aVector)
	{
	iGc->MoveBy(aVector);
	}

void CTestDirectGdiContext::Plot(const TPoint& aPoint)
	{
	iGc->Plot(aPoint);
	}

void CTestDirectGdiContext::DrawLine(const TPoint& aStart, const TPoint& aEnd)
	{
	iGc->DrawLine(aStart, aEnd);
	}

void CTestDirectGdiContext::DrawLineTo(const TPoint& aPoint)
	{
	iGc->DrawLineTo(aPoint);
	}

void CTestDirectGdiContext::DrawLineBy(const TPoint& aVector)
	{
	iGc->DrawLineBy(aVector);
	}

void CTestDirectGdiContext::DrawRect(const TRect& aRect)
	{
	iGc->DrawRect(aRect);
	}

void CTestDirectGdiContext::DrawRoundRect(const TRect& aRect,
		const TSize& aCornerSize)
	{
	iGc->DrawRoundRect(aRect, aCornerSize);
	}

void CTestDirectGdiContext::DrawPolyLine(const CArrayFix<TPoint>& aPointList)
	{
	TArray<TPoint> array = aPointList.Array();
	iGc->DrawPolyLine(array);
	}

void CTestDirectGdiContext::DrawPolyLineNoEndPoint(const CArrayFix<TPoint>& aPointList)
	{
	TArray<TPoint> array = aPointList.Array();
	iGc->DrawPolyLineNoEndPoint(array);
	}

void CTestDirectGdiContext::DrawPolygon(const CArrayFix<TPoint>& aPoints,
		DirectGdi::TFillRule aRule)
	{
	TArray<TPoint> array = aPoints.Array();
	iGc->DrawPolygon(array, aRule);
	}

void CTestDirectGdiContext::DrawArc(const TRect& aRect, const TPoint& aStart,
		const TPoint& aEnd)
	{
	iGc->DrawArc(aRect, aStart, aEnd);
	}

void CTestDirectGdiContext::DrawPie(const TRect& aRect, const TPoint& aStart,
		const TPoint& aEnd)
	{
	iGc->DrawPie(aRect, aStart, aEnd);
	}

void CTestDirectGdiContext::DrawEllipse(const TRect& aRect)
	{
	iGc->DrawEllipse(aRect);
	}

void CTestDirectGdiContext::BitBlt(const TPoint& aPoint,
		const CFbsBitmap& aBitmap)
	{
	iGc->BitBlt(aPoint, aBitmap);
	}

void CTestDirectGdiContext::DrawBitmap(const TRect& aDestRect,
		const CFbsBitmap& aSource)
	{
	iGc->DrawBitmap(aDestRect, aSource);
	}

void CTestDirectGdiContext::BitBlt(const TPoint& aDestPos,
		const CFbsBitmap& aBitmap, const TRect& aSrcRect)
	{
	iGc->BitBlt(aDestPos, aBitmap, aSrcRect);
	}

void CTestDirectGdiContext::BitBltMasked(const TPoint& aDestPos,
		const CFbsBitmap& aBitmap, const TRect& aSrcRect,
		const CFbsBitmap& aMask, TBool aInvertMask)
	{
	iGc->BitBltMasked(aDestPos, aBitmap, aSrcRect, aMask, aInvertMask);
	}

void CTestDirectGdiContext::BitBltMasked(const TPoint& aDestPos, const CFbsBitmap& aBitmap, 
		const TRect& aSourceRect, const CFbsBitmap& aAlpha, 
		const TPoint& aAlphaPos)
	{
	iGc->BitBltMasked(aDestPos, aBitmap, aSourceRect, aAlpha, aAlphaPos);
	}

void CTestDirectGdiContext::DrawBitmap(const TRect& aDestRect,
		const CFbsBitmap& aBitmap, const TRect& aSrcRect)
	{
	iGc->DrawBitmap(aDestRect, aBitmap, aSrcRect);
	}

void CTestDirectGdiContext::DrawBitmapMasked(const TRect& aDestRect,
		const CFbsBitmap& aBitmap, const TRect& aSrcRect,
		const CFbsBitmap& aMask, TBool aInvertMask)
	{
	iGc->DrawBitmapMasked(aDestRect, aBitmap, aSrcRect, aMask, aInvertMask);
	}

void CTestDirectGdiContext::CopyRect(const TPoint& aOffset, const TRect& aRect)
	{
	iGc->CopyRect(aOffset, aRect);
	}

TBool CTestDirectGdiContext::HasBrushPattern() const
	{
	return iGc->HasBrushPattern();
	}

TBool CTestDirectGdiContext::HasFont() const
	{
	return iGc->HasFont();
	}

void CTestDirectGdiContext::ExternalizeL(RWriteStream& aWriteStream)
	{
	iGc->ExternalizeL(aWriteStream);
	}

void CTestDirectGdiContext::InternalizeL(RReadStream& aReadStream)
	{
	iGc->InternalizeL(aReadStream);
	}

void CTestDirectGdiContext::SetCharJustification(TInt aExcessWidth,
		TInt aNumGaps)
	{
	iGc->SetCharJustification(aExcessWidth, aNumGaps);
	}

void CTestDirectGdiContext::SetWordJustification(TInt aExcessWidth,
		TInt aNumChars)
	{
	iGc->SetWordJustification(aExcessWidth, aNumChars);
	}

void CTestDirectGdiContext::SetUnderlineStyle(DirectGdi::TFontUnderline aUnderlineStyle)
	{
	iGc->SetUnderlineStyle((aUnderlineStyle));
	}

void CTestDirectGdiContext::SetStrikethroughStyle(DirectGdi::TFontStrikethrough aStrikethroughStyle)
	{
	iGc->SetStrikethroughStyle(aStrikethroughStyle);
	}

void CTestDirectGdiContext::UpdateJustification(const TDesC& aText, const DirectGdi::TTextParameters* aParam)
	{
	iGc->UpdateJustification(aText, aParam);
	}

void CTestDirectGdiContext::UpdateJustificationVertical(const TDesC& aText, const DirectGdi::TTextParameters* aParam, TBool aUp)
	{
	iGc->UpdateJustificationVertical(aText, aParam, aUp);
	}

void CTestDirectGdiContext::SetFontNoDuplicate(const CFont* aFont)
	{
	iGc->SetFontNoDuplicate((CDirectGdiFont*)aFont);
	}

TInt CTestDirectGdiContext::Activate(RDirectGdiImageTarget& aTarget)
	{
	return iGc->Activate(aTarget);
	}


void CTestDirectGdiContext::NoJustifyAutoUpdate()
	{
	iGc->NoJustifyAutoUpdate();
	}

void CTestDirectGdiContext::SetJustifyAutoUpdate()
	{
	iGc->SetJustifyAutoUpdate();
	}

TInt CTestDirectGdiContext::GetError()
	{
	return iGdiDriver->GetError();
	}

// text drawing
void CTestDirectGdiContext::DrawText(const TDesC& aText, const DirectGdi::TTextParameters* aParam)
	{
	iGc->DrawText(aText, aParam);
	}

void CTestDirectGdiContext::DrawText(const TDesC& aText, const DirectGdi::TTextParameters* aParam, const TPoint& aPosition)
	{
	iGc->DrawText(aText, aParam, aPosition);
	}

void CTestDirectGdiContext::DrawText(const TDesC& aText, const DirectGdi::TTextParameters* aParam, const TRect& aBox)
	{
	iGc->DrawText(aText, aParam, aBox);
	}

void CTestDirectGdiContext::DrawText(const TDesC& aText, const DirectGdi::TTextParameters* aParam, const TRect& aBox, TInt aBaselineOffset, 
			DirectGdi::TTextAlign aAlignment, TInt aMargin)
	{
	iGc->DrawText(aText, aParam, aBox, aBaselineOffset, aAlignment, aMargin);
	}

void CTestDirectGdiContext::DrawTextVertical(const TDesC& aText, const DirectGdi::TTextParameters* aParam, TBool aUp)
	{
	iGc->DrawTextVertical(aText, aParam, aUp);
	}

void CTestDirectGdiContext::DrawTextVertical(const TDesC& aText, const DirectGdi::TTextParameters* aParam, const TPoint& aPosition, TBool aUp)
	{
	iGc->DrawTextVertical(aText, aParam, aPosition, aUp);
	}

void CTestDirectGdiContext::DrawTextVertical(const TDesC& aText, const DirectGdi::TTextParameters* aParam, const TRect& aBox, TBool aUp)
	{
	iGc->DrawTextVertical(aText, aParam, aBox, aUp);
	}

void CTestDirectGdiContext::DrawTextVertical(const TDesC& aText, const DirectGdi::TTextParameters* aParam, const TRect& aBox, TInt aBaselineOffset, 
			TBool aUp, DirectGdi::TTextAlign aVerticalAlignment, TInt aMargin)
	{
	iGc->DrawTextVertical(aText, aParam, aBox, aBaselineOffset, aUp, aVerticalAlignment, aMargin);
	}

void CTestDirectGdiContext::DrawTextVertical(const TDesC& aText, const DirectGdi::TTextParameters* aParam, const TRect& aBox, TInt aBaselineOffset, 
		TInt aTextWidth, TBool aUp, DirectGdi::TTextAlign aVerticalAlignment, TInt aMargin)
	{
	iGc->DrawTextVertical(aText, aParam, aBox, aBaselineOffset, aTextWidth, aUp, aVerticalAlignment, aMargin);
	}

void CTestDirectGdiContext::DrawResource(const TPoint& aPos, 
										const TDrawableSourceAndEquivRotatedBmps& aSource, 
										DirectGdi::TGraphicsRotation aRotation)
	{
	iGc->DrawResource(aPos, *(aSource.iDrawableSrc), aRotation);
	}

void CTestDirectGdiContext::DrawResource(const TRect& aDestRect, 
										const TDrawableSourceAndEquivRotatedBmps& aSource,
										DirectGdi::TGraphicsRotation aRotation)
	{
	iGc->DrawResource(aDestRect, *(aSource.iDrawableSrc), aRotation);
	}

void CTestDirectGdiContext::DrawResource(const TRect& aDestRect, 
										const TDrawableSourceAndEquivRotatedBmps& aSource, 
										const TRect& aSrcRect,
										DirectGdi::TGraphicsRotation aRotation)
	{
	iGc->DrawResource(aDestRect, *(aSource.iDrawableSrc), aSrcRect, aRotation);
	}

void CTestDirectGdiContext::DrawResource(const TRect& aDestRect, 
										const TDrawableSourceAndEquivRotatedBmps& aSource, 
										const TDesC8& aParam)
	{
	iGc->DrawResource(aDestRect, *(aSource.iDrawableSrc), aParam);
	}

TInt CTestDirectGdiContext::GetInterface(TUid aInterfaceId, TAny*& aInterface)
	{
	return iGc->GetInterface(aInterfaceId, aInterface);
	}

void CTestDirectGdiContext::CopySettings(const CTContextBase& aGc)
	{
	CTestDirectGdiContext* gc = (CTestDirectGdiContext*)&aGc;
	iGc->CopySettings(*(gc->iGc));
	}
