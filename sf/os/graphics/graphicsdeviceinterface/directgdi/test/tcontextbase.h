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

/**
 @file
 @test
 @internalComponent - Internal Symbian test code 
*/

#ifndef TCONTEXTBASE_H
#define TCONTEXTBASE_H

#include <graphics/directgditypes.h>
#include <gdi.h>

// Forward declarations.
//
class CFbsBitGc;
class CDirectGdiContext;
class CFbsBitGcFont;
class CDirectGdiDriver;
class CFbsBitmapDevice;
class RDirectGdiDrawableSource;
class RDirectGdiImageTarget;

/**
Struct to allow the equivalent of an image source to be drawn using BitGdi when testing the
DrawResource methods. When an image source is created for testing, rotated bitmap versions
of it are also created to allow the tests to be drawn in BitGdi using DrawBitmap
*/
struct TDrawableSourceAndEquivRotatedBmps
	{
	TDrawableSourceAndEquivRotatedBmps() :
		iDrawableSrc(NULL),
		iBmpRotNone(NULL),
		iBmpRot90(NULL),
		iBmpRot180(NULL),
		iBmpRot270(NULL)
	{
	}
	
	RDirectGdiDrawableSource* iDrawableSrc;
	CFbsBitmap* iBmpRotNone;
	CFbsBitmap* iBmpRot90;
	CFbsBitmap* iBmpRot180;
	CFbsBitmap* iBmpRot270;
	};

/**
Base class for Contexts in Test.
*/
class CTContextBase : public CBase
	{
public:
	virtual void SetOrigin(const TPoint& aOrigin) = 0;
	virtual void SetClippingRegion(const TRegion& aRegion) = 0;
	virtual void ResetClippingRegion() = 0;
	virtual void SetDrawMode(DirectGdi::TDrawMode aMode) = 0;
	virtual void SetPenColor(const TRgb& aColor) = 0;
	virtual void SetPenStyle(DirectGdi::TPenStyle aStyle) = 0;
	virtual void SetPenSize(const TSize& aSize) = 0;
	virtual void SetTextShadowColor(const TRgb& aColor) = 0;
	virtual void SetBrushColor(const TRgb& aColor) = 0;
	virtual void SetBrushStyle(DirectGdi::TBrushStyle aStyle) = 0;
	virtual void SetBrushOrigin(const TPoint& aOrigin) = 0;
	virtual void SetBrushPattern(const CFbsBitmap& aPattern) = 0;
	virtual void SetBrushPattern(TInt aHandle) = 0;
	virtual void ResetBrushPattern() = 0;
	virtual void SetFont(const CFont* aFont) = 0;
	virtual void ResetFont() = 0;
	virtual void Reset() = 0;
	virtual TRgb BrushColor() = 0;
	virtual TRgb PenColor() = 0;
	virtual TRgb TextShadowColor() = 0;
	virtual void Clear(const TRect& aRect) = 0;
	virtual void Clear() = 0;
	virtual void MoveTo(const TPoint& aPoint) = 0;
	virtual void MoveBy(const TPoint& aVector) = 0;
	virtual void Plot(const TPoint& aPoint) = 0;
	virtual void DrawLine(const TPoint& aStart, const TPoint& aEnd) = 0;
	virtual void DrawLineTo(const TPoint& aPoint) = 0;
	virtual void DrawLineBy(const TPoint& aVector) = 0;
	virtual void DrawRect(const TRect& aRect) = 0;
	virtual void DrawRoundRect(const TRect& aRect, const TSize& aEllipse) = 0;
	virtual void DrawPolyLine(const CArrayFix<TPoint>& aPointList) = 0;
	virtual void DrawPolyLineNoEndPoint(const CArrayFix<TPoint>& aPointList) = 0;
	virtual void DrawPolygon(const CArrayFix<TPoint>& aPoints,
			DirectGdi::TFillRule aRule) = 0;
	virtual void DrawArc(const TRect& aRect, const TPoint& aStart,
			const TPoint& aEnd) = 0;
	virtual void DrawPie(const TRect& aRect, const TPoint& aStart,
			const TPoint& aEnd) = 0;
	virtual void DrawEllipse(const TRect& aRect) = 0;
	virtual void BitBlt(const TPoint& aPoint, const CFbsBitmap& aBitmap) = 0;
	virtual void BitBlt(const TPoint& aDestPos, const CFbsBitmap& aBitmap,
			const TRect& aSrcRect) = 0;
	virtual void BitBltMasked(const TPoint& aDestPos,
			const CFbsBitmap& aBitmap, const TRect& aSrcRect,
			const CFbsBitmap& aMask, TBool aInvertMask) = 0;
	virtual void BitBltMasked(const TPoint& aDestPos, const CFbsBitmap& aBitmap, 
			const TRect& aSourceRect, const CFbsBitmap& aAlpha, 
			const TPoint& aAlphaPos) = 0;
	virtual void
			DrawBitmap(const TRect& aDestRect, const CFbsBitmap& aSource) = 0;
	virtual void DrawBitmap(const TRect& aDestRect, const CFbsBitmap& aBitmap,
			const TRect& aSrcRect) = 0;
	virtual void DrawBitmapMasked(const TRect& aDestRect,
			const CFbsBitmap& aBitmap, const TRect& aSrcRect,
			const CFbsBitmap& aMask, TBool aInvertMask) = 0;
	virtual void CopyRect(const TPoint& aOffset, const TRect& aRect) = 0;
	virtual TBool HasBrushPattern() const = 0;
	virtual TBool HasFont() const = 0;
	virtual void ExternalizeL(RWriteStream& aWriteStream) = 0;
	virtual void InternalizeL(RReadStream& aReadStream) = 0;
	virtual void SetCharJustification(TInt aExcessWidth, TInt aNumGaps) = 0;
	virtual void SetWordJustification(TInt aExcessWidth, TInt aNumChars) = 0;
	virtual void SetUnderlineStyle(DirectGdi::TFontUnderline aUnderlineStyle) = 0;
	virtual void
			SetStrikethroughStyle(DirectGdi::TFontStrikethrough aStrikethroughStyle) = 0;
	virtual void UpdateJustification(const TDesC& aText, const DirectGdi::TTextParameters* aParam) = 0;
	virtual void UpdateJustificationVertical(const TDesC& aText, const DirectGdi::TTextParameters* aParam, TBool aUp) = 0;
	virtual void SetFontNoDuplicate(const CFont* aFont) = 0;
	virtual TInt GetError() = 0;
	// text drawing
	virtual void DrawText(const TDesC& aText, const DirectGdi::TTextParameters* aParam) = 0;
	virtual void DrawText(const TDesC& aText, const DirectGdi::TTextParameters* aParam, const TPoint& aPosition) = 0;
	virtual void DrawText(const TDesC& aText, const DirectGdi::TTextParameters* aParam, const TRect& aBox) = 0;
	virtual void DrawText(const TDesC& aText, const DirectGdi::TTextParameters* aParam, const TRect& aBox, TInt aBaselineOffset, 
		DirectGdi::TTextAlign aAlignment = DirectGdi::ELeft, TInt aMargin = 0) = 0;
	virtual void DrawTextVertical(const TDesC& aText, const DirectGdi::TTextParameters* aParam, TBool aUp) = 0;
	virtual void DrawTextVertical(const TDesC& aText, const DirectGdi::TTextParameters* aParam, const TPoint& aPosition, TBool aUp) = 0;
	virtual void DrawTextVertical(const TDesC& aText, const DirectGdi::TTextParameters* aParam, const TRect& aBox, TBool aUp) = 0;
	virtual void DrawTextVertical(const TDesC& aText, const DirectGdi::TTextParameters* aParam, const TRect& aBox, TInt aBaselineOffset, 
			TBool aUp, DirectGdi::TTextAlign aVerticalAlignment = DirectGdi::ELeft, TInt aMargin = 0) = 0;
	virtual void DrawTextVertical(const TDesC& aText, const DirectGdi::TTextParameters* aParam, const TRect& aBox, TInt aBaselineOffset, 
			TInt aTextWidth, TBool aUp, DirectGdi::TTextAlign aVerticalAlignment = DirectGdi::ELeft, TInt aMargin = 0) = 0;
	// New functionality for image based resource (not part of BitGdi)
	virtual void DrawResource(const TPoint& aPos, 
							const TDrawableSourceAndEquivRotatedBmps& aSource, 
							DirectGdi::TGraphicsRotation aRotation=DirectGdi::EGraphicsRotationNone) = 0;
	virtual void DrawResource(const TRect& aDestRect, 
							const TDrawableSourceAndEquivRotatedBmps& aSource,
							DirectGdi::TGraphicsRotation aRotation=DirectGdi::EGraphicsRotationNone) = 0;
	virtual void DrawResource(const TRect& aDestRect, 
							const TDrawableSourceAndEquivRotatedBmps& aSource, 
							const TRect& aSrcRect,
							DirectGdi::TGraphicsRotation aRotation=DirectGdi::EGraphicsRotationNone) = 0;
	// New functionality for non-image based resource (not part of BitGdi)
	virtual void DrawResource(const TRect& aDestRect, 
							const TDrawableSourceAndEquivRotatedBmps& aSource, 
							const TDesC8& aParam) = 0;
	virtual TInt GetInterface(TUid aInterfaceId, TAny*& aInterface) = 0;
	virtual void CopySettings(const CTContextBase& aGc) = 0;
	};

/**
Concrete implementation of a CTContextBase.
This class implements the rendering mechanism for BitGDI.
*/
class CTBitGdiContext : public CTContextBase
	{
public:
	static CTBitGdiContext* NewL(CFbsBitmapDevice* aDevice, TBool aActivate=ETrue);
	virtual ~CTBitGdiContext();

	// from CTContextBase
	virtual void SetOrigin(const TPoint& aOrigin);
	virtual void SetClippingRegion(const TRegion& aRegion);
	virtual void ResetClippingRegion();
	virtual void SetDrawMode(DirectGdi::TDrawMode aMode);
	virtual void SetPenColor(const TRgb& aColor);
	virtual void SetPenStyle(DirectGdi::TPenStyle aStyle);
	virtual void SetPenSize(const TSize& aSize);
	virtual void SetTextShadowColor(const TRgb& aColor);
	virtual void SetBrushColor(const TRgb& aColor);
	virtual void SetBrushStyle(DirectGdi::TBrushStyle aStyle);
	virtual void SetBrushOrigin(const TPoint& aOrigin);
	virtual void SetBrushPattern(const CFbsBitmap& aPattern);
	virtual void SetBrushPattern(TInt aHandle);
	virtual void ResetBrushPattern();
	virtual void SetFont(const CFont* aFont);
	virtual void ResetFont();
	virtual void Reset();
	virtual TRgb BrushColor();
	virtual TRgb PenColor();
	virtual TRgb TextShadowColor();
	virtual void Clear(const TRect& aRect);
	virtual void Clear();
	virtual void MoveTo(const TPoint& aPoint);
	virtual void MoveBy(const TPoint& aVector);
	virtual void Plot(const TPoint& aPoint);
	virtual void DrawLine(const TPoint& aStart, const TPoint& aEnd);
	virtual void DrawLineTo(const TPoint& aPoint);
	virtual void DrawLineBy(const TPoint& aVector);
	virtual void DrawRect(const TRect& aRect);
	virtual void DrawRoundRect(const TRect& aRect, const TSize& aEllipse);
	virtual void DrawPolyLine(const CArrayFix<TPoint>& aPointList);
	virtual void DrawPolyLineNoEndPoint(const CArrayFix<TPoint>& aPointList);
	virtual void DrawPolygon(const CArrayFix<TPoint>& aPoints,
			DirectGdi::TFillRule aRule);
	virtual void DrawArc(const TRect& aRect, const TPoint& aStart,
			const TPoint& aEnd);
	virtual void DrawPie(const TRect& aRect, const TPoint& aStart,
			const TPoint& aEnd);
	virtual void DrawEllipse(const TRect& aRect);
	virtual void BitBlt(const TPoint& aPoint, const CFbsBitmap& aBitmap);
	virtual void BitBlt(const TPoint& aDestPos, const CFbsBitmap& aBitmap,
			const TRect& aSrcRect);
	virtual void BitBltMasked(const TPoint& aDestPos,
			const CFbsBitmap& aBitmap, const TRect& aSrcRect,
			const CFbsBitmap& aMask, TBool aInvertMask);
	virtual void BitBltMasked(const TPoint& aDestPos, const CFbsBitmap& aBitmap, 
			const TRect& aSourceRect, const CFbsBitmap& aAlpha, 
			const TPoint& aAlphaPos);
	virtual void DrawBitmap(const TRect& aDestRect, const CFbsBitmap& aSource);
	virtual void DrawBitmap(const TRect& aDestRect, const CFbsBitmap& aBitmap,
			const TRect& aSrcRect);
	virtual void DrawBitmapMasked(const TRect& aDestRect,
			const CFbsBitmap& aBitmap, const TRect& aSrcRect,
			const CFbsBitmap& aMask, TBool aInvertMask);
	virtual void CopyRect(const TPoint& aOffset, const TRect& aRect);
	virtual void ExternalizeL(RWriteStream& aWriteStream);
	virtual void InternalizeL(RReadStream& aReadStream);
	virtual TBool HasBrushPattern() const;
	virtual TBool HasFont() const;
	virtual void SetCharJustification(TInt aExcessWidth, TInt aNumGaps);
	virtual void SetWordJustification(TInt aExcessWidth, TInt aNumChars);
	virtual void SetUnderlineStyle(DirectGdi::TFontUnderline aUnderlineStyle);
	virtual void SetStrikethroughStyle(DirectGdi::TFontStrikethrough aStrikethroughStyle);
	virtual void UpdateJustification(const TDesC& aText, const DirectGdi::TTextParameters* aParam);
	virtual void UpdateJustificationVertical(const TDesC& aText, const DirectGdi::TTextParameters* aParam, TBool aUp);
	virtual void SetFontNoDuplicate(const CFont* aFont);
	virtual TInt GetError();
	// text drawing
	virtual void DrawText(const TDesC& aText, const DirectGdi::TTextParameters* aParam);
	virtual void DrawText(const TDesC& aText, const DirectGdi::TTextParameters* aParam, const TPoint& aPosition);
	virtual void DrawText(const TDesC& aText, const DirectGdi::TTextParameters* aParam, const TRect& aBox);
	virtual void DrawText(const TDesC& aText, const DirectGdi::TTextParameters* aParam, const TRect& aBox, TInt aBaselineOffset, 
			DirectGdi::TTextAlign aAlignment = DirectGdi::ELeft, TInt aMargin = 0);
	virtual void DrawText(const TDesC& aText, const DirectGdi::TTextParameters* aParam, const TRect& aBox, TInt aBaselineOffset, 
			TInt aTextWidth, DirectGdi::TTextAlign aAlignment = DirectGdi::ELeft, TInt aMargin = 0);
	virtual void DrawTextVertical(const TDesC& aText, const DirectGdi::TTextParameters* aParam, TBool aUp);
	virtual void DrawTextVertical(const TDesC& aText, const DirectGdi::TTextParameters* aParam, const TPoint& aPosition, TBool aUp);
	virtual void DrawTextVertical(const TDesC& aText, const DirectGdi::TTextParameters* aParam, const TRect& aBox, TBool aUp);
	virtual void DrawTextVertical(const TDesC& aText, const DirectGdi::TTextParameters* aParam, const TRect& aBox, TInt aBaselineOffset, 
			TBool aUp, DirectGdi::TTextAlign aVerticalAlignment = DirectGdi::ELeft, TInt aMargin = 0);
	virtual void DrawTextVertical(const TDesC& aText, const DirectGdi::TTextParameters* aParam, const TRect& aBox, TInt aBaselineOffset, 
			TInt aTextWidth, TBool aUp, DirectGdi::TTextAlign aVerticalAlignment = DirectGdi::ELeft, TInt aMargin = 0);

   // New functionality for image based resource (not part of BitGdi)
	virtual void DrawResource(const TPoint& aPos, 
							const TDrawableSourceAndEquivRotatedBmps& aSource, 
							DirectGdi::TGraphicsRotation aRotation=DirectGdi::EGraphicsRotationNone);
	virtual void DrawResource(const TRect& aDestRect, 
							const TDrawableSourceAndEquivRotatedBmps& aSource,
							DirectGdi::TGraphicsRotation aRotation=DirectGdi::EGraphicsRotationNone);
	virtual void DrawResource(const TRect& aDestRect, 
							const TDrawableSourceAndEquivRotatedBmps& aSource, 
							const TRect& aSrcRect,
							DirectGdi::TGraphicsRotation aRotation=DirectGdi::EGraphicsRotationNone);
	// New functionality for non-image based resource (not part of BitGdi)
	virtual void DrawResource(const TRect& aDestRect, 
							const TDrawableSourceAndEquivRotatedBmps& aSource, 
							const TDesC8& aParam);
 
	virtual TInt Activate(CBitmapDevice *aDevice);
	virtual TInt GetInterface(TUid aInterfaceId, TAny*& aInterface);
	virtual void CopySettings(const CTContextBase& aGc);
private:
	CTBitGdiContext();
	void ConstructL(CFbsBitmapDevice* aDevice, TBool aActivate);
	void SetError(TInt aErr);

public:
	CFbsBitGc* iGc;
	TInt iError;
	};

	
/**
Concrete implementation of a CTContextBase.
This class implements the rendering mechanism for DirectGDI.
*/
class CTestDirectGdiContext : public CTContextBase
	{
public:
	static CTestDirectGdiContext* NewL();
	virtual ~CTestDirectGdiContext();

	// from CTContextBase
	virtual void SetOrigin(const TPoint& aOrigin);
	virtual void SetClippingRegion(const TRegion& aRegion);
	virtual void ResetClippingRegion();
	virtual void SetDrawMode(DirectGdi::TDrawMode aMode);
	virtual void SetPenColor(const TRgb& aColor);
	virtual void SetPenStyle(DirectGdi::TPenStyle aStyle);
	virtual void SetPenSize(const TSize& aSize);
	virtual void SetTextShadowColor(const TRgb& aColor);
	virtual void SetBrushColor(const TRgb& aColor);
	virtual void SetBrushStyle(DirectGdi::TBrushStyle aStyle);
	virtual void SetBrushOrigin(const TPoint& aOrigin);
	virtual void SetBrushPattern(const CFbsBitmap& aPattern);
	virtual void SetBrushPattern(TInt aHandle);
	virtual void ResetBrushPattern();
	virtual void SetFont(const CFont* aFont);
	virtual void ResetFont();
	virtual void Reset();
	virtual TRgb BrushColor();
	virtual TRgb PenColor();
	virtual TRgb TextShadowColor();
	virtual void Clear(const TRect& aRect);
	virtual void Clear();
	virtual void MoveTo(const TPoint& aPoint);
	virtual void MoveBy(const TPoint& aVector);
	virtual void Plot(const TPoint& aPoint);
	virtual void DrawLine(const TPoint& aStart, const TPoint& aEnd);
	virtual void DrawLineTo(const TPoint& aPoint);
	virtual void DrawLineBy(const TPoint& aVector);
	virtual void DrawRect(const TRect& aRect);
	virtual void DrawRoundRect(const TRect& aRect, const TSize& aEllipse);
	virtual void DrawPolyLine(const CArrayFix<TPoint>& aPointList);
	virtual void DrawPolyLineNoEndPoint(const CArrayFix<TPoint>& aPointList);
	virtual void DrawPolygon(const CArrayFix<TPoint>& aPoints,
			DirectGdi::TFillRule aRule);
	virtual void DrawArc(const TRect& aRect, const TPoint& aStart,
			const TPoint& aEnd);
	virtual void DrawPie(const TRect& aRect, const TPoint& aStart,
			const TPoint& aEnd);
	virtual void DrawEllipse(const TRect& aRect);
	virtual void BitBlt(const TPoint& aPoint, const CFbsBitmap& aBitmap);
	virtual void BitBlt(const TPoint& aDestPos, const CFbsBitmap& aBitmap,
			const TRect& aSrcRect);
	virtual void BitBltMasked(const TPoint& aDestPos,
			const CFbsBitmap& aBitmap, const TRect& aSrcRect,
			const CFbsBitmap& aMask, TBool aInvertMask);
	virtual void BitBltMasked(const TPoint& aDestPos, const CFbsBitmap& aBitmap, 
			const TRect& aSourceRect, const CFbsBitmap& aAlpha, 
			const TPoint& aAlphaPos);
	virtual void DrawBitmap(const TRect& aDestRect, const CFbsBitmap& aSource);
	virtual void DrawBitmap(const TRect& aDestRect, const CFbsBitmap& aBitmap,
			const TRect& aSrcRect);
	virtual void DrawBitmapMasked(const TRect& aDestRect,
			const CFbsBitmap& aBitmap, const TRect& aSrcRect,
			const CFbsBitmap& aMask, TBool aInvertMask);
	virtual void CopyRect(const TPoint& aOffset, const TRect& aRect);
	virtual void ExternalizeL(RWriteStream& aWriteStream);
	virtual void InternalizeL(RReadStream& aReadStream);
	virtual TBool HasBrushPattern() const;
	virtual TBool HasFont() const;
	virtual void SetCharJustification(TInt aExcessWidth, TInt aNumGaps);
	virtual void SetWordJustification(TInt aExcessWidth, TInt aNumChars);
	virtual void SetUnderlineStyle(DirectGdi::TFontUnderline aUnderlineStyle);
	virtual void SetStrikethroughStyle(DirectGdi::TFontStrikethrough aStrikethroughStyle);
	virtual void UpdateJustification(const TDesC& aText, const DirectGdi::TTextParameters* aParam);
	virtual void UpdateJustificationVertical(const TDesC& aText, const DirectGdi::TTextParameters* aParam, TBool aUp);
	virtual void SetFontNoDuplicate(const CFont* aFont);
	virtual TInt GetError();
	// text drawing
	virtual void DrawText(const TDesC& aText, const DirectGdi::TTextParameters* aParam);
	virtual void DrawText(const TDesC& aText, const DirectGdi::TTextParameters* aParam, const TPoint& aPosition);
	virtual void DrawText(const TDesC& aText, const DirectGdi::TTextParameters* aParam, const TRect& aBox);
	virtual void DrawText(const TDesC& aText, const DirectGdi::TTextParameters* aParam, const TRect& aBox, TInt aBaselineOffset, 
		DirectGdi::TTextAlign aAlignment = DirectGdi::ELeft, TInt aMargin = 0);
	virtual void DrawTextVertical(const TDesC& aText, const DirectGdi::TTextParameters* aParam, TBool aUp);
	virtual void DrawTextVertical(const TDesC& aText, const DirectGdi::TTextParameters* aParam, const TPoint& aPosition, TBool aUp);
	virtual void DrawTextVertical(const TDesC& aText, const DirectGdi::TTextParameters* aParam, const TRect& aBox, TBool aUp);
	virtual void DrawTextVertical(const TDesC& aText, const DirectGdi::TTextParameters* aParam, const TRect& aBox, TInt aBaselineOffset, 
			TBool aUp, DirectGdi::TTextAlign aVerticalAlignment = DirectGdi::ELeft, TInt aMargin = 0);
	virtual void DrawTextVertical(const TDesC& aText, const DirectGdi::TTextParameters* aParam, const TRect& aBox, TInt aBaselineOffset, 
			TInt aTextWidth, TBool aUp, DirectGdi::TTextAlign aVerticalAlignment = DirectGdi::ELeft, TInt aMargin = 0);
	// New functionality for image based resource (not part of BitGdi)
	virtual void DrawResource(const TPoint& aPos, 
							const TDrawableSourceAndEquivRotatedBmps& aSource, 
							DirectGdi::TGraphicsRotation aRotation=DirectGdi::EGraphicsRotationNone);
	virtual void DrawResource(const TRect& aDestRect, 
							const TDrawableSourceAndEquivRotatedBmps& aSource,
							DirectGdi::TGraphicsRotation aRotation=DirectGdi::EGraphicsRotationNone);
	virtual void DrawResource(const TRect& aDestRect, 
							const TDrawableSourceAndEquivRotatedBmps& aSource, 
							const TRect& aSrcRect,
							DirectGdi::TGraphicsRotation aRotation=DirectGdi::EGraphicsRotationNone);
	// New functionality for non-image based resource (not part of BitGdi)
	virtual void DrawResource(const TRect& aDestRect, 
							const TDrawableSourceAndEquivRotatedBmps& aSource, 
							const TDesC8& aParam);

	virtual TInt GetInterface(TUid aInterfaceId, TAny*& aInterface);

	// CTestDirectGdiContext specific
	virtual TInt Activate(RDirectGdiImageTarget& aTarget);
	void NoJustifyAutoUpdate();
	void SetJustifyAutoUpdate();
	virtual void CopySettings(const CTContextBase& aGc);
	
private:
	CTestDirectGdiContext();
	void ConstructL();

public:
	CDirectGdiDriver* iGdiDriver;
	CDirectGdiContext* iGc;
	};

#endif
