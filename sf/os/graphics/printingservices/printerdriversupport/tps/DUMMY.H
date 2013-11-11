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
//

#include <gdi.h>

class CDummyDevice : public	CGraphicsDevice
	{
protected:
	CDummyDevice();
public:
	IMPORT_C static CDummyDevice* NewL();
	IMPORT_C ~CDummyDevice();
	IMPORT_C TInt HorizontalTwipsToPixels(TInt aTwips) const;
	IMPORT_C TInt VerticalTwipsToPixels(TInt aTwips) const;
	IMPORT_C TInt HorizontalPixelsToTwips(TInt aPixels) const;
	IMPORT_C TInt VerticalPixelsToTwips(TInt aPixels) const;
	IMPORT_C TInt GetNearestFontInTwips(CFont*& aFont,const TFontSpec& aFontSpec);
	IMPORT_C TInt GetNearestFontToDesignHeightInTwips(CFont*& aFont,const TFontSpec& aFontSpec);
	IMPORT_C void ReleaseFont(CFont* aFont);
	IMPORT_C TDisplayMode DisplayMode() const;
	IMPORT_C TSize SizeInPixels() const;
	IMPORT_C TSize SizeInTwips() const;
    IMPORT_C TInt CreateContext(CGraphicsContext*& aGC);
    IMPORT_C TInt NumTypefaces() const;
    IMPORT_C void TypefaceSupport(TTypefaceSupport& aTypefaceSupport,TInt aTypefaceIndex) const;
	IMPORT_C TInt FontHeightInTwips(TInt aTypefaceIndex,TInt aHeightIndex) const;
	IMPORT_C void PaletteAttributes(TBool& aModifiable,TInt& aNumEntries) const;
	IMPORT_C void SetPalette(CPalette* aPalette);
	IMPORT_C TInt GetPalette(CPalette*& aPalette) const;
public:
	CConsoleBase* iConsole;
	};

class CDummyGc : public	CGraphicsContext
	{
public:
	IMPORT_C CDummyGc(CDummyDevice* aDevice);
	IMPORT_C ~CDummyGc();
	IMPORT_C CGraphicsDevice* Device() const;
	IMPORT_C void SetOrigin(const TPoint& aPos=TPoint(0,0));
	IMPORT_C void SetDrawMode(TDrawMode aDrawingMode);
	IMPORT_C void SetClippingRect(const TRect& aRect);
	IMPORT_C void CancelClippingRect();
	IMPORT_C void Reset();

	IMPORT_C void UseFont(const CFont* aFont);
	IMPORT_C void DiscardFont();
	IMPORT_C void SetUnderlineStyle(TFontUnderline aUnderlineStyle);
	IMPORT_C void SetStrikethroughStyle(TFontStrikethrough aStrikethroughStyle);
	IMPORT_C void SetWordJustification(TInt aExcessWidth,TInt aNumGaps);
	IMPORT_C void SetCharJustification(TInt aExcessWidth,TInt aNumChars);

	IMPORT_C void SetPenColor(const TRgb& aColor);
	IMPORT_C void SetPenStyle(TPenStyle aPenStyle);
	IMPORT_C void SetPenSize(const TSize& aSize);

	IMPORT_C void SetBrushColor(const TRgb& aColor);
	IMPORT_C void SetBrushStyle(TBrushStyle aBrushStyle);
	IMPORT_C void SetBrushOrigin(const TPoint& aOrigin);
	IMPORT_C void UseBrushPattern(const CFbsBitmap* aBitmap);
	IMPORT_C void DiscardBrushPattern();
	IMPORT_C void MoveTo(const TPoint& aPoint);
	IMPORT_C void MoveBy(const TPoint& aVector);
	IMPORT_C void Plot(const TPoint& aPoint);
	IMPORT_C void DrawArc(const TRect& aRect,const TPoint& aStart,const TPoint& aEnd);
	IMPORT_C void DrawLine(const TPoint& aPoint1,const TPoint& aPoint2);
	IMPORT_C void DrawLineTo(const TPoint& aPoint);
	IMPORT_C void DrawLineBy(const TPoint& aVector);
	IMPORT_C void DrawPolyLine(const CArrayFix<TPoint>* aPointList);
	IMPORT_C void DrawPolyLine(const TPoint* aPointList,TInt aNumPoints);
	IMPORT_C void DrawPie(const TRect& aRect,const TPoint& aStart,const TPoint& aEnd);
	IMPORT_C void DrawEllipse(const TRect& aRect);
	IMPORT_C void DrawRect(const TRect& aRect);
	IMPORT_C void DrawRoundRect(const TRect& aRect,const TSize& aCornerSize);
	IMPORT_C TInt DrawPolygon(const CArrayFix<TPoint>* aPointList,TFillRule aFillRule=EAlternate);
	IMPORT_C TInt DrawPolygon(const TPoint* aPointList,TInt aNumPoints,TFillRule aFillRule=EAlternate);
	IMPORT_C void DrawBitmap(const TPoint& aTopLeft,const CFbsBitmap* aSource);
	IMPORT_C void DrawBitmap(const TRect& aDestRect,const CFbsBitmap* aSource);
	IMPORT_C void DrawBitmap(const TRect& aDestRect,const CFbsBitmap* aSource,const TRect& aSourceRect);
	IMPORT_C void DrawBitmapMasked(const TRect& aDestRect,const CFbsBitmap* aBitmap,const TRect& aSourceRect,const CFbsBitmap* aMaskBitmap,TBool aInvertMask);
	IMPORT_C void DrawBitmapMasked(const TRect& aDestRect,const CWsBitmap* aBitmap,const TRect& aSourceRect,const CWsBitmap* aMaskBitmap,TBool aInvertMask);
	IMPORT_C void DrawText(const TDesC& aString,const TPoint& aPosition);
	IMPORT_C void DrawText(const TDesC& aString,const TRect& aBox,TInt aBaselineOffset,TTextAlign aHoriz=ELeft,TInt aLeftMrg=0);

protected:
	CDummyDevice* iDevice;
	};

