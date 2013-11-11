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
// Declarations common to both the GDI adaptation layer and
// modules and interfaces that make use of that layer.
// 
//

#ifndef DIRECTGDITYPES_H
#define DIRECTGDITYPES_H

#include <e32def.h>
#include <e32const.h>

/**
@publishedPartner
@prototype
*/

namespace DirectGdi
	{
	/** Defines possible clockwise rotation values.
	@publishedPartner
	@prototype
	@deprecated
	*/
	enum TGraphicsRotation
		{
		/** No rotation. */
		EGraphicsRotationNone,
		/** A 90 degree rotation. */
		EGraphicsRotation90,
		/** A 180 degree rotation. */
		EGraphicsRotation180,
		/** A 270 degree rotation. */
		EGraphicsRotation270
		};	


	/** Text alignment.
	@publishedPartner
	@prototype
	@deprecated
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
	Drawing modes. The drawing modes define the way that the pen and brush colors are
	drawn, EDrawModePEN means do alpha blending if appropriate, EDrawModeWriteAlpha means 
	don't do alpha blending.

	@see SetDrawMode()
	@publishedPartner
	@prototype
	@deprecated
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
	@deprecated
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
	Brush styles. 
	@publishedPartner
	@prototype
	@deprecated
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
	Rules used to fill self-intersecting polygons. 

	The filling of a polygon proceeds as follows: for a given point in the 
	polygon, then:

	if the rule is TFillRule::EAlternate (default) and it has an odd winding 
	number, then fill the surrounding area.

	if the rule is TFillRule::EWinding and it has a winding number greater than 
	zero, then fill the surrounding area. 
	@publishedPartner
	@prototype
	@deprecated
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
	@deprecated
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
	@deprecated
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
	It is used by CGraphicsContext::DrawText() and CGraphicsContext::DrawTextVertical() family of APIs
	to draw text from iStart to iEnd within the supplied text descriptor.
	@publishedPartner
	@prototype
	@deprecated
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
	}

#endif // DIRECTGDITYPES_H
