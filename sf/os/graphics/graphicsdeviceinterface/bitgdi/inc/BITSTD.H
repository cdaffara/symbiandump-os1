// Copyright (c) 1997-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef __BITSTD_H__
#define __BITSTD_H__

#include <e32std.h>
#include <gdi.h>
#include <fbs.h>
#include <bitbase.h>

#ifndef SYMBIAN_ENABLE_SPLIT_HEADERS
#include <bitgdi/bitgcextradata.h>
#endif

// For use when testing the code, allows the fast blending to be disabled so back to back comparisons
// can done be to check either timing or exact output.
//#define __ALLOW_FAST_BLEND_DISABLE__

/**
 @publishedAll
 @released 
*/
class CFbsBitGcFont : public CFbsFont
	{
public:
	IMPORT_C CFbsBitGcFont();
	IMPORT_C virtual ~CFbsBitGcFont();
	IMPORT_C CBitmapFont* Address() const;
	IMPORT_C TInt Duplicate(TInt aHandle);
	IMPORT_C void Reset();
	IMPORT_C void operator=(const CFbsBitGcFont& aFont);
protected:
	TBool iCopy;
	};


/**
A bitmap managed by the font and bitmap server.
@publishedAll
@released 
*/
class CFbsBitGcBitmap : public CFbsBitmap

	{
public:
	IMPORT_C CBitwiseBitmap* Address() const;
	IMPORT_C void LockHeap() const;
	IMPORT_C void UnlockHeap() const;
	};

/**
 @publishedAll
 @released 
*/
class TEllipse
	{
public:
	/**	
	Enumeration for the ellipse production life cycle
	*/
	enum TEllipseStatus
		{
		/**
		Ellipse has been initialised but has no area
		*/
		EInitialised,
		/**
		The ellipse is drawn as two parabolas. These are in two sectors, the top and bottom
		half of the rectangle. The first sector means the top parabola has been produced.
		*/
		EFirstSector,
		/**
		The parabola that occupies the bottom half of the rectangle has been produced. 
		*/
		ESecondSector,
		/**
		The ellipse is complete
		*/
		EComplete,
		/**
		The ellipse has either a height or width of two units or less
		and so takes the appearance of a line
		*/
		ELine
		};
public:
	IMPORT_C void Construct(const TRect& aRect);
	IMPORT_C TBool SingleStep(TPoint& aTopLeft,TPoint& aTopRight,TPoint& aBottomLeft,TPoint& aBottomRight);
	IMPORT_C TBool NextStep(TPoint& aTopLeft,TPoint& aTopRight,TPoint& aBottomLeft,TPoint& aBottomRight);
	IMPORT_C TPoint Intersection(const TRect& aRect,const TPoint& aPoint);
protected:
	IMPORT_C TBool Output(TPoint& aTopLeft,TPoint& aTopRight,TPoint& aBottomLeft,TPoint& aBottomRight);
protected:
	TEllipseStatus iStatus;
	TInt iA;
	TInt iB;
	TInt64 iASquared;
	TInt64 iBSquared;
	TInt64 iASquBSqu;
	TInt iX;
	TInt iY;
	TInt iXAdj;
	TInt iYAdj;
	TPoint iOffset;
	TInt64 iD1;
	TInt64 iD2;
	};

/**
 @publishedAll
 @released 
*/
class CPolygonFiller : public CBase

	{
public:

	/**
	Describes how pixels are to be displayed in the polygon. aUsage should be select to
	one of these values before CPolygonFiller::Construct is used.
	*/
	enum TUsage
		{
		/**
		A request for all pixel runs in sequential order
		*/
		EGetAllPixelRunsSequentially,
		/**
		A request for all pixel runs in sequential order but only for specified lines.
		*/
		EGetPixelRunsSequentiallyForSpecifiedScanLines
		};
public:
	IMPORT_C CPolygonFiller();
	IMPORT_C ~CPolygonFiller();
	IMPORT_C void Construct(const CArrayFix<TPoint>* aPointArray,CGraphicsContext::TFillRule aFillRule,TUsage aUsage=EGetAllPixelRunsSequentially); // N.B. this cannot fail
	IMPORT_C void Construct(const TPoint* aPointList,TInt aNumPoints, CGraphicsContext::TFillRule aFillRule, TUsage aUsage=EGetAllPixelRunsSequentially); // N.B. this cannot fail
	IMPORT_C void Reset();
	IMPORT_C void GetNextPixelRun(TBool& aExists, TInt& aScanLine, TInt& aStart, TInt& aEnd);
	IMPORT_C void GetNextPixelRunOnSpecifiedScanLine(TBool& aExists, TInt aScanLine, TInt& aStart, TInt& aEnd);
private: // data-types for the fast algorithm
	struct SFastEdge
		{
		TInt upperVertex;
		TInt lowerVertex;
		TInt firstVertex;
		};
	struct SFastScanLineIntersection;
	struct SFastActiveEdge
		{
		SFastEdge* edgePtr;
		TLinearDDA lineGenerator;
		SFastScanLineIntersection* scanLineIntersectionPtr;
		};
	struct SFastScanLineIntersection
		{
		TInt firstPixel;
		TInt lastPixel;
		SFastActiveEdge* activeEdgePtr;
		};
private: // data-types for the slow algorithm
	struct SSlowScanLineIntersection
		{
		TInt firstPixel;
		TInt lastPixel;
		TInt firstVertexOfEdge;
		};
private: // data-types for both algorithms
	struct SFastData
		{
		TPoint* vertexList;
		SFastEdge* edgeList;
		SFastActiveEdge* activeEdgeList;
		SFastScanLineIntersection* scanLineIntersectionList;
		TInt numActiveEdges;
		TInt numScanLineIntersections;
		TInt nextEdgeToActivate;
		};
	struct SSlowData
		{
		enum {EStoreSize=8};
		TLinearDDA lineGenerator;
		SSlowScanLineIntersection scanLineIntersectionList[EStoreSize];
		TInt numIntersectionsWithSameFirstPixelPreviouslyMet;
		TInt numIntersectionsWithSameFirstPixelMetThisTime;
		TInt numScanLineIntersections;
		TBool scanLineComplete;
		TInt firstPixelOfLastIntersectionInPrevBuffer;
		};
private:
	void Construct(CGraphicsContext::TFillRule aFillRule,TUsage aUsage);
	void FastHandleVertexIntersection(TInt& aCurrentActiveEdge, TBool aIsLowerVertex);
	void SetFastIntersection(SFastActiveEdge& aActiveEdge, SFastScanLineIntersection& aScanLineIntersection);
	void SlowHandleVertexIntersection(SSlowScanLineIntersection& aScanLineIntersection, TInt& aVertexStartingCurrentEdge,TBool aIsLowerVertex);
	void JumpToCurrentScanLine(TLinearDDA& aLineGenerator, const TPoint& aUpper, const TPoint& aLower,TPoint& aStartPos, TPoint& aEndPos) const;
	const TPoint& Point(TInt aIndex);
private:
	const CArrayFix<TPoint>* iPointArray; // not owned by the class
	const TPoint* iPointList; // not owned by the class
	CGraphicsContext::TFillRule iFillRule;
	TBool iUseFastAlgorithm;
	TInt iNumVertexes;
	TBool iToggler; // used by EAlternate fill-rule
	TInt iNestingLevel; // used by EWinding fill-rule
	TInt iScanLineIntersection;
	TInt iRightMostPixelOnScanLine;
	TInt iFirstVertex;
	TBool iPolygonIsAllHorizontal;
	TInt iFirstScanLine;
	TInt iLastScanLine;
	TInt iCurrentScanLine;
	SFastData iFastData;
	SSlowData iSlowData;
private:
	friend class TCompareEdgesUpperY;
	friend class TCompareActiveEdgesFirstVertex;
	friend class TCompareScanLineIntersectionsFirstPixel;
	friend class TSwapEdges;
	friend class TSwapActiveEdges;
	friend class TSwapScanLineIntersections;
	};

class CFbsDevice;
class CFbsDrawDevice;
class TOpenFontCharMetrics;
class CGraphicsAccelerator;
class CWsBitmap;
class MFastBlend;
class CFbsRasterizer;
class CFbsBitGcExtraData;

/** 
Concrete implementation of a bitmapped graphics context.

The class provides new functionality, and implementations of the pure virtual 
functions defined in CGraphicsContext and CBitmapContext. 
@publishedAll
@released
*/
class CFbsBitGc : public CBitmapContext
	{
public:


/** Defines possible rotation values. */
	enum TGraphicsOrientation
		{
	/** Normal orientation is supported. */
		EGraphicsOrientationNormal,
	/** A 90 degree rotation is supported. */
		EGraphicsOrientationRotated90,
	/** A 180 degree rotation is supported. */
		EGraphicsOrientationRotated180,
	/** A 270 degree rotation is supported. */
		EGraphicsOrientationRotated270
		};
public:
	IMPORT_C static CFbsBitGc* NewL();
	IMPORT_C virtual ~CFbsBitGc();
	IMPORT_C void Activate(CFbsDevice* aDevice);
	IMPORT_C void ActivateNoJustAutoUpdate(CFbsDevice* aDevice);
	IMPORT_C void BitBlt(const TPoint& aPoint,const CFbsBitGc& aGc);
	IMPORT_C void BitBlt(const TPoint& aPoint,const CFbsBitGc& aGc,const TRect& aSourceRect);
	IMPORT_C void BitBlt(const TPoint& aPoint,const CFbsBitmap* aBitmap);
	IMPORT_C void BitBlt(const TPoint& aPoint,const CFbsBitmap* aBitmap,const TRect& aSourceRect);
	IMPORT_C void BitBltMasked(const TPoint& aPoint,const CFbsBitmap* aBitmap,const TRect& aSourceRect,const CFbsBitmap* aMaskBitmap,TBool aInvertMask);
	IMPORT_C void CancelClipping();
	IMPORT_C void CancelClippingRect();
	IMPORT_C void CancelClippingRegion();
	IMPORT_C void Clear();
	IMPORT_C void Clear(const TRect& aRect);
	IMPORT_C void CopyRect(const TPoint& aOffset,const TRect& aRect);
	IMPORT_C void CopySettings(const CFbsBitGc& aGc);
	IMPORT_C CGraphicsDevice* Device() const;
	IMPORT_C void DiscardBrushPattern();
	IMPORT_C void DiscardFont();
	IMPORT_C void DrawArc(const TRect& aRect,const TPoint& aStart,const TPoint& aEnd);
	IMPORT_C void DrawPie(const TRect& aRect,const TPoint& aStart,const TPoint& aEnd);
	IMPORT_C void DrawBitmap(const TPoint& aTopLeft,const CFbsBitmap* aSource);
	IMPORT_C void DrawBitmap(const TRect& aDestRect,const CFbsBitmap* aSource);
	IMPORT_C void DrawBitmap(const TRect& aDestRect,const CFbsBitmap* aSource,const TRect& aSourceRect);
	IMPORT_C void DrawBitmapMasked(const TRect& aDestRect,const CFbsBitmap* aBitmap,const TRect& aSourceRect,const CFbsBitmap* aMaskBitmap,TBool aInvertMask);
	IMPORT_C void DrawBitmapMasked(const TRect& aDestRect,const CWsBitmap* aBitmap,const TRect& aSourceRect,const CWsBitmap* aMaskBitmap,TBool aInvertMask);
	IMPORT_C void DrawRoundRect(const TRect& aRect,const TSize& aEllipse);
	IMPORT_C void DrawPolyLine(const CArrayFix<TPoint>* aPointList);
	IMPORT_C void DrawPolyLineNoEndPoint(const CArrayFix<TPoint>* aPointList);
	IMPORT_C void DrawPolyLine(const TPoint* aPointList,TInt aNumPoints);
	IMPORT_C void DrawPolyLineNoEndPoint(const TPoint* aPointList,TInt aNumPoints);
	IMPORT_C TInt DrawPolygon(const CArrayFix<TPoint>* aPointList,CGraphicsContext::TFillRule aFillRule=CGraphicsContext::EAlternate);
	IMPORT_C TInt DrawPolygon(const TPoint* aPointList,TInt aNumPoints,CGraphicsContext::TFillRule aFillRule=CGraphicsContext::EAlternate);
	IMPORT_C void DrawEllipse(const TRect& aRect);
	IMPORT_C void DrawLine(const TPoint& aStart,const TPoint& aEnd);
	IMPORT_C void DrawLineTo(const TPoint& aPoint);
	IMPORT_C void DrawLineBy(const TPoint& aVector);
	IMPORT_C void DrawRect(const TRect& aRect);
	IMPORT_C void DrawText(const TDesC& aText);
	IMPORT_C void DrawText(const TDesC& aText,const TPoint& aPosition);
	IMPORT_C void DrawText(const TDesC& aText,const TRect& aBox);
	IMPORT_C void DrawText(const TDesC& aText,const TRect& aBox,TInt aBaselineOffset,TTextAlign aHrz=ELeft,TInt aMargin=0);
	IMPORT_C void DrawText(const TDesC& aText,const TRect& aBox,TInt aBaselineOffset,TInt aTextWidth,TTextAlign aHrz=ELeft,TInt aMargin=0);
	IMPORT_C void DrawTextVertical(const TDesC& aText,TBool aUp);
	IMPORT_C void DrawTextVertical(const TDesC& aText,const TPoint& aPosition,TBool aUp);
	IMPORT_C void DrawTextVertical(const TDesC& aText,const TRect& aBox,TBool aUp);
	IMPORT_C void DrawTextVertical(const TDesC& aText,const TRect& aBox,TInt aBaselineOffset,TBool aUp,TTextAlign aVert=ELeft,TInt aMargin=0);
	IMPORT_C void DrawTextVertical(const TDesC& aText,const TRect& aBox,TInt aBaselineOffset,TInt aTextWidth,TBool aUp,TTextAlign aVert=ELeft,TInt aMargin=0);
	IMPORT_C void MapColors(const TRect& aRect,const TRgb* aColors,TInt aNumPairs=2,TBool aMapForwards=ETrue);
	IMPORT_C void MoveTo(const TPoint& aPoint);
	IMPORT_C void MoveBy(const TPoint& aVector);
	IMPORT_C void OrientationsAvailable(TBool aOrientation[4]);
	IMPORT_C void Plot(const TPoint& aPoint);
	IMPORT_C void RectDrawnTo(TRect& aRect);
	IMPORT_C void Reset();
	IMPORT_C void Resized();
	IMPORT_C void SetBrushColor(const TRgb& aColor);
	IMPORT_C void SetBrushOrigin(const TPoint& aOrigin);
	IMPORT_C void SetBrushStyle(TBrushStyle aBrushStyle);
	IMPORT_C void SetClippingRegion(const TRegion* aRegion);
	IMPORT_C void SetClippingRect(const TRect& aRect);
	IMPORT_C void SetDitherOrigin(const TPoint& aPoint);
	IMPORT_C void SetDrawMode(TDrawMode);
	IMPORT_C void SetOrigin(const TPoint& aPoint=TPoint(0,0));
	IMPORT_C void SetPenColor(const TRgb& aColor);
	IMPORT_C void SetPenStyle(TPenStyle);
	IMPORT_C void SetPenSize(const TSize& aSize);
	IMPORT_C void SetCharJustification(TInt aExcessWidth,TInt aNumGaps);
	IMPORT_C void SetWordJustification(TInt aExcessWidth,TInt aNumChars);
	IMPORT_C void SetUnderlineStyle(TFontUnderline aUnderlineStyle);
	IMPORT_C void SetUserDisplayMode(TDisplayMode aDisplayMode);
	IMPORT_C void SetStrikethroughStyle(TFontStrikethrough aStrikethroughStyle);
	IMPORT_C void SetShadowMode(TBool aShadowMode = EFalse);
	inline void SetFadeMode(TBool aFadeMode = EFalse);			//Deprecated
	inline void SetFadingParameters(TUint8 aBlackMap = 0);		//Deprecated
	IMPORT_C void SetFaded(TBool aFaded);		//Virtual from CBitmapContext
	IMPORT_C void SetFadingParameters(TUint8 aBlackMap,TUint8 aWhiteMap);	//Virtual from CBitmapContext
	IMPORT_C TBool SetOrientation(TGraphicsOrientation aOrientation);
	IMPORT_C void ShadowArea(const TRegion* aRegion);
	IMPORT_C void FadeArea(const TRegion* aRegion);
	IMPORT_C void UpdateJustification(const TDesC& aText);
	IMPORT_C void UpdateJustificationVertical(const TDesC& aText,TBool aUp);
	IMPORT_C void UseBrushPattern(const CFbsBitmap* aBitmap);
	IMPORT_C TInt UseBrushPattern(TInt aFbsBitmapHandle);
	IMPORT_C void UseFont(const CFont* aFont);
	IMPORT_C TInt UseFont(TInt aFontHandle);
	IMPORT_C void UseFontNoDuplicate(const CFbsBitGcFont* aFont);
	IMPORT_C TBool IsBrushPatternUsed() const;
	IMPORT_C TBool IsFontUsed() const;
	inline static TInt16 Load16(const TUint8* aPtr) { return TInt16(aPtr[0]+(aPtr[1]<<8)); }
	IMPORT_C TInt AlphaBlendBitmaps(const TPoint& aDestPt,
									const CFbsBitmap* aSrcBmp1,
									const CFbsBitmap* aSrcBmp2,
									const TRect& aSrcRect1,
									const TPoint& aSrcPt2,
									const CFbsBitmap* aAlphaBmp,
									const TPoint& aAlphaPt);
	IMPORT_C TInt AlphaBlendBitmaps(const TPoint& aDestPt,
									const CFbsBitmap* aSrcBmp,
									const TRect& aSrcRect,
									const CFbsBitmap* aAlphaBmp,
									const TPoint& aAlphaPt);
	IMPORT_C void ExternalizeL(RWriteStream& aWriteStream);
	IMPORT_C void InternalizeL(RReadStream& aReadStream);
	IMPORT_C TInt SetClippingRegion(const TRegion& aRegion);

	IMPORT_C TInt AlphaBlendBitmaps(const TPoint& aDestPt,
											const CWsBitmap* aSrcBmp,
											const TRect& aSrcRect,
											const CWsBitmap* aAlphaBmp,
											const TPoint& aAlphaPt);
	IMPORT_C TRgb BrushColor();
	IMPORT_C TRgb PenColor();	
	IMPORT_C void ChangeDevice(CFbsDevice* aDevice);
	// New DrawText API's that take in extra paramemters such as context. Extensible.
	IMPORT_C void DrawText(const TDesC& aText,const TTextParameters* aParam);
	IMPORT_C void DrawText(const TDesC& aText,const TTextParameters* aParam,const TPoint& aPosition);
	IMPORT_C void DrawText(const TDesC& aText,const TTextParameters* aParam,const TRect& aBox);
	IMPORT_C void DrawText(const TDesC& aText,const TTextParameters* aParam,const TRect& aBox,TInt aBaselineOffset,TTextAlign aHrz=ELeft,TInt aMargin=0);
	IMPORT_C void DrawText(const TDesC& aText,const TTextParameters* aParam,const TRect& aBox,TInt aBaselineOffset,TInt aTextWidth,TTextAlign aHrz=ELeft,TInt aMargin=0);
	IMPORT_C void DrawTextVertical(const TDesC& aText,const TTextParameters* aParam,TBool aUp);
	IMPORT_C void DrawTextVertical(const TDesC& aText,const TTextParameters* aParam,const TPoint& aPosition,TBool aUp);
	IMPORT_C void DrawTextVertical(const TDesC& aText,const TTextParameters* aParam,const TRect& aBox,TBool aUp);
	IMPORT_C void DrawTextVertical(const TDesC& aText,const TTextParameters* aParam,const TRect& aBox,TInt aBaselineOffset,TBool aUp,TTextAlign aVert=ELeft,TInt aMargin=0);
	IMPORT_C void DrawTextVertical(const TDesC& aText,const TTextParameters* aParam,const TRect& aBox,TInt aBaselineOffset,TInt aTextWidth,TBool aUp,TTextAlign aVert=ELeft,TInt aMargin=0);
	IMPORT_C void UpdateJustification(const TDesC& aText,const TTextParameters* aParam);
	IMPORT_C void UpdateJustificationVertical(const TDesC& aText,const TTextParameters* aParam,TBool aUp);
private:
	CFbsBitGc();
	void AddRect(const TRect& aRect);
	TBool AnalyseEllipse(const TRect& rc, TPoint& srad, TPoint& erad, TInt& startq, TInt& endq, TBool* quads);
	void BitMultiply(TUint32* aBinaryDataPtr,TInt aBitLength,TInt aMultiplier);
	void CheckDevice() const;
	TBool CheckDevice(const TRect& aRect) const;
	TUint32* ClipBinaryArray(TUint32* aArray,TUint32* aArrayLimit,TInt aArrayWordWd,TInt& aDataWd,TInt& aDataHt,TPoint& aPos);
	void ClipFillLine(TPoint,TPoint);
	void CopyCharWord(TUint32* aBinaryDataPtr,const TUint8* aData,TInt aBitShift);
	void CopyCharLine(TUint32* aBinaryDataPtr,TInt aBufferWords,const TUint8* aData,TInt aBitShift,TInt aCharWidth);
	void DrawText(const TDesC& aText,const TTextParameters* aParam,const TPoint& aPosition,TTextAlign aAlignment,
				  CFont::TTextDirection aDirection,const TRect* aBox = NULL);
	void DoBitBlt(const TPoint& aDest,CFbsDevice* aDevice,const TRect& aSourceRect);
	void DoBitBlt(const TPoint& aDest,CBitwiseBitmap* aBitmap,TUint32* aBase,TInt aStride,const TRect& aSourceRect);
	void DoBitBltMasked(const TPoint& aDest,CBitwiseBitmap* aSourceBitmap,TUint32* aSourceBase,const TRect& aSourceRect,CBitwiseBitmap* aMaskBitmap,TUint32* aMaskBase,TBool aInvertMask,const TPoint& aDitherOrigin,TInt aShadowMode);
	void DoBitBltMaskedFlicker(const TPoint& aDest,CBitwiseBitmap* aSourceBitmap,TUint32* aSourceBase,const TRect& aSourceRect,CBitwiseBitmap* aMaskBitmap,TUint32* aMaskBase,TBool aInvertMask,const TPoint& aDitherOrigin,TInt aShadowMode);
	void DoBitBltMaskedNonFlicker(const TPoint& aDest,CBitwiseBitmap* aSourceBitmap,TUint32* aSourceBase,const TRect& aSourceRect,CBitwiseBitmap* aMaskBitmap,TUint32* aMaskBase,TBool aInvertMask,const TPoint& aDitherOrigin,TInt aShadowMode);
	void DoBitBltMaskedNonFlickerSolid(const TPoint& aDest,CBitwiseBitmap* aSourceBitmap,TUint32* aSourceBase,const TRect& aSourceRect,CBitwiseBitmap* aMaskBitmap,TUint32* aMaskBase,TBool aInvertMask,const TPoint& aDitherOrigin,TInt aShadowMode);
	void DoBitBltMaskedNonFlickerPatterned(const TPoint& aDest,CBitwiseBitmap* aSourceBitmap,TUint32* aSourceBase,const TRect& aSourceRect,CBitwiseBitmap* aMaskBitmap,TUint32* aMaskBase,TBool aInvertMask,const TPoint& aDitherOrigin,TInt aShadowMode);
	void DoBitBltAlpha(const TPoint& aDest, CBitwiseBitmap* aSourceBitmap,
					   TUint32* aSourceBase, const TRect& aSourceRect,
					   CBitwiseBitmap* aMaskBitmap, TUint32* aMaskBase,
					   const TPoint& aAlphaPoint, TInt aShadowMode, TBool aInvertMask);
	void DoBitBltAlpha(const TPoint& aDestPt,
					   const CBitwiseBitmap* aSrcBmp1,
					   TUint32* aSrcBmpDataAddr1,
					   const CBitwiseBitmap* aSrcBmp2,
					   TUint32* aSrcBmpDataAddr2,
					   const CBitwiseBitmap* aAlphaBmp,
					   TUint32* aAlphaBmpDataAddr,
					   const TRect& aSrcRect1,
					   const TPoint& aSrcPt2,
					   const TPoint& aAlphaPt,
					   TInt aShadowMode);
	void DoCopyRect(const TPoint&,const TRect&);
	void DoDrawBitmap(const TRect&,CBitwiseBitmap*,TUint32* aBase,TInt aStride,const TRect&,const TPoint& aDitherOrigin);
	void DoDrawBitmapMasked(const TRect& aDestRect,CBitwiseBitmap* aSourceBitmap,TUint32* aSourceBase,const TRect& aSourceRect,CBitwiseBitmap* aMaskBitmap,TUint32* aMaskBase,TBool aInvertMask,const TPoint& aDitherOrigin);
	void DoDrawLine(TPoint aStart,TPoint aEnd,TBool aDrawStartPoint);
	void DoDrawDottedWideLine(const TPoint& pt1,const TPoint& pt2,TBool drawstart,const TRect& screenrect);
	void DoDrawSolidWideLine(const TPoint& pt1,const TPoint& pt2,TBool drawstart,const TRect& screenrect);
	void DoDrawText(CFont::TPositionParam& aParam,const TInt aEnd);
	void DoDrawCharacter(const TPoint& aTopLeft,const TSize& aDataSize,const TUint8* aData);
	void DoDrawTextEx(CFont::TPositionParam& aParam,const CBitmapFont* font,const TInt aEnd, const TInt aUnderlineStrikethroughOffset);
	void DoDrawCharacterEx(const TPoint& aTopLeft,const TSize& aDataSize,const TUint8* aData,
		TBool aBold,TBool aItalic,TInt aSemiAscent);
	void DoDrawCharacterAntiAliased(const TPoint& aTopLeft,const TSize& aDataSize,const TUint8* aData, const TGlyphBitmapType aGlyphType);
	void DoDrawTextLarge(CFont::TPositionParam& aParam,const CBitmapFont* font,const TInt aEnd);
	void DoDrawCharacterLarge(const TPoint& aTopLeft,const TSize& aDataSize,const TUint8* aData,
		TBool aBold,TBool aItalic,TInt aSemiAscent,TInt aWidthFactor,TInt aHeightFactor);
	void DoDrawCharacterExLarge(const TPoint& aTopLeft,const TSize& aDataSize,const TUint8* aData,
		TBool aBold,TBool aItalic,TInt aSemiAscent,TInt aWidthFactor,TInt aHeightFactor);
	void DoDrawCharacterMultiplied(const TPoint& aTopLeft,const TSize& aDataSize,const TUint8* aData,
		TBool aBold,TBool aItalic,TInt aSemiAscent,TInt aWidthMultiplier,TInt aHeightMultiplier);
	void DoDrawTextVertical(CFont::TPositionParam& aParam,const CBitmapFont* font,TBool aUp,const TInt aEnd);
	void DoDrawCharacterVertical(const TPoint& aTopLeft,const TSize& aDataSize,const TUint8* aData,
		TBool aBold,TBool aItalic,TInt aSemiAscent,TInt aWidthFactor,TInt aHeightFactor,TBool aUp);
	void DoDrawCharacterVerticalAntiAliased(const TPoint& aTopLeft,const TSize& aDataSize,const TUint8* aData,TBool aUp, TGlyphBitmapType aGlyphBitmapType);
	void DoPlot(const TPoint& pt);
	void EllipseFill(const TRect& aRect);
	void EllipseOutline(const TRect& aRect);
	void EllipseOutlineWide(const TRect& aRect);
	void GetStrikethroughMetrics(TInt& aTop,TInt& aBottom);
	void OutputCharLine(TPoint aPrintPos,TUint32* aBuffer,TInt aDataLength,TInt aNum,TBool aBold,TInt aWidthFactor,TInt aHeightFactor);
	void OutputCharLineMultiplied(TPoint aPrintPos,TUint32* aBuffer,TInt aDataLength,TInt aNum,TBool aBold,TInt aWidthMultiplier,TInt aHeightMultiplier);
	void OutputCharLineVertical(TPoint aPrintPos,TUint32* aBuffer,TInt aDataLength,TInt aNum,TBool aBold,TInt aWidthFactor,TInt aHeightFactor,TBool aUp);
	void PenAllocate();
	void PenDrawClipped(TPoint aPoint);
	void PenDrawDeferred(TPoint aPoint,TInt* aArray,TInt aFirstElement);
	void PieArcOutline(const TRect& aRect,const TPoint& aStartRadius,const TPoint& aEndRadius,TBool aDoPie);
	void PieFill(const TRect& ellrect, const TPoint& aStartIntersect, const TPoint& aEndIntersect,
		TInt aStartQuadrant, TInt aEndQuadrant, const TBool* aQuadrants);
	void PieShell(const TRect& ellrect, const TPoint& startradius, const TPoint& endradius,
		const TBool* quads, TInt startquad, TInt endquad);
	void PieTriangles(TBool aInside,const TPoint& aStart,const TPoint& aEnd);
	void PieSliver(const TRect& ellrect,const TPoint& startradius,const TPoint& endradius,TInt quad);
	void PolyFill(const CArrayFix<TPoint>* aPointList,TFillRule aFillRule);
	void PolyFillLarge(const CArrayFix<TPoint>* aPointList,TFillRule aFillRule);
	void PolyFill(const TPoint* aPointList,TInt aNumPoints,TFillRule aFillRule);
	void PolyFillLarge(const TPoint* aPointList,TInt aNumPoints,TFillRule aFillRule);
	void PolyOutline(const CArrayFix<TPoint>* aPointList);
	void PolyOutline(const TPoint* aPointList,TInt aNumPoints);
	void RectFill(const TRect& aRect);
	void RoundRectFill(const TRect& aRect,TSize aSize);
	void RoundRectOutline(const TRect& aRect,TSize aSize);
	void SetupDevice() const;
	void ShadowFadeArea(const TRegion* aRegion,TInt8 aShadowMode);
	TBool UserClipRect(TRect& aRect);
	CGraphicsAccelerator* GraphicsAccelerator();
	TUint MemoryOffsetForPixelPitch(TUint aX, TDisplayMode aDisplayMode);
	TUint32* GetScanLineOffsetPtr(CBitwiseBitmap* aSrce, TUint32*& aSlptr, TInt aLength, TPoint aPixel,TUint32* aBase, TLineScanningPosition& aLineScanningPosition, TUint aXOffset);
	void APIExGetUnderlineMetrics(TAny*& aOutput);
	TInt APIExSetShadowColor(TAny* aShadowColor);
	TInt APIExGetShadowColor(TAny*& aShadowColor);
	TInt FastBlendInterface(const CBitwiseBitmap* aSource, const CBitwiseBitmap* aMask, MFastBlend*& aFastBlend) const;
	TInt APIExIsFbsBitGc(TAny*& aIsCFbsBitGc);
	TDisplayMode ScanLineBufferDisplayMode(CFbsDrawDevice* aDrawDevice);
	TInt BaselineCorrection();
	CFbsRasterizer* PrepareRasterizerForExtendedBitmap(const CFbsBitmap& aBitmap);
	CFbsRasterizer* PrepareRasterizerForExtendedBitmap(const CFbsBitmap& aBitmap, const TRect& aDestRect, const TPoint& aOffset);
	CFbsRasterizer* PrepareRasterizerForExtendedBitmap(const CFbsBitmap& aBitmap, const TRect& aDestRect, const TRect& aSourceRect);
protected:
	/** This function should not be used by externals but must retain the
	same ordinal number to maintain BC, thus is exported.*/
	IMPORT_C TInt APIExtension(TUid aUid, TAny*& aOutput, TAny* aInput);
	
private:
	IMPORT_C void Reserved_CGraphicsContext_2();

	IMPORT_C void Reserved_CBitmapContext_1();
	IMPORT_C void Reserved_CBitmapContext_2();
	IMPORT_C void Reserved_CBitmapContext_3();
	
	IMPORT_C virtual void Reserved_CFbsBitGc_1();	
	IMPORT_C virtual void Reserved_CFbsBitGc_2();	
	IMPORT_C virtual void Reserved_CFbsBitGc_3();	
	IMPORT_C virtual void Reserved_CFbsBitGc_4();	
	IMPORT_C virtual void Reserved_CFbsBitGc_5();	
	
private:
	CFbsBitGcBitmap iBrushBitmap;
	TBool iBrushUsed;
	TRgb iBrushColor;
	TPoint iBrushOrigin;
	TBrushStyle iBrushStyle;
	TRect iClipRect;
	TRegionFix<1> iDefaultRegion;
	const TRegion* iDefaultRegionPtr;
	TRect iUserClipRect;
	CFbsDevice* iDevice;
	TPoint iDitherOrigin;
	TInt iDotLength;
	TInt iDotMask;
	TInt iDotParam;
	TInt iDotDirection;
	TDrawMode iDrawMode;
	TRect iDrawnTo;
	CFbsBitGcFont iFont;
	TInt iCharJustExcess;
	TInt iCharJustNum;
	TInt iWordJustExcess;
	TInt iWordJustNum;
	TPoint iLastPrintPosition;
	TPoint iLinePosition;
	TPoint iOrigin;
	CFbsBitGcExtraData* iFbsBitGcExtraData;
	TRgb iPenColor;
	TPenStyle iPenStyle;
	TSize iPenSize;
	TInt8 iShadowMode;
	TInt8 iAutoUpdateJustification;
	TUint8 iFadeBlackMap;
	TUint8 iFadeWhiteMap;
	TFontStrikethrough iStrikethrough;
	TFontUnderline iUnderline;
	TDisplayMode iUserDisplayMode;
	RRegion iClippingRegion;
#if defined(__ALLOW_FAST_BLEND_DISABLE__)
	TBool iFastBlendDisabled;
#endif
	};

inline void CFbsBitGc::SetFadeMode(TBool aFadeMode /*=EFalse*/)
	{SetFaded(aFadeMode);}
inline void CFbsBitGc::SetFadingParameters(TUint8 aBlackMap /*=0*/)
	{SetFadingParameters(aBlackMap,255);}		//255 is the value of white map when not fading

#endif

