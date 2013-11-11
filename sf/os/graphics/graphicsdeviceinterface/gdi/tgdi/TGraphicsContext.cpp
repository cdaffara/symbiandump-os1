// Copyright (c) 2002-2009 Nokia Corporation and/or its subsidiary(-ies).
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
 @internalComponent Internal Symbian test code
*/

#include "TTYPES.H"
#include "TGraphicsContext.h"



// Utility functions to show contents of test data using test.Printf

extern void PrintTestData (const TDesC& aTitle , const TDesC16& aDataBuffer);

extern void PrintTestData(const TDesC& aTitle, const TText16* aDataBuffer, const TInt aSize);




_LIT(KTestFontName, "Non Functional Test Font");


CTestPalette2::~CTestPalette2()
	{
	iArray = 0; //avoid attempting to deallocate iArray in ~CPalette
	iNumEntries = 0;
	}

//
//
// CTestGraphicsDevice
//
//
CTestGraphicsDevice* CTestGraphicsDevice::NewL(TSize aSizeInPixels)
	{
	CTestGraphicsDevice* theDevice = new (ELeave) CTestGraphicsDevice(aSizeInPixels);
	CleanupStack::PushL(theDevice);
	theDevice->iPalette = new (ELeave) CTestPalette2;
	theDevice->iPalette->SetEntry(0, KRgbBlack);
	theDevice->iPalette->SetEntry(1, KRgbWhite);
	theDevice->iPalette->SetEntry(2, KRgbMagenta);
	theDevice->iPalette->SetEntry(3, KRgbCyan);
	
	CleanupStack::Pop(theDevice);
	return theDevice;
	}

CTestGraphicsDevice::CTestGraphicsDevice(TSize aSizeInPixels)
	: iSize(aSizeInPixels),
	iHorizontalTwipsToPixels(40),
	iVerticalTwipsToPixels(40)
	{
	}
CTestGraphicsDevice::~CTestGraphicsDevice()
	{
	delete iPalette;
	}

void CTestGraphicsDevice::SetHorizontalTwipsToPixels(TInt aTwipsToPixels)
	{
	iHorizontalTwipsToPixels = aTwipsToPixels;
	}

void CTestGraphicsDevice::SetVerticalTwipsToPixels(TInt aTwipsToPixels)
	{
	iVerticalTwipsToPixels = aTwipsToPixels;
	}

TDisplayMode CTestGraphicsDevice::DisplayMode() const
	{
	return EColor16M;
	}

TSize CTestGraphicsDevice::SizeInPixels() const
	{
	return iSize;
	}

TSize CTestGraphicsDevice::SizeInTwips() const
	{
	return TSize(iSize.iWidth * iHorizontalTwipsToPixels,
		iSize.iHeight * iVerticalTwipsToPixels);
	}

TInt CTestGraphicsDevice::CreateContext(CGraphicsContext*& aGC)
	{
	CGraphicsContext* r = new CTestGraphicsContext(this);
	if (!r)
		return KErrNoMemory;
	aGC = r;
	return KErrNone;
	}

TInt CTestGraphicsDevice::NumTypefaces() const
	{
	return 1;
	}

void CTestGraphicsDevice::TypefaceSupport(TTypefaceSupport& aTypefaceSupport, TInt aTypefaceIndex) const
	{
	// The only font we have at the moment is 10 pixels * 12 pixels for every character
	__ASSERT_ALWAYS(aTypefaceIndex == 0,
		CTestGraphicsContext::Panic(CTestGraphicsContext::ETypefaceIndexOutOfRange));
	aTypefaceSupport.iIsScalable = EFalse;
	aTypefaceSupport.iMaxHeightInTwips = iVerticalTwipsToPixels * 12;
	aTypefaceSupport.iMinHeightInTwips = iVerticalTwipsToPixels * 10;
	aTypefaceSupport.iNumHeights = 1;
	aTypefaceSupport.iTypeface.iName = KTestFontName;
	aTypefaceSupport.iTypeface.SetIsProportional(ETrue); // a bit of a lie
	aTypefaceSupport.iTypeface.SetIsSerif(EFalse);
	aTypefaceSupport.iTypeface.SetIsSymbol(EFalse);
	}

TInt CTestGraphicsDevice::FontHeightInTwips(TInt aTypefaceIndex, TInt aHeightIndex) const
	{
	// The only font we have at the moment is 10 pixels * 12 pixels for every character
	__ASSERT_ALWAYS(aTypefaceIndex == 0,
		CTestGraphicsContext::Panic(CTestGraphicsContext::ETypefaceIndexOutOfRange));
	return iVerticalTwipsToPixels * FontHeightInPixels(aTypefaceIndex, aHeightIndex);
	}

void CTestGraphicsDevice::PaletteAttributes(TBool& aModifiable, TInt& aNumEntries) const
	{
	aModifiable = ETrue;
	aNumEntries = 4;
	}

void CTestGraphicsDevice::SetPalette(CPalette* aPalette)
	{
	for (TInt i = 0; i != CTestPalette2::KNumEntries; ++i)
		{
		TRgb col = aPalette->GetEntry(i);
		iPalette -> SetEntry(i, col);
		}
	}

TInt CTestGraphicsDevice::GetPalette(CPalette*& aPalette) const
	{
	aPalette =  const_cast<CTestPalette2*>(iPalette);
	return KErrNone;
	}

void CTestGraphicsDevice::GetPixel(TRgb& aColor, const TPoint&) const
	{
	aColor = KRgbWhite;
	}

void CTestGraphicsDevice::GetScanLine(TDes8&, const TPoint&, TInt, TDisplayMode) const
	{
	__ASSERT_DEBUG(0, CTestGraphicsContext::Panic(CTestGraphicsContext::EUnimplemented));
	}

TInt CTestGraphicsDevice::AddFile(const TDesC&, TInt&)
	{
	return KErrNotSupported;
	}

void CTestGraphicsDevice::RemoveFile(TInt)
	{
	}

TInt CTestGraphicsDevice::GetNearestFontInPixels(CFont*& aFont, const TFontSpec& aFontSpec)
	{
	return GetNearestFontToDesignHeightInPixels(aFont, aFontSpec);
	}

TInt CTestGraphicsDevice::GetNearestFontToDesignHeightInPixels(CFont*& aFont, const TFontSpec& /*aFontSpec*/)
	{
	CTestFont* font = new CTestFont();
	if (!font)
		return KErrNoMemory;
	aFont = font;
	return KErrNone;
	}

TInt CTestGraphicsDevice::GetNearestFontToMaxHeightInPixels(CFont*& aFont, const TFontSpec& /*aFontSpec*/, TInt /*aMaxHeight*/)
	{
	CTestFont* font = new CTestFont();
	if (!font)
		return KErrNoMemory;
	aFont = font;
	return KErrNone;
	}

TInt CTestGraphicsDevice::FontHeightInPixels(TInt, TInt) const
	{
	return 12;
	}

TInt CTestGraphicsDevice::HorizontalTwipsToPixels(TInt aTwips) const
	{
	return aTwips / iHorizontalTwipsToPixels;
	}

TInt CTestGraphicsDevice::VerticalTwipsToPixels(TInt aTwips) const
	{
	return aTwips / iVerticalTwipsToPixels;
	}

TInt CTestGraphicsDevice::HorizontalPixelsToTwips(TInt aPixels) const
	{
	return aPixels * iHorizontalTwipsToPixels;
	}

TInt CTestGraphicsDevice::VerticalPixelsToTwips(TInt aPixels) const
	{
	return aPixels * iVerticalTwipsToPixels;
	}

TInt CTestGraphicsDevice::GetNearestFontInTwips(CFont*& aFont, const TFontSpec& aFontSpec)
	{
	return GetNearestFontToDesignHeightInTwips(aFont, aFontSpec);
	}

TInt CTestGraphicsDevice::GetNearestFontToDesignHeightInTwips(CFont*& aFont, const TFontSpec& aFontSpec)
	{
	TFontSpec fontSpec = aFontSpec;
	fontSpec.iHeight = VerticalTwipsToPixels(fontSpec.iHeight);
	return GetNearestFontToDesignHeightInPixels(aFont, fontSpec);
	}

TInt CTestGraphicsDevice::GetNearestFontToMaxHeightInTwips(CFont*& aFont, const TFontSpec& aFontSpec, TInt aMaxHeight)
	{
	TFontSpec fontSpec = aFontSpec;
	fontSpec.iHeight = VerticalTwipsToPixels(fontSpec.iHeight);
	return GetNearestFontToMaxHeightInPixels(aFont, fontSpec, VerticalTwipsToPixels(aMaxHeight));
	}

void CTestGraphicsDevice::ReleaseFont(CFont* aFont)
	{
	__ASSERT_ALWAYS(aFont->TypeUid() == TUid::Uid(12345),
		CTestGraphicsContext::Panic(CTestGraphicsContext::EUnknownFont));
	delete static_cast<CTestFont*>(aFont);
	}

//
//
// CTestGraphicsContext
//
//
void CTestGraphicsContext::Panic(TInt aReason)
	{
	User::Panic(_L("CTestGC"), aReason);
	}

CTestGraphicsContext::CTestGraphicsContext(CTestGraphicsDevice* aGd)
	: iGd(aGd), iDrawMode(EDrawModePEN), iPenSize(1,1)
	{
	}

CTestGraphicsContext::~CTestGraphicsContext()
	{
	iLineArray.Reset();
	}
void CTestGraphicsContext::AddRectToDrawnArea(const TRect& aRect)
	{
	if (iDrawnArea.IsEmpty())
		iDrawnArea = aRect;
	else
		iDrawnArea.BoundingRect(aRect);
	// only one condition at the moment
	if (iDrawMode == EDrawModeXOR)
		{
		if (iAreaDrawnWithCondition.IsEmpty())
			iAreaDrawnWithCondition = aRect;
		else
			iAreaDrawnWithCondition.BoundingRect(aRect);
		}
	}

void CTestGraphicsContext::AddPointToDrawnArea(const TPoint& aPoint)
	{
	AddRectToDrawnArea(TRect(aPoint, iPenSize));
	}

CGraphicsDevice* CTestGraphicsContext::Device() const
	{
	return iGd;
	}

void CTestGraphicsContext::SetOrigin(const TPoint& aPos)
	{
	iOrigin = aPos;
	}

void CTestGraphicsContext::SetDrawMode(TDrawMode aDrawingMode)
	{
	iDrawMode = aDrawingMode;
	}

void CTestGraphicsContext::SetClippingRect(const TRect& /*aRect*/)
	{
	}

void CTestGraphicsContext::CancelClippingRect()
	{
	}

void CTestGraphicsContext::Reset()
	{
	iDrawMode = EDrawModePEN;
	iFont = 0;
	iPenSize.iWidth = 1;
	iPenSize.iHeight = 1;
	}

void CTestGraphicsContext::UseFont(const CFont* aFont)
	{
	iFont = aFont;
	}

void CTestGraphicsContext::DiscardFont()
	{
	iFont = 0;
	}

void CTestGraphicsContext::SetUnderlineStyle(TFontUnderline /*UnderlineStyle*/)
	{
	}

void CTestGraphicsContext::SetStrikethroughStyle(TFontStrikethrough /*aStrikethroughStyle*/)
	{
	}

void CTestGraphicsContext::SetWordJustification(TInt /*aExcessWidth*/,TInt /*aNumGaps*/)
	{
	}

void CTestGraphicsContext::SetCharJustification(TInt /*aExcessWidth*/,TInt /*aNumChars*/)
	{
	}

void CTestGraphicsContext::SetPenColor(const TRgb& aColor)
	{
	CPalette* palette;
	iGd->GetPalette(palette);
	iPenColorIndex = palette->NearestIndex(aColor);
	}

void CTestGraphicsContext::SetPenStyle(TPenStyle /*aPenStyle*/)
	{
	}

void CTestGraphicsContext::SetPenSize(const TSize& aSize)
	{
	iPenSize = aSize;
	}

void CTestGraphicsContext::SetBrushColor(const TRgb& /*aColor*/)
	{
	}

void CTestGraphicsContext::SetBrushStyle(TBrushStyle /*aBrushStyle*/)
	{
	}

void CTestGraphicsContext::SetBrushOrigin(const TPoint& /*aOrigin*/)
	{
	}

void CTestGraphicsContext::UseBrushPattern(const CFbsBitmap* /*aBitmap*/)
	{
	}

void CTestGraphicsContext::DiscardBrushPattern()
	{
	}

void CTestGraphicsContext::MoveTo(const TPoint& aPoint)
	{
	iCurrentPos = iOrigin + aPoint;
	}

void CTestGraphicsContext::MoveBy(const TPoint& aVector)
	{
	iCurrentPos += aVector;
	}

void CTestGraphicsContext::Plot(const TPoint& aPoint)
	{
	iCurrentPos = iOrigin + aPoint;
	AddPointToDrawnArea(iCurrentPos);
	}

void CTestGraphicsContext::DrawArc(const TRect& aRect,const TPoint& /*aStart*/,const TPoint& aEnd)
	{
	TRect r = aRect;
	r.Move(iOrigin);
	AddRectToDrawnArea(r);
	iCurrentPos = iOrigin + aEnd;
	}

void CTestGraphicsContext::DrawLine(const TPoint& aPoint1,const TPoint& aPoint2)
	{
	AddPointToDrawnArea(iOrigin + aPoint1);
	iCurrentPos = iOrigin + aPoint2;
	AddPointToDrawnArea(iCurrentPos);
	}

void CTestGraphicsContext::DrawLineTo(const TPoint& aPoint)
	{
	AddPointToDrawnArea(iCurrentPos);
	iCurrentPos = iOrigin + aPoint;
	AddPointToDrawnArea(iCurrentPos);
	}

void CTestGraphicsContext::DrawLineBy(const TPoint& aVector)
	{
	AddPointToDrawnArea(iCurrentPos);
	iCurrentPos += aVector;
	AddPointToDrawnArea(iCurrentPos);
	}

void CTestGraphicsContext::DrawPolyLine(const CArrayFix<TPoint>* aPointList)
	{
	TInt num = aPointList->Count();
	while (num--)
		{
		iCurrentPos = iOrigin + (*aPointList)[num - 1];
		AddPointToDrawnArea(iCurrentPos);
		}
	}

void CTestGraphicsContext::DrawPolyLine(const TPoint* aPointList,TInt aNumPoints)
	{
	while (aNumPoints--)
		{
		iCurrentPos = iOrigin + aPointList[aNumPoints - 1];
		AddPointToDrawnArea(iCurrentPos);
		}
	}

void CTestGraphicsContext::DrawPie(const TRect& aRect,
	const TPoint& /*aStart*/, const TPoint& aEnd)
	{
	TRect r = aRect;
	r.Move(iOrigin);
	AddRectToDrawnArea(r);
	iCurrentPos = iOrigin + aEnd;
	}

void CTestGraphicsContext::DrawEllipse(const TRect& aRect)
	{
	TRect r = aRect;
	r.Move(iOrigin);
	AddRectToDrawnArea(r);
	}

void CTestGraphicsContext::DrawRect(const TRect& aRect)
	{
	TRect r = aRect;
	r.Move(iOrigin);
	AddRectToDrawnArea(r);
	}

void CTestGraphicsContext::DrawRoundRect(const TRect& aRect,const TSize& aCornerSize)
	{
	TRect r = aRect;
	r.Move(iOrigin);
	r.Grow(aCornerSize);
	AddRectToDrawnArea(r);
	}

TInt CTestGraphicsContext::DrawPolygon(const CArrayFix<TPoint>* aPointList,TFillRule /*aFillRule*/)
	{
	TInt num = aPointList->Count();
	while (num--)
		{
		iCurrentPos = iOrigin + (*aPointList)[num - 1];
		AddPointToDrawnArea(iCurrentPos);
		}
	return KErrNone;
	}

TInt CTestGraphicsContext::DrawPolygon(const TPoint* aPointList,TInt aNumPoints,TFillRule /*aFillRule*/)
	{
	while (aNumPoints--)
		{
		iCurrentPos = iOrigin + aPointList[aNumPoints - 1];
		AddPointToDrawnArea(iCurrentPos);
		}
	return KErrNone;
	}

void CTestGraphicsContext::DrawBitmap(const TPoint& /*aTopLeft*/,const CFbsBitmap* /*aSource*/)
	{
	}

void CTestGraphicsContext::DrawBitmap(const TRect& /*aDestRect*/,const CFbsBitmap* /*aSource*/)
	{
	}

void CTestGraphicsContext::DrawBitmap(const TRect& /*aDestRect*/,const CFbsBitmap* /*aSource*/,const TRect& /*aSourceRect*/)
	{
	}

void CTestGraphicsContext::DrawBitmapMasked(const TRect& /*aDestRect*/,const CFbsBitmap* /*aBitmap*/,const TRect& /*aSourceRect*/,const CFbsBitmap* /*aMaskBitmap*/,TBool /*aInvertMask*/)
	{}
	
void CTestGraphicsContext::DrawBitmapMasked(const TRect& /*aDestRect*/,const CWsBitmap* /*aBitmap*/,const TRect& /*aSourceRect*/,const CWsBitmap* /*aMaskBitmap*/,TBool /*aInvertMask*/)
	{
	}

void CTestGraphicsContext::MapColors(const TRect &/*aRect*/,const TRgb */*aColors*/,TInt /*aNumPairs*/,TBool /*aMapForwards*/)
	{}
	
TInt CTestGraphicsContext::SetClippingRegion(const TRegion &/*aRegion*/)
	{
		return KErrNone;
	}	
	
void CTestGraphicsContext::CancelClippingRegion()
	{}
	
void CTestGraphicsContext::DrawTextVertical(const TDesC& /*aText*/,const TPoint& /*aPos*/,TBool /*aUp*/)
	{}
	
void CTestGraphicsContext::DrawTextVertical(const TDesC& /*aText*/,const TRect& /*aBox*/,TInt /*aBaselineOffset*/,TBool /*aUp*/,TTextAlign /*aVert*/,TInt /*aMargin*/)
	{}
	
/**
 *
 * DrawText - merely add line "drawn" to collection
 *
 */


void CTestGraphicsContext::DrawText(const TDesC& aText,const TPoint& aPosition)
	{

#ifdef PRINT_DRAWTEXT_LINES

	_LIT(KDrawTextFormat, "DrawText called for position %d,%d:");
	_LIT(KDrawTextTitle, "Text being drawn");
		test.Printf(KDrawTextFormat, aPosition.iX, aPosition.iY);
		PrintTestData(KDrawTextTitle, aText);

#endif /* PRINT_DRAWTEXT_LINES */
	
	TTestGCDisplayLine thisLine;
	thisLine.Set(aPosition, aText);
	iLineArray.Append(thisLine);

	}

void CTestGraphicsContext::DrawText(const TDesC& aText,const TRect& aBox,TInt aBaselineOffset,
	TTextAlign /*aAlignment*/, TInt aLeftMargin)
	{
	TPoint pos(aBox.iBr.iX + aLeftMargin, aBox.iTl.iY + aBaselineOffset);
	pos += iOrigin;
	DrawText(aText, pos);
	}

void CTestGraphicsContext::DrawText(const TDesC& aText, const TPoint& aPosition,
	const TDrawTextParam&)
	{
	DrawText(aText, aPosition);
	}

//
//
// CTestFont
//
//
TUid CTestFont::DoTypeUid() const
	{
	return TUid::Uid(12345);
	}

TInt CTestFont::DoHeightInPixels() const
	{
	return 12;
	}

TInt CTestFont::DoAscentInPixels() const
	{
	return 10;
	}

TInt CTestFont::DoCharWidthInPixels(TChar aChar) const
	{
	switch (aChar)
		{
	case 0x200C:
	case 0x200D:
	case 0x200E:
	case 0x200F:
	case 0x202A:
	case 0x202B:
	case 0x202C:
	case 0x202D:
	case 0x202E:
	case 0xFFFF:
		return 0;
	default:
		return 10;
		}
	}

TInt CTestFont::DoTextWidthInPixels(const TDesC& aText) const
	{
	TInt total = 0;
	const TText* p = aText.Ptr();
	const TText* end = p + aText.Length();
	for (;p != end; ++p)
		total += CharWidthInPixels(*p);
	return total;
	}

TInt CTestFont::DoBaselineOffsetInPixels() const
	{
	return 10;
	}

TInt CTestFont::DoTextCount(const TDesC& aText,TInt aWidthInPixels) const
	{
	TInt excess;
	return DoTextCount(aText, aWidthInPixels, excess);
	}

TInt CTestFont::DoTextCount(const TDesC& aText,TInt aWidthInPixels,TInt& aExcessWidthInPixels) const
	{
	aExcessWidthInPixels = aWidthInPixels;
	const TText* start = aText.Ptr();
	const TText* end = start + aText.Length();
	const TText* p = start;
	TInt width;
	while (p != end
		&& (width = CharWidthInPixels(*p)) <= aExcessWidthInPixels)
		{
		aExcessWidthInPixels -= width;
		++p;
		}
	return p - start;
	}

TInt CTestFont::DoMaxCharWidthInPixels() const
	{
	return 10;
	}

TInt CTestFont::DoMaxNormalCharWidthInPixels() const
	{
	return 10;
	}

TFontSpec CTestFont::DoFontSpecInTwips() const
	{
	return TFontSpec(KTestFontName, 12);
	}

CFont::TCharacterDataAvailability 
 CTestFont::DoGetCharacterData(TUint aCode, TOpenFontCharMetrics& aMetrics,
 	const TUint8*& aBitmap, TSize& aBitmapSize) const
   	{
 	TInt width;
   	switch (aCode)
   		{
   	case 0x001B:
 		// ESC character should cause this fault; no character data available.
   		return CFont::ENoCharacterData;
 	case 'W':
 		// We want 'W' to have side-bearings
 		CFont::DoGetCharacterData(aCode, aMetrics, aBitmap, aBitmapSize);
 		width = aMetrics.Width();
 		aMetrics.SetHorizBearingX(-1);
 		aMetrics.SetWidth(width + 2);
 		return CFont::ECharacterWidthOnly ;
 	case 0x0648:
 		// Arabic Waw-- has massive left side-bearing
 		CFont::DoGetCharacterData(aCode, aMetrics, aBitmap, aBitmapSize);
 		width = aMetrics.Width();
 		aMetrics.SetHorizBearingX(-5);
 		aMetrics.SetWidth(width + 5);
 		return CFont::ECharacterWidthOnly ;
	case 'X':
		// We want 'X' to have a left side-bearing only
		CFont::DoGetCharacterData(aCode, aMetrics, aBitmap, aBitmapSize);
		aMetrics.SetHorizBearingX(-1);
		return CFont::ECharacterWidthOnly ;
	case 0x05EA:
		// We want Hebrew Tav to have a +ve left side-bearing
		CFont::DoGetCharacterData(aCode, aMetrics, aBitmap, aBitmapSize);
		aMetrics.SetHorizBearingX(1);
		return CFont::ECharacterWidthOnly ;
	case 0x304B:
		// We want Hiragana Ka to have a top end-bearing
		CFont::DoGetCharacterData(aCode, aMetrics, aBitmap, aBitmapSize);
		aMetrics.SetVertBearingY(-1);
		return CFont::EAllCharacterData ;
	case 0x0020:
		CFont::DoGetCharacterData(aCode, aMetrics, aBitmap, aBitmapSize);
		aMetrics.SetWidth(0);
		aMetrics.SetHeight(0);
		aMetrics.SetVertAdvance(0);
		return CFont::ECharacterWidthOnly ;	
	default:
		return CFont::DoGetCharacterData(aCode, aMetrics, aBitmap, aBitmapSize);
		}
	}
