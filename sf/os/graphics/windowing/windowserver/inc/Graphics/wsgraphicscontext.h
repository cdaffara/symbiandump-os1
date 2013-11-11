// Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Interface for Render Stage Abstract Graphics Context
// 
//

/**
 @publishedPartner
 @prototype
*/

#ifndef __WSGRAPHICSCONTEXT_H__
#define __WSGRAPHICSCONTEXT_H__

#include <w32std.h>
#include <graphics/wsgraphicdrawerinterface.h>

/** Render Stage abstract graphics context.

This interface is provided by Render Stages.  It abstracts the graphics context
of the pixel target.  The intention is to allow Render Stages to intercept
draw operations so that they can be batched (to reduce flickering), modified
(to provide transition effects), or re-directed (to alternate back-end
architectures).

@publishedPartner
@prototype
*/
class MWsGraphicsContext : public MWsObjectProvider
	{
public:
	DECLARE_WS_TYPE_ID(KMWsGraphicsContext)
	/** Text alignment.
	@publishedPartner
	@prototype
	*/
	enum TTextAlign
		{
		/** Text is left-aligned. */
		ELeft,
		/** Text is centred. */
		ECenter,
		/** Text is right-aligned. */
		ERight
		};

	/** 
	Brush styles. 
	@publishedPartner
	@prototype
	*/
	enum TBrushStyle
		{
		/** The brush fill has no effect (default). */
		ENullBrush,
		/** The brush fills with a solid single colour, determined by 
		SetBrushColor() and the drawing mode. */
		ESolidBrush,
		/** The brush fills with a selected bitmap pattern, set by 
		UseBrushPattern(). */
		EPatternedBrush,
		/** The brush fills with vertical hatching  lines going from top to 
		bottom. */
		EVerticalHatchBrush,
		/** The brush fills with diagonal hatching lines going from bottom 
		left to top right. */
		EForwardDiagonalHatchBrush,
		/** The brush fills with horizontal hatching lines going from left 
		to right. */
		EHorizontalHatchBrush,
		/** The brush fills with rearward diagonal hatching lines going from top 
		left to bottom right. */
		ERearwardDiagonalHatchBrush,
		/** The brush fills with horizontal and vertical hatching  lines going 
		from left to right plus lines going from top to bottom  giving the 
		effect of a grid of small squares */
		ESquareCrossHatchBrush,
		/** The brush fills with forward diagonal and rearward diagonal hatching 
		lines going from bottom left to top right plus lines going from top left 
		to bottom right giving the effect of a grid of small diamonds. */
		EDiamondCrossHatchBrush
		};

	/** 
	Drawing modes. The drawing modes define the way that the pen and brush colors are
	drawn, EDrawModePEN means do alpha blending if appropriate, EDrawModeWriteAlpha means 
	don't do alpha blending.

	@see SetDrawMode()
	@publishedPartner
	@prototype
	*/
	enum TDrawMode
		{		
		/** Uses both pen and brush colour as they are. */
		EDrawModePEN=32,		
		/** Writes alpha information in the source directly into the destination, rather than blending. */
		EDrawModeWriteAlpha=64
		};

	/** 
	Pen styles. The screen pattern unit in each definition below describes the 
	pattern drawn by the line 1 represents a pixel drawn, 0 represents a 
	pixel that is not affected. 
	@publishedPartner
	@prototype
	*/
	enum TPenStyle
		{
		/** The pen does not draw. Screen pattern unit = 00... */
		ENullPen,
		/** A solid line (default). Screen pattern unit = 11... */
		ESolidPen,
		/** A dotted line. Screen pattern unit = 1000... */
		EDottedPen,
		/** A dashed line. Screen pattern unit = 111000... */
		EDashedPen,
		/** A line of alternating dashes and dots. Screen pattern unit = 
		1111001100... */
		EDotDashPen,
		/** A line of alternating single dashes and pairs of dots. Screen 
		pattern unit = 11110011001100... */
		EDotDotDashPen
		};

	/** 
	Rules used to fill self-intersecting polygons. 

	The filling of a polygon proceeds as follows: for a given point in the 
	polygon, then:

	if the rule is TFillRule::EAlternate (default) and it has an odd winding 
	number, then fill the surrounding area.

	if the rule is TFillRule::EWinding and it has a winding number greater than 
	zero, then fill the surrounding area. 
	@publishedPartner
	@prototype
	*/
	enum TFillRule
		{
		/** Only fill areas with odd winding numbers. */
		EAlternate,
		/** Fill areas with winding numbers greater than zero. */
		EWinding
		};

	/**
	Font underline flags. 
	@publishedPartner
	@prototype
	*/
	enum TFontUnderline
		{
		/** Font is not underlined. */
		EUnderlineOff,
		/** Font is underlined. */
		EUnderlineOn
		};

	/**
	Font strike-through flags. 
	@publishedPartner
	@prototype
	*/
	enum TFontStrikethrough
		{
		/** Font is not struck-through. */
		EStrikethroughOff,
		/** Font is struck-through. */
		EStrikethroughOn
		};

	/**
	Parameters used in drawing text within supplied context.
	It is used by MWsGraphicsContext::DrawText() and MWsGraphicsContext::DrawTextVertical() family of API's
	to draw text from iStart to iEnd withing the supplied text descriptor.
	
	@publishedPartner
	@prototype
	*/
	class TTextParameters
		{
	public:
		TTextParameters():
			iStart(0),
			iEnd(KMaxTInt),
			iFlags(0)
				{
				}
		TInt iStart;
		TInt iEnd;
		TUint16 iFlags;
		/* Reserved for future use */
		TAny* iReserved1;
		TAny* iReserved2;
		TAny* iReserved3;
		TAny* iReserved4;
		};
	
public:
	// The following are equivalent to CFbsBitGc functionality which maps 
	// to client’s CWindowGc requests
	virtual void BitBlt(const TPoint& aDestPos, const CFbsBitmap& aSourceBitmap) = 0;
	virtual void BitBlt(const TPoint& aDestPos, const CFbsBitmap& aSourceBitmap,	const TRect& aSourceRect) = 0;
	virtual void BitBltMasked(const TPoint& aDestPos, const CFbsBitmap& aSourceBitmap, const TRect& aSourceRect, const CFbsBitmap& aMaskBitmap, TBool aInvertMask) = 0;
	virtual void BitBltMasked(const TPoint& aDestPos, const CFbsBitmap& aSourceBitmap, const TRect& aSourceRect, const CFbsBitmap& aMaskBitmap, const TPoint& aMaskPos) = 0;
	virtual void ResetClippingRegion() = 0;
	virtual void Clear() = 0;
	virtual void Clear(const TRect& aRect) = 0;
	virtual void ResetBrushPattern() = 0;
	virtual void ResetFont() = 0;
	virtual void DrawArc(const TRect& aRect, const TPoint& aStart, const TPoint& aEnd) = 0;
	virtual void DrawPie(const TRect& aRect, const TPoint& aStart, const TPoint& aEnd) = 0;
	virtual void DrawBitmap(const TRect& aDestRect, const CFbsBitmap& aSourceBitmap) = 0;
	virtual void DrawBitmap(const TRect& aDestRect, const CFbsBitmap& aSourceBitmap, const TRect& aSourceRect) = 0;
	virtual void DrawBitmapMasked(const TRect& aDestRect, const CFbsBitmap& aSourceBitmap,const TRect& aSourceRect, const CFbsBitmap& aMaskBitmap, TBool aInvertMask) = 0;
	virtual void DrawRoundRect(const TRect& aRect, const TSize& aEllipse) = 0;
	virtual void DrawPolyLine(const TArray<TPoint>& aPointList) = 0; 
	virtual void DrawPolyLineNoEndPoint(const TArray<TPoint>& aPointList) = 0;  
	virtual void DrawPolygon(const TArray<TPoint>& aPointList, TFillRule aFillRule=EAlternate) = 0;
	virtual void DrawEllipse(const TRect& aRect) = 0;
	virtual void DrawLine(const TPoint& aStart, const TPoint& aEnd) = 0;
	virtual void DrawLineTo(const TPoint& aPoint) = 0;
	virtual void DrawLineBy(const TPoint& aVector) = 0;
	virtual void DrawRect(const TRect& aRect) = 0;
	virtual void DrawText(const TDesC& aText,const TTextParameters* aParam) = 0;
	virtual void DrawText(const TDesC& aText,const TTextParameters* aParam,const TPoint& aPosition) = 0;
	virtual void DrawText(const TDesC& aText,const TTextParameters* aParam,const TRect& aClipRect) = 0;
	virtual void DrawText(const TDesC& aText,const TTextParameters* aParam,const TRect& aClipFillRect,TInt aBaselineOffset,TTextAlign aHrz=ELeft,TInt aMargin=0) = 0;
	virtual void DrawTextVertical(const TDesC& aText,const TTextParameters* aParam,TBool aUp) = 0;
	virtual void DrawTextVertical(const TDesC& aText,const TTextParameters* aParam,const TPoint& aPosition,TBool aUp) = 0;
	virtual void DrawTextVertical(const TDesC& aText,const TTextParameters* aParam,const TRect& aClipRect,TBool aUp) = 0;
	virtual void DrawTextVertical(const TDesC& aText,const TTextParameters* aParam,const TRect& aClipRect,TInt aBaselineOffset,TBool aUp,TTextAlign aVert=ELeft,TInt aMargin=0) = 0;
	virtual void DrawTextVertical(const TDesC& aText,const TTextParameters* aParam,const TRect& aClipRect,TInt aBaselineOffset,TInt aTextWidth,TBool aUp,TTextAlign aVert=ELeft,TInt aMargin=0) = 0;
	virtual void MoveTo(const TPoint& aPoint) = 0;
	virtual void MoveBy(const TPoint& aVector) = 0;
	virtual void Plot(const TPoint& aPoint) = 0;
	virtual void Reset() = 0;
	virtual void SetBrushColor(const TRgb& aColor) = 0;
	virtual void SetBrushOrigin(const TPoint& aOrigin) = 0;
	virtual void SetBrushStyle(TBrushStyle aBrushStyle) = 0;
	virtual void SetClippingRegion(const TRegion& aRegion) = 0;
	virtual void SetDrawMode(TDrawMode aDrawMode) = 0;
	virtual void SetOrigin(const TPoint& aPoint = TPoint(0, 0)) = 0;
	virtual void SetPenColor(const TRgb& aColor) = 0;
	virtual void SetPenStyle(TPenStyle aPenStyle) = 0;
	virtual void SetPenSize(const TSize& aSize) = 0;
	virtual void SetTextShadowColor(const TRgb& aColor) = 0;
	virtual void SetCharJustification(TInt aExcessWidth, TInt aNumChars) = 0;
	virtual void SetWordJustification(TInt aExcessWidth, TInt aNumGaps) = 0;
	virtual void SetUnderlineStyle(TFontUnderline aUnderlineStyle) = 0;
	virtual void SetStrikethroughStyle(TFontStrikethrough aStrikethroughStyle) = 0;
	virtual void SetBrushPattern(const CFbsBitmap& aBitmap) = 0;
	virtual void SetBrushPattern(TInt aFbsBitmapHandle) = 0;
	virtual void SetFont(const CFont* aFont) = 0;
	
	// CFbsBitGc functionality used by Wserv for its internal operations
	virtual void CopyRect(const TPoint& aOffset, const TRect& aRect) = 0;
	virtual void UpdateJustification(const TDesC& aText,const TTextParameters* aParam) = 0;
	virtual void UpdateJustificationVertical(const TDesC& aText,const TTextParameters* aParam,TBool aUp) = 0;
	virtual void SetFontNoDuplicate(const CFont* aFont) = 0;
	virtual TBool HasBrushPattern() const = 0;
	virtual TBool HasFont() const = 0;
	virtual TRgb BrushColor() const = 0;
	virtual TRgb PenColor() const = 0;
	virtual TRgb TextShadowColor() const = 0;
	
	// Return current error status of MWsGraphicsContext
	virtual TInt GetError() = 0;
	
	/** The origin of the GC relative to the screen's origin
		@return the origin */
	virtual TPoint Origin() const = 0;
	/** The clipping region currently being used
		@return the clipping region */
	virtual const TRegion& ClippingRegion() = 0;
	/** Saves the state of the GC to an internal buffer.  Several GC states can be saved in a FILO.
		Do not restore a GC (using Pop()) that wasn't properly saved!
		@return KErrNone if successful, else one of the system-wide error codes. */
	virtual TInt Push() = 0;
	/** Restores the last GC state that was saved. */
	virtual void Pop() = 0;
	};

#endif	//__WSGRAPHICSCONTEXT_H__
