// Copyright (c) 1997-2010 Nokia Corporation and/or its subsidiary(-ies).
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
// Font and text classes used by printer drivers.

#ifndef PDRSTOREFONT_H
#define PDRSTOREFONT_H

/**
@file
@internalAll - Internal to Symbian
@released
 */

#include <s32mem.h>
#include <gdi.h>


class CFbsBitGc;
class TPdrResource;
class CPdrDevice;
class CFontInfo;
class CPdrTranslates;
class CInfoFont;
class CTypefaceFonts;
class TTextFormat;
class CPdrControl;
class CFbsFont;


class CPdrResources : public CBase
	{
public:
	IMPORT_C CPdrResources();
	IMPORT_C void RestoreL(CStreamStore& aStore, TStreamId aStreamId);
	IMPORT_C ~CPdrResources();
	IMPORT_C TPtrC8 ResourceString(TInt anId) const;
private:
	TInt iNumResources;
	
	TPdrResource* iResourceList;
	};


class TTypefaceFontsEntry	
	{
public:
	IMPORT_C void InternalizeL(RReadStream& aStream);
public:
	
	TStreamId iStreamId;
	
	TBool iNotInPortrait;
	
	TBool iNotInLandscape;
	};


class CPdrModelInfo : public CBase	
	{
public:
	IMPORT_C CPdrModelInfo();
	IMPORT_C ~CPdrModelInfo();
	IMPORT_C void InternalizeL(RReadStream& aStream);
public:
	
	TInt iFlags;
	
	TInt iKPixelWidthInTwips;
	
	TInt iKPixelHeightInTwips;
	
	TPoint iPortraitOffsetInPixels;
	
	TPoint iLandscapeOffsetInPixels;
	
	TMargins iMinMarginsInPixels;
	
	TDisplayMode iDisplayMode;
	
	TInt iNumTypefaceFonts;
	
	TTypefaceFontsEntry* iTypefaceFontsEntryList;
	
	TStreamId iResourcesStreamId;
	
	TStreamId iSpareStreamId; 
	};


/** 
@internalTechnology
 */
class CPdrTypefaceStore : public CTypefaceStore
	{
public:
	IMPORT_C static CPdrTypefaceStore* NewL(CStreamStore& aStore, TInt aNumTypefacesFonts, TTypefaceFontsEntry* aTypefaceFontsEntryList, TPageSpec::TPageOrientation aPageOrientation, TInt aKPixelHeightInTwips, CPdrDevice* aPdrDevice);
	IMPORT_C ~CPdrTypefaceStore();
	// CTypefaceStore implementation
	IMPORT_C TInt GetNearestFontInTwips(CFont*& aFont, const TFontSpec& aFontSpec);
	IMPORT_C TInt GetNearestFontToDesignHeightInTwips(CFont*& aFont, const TFontSpec& aFontSpec);
	IMPORT_C TInt GetNearestFontToMaxHeightInTwips(CFont*& aFont, const TFontSpec& aFontSpec, TInt aMaxHeight);
	IMPORT_C TInt NumTypefaces() const;
	IMPORT_C TInt FontHeightInTwips(TInt aTypefaceIndex, TInt aHeightIndex) const;
	IMPORT_C void TypefaceSupport(TTypefaceSupport &aTypefaceSupport, TInt aTypefaceIndex) const;
	IMPORT_C void SetPageOrientation(TPageSpec::TPageOrientation aPageOrientation);
private:
	CPdrTypefaceStore(CStreamStore& aStore, TInt aKPixelHeightInTwips, CPdrDevice* aPdrDevice);
	void ConstructL(TInt aNumTypefacesFonts, TTypefaceFontsEntry* aTypefaceFontsEntryList, TPageSpec::TPageOrientation aPageOrientation);
	CFontInfo* FontInfoL(TStreamId aStreamId) const;
	CPdrTranslates* TranslatesL(TStreamId aStreamId) const;
	TInt GetNearestFontHeightIndex(TInt aTypefaceIndex, TInt aHeightInTwips) const;
	TFontStyle GetNearestFontStyle(TInt aTypefaceIndex, TInt aHeightIndex, const TFontStyle& aFontStyle) const;
	TBool IsFontLoaded(CFont*& aFont, const TFontSpec& aFontSpecInTwips, TInt aHeightInPixels) const;
	CInfoFont* NewFontL(TInt aBaselineOffsetInPixels, const TFontSpec& aFontSpecInTwips, TInt aFontInfoHeightInTwips, TInt aHeightInPixels, CPdrTranslates* aTranslates, const TDesC8& aCommandString, TStreamId aFontInfoStreamId);
	TInt VerticalTwipsToPixels(TInt aTwipsHeight) const;
public:
	CPdrDevice* iPdrDevice;
private:
	CStreamStore* iStore;
	TInt iKPixelHeightInTwips;
	CArrayPtr<CTypefaceFonts>* iTypefaceFontsList;
	CArrayPtr<CTypefaceFonts>* iPortraitTypefaceFontsList;
	CArrayPtr<CTypefaceFonts>* iLandscapeTypefaceFontsList;
	CArrayPtr<CTypefaceFonts>* iCurrentTypefaceFontsList;
	CArrayPtr<CPdrTranslates>* iTranslatesList;
	CArrayPtr<CFontInfo>* iFontInfoList;
	};

class CPageTextEntry : public CBase
	{
public:
	IMPORT_C CPageTextEntry(const TPoint& aDrawPos, TInt aHeightInPixels, TInt aTextWidthInPixels, HBufC8* aText, TTextFormat* aTextFormat);
	IMPORT_C ~CPageTextEntry();
	IMPORT_C TPoint TopTextPos();
public:
	
	TPoint iDrawPos;
	
	TInt iHeightInPixels;
	
	TInt iTextWidthInPixels;
	
	HBufC8* iText;
	
	TTextFormat* iTextFormat;
	};


class CPageText : public CBase
	{
private:
	CPageText();
	void ConstructL();
public:
	IMPORT_C static CPageText* NewL();
	IMPORT_C ~CPageText();
	IMPORT_C void Reset();
	IMPORT_C void AddEntryL(const TPoint& aPoint, const TFontUnderline aUnderlineStyle, const TFontStrikethrough aStrikethroughStyle, const TRgb& aColor, const CInfoFont* aFont, const TDesC& aString);
	IMPORT_C TInt NumEntries();
	IMPORT_C CPageTextEntry* operator [] (TInt anIndex);
	/** This function is internal only, and is not intended for use. */
	inline TInt MaxFontHeightInPixels() const {return iMaxFontHeightInPixels;}
private:
	
	TInt iMaxFontHeightInPixels;
	
	CArrayPtr<TTextFormat>* iTextFormatList;
	
	CArrayPtr<CPageTextEntry>* iPageTextEntryList;
	};


class CPdrGc : public CGraphicsContext	
	{
private:
	CPdrGc(CPdrDevice* aDevice);
	void ConstructL();
public:
	static CPdrGc* NewL(CPdrDevice* aDevice);
	IMPORT_C ~CPdrGc();

	IMPORT_C CGraphicsDevice* Device() const;
	IMPORT_C void SetOrigin(const TPoint& aPos = TPoint(0, 0));
	IMPORT_C void SetDrawMode(TDrawMode aDrawingMode);
	IMPORT_C void SetClippingRect(const TRect& aRect);
	IMPORT_C void CancelClippingRect();
	IMPORT_C void Reset();

	IMPORT_C void UseFont(const CFont* aFont);
	IMPORT_C void DiscardFont();

	IMPORT_C void SetUnderlineStyle(TFontUnderline aUnderlineStyle);
	IMPORT_C void SetStrikethroughStyle(TFontStrikethrough aStrikethroughStyle);
	IMPORT_C void SetWordJustification(TInt aExcessWidth, TInt aNumGaps);
	IMPORT_C void SetCharJustification(TInt aExcessWidth, TInt aNumChars);

	IMPORT_C void SetPenColor(const TRgb& aColor);
	IMPORT_C void SetPenStyle(TPenStyle aPenStyle);
	IMPORT_C void SetPenSize(const TSize& aSize = TSize(1, 1));

	IMPORT_C void SetBrushColor(const TRgb& aColor);
	IMPORT_C void SetBrushStyle(TBrushStyle aBrushStyle);
	IMPORT_C void SetBrushOrigin(const TPoint& aOrigin);
	IMPORT_C void UseBrushPattern(const CFbsBitmap* aBitmap);
	IMPORT_C void DiscardBrushPattern();

	IMPORT_C void MoveTo(const TPoint& aPoint);
	IMPORT_C void MoveBy(const TPoint& aVector);
	IMPORT_C void Plot(const TPoint& aPoint);

	IMPORT_C void DrawArc(const TRect& aRect, const TPoint& aStart, const TPoint& aEnd);
	IMPORT_C void DrawLine(const TPoint& aPoint1, const TPoint& aPoint2);
	IMPORT_C void DrawLineTo(const TPoint& aPoint);
	IMPORT_C void DrawLineBy(const TPoint& aVector);
	IMPORT_C void DrawPolyLine(const CArrayFix<TPoint>* aPointList);
	IMPORT_C void DrawPolyLine(const TPoint* aPointList, TInt aNumPoints);

	IMPORT_C void DrawPie(const TRect& aRect, const TPoint& aStart, const TPoint& aEnd);
	IMPORT_C void DrawEllipse(const TRect& aRect);
	IMPORT_C void DrawRect(const TRect& aRect);
	IMPORT_C void DrawRoundRect(const TRect& aRect, const TSize& aCornerSize);
	IMPORT_C TInt DrawPolygon(const CArrayFix<TPoint>* aPointList, TFillRule aFillRule = EAlternate);
	IMPORT_C TInt DrawPolygon(const TPoint* aPointList, TInt aNumPoints, TFillRule aFillRule = EAlternate);

	IMPORT_C void DrawBitmap(const TPoint& aTopLeft, const CFbsBitmap* aSource);
	IMPORT_C void DrawBitmap(const TRect& aDestRect, const CFbsBitmap* aSource);
	IMPORT_C void DrawBitmap(const TRect& aDestRect, const CFbsBitmap* aSource, const TRect& aSourceRect);
	IMPORT_C void DrawBitmapMasked(const TRect& aDestRect,const CFbsBitmap* aBitmap,const TRect& aSourceRect,const CFbsBitmap* aMaskBitmap,TBool aInvertMask);
	IMPORT_C void DrawBitmapMasked(const TRect& aDestRect,const CWsBitmap* aBitmap,const TRect& aSourceRect,const CWsBitmap* aMaskBitmap,TBool aInvertMask);

	IMPORT_C void DrawText(const TDesC& aString, const TPoint& aPosition);
	IMPORT_C void DrawTextL(const TDesC& aString, const TPoint& aPosition);
	IMPORT_C void DrawText(const TDesC& aString, const TRect& aBox, TInt aBaselineOffset, TTextAlign aHoriz = ELeft, TInt aLeftMrg = 0);
									
	IMPORT_C void MapColors(const TRect& aRect,const TRgb* aColors,TInt aNumPairs,TBool aMapForwards);
	IMPORT_C TInt SetClippingRegion(const TRegion &aRegion);
	IMPORT_C void CancelClippingRegion();
	IMPORT_C void DrawTextVertical(const TDesC& aText,const TPoint& aPos,TBool aUp);
	IMPORT_C void DrawTextVertical(const TDesC& aText,const TRect& aBox,TInt aBaselineOffset,TBool aUp,TTextAlign aVert=ELeft,TInt aMargin=0);								
	IMPORT_C TInt AlphaBlendBitmaps(const TPoint& aDestPt, const CFbsBitmap* aSrcBmp, const TRect& aSrcRect, const CFbsBitmap* aAlphaBmp, const TPoint& aAlphaPt);
	IMPORT_C TInt AlphaBlendBitmaps(const TPoint& aDestPt, const CWsBitmap* aSrcBmp,  const TRect& aSrcRect, const CWsBitmap*  aAlphaBmp, const TPoint& aAlphaPt);
	
private:
	CPdrControl* PdrControl() const;

private:
	
	CPdrDevice* iPdrDevice;
	
	CFbsBitGc* iBandedGc;
	
	CFbsFont* iFbsFont;
	
	CFont* iBandedFont;
	
	CInfoFont* iFont;
	
	TPoint iOrigin;
	
	TPoint iPosition;
	
	TFontUnderline iUnderlineStyle;
	
	TFontStrikethrough iStrikethroughStyle;
	
	TRect iClippingRect;  // Relative to absolute origin
	
	TInt iWordExcessWidthInPixels;
	
	TInt iNumGaps;
	
	TInt iCharExcessWidthInPixels;
	
	TInt iNumChars;
	
	TRgb iPenColor;
	
	TBool iPrintTextUsingBitmaps;
	};



#endif // PDRSTOREFONT_H
