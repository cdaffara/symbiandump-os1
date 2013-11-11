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
// Declaration of a Graphics Device Interface (GDI) Context, which hooks
// directly into a platform specific adaptation (typically hardware accelerated).
// 
//

#ifndef DIRECTGDICONTEXT_H
#define DIRECTGDICONTEXT_H

/**
@file
@publishedPartner
@prototype
*/

#include <graphics/directgdifont.h>
#include <graphics/directgditypes.h>
#include <e32std.h>
#include <e32def.h>

// Forward declarations.
//
class CFbsBitmap;
class RDirectGdiImageTarget;
class MDirectGdiEngine;
class CDirectGdiDriver;
class RDirectGdiDrawableSource;


/**
This class typically delegates to a platform specific rendering engine 
which uses a generic interface and provides a platform specific mechanism. 
The rendering engine may or may not take advantage of hardware acceleration, 
depending on its implementation.

@publishedPartner
@prototype
@deprecated

@see CBitmapContext 
*/
NONSHARABLE_CLASS(CDirectGdiContext): public CBase
	{
public:
	IMPORT_C static CDirectGdiContext* NewL(CDirectGdiDriver& aDriver);
	IMPORT_C virtual ~CDirectGdiContext();
	IMPORT_C TInt Activate(RDirectGdiImageTarget& aTarget);
	IMPORT_C void NoJustifyAutoUpdate();
	IMPORT_C void SetJustifyAutoUpdate();
	// The following are equivalent to CFbsBitGc functionality which maps 
	// to client’s CWindowGc requests
	IMPORT_C void BitBlt(const TPoint& aDestPos, const CFbsBitmap& aSourceBitmap);
	IMPORT_C void BitBlt(const TPoint& aDestPos, const CFbsBitmap& aSourceBitmap,	const TRect& aSourceRect);
	IMPORT_C void BitBltMasked(const TPoint& aDestPos, 
		const CFbsBitmap& aSourceBitmap, const TRect& aSourceRect, 
		const CFbsBitmap& aMaskBitmap, TBool aInvertMask);
	IMPORT_C void BitBltMasked(const TPoint& aDestPos,
		const CFbsBitmap& aSourceBitmap, const TRect& aSourceRect,
		const CFbsBitmap& aMaskBitmap, const TPoint& aMaskPos);
	IMPORT_C void ResetClippingRegion();
	IMPORT_C void Clear();
	IMPORT_C void Clear(const TRect& aRect);
	IMPORT_C void ResetBrushPattern();
	IMPORT_C void ResetFont();
	IMPORT_C void DrawArc(const TRect& aRect, const TPoint& aStart, const TPoint& aEnd);
	IMPORT_C void DrawPie(const TRect& aRect, const TPoint& aStart, const TPoint& aEnd);
	IMPORT_C void DrawBitmap(const TRect& aDestRect, const CFbsBitmap& aSourceBitmap);
	IMPORT_C void DrawBitmap(const TRect& aDestRect, 
		const CFbsBitmap& aSourceBitmap, const TRect& aSourceRect);
	IMPORT_C void DrawBitmapMasked(const TRect& aDestRect, 
		const CFbsBitmap& aSourceBitmap,const TRect& aSourceRect, 
		const CFbsBitmap& aMaskBitmap, TBool aInvertMask);
	IMPORT_C void DrawRoundRect(const TRect& aRect, const TSize& aCornerSize);
	IMPORT_C void DrawPolyLine(const TArray<TPoint>& aPointList);
	IMPORT_C void DrawPolyLineNoEndPoint(const TArray<TPoint>& aPointList);
	IMPORT_C void DrawPolygon(const TArray<TPoint>& aPointList, DirectGdi::TFillRule aFillRule=DirectGdi::EAlternate);
	IMPORT_C void DrawEllipse(const TRect& aRect);
	IMPORT_C void DrawLine(const TPoint& aStart, const TPoint& aEnd);
	IMPORT_C void DrawLineTo(const TPoint& aPoint);
	IMPORT_C void DrawLineBy(const TPoint& aVector);
	IMPORT_C void DrawRect(const TRect& aRect);
	IMPORT_C void DrawText(const TDesC& aText, const DirectGdi::TTextParameters* aParam);
	IMPORT_C void DrawText(const TDesC& aText, const DirectGdi::TTextParameters* aParam, const TPoint& aPosition);
	IMPORT_C void DrawText(const TDesC& aText, const DirectGdi::TTextParameters* aParam, const TRect& aClipRect);
	IMPORT_C void DrawText(const TDesC& aText, const DirectGdi::TTextParameters* aParam, const TRect& aClipFillRect, TInt aBaselineOffset, 
		DirectGdi::TTextAlign aAlignment = DirectGdi::ELeft, TInt aMargin = 0);
	IMPORT_C void DrawTextVertical(const TDesC& aText, const DirectGdi::TTextParameters* aParam, TBool aUp);
	IMPORT_C void DrawTextVertical(const TDesC& aText, const DirectGdi::TTextParameters* aParam, const TPoint& aPosition, TBool aUp);
	IMPORT_C void DrawTextVertical(const TDesC& aText, const DirectGdi::TTextParameters* aParam, const TRect& aClipRect, TBool aUp);
	IMPORT_C void DrawTextVertical(const TDesC& aText, const DirectGdi::TTextParameters* aParam, const TRect& aClipRect, TInt aBaselineOffset, 
		TBool aUp, DirectGdi::TTextAlign aVerticalAlignment = DirectGdi::ELeft, TInt aMargin = 0);
	IMPORT_C void DrawTextVertical(const TDesC& aText, const DirectGdi::TTextParameters* aParam, const TRect& aClipRect, TInt aBaselineOffset, 
		TInt aTextWidth, TBool aUp, DirectGdi::TTextAlign aVerticalAlignment = DirectGdi::ELeft, TInt aMargin = 0);
	IMPORT_C void MoveTo(const TPoint& aPoint);
	IMPORT_C void MoveBy(const TPoint& aVector);
	IMPORT_C void Plot(const TPoint& aPoint);
	IMPORT_C void Reset();
	IMPORT_C void SetBrushColor(const TRgb& aColor);
	IMPORT_C void SetBrushOrigin(const TPoint& aOrigin);
	IMPORT_C void SetBrushStyle(DirectGdi::TBrushStyle aBrushStyle);
	IMPORT_C void SetClippingRegion(const TRegion& aRegion);
	IMPORT_C void SetDrawMode(DirectGdi::TDrawMode aDrawMode);
	IMPORT_C void SetOrigin(const TPoint& aPoint = TPoint(0, 0));
	IMPORT_C void SetPenColor(const TRgb& aColor);
	IMPORT_C void SetPenStyle(DirectGdi::TPenStyle aPenStyle);
	IMPORT_C void SetPenSize(const TSize& aSize);
	IMPORT_C void SetTextShadowColor(const TRgb& aColor);
	IMPORT_C void SetCharJustification(TInt aExcessWidth, TInt aNumChars);
	IMPORT_C void SetWordJustification(TInt aExcessWidth, TInt aNumGaps);
	IMPORT_C void SetUnderlineStyle(DirectGdi::TFontUnderline aUnderlineStyle);
	IMPORT_C void SetStrikethroughStyle(DirectGdi::TFontStrikethrough aStrikethroughStyle);
	IMPORT_C void SetBrushPattern(const CFbsBitmap& aBitmap);
	IMPORT_C void SetBrushPattern(TInt aFbsBitmapHandle);
	IMPORT_C void SetFont(const CFont* aFont);

	// CFbsBitGc functionality used by Wserv for its internal operations. Compatibility of these functions is not guaranteed in future releases.
	IMPORT_C void CopyRect(const TPoint& aOffset, const TRect& aRect);	/**< WARNING: Function for internal use ONLY. Compatibility is not guaranteed in future releases.*/
	IMPORT_C void CopySettings(const CDirectGdiContext& aGc);	/**< WARNING: Function for internal use ONLY. Compatibility is not guaranteed in future releases.*/
	IMPORT_C void UpdateJustification(const TDesC& aText, const DirectGdi::TTextParameters* aParam);	/**< WARNING: Function for internal use ONLY. Compatibility is not guaranteed in future releases.*/
	IMPORT_C void UpdateJustificationVertical(const TDesC& aText, const DirectGdi::TTextParameters* aParam, TBool aUp);	/**< WARNING: Function for internal use ONLY. Compatibility is not guaranteed in future releases.*/
	IMPORT_C void SetFontNoDuplicate(const CDirectGdiFont* aFont);	/**< WARNING: Function for internal use ONLY. Compatibility is not guaranteed in future releases.*/
	IMPORT_C TBool HasBrushPattern() const;	/**< WARNING: Function for internal use ONLY. Compatibility is not guaranteed in future releases.*/
	IMPORT_C TBool HasFont() const;	/**< WARNING: Function for internal use ONLY. Compatibility is not guaranteed in future releases.*/
	IMPORT_C void ExternalizeL(RWriteStream& aWriteStream);	/**< WARNING: Function for internal use ONLY. Compatibility is not guaranteed in future releases.*/
	IMPORT_C void InternalizeL(RReadStream& aReadStream);	/**< WARNING: Function for internal use ONLY. Compatibility is not guaranteed in future releases.*/
	IMPORT_C TRgb BrushColor() const;	/**< WARNING: Function for internal use ONLY. Compatibility is not guaranteed in future releases.*/
	IMPORT_C TRgb PenColor() const;	/**< WARNING: Function for internal use ONLY. Compatibility is not guaranteed in future releases.*/
	IMPORT_C TRgb TextShadowColor() const;	/**< WARNING: Function for internal use ONLY. Compatibility is not guaranteed in future releases.*/

	// New functionality for image based resource
	IMPORT_C void DrawResource(const TPoint& aPos, 
		const RDirectGdiDrawableSource& aSource, 
		DirectGdi::TGraphicsRotation aRotation=DirectGdi::EGraphicsRotationNone);
	IMPORT_C void DrawResource(const TRect& aDestRect, 
		const RDirectGdiDrawableSource& aSource,
		DirectGdi::TGraphicsRotation aRotation=DirectGdi::EGraphicsRotationNone);
	IMPORT_C void DrawResource(const TRect& aDestRect, 
		const RDirectGdiDrawableSource& aSource, const TRect& aSrcRect,
		DirectGdi::TGraphicsRotation aRotation=DirectGdi::EGraphicsRotationNone);
	// New functionality for non-image based resource
	IMPORT_C void DrawResource(const TRect& aDestRect, 
		const RDirectGdiDrawableSource& aSource, const TDesC8& aParam);
	IMPORT_C TInt GetInterface(TUid aInterfaceId, TAny*& aInterface);
	
private:
	CDirectGdiContext(CDirectGdiDriver& aDirectGdiDriver);
	void ConstructL();
	void CleanUpBrushPattern();
	void DrawText(const TDesC& aText, const DirectGdi::TTextParameters* aParam, const TPoint& aPosition, DirectGdi::TTextAlign aAlignment, 
		CFont::TTextDirection aDirection, const TRect* aClipRect = NULL, const TRect* aFillRect = NULL);
	void DoDrawText(CFont::TPositionParam& aParam, const TInt aEnd, const TRect& aClipRect);
	void DoDrawTextEx(CFont::TPositionParam& aParam, const TInt aEnd, const TRect& aClipRect, const TInt aUnderlineStrikethroughOffset);
	void CalculateClipRect2PlusBaselineOffsetAndMargin(const TDesC& aText, const DirectGdi::TTextParameters* aParam, const TPoint& aPosition, TBool aUp,
		TRect& aClipRect2, TInt& aBaselineOffset, TInt& aMargin);
	void DrawTextVertical(const TDesC& aText, const DirectGdi::TTextParameters* aParam, const TRect* aClipRect1, const TRect* aClipRect2, const TRect* aFillRect,
		TInt aBaselineOffset, TInt aTextWidth, TBool aUp, DirectGdi::TTextAlign aVert, TInt aMargin);
	void DoDrawTextVertical(CFont::TPositionParam& aParam, TBool aUp, const TInt aEnd, TRect& aClipRect);
	void DoBitBltMasked(const TPoint& aDestPos, const CFbsBitmap& aSourceBitmap, const TRect& aSourceRect, 
			const CFbsBitmap& aMaskBitmap, TBool aInvertMask, const TPoint& aMaskPos);
	TBool ValidateBitmap (const CFbsBitmap& aBitmap);
	TBool ValidateSourceAndMaskBitmaps (const CFbsBitmap& aSourceBitmap, const CFbsBitmap& aMaskBitmap);
	TRect IntersectBitmapWithRect(const CFbsBitmap& aBitmap, const TRect& aRect) const;
	void Rotate(TPoint& aPoint, const TPoint& aOrigin, TBool aUp);
	void GetUnderlineMetrics(TInt& aTop, TInt& aBottom);
	void GetStrikethroughMetrics(TInt& aTop, TInt& aBottom);
	void FillRect(const TRect& aRect, const TRgb& aColor, const TRect& aClipRect);
	TInt BaselineCorrection();
private:
	/** Engine that will perform the action for most operations.
	The implementation may take advantage of hardware acceleration.
	*/
	MDirectGdiEngine*				iEngine;

	CDirectGdiDriver&				iDriver;			/**< For constructing and obtaining a reference to a rendering engine instance. */
	TBool							iActivated;			/**< Flag for whether context has been successfully activated. */
	RRegion							iClippingRegion;	/**< Local copy of clipping region.*/
	TPoint							iOrigin;			/**< The origin of the drawing engine coordinate system. */
	CDirectGdiFont					iFont;				/**< The current font. */
	TInt							iCharJustExcess;	/**< Amount of space to be used for letterspacing. */
	TInt							iCharJustNum;		/**< Number of glyph groups to be letterspaced. */
	TInt							iWordJustExcess;	/**< Amount of space to be used for wordspacing. */
	TInt							iWordJustNum;		/**< Number of spaces to be used for wordspacing. */
	TPoint							iLastPrintPosition;	/**< Current text position. */
	DirectGdi::TFontStrikethrough	iStrikethrough;		/**< Font stike-through flags. */
	DirectGdi::TFontUnderline		iUnderline;			/**< Font underline flags. */
	TRgb							iPenColor;			/**< The current pen colour. The default pen colour is black. */
	TSize							iPenSize;			/**< The current pen size. */
	DirectGdi::TPenStyle			iPenStyle;			/**< The current pen style. */
	DirectGdi::TDrawMode			iDrawMode;			/**< The current drawing mode. */
	TRgb							iTextShadowColor;	/**< Colour for text shadows. */
	TRgb							iBrushColor;		/**< The current brush colour, the default brush colour is white. */
	DirectGdi::TBrushStyle			iBrushStyle;		/**< The current brush style. */
	CFbsBitmap						iBrushPattern;		/**< The currently selected brush pattern. */
	TBool							iBrushPatternUsed;
	TPoint							iBrushOrigin;		/**< The current brush origin. */
	TBool							iAutoUpdateJustification;
	};

#endif
