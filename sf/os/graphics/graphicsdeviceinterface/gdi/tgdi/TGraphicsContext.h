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
// Simple stubs for testing purposes
// 
//

/**
 @file
 @test
 @internalComponent Internal Symbian test code
*/

#ifndef __TGRAPHICSCONTEXT_H__
#define __TGRAPHICSCONTEXT_H__

#include <gdi.h>

/**
 *
 * Class to provide a dummy palette for test purposes
 *
 */


class CTestPalette2 : public CPalette
	{
public:
	enum { KNumEntries = 4 };
	CTestPalette2()
		{
		iArray = iRgbArray;
		iNumEntries = KNumEntries;
		}
	~CTestPalette2();
private:
	TRgb iRgbArray[KNumEntries];
	};

/**
 *
 * Class to provide a dummy graphics device for test purposes
 *
 */


class CTestGraphicsDevice : public CBitmapDevice
	{
public:
	static CTestGraphicsDevice* NewL(TSize aSizeInPixels);
	~CTestGraphicsDevice();
	void SetHorizontalTwipsToPixels(TInt aTwipsToPixels);
	void SetVerticalTwipsToPixels(TInt aTwipsToPixels);
	TDisplayMode DisplayMode() const;
	TSize SizeInPixels() const;
	TSize SizeInTwips() const;
	TInt CreateContext(CGraphicsContext*& aGC);
	TInt NumTypefaces() const;
	void TypefaceSupport(TTypefaceSupport& aTypefaceSupport,TInt aTypefaceIndex) const;
	TInt FontHeightInTwips(TInt aTypefaceIndex,TInt aHeightIndex) const;
	void PaletteAttributes(TBool& aModifiable,TInt& aNumEntries) const;
	void SetPalette(CPalette* aPalette);
	TInt GetPalette(CPalette*& aPalette) const;

	// from CBitmapFont
	void GetPixel(TRgb& aColor,const TPoint& aPixel) const;
	void GetScanLine(TDes8& aBuf,const TPoint& aStartPixel,TInt aLength,TDisplayMode aDispMode) const;
	TInt AddFile(const TDesC& aName,TInt& aId);
	void RemoveFile(TInt aId);
	TInt GetNearestFontInPixels(CFont*& aFont,const TFontSpec& aFontSpec);
	TInt GetNearestFontToDesignHeightInPixels(CFont*& aFont, const TFontSpec& aFontSpec);
	TInt GetNearestFontToMaxHeightInPixels(CFont*& aFont, const TFontSpec& aFontSpec, TInt aMaxHeight);
	TInt FontHeightInPixels(TInt aTypefaceIndex,TInt aHeightIndex) const;

	// from MGraphicsDeviceMap from CBitmapFont
	TInt HorizontalTwipsToPixels(TInt aTwips) const;
	TInt VerticalTwipsToPixels(TInt aTwips) const;
	TInt HorizontalPixelsToTwips(TInt aPixels) const;
	TInt VerticalPixelsToTwips(TInt aPixels) const;
	TInt GetNearestFontInTwips(CFont*& aFont,const TFontSpec& aFontSpec);
	TInt GetNearestFontToDesignHeightInTwips(CFont*& aFont, const TFontSpec& aFontSpec);
	TInt GetNearestFontToMaxHeightInTwips(CFont*& aFont, const TFontSpec& aFontSpec, TInt aMaxHeight);
	void ReleaseFont(CFont* aFont);
private:
	CTestGraphicsDevice(TSize aSizeInPixels);
private:
	TSize iSize;
	TInt iHorizontalTwipsToPixels;
	TInt iVerticalTwipsToPixels;
	CTestPalette2* iPalette;
	};


/**
 *
 * Class representing a line of displayed data
 *
 */


class TTestGCDisplayLine 
	{
public:
	TPoint iLinePos;
	TBuf16<80> iLineData;

	void Set(const TPoint& aLinePos, const TDesC16& aLineData)
		{
			iLineData = aLineData;
			iLinePos = aLinePos;
		}

	TPtrC16 LineData() const
		{
			return TPtrC16(iLineData.Ptr(), iLineData.Length());
		}

	const TPoint& Position() const
		{
			return iLinePos;
		}
	
	};

/**
 *
 * Class to provide a dummy graphics context for test purposes
 *
 */

class CTestGraphicsContext : public CGraphicsContext
	{
public:
	enum TPanic
		{
		EUnimplemented,
		ETypefaceIndexOutOfRange,
		EUnknownFont
		};
	static void Panic(TInt aReason);
	CTestGraphicsContext(CTestGraphicsDevice* aGd);
	~CTestGraphicsContext();
	CGraphicsDevice* Device() const;
	void SetOrigin(const TPoint& aPos);
	void SetDrawMode(TDrawMode aDrawingMode);
	void SetClippingRect(const TRect& aRect);
	void CancelClippingRect();
	void Reset();
	void UseFont(const CFont* aFont);
	void DiscardFont();
	void SetUnderlineStyle(TFontUnderline aUnderlineStyle);
	void SetStrikethroughStyle(TFontStrikethrough aStrikethroughStyle);
	void SetWordJustification(TInt aExcessWidth,TInt aNumGaps);
	void SetCharJustification(TInt aExcessWidth,TInt aNumChars);
	void SetPenColor(const TRgb& aColor);
	void SetPenStyle(TPenStyle aPenStyle);
	void SetPenSize(const TSize& aSize);
	void SetBrushColor(const TRgb& aColor);
	void SetBrushStyle(TBrushStyle aBrushStyle);
	void SetBrushOrigin(const TPoint& aOrigin);
	void UseBrushPattern(const CFbsBitmap* aBitmap);
	void DiscardBrushPattern();
	void MoveTo(const TPoint& aPoint);
	void MoveBy(const TPoint& aVector);
	void Plot(const TPoint& aPoint);
	void DrawArc(const TRect& aRect,const TPoint& aStart,const TPoint& aEnd);
	void DrawLine(const TPoint& aPoint1,const TPoint& aPoint2);
	void DrawLineTo(const TPoint& aPoint);
	void DrawLineBy(const TPoint& aVector);
	void DrawPolyLine(const CArrayFix<TPoint>* aPointList);
	void DrawPolyLine(const TPoint* aPointList,TInt aNumPoints);
	void DrawPie(const TRect& aRect,const TPoint& aStart,const TPoint& aEnd);
	void DrawEllipse(const TRect& aRect);
	void DrawRect(const TRect& aRect);
	void DrawRoundRect(const TRect& aRect,const TSize& aCornerSize);
	TInt DrawPolygon(const CArrayFix<TPoint>* aPointList,TFillRule aFillRule);
	TInt DrawPolygon(const TPoint* aPointList,TInt aNumPoints,TFillRule aFillRule);
	void DrawBitmap(const TPoint& aTopLeft,const CFbsBitmap* aSource);
	void DrawBitmap(const TRect& aDestRect,const CFbsBitmap* aSource);
	void DrawBitmap(const TRect& aDestRect,const CFbsBitmap* aSource,const TRect& aSourceRect);
	void DrawBitmapMasked(const TRect& aDestRect,const CFbsBitmap* aBitmap,const TRect& aSourceRect,const CFbsBitmap* aMaskBitmap,TBool aInvertMask);
	void DrawBitmapMasked(const TRect& aDestRect,const CWsBitmap* aBitmap,const TRect& aSourceRect,const CWsBitmap* aMaskBitmap,TBool aInvertMask);
	void DrawText(const TDesC& aText,const TPoint& aPosition);
	void DrawText(const TDesC& aText,const TRect& aBox,TInt aBaselineOffset,TTextAlign aAlignment,
		TInt aLeftMargin);
	void DrawText(const TDesC& aText, const TPoint& aPosition,
		const TDrawTextParam&);
		
	void MapColors(const TRect &aRect,const TRgb *aColors,TInt aNumPairs,TBool aMapForwards);
	TInt SetClippingRegion(const TRegion &aRegion);
	void CancelClippingRegion();
	void DrawTextVertical(const TDesC& aText,const TPoint& aPos,TBool aUp);
	void DrawTextVertical(const TDesC& aText,const TRect& aBox,TInt aBaselineOffset,TBool aUp,TTextAlign aVert=ELeft,TInt aMargin=0);

	void ClearDrawnArea()
		{
		iDrawnArea.iTl.iX = iDrawnArea.iBr.iX = iDrawnArea.iTl.iY = iDrawnArea.iBr.iY = 0;
		}
	TRect DrawnArea() const
		{
		return iDrawnArea;
		}
	void ClearAreaDrawnWithCondition()
		{
		iAreaDrawnWithCondition.iTl.iX = iAreaDrawnWithCondition.iBr.iX
			= iAreaDrawnWithCondition.iTl.iY = iAreaDrawnWithCondition.iBr.iY = 0;
		}
	const TRect& AreaDrawnWithCondition() const
		{
		return iAreaDrawnWithCondition;
		}
	void AddRectToDrawnArea(const TRect& aRect);
	void AddPointToDrawnArea(const TPoint& aPoint);

	const TTestGCDisplayLine& DisplayLine(TInt index)
		{
			return iLineArray[index];
		}
private:
	CTestGraphicsDevice* iGd;
	TPoint iOrigin;
	TDrawMode iDrawMode;
	const CFont* iFont;
	TSize iPenSize;
	TPoint iCurrentPos;
	TRect iDrawnArea;
	TRect iAreaDrawnWithCondition;
	TInt iPenColorIndex;
	RArray<TTestGCDisplayLine> iLineArray;
	};

/**
 *
 * Class to provide a dummy font for test purposes
 *
 */
class CTestFont : public CFont
	{
public:
	virtual TUid DoTypeUid() const;
	TInt DoHeightInPixels() const;
	TInt DoAscentInPixels() const;
	TInt DoCharWidthInPixels(TChar aChar) const;
	TInt DoTextWidthInPixels(const TDesC& aText) const;
	TInt DoBaselineOffsetInPixels() const;
	TInt DoTextCount(const TDesC& aText,TInt aWidthInPixels) const;
	TInt DoTextCount(const TDesC& aText,TInt aWidthInPixels,TInt& aExcessWidthInPixels) const;
	TInt DoMaxCharWidthInPixels() const;
	TInt DoMaxNormalCharWidthInPixels() const;
	TFontSpec DoFontSpecInTwips() const;
	TCharacterDataAvailability DoGetCharacterData(TUint aCode,
		TOpenFontCharMetrics& aMetrics,const TUint8*& aBitmap,TSize& aBitmapSize) const;
	};


#endif /* __TGRAPHICSCONTEXT_H__ */
